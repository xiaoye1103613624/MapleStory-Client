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
#include "UIBuddyList.h"

#include <nlnx/nx.hpp>
#include <string>

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "UISoftKey.h"
#include "../../Net/Packets/BuddyPackets.h"

namespace ms
{
	UIBuddyList::UIBuddyList() : UIElement(Point<int16_t>(740, 20), Point<int16_t>(200, 320)),
		title(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK)
	{
		nl::node buddy = nl::nx::UI["BuddyList.img"];
		if (buddy["backgrnd"])
			sprites.emplace_back(Sprite(buddy["backgrnd"], Point<int16_t>(0, 0)));

		const Point<int16_t> btnpos[] = {
			Point<int16_t>(20, 280),   // BtAdd
			Point<int16_t>(180, 8)     // BtClose
		};
		const char* btnname[] = { "BtAdd", "BtClose" };
		for (uint16_t i = 0; i < 2; i++)
		{
			nl::node bn = buddy[btnname[i]];
			buttons[i] = bn
				? std::make_unique<MapleButton>(bn)
				: std::make_unique<MapleButton>(bn, btnpos[i]);
		}
	}

	void UIBuddyList::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		title.change_text("好友 (" + std::to_string(list.buddies.size()) + "/" + std::to_string(list.capacity) + ")");
		title.draw(position + Point<int16_t>(10, 8));

		int16_t y = 40;
		for (const auto& b : list.buddies)
		{
			OutlinedText row(Text::Font::A11M, Text::Alignment::LEFT,
				Color::Name::WHITE, Color::Name::BLACK);

			std::string info = b.name;
			if (!b.group.empty() && b.group != "All")
				info += " [" + b.group + "]";
			info += b.online ? (" 频道" + std::to_string(b.channel + 1)) : " 离线";
			row.change_text(info);
			row.draw(position + Point<int16_t>(10, y));
			y += ROW_H;
		}

		UIElement::draw_buttons(alpha);
	}

	void UIBuddyList::update()
	{
	}

	void UIBuddyList::doubleclick(Point<int16_t> cursorpos)
	{
		int16_t row = row_by_position(cursorpos - position);
		if (row >= 0 && row < static_cast<int16_t>(list.buddies.size()))
			BuddyDeletePacket(list.buddies[row].cid).dispatch();
	}

	UIElement::Type UIBuddyList::get_type() const
	{
		return TYPE;
	}

	void UIBuddyList::update_buddies(const BuddyListData& data)
	{
		list = data;
	}

	void UIBuddyList::set_capacity(int32_t cap)
	{
		list.capacity = cap;
	}

	int32_t UIBuddyList::get_capacity() const
	{
		return list.capacity;
	}

	void UIBuddyList::update_buddy_channel(int32_t cid, int32_t channel)
	{
		for (auto& b : list.buddies)
		{
			if (b.cid == cid)
			{
				b.channel = channel;
				b.online = (channel != -2 && channel != -1);
				break;
			}
		}
	}

	Button::State UIBuddyList::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BtAdd:
				prompt_add();
				break;
			case BtClose:
				deactivate();
				break;
		}
		return Button::State::NORMAL;
	}

	int16_t UIBuddyList::row_by_position(Point<int16_t> pos) const
	{
		int16_t row = (pos.y() - 40) / ROW_H;
		if (pos.x() < 10 || pos.x() > 190 || row < 0)
			return -1;
		return row;
	}

	void UIBuddyList::prompt_add()
	{
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			if (!s.empty())
				BuddyAddPacket(s, "All").dispatch();
		});
	}
}
