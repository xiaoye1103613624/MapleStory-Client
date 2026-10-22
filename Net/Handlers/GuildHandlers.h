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

#include "../PacketHandler.h"

#include "../../IO/UITypes/UIGuild.h"

namespace ms
{
	// 解析服务端下发的公会包（GUILD_OPERATION=65）。
	// 字段序以 s9 GuildPackets.getGuildInfo(0x1A)/guildInvite(0x05)/newGuildMember(0x27)/
	// memberLeft(0x2c/0x2f)/changeRank(0x40)/guildNotice(0x44)/guildMemberOnline(0x3d) 为准。
	class GuildHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};
}
