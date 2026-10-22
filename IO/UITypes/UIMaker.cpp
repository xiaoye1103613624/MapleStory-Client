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
#include "UIMaker.h"

#include "../Components/MapleButton.h"
#include "../../Net/Packets/MakerPackets.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIMaker::UIMaker() :
		title(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, "Maker"),
		status(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR,
			"Maker recipes require Maker.img (not mounted by nlnx loader).")
	{
		dimension = Point<int16_t>(380, 460);

#ifdef USE_NX
		nl::node panel = nl::nx::UI["UIWindow.img"]["Maker"]["backgrnd"];
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
#endif

		list_origin = Point<int16_t>(24, 86);
		load_recipes();
	}

	void UIMaker::load_recipes()
	{
		// 注意：nlnx 仅挂载标准根（UI/String/Mob/Map/Skill/Item…），无 Maker.img 命名空间
		// 亦无通用 get_root()/load()，故客户端无法列出配方。保留配方结构，待 Maker.img
		// 挂载（客户端数据加载变更）后可填充；当前窗口仅作制造发包管线外壳。
		recipes.clear();
		icons.clear();
		names.clear();
		costs.clear();
		scroll = 0;
	}

	void UIMaker::draw(float inter) const
	{
		if (has_background)
			background.draw(position, inter);

		title.draw(position + Point<int16_t>(dimension.x() / 2, 8));

		if (auto it = buttons.find(Buttons::BtClose); it != buttons.end() && it->second)
			it->second->draw(position);

		if (recipes.empty())
		{
			status.draw(position + list_origin);
			return;
		}

		int16_t visible = (dimension.y() - list_origin.y() - 16) / row_h;
		int16_t first = scroll / row_h;

		for (int16_t i = first; i < first + visible && i < static_cast<int16_t>(recipes.size()); i++)
		{
			Point<int16_t> base = position + list_origin + Point<int16_t>(0, i * row_h - scroll);
			icons[i].draw(base + Point<int16_t>(0, 8), inter);
			names[i].draw(base + Point<int16_t>(40, 4));
			costs[i].draw(base + Point<int16_t>(40, 26));
		}
	}

	Button::State UIMaker::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::BtClose:
				deactivate();
				return Button::State::NORMAL;
		}

		return Button::State::DISABLED;
	}

	Cursor::State UIMaker::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (clicked)
		{
			Point<int16_t> local = cursorpos - position - list_origin + Point<int16_t>(0, scroll);
			if (local.x() >= 0 && local.x() < dimension.x() - 40 && local.y() >= 0)
			{
				int16_t row = local.y() / row_h;
				if (row >= 0 && row < static_cast<int16_t>(recipes.size()))
					MakerCreatePacket(recipes[row].id, recipes[row].itemId).dispatch();
			}
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UIMaker::send_scroll(double yoffset)
	{
		scroll += static_cast<int16_t>(yoffset * row_h);

		int16_t maxscroll = std::max(
			0,
			static_cast<int16_t>(static_cast<int32_t>(recipes.size()) * row_h) - (dimension.y() - list_origin.y() - 16)
		);

		if (scroll < 0)
			scroll = 0;
		if (scroll > maxscroll)
			scroll = maxscroll;
	}

	UIElement::Type UIMaker::get_type() const
	{
		return TYPE;
	}
}
