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
#include "ScrollingNotice.h"

namespace ms
{
	ScrollingNotice::ScrollingNotice()
	{
		width = 800;
		background = ColorBox(width, 23, Color::Name::BLACK, 0.535f);
		notice = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::YELLOW);

		xpos.set(0.0);
		active = false;
	}

	void ScrollingNotice::setnotice(std::string n)
	{
		notice.change_text(n);
		xpos.set(static_cast<double>(width));
		active = n.size() > 0;
	}

	void ScrollingNotice::draw(float alpha) const
	{
		if (active)
		{
			int16_t interx = static_cast<int16_t>(std::round(xpos.get(alpha)));
			auto position = Point<int16_t>(interx, -1);

			background.draw(Point<int16_t>(0, 0));
			notice.draw(position);
		}
	}

	void ScrollingNotice::update()
	{
		if (active)
		{
			int16_t new_width = Constants::Constants::get().get_viewwidth();

			if (new_width != width)
			{
				width = new_width;
				background.setwidth(width);
				xpos.set(static_cast<double>(width));
			}

			xpos -= 0.5;

			auto xmin = static_cast<double>(-notice.width());

			// 原生行为：公告文字完整滚出左边界后，连同 23px 全宽黑底一起消失。
			// 旧实现把 xpos 绕回右边界、active 永远为真，于是登录后一条
			// 「Welcome to Scania!」就让屏幕顶部 0..22 变成永久半透明黑条
			// （参考端同一位置是天空 (255,238,221)，本端被压成 (119,111,103)），
			// 并把落进该区间的一切 UI 一起压暗——小地图标题栏只是最先被发现的那个。
			if (xpos.last() < xmin)
				active = false;
		}
	}
}