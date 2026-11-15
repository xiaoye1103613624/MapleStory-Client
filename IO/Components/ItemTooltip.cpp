//////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////
#include "ItemTooltip.h"

#include "../../Constants.h"

#include "../../Data/ItemData.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		constexpr int16_t ICON_BOX_SIZE = 32;
		constexpr float ICON_BOX_SCALE = 2.0f;
		constexpr int16_t ICON_BOX_X = 6;
		constexpr int16_t ICON_BOX_Y = 70;
		constexpr int16_t ICON_OFF_X = 2;
		constexpr int16_t ICON_OFF_Y = 2;
		constexpr GLfloat ICON_BOX_R = 74.0f / 255.0f;
		constexpr GLfloat ICON_BOX_G = 74.0f / 255.0f;
		constexpr GLfloat ICON_BOX_B = 94.0f / 255.0f;
	}

	ItemTooltip::ItemTooltip()
	{
		itemid = 0;
		iconbase_box = ColorBox(ICON_BOX_SIZE, ICON_BOX_SIZE,
			Color(ICON_BOX_R, ICON_BOX_G, ICON_BOX_B, 1.0f), 1.0f);
	}

	bool ItemTooltip::set_item(int32_t iid)
	{
		if (itemid == iid)
			return false;

		itemid = iid;

		if (itemid == 0)
			return false;

		const ItemData& idata = ItemData::get(itemid);

		itemicon = idata.get_icon(idata.get_id() / 10000 == 238);
		untradable = idata.is_untradable();
		unique = idata.is_unique();

		std::string quality = "";

		if (unique && untradable)
			quality = "One-of-a-kind Item, Untradable";
		else if (unique && !untradable)
			quality = "One-of-a-kind Item";
		else if (!unique && untradable)
			quality = "Untradable";
		else
			quality = "";

		name = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, idata.get_name(), 240);
		// 与装备 tip 正文色对齐 TOOLTIPBODY（奶油色）。
		desc = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::TOOLTIPBODY, idata.get_desc(), 185);
		qual = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::ORANGE, quality, 185);

		fillwidth = 264;
		fillheight = 83 + name.height();
		descdelta = desc.height() - 80;

		if (descdelta > 0)
			fillheight += descdelta;

		return true;
	}

	void ItemTooltip::draw_item_icon(Point<int16_t> pos) const
	{
		const Point<int16_t> boxpos = pos + Point<int16_t>(ICON_BOX_X, ICON_BOX_Y);
		iconbase_box.draw(boxpos);

		if (!itemicon.is_valid())
			return;

		// 与 EquipTooltip 同口径：2x 绘制时把 origin*2 加回，使图标左上角落在底板 (2,2)。
		const Point<int16_t> ivorg = itemicon.get_origin();
		const Point<int16_t> iconpos(
			boxpos.x() + ICON_OFF_X + static_cast<int16_t>(ivorg.x() * ICON_BOX_SCALE),
			boxpos.y() + ICON_OFF_Y + static_cast<int16_t>(ivorg.y() * ICON_BOX_SCALE)
		);

		itemicon.draw(DrawArgument(iconpos, ICON_BOX_SCALE, ICON_BOX_SCALE));
	}

	void ItemTooltip::draw(Point<int16_t> pos) const
	{
		if (itemid == 0)
			return;

		int16_t max_width = Constants::Constants::get().get_viewwidth();
		int16_t max_height = Constants::Constants::get().get_viewheight();
		int16_t cur_width = pos.x() + fillwidth + 32;
		int16_t cur_height = pos.y() + fillheight + 40;

		int16_t adj_x = cur_width - max_width;
		int16_t adj_y = cur_height - max_height;

		int16_t adj_d = descdelta > 0 ? descdelta : 0;
		int16_t adj_t = (untradable || unique) ? 19 : 0;

		if (adj_x > 0)
			pos.shift_x(adj_x * -1);

		if (adj_y > 0)
			pos.shift_y(adj_y * -1);

		// 框体 = 内容包围盒外扩 6px（Q1：程序描边，见 TextTooltip.h TooltipBackdrop）
		backdrop.draw(pos + Point<int16_t>(-6, -6), fillwidth + 28, fillheight + adj_t + adj_d + 18);
		name.draw(pos + Point<int16_t>(22, 8));

		if (untradable || unique)
			qual.draw(pos + Point<int16_t>(148, 27));

		pos.shift(14, 18 + name.height() + adj_t);

		draw_item_icon(pos);
		desc.draw(pos + Point<int16_t>(90, -6));
	}
}
