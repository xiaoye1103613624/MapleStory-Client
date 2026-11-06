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
#include "MovePathEncode.h"

#include <vector>

namespace ms
{
	// Opcode: NPC_ACTION(197 / 0xC5) — s9 NPCAnimationHandler length==6 对话分支。
	// body = int oid + byte + byte；handler 原样回广播 SendOpcode.NPC_ACTION。
	// Odin 系客户端对话起手固定 (-1, 0)；移动分支 length>6 另切片。
	class NpcActionTalkPacket : public OutPacket
	{
	public:
		explicit NpcActionTalkPacket(int32_t oid) : OutPacket(OutPacket::Opcode::NPC_ACTION)
		{
			write_int(oid);
			write_byte(-1);
			write_byte(0);
		}
	};

	// Opcode: NPC_ACTION(197) 移动分支（length>6）。
	// 包形对齐 Maplestory95 CNpc::GenerateMovePath @ 0x671590：
	//   int oid + byte nAction + byte nChatIdx + CMovePath::Encode(...)
	// Encode 权威：0x666e20（见 MovePathEncode.h）。s9 丢弃尾 9（空 keyPad+rect）。
	class NpcActionMovePacket : public MovePathPacket
	{
	public:
		NpcActionMovePacket(int32_t oid, int8_t action, int8_t chat_idx,
			int16_t x, int16_t y, int16_t vx, int16_t vy,
			const std::vector<MovePathElem>& elems)
			: MovePathPacket(OutPacket::Opcode::NPC_ACTION)
		{
			write_int(oid);
			write_byte(action);
			write_byte(chat_idx);
			encode_movepath(x, y, vx, vy, elems);
		}
	};

	// Packet which requests a dialog with a server-sided NPC
	// Opcode: TALK_TO_NPC(58)。构造时顺带派发 NPC_ACTION 对话姿态，
	// 所有 TalkToNPC 调用点（点击/SoftKey/快捷）自动继承，避免漏发。
	class TalkToNPCPacket : public OutPacket
	{
	public:
		TalkToNPCPacket(int32_t oid) : OutPacket(OutPacket::Opcode::TALK_TO_NPC)
		{
			NpcActionTalkPacket(oid).dispatch();
			write_int(oid);
		}
	};

	// Opcode: CLICK_GUIDE(162 / 0xA2) — 空包。s9 ClickGuideHandler 按职业开
	// MIMO(1101008 骑士团) / LILIN(1202000 其它) NPC 脚本。
	class ClickGuidePacket : public OutPacket
	{
	public:
		ClickGuidePacket() : OutPacket(OutPacket::Opcode::CLICK_GUIDE) {}
	};

	// Packet which sends a response to an NPC dialog to the server
	// Opcode: NPC_TALK_MORE(60)
	class NpcTalkMorePacket : public OutPacket
	{
	public:
		NpcTalkMorePacket(int8_t lastmsg, int8_t response) : OutPacket(OutPacket::Opcode::NPC_TALK_MORE)
		{
			write_byte(lastmsg);
			write_byte(response);
		}

		NpcTalkMorePacket(const std::string& response) : NpcTalkMorePacket(2, 1)
		{
			write_string(response);
		}

		NpcTalkMorePacket(int32_t selection) : NpcTalkMorePacket(4, 1)
		{
			write_int(selection);
		}

		// SENDGETNUMBER OK: msgtype=3, response=1, then int amount
		// (tag avoids ambiguous/recursive delegate with NpcTalkMorePacket(int8_t,int8_t))
		struct GetNumber {};
		NpcTalkMorePacket(GetNumber, int32_t amount) : OutPacket(OutPacket::Opcode::NPC_TALK_MORE)
		{
			write_byte(3);
			write_byte(1);
			write_int(amount);
		}

