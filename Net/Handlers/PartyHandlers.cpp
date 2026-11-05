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
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIOptionMenu.h"
#include "../../IO/UITypes/UIParty.h"
#include "../../IO/UITypes/UIPartySearch.h"
#include "../../IO/UITypes/UINotice.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Net/Packets/PartyPackets.h"
#include "../../Net/Packets/PartySearchPackets.h"
#include "../../Util/Misc.h"

#include <string>

namespace ms
{
	void PartyHandler::handle(InPacket& recv) const
	{
		auto ensure_sm = [](Color::Name color, const std::string& tip) {
			auto mt = (color == Color::Name::RED) ? UIChatBar::MessageType::RED
				: (color == Color::Name::WHITE) ? UIChatBar::MessageType::WHITE
				: UIChatBar::MessageType::YELLOW;
			if (auto chatbar = UI::get().get_element<UIChatBar>())
			{
				chatbar->show_message(tip.c_str(), mt);
				return;
			}
		};

		uint8_t mode = recv.read_byte();

		auto uiparty = UI::get().get_element<UIParty>();
		auto ensure_party = [&]() -> Optional<UIParty> {
			if (!uiparty)
				uiparty = UI::get().emplace<UIParty>();
			else if (!uiparty->is_active())
				uiparty->makeactive();
			return uiparty;
		};

		switch (mode)
		{
			case 0x04: // partyInvite / partySearchInvite
			{
				int32_t partyid = recv.read_int();
				std::string from = recv.read_string();
				recv.read_byte();

				while (recv.available())
					recv.skip_byte();
				bool from_search = (from.size() >= 4 && from.compare(0, 4, "PS: ") == 0);
				std::string display = from_search ? from.substr(4) : from;
				// 游戏设置：组队邀请 / 寻找组队 —— 拒绝位则静默回绝，不弹 YesNo
				auto opt = from_search ? UIGameOpt::PARTY_SEARCH : UIGameOpt::PARTY_APPLY;
				if (!UIGameOpt::accepts(opt))
				{
					DenyPartyRequestPacket(from).dispatch();
					if (from_search)
					{
						if (auto ps = UI::get().get_element<UIPartySearch>())
							ps->set_searching(false);
					}
					LOG(LOG_DEBUG, "PartyHandler: GameOpt refuse "
						<< (from_search ? "PARTY_SEARCH" : "PARTY_APPLY")
						<< " from=" << display);
					break;
				}
				std::string msg = from_search
					? (display + " found you via Party Search. Join their party?")
					: (display + " has invited you to a party.");
				// YesNo 已显，不重复 SM

				UI::get().emplace<UIYesNo>(msg, [partyid, from, from_search](bool yes)
				{
					if (yes)
					{
						PartyJoinPacket(partyid).dispatch();
					}
					else
					{
						// Deny 包需带原始串（含可选 "PS: "），供服务端 split
						DenyPartyRequestPacket(from).dispatch();
					}

					if (from_search)
					{
						if (auto ps = UI::get().get_element<UIPartySearch>())
							ps->set_searching(false);
					}
				});

				break;
			}
			case 0x07: // SILENT_UPDATE / LOG_ONOFF
			case 0x0F: // JOIN — 开/刷新队伍窗
			{
				int32_t partyid = recv.read_int();
				if (mode == 0x0F)
					recv.read_string(); // target name

				Party party;
				party.id = partyid;
				parse_party_status(recv, party);
				while (recv.available())
					recv.skip_byte();
				if (ensure_party())
					uiparty->update_party(party);
				// Party UI 已刷，不 SM
				break;
			}
			case 0x0C: // LEAVE / EXPEL / DISBAND
			{
				int32_t partyid = recv.read_int();
				int32_t targetid = recv.read_int();
				uint8_t flag = recv.read_byte();

				if (flag == 0)
				{
					while (recv.available())
						recv.skip_byte();
					// DISBAND：已开窗直接关；未开才 tip
					if (uiparty && uiparty->is_active())
						uiparty->deactivate();
					else
						ensure_sm(Color::Name::WHITE, "Party disbanded");
				}
				else
				{
					recv.read_byte();       // expel flag
					std::string tname = recv.read_string();
					Party party;
					party.id = partyid;
					parse_party_status(recv, party);
					while (recv.available())
						recv.skip_byte();
					if (ensure_party())
						uiparty->update_party(party);
					else if (!tname.empty())
						ensure_sm(Color::Name::WHITE, tname + " left");
				}
				(void)targetid;
				break;
			}
			case 0x1B: // CHANGE_LEADER
			{
				int32_t targetid = recv.read_int();
				recv.read_byte(); // 0
				while (recv.available())
					recv.skip_byte();
				if (ensure_party())
					uiparty->set_leader(targetid);
				else
					ensure_sm(Color::Name::YELLOW,
						"Party leader -> #" + std::to_string(targetid));
				(void)targetid;
				break;
			}
			default:
			{
				LOG(LOG_DEBUG, "Party mode=" << static_cast<int>(mode)
					<< " rem=" << recv.available());
				while (recv.available())
					recv.skip_byte();
				// 未知/失败码：已开 Party 静默；否则 tip
				static const char* fails[] = {
					nullptr,
					"Cannot create party",
					"Already in a party",
					"Party name taken",
					nullptr,
					"Party full",
					"Cannot find player",
					nullptr,
					"Not in a party",
					"Not party leader",
					"Invite declined"
				};
				const char* msg = (mode < 11) ? fails[mode] : nullptr;
				std::string tip = msg ? msg : ("Party mode " + std::to_string(mode));
				if (uiparty && uiparty->is_active())
					break;
				if (msg || mode != 0)
					ensure_sm(Color::Name::YELLOW, tip);
				break;
			}
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
		while (recv.available())
			recv.skip_byte();

		auto uiparty = UI::get().get_element<UIParty>();
		if (!uiparty)
			uiparty = UI::get().emplace<UIParty>();
		else if (!uiparty->is_active())
			uiparty->makeactive();
		if (uiparty)
			uiparty->update_member_hp(cid, curhp, maxhp);
		// 血条 UI 已刷，不 tip
		(void)cid; (void)curhp; (void)maxhp;
	}
}
