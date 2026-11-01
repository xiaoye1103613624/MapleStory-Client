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
				write_int(std::get<2>(keymap));		// action
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

	// Opcode: USE_DOOR(133 / 0x85) — byte party_door(0/1), byte mode
	class UseDoorPacket : public OutPacket
	{
	public:
		UseDoorPacket(bool party_door) : OutPacket(OutPacket::Opcode::USE_DOOR)
		{
			write_byte(party_door ? 1 : 0);
			write_byte(0);
		}
	};
}