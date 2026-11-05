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
#include "MapObjectHandlers.h"

#include "Helpers/LoginParser.h"
#include "Helpers/MovementParser.h"

#include "../../Character/Buff.h"
#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"
#include "../../IO/UITypes/UIMiscShells.h"
#include "../../IO/UITypes/UITreasureBox.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIItemInventory.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Graphics/Color.h"
#include "../../Util/Misc.h"
#include <string>

namespace ms
{
	namespace {
		void map_tip(const std::string& tip, Color::Name color = Color::Name::YELLOW)
		{
			// chat 优先；无 ChatBar 才 SM；不强制 FloatNotice
			auto mt = (color == Color::Name::RED) ? UIChatBar::MessageType::RED
				: (color == Color::Name::WHITE) ? UIChatBar::MessageType::WHITE
				: UIChatBar::MessageType::YELLOW;
			if (auto chatbar = UI::get().get_element<UIChatBar>())
			{
				chatbar->show_message(tip.c_str(), mt);
				return;
			}
		}

		////////////////////////////////////////////////////////////////////////////////
		// SPAWN_MONSTER / SPAWN_MONSTER_CONTROL 共用可变段解析。
		// 依据 s9 PacketCreator.java：
		//   spawnMonsterInternal            :1514-1569
		//   encodeParentlessMobSpawnEffect  :1449-1459
		//   encodeTemporary                 :1461-1502
		//   writeLongEncodeTemporaryMask    :3078-3091
		//   spawnFakeMonster                :1578-1598（marker 是 short(-2)）
		//   makeMonsterReal                 :1606-1619（marker 是 short(-1)，无 team 字节）
		//
		// 掩码段：4 个 int。Java 的 int 移位按 mod32 折叠，value>>32 == value，
		// 所以 first 半区的 bit 同时写进 masks[0] 和 masks[1]，second 半区写进
		// masks[2] 和 masks[3] ⇒ 条数 = popcount(m0)+popcount(m2)，也等于
		// 四个 popcount 之和除二。普通 SPAWN_MOB 路径服务端整段 p.skip(16)
		// （零），控制器路径才可能非零（带 debuff 的怪被接管时，修前写死
		// skip(16) 会把 pos/stance/fh 读进状态数据中间）。
		bool consume_mob_stati(InPacket& recv)
		{
			if (recv.length() < 16)
				return false;

			const uint32_t m0 = static_cast<uint32_t>(recv.read_int());
			const uint32_t m1 = static_cast<uint32_t>(recv.read_int());
			const uint32_t m2 = static_cast<uint32_t>(recv.read_int());
			const uint32_t m3 = static_cast<uint32_t>(recv.read_int());

			auto popcount = [](uint32_t m)
			{
				int n = 0;
				for (; m; m &= m - 1) ++n;
				return n;
			};

			const int count = (popcount(m0) + popcount(m1) + popcount(m2) + popcount(m3)) / 2;

			// 每条状态：short 数值 + int 技能id（mobskill 为 short+short，同宽）+ short(-1)
			if (recv.length() < static_cast<size_t>(count) * 8)
				return false;

			recv.skip(static_cast<size_t>(count) * 8);

			// 反射计数器（:1493-1501）：w 0x20000000 / m 0x40000000（都是 first 半区），
			// 存在者各 +int，任一存在再 +int(100)。
			const bool reflect_w = (m0 | m1) & 0x20000000u;
			const bool reflect_m = (m0 | m1) & 0x40000000u;

			const size_t need = (reflect_w ? 4u : 0u) + (reflect_m ? 4u : 0u)
				+ ((reflect_w || reflect_m) ? 4u : 0u);

			if (recv.length() < need)
				return false;

			recv.skip(need);
			return true;
		}

		// 尾段解析结果。
		struct MobSpawnTail
		{
			int8_t effect = 0;		// >0 出生特效；-3 父怪死后才出；-1/-2 marker
			bool fadein = false;	// marker == -2（服务端 newSpawn ⇒ 淡入）
			bool parent = false;	// 带 parentOid 的形态
			int32_t parent_oid = 0;
			int8_t team = 0;
			int32_t item_effect = 0;
			bool valid = false;
		};

		// fh 之后的尾段共四种形态，字节宽度（不含前面 oid..fh）：
		//   6  = parentless 无特效      [byte marker, byte team, int itemEffect]
		//        或 makeMonsterReal     [short(-1)=FF FF, int]        —— 无 team 字节
		//   7  = fake 无特效            [short(-2)=FE FF, byte team, int]
		//  10  = parent 在场            [byte(effect|-3), int oid, byte team, int]
		//        或 parentless 特效≠15  [byte e, byte 0, short 0, byte marker, byte team, int]
		//  11  = parentless 特效 15     特效 15 多一个 byte0
		//        或 fake 特效           [byte e, byte 0, short 0, short(-2), byte team, int]
		// 判别一律用「相对当前游标的字节值」，不猜：
		//   inspect_int  给出 index 0..3，inspect_long 给出 0..7，足够看 idx1..5。
		// parent 的 int oid 为正数（服务端自增 objectId，< 2^31），所以
		// 「idx1..3 全零 且 idx4∈{FE,FF}」不可能是 parent；反之亦然。
		// 分派失败 ⇒ 判无效，调用方整包弃用，绝不把错位字节喂给 team/newspawn
		// （修前的做法：marker 被当 team 读，team 恒 -1/-2、淡入标志失效）。
		struct MobPeek
		{
			InPacket& recv;

			uint32_t i0_3() const { return static_cast<uint32_t>(recv.inspect_int()); }
			uint64_t i0_7() const { return static_cast<uint64_t>(recv.inspect_long()); }

			uint8_t at(size_t i) const
			{
				return static_cast<uint8_t>((i0_7() >> (8 * i)) & 0xFFu);
			}
		};

