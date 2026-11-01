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
#pragma once

#include "Tooltip.h"

#include "../../Graphics/Geometry.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

namespace ms
{
	// Q1 定案（PLAN §6）：v83 原生 tooltip 框体是程序描边而非贴图 —— 包内无 UIToolTip.img、
	// UIWindow.img/ToolTip 无九切片，且包内全部九切片族均为浅底气泡（证据见
	// docs/features/ui-v83-window-restore/evidence/w5-tooltip/coords.md）。
	// 用半透明黑底 + 1px 灰边近似原生深底细边观感，5 个 tooltip 组件共用。
	class TooltipBackdrop
	{
	public:
		void draw(Point<int16_t> topleft, int16_t width, int16_t height) const
		{
			ColorBox fill(width, height, Color(0.0f, 0.0f, 0.0f, 0.9f), 1.0f);
			fill.draw(topleft);

			const Color edge(0.5f, 0.5f, 0.5f, 1.0f);

			ColorBox top(width, 1, edge, 1.0f);
			top.draw(topleft);

			ColorBox bottom(width, 1, edge, 1.0f);
			bottom.draw(topleft + Point<int16_t>(0, height - 1));

			ColorBox side(1, height - 2, edge, 1.0f);
			side.draw(topleft + Point<int16_t>(0, 1));
			side.draw(topleft + Point<int16_t>(width - 1, 1));
		}
	};

	class TextTooltip : public Tooltip
	{
	public:
		TextTooltip();

		void draw(Point<int16_t> position) const override;

		bool set_text(std::string text, uint16_t maxwidth = 340, bool formatted = true, int16_t line_adj = 2);

	private:
		TooltipBackdrop backdrop;
		std::string text;
		Text text_label;
	};
}