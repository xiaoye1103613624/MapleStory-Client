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
	// 好友交互包（c2s）：Opcode::BUDDYLIST=130，对应 s9 RecvOpcode.BUDDYLIST_MODIFY=0x82=130 → BuddylistModifyHandler。
	// 注意：s2c 收包侧 BUDDYLIST 仍为 63（s9 SendOpcode.BUDDYLIST=0x3F=63，见 PacketSwitch.cpp:158），c2s 与 s2c 在 v83 下本就非对称。
	// 字段序（mode 字节 + add=1+name+group / accept=2+cid / delete=3+cid）已与 BuddylistModifyHandler 逐项对齐。
	class BuddyAddPacket : public OutPacket
	{
	public:
		BuddyAddPacket(const std::string& name, const std::string& group) : OutPacket(OutPacket::Opcode::BUDDYLIST)
		{
			write_byte(1);
			write_string(name);
			write_string(group);
		}
	};

	class BuddyAcceptPacket : public OutPacket
	{
	public:
		BuddyAcceptPacket(int32_t otherCid) : OutPacket(OutPacket::Opcode::BUDDYLIST)
		{
			write_byte(2);
			write_int(otherCid);
		}
	};

	class BuddyDeletePacket : public OutPacket
	{
	public:
		BuddyDeletePacket(int32_t otherCid) : OutPacket(OutPacket::Opcode::BUDDYLIST)
		{
			write_byte(3);
			write_int(otherCid);
		}
	};
}