		bool read_mob_spawn_tail(InPacket& recv, MobSpawnTail& t)
		{
			const size_t len = recv.length();

			if (len < 6)
				return false;

			const MobPeek pk{ recv };

			auto plain_marker = [&recv, &t]() -> bool
			{
				const int8_t marker = recv.read_byte();

				if (marker != -1 && marker != -2)
					return false;

				t.fadein = marker == -2;
				t.team = recv.read_byte();
				t.item_effect = recv.read_int();
				return true;
			};

			auto parentless_effect_prefix = [&pk](uint8_t& marker_at) -> bool
			{
				// [byte e][byte 0][short 0] ⇒ idx1..3 == 0，idx4 是 marker
				if ((pk.i0_3() >> 8) != 0)
					return false;

				const uint8_t m = pk.at(4);

				if (m != 0xFF && m != 0xFE)
					return false;

				marker_at = m;
				return true;
			};

			switch (len)
			{
			case 6:
			{
				// makeMonsterReal：short(-1)（idx0..1 = FF FF）+ int，没有 team。
				if (static_cast<uint16_t>(recv.inspect_short()) == 0xFFFFu)
				{
					recv.skip(2);
					t.effect = -1;
					t.item_effect = recv.read_int();
					t.valid = true;
					return true;
				}

				if (!plain_marker())
					return false;

				t.valid = true;
				return true;
			}
			case 7:
			{
				// spawnFakeMonster 无特效：short(-2) = FE FF
				if (static_cast<uint16_t>(recv.inspect_short()) != 0xFFFEu)
					return false;

				recv.skip(2);
				t.effect = -2;
				t.fadein = true;
				t.team = recv.read_byte();
				t.item_effect = recv.read_int();
				t.valid = true;
				return true;
			}
			case 10:
			{
				uint8_t marker = 0;

				if (parentless_effect_prefix(marker))
				{
					// [e][0][short0][marker][team][int]
					t.effect = recv.read_byte();
					recv.skip(3); // byte0 + short0

					const int8_t mk = recv.read_byte();

					if (mk != -1 && mk != -2)
						return false;

					t.fadein = mk == -2;
					t.team = recv.read_byte();
					t.item_effect = recv.read_int();
					t.valid = true;
					return true;
				}

				// parent 形态：byte(effect|-3) + int(parentOid) + byte(team) + int
				const int8_t e = recv.read_byte();

				if (e == 0)
					return false;

				t.effect = e;
				t.parent = true;
				t.parent_oid = recv.read_int();

				if (t.parent_oid <= 0)
					return false;

				t.team = recv.read_byte();
				t.item_effect = recv.read_int();
				t.valid = true;
				return true;
			}
			case 11:
			{
				// 特效 15（parentless，多一个 byte0）或 fake 特效（short(-2) 代替 marker byte）
				if ((pk.i0_3() >> 8) != 0)
					return false;

				const uint8_t i4 = pk.at(4);
				const uint8_t i5 = pk.at(5);

				if (i4 == 0 && (i5 == 0xFF || i5 == 0xFE))
				{
					// [15][0][short0][0][marker][team][int]
					const int8_t e = recv.read_byte();

					if (e != 15)
						return false;

					recv.skip(4); // byte0 + short0 + byte0

					const int8_t mk = recv.read_byte();

					if (mk != -1 && mk != -2)
						return false;

					t.effect = e;
					t.fadein = mk == -2;
					t.team = recv.read_byte();
					t.item_effect = recv.read_int();
					t.valid = true;
					return true;
				}

				if (i4 == 0xFE && i5 == 0xFF)
				{
					// fake + 特效：[e][0][short0][short(-2)][team][int]
					t.effect = recv.read_byte();

					if (t.effect <= 0)
						return false;

					recv.skip(3); // byte0 + short0
					recv.skip(2); // short(-2)
					t.fadein = true;
					t.team = recv.read_byte();
					t.item_effect = recv.read_int();
					t.valid = true;
					return true;
				}

				return false;
			}
			default:
				return false;
			}
		}
	}

	void SpawnCharHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.read_int();

		// We don't need to spawn the player twice
		if (Stage::get().is_player(cid))
		{
			while (recv.available())
				recv.skip_byte();
			return;
		}

		uint16_t level = recv.read_short();
		std::string name = recv.read_string();

		std::string guildname = recv.read_string();
		recv.read_short();	// guildlogobg
		recv.read_byte();	// guildlogobgcolor
		recv.read_short();	// guildlogo
		recv.read_byte();	// guildlogocolor

		recv.skip(8);

		bool morphed = recv.read_int() == 2;
		int32_t buffmask1 = recv.read_int();
		int16_t buffvalue = 0;

		if (buffmask1 != 0)
			buffvalue = morphed ? recv.read_short() : recv.read_byte();

		recv.read_int(); // buffmask 2

		recv.skip(43);

		recv.read_int(); // 'mount'

		recv.skip(61);

		int16_t job = recv.read_short();
		LookEntry look = LoginParser::parse_look(recv);

		recv.read_int(); // count of 5110000 
		recv.read_int(); // 'itemeffect'
		recv.read_int(); // 'chair'

		Point<int16_t> position = recv.read_point();
		int8_t stance = recv.read_byte();

		recv.skip(3);

		for (size_t i = 0; i < 3; i++)
		{
			int8_t available = recv.read_byte();

			if (available == 1)
			{
				recv.read_byte();	// 'byte2'
				recv.read_int();	// petid
				recv.read_string();	// name
				recv.read_int();	// unique id
				recv.read_int();
				recv.read_point();	// pos
				recv.read_byte();	// stance
				recv.read_int();	// fhid
			}
			else
			{
				break;
			}
		}

		recv.read_int(); // mountlevel
		recv.read_int(); // mountexp
		recv.read_int(); // mounttiredness

		// 商店 / 小游戏公告箱 + 黑板 + 戒指 + 新年卡 + team（s9 spawnPlayerMapObject
		// PC:2033-2056）。修前把箱型首字节当标志再猜 string（两形态首字节后都是
		// int oid + str desc，从 byte 位起读 string ⇒ 店招永远读不出）；尾部
		// skip(3)+read_byte 也不是 ring 块+team 的真实布局。
		//
		// addAnnounceBox（PC:2203-2223）三形态：
		//   无箱:  byte(0)
		//   商店:  byte(4) int oid str desc byte0 byte0 byte1 byte(avail) byte0   (9 + str)
		//   小游戏: byte(1|2) int oid str desc bool(pw) byte piece byte amt byte cap byte join (9 + str)
		// 黑板（PC:2039-2043）：byte(1) str text 或 byte(0)。
		uint8_t box_flag = recv.read_byte();
		std::string shop_title;

		if (box_flag == 4 || box_flag == 1 || box_flag == 2)
		{
			int32_t box_oid = recv.read_int();
			std::string desc = recv.read_string();
			recv.skip(5); // 两形态描述后都是 5 字节尾（shop: 0,0,1,avail,0 / game: pw,piece,amt,cap,join）

			if (box_flag == 4 && !desc.empty())
				shop_title = desc;
			(void)box_oid;
		}

		bool chalkboard = recv.read_bool();
		std::string chalktext = chalkboard ? recv.read_string() : "";

		// 其后是 crush/friendship/marriage ring、新年卡、byte0 byte0，最后 byte(team)。
		// ring 块变长且无显式计数，逐块解析需要完整 ring 语义（波 2 不做）；
		// team 是**包尾最后一字节**（PC:2056 return p 前唯一写入），直接从尾部取，
		// 不受中段长度影响。中段先跳过。
		int8_t team = 0;

		if (recv.length() >= 3)
		{
			// 尾部三字节固定：byte0（newyear 后再一个 0）byte0 team —— 实际为
			// [ring 区...][byte 0][byte 0][byte team]；只保证 team 取到包尾。
			recv.skip(recv.length() - 1);
			team = recv.read_byte();
		}
		else
		{
			while (recv.available())
				recv.skip_byte();
		}

