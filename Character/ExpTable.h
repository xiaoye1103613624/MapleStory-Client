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

#include <array>

namespace ms
{
	namespace ExpTable
	{
		constexpr size_t LEVELCAP = 256;
		extern const std::array<int64_t, LEVELCAP> values;

		// 升到下一级所需经验。level 超过经验表范围（>255）时回落到与服务端一致的封顶值
		// 2000000000，以支持超过 256 的等级上限（如 999）。内部做边界钳制，level<=0 返回
		// values[0]，绝不返回 0，调用方无需担心除零。
		int64_t required_exp(int level);
	}
}