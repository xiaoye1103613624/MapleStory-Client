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
#include "PartyHandlers.h"

#include "../../IO/UI.h"
#include "../../IO/UITypes/UIParty.h"

namespace ms
{
	void PartyHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.read_byte();

		auto uiparty = UI::get().get_element<UIParty>();
		if (!uiparty)
			uiparty = UI::get().emplace<UIParty>();

		switch (mode)
		{
			case 0x07: // SILENT_UPDATE / LOG_ONOFF
			case 0x0F: // JOIN
			{
				int32_t partyid = recv.read_int();
				if (mode == 0x0F)
					recv.read_string(); // target name

				Party party;
				party.id = partyid;
				parse_party_status(recv, party);
				uiparty->update_party(party);
				break;
			}
			case 0x0C: // LEAVE / EXPEL / DISBAND
			{
				int32_t partyid = recv.read_int();
				int32_t targetid = recv.read_int();
				uint8_t flag = recv.read_byte();

				if (flag == 0)
				{
					// DISBAND
					uiparty->deactivate();
				}
				else
				{
					recv.read_byte();       // expel flag
					recv.read_string();     // target name
					Party party;
					party.id = partyid;
					parse_party_status(recv, party);
					uiparty->update_party(party);
				}
				break;
			}
			case 0x1B: // CHANGE_LEADER
			{
				int32_t targetid = recv.read_int();
				recv.read_byte(); // 0
				uiparty->set_leader(targetid);
				break;
			}
			default:
				break;
		}
	}

	void PartyHandler::parse_party_status(InPacket& recv, Party& party) const
	{
		int32_t ids[6];
		for (int i = 0; i < 6; i++)
			ids[i] = recv.read_int();

		std::string names[6];
		for (int i = 0; i < 6; i++)
			names[i] = recv.read_padded_string(13);

		int32_t jobs[6];
		for (int i = 0; i < 6; i++)
			jobs[i] = recv.read_int();

		int32_t levels[6];
		for (int i = 0; i < 6; i++)
			levels[i] = recv.read_int();

		int32_t chans[6];
		for (int i = 0; i < 6; i++)
			chans[i] = recv.read_int();

		party.leader = recv.read_int();

		int32_t mapids[6];
		for (int i = 0; i < 6; i++)
			mapids[i] = recv.read_int();

		// 6 × 门信息（townId/areaId/x/y），客户端列表无需，跳过
		for (int i = 0; i < 6; i++)
		{
			recv.read_int();
			recv.read_int();
			recv.read_int();
			recv.read_int();
		}

		for (int i = 0; i < 6; i++)
		{
			PartyMember m;
			m.id = ids[i];
			m.name = names[i];
			m.job = jobs[i];
			m.level = levels[i];
			m.channel = chans[i];
			m.mapid = mapids[i];
			m.online = (chans[i] != -2);
			party.members[i] = m;
		}
	}

	void PartyMemberHPHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.read_int();
		int32_t curhp = recv.read_int();
		int32_t maxhp = recv.read_int();

		auto uiparty = UI::get().get_element<UIParty>();
		if (uiparty)
			uiparty->update_member_hp(cid, curhp, maxhp);
	}
}