		Stage::get().get_chars().spawn(
			{ cid, look, level, job, name, stance, position }
		);
		// team 供竞技场分队着色；CharSpawn 尚无该字段（波 2 后续切片接入），
		// 本包至少不再错位。
		(void)team;
		// 店招 / 黑板：仅场标，不叠 speak
		auto& markers = Stage::get().get_field_markers();
		if (!shop_title.empty())
		{
			const int32_t shop_oid = cid ^ 0x30000000;
			if (markers.contains(shop_oid))
				markers.set_label(shop_oid, shop_title);
			else
				markers.spawn(shop_oid, FieldMarker::Kind::DOOR,
					position + Point<int16_t>(0, -60), shop_title, 0, 0);
		}
		if (chalkboard && !chalktext.empty())
		{
			const int32_t chalk_oid = cid ^ 0x31000000;
			if (markers.contains(chalk_oid))
				markers.set_label(chalk_oid, chalktext);
			else
				markers.spawn(chalk_oid, FieldMarker::Kind::DOOR,
					position + Point<int16_t>(0, -80), chalktext, 0, 0);
		}
		if (!guildname.empty())
		{
			const int32_t guild_oid = cid ^ 0x32000000;
			if (markers.contains(guild_oid))
				markers.set_label(guild_oid, guildname);
			else
				markers.spawn(guild_oid, FieldMarker::Kind::DOOR,
					position + Point<int16_t>(0, -100), guildname, 0, 0);
		}
		while (recv.available())
			recv.skip_byte();
	}

	void RemoveCharHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.read_int();
		while (recv.available())
			recv.skip_byte();

		// 门口店标 / 黑板 / 公会名 与 SpawnChar 用同一 oid 约定
		Stage::get().get_field_markers().remove(cid ^ 0x30000000);
		Stage::get().get_field_markers().remove(cid ^ 0x31000000);
		Stage::get().get_field_markers().remove(cid ^ 0x32000000);
		Stage::get().get_chars().remove(cid);
	}

	void SpawnPetHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.read_int();
		Optional<Char> character = Stage::get().get_character(cid);

		if (!character)
		{
			while (recv.available())
				recv.skip_byte();
			return;
		}

		uint8_t petindex = recv.read_byte();
		int8_t mode = recv.read_byte();

		if (mode == 1)
		{
			// addPetInfo(showpet=true)（s9 PacketCreator:4455-4468 via showPet:4480）：
			// byte(1) byte(0) int itemId str name **long uniqueId** pos byte stance
			// **short fh** bool nameTag bool balloon。
			// 修前 uniqueId 用 int+skip(4) 凑宽度、fh 用 int 吞掉 nameTag：
			// 总长对齐不漂移，但 pid 高 32 位丢失（宠物 pid 会过 2^31）、
			// nameTag/balloon 被并进 fhid。按原类型读。
			recv.skip(1); // byte(1) available

			int32_t itemid = recv.read_int();
			std::string name = recv.read_string();
			int64_t uniqueid = recv.read_long();

			Point<int16_t> pos = recv.read_point();
			uint8_t stance = recv.read_byte();
			int32_t fhid = recv.read_short();

			// nameTag/balloon：波 2 的宠物名牌接线，这里先吃满。
			bool nametag = recv.available() ? recv.read_bool() : false;
			bool balloon = recv.available() ? recv.read_bool() : false;
			(void)nametag; (void)balloon;

			character->add_pet(petindex, itemid, name,
				static_cast<int32_t>(uniqueid), pos, stance, fhid);
		}
		else if (mode == 0)
		{
			bool hunger = recv.read_bool();

			character->remove_pet(petindex, hunger);
		}
		while (recv.available())
			recv.skip_byte();
	}

	void CharMovedHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.read_int();
		recv.skip(4);
		std::vector<Movement> movements = MovementParser::parse_movements(recv);
		while (recv.available())
			recv.skip_byte();

		Stage::get().get_chars().send_movement(cid, movements);
	}

	void UpdateCharLookHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.read_int();
		recv.read_byte();
		LookEntry look = LoginParser::parse_look(recv);
		while (recv.available())
			recv.skip_byte();

		Stage::get().get_chars().update_look(cid, look);
	}

	void ShowForeignEffectHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.read_int();
		int8_t effect = recv.read_byte();

		if (effect == 10) // recovery
		{
			if (recv.available())
				recv.read_byte();
			// 回血过密，不 tip
		}
		else if (effect == 13) // card effect
		{
			Stage::get().show_character_effect(cid, CharEffect::MONSTER_CARD);
			// 卡牌特效自显，不 tip
		}
		else if (effect == 4) // showPetLevelUp（PC:4542-4548：byte0 + byte index）
		{
			if (recv.available())
				recv.skip_byte(); // 0
			if (recv.available())
				recv.skip_byte(); // pet index（宠物升级冒泡留给宠物线）
		}
		else if (recv.available()) // skill
		{
			// 三种技能形态（s9 PacketCreator）：
			//   旧 showBuffEffect PC:3488-3496：byte(effect=等级注释) int(skill) byte(dir) byte1 long0
			//   Rien 版    PC:3499-3507：byte(effect) int(skill) byte0 **byte(skillLv)** byte(dir)
			//   showBerserk PC:3529-3537：byte(1) int(1320006) byte(A9) **byte(skillLv)** byte(bool)
			// 修前一律把 effectId 当技能等级传给 show_buff ⇒ Rien/berserk 形态下
			// 等级恒错（effectId 是特效号不是等级）。int(skill) 之后的剩余长度
			// 唯一区分：10 ⇒ 旧形态（等级就是 effectId）；3 ⇒ 等级在第二字节。
			int32_t skillid = recv.read_int();
			int8_t level = effect;

			if (recv.length() == 3)
			{
				recv.skip_byte(); // filler 0 / A9
				level = recv.read_byte();
				recv.skip_byte(); // direction / berserk bool
			}
			else if (recv.length() >= 10)
			{
				recv.skip_byte(); // direction
			}

			Stage::get().get_combat().show_buff(cid, skillid, level);
		}
		else
		{
			Stage::get().show_character_effect(cid, CharEffect::LEVELUP);
			// LevelUp 特效已显，不 tip
		}
		while (recv.available())
			recv.skip_byte();
	}

	void SpawnMobHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.read_int();
		recv.read_byte(); // 5 if controller == null
		int32_t id = recv.read_int();

		// 状态块：正常 spawn 是服务端 p.skip(16) 的 16 个零字节；
		// makeMonsterReal（扎昆本体转可选中，MapleMap.java:1483/2086 走 SPAWN_MOB）
		// 用 encodeTemporary，非空 stati 时变长 ⇒ 统一按掩码解析。
		if (!consume_mob_stati(recv))
		{
			// 掩码都不完整 ⇒ 短包/畸形包，弃用
			LOG(LOG_WARN, "SpawnMob short packet oid=" << oid);
			return;
		}

		Point<int16_t> position = recv.read_point();
		int8_t stance = recv.read_byte();

		recv.skip(2); // origin fh（服务端固定写 0）

		uint16_t fh = recv.read_short();

		MobSpawnTail tail;

		if (!read_mob_spawn_tail(recv, tail))
		{
			// 形态判别失败 ⇒ 宁可不刷怪，也不要用错位字节污染 team/fadein
			// （修前正是这里把 marker 当 team、把 team 当 newspawn）。
			LOG(LOG_WARN, "SpawnMob bad tail oid=" << oid << " id=" << id
				<< " len=" << recv.length());
			return;
		}

		Stage::get().get_mobs().spawn(
			{ oid, id, 0, stance, fh, tail.fadein, tail.team, position,
				tail.effect, tail.parent, tail.parent_oid }
		);
	}

	void KillMobHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.read_int();
		int8_t animation = recv.read_byte();
		while (recv.available())
			recv.skip_byte();

		Stage::get().get_mobs().remove(oid, animation);
	}

	void SpawnMobControllerHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.read_byte();
		int32_t oid = recv.read_int();

		if (mode == 0)
		{
			// stopControlling / removeMonsterInvisibility：byte(0) + oid（PC:1432-1437/1627-1632）。
			// 修前此处 TODO 把「解除隐身」当作 unused 丢弃；实际服务端 Ariant/PQ 的
			// makeMonsterInvisible 发的也是本包，mode0=停止控制 ⇒ 交回普通控制流。
			while (recv.available())
				recv.skip_byte();
			Stage::get().get_mobs().set_control(oid, false);
		}
		else
		{
			if (recv.length() >= 4)
			{
				recv.skip(1); // controller marker（5）

				int32_t id = recv.read_int();

				// 控制器路径走 encodeTemporary（PC:1461-1502）：16 字节掩码 +
				// 每状态 8 字节 + 反射计数器。修前写死 skip(16)：接管一只带
				// debuff 的怪 ⇒ pos/stance/fh 全读进状态数据。
				if (!consume_mob_stati(recv))
				{
					LOG(LOG_WARN, "SpawnMobC short stati oid=" << oid);
					return;
				}

				Point<int16_t> position = recv.read_point();
				int8_t stance = recv.read_byte();

				recv.skip(2); // origin fh

				uint16_t fh = recv.read_short();

				MobSpawnTail tail;

				if (!read_mob_spawn_tail(recv, tail))
				{
					LOG(LOG_WARN, "SpawnMobC bad tail oid=" << oid << " id=" << id
						<< " len=" << recv.length());
					return;
				}

				Stage::get().get_mobs().spawn(
					{ oid, id, mode, stance, fh, tail.fadein, tail.team, position,
						tail.effect, tail.parent, tail.parent_oid }
				);
			}
			else
			{
				// removeMonsterInvisibility（PC:1432-1437：byte(1) + oid，无后续）：
				// 怪此前被 makeMonsterInvisible 藏起，这里恢复可见 ⇒ 重发控制请求即可，
				// 本包没有位置信息，无事可做。
				while (recv.available())
					recv.skip_byte();
			}
		}
	}

	void MobMovedHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.read_int();

		recv.read_byte();
		recv.read_byte(); // useskill
		recv.read_byte(); // skill
		recv.read_byte(); // skill 1
		recv.read_byte(); // skill 2
		recv.read_byte(); // skill 3
		recv.read_byte(); // skill 4

		Point<int16_t> position = recv.read_point();
		std::vector<Movement> movements = MovementParser::parse_movements(recv);
		while (recv.available())
			recv.skip_byte();

		Stage::get().get_mobs().send_movement(oid, position, std::move(movements));
	}

	void ShowMobHpHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.read_int();
		int8_t hppercent = recv.read_byte();
		while (recv.available())
			recv.skip_byte();
		uint16_t playerlevel = Stage::get().get_player().get_stats().get_stat(MapleStat::Id::LEVEL);

		Stage::get().get_mobs().send_mobhp(oid, hppercent, playerlevel);

		// MobGage：已开才刷；否则仅头上血条（不强制 HUD）
		if (auto gage = UI::get().get_element<UIMobGage>())
		{
			if (gage->is_active())
				gage->set_tip("HP " + std::to_string(static_cast<int>(hppercent)) + "%");
		}
		(void)oid;
	}

	void SpawnNpcHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.read_int();
		int32_t id = recv.read_int();
		Point<int16_t> position = recv.read_point();
		bool flip = recv.read_bool();
		uint16_t fh = recv.read_short();

		recv.read_short(); // 'rx'
		recv.read_short(); // 'ry'
		while (recv.available())
			recv.skip_byte();

		Stage::get().get_npcs().spawn(
			{ oid, id, position, flip, fh }
		);
		// 进图刷 NPC 太密，不 tip
		(void)id;
	}

	void SpawnNpcControllerHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.read_byte();
		int32_t oid = recv.read_int();

		if (mode == 0)
		{
			while (recv.available())
				recv.skip_byte();
			Stage::get().get_npcs().remove(oid);
		}
		else
		{
			int32_t id = recv.read_int();
			Point<int16_t> position = recv.read_point();
			bool flip = recv.read_bool();
			uint16_t fh = recv.read_short();

			recv.read_short();	// 'rx'
			recv.read_short();	// 'ry'
			recv.read_bool();	// 'minimap'
			while (recv.available())
				recv.skip_byte();

			Stage::get().get_npcs().spawn(
				{ oid, id, position, flip, fh }
			);
			(void)id;
		}
	}

	void DropLootHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.read_byte();
		int32_t oid = recv.read_int();
		bool meso = recv.read_bool();
		int32_t itemid = recv.read_int();
		int32_t owner = recv.read_int();
		int8_t pickuptype = recv.read_byte();
		Point<int16_t> dropto = recv.read_point();

		recv.skip(4);

		Point<int16_t> dropfrom;

		if (mode != 2)
		{
			dropfrom = recv.read_point();

			recv.skip(2);

			Sound(Sound::Name::DROP).play();
		}
		else
		{
			dropfrom = dropto;
		}

		if (!meso)
			recv.skip(8);

		bool playerdrop = !recv.read_bool();
		while (recv.available())
			recv.skip_byte();

		Stage::get().get_drops().spawn(
			{ oid, itemid, meso, owner, dropfrom, dropto, pickuptype, mode, playerdrop }
		);
	}

	void RemoveLootHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.read_byte();
		int32_t oid = recv.read_int();

		Optional<PhysicsObject> looter;

		if (mode > 1)
		{
			int32_t cid = recv.read_int();

			// removeItemFromMap（PC:2586-2613）：animation>=2 时 int(cid)，
			// 宠物拾取（Character.java:2078 走 animation=5 + pet=true）再跟一个 slot byte。
			// 修前用 if(available>0) read_byte else 设 looter —— 两个分支互斥，
			// 宠物拾取时 looter 永远为空 ⇒ 掉落物直接消失而不是飞向宠物。
			if (recv.available() > 0)
				recv.read_byte(); // pet slot（animation=5 才有）

			if (auto character = Stage::get().get_character(cid))
				looter = character->get_phobj(); // 宠物贴附在主人处，飞向其位置

			Sound(Sound::Name::PICKUP).play();
			// Pickup 过密，不 tip
			(void)cid;
		}
		while (recv.available())
			recv.skip_byte();

		Stage::get().get_drops().remove(oid, mode, looter.get());
	}

	void HitReactorHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.read_int();
		int8_t state = recv.read_byte();
		Point<int16_t> point = recv.read_point();
		int8_t stance = recv.read_byte(); // TODO: When is this different than state?
		recv.skip(2); // TODO: Unused
		recv.skip(1); // "frame" delay but this is in the WZ file?
		while (recv.available())
			recv.skip_byte();

		Stage::get().get_reactors().trigger(oid, state);

		// 宝箱：ensure TreasureBox；开箱(state≥4)无背包时场标
		if (state > 0)
		{
			auto box = UI::get().get_element<UITreasureBox>();
			if (!box) box = UI::get().emplace<UITreasureBox>();
			else if (!box->is_active()) box->makeactive();
			if (box)
				box->set_opened(state >= 4);
			if (state >= 4)
			{
				if (auto inv = UI::get().get_element<UIItemInventory>())
				{
					if (inv->is_active())
						return;
				}
				auto& markers = Stage::get().get_field_markers();
				const int32_t mid = oid ^ 0x34000000;
				if (markers.contains(mid))
					markers.set_label(mid, "Chest opened");
				else
					markers.spawn(mid, FieldMarker::Kind::DOOR, point + Point<int16_t>(0, -40),
						"Chest opened", 0, 2500);
			}
		}
		(void)point;
		(void)stance;
	}

	void SpawnReactorHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.read_int();
		int32_t rid = recv.read_int();
		int8_t state = recv.read_byte();
		Point<int16_t> point = recv.read_point();
		while (recv.available())
			recv.skip_byte();

		// TODO: Unused, Check this!
		// uint16_t fhid = recv.read_short();
		// recv.read_byte()

		Stage::get().get_reactors().spawn(
			{ oid, rid, state, point }
		);
		(void)rid;
	}

	void RemoveReactorHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.read_int();
		int8_t state = recv.read_byte();
		Point<int16_t> point = recv.read_point();
		while (recv.available())
			recv.skip_byte();

		Stage::get().get_reactors().remove(oid, state, point);
		(void)oid; (void)state; (void)point;
	}

	void ChalkboardHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		bool open = recv.available() ? recv.read_bool() : false;
		std::string text = open && recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "Chalkboard cid=" << cid << " open=" << open << " len=" << text.size());
		const int32_t chalk_oid = cid ^ 0x31000000;
		auto& markers = Stage::get().get_field_markers();
		if (auto character = Stage::get().get_character(cid))
		{
			if (open && !text.empty())
			{
				// 场标承接黑板文案，不叠 speak
				if (markers.contains(chalk_oid))
					markers.set_label(chalk_oid, text);
				else
				{
					Point<int16_t> pos = character->get_position();
					markers.spawn(chalk_oid, FieldMarker::Kind::DOOR,
						pos + Point<int16_t>(0, -80), text, 0, 0);
				}
			}
			else
			{
				markers.remove(chalk_oid);
			}
		}
		else if (!open)
			markers.remove(chalk_oid);
		while (recv.available())
			recv.skip_byte();
		// speak / 场标已显，不 tip
	}

	void FacialExpressionHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t expression = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "FacialExpression cid=" << cid << " exp=" << expression);
		while (recv.available())
			recv.skip_byte();
		if (auto character = Stage::get().get_character(cid))
			character->set_expression(expression);
		(void)expression;
	}

	void ShowChairHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t itemid = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "ShowChair cid=" << cid << " item=" << itemid);
		while (recv.available())
			recv.skip_byte();
		if (auto character = Stage::get().get_character(cid))
			character->set_state(itemid != 0 ? Char::State::SIT : Char::State::STAND);
		(void)itemid;
	}

	void SkillEffectHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t skill = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t level = recv.available() ? recv.read_byte() : 0;
		int8_t flags = recv.available() ? recv.read_byte() : 0;
		int8_t speed = recv.available() ? recv.read_byte() : 0;
		int8_t direction = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "SkillEffect cid=" << cid << " skill=" << skill
			<< " lv=" << (int)level << " flags=" << (int)flags
			<< " speed=" << (int)speed << " dir=" << (int)direction);
		while (recv.available())
			recv.skip_byte();
		if (skill > 0)
			Stage::get().get_combat().show_buff(cid, skill, level > 0 ? level : 1);
		else if (auto character = Stage::get().get_character(cid))
		{
			character->show_iron_body();
			character->set_afterimage(skill);
		}
		(void)level; (void)flags; (void)speed; (void)direction;
	}

	void CancelSkillEffectHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t skill = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "CancelSkillEffect cid=" << cid << " skill=" << skill);
		while (recv.available())
			recv.skip_byte();
		if (auto character = Stage::get().get_character(cid))
		{
			character->set_afterimage(0);
			if (skill > 0)
				character->show_iron_body();
		}
		(void)skill;
	}

	void DamagePlayerHandler::handle(InPacket& recv) const
	{
		// PacketCreator.damagePlayer — variable layout; show damage from safe prefix
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t skill = recv.available() ? recv.read_byte() : 0;
		if (skill == -3 && recv.available() >= 4)
			recv.read_int();
		int32_t damage = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "DamagePlayer cid=" << cid << " skill=" << (int)skill << " dmg=" << damage);
		while (recv.available())
			recv.skip_byte();
		// damagePlayer（PC:2635）：skill==-4 是无显示形态（服务端用它携带纯数值
		// 结算，如反伤/自伤记账），修前照常弹伤害数字 ⇒ 别人看不见的伤害我们
		// 却满屏冒数字。direction/来源被尾丢弃，受击朝向留给后续切片。
		if (skill != -4 && damage > 0)
		{
			if (auto character = Stage::get().get_character(cid))
				character->show_damage(damage);
		}
	}

	void GiveForeignBuffHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		// s9 giveForeignBuff（PacketCreator.java:3040-3048）：int cid + writeLongMask
		// + 每个 statup 一个 writeShort + int(0) + short(0)。
		// writeLongMask 按 isFirst() 分成两半写（:3050-3061），两半用的是同一套 bit 位号，
		// 所以 first/second 可以在同一位上各占一个 buff（如 AranCombo 与 ACC 都是 0x1000000000）。
		// 数据块个数 = popcount(first) + popcount(second)，不能用 popcount(a|b)——
		// 撞位时会少数一块，尾部 int+short 就跟着错位。
		uint64_t firstmask = 0, secondmask = 0;
		if (recv.available() >= 16)
		{
			firstmask = static_cast<uint64_t>(recv.read_long());
			secondmask = static_cast<uint64_t>(recv.read_long());
		}
		auto popcount = [](uint64_t m)
		{
			int n = 0;
			for (; m; m &= m - 1)
				++n;
			return n;
		};
		int bits = popcount(firstmask) + popcount(secondmask);
		for (int i = 0; i < bits && recv.available() >= 2; ++i)
			recv.read_short();
		if (recv.available() >= 4)
			recv.read_int();
		if (recv.available() >= 2)
			recv.read_short();

		// Known visual buffs → show_buff (foreign packet has no skill id)
		struct Vis { Buffstat::Id id; bool first; int32_t skill; };
		static const Vis kVis[] = {
			{ Buffstat::Id::DARKSIGHT, false, 4001003 },
			{ Buffstat::Id::SHADOWPARTNER, false, 4111002 },
			{ Buffstat::Id::COMBO, false, 1111002 },
			{ Buffstat::Id::MAGIC_GUARD, false, 2001002 },
			{ Buffstat::Id::SOULARROW, false, 3101004 },
			{ Buffstat::Id::SHADOW_CLAW, false, 4121006 },
			{ Buffstat::Id::MAPLE_WARRIOR, false, 1121000 },
			{ Buffstat::Id::STANCE, false, 1121002 },
			{ Buffstat::Id::BOOSTER, false, 1101004 },
			{ Buffstat::Id::SPEED_INFUSION, true, 5121009 },
			{ Buffstat::Id::DASH, true, 5001005 },
			{ Buffstat::Id::MONSTER_RIDING, true, 1004 },
		};
		int32_t shown = 0;
		for (const Vis& v : kVis)
		{
			auto& codes = v.first ? Buffstat::first_codes : Buffstat::second_codes;
			auto it = codes.find(v.id);
			if (it == codes.end())
				continue;
			uint64_t mask = v.first ? firstmask : secondmask;
			if ((mask & it->second) == 0)
				continue;
			// 原本命中第一个就 break：队友同时带多个可见 buff（如暗影伙伴 + 连击）时
			// 只会渲染一个特效，这里逐个都放。
			Stage::get().get_combat().show_buff(cid, v.skill, 1);
			shown++;
		}
		if (shown == 0 && bits > 0)
		{
			if (auto character = Stage::get().get_character(cid))
				character->show_iron_body();
		}
		LOG(LOG_DEBUG, "GiveForeignBuff cid=" << cid << " bits=" << bits
			<< " shown=" << shown << " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
	}

	void CancelForeignBuffHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		uint64_t mask0 = 0, mask1 = 0;
		if (recv.available() >= 16)
		{
			mask0 = static_cast<uint64_t>(recv.read_long());
			mask1 = static_cast<uint64_t>(recv.read_long());
		}
		int bits = 0;
		for (uint64_t m = mask0 | mask1; m; m &= m - 1)
			++bits;
		LOG(LOG_DEBUG, "CancelForeignBuff cid=" << cid << " bits=" << bits
			<< " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		if (auto character = Stage::get().get_character(cid))
			character->show_iron_body();
		(void)bits;
	}

	void CatchMonsterHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t success = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "CatchMonster oid=" << oid << " ok=" << (int)success);
		while (recv.available())
			recv.skip_byte();
		if (success)
		{
			const int32_t mid = oid ^ 0x53000000;
			auto& markers = Stage::get().get_field_markers();
			Point<int16_t> pos = Stage::get().get_mobs().get_mob_position(oid);
			if (markers.contains(mid))
				markers.set_label(mid, "Catch");
			else
				markers.spawn(mid, FieldMarker::Kind::SUMMON, pos, "Catch", 0, 2000);
		}
		else
		{
			if (auto inv = UI::get().get_element<UIItemInventory>())
			{
				if (inv->is_active())
					return;
			}
			const int32_t mid = oid ^ 0x53000000;
			auto& markers = Stage::get().get_field_markers();
			Point<int16_t> pos = Stage::get().get_mobs().get_mob_position(oid);
			if (markers.contains(mid))
				markers.set_label(mid, "Catch fail");
			else
				markers.spawn(mid, FieldMarker::Kind::SUMMON, pos, "Catch fail", 0, 1500);
		}
	}

	void CatchMonsterWithItemHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t itemid = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t success = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "CatchMonsterWithItem oid=" << oid << " item=" << itemid << " ok=" << (int)success);
		while (recv.available())
			recv.skip_byte();
		if (success)
		{
			const int32_t mid = oid ^ 0x53000000;
			auto& markers = Stage::get().get_field_markers();
			Point<int16_t> pos = Stage::get().get_mobs().get_mob_position(oid);
			if (markers.contains(mid))
				markers.set_label(mid, "Catch");
			else
				markers.spawn(mid, FieldMarker::Kind::SUMMON, pos, "Catch", itemid, 2000);
		}
		else
		{
			if (auto inv = UI::get().get_element<UIItemInventory>())
			{
				if (inv->is_active())
					return;
			}
			const int32_t mid = oid ^ 0x53000000;
			auto& markers = Stage::get().get_field_markers();
			Point<int16_t> pos = Stage::get().get_mobs().get_mob_position(oid);
			if (markers.contains(mid))
				markers.set_label(mid, "Catch fail");
			else
				markers.spawn(mid, FieldMarker::Kind::SUMMON, pos, "Catch fail", itemid, 1500);
		}
	}

	void PetChatHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t index = recv.available() ? recv.read_byte() : 0;
		if (recv.available())
			recv.read_byte(); // 0
		int8_t act = recv.available() ? recv.read_byte() : 0;
		std::string text = recv.available() ? recv.read_string() : "";
		bool balloon = recv.available() ? recv.read_bool() : false;
		LOG(LOG_DEBUG, "PetChat cid=" << cid << " idx=" << (int)index << " act=" << (int)act
			<< " balloon=" << balloon << " len=" << text.size());
		while (recv.available())
			recv.skip_byte();
		if (!text.empty())
		{
			if (auto character = Stage::get().get_character(cid))
				character->speak(text);
		}
		(void)act;
	}

	void PetNameChangeHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t slot = recv.available() ? recv.read_byte() : 0;
		std::string name = recv.available() ? recv.read_string() : "";
		bool tag = recv.available() ? recv.read_bool() : false;
		LOG(LOG_DEBUG, "PetNameChange cid=" << cid << " slot=" << (int)slot
			<< " name=" << name << " tag=" << tag);
		while (recv.available())
			recv.skip_byte();
		if (auto character = Stage::get().get_character(cid))
			character->rename_pet(static_cast<uint8_t>(slot), name);
		// 宠物名已改，不 tip
		(void)tag;
	}

	void PetCommandHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t index = recv.available() ? recv.read_byte() : 0;
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "PetCommand cid=" << cid << " idx=" << (int)index
			<< " mode=" << (int)mode << " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		if (auto character = Stage::get().get_character(cid))
		{
			// mode 当作 stance 字节；位置贴主人脚下
			Point<int16_t> pos = character->get_position() + Point<int16_t>(20, 0);
			character->move_pet(static_cast<uint8_t>(index), pos, static_cast<uint8_t>(mode));
		}
	}

	void RemoveNpcHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "RemoveNpc oid=" << oid);
		Stage::get().get_npcs().remove(oid);
		while (recv.available())
			recv.skip_byte();
	}

	void SpawnMistHandler::handle(InPacket& recv) const
	{
		// s9 PacketCreator.spawnMist（PacketCreator.java:4074-4088）：
		// int oid, int type, int owner, int skill, byte level, short skillDelay,
		// int x1, int y1, int x2, int y2, int 0 —— 矩形框是 4 个 **int**，
		// 修前按 4 个 short 读 ⇒ 中心点/尺寸全错，skillDelay 也没消费。
		// type: 0 mob mist / 1 poison / 2 smokescreen / 4 recovery（PC:4077）。
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t mist_type = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t owner = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t skill = recv.available() >= 4 ? recv.read_int() : 0;

		int8_t level = 0;
		if (recv.available())
			level = recv.read_byte(); // skillLevel

		if (recv.available() >= 2)
			recv.read_short(); // skillDelay

		// 雾区矩形（地图坐标，int）。波 3 区域渲染接线，这里先取左上角为锚点。
		// 读不满 16 字节（短包/旧服务端）时退回主人坐标，绝不猜框。
		int32_t x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		bool havebox = recv.available() >= 16;

		if (havebox)
		{
			x1 = recv.read_int();
			y1 = recv.read_int();
			x2 = recv.read_int();
			y2 = recv.read_int();
		}

		Point<int16_t> pos(static_cast<int16_t>(x1), static_cast<int16_t>(y1));

		if (!havebox)
		{
			if (auto owner_char = Stage::get().get_character(owner))
				pos = owner_char->get_position();
			else
				pos = Stage::get().get_player().get_position();
		}

		LOG(LOG_DEBUG, "SpawnMist oid=" << oid << " type=" << mist_type
			<< " owner=" << owner << " skill=" << skill << " lv=" << (int)level
			<< " box=(" << x1 << "," << y1 << ")-(" << x2 << "," << y2 << ")");

		// G4：存活期由 REMOVE_MIST 驱动（包内无 duration）⇒ lifetime=0。
		// 区域贴图：Skill tile 平铺（IDA sub_432776 / MobSkill tile 串），非 Map/Effect。
		if (!havebox)
		{
			// 短包无框：退化为点雾（1×1），仍尝试加载 tile。
			x1 = pos.x();
			y1 = pos.y();
			x2 = x1 + 1;
			y2 = y1 + 1;
		}

		Stage::get().get_field_markers().spawn_mist(
			oid, owner, skill, mist_type, level, x1, y1, x2, y2, 0);

		while (recv.available())
			recv.skip_byte();
	}

	void RemoveMistHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "RemoveMist oid=" << oid);
		Stage::get().get_field_markers().remove(oid);
		while (recv.available())
			recv.skip_byte();
	}

	void SpawnDoorHandler::handle(InPacket& recv) const
	{
		// s9 spawnDoor（PC:1131-1144）：bool launched + int ownerid + pos。
		// ownerid 即门 id（RemoveDoor 同键；Door.java ownerId<0 的失败分支
		// 服务端根本不 spawn ⇒ 本包 owner 恒 >0，无 0 号门歧义）。
		bool launched = recv.available() ? recv.read_bool() : false;
		int32_t owner = recv.available() >= 4 ? recv.read_int() : 0;
		Point<int16_t> pos = recv.available() >= 4 ? recv.read_point() : Point<int16_t>(0, 0);
		LOG(LOG_DEBUG, "SpawnDoor owner=" << owner << " launched=" << launched
			<< " (" << pos.x() << "," << pos.y() << ")");
		while (recv.available())
			recv.skip_byte();

		// G6 真对象化：Map/Obj/door.img 门板+把手，踩门上行 USE_DOOR。
		Stage::get().get_field_markers().spawn_door(owner, pos, launched);
	}

	void RemoveDoorHandler::handle(InPacket& recv) const
	{
		if (recv.available())
			recv.read_byte();
		int32_t owner = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "RemoveDoor owner=" << owner);
		Stage::get().get_field_markers().remove(owner);
		while (recv.available())
			recv.skip_byte();
	}

	void SpawnSummonHandler::handle(InPacket& recv) const
	{
		// s9 PacketCreator.spawnSummon（PacketCreator.java:1167-1181）：
		// int cid, int oid, int skill, byte 0x0A(v83 填充), byte level,
		// pos, byte stance, short 0, byte moveType, bool !puppet, bool !animated.
		// 修前把 0x0A 当 level 读、pos 从错位字节起读 ⇒ 每只召唤兽等级恒 10、
		// 落位错 1 字节；stance/moveType/puppet/animated 全被当成尾巴丢掉。
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t skill = recv.available() >= 4 ? recv.read_int() : 0;

		if (recv.available())
			recv.skip_byte(); // 0x0A v83 filler

		int8_t skill_level = recv.available() ? recv.read_byte() : 0;

		Point<int16_t> pos = Stage::get().get_player().get_position();

		if (recv.available() >= 4)
			pos = recv.read_point();
		else if (auto owner = Stage::get().get_character(cid))
			pos = owner->get_position();

		int8_t stance = recv.available() ? recv.read_byte() : 0;

		if (recv.available() >= 2)
			recv.skip_short(); // short 0

		// moveType: 0 不动，1 跟随，2/4 仅传送跟随，3 鸟跟随（PC:1177）
		int8_t movetype = recv.available() ? recv.read_byte() : 0;
		bool attacking = recv.available() ? recv.read_bool() : true; // !puppet
		bool animated = recv.available() ? recv.read_bool() : false;

		LOG(LOG_DEBUG, "SpawnSummon cid=" << cid << " oid=" << oid << " skill=" << skill
			<< " lv=" << (int)skill_level << " stance=" << (int)stance
			<< " mt=" << (int)movetype << " atk=" << attacking << " anim=" << animated
			<< " rem=" << recv.available());

		Stage::get().get_field_markers().spawn_summon(oid, cid, skill, pos, skill_level);

		// animated（PC:1179 !animated 取反）⇒ 入场闪光 summoned 段；播完由状态机回 stand。
		if (animated)
			Stage::get().get_field_markers().summon_spawn(oid);

		// moveType/puppet（PC:1177-1178）：召唤兽跟随/傀儡语义。客户端召唤兽位置由
		// MOVE_SUMMON 包驱动（原生同样是包驱动而非本地 AI ⇒ 这里无需消费），
		// puppet 只影响服务端伤害判定；本地留痕日志即可。
		(void)stance; (void)movetype; (void)attacking;

		while (recv.available())
			recv.skip_byte();
	}

	void RemoveSummonHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t anim = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "RemoveSummon cid=" << cid << " oid=" << oid << " anim=" << (int)anim);
		// s9 removeSummon（PC:1190-1196）anim = animated ? 4 : 1：4 表示播死亡动画，
		// 1 表示直接消失。修前一律 remove ⇒ 召唤兽（含波3真对象化后）从不播 die。
		if (anim == 4)
			Stage::get().get_field_markers().summon_die(oid);
		else
			Stage::get().get_field_markers().remove(oid);
		while (recv.available())
			recv.skip_byte();
		(void)cid; (void)anim;
	}

	void DamageMonsterHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		if (recv.available())
			recv.read_byte();
		int32_t damage = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t curhp = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t maxhp = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "DamageMonster oid=" << oid << " dmg=" << damage
			<< " hp=" << curhp << "/" << maxhp);
		while (recv.available())
			recv.skip_byte();
		if (maxhp > 0)
		{
			int8_t pct = static_cast<int8_t>((curhp * 100) / maxhp);
			uint16_t lv = Stage::get().get_player().get_stats().get_stat(MapleStat::Id::LEVEL);
			Stage::get().get_mobs().send_mobhp(oid, pct, lv);
			if (auto gage = UI::get().get_element<UIMobGage>())
			{
				if (gage->is_active())
					gage->set_tip("HP " + std::to_string(static_cast<int>(pct)) + "%");
			}
		}
		// 大伤害数字自显，不 tip
		(void)damage;
	}

	void ShowItemEffectHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t itemid = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "ShowItemEffect cid=" << cid << " item=" << itemid);
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (auto character = Stage::get().get_character(cid))
			pos = character->get_position();
		{
			const int32_t oid = cid ^ 0x50000000;
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, "ItemFX");
			else
				markers.spawn(oid, FieldMarker::Kind::SUMMON, pos, "ItemFX", itemid, 2500);
		}
		while (recv.available())
			recv.skip_byte();
		(void)itemid;
	}

	void UpdateCharBoxHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t open = recv.available() ? recv.read_byte() : 0;
		std::string title;
		if (open && recv.available())
			title = recv.read_string();
		LOG(LOG_DEBUG, "UpdateCharBox cid=" << cid << " open=" << (int)open
			<< " title=" << title << " rem=" << recv.available());
		const int32_t box_oid = cid ^ 0x30000000; // 与 SpawnChar shop 同 oid 约定
		auto& markers = Stage::get().get_field_markers();
		if (auto character = Stage::get().get_character(cid))
		{
			if (open && !title.empty())
			{
				// 场标承接店招，不叠 speak
				if (markers.contains(box_oid))
					markers.set_label(box_oid, title);
				else
				{
					Point<int16_t> pos = character->get_position();
					markers.spawn(box_oid, FieldMarker::Kind::DOOR,
						pos + Point<int16_t>(0, -60), title, 0, 0);
				}
			}
			else if (!open)
			{
				markers.remove(box_oid);
			}
		}
		else if (!open)
			markers.remove(box_oid);
		while (recv.available())
			recv.skip_byte();
		// 场标已显，不 tip
	}

	void SpawnDragonHandler::handle(InPacket& recv) const
	{
		// s9 spawnDragon（PC:7405-7415）：int ownerId + short x + short 0
		// + short y + short 0 + byte stance + byte 0 + short job。
		// 修前 stance/job 落进 skip；job 用于龙形映射（部署只有 8840000 一只，
		// 全档同帧集，见 MapDragon.h 取证注）。
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int16_t x = recv.available() >= 2 ? recv.read_short() : 0;
		if (recv.available() >= 2)
			recv.read_short();
		int16_t y = recv.available() >= 2 ? recv.read_short() : 0;
		if (recv.available() >= 2)
			recv.read_short();

		int8_t stance = recv.available() ? recv.read_byte() : 0;
		if (recv.available())
			recv.skip_byte(); // 恒 0
		int16_t job = recv.available() >= 2 ? recv.read_short() : 0;

		LOG(LOG_DEBUG, "SpawnDragon cid=" << cid << " (" << x << "," << y << ")"
			<< " stance=" << (int)stance << " job=" << job << " rem=" << recv.available());

		// G5 真对象化：Mob/8840000.img 帧集（stand/move/hit/die1 探针在案）。
		Stage::get().get_field_markers().spawn_dragon(cid ^ 0x40000000, cid,
			Point<int16_t>(x, y), stance, job);

		while (recv.available())
			recv.skip_byte();
	}

	void MoveDragonHandler::handle(InPacket& recv) const
	{
		// s9 moveDragon：int cid + pos(start) + movement list（末尾 8 字节 state）。
		// 修前只读 start 挪场标；真对象化后末字节（含朝向奇偶）交给
		// MapDragon::apply_move 决定 flip 与 move 段。
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "MoveDragon cid=" << cid << " rem=" << recv.available());
		if (recv.available() >= 4)
		{
			Point<int16_t> pos = recv.read_point();
			std::vector<Movement> movements = MovementParser::parse_movements(recv);

			uint8_t last_state = 4;	// 默认站立态
			if (!movements.empty())
				last_state = movements.back().newstate;
			else if (recv.available() >= 2)
				last_state = static_cast<uint8_t>(recv.read_short());

			Stage::get().get_field_markers().dragon_move(cid ^ 0x40000000, pos, last_state);
		}
		while (recv.available())
			recv.skip_byte();
	}

	void RemoveDragonHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "RemoveDragon cid=" << cid);
		Stage::get().get_field_markers().remove(cid ^ 0x40000000);
		while (recv.available())
			recv.skip_byte();
	}

	void SummonSkillHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t skill = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t stance = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "SummonSkill cid=" << cid << " skill=" << skill << " stance=" << (int)stance);
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (auto owner = Stage::get().get_character(cid))
			pos = owner->get_position();
		const int32_t marker_oid = cid ^ 0x61000000;
		Stage::get().get_field_markers().spawn_summon(marker_oid, cid, skill, pos, 0, 3000);
		Stage::get().get_field_markers().set_summon_stance(marker_oid, stance);
		while (recv.available())
			recv.skip_byte();
		(void)skill;
	}

	void MoveSummonHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "MoveSummon cid=" << cid << " oid=" << oid << " rem=" << recv.available());
		if (recv.available() >= 4)
		{
			Point<int16_t> pos = recv.read_point();
			Stage::get().get_field_markers().move(oid, pos);
		}
		while (recv.available())
			recv.skip_byte();
		(void)cid;
	}

	void SummonAttackHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		if (recv.available())
			recv.read_byte(); // level
		int8_t dir = recv.available() ? recv.read_byte() : 0;
		int8_t count = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "SummonAttack cid=" << cid << " oid=" << oid
			<< " dir=" << (int)dir << " hits=" << (int)count);
		int32_t total = 0;
		for (int8_t i = 0; i < count && recv.available() >= 9; ++i)
		{
			recv.read_int(); // mob oid
			recv.read_byte();
			total += recv.read_int();
		}
		if (total > 0)
		{
			const int32_t mid = oid ^ 0x76000000;
			const std::string label = "Hit" + std::to_string(total);
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(mid))
				markers.set_label(mid, label);
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(mid, FieldMarker::Kind::SUMMON, pos, label, 0, 1500);
			}
		}
		while (recv.available())
			recv.skip_byte();
		(void)cid; (void)dir;
	}

	void DamageSummonHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		if (recv.available())
			recv.read_byte();
		int32_t damage = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t from = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "DamageSummon cid=" << cid << " oid=" << oid
			<< " dmg=" << damage << " from=" << from);
		Stage::get().get_field_markers().apply_damage(oid, damage);
		while (recv.available())
			recv.skip_byte();
		(void)cid; (void)from;
	}

	void ApplyMonsterStatusHandler::handle(InPacket& recv) const
	{
		// s9 applyMonsterStatus（PacketCreator.java:4009-4036）精确布局：
		//   int(oid) + long(0) + writeIntMask(= int firstmask + int secondmask，两个 int！)
		//   + 每状态[short(value) + int(skill)（mobskill 为 short type+short level，同宽）+ short(-1)] = 8B/条
		//   + [int(reflection)]* + byte(size) + int(0)
		// 修前两处错位：① mask 只消费一个 int（secondmask 4 字节被当成首条 value 读，
		// 单 buff 也错位）；② 只读第一条 6 字节、尾随 short(-1) 不消费 ⇒ 多 buff 整体错位。
		//（审计 G1，P0；技能图标化 = G9，随资源决策二期。）
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		if (recv.available() >= 8)
			recv.read_long(); // pad
		const uint32_t mask0 = recv.available() >= 4 ? static_cast<uint32_t>(recv.read_int()) : 0;
		const uint32_t mask1 = recv.available() >= 4 ? static_cast<uint32_t>(recv.read_int()) : 0;

		auto popcount = [](uint32_t m)
		{
			int n = 0;
			for (; m; m &= m - 1) ++n;
			return n;
		};
		const int bits = popcount(mask0) + popcount(mask1);

		int32_t skill = 0;
		int skipped = 0;
		for (int i = 0; i < bits && recv.available() >= 8; ++i)
		{
			recv.read_short(); // value
			const int32_t sk = recv.read_int(); // skill id（mobskill 的 type|level 拼宽，同 4B）
			recv.read_short(); // -1（s9 注释：可能是 buffTime，客户端不显示）
			if (i == 0)
				skill = sk;
			else
				++skipped;
		}

		// 尾部固定 byte(size)+int(0)，前面是 0 或多个 int(reflection)（s9 反射计数）。
		if (recv.available() >= 5)
		{
			recv.skip(recv.available() - 5);
			recv.read_byte(); // size（含 reflection/2 的语义，展示不用）
			recv.read_int(); // 0
		}
		while (recv.available())
			recv.skip_byte();

		LOG(LOG_DEBUG, "ApplyMonsterStatus oid=" << oid << " bits=" << bits
			<< " skill=" << skill);
		if (bits > 0)
		{
			const int32_t mid = oid ^ 0x54000000;
			auto& markers = Stage::get().get_field_markers();
			std::string label = skill ? ("Debuff#" + std::to_string(skill)) : "Debuff";
			if (skipped > 0)
				label += "+" + std::to_string(skipped); // 多状态聚合提示
			if (markers.contains(mid))
				markers.set_label(mid, label);
			else
			{
				Point<int16_t> pos = Stage::get().get_mobs().get_mob_position(oid);
				markers.spawn(mid, FieldMarker::Kind::SUMMON, pos, label, skill, 2500);
			}
		}
		(void)oid; (void)skill;
	}

	void CancelMonsterStatusHandler::handle(InPacket& recv) const
	{
		// s9 cancelMonsterStatus（:4038-4045）：int(oid)+long(0)+int(first)+int(second)+int(0)。
		// 修前 mask 也只读一个 int；本包无变长段、错位不改变行为，补齐保持协议整洁。
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		if (recv.available() >= 8)
			recv.read_long();
		const uint32_t mask0 = recv.available() >= 4 ? static_cast<uint32_t>(recv.read_int()) : 0;
		const uint32_t mask1 = recv.available() >= 4 ? static_cast<uint32_t>(recv.read_int()) : 0;
		int bits = 0;
		for (uint32_t m = mask0 | mask1; m; m &= m - 1)
			++bits;
		LOG(LOG_DEBUG, "CancelMonsterStatus oid=" << oid << " bits=" << bits
			<< " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		Stage::get().get_field_markers().remove(oid ^ 0x54000000);
		(void)oid; (void)bits;
	}

	void MovePetHandler::handle(InPacket& recv) const
	{
		// movePet: cid, slot, pid, movements
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		uint8_t slot = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		if (recv.available() >= 4)
			recv.read_int(); // pet unique id
		std::vector<Movement> movements = MovementParser::parse_movements(recv);
		LOG(LOG_DEBUG, "MovePet cid=" << cid << " slot=" << (int)slot
			<< " moves=" << movements.size());
		while (recv.available())
			recv.skip_byte();
		if (movements.empty())
			return;
		const Movement& last = movements.back();
		if (auto character = Stage::get().get_character(cid))
			character->move_pet(slot, Point<int16_t>(last.xpos, last.ypos), last.newstate);
	}

	void PetExceptionListHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t idx = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "PetExceptionList cid=" << cid << " idx=" << (int)idx
			<< " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		(void)cid; (void)idx;
	}
}