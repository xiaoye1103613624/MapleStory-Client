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
#include "UIGuild.h"

#include <nlnx/nx.hpp>
#include <string>

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "UISoftKey.h"
#include "../../Net/Packets/GuildPackets.h"

namespace ms
{
	UIGuild::UIGuild() : UIElement(Point<int16_t>(300, 40), Point<int16_t>(300, 380)),
		title(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK),
		notice_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW, Color::Name::BLACK)
	{
		nl::node guild = nl::nx::UI["GuildUI.img"];
		if (guild["backgrnd"])
			sprites.emplace_back(Sprite(guild["backgrnd"], Point<int16_t>(0, 0)));

		const Point<int16_t> btnpos[] = {
			Point<int16_t>(20, 340),    // BtCreate
			Point<int16_t>(110, 340),   // BtInvite
			Point<int16_t>(200, 340),   // BtNotice
			Point<int16_t>(280, 8)      // BtClose
		};
		const char* btnname[] = { "BtCreate", "BtInvite", "BtNotice", "BtClose" };
		for (uint16_t i = 0; i < 4; i++)
		{
			nl::node bn = guild[btnname[i]];
			buttons[i] = bn
				? std::make_unique<MapleButton>(bn)
				: std::make_unique<MapleButton>(bn, btnpos[i]);
		}
	}

	void UIGuild::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		title.change_text(guild.name.empty() ? "公会" : ("公会: " + guild.name));
		title.draw(position + Point<int16_t>(10, 8));

		notice_label.change_text(guild.notice);
		notice_label.draw(position + Point<int16_t>(10, 28));

		int16_t y = 55;
		for (const auto& m : guild.members)
		{
			OutlinedText row(Text::Font::A11M, Text::Alignment::LEFT,
				Color::Name::WHITE, Color::Name::BLACK);
			std::string info = m.name + " J" + std::to_string(m.job)
				+ " Lv" + std::to_string(m.level)
				+ " 职位" + std::to_string(m.rank)
				+ (m.online ? " [在线]" : " [离线]");
			row.change_text(info);
			row.draw(position + Point<int16_t>(10, y));
			y += ROW_H;
		}

		UIElement::draw_buttons(alpha);
	}

	void UIGuild::update()
	{
	}

	void UIGuild::doubleclick(Point<int16_t> cursorpos)
	{
		int16_t row = row_by_position(cursorpos - position);
		if (row >= 0 && row < static_cast<int16_t>(guild.members.size()))
		{
			const GuildMember& m = guild.members[row];
			GuildExpelPacket(m.id, m.name).dispatch();
		}
	}

	UIElement::Type UIGuild::get_type() const
	{
		return TYPE;
	}

	void UIGuild::set_guild(const GuildData& g)
	{
		guild = g;
	}

	void UIGuild::clear_guild()
	{
		guild = GuildData{};
	}

	void UIGuild::add_member(const GuildMember& m)
	{
		guild.members.push_back(m);
	}

	void UIGuild::remove_member(int32_t id)
	{
		for (size_t i = 0; i < guild.members.size(); i++)
		{
			if (guild.members[i].id == id)
			{
				guild.members.erase(guild.members.begin() + i);
				break;
			}
		}
	}

	void UIGuild::set_member_rank(int32_t id, int32_t rank)
	{
		for (auto& m : guild.members)
			if (m.id == id)
				m.rank = rank;
	}

	void UIGuild::set_member_online(int32_t id, bool online)
	{
		for (auto& m : guild.members)
			if (m.id == id)
				m.online = online;
	}

	void UIGuild::set_notice(const std::string& notice)
	{
		guild.notice = notice;
	}

	int16_t UIGuild::row_by_position(Point<int16_t> pos) const
	{
		int16_t row = (pos.y() - 55) / ROW_H;
		if (pos.x() < 10 || pos.x() > 290 || row < 0)
			return -1;
		return row;
	}

	Button::State UIGuild::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BtCreate:
				prompt_create();
				break;
			case BtInvite:
				prompt_invite();
				break;
			case BtNotice:
				prompt_notice();
				break;
			case BtClose:
				deactivate();
				break;
		}
		return Button::State::NORMAL;
	}

	void UIGuild::prompt_create()
	{
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			if (s.length() >= 3 && s.length() <= 12)
				GuildCreatePacket(s).dispatch();
		});
	}

	void UIGuild::prompt_invite()
	{
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			if (!s.empty())
				GuildInvitePacket(s).dispatch();
		});
	}

	void UIGuild::prompt_notice()
	{
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			if (!s.empty() && s.length() <= 100)
				GuildNoticePacket(s).dispatch();
		});
	}
}
