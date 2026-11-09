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
	// 公会交互包：Opcode GUILD_OPERATION(65)，与 s9 GuildOperationHandler 字段序对齐。
	class GuildCreatePacket : public OutPacket
	{
	public:
		GuildCreatePacket(const std::string& name) : OutPacket(OutPacket::Opcode::GUILD_OPERATION)
		{
			write_byte(0x02);
			write_string(name);
		}
	};

	class GuildInvitePacket : public OutPacket
	{
	public:
		GuildInvitePacket(const std::string& targetName) : OutPacket(OutPacket::Opcode::GUILD_OPERATION)
		{
			write_byte(0x05);
			write_string(targetName);
		}
	};

	class GuildJoinPacket : public OutPacket
	{
	public:
		GuildJoinPacket(int32_t gid, int32_t cid) : OutPacket(OutPacket::Opcode::GUILD_OPERATION)
		{
			write_byte(0x06);
			write_int(gid);
			write_int(cid);
		}
	};

	class GuildLeavePacket : public OutPacket
	{
	public:
		GuildLeavePacket(int32_t cid, const std::string& name) : OutPacket(OutPacket::Opcode::GUILD_OPERATION)
		{
			write_byte(0x07);
			write_int(cid);
			write_string(name);
		}
	};

	class GuildExpelPacket : public OutPacket
	{
	public:
		GuildExpelPacket(int32_t cid, const std::string& name) : OutPacket(OutPacket::Opcode::GUILD_OPERATION)
		{
			write_byte(0x08);
			write_int(cid);
			write_string(name);
		}
	};

	class GuildNoticePacket : public OutPacket
	{
	public:
		GuildNoticePacket(const std::string& notice) : OutPacket(OutPacket::Opcode::GUILD_OPERATION)
		{
			write_byte(0x10);
			write_string(notice);
		}
	};

	// 修改成员职位（s9 GuildOperationHandler case 0x0e：int cid + byte newRank；
	// newRank∈[3,5]，会长(guildrank==1)才可发，服务端校验）。
	class GuildChangeRankPacket : public OutPacket
	{
	public:
		GuildChangeRankPacket(int32_t cid, int8_t new_rank) : OutPacket(OutPacket::Opcode::GUILD_OPERATION)
		{
			write_byte(0x0E);
			write_int(cid);
			write_byte(new_rank);
		}
	};

	// Opcode: DENY_GUILD_REQUEST(127 / 0x7F) — string inviter/guild name
	class DenyGuildRequestPacket : public OutPacket
	{
	public:
		DenyGuildRequestPacket(const std::string& from) : OutPacket(OutPacket::Opcode::DENY_GUILD_REQUEST)
		{
			write_string(from);
		}
	};
}
