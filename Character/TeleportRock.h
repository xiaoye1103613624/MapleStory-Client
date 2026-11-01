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

#include <vector>
#include <cstdint>

namespace ms
{
	// Saved locations for the 'teleport rock' and 'VIP teleport rock' cash items
	class TeleportRock
	{
	public:
		void addlocation(int32_t);
		void addviplocation(int32_t);
		void clearlocations();
		void clearviplocations();
		const std::vector<int32_t>& get_locations() const { return locations; }
		const std::vector<int32_t>& get_viplocations() const { return viplocations; }
		// 第一个有效地图（非 999999999 / 0）
		int32_t first_valid(bool vip) const;

	private:
		std::vector<int32_t> locations;
		std::vector<int32_t> viplocations;
	};
}