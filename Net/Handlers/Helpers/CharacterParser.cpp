//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#include "CharacterParser.h"

#include "ItemParser.h"
#include "../../PacketError.h"

#include <fstream>
#include <vector>

namespace ms
{
	static void cp_trace(const std::string& label, size_t remain)
	{
		static std::ofstream t("E:\\MXD\\BeiDou-Client-xy\\setfield_trace.log", std::ios::app);
		if (t.is_open())
		{
			t << "[cp] " << label << " remain=" << remain << std::endl;
			t.flush();
		}
	}

	void CharacterParser::parse_inventory(InPacket& recv, Inventory& invent)
	{
		cp_trace("inventory-start", recv.length());
		// 严格对齐 s9 PacketCreator.addInventoryInfo：
		//   meso(long) + 5×slotmax(byte) + getTime(-2)(long)
		//   + 装备(非现金) 列表：每项 addItemInfo，结尾 writeShort(0)
		//   + 装备(现金)   列表：每项 addItemInfo，结尾 writeShort(0)
		//   + 装备背包     列表：每项 addItemInfo，结尾 writeInt(0)
		//   + USE/SETUP/ETC/CASH：每项 addItemInfo，各自结尾 writeByte(0)
		// 旧实现用 3 个 read_short 循环 + skip(2)，把「装备背包的 writeInt(0) 结尾」
		// 误当 writeShort(0)（少读 2 字节）且漏读整组现金装备，导致后续技能/任务整段
		// 错位 → Stack underflow → 进图卡选角。
		invent.set_meso(recv.read_long());
		invent.set_slotmax(InventoryType::EQUIP, recv.read_byte());
		invent.set_slotmax(InventoryType::USE, recv.read_byte());
		invent.set_slotmax(InventoryType::SETUP, recv.read_byte());
		invent.set_slotmax(InventoryType::ETC, recv.read_byte());
		invent.set_slotmax(InventoryType::CASH, recv.read_byte());

		recv.skip(8); // getTime(-2)

		// 装备（非现金）：s9 直接写道具 + writeShort(0) 终结符（无独立 count）。循环读到 0 即止；
		// 空段时首 read_short 读到终结符(0) 自动结束，终结符被本次读走，不会串到 cash 段。
		int16_t pos = recv.read_short();
		while (pos != 0)
		{
			ItemParser::parse_item(recv, InventoryType::EQUIPPED, pos, invent);
			pos = recv.read_short();
		}

		// 装备（现金）：结构同上
		pos = recv.read_short();
		while (pos != 0)
		{
			ItemParser::parse_item(recv, InventoryType::EQUIPPED, pos, invent);
			pos = recv.read_short();
		}

		// 装备背包：位置服务端按 writeShort 写，段尾 writeInt(0) 终结符（高 2 字节充当循环结束的 0，
		// 低 2 字节由 skip(2) 吃掉）。注意：原始实现用 read_int 读位置（每枚多吞 2 字节）也会导致错位。
		int16_t ipos = recv.read_short();
		while (ipos != 0)
		{
			ItemParser::parse_item(recv, InventoryType::EQUIP, ipos, invent);
			ipos = recv.read_short();
		}
		recv.skip(2); // writeInt(0) 终结符的低 2 字节

		const InventoryType::Id byte_lists[3] =
		{
			InventoryType::USE, InventoryType::SETUP, InventoryType::ETC
		};

		for (InventoryType::Id inv : byte_lists)
		{
			int8_t bpos = recv.read_byte();
			while (bpos != 0)
			{
				ItemParser::parse_item(recv, inv, bpos, invent);
				bpos = recv.read_byte();
			}
		}

		// CASH：s9 addInventoryInfo 不在 CASH 段末尾写 writeByte(0) 终结符（它是 inventory
		// 的最后一个子段，包尾即结束）。CASH 之后紧跟 addSkillInfo 写的 writeByte(0)（"start of
		// skills" 标记），该 0 字节由 parse_skillbook 消费。故这里用 inspect_byte() 预看而非
		// 读掉：遇到 0 即视为 CASH 结束并退出（不消耗那个 0），把 0 留给 parse_skillbook，
		// 避免双重消费导致技能段 size 错位 1 字节 → Stack underflow（进图卡选角）。
		while (recv.length() > 0 && recv.inspect_byte() != 0)
		{
			int8_t cpos = recv.read_byte();
			ItemParser::parse_item(recv, InventoryType::CASH, cpos, invent);
		}
	}

