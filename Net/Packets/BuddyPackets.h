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
	// 好友交互包：Opcode BUDDYLIST(63)，与 s9 BuddylistModifyHandler 字段序对齐。
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
