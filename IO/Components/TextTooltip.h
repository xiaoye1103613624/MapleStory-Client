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
	// 5 个 tooltip 组件共用。
	//
	// 2026-11-08 逐像素复校（参考帧《背包中鼠标悬浮物品上显示的物品信息内容.png》）：
	// 背板填充 = 实心 (79,79,119)，四边 1px 亮边 = (85,85,130)，**没有灰色外框**。
	// 旧实现填充 (85,85,130) + 灰边 (128,128,128) 是「背景不对」的主因（偏亮 + 多一圈灰框）。
	// 「实心」不是审美选择：同一块背板压米色窗 (79.2,79.2,119.6) / 压深色画面 (78.6,78.7,124.2)
	// 只差 ΔR=0.6（0.9α 应差 ≈21）⇒ 透出 ≈0.3%，见
	// docs/features/ui-v83-window-restore/evidence/native-verdicts-2026-11-03/native-tooltip-alpha-raw.txt。
	class TooltipBackdrop
	{
	public:
		void draw(Point<int16_t> topleft, int16_t width, int16_t height) const
		{
			ColorBox fill(width, height, Color(79.0f / 255.0f, 79.0f / 255.0f, 119.0f / 255.0f, 1.0f), 1.0f);
			fill.draw(topleft);

			const Color edge(85.0f / 255.0f, 85.0f / 255.0f, 130.0f / 255.0f, 1.0f);

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