	void CharacterParser::parse_skillbook(InPacket& recv, SkillBook& skills)
	{
		cp_trace("skillbook-start", recv.length());
		// s9 PacketCreator.addSkillInfo：writeByte(0) + writeShort(size) + entries。
		// CASH 段无独立终结符，正常应由本函数消费该 0。若库存解析多吞 1 字节把
		// 标记吃掉，硬读前导字节会把 size 低位当标记 → size 错位 → Stack underflow
		//（实测 remain=354 失败 / remain=355 偶发成功）。
		// masterlevel：对齐 Angel is_skill_need_master_level@0x4e8f04 与 s9
		// Skill.isFourthJob（job%10==2 + Evan 特例），旧公式
		// (id%100000)/10000==2 在 Evan 等线会多读 4B。
		auto skill_has_masterlevel = [](int32_t skill_id) -> bool
		{
			const int32_t job = skill_id / 10000;
			if (job == 2212)
				return false;
			if (skill_id == 22170001 || skill_id == 22171003 || skill_id == 22171004
				|| skill_id == 22181002 || skill_id == 22181003)
				return true;
			if (job % 100 == 0)
				return false;
			return job % 10 == 2;
		};

		struct Entry
		{
			int32_t id;
			int32_t level;
			int32_t master;
			int64_t expiration;
		};

		auto attempt = [&](bool consume_marker) -> bool
		{
			const size_t start = recv.position();
			std::vector<Entry> batch;

			try
			{
				if (consume_marker)
				{
					if (recv.length() < 3)
						return false;
					recv.read_byte();
				}
				else if (recv.length() < 2)
				{
					return false;
				}

				const int16_t size = recv.read_short();
				if (size < 0)
					return false;
				// 每条至少 id+level+expiration = 16B；明显越界则换帧
				if (static_cast<size_t>(size) * 16u > recv.length())
					return false;

				batch.reserve(static_cast<size_t>(size));
				for (int16_t i = 0; i < size; i++)
				{
					Entry e;
					e.id = recv.read_int();
					e.level = recv.read_int();
					e.expiration = recv.read_long();
					e.master = skill_has_masterlevel(e.id) ? recv.read_int() : 0;
					batch.push_back(e);
				}

				for (const Entry& e : batch)
					skills.set_skill(e.id, e.level, e.master, e.expiration);

				cp_trace(consume_marker ? "skillbook-framed-marker" : "skillbook-framed-raw",
					recv.length());
				return true;
			}
			catch (const PacketError&)
			{
				recv.seek(start);
				return false;
			}
		};

		if (attempt(true))
			return;
		if (attempt(false))
			return;

		throw PacketError("parse_skillbook: cannot frame skill segment");
	}

	void CharacterParser::parse_cooldowns(InPacket& recv, Player& player)
	{
		cp_trace("cooldowns-start", recv.length());
		int16_t size = recv.read_short();

		for (int16_t i = 0; i < size; i++)
		{
			int32_t skill_id = recv.read_int();
			int32_t cooltime = recv.read_short();
			player.add_cooldown(skill_id, cooltime);
		}
	}

	void CharacterParser::parse_questlog(InPacket& recv, QuestLog& quests)
	{
		cp_trace("questlog-start", recv.length());
		// First list: started quests (+ optional infoNumber twin entries as their own qid/qdata).
		int16_t size = recv.read_short();

		for (int16_t i = 0; i < size; i++)
		{
			int16_t qid = recv.read_short();
			std::string qdata = recv.read_string();
			quests.add_started(qid, qdata);
		}

		// Second list: completed quests with completion time.
		size = recv.read_short();

		for (int16_t i = 0; i < size; i++)
		{
			int16_t qid = recv.read_short();
			int64_t time = recv.read_long();
			quests.add_completed(qid, time);
		}
	}

	void CharacterParser::parse_ring1(InPacket& recv)
	{
		cp_trace("ring1-start", recv.length());
		int16_t rsize = recv.read_short();

		for (int16_t i = 0; i < rsize; i++)
		{
			recv.read_int();
			recv.read_padded_string(13);
			recv.read_int();
			recv.read_int();
			recv.read_int();
			recv.read_int();
		}
	}

	void CharacterParser::parse_ring2(InPacket& recv)
	{
		cp_trace("ring2-start", recv.length());
		int16_t rsize = recv.read_short();
		cp_trace("ring2-rsize", static_cast<size_t>(rsize));

		for (int16_t i = 0; i < rsize; i++)
		{
			recv.read_int();
			recv.read_padded_string(13);
			recv.read_int();
			recv.read_int();
			recv.read_int();
			recv.read_int();
			recv.read_int();
		}
	}

	bool CharacterParser::parse_marriage_block(InPacket& recv, MarriageInfo& marriage)
	{
		// 块体（不含前置 present 标志）：与 s9 addRingInfo / OnMarriageResult(11) 逐字段同构。
		// 全量长度 = 3×int(12) + short(2) + 2×int(8) + 2×fixed13(26) = 48 字节。
		// 不足 48 视为残包（理论上 s9 完整块恒 48）：拒读，调用方保持旧数据。
		if (recv.length() < 48)
			return false;

		MarriageInfo info;
		info.present = true;
		info.relationship_id = recv.read_int();
		info.groom_id = recv.read_int();
		info.bride_id = recv.read_int();
		info.status = recv.read_short();
		info.ring_item_id = recv.read_int();
		recv.read_int();	// 第二个 ringItemId（s9 对配偶侧写同一 id）
		info.groom_name = recv.read_padded_string(13);
		info.bride_name = recv.read_padded_string(13);

		marriage = std::move(info);
		return true;
	}

	void CharacterParser::parse_ring3(InPacket& recv, MarriageInfo& marriage)
	{
		cp_trace("ring3-start", recv.length());
		// s9 PacketCreator.addRingInfo marriage 块：present 为 short（0/1）。
		int16_t rsize = recv.read_short();

		if (rsize <= 0)
		{
			marriage = MarriageInfo{};
			return;
		}

		parse_marriage_block(recv, marriage);
	}

	void CharacterParser::parse_minigame(InPacket& recv)
	{
		recv.skip(2);
	}

	void CharacterParser::parse_monsterbook(InPacket& recv, MonsterBook& monsterbook)
	{
		cp_trace("monsterbook-start", recv.length());
		monsterbook.set_cover(recv.read_int());

		recv.skip(1);

		int16_t size = recv.read_short();

		for (int16_t i = 0; i < size; i++)
		{
			int16_t cid = recv.read_short();
			int8_t mblv = recv.read_byte();

			monsterbook.add_card(cid, mblv);
		}
	}

	void CharacterParser::parse_teleportrock(InPacket& recv, TeleportRock& teleportrock)
	{
		cp_trace("teleportrock-start", recv.length());
		for (size_t i = 0; i < 5; i++)
			teleportrock.addlocation(recv.read_int());

		for (size_t i = 0; i < 10; i++)
			teleportrock.addviplocation(recv.read_int());
	}

	void CharacterParser::parse_nyinfo(InPacket& recv)
	{
		cp_trace("nyinfo-start", recv.length());
		int16_t nysize = recv.read_short();

		for (int16_t i = 0; i < nysize; i++)
		{
			recv.read_int();	// NewYear Id
			recv.read_int();	// NewYear SenderId
			recv.read_string();	// NewYear SenderName
			recv.read_bool();	// NewYear enderCardDiscarded
			recv.read_long();	// NewYear DateSent
			recv.read_int();	// NewYear ReceiverId
			recv.read_string();	// NewYear ReceiverName
			recv.read_bool();	// NewYear eceiverCardDiscarded
			recv.read_bool();	// NewYear eceiverCardReceived
			recv.read_long();	// NewYear DateReceived
			recv.read_string();	// NewYear Message
		}
	}

	void CharacterParser::parse_areainfo(InPacket& recv)
	{
		cp_trace("areainfo-start", recv.length());
		std::map<int16_t, std::string> areainfo;
		int16_t arsize = recv.read_short();

		for (int16_t i = 0; i < arsize; i++)
		{
			int16_t area = recv.read_short();
			areainfo[area] = recv.read_string();
		}
	}
}