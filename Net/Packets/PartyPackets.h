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
	// 组队交互包：Opcode PARTY_OPERATION(124)，与 s9 PartyOperationHandler 字段序对齐。
	class PartyCreatePacket : public OutPacket
	{
	public:
		PartyCreatePacket() : OutPacket(OutPacket::Opcode::PARTY_OPERATION)
		{
			write_byte(1);
		}
	};

	class PartyLeavePacket : public OutPacket
	{
	public:
		PartyLeavePacket() : OutPacket(OutPacket::Opcode::PARTY_OPERATION)
		{
			write_byte(2);
		}
	};

	class PartyJoinPacket : public OutPacket
	{
	public:
		PartyJoinPacket(int32_t partyid) : OutPacket(OutPacket::Opcode::PARTY_OPERATION)
		{
			write_byte(3);
			write_int(partyid);
		}
	};

	class PartyInvitePacket : public OutPacket
	{
	public:
		PartyInvitePacket(const std::string& name) : OutPacket(OutPacket::Opcode::PARTY_OPERATION)
		{
			write_byte(4);
			write_string(name);
		}
	};

	class PartyExpelPacket : public OutPacket
	{
	public:
		PartyExpelPacket(int32_t cid) : OutPacket(OutPacket::Opcode::PARTY_OPERATION)
		{
			write_byte(5);
			write_int(cid);
		}
	};

	class PartyChangeLeaderPacket : public OutPacket
	{
	public:
		PartyChangeLeaderPacket(int32_t cid) : OutPacket(OutPacket::Opcode::PARTY_OPERATION)
		{
			write_byte(6);
			write_int(cid);
		}
	};
}
