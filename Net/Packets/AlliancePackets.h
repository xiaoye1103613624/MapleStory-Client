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
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.	//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	class OutPacket;

	// 联盟出站包：Opcode ALLIANCE_OPERATION(143)，对接 s9 AllianceOperationHandler(action 字节)
	class AllianceActionPacket : public OutPacket
	{
	public:
		AllianceActionPacket(int8_t action) : OutPacket(OutPacket::Opcode::ALLIANCE_OPERATION)
		{
			write_byte(action);
		}
	};

	// 修改联盟公告（action 0x0A）
	class AllianceNoticePacket : public OutPacket
	{
	public:
		AllianceNoticePacket(const std::string& notice) : OutPacket(OutPacket::Opcode::ALLIANCE_OPERATION)
		{
			write_byte(0x0A);
			write_string(notice);
		}
	};

	// 邀请公会加入联盟（action 0x03）
	class AllianceInvitePacket : public OutPacket
	{
	public:
		AllianceInvitePacket(const std::string& guildName) : OutPacket(OutPacket::Opcode::ALLIANCE_OPERATION)
		{
			write_byte(0x03);
			write_string(guildName);
		}
	};

	// Opcode: DENY_ALLIANCE_REQUEST(144 / 0x90) — byte(0) + inviter + guild
	class DenyAllianceRequestPacket : public OutPacket
	{
	public:
		DenyAllianceRequestPacket(const std::string& inviter, const std::string& guild)
			: OutPacket(OutPacket::Opcode::DENY_ALLIANCE_REQUEST)
		{
			write_byte(0);
			write_string(inviter);
			write_string(guild);
		}
	};
}
