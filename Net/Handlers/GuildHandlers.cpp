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
#include "GuildHandlers.h"

#include "../../IO/UI.h"
#include "../../IO/UITypes/UIGuild.h"

namespace ms
{
	void GuildHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.read_byte();

		auto uiguild = UI::get().get_element<UIGuild>();
		if (!uiguild)
			uiguild = UI::get().emplace<UIGuild>();

		switch (mode)
		{
			case 0x1A: // getGuildInfo
			{
				uint8_t inGuild = recv.read_byte();
				if (inGuild == 0)
				{
					uiguild->clear_guild();
					uiguild->deactivate();
					break;
				}
				GuildData g;
				g.id = recv.read_int();
				g.name = recv.read_string();
				for (int i = 0; i < 5; i++)
					g.rankTitles[i] = recv.read_string();

				int16_t count = recv.read_byte();
				std::vector<int32_t> ids(count);
				for (int16_t i = 0; i < count; i++)
					ids[i] = recv.read_int();
				for (int16_t i = 0; i < count; i++)
				{
					GuildMember m;
					m.id = ids[i];
					m.name = recv.read_padded_string(13);
					m.job = recv.read_int();
					m.level = recv.read_int();
					m.rank = recv.read_int();
					m.online = (recv.read_int() == 1);
					recv.read_int(); // signature
					m.allianceRank = recv.read_int();
					g.members.push_back(m);
				}
				g.capacity = recv.read_int();
				recv.read_short(); // logoBG
				recv.read_byte();  // logoBGColor
				recv.read_short(); // logo
				recv.read_byte();  // logoColor
				g.notice = recv.read_string();
				g.gp = recv.read_int();
				g.allianceId = recv.read_int();
				uiguild->set_guild(g);
				break;
			}
			case 0x05: // guildInvite
			{
				recv.read_int(); // guildId
				recv.read_string(); // name
				// 收到邀请：v1 不自动接受（自动接受需玩家自身 cid，留待后续「地图玩家交互」任务）
				break;
			}
			case 0x27: // newGuildMember
			{
				recv.read_int(); // guildId
				GuildMember m;
				m.id = recv.read_int();
				m.name = recv.read_padded_string(13);
				m.job = recv.read_int();
				m.level = recv.read_int();
				m.rank = recv.read_int();
				m.online = (recv.read_int() == 1);
				recv.read_int(); // signature
				m.allianceRank = recv.read_int();
				uiguild->add_member(m);
				break;
			}
			case 0x2c: // member left
			case 0x2f: // member expelled
			{
				recv.read_int(); // guildId
				int32_t id = recv.read_int();
				recv.read_string(); // name
				uiguild->remove_member(id);
				break;
			}
			case 0x40: // changeRank
			{
				recv.read_int(); // guildId
				int32_t id = recv.read_int();
				int32_t rank = recv.read_byte();
				uiguild->set_member_rank(id, rank);
				break;
			}
			case 0x44: // guildNotice
			{
				recv.read_int(); // guildId
				std::string notice = recv.read_string();
				uiguild->set_notice(notice);
				break;
			}
			case 0x3d: // guildMemberOnline
			{
				recv.read_int(); // guildId
				int32_t id = recv.read_int();
				bool online = recv.read_bool();
				uiguild->set_member_online(id, online);
				break;
			}
			default: // genericGuildMessage 等状态字节
				break;
		}
	}
}
