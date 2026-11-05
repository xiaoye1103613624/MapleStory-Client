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
#pragma once

#include "../OutPacket.h"

#include "../KeyMapTranslate.h"

#include "../../Character/MapleStat.h"

#include "../../IO/UITypes/UIKeyConfig.h"

namespace ms
{
	// Requests a stat increase by spending AP
	// Opcode: SPEND_AP(87)
	class SpendApPacket : public OutPacket
	{
	public:
		SpendApPacket(MapleStat::Id stat) : OutPacket(OutPacket::Opcode::SPEND_AP)
		{
			write_time();
			write_int(MapleStat::codes[stat]);
		}
	};

	// Opcode: AUTO_DISTRIBUTE_AP(88 / 0x58) — s9 AssignAPProcessor.APAutoAssignAction
	// 标准分支（DB 种子 use_server_auto_assigner=false）逐字段坐实：
	//   inPacket.skip(8)  // 时间戳区（两个 int 宽）
	//   循环 2 次 { int type; int val; }
	//   type 走 Stat.getBy5ByteEncoding：STR=64 / DEX=128 / INT=256 / LUK=512
	//   val 须 0..remainingAp，否则服务端直接 return（不惩罚）
	// 客户端把"全点进主属性"算成 (主属性=全部AP, 副属性=0) 两组下发。
	// 注意：本包 type 编码与 SPEND_AP 的 MapleStat::codes **不是同一套**
	// （codes 是 MapleStatInfo 位掩码，这里是 s9 Stat 5-byte 编码），勿复用。
	class AutoAssignApPacket : public OutPacket
	{
	public:
		enum StatCode : int32_t
		{
			STR = 64,
			DEX = 128,
			INT = 256,
			LUK = 512
		};

		AutoAssignApPacket(StatCode first, int32_t first_val, StatCode second, int32_t second_val)
			: OutPacket(OutPacket::Opcode::AUTO_DISTRIBUTE_AP)
		{
			write_time();
			write_int(0);	// skip(8) 的第二字（服务端不读值）
			write_int(static_cast<int32_t>(first));
			write_int(first_val);
			write_int(static_cast<int32_t>(second));
			write_int(second_val);
		}
	};

	// Opcode: HEAL_OVER_TIME(89 / 0x59) — 自然回血/回蓝上报。
	// s9 HealOvertimeHandler：skip(8) + short healHP + short healMP；
	// HP 上限 abHeal = 77 * map.recovery * 1.5，超限 autoban。
	class HealOvertimePacket : public OutPacket
	{
	public:
		HealOvertimePacket(int16_t heal_hp, int16_t heal_mp)
			: OutPacket(OutPacket::Opcode::HEAL_OVER_TIME)
		{
			skip(8);
			write_short(heal_hp);
			write_short(heal_mp);
		}
	};

	// Requests a skill level increase by spending SP
	// Opcode: SPEND_SP(90)
	class SpendSpPacket : public OutPacket
	{
	public:
		SpendSpPacket(int32_t skill_id) : OutPacket(OutPacket::Opcode::SPEND_SP)
		{
			write_time();
			write_int(skill_id);
		}
	};

	// Requests the server to change key mappings
	// Opcode: CHANGE_KEYMAP(135)
	class ChangeKeyMapPacket : public OutPacket
	{
	public:
		ChangeKeyMapPacket(std::vector<std::tuple<KeyConfig::Key, KeyType::Id, int32_t>> updated_actions) : OutPacket(OutPacket::Opcode::CHANGE_KEYMAP)
		{
			write_int(0); // mode
			write_int(updated_actions.size()); // Number of key changes

			for (size_t i = 0; i < updated_actions.size(); i++)
			{
				auto keymap = updated_actions[i];

				write_int(std::get<0>(keymap));		// key
				write_byte(std::get<1>(keymap));	// type
				// 出站回译：本枚举 → v83 服务端编号（与 KeymapHandler 的 from_server 互逆），
				// 保证服务端键位存档仍是 v83 语义，重登回包再经 from_server 不串号。
				write_int(KeyAction::to_server(static_cast<KeyAction::Id>(std::get<2>(keymap))));		// action
			}
		}
	};

	// Opcode: GIVE_FAME(95 / 0x5F) — s9 GiveFameHandler: int cid, byte mode(1=up / 0=down)
	class GiveFamePacket : public OutPacket
	{
	public:
		GiveFamePacket(int32_t cid, bool up) : OutPacket(OutPacket::Opcode::GIVE_FAME)
		{
			write_int(cid);
			write_byte(up ? 1 : 0);
		}
	};

	// Opcode: MESO_DROP(94 / 0x5E) — s9 DropMeso: int tick, int amount
	class MesoDropPacket : public OutPacket
	{
	public:
		MesoDropPacket(int32_t amount) : OutPacket(OutPacket::Opcode::MESO_DROP)
		{
			write_time();
			write_int(amount);
		}
	};

	// Opcode: FACE_EXPRESSION(51 / 0x33) — int expression id
	class FaceExpressionPacket : public OutPacket
	{
	public:
		FaceExpressionPacket(int32_t emote) : OutPacket(OutPacket::Opcode::FACE_EXPRESSION)
		{
			write_int(emote);
		}
	};

	// Opcode: USE_CHAIR(43 / 0x2B) — int itemid
	class UseChairPacket : public OutPacket
	{
	public:
		UseChairPacket(int32_t itemid) : OutPacket(OutPacket::Opcode::USE_CHAIR)
		{
			write_int(itemid);
		}
	};

	// Opcode: CANCEL_CHAIR(42 / 0x2A)
	class CancelChairPacket : public OutPacket
	{
	public:
		CancelChairPacket() : OutPacket(OutPacket::Opcode::CANCEL_CHAIR) {}
	};

	// Opcode: CLOSE_CHALKBOARD(50 / 0x32)
	class CloseChalkboardPacket : public OutPacket
	{
	public:
		CloseChalkboardPacket() : OutPacket(OutPacket::Opcode::CLOSE_CHALKBOARD) {}
	};

	// Opcode: USE_ITEMEFFECT(52 / 0x34) — int itemid
	class UseItemEffectPacket : public OutPacket
	{
	public:
		UseItemEffectPacket(int32_t itemid) : OutPacket(OutPacket::Opcode::USE_ITEMEFFECT)
		{
			write_int(itemid);
		}
	};

	// Opcode: USE_DOOR(133 / 0x85) — int ownerid + byte
	// s9 DoorHandler.java:38-40：readInt(ownerid) 匹配当前地图 DoorObject，
	// readByte（town/target 方向标记）服务端不消费；ownerid==0 ⇒ 取消自己放的门。
	// 修前写成 byte+byte ⇒ 服务端第一个 readInt 把两字节+后续读错位（永远
	// 匹配不到门，只回 blockedMessage(6)）。
	class UseDoorPacket : public OutPacket
	{
	public:
		UseDoorPacket(int32_t ownerid) : OutPacket(OutPacket::Opcode::USE_DOOR)
		{
			write_int(ownerid);
			write_byte(0);
		}
	};
}