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
#include "UIAlliance.h"

#include "../Components/MapleButton.h"
#include "../../Net/Packets/AlliancePackets.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIAlliance::UIAlliance() :
		title(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, "Alliance"),
		name_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, ""),
		notice_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, ""),
		guild_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, "")
	{
		dimension = Point<int16_t>(360, 320);

#ifdef USE_NX
		nl::node panel = nl::nx::UI["UIWindow.img"]["UserList"]["GuildUnion"]["backgrnd"];
		if (panel)
		{
			background = panel;
			has_background = true;
			dimension = background.get_dimensions();
		}

		nl::node close = nl::nx::UI["Basic.img"]["BtClose3"];
		if (!close)
			close = nl::nx::UI["Basic.img"]["BtClose2"];
		if (close)
			buttons[Buttons::BtClose] = std::make_unique<MapleButton>(close, Point<int16_t>(dimension.x() - 21, 6));

		nl::node bref = nl::nx::UI["Basic.img"]["BtOK"];
		if (bref)
			buttons[Buttons::BtRefresh] = std::make_unique<MapleButton>(bref, Point<int16_t>(20, dimension.y() - 36));

		nl::node bleave = nl::nx::UI["Basic.img"]["BtCancel"];
		if (bleave)
			buttons[Buttons::BtLeave] = std::make_unique<MapleButton>(bleave, Point<int16_t>(100, dimension.y() - 36));
#endif

		// 打开即请求联盟信息（服务端回 getAllianceInfo 0x0C）
		AllianceActionPacket(0x01).dispatch();
	}

	void UIAlliance::set_alliance(int32_t id, const std::string& name, const std::vector<std::string>& ranks,
		const std::vector<int32_t>& guilds, const std::string& notice)
	{
		alliance_id = id;
		alliance_name = name;
		alliance_notice = notice;
		this->guilds = guilds;
		(void)ranks;

		name_label.change_text("Alliance: " + name);
		notice_label.change_text("Notice: " + notice);
		refresh_guild_label();
	}

	void UIAlliance::set_notice(const std::string& notice)
	{
		alliance_notice = notice;
		notice_label.change_text("Notice: " + notice);
	}

	void UIAlliance::refresh_guild_label()
	{
		std::string txt = "Guilds (" + std::to_string(guilds.size()) + "):";
		for (int32_t g : guilds)
			txt += " " + std::to_string(g);
		guild_label.change_text(txt);
	}

	void UIAlliance::draw(float inter) const
	{
		if (has_background)
			background.draw(position, inter);

		title.draw(position + Point<int16_t>(dimension.x() / 2, 8));
		name_label.draw(position + Point<int16_t>(20, 48));
		notice_label.draw(position + Point<int16_t>(20, 68));
		guild_label.draw(position + Point<int16_t>(20, 88));

		for (const auto& b : buttons)
			if (b.second)
				b.second->draw(position);
	}

	Button::State UIAlliance::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::BtClose:
				deactivate();
				return Button::State::NORMAL;
			case Buttons::BtRefresh:
				AllianceActionPacket(0x01).dispatch();
				return Button::State::NORMAL;
			case Buttons::BtLeave:
				AllianceActionPacket(0x02).dispatch();
				deactivate();
				return Button::State::NORMAL;
		}

		return Button::State::DISABLED;
	}

	UIElement::Type UIAlliance::get_type() const
	{
		return TYPE;
	}
}
