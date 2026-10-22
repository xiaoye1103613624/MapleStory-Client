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
#include "UIParty.h"

#include <nlnx/nx.hpp>
#include <string>

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "UISoftKey.h"
#include "../../Net/Packets/PartyPackets.h"

namespace ms
{
	UIParty::UIParty() : UIElement(Point<int16_t>(520, 20), Point<int16_t>(200, 300)),
		title(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK)
	{
		nl::node party = nl::nx::UI["Party.img"];
		if (party["backgrnd"])
			sprites.emplace_back(Sprite(party["backgrnd"], Point<int16_t>(0, 0)));

		const Point<int16_t> btnpos[] = {
			Point<int16_t>(20, 250),   // BtInvite
			Point<int16_t>(100, 250),  // BtLeave
			Point<int16_t>(180, 8)     // BtClose
		};
		const char* btnname[] = { "BtInvite", "BtLeave", "BtClose" };
		for (uint16_t i = 0; i < 3; i++)
		{
			nl::node bn = party[btnname[i]];
			buttons[i] = bn
				? std::make_unique<MapleButton>(bn)
				: std::make_unique<MapleButton>(bn, btnpos[i]);
		}
	}

	void UIParty::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		title.change_text("队伍 #" + std::to_string(party.id));
		title.draw(position + Point<int16_t>(10, 8));

		int16_t y = 40;
		for (const auto& m : party.members)
		{
			if (m.id == 0)
				continue;

			OutlinedText row(Text::Font::A11M, Text::Alignment::LEFT,
				Color::Name::WHITE, Color::Name::BLACK);

			std::string info = m.name + " (Lv." + std::to_string(m.level) + " J" + std::to_string(m.job) + ")";
			if (m.online)
				info += " 频道" + std::to_string(m.channel + 1);
			else
				info += " 离线";
			if (m.id == party.leader)
				info += " ★";

			row.change_text(info);
			row.draw(position + Point<int16_t>(10, y));
			y += 30;
		}

		UIElement::draw_buttons(alpha);
	}

	void UIParty::update()
	{
	}

	UIElement::Type UIParty::get_type() const
	{
		return TYPE;
	}

	void UIParty::update_party(const Party& p)
	{
		party = p;
	}

	void UIParty::set_leader(int32_t cid)
	{
		party.leader = cid;
	}

	void UIParty::update_member_hp(int32_t cid, int32_t curhp, int32_t maxhp)
	{
		memberhp[cid] = { curhp, maxhp };
	}

	Button::State UIParty::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BtInvite:
				prompt_invite();
				break;
			case BtLeave:
				PartyLeavePacket().dispatch();
				break;
			case BtClose:
				deactivate();
				break;
		}
		return Button::State::NORMAL;
	}

	void UIParty::prompt_invite()
	{
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			if (!s.empty())
				PartyInvitePacket(s).dispatch();
		});
	}
}