		// AskAvatar / MembershopAvatar OK（IDA sub_74713D / sub_74730B）：
		// lastMsg=7|8, action=1, selection=byte(index)。Cancel：action=0（无 selection）。
		struct AskAvatar {};
		NpcTalkMorePacket(AskAvatar, int8_t lastmsg, int8_t selection) : OutPacket(OutPacket::Opcode::NPC_TALK_MORE)
		{
			write_byte(lastmsg);
			write_byte(1);
			write_byte(selection);
		}

		// AskBoxText OK（IDA sub_746C46）：lastMsg=13, action=1, string。
		// Cancel/Close 本仓发 (13,0)；IDA Close(result==3) 静默，见 a11j。
		struct AskBoxText {};
		NpcTalkMorePacket(AskBoxText, const std::string& response) : OutPacket(OutPacket::Opcode::NPC_TALK_MORE)
		{
			write_byte(13);
			write_byte(1);
			write_string(response);
		}

		// InitialQuiz OK/超时（IDA 7EB276）：lastMsg=5 + maple string，**无** action。
		// 空串 = 超时 / Escape 放弃（对齐超时路径，避免脚本挂死）。
		struct AskQuiz {};
		NpcTalkMorePacket(AskQuiz, const std::string& response) : OutPacket(OutPacket::Opcode::NPC_TALK_MORE)
		{
			write_byte(5);
			write_string(response);
		}

		// SpeedQuiz 出站（IDA 7EC934）：lastMsg=6 + maple string，**无** action。
		// 空串=超时/Next；"__GIVEUP__"=放弃（IDA 7EC46B）。
		struct AskSpeedQuiz {};
		NpcTalkMorePacket(AskSpeedQuiz, const std::string& response) : OutPacket(OutPacket::Opcode::NPC_TALK_MORE)
		{
			write_byte(6);
			write_string(response);
		}

		// AskPet / AskPetAll OK（IDA 7474A2 / 74775C）：lastMsg=9|10, action=1, int64 uniqueId。
		// Cancel：action=0。空列表早退：action=2（无 uniqueId）。空正文：action=1 + 8×0。
		struct AskPet {};
		NpcTalkMorePacket(AskPet, int8_t lastmsg, int8_t action, int64_t unique_id = 0) : OutPacket(OutPacket::Opcode::NPC_TALK_MORE)
		{
			write_byte(lastmsg);
			write_byte(action);
			if (action == 1)
				write_long(unique_id);
		}

		// DimensionalMirror OK（IDA 747A46）：lastMsg=14, action=1, int selection；Cancel：action=0。
		struct DimensionalMirror {};
		NpcTalkMorePacket(DimensionalMirror, int8_t action, int32_t selection = 0) : OutPacket(OutPacket::Opcode::NPC_TALK_MORE)
		{
			write_byte(14);
			write_byte(action);
			if (action == 1)
				write_int(selection);
		}
	};

	// Packet which tells the server of an interaction with an NPC shop
	// Opcode: NPC_SHOP_ACTION(61)
	class NpcShopActionPacket : public OutPacket
	{
	public:
		// Requests that an item should be bought from or sold to a NPC shop
		NpcShopActionPacket(int16_t slot, int32_t itemid, int16_t qty, bool buy) : NpcShopActionPacket(buy ? Mode::BUY : Mode::SELL)
		{
			write_short(slot);
			write_int(itemid);
			write_short(qty);
		}

		// Requests that an item should be recharged at a NPC shop
		NpcShopActionPacket(int16_t slot) : NpcShopActionPacket(Mode::RECHARGE)
		{
			write_short(slot);
		}

		// Requests exiting from a NPC shop
		NpcShopActionPacket() : NpcShopActionPacket(Mode::LEAVE) {}

	protected:
		enum Mode : int8_t
		{
			BUY, SELL, RECHARGE, LEAVE
		};

		NpcShopActionPacket(Mode mode) : OutPacket(OutPacket::Opcode::NPC_SHOP_ACTION)
		{
			write_byte(mode);
		}
	};
}