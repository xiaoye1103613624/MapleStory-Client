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
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <string>

namespace ms
{
	// 玩家交易包：Opcode PLAYER_INTERACTION(314)，字段序以 s9 PlayerInteractionHandler
	// / PacketCreator 为准。Action 值：CREATE=0 / INVITE=2 / DECLINE=3 / VISIT=4 /
	// CHAT=6 / EXIT=0xA / SET_ITEMS=0xF / SET_MESO=0x10 / CONFIRM=0x11。
	//
	// 注意：s9 的 Action 枚举里虽有 TRANSACTION(0x14)，但 handler 没有任何 0x14 分支；
	// 成交由双方各发一次 CONFIRM(0x11) 完成握手（Trade.checkCompleteHandshake）。
	// 因此不要发 0x14 —— 那是一条死链路。

	// 发起交易前必须 CREATE(createType=3)：s9 Trade.startTrade 才会建立 chr.trade，
	// 否则随后的 INVITE 会在 inviteTrade 里访问 c1.getTrade().isFullTrade() 而失败。
	class TradeCreatePacket : public OutPacket
	{
	public:
		TradeCreatePacket() : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0);
			write_byte(3);
		}
	};

	class TradeInvitePacket : public OutPacket
	{
	public:
		TradeInvitePacket(int32_t otherCid) : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(2);
			write_int(otherCid);
		}
	};

	// 拒绝交易邀请（Trade.declineTrade），比 EXIT 更贴合语义：不会误关个人商店/小游戏。
	class TradeDeclinePacket : public OutPacket
	{
	public:
		TradeDeclinePacket() : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(3);
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
		// s9 PlayerInteractionHandler.SET_ITEMS 要求 targetSlot ∈ [1,9]，越界会直接
		// warn + enableActions + return（静默丢包）。调用方应传 UITrade::next_slot()。
		TradeAddItemPacket(uint8_t ivType, int16_t pos, int16_t quantity, uint8_t targetSlot)
			: OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			if (targetSlot < 1)
				targetSlot = 1;
			else if (targetSlot > 9)
				targetSlot = 9;
			write_byte(0x0F);
			write_byte(ivType);
			write_short(pos);
			write_short(quantity);
			write_byte(targetSlot);
		}
	};

	// 确认/成交：s9 Trade.completeTrade 的握手。双方各按一次才真正交换。
	class TradeConfirmPacket : public OutPacket
	{
	public:
		TradeConfirmPacket() : OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)
		{
			write_byte(0x11);
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

	// 发起交易邀请的统一步骤：先 CREATE(0)+3 建立 chr.trade，再 INVITE(2)。
	// 少发 CREATE 会让 s9 Trade.inviteTrade 在 c1.getTrade().isFullTrade() 处失败。
	inline void dispatch_trade_invite(int32_t otherCid)
	{
		TradeCreatePacket().dispatch();
		TradeInvitePacket(otherCid).dispatch();
	}
}
