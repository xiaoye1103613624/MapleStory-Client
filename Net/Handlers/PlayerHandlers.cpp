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
#include "PlayerHandlers.h"

#include "Helpers/LoginParser.h"

#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"

#include "../../IO/UITypes/UIBuffList.h"
#include "../../IO/UITypes/UICashShop.h"
#include "../../IO/UITypes/UIChannel.h"
#include "../../IO/UITypes/UICharInfo.h"
#include "../../IO/UITypes/UIComboCounter.h"
#include "../../IO/UITypes/UISkillBook.h"
#include "../../IO/UITypes/UIKeyConfig.h"
#include "../../IO/UITypes/UIStatsInfo.h"
#include "../../IO/UITypes/UIMiscShells.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Net/Packets/PlayerInteractionPackets.h"
#include "../../Character/SkillMacros.h"
#include "../../Configuration.h"
#include "../../Graphics/Color.h"
#include "../../Util/Misc.h"
#include <string>

namespace ms
{
	void ChangeChannelHandler::handle(InPacket& recv) const
	{
		LoginParser::parse_login(recv);

		auto cashshop = UI::get().get_element<UICashShop>();

		if (cashshop)
			cashshop->exit_cashshop();

		// 频道窗已开（用户主动换频）：自显，不 tip；否则 chat/SM
		bool ch_open = false;
		if (auto ch = UI::get().get_element<UIChannel>())
			ch_open = ch->is_active();
		while (recv.available())
			recv.skip_byte();
		if (!ch_open)
		{
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message("Changing channel...", UIChatBar::MessageType::WHITE);
		}
	}

	void ChangeStatsHandler::handle(InPacket& recv) const
	{
		recv.read_bool(); // 'itemreaction'
		int32_t updatemask = recv.read_int();

		bool recalculate = false;

		for (auto iter : MapleStat::codes)
			if (updatemask & iter.second)
				recalculate |= handle_stat(iter.first, recv);

		while (recv.available())
			recv.skip_byte();

		if (recalculate)
		{
			Stage::get().get_player().recalc_stats(false);
			if (auto statsinfo = UI::get().get_element<UIStatsInfo>())
				statsinfo->update_all_stats();
		}

		UI::get().enable();
	}

	bool ChangeStatsHandler::handle_stat(MapleStat::Id stat, InPacket& recv) const
	{
		Player& player = Stage::get().get_player();

		bool recalculate = false;

		switch (stat)
		{
		case MapleStat::Id::SKIN:
			// 服务端按掩码值分档写宽度：Stat.SKIN=0x1 走 writeByte
			// （s9 PacketCreator.java:1042-1043），这里读 short 会多吞 1 字节，
			// 把同一包里后面的 FACE/JOB/属性值整体错位一位。
			player.change_look(stat, recv.read_byte());
			break;
		case MapleStat::Id::FACE:
		case MapleStat::Id::HAIR:
			player.change_look(stat, recv.read_int());
			break;
		case MapleStat::Id::LEVEL:
			player.change_level(recv.read_byte());
			// CharInfo 开着看自己：请求刷等级
			if (auto ci = UI::get().get_element<UICharInfo>())
			{
				if (ci->is_active())
					CharInfoRequestPacket(player.get_oid()).dispatch();
			}
			break;
		case MapleStat::Id::JOB:
			player.change_job(recv.read_short());
			if (auto ci = UI::get().get_element<UICharInfo>())
			{
				if (ci->is_active())
					CharInfoRequestPacket(player.get_oid()).dispatch();
			}
			break;
		case MapleStat::Id::EXP:
			player.get_stats().set_exp(recv.read_int());
			break;
		case MapleStat::Id::MESO:
			player.get_inventory().set_meso(recv.read_int());
			break;
		case MapleStat::Id::GACHAEXP:
			// 掩码 0x200000 在服务端落在兜底的 writeInt 分支（s9 PacketCreator.java:1059），
			// default 的 read_short 会截断成低 16 位，剩下 2 字节被包尾 drain 吞掉。
			// 客户端没有扭蛋经验的消费者，这里只为保持后续字段对齐而按 int 跳过。
			// 注：PET(0x180008) 不照此处理——服务端 petStatUpdate 自注释是「结构未还原、
			// 实际什么都不做」的占位包（PacketCreator.java:4573），没有可信布局可依。
			recv.skip(4);
			break;
		default:
			player.get_stats().set_stat(stat, recv.read_short());
			recalculate = true;
			break;
		}

		bool update_statsinfo = need_statsinfo_update(stat);

		if (update_statsinfo && !recalculate)
			if (auto statsinfo = UI::get().get_element<UIStatsInfo>())
				statsinfo->update_stat(stat);

		bool update_skillbook = need_skillbook_update(stat);

		if (update_skillbook)
		{
			int16_t value = player.get_stats().get_stat(stat);

			if (auto skillbook = UI::get().get_element<UISkillBook>())
				skillbook->update_stat(stat, value);
		}

		return recalculate;
	}

