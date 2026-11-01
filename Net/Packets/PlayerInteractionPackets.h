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

	// 创建个人店/雇佣商人：action CREATE(0) + createType(4|5) + maple-string title + skip3 + int cashItemId
	// 对照 s9 PlayerInteractionHandler：title 在 CREATE，不在 OPEN_STORE。
	class HiredMerchantCreatePacket : public OutPacket
	{
	public:
		HiredMerchantCreatePacket(const std::string& title, int32_t cash_itemid, int8_t create_type = 4)
			: OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x00); // CREATE
			write_byte(create_type);
			write_string(title);
			write_byte(0);
			write_byte(0);
			write_byte(0);
			write_int(cash_itemid);
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

	// 小游戏：READY(0x3A) / START(0x3D) / EXIT(0x0A)
	class MinigameReadyPacket : public OutPacket
	{
	public:
		MinigameReadyPacket() : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x3A);
		}
	};

	class MinigameStartPacket : public OutPacket
	{
	public:
		MinigameStartPacket() : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x3D);
		}
	};

	class MinigameExitPacket : public OutPacket
	{
	public:
		MinigameExitPacket() : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x0A);
		}
	};

	// 五子棋落子：action 0x3E → byte x, byte y, byte pieceType
	class MinigameOmokPlacePacket : public OutPacket
	{
	public:
		MinigameOmokPlacePacket(uint8_t x, uint8_t y, uint8_t piece) : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x3E);
			write_byte(x);
			write_byte(y);
			write_byte(piece);
		}
	};

	// 翻牌：action 0x3F → byte turn, byte slot
	class MinigameMatchCardSelectPacket : public OutPacket
	{
	public:
		MinigameMatchCardSelectPacket(uint8_t turn, uint8_t slot) : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x3F);
			write_byte(turn);
			write_byte(slot);
		}
	};
}