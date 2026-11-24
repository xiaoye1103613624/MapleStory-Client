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

#include "../../Template/Point.h"

#include <string>
#include <vector>

namespace ms
{
	// 世界地图悬浮提示的在线玩家行（跨 UI/UIState/MapTooltip 共享，避免头文件反向依赖）。
	struct MapTooltipPlayer
	{
		std::string name;
		int16_t level = 0;
		int8_t channel = 0;
	};

	// Interface for tooltips
	// Window with helpful information that appears on mouse hover at a specific location
	class Tooltip
	{
	public:
		// Possible parent UIs for Tooltips
		enum Parent
		{
			NONE,
			EQUIPINVENTORY,
			ITEMINVENTORY,
			SKILLBOOK,
			SHOP,
			EVENT,
			TEXT,
			KEYCONFIG,
			WORLDMAP,
			MINIMAP
		};

		virtual ~Tooltip() {}

		virtual void draw(Point<int16_t> cursorpos) const = 0;
	};
}