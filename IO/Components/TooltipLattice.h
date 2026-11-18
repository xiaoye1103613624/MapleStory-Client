//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																									//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.												//
//																									//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																									//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "..//..//Graphics//Geometry.h"

namespace ms
{
	// 原生 tooltip 十字织纹背板（2026-11-15 从原生截图逐像素提取 41x34 tile，
	// 重建 MAE=1.04；底(79,74,119)/亮线(85,85,130)/暗线(68,68,108)，全不透明）。
	// 原生图案 = 田字格 + 每格大 X：竖亮带 + 水平亮带 + 对角线，41x34 tile 无缝平铺。
	// 平铺 API：GraphicsGL::ensure_raw 上传一次（幂等去重），draw_raw 按整 tile 循环，
	// 边缘不足一个 tile 处留纯底色不拉伸（参照图2 边缘行为）。
	class TooltipLattice
	{
	public:
		static constexpr size_t ATLAS_ID = 0x4C415454; // 'LATT'
		static constexpr int16_t TILE_W = 41;
		static constexpr int16_t TILE_H = 34;

		// 首次调用上传 tile 到共享 atlas；后续调用幂等去重。
		void ensure() const;

		// 在 [topleft, topleft+width x height) 内平铺 tile；不足一个 tile 的边缘留底色。
		void draw(Point<int16_t> topleft, int16_t width, int16_t height) const;
	};
}
