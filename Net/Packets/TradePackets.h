////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <string>

namespace ms
{
	// 玩家交易包：Opcode PLAYER_INTERACTION(314)，与 s9 PlayerInteractionHandler 字段序对齐。
	// 动作码见 s9 Action 枚举（INVITE=2 / VISIT=4 / CHAT=6 / EXIT=0xA / SET_MESO=0x10 /
	// SET_ITEMS=0xF / CONFIRM=0x11 / TRANSACTION=0x14）。
	class TradeInvitePacket : public OutPacket
	{
	public:
		TradeInvitePacket(int32_t otherCid) : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(2);
			write_int(otherCid);
		}
	};

	class TradeVisitPacket : public OutPacket
	{
	public:
		TradeVisitPacket() : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(4);
		}
	};

	class TradeChatPacket : public OutPacket
	{
	public:
		TradeChatPacket(const std::string& msg) : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(6);
			write_string(msg);
		}
	};

	class TradeSetMesoPacket : public OutPacket
	{
	public:
		TradeSetMesoPacket(int32_t meso) : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x10);
			write_int(meso);
		}
	};

	class TradeAddItemPacket : public OutPacket
	{
	public:
		TradeAddItemPacket(uint8_t ivType, int16_t pos, int16_t quantity, uint8_t targetSlot)
			: OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x0F);
			write_byte(ivType);
			write_short(pos);
			write_short(quantity);
			write_byte(targetSlot);
		}
	};

	class TradeConfirmPacket : public OutPacket
	{
	public:
		TradeConfirmPacket() : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x11);
		}
	};

	class TradeTransactionPacket : public OutPacket
	{
	public:
		TradeTransactionPacket() : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x14);
		}
	};

	class TradeExitPacket : public OutPacket
	{
	public:
		TradeExitPacket() : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x0A);
		}
	};
}
