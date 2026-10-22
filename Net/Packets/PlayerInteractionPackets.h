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

namespace ms
{
	// Packet to request character info
	// Opcode: CHAR_INFO_REQUEST(97)
	class CharInfoRequestPacket : public OutPacket
	{
	public:
		CharInfoRequestPacket(int32_t character_id) : OutPacket(OutPacket::Opcode::CHAR_INFO_REQUEST)
		{
			write_random();
			write_int(character_id);
		}
	};

	// 雇佣商人 / 个人商店出站包：Opcode PLAYER_INTERACTION(314)，与 s9 PlayerInteractionHandler Action 对齐。

	// 开设雇佣商人：action OPEN_STORE(0x0B) -> byte 0x01（服务端 p.readByte()）。
	class HiredMerchantOpenPacket : public OutPacket
	{
	public:
		HiredMerchantOpenPacket() : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x0B);
			write_byte(0x01);
		}
	};

	// 店主上架物品：action SET_ITEMS(0x0F) -> byte ivType, short pos, short qty, byte targetSlot(1..9)。
	class HiredMerchantSetItemPacket : public OutPacket
	{
	public:
		HiredMerchantSetItemPacket(int8_t ivType, int16_t pos, int16_t quantity, int8_t targetSlot) : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x0F);
			write_byte(ivType);
			write_short(pos);
			write_short(quantity);
			write_byte(targetSlot);
		}
	};

	// 购买雇佣商人物品：action MERCHANT_BUY(0x22) -> byte slot, short quantity。
	class HiredMerchantBuyPacket : public OutPacket
	{
	public:
		HiredMerchantBuyPacket(int8_t slot, int16_t quantity) : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x22);
			write_byte(slot);
			write_short(quantity);
		}
	};
}