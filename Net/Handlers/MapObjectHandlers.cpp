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

		// 商店 / 小游戏门口标（1–6 常见为有店名；过大则当无标题以免吞掉后续 chalkboard）
		uint8_t shop_flag = recv.read_byte();
		std::string shop_title;
		if (shop_flag >= 1 && shop_flag <= 6 && recv.available() >= 2)
		{
			try {
				shop_title = recv.read_string();
				if (shop_title.size() > 48)
					shop_title.clear();
			} catch (...) {
				shop_title.clear();
			}
		}
		bool chalkboard = recv.read_bool();
		std::string chalktext = chalkboard ? recv.read_string() : "";

		recv.skip(3);
		recv.read_byte(); // team

		Stage::get().get_chars().spawn(
			{ cid, look, level, job, name, stance, position }
		);
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
			recv.skip(1);

			int32_t itemid = recv.read_int();
			std::string name = recv.read_string();
			int32_t uniqueid = recv.read_int();

			recv.skip(4);

			Point<int16_t> pos = recv.read_point();
			uint8_t stance = recv.read_byte();
			int32_t fhid = recv.read_int();

			character->add_pet(petindex, itemid, name, uniqueid, pos, stance, fhid);
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
		else if (recv.available()) // skill
		{
			int32_t skillid = recv.read_int();
			recv.read_byte(); // 'direction'
			// 9 more bytes after this

			Stage::get().get_combat().show_buff(cid, skillid, effect);
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

		// 状态块长度：本表原样继承 HeavenMS/v83 的 22 字节。s9 的 spawnMonsterInternal 走
		// writeLongEncodeTemporaryMask（4 个 int = 16 字节）后才写 pos，多跳 6 字节会把
		// pos/stance/fh 整体挪位，并在读 effect 时越界抛 PacketError —— PacketSwitch 只捕获
		// PacketError，handler 尾部的 spawn() 就不会执行，表现为「怪物根本不出现」。
		recv.skip(16);

		Point<int16_t> position = recv.read_point();
		int8_t stance = recv.read_byte();

		recv.skip(2);

		uint16_t fh = recv.read_short();
		int8_t effect = recv.read_byte();

		if (effect > 0)
		{
			recv.read_byte();
			recv.read_short();

			if (effect == 15)
				recv.read_byte();
		}

		int8_t team = recv.read_byte();

		recv.skip(4);

		while (recv.available())
			recv.skip_byte();
		Stage::get().get_mobs().spawn(
			{ oid, id, 0, stance, fh, effect == -2, team, position }
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
			while (recv.available())
				recv.skip_byte();
			Stage::get().get_mobs().set_control(oid, false);
		}
		else
		{
			if (recv.available())
			{
				recv.skip(1);

				int32_t id = recv.read_int();

				// 同上：SPAWN_MOB_C 的状态块也是 16 字节（s9 requestController 分支走
				// encodeTemporary，内部同样写 4 个 int 掩码）。
				recv.skip(16);

				Point<int16_t> position = recv.read_point();
				int8_t stance = recv.read_byte();

				recv.skip(2);

				uint16_t fh = recv.read_short();
				int8_t effect = recv.read_byte();

				if (effect > 0)
				{
					recv.read_byte();
					recv.read_short();

					if (effect == 15)
						recv.read_byte();
				}

				int8_t team = recv.read_byte();

				recv.skip(4);

				while (recv.available())
					recv.skip_byte();
				Stage::get().get_mobs().spawn(
					{ oid, id, mode, stance, fh, effect == -2, team, position }
				);
			}
			else
			{
				// TODO: Remove monster invisibility, not used (maybe in an event script?), Check this!
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

			if (recv.available() > 0)
				recv.read_byte(); // pet
			else if (auto character = Stage::get().get_character(cid))
				looter = character->get_phobj();

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
		if (auto character = Stage::get().get_character(cid))
			character->show_damage(damage);
		// 伤害数字自显，不 tip
		(void)damage;
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
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t mist_type = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t owner = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t skill = recv.available() >= 4 ? recv.read_int() : 0;
		if (recv.available())
			recv.read_byte(); // skillLevel
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (recv.available() >= 8)
		{
			int16_t x1 = recv.read_short();
			int16_t y1 = recv.read_short();
			recv.read_short(); // x2
			recv.read_short(); // y2
			pos = Point<int16_t>(x1, y1);
		}
		else if (auto owner_char = Stage::get().get_character(owner))
			pos = owner_char->get_position();
		LOG(LOG_DEBUG, "SpawnMist oid=" << oid << " type=" << mist_type
			<< " owner=" << owner << " skill=" << skill << " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		Stage::get().get_field_markers().spawn_mist(oid, owner, skill, pos, 60000);
		(void)mist_type; (void)skill;
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
		bool launched = recv.available() ? recv.read_bool() : false;
		int32_t owner = recv.available() >= 4 ? recv.read_int() : 0;
		Point<int16_t> pos = recv.available() >= 4 ? recv.read_point() : Point<int16_t>(0, 0);
		LOG(LOG_DEBUG, "SpawnDoor owner=" << owner << " launched=" << launched
			<< " (" << pos.x() << "," << pos.y() << ")");
		while (recv.available())
			recv.skip_byte();
		{
			const std::string label = launched ? "Door*" : "Door";
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(owner))
				markers.set_label(owner, label);
			else
				markers.spawn(owner, FieldMarker::Kind::DOOR, pos, label);
		}
		// 门标记自显，不 tip
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
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t skill = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t skill_level = recv.available() ? recv.read_byte() : 0;
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (recv.available() >= 4)
			pos = recv.read_point();
		else if (auto owner = Stage::get().get_character(cid))
			pos = owner->get_position();
		LOG(LOG_DEBUG, "SpawnSummon cid=" << cid << " oid=" << oid << " skill=" << skill
			<< " lv=" << (int)skill_level << " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		Stage::get().get_field_markers().spawn_summon(oid, cid, skill, pos, skill_level);
		// 召唤物自显，不 tip
		(void)skill; (void)skill_level;
	}

	void RemoveSummonHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t anim = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "RemoveSummon cid=" << cid << " oid=" << oid << " anim=" << (int)anim);
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
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int16_t x = recv.available() >= 2 ? recv.read_short() : 0;
		if (recv.available() >= 2)
			recv.read_short();
		int16_t y = recv.available() >= 2 ? recv.read_short() : 0;
		LOG(LOG_DEBUG, "SpawnDragon cid=" << cid << " (" << x << "," << y << ") rem=" << recv.available());
		{
			const int32_t oid = cid ^ 0x40000000;
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, "Dragon");
			else
				markers.spawn(oid, FieldMarker::Kind::DRAGON, Point<int16_t>(x, y), "Dragon");
		}
		while (recv.available())
			recv.skip_byte();
	}

	void MoveDragonHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "MoveDragon cid=" << cid << " rem=" << recv.available());
		if (recv.available() >= 4)
		{
			Point<int16_t> pos = recv.read_point();
			Stage::get().get_field_markers().move(cid ^ 0x40000000, pos);
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
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		if (recv.available() >= 8)
			recv.read_long(); // pad
		uint32_t mask = recv.available() >= 4 ? static_cast<uint32_t>(recv.read_int()) : 0;
		int bits = 0;
		for (uint32_t m = mask; m; m &= m - 1)
			++bits;
		int32_t skill = 0;
		if (bits > 0 && recv.available() >= 6)
		{
			recv.read_short(); // value
			skill = recv.read_int();
		}
		LOG(LOG_DEBUG, "ApplyMonsterStatus oid=" << oid << " bits=" << bits
			<< " skill=" << skill << " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		if (bits > 0)
		{
			const int32_t mid = oid ^ 0x54000000;
			auto& markers = Stage::get().get_field_markers();
			const std::string label = skill ? ("Debuff#" + std::to_string(skill)) : "Debuff";
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
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		if (recv.available() >= 8)
			recv.read_long();
		uint32_t mask = recv.available() >= 4 ? static_cast<uint32_t>(recv.read_int()) : 0;
		int bits = 0;
		for (uint32_t m = mask; m; m &= m - 1)
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