	bool ChangeStatsHandler::need_statsinfo_update(MapleStat::Id stat) const
	{
		switch (stat)
		{
		case MapleStat::Id::JOB:
		case MapleStat::Id::STR:
		case MapleStat::Id::DEX:
		case MapleStat::Id::INT:
		case MapleStat::Id::LUK:
		case MapleStat::Id::HP:
		case MapleStat::Id::MAXHP:
		case MapleStat::Id::MP:
		case MapleStat::Id::MAXMP:
		case MapleStat::Id::AP:
		case MapleStat::Id::LEVEL:
		case MapleStat::Id::EXP:
			return true;
		default:
			return false;
		}
	}

	bool ChangeStatsHandler::need_skillbook_update(MapleStat::Id stat) const
	{
		switch (stat)
		{
		case MapleStat::Id::JOB:
		case MapleStat::Id::SP:
			return true;
		default:
			return false;
		}
	}

	void BuffHandler::handle(InPacket& recv) const
	{
		uint64_t firstmask = recv.read_long();
		uint64_t secondmask = recv.read_long();

		// 旧实现在这里写的是 `switch (secondmask) case Buffstat::BATTLESHIP:`——
		// BATTLESHIP 是枚举序号 21，不是掩码位（它的 0xA00000040 又是多位合成值，
		// 服务端从不这样写，见 Buff.cpp 表注释），命中时会把整包剩下的 buff 直接 drain 掉。
		// 掩码位解析统一走下面的 consume。
		//
		// 服务端 writeLongMask 先写 isFirst 的位、再写另一半（s9 PacketCreator.java:3060-3061），
		// 随后按 statups 列表顺序每对写 short+int+int（:2839-2843），且注释明确要求列表按
		// BuffStat 声明序（:2821-2825）——也就是按位递增。
		// 旧实现遍历 std::unordered_map，顺序由哈希决定：一个包里有两块以上时，
		// (bs,value,skillid,duration) 会被张冠李戴（Hyper Body 的 HP%/MP% 互换即此），
		// 表里缺位或少读还会把后面的 buff 整体错位。现按掩码位递增逐块消费，
		// 位→枚举交给 Buffstat::by_code，认不出的位照读不用。
		auto consume = [&](uint64_t mask, bool first)
		{
			for (uint8_t bit = 0; bit < 64; bit++)
			{
				uint64_t code = static_cast<uint64_t>(1) << bit;

				if (mask & code)
					handle_buff(recv, Buffstat::by_code(code, first));
			}
		};

		consume(firstmask, true);
		consume(secondmask, false);

		while (recv.available())
			recv.skip_byte();
		Stage::get().get_player().recalc_stats(false);
	}

	void ApplyBuffHandler::handle_buff(InPacket& recv, Buffstat::Id bs) const
	{
		int16_t value = recv.read_short();
		int32_t skillid = recv.read_int();
		int32_t duration = recv.read_int();

		// by_code 认不出的位（服务端有 BuffStat、客户端枚举表还没补齐）：
		// 上面 3 次读已经把这一块的 10 字节消耗掉，后面的 buff 才不会错位；
		// 但不写进 player.buffs，也不挂 UIBuffList——没有对应 stat，图标既无法
		// 表达效果，CANCEL 时也取不回 skillid，会在列表里留一个永不清除的空图标。
		if (bs == Buffstat::Id::NONE)
			return;

		Stage::get().get_player().give_buff({ bs, value, skillid, duration });

		auto bufflist = UI::get().get_element<UIBuffList>();
		if (!bufflist)
			bufflist = UI::get().emplace<UIBuffList>();
		else if (!bufflist->is_active())
			bufflist->makeactive();
		if (bufflist)
			bufflist->add_buff(skillid, duration);
	}

	void CancelBuffHandler::handle_buff(InPacket&, Buffstat::Id bs) const
	{
		if (bs == Buffstat::Id::NONE)
			return;

		int32_t skillid = Stage::get().get_player().get_buff_skillid(bs);
		Stage::get().get_player().cancel_buff(bs);
		if (skillid != 0)
		{
			if (auto bufflist = UI::get().get_element<UIBuffList>())
				bufflist->remove_buff(skillid);
		}
	}

	void RecalculateStatsHandler::handle(InPacket&) const
	{
		Stage::get().get_player().recalc_stats(false);
		if (auto statsinfo = UI::get().get_element<UIStatsInfo>())
			statsinfo->update_all_stats();
	}

	void UpdateSkillHandler::handle(InPacket& recv) const
	{
		recv.skip(3);

		int32_t skillid = recv.read_int();
		int32_t level = recv.read_int();
		int32_t masterlevel = recv.read_int();
		int64_t expire = recv.read_long();

		Stage::get().get_player().change_skill(skillid, level, masterlevel, expire);
		while (recv.available())
			recv.skip_byte();

		if (auto skillbook = UI::get().get_element<UISkillBook>())
			skillbook->update_skills(skillid);

		// BeiDou 行为：UPDATE_SKILLS 静默更新（SkillBook 打开时等级自显）。
		// 旧实现在此自动弹 UISkillUp 调试浮层——进图时服务端下发初始技能即触发，
		// 其构造又会 emplace UISoftKey（"SkillUp id" 软键盘），曾在 UISoftKey::draw
		// 对空 BtCaseKeys 解引用导致 0xC0000005 闪退（事件日志 RVA 0x1EC372，
		// 2026-10-29 softkey_trace.log 调用栈实证：UpdateSkillHandler::handle）。
		// UISkillUp 保留为手动调试工具（UIStatusBar/UISkillBook 等显式调用处）。

		UI::get().enable();
	}

	void SkillMacrosHandler::handle(InPacket& recv) const
	{
		uint8_t size = recv.read_byte();
		auto& macros = SkillMacros::get();
		macros.clear();

		for (uint8_t i = 0; i < size && i < SkillMacros::MAX_SLOTS; i++)
		{
			SkillMacroSlot slot;
			slot.name = recv.read_string();
			slot.shout = static_cast<uint8_t>(recv.read_byte());
			slot.skill1 = recv.read_int();
			slot.skill2 = recv.read_int();
			slot.skill3 = recv.read_int();
			macros.set_slot(i, std::move(slot));
		}
		while (recv.available())
			recv.skip_byte();

		LOG(LOG_DEBUG, "SkillMacrosHandler: slots=" << macros.size());
		// 进图静默加载宏；技能书已开则重刷宏标签
		if (auto sb = UI::get().get_element<UISkillBook>())
		{
			if (sb->is_active())
				sb->refresh_macros();
		}
	}

	void ShowComboHandler::handle(InPacket& recv) const
	{
		// 服务端 PacketCreator.showCombo（SendOpcode.SHOW_COMBO=0xE1）：writeInt(count)。
		// 计数由服务端 Character.setCombo 在递增时下发，客户端不再自行累加。
		int32_t count = recv.available() >= 4 ? recv.read_int() : 0;
		while (recv.available())
			recv.skip_byte();

		auto ui = UI::get().get_element<UIComboCounter>();
		if (!ui)
			ui = UI::get().emplace<UIComboCounter>();
		else if (!ui->is_active())
			ui->makeactive();
		if (ui)
			ui->set_combo(count);
	}

	void AddCooldownHandler::handle(InPacket& recv) const
	{
		int32_t skill_id = recv.read_int();
		int16_t cooltime = recv.read_short();
		while (recv.available())
			recv.skip_byte();

		Stage::get().get_player().add_cooldown(skill_id, cooltime);
		(void)skill_id; (void)cooltime;
		// CD 图标自显示，不 tip
	}

	void KeymapHandler::handle(InPacket& recv) const
	{
		recv.skip(1);

		for (uint8_t i = 0; i < 90; i++)
		{
			uint8_t type = recv.read_byte();
			int32_t action = recv.read_int();

			UI::get().add_keymapping(i, type, action);
		}
		while (recv.available())
			recv.skip_byte();
		// 进图静默加载；键位窗已开则刷新，不 tip
		if (auto kc = UI::get().get_element<UIKeyConfig>())
		{
			if (kc->is_active())
				kc->makeactive();
		}
	}

	void AutoHpPotHandler::handle(InPacket& recv) const
	{
		int32_t itemid = recv.read_int();
		while (recv.available())
			recv.skip_byte();
		Setting<AutoHpPotItem>::get().save(itemid);
		LOG(LOG_DEBUG, "AutoHpPot item=" << itemid);
		if (auto kc = UI::get().get_element<UIKeyConfig>())
		{
			if (!kc->is_active())
				kc->makeactive();
			return;
		}
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(("Auto HP pot #" + std::to_string(itemid)).c_str(),
				UIChatBar::MessageType::WHITE);
	}

	void AutoMpPotHandler::handle(InPacket& recv) const
	{
		int32_t itemid = recv.read_int();
		while (recv.available())
			recv.skip_byte();
		Setting<AutoMpPotItem>::get().save(itemid);
		LOG(LOG_DEBUG, "AutoMpPot item=" << itemid);
		if (auto kc = UI::get().get_element<UIKeyConfig>())
		{
			if (!kc->is_active())
				kc->makeactive();
			return;
		}
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(("Auto MP pot #" + std::to_string(itemid)).c_str(),
				UIChatBar::MessageType::WHITE);
	}
}