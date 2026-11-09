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

#include <string>

#include "EquipQuality.h"

namespace ms
{
	class Equip
	{
	public:
		enum Potential
		{
			POT_NONE,
			POT_HIDDEN,
			POT_RARE,
			POT_EPIC,
			POT_UNIQUE,
			POT_LEGENDARY,
			LENGTH
		};

		Equip(int32_t item_id, int64_t expiration, const std::string& owner, int16_t flags, uint8_t slots, uint8_t level, const EnumMap<EquipStat::Id, uint16_t>& stats, uint8_t itemlevel, int16_t itemexp, int32_t vicious, int16_t p1, int16_t p2, int16_t p3, int16_t bp1, int16_t bp2);

		int32_t get_item_id() const;
		int64_t get_expiration() const;
		const std::string& get_owner() const;
		int16_t get_flags() const;
		uint8_t get_slots() const;
		uint8_t get_level() const;
		uint8_t get_itemlevel() const;
		// 道具经验（成长系统）：原生 tooltip「道具经验」行的数据源，值为百分比整数部分。
		int16_t get_itemexp() const;
		uint16_t get_stat(EquipStat::Id type) const;
		int32_t get_vicious() const;
		Potential get_potrank() const;
		EquipQuality::Id get_quality() const;
		// 095 潜能/魔方：3 条主潜能 + 2 条 bonus 潜能（值为 Potential.img 行 id，0 = 空）。
		int16_t get_potential(size_t index) const;
		int16_t get_bonus_potential(size_t index) const;

	private:
		EnumMap<EquipStat::Id, uint16_t> stats;
		int32_t item_id;
		int64_t expiration;
		std::string owner;
		int16_t flags;
		uint8_t slots;
		uint8_t level;
		uint8_t itemlevel;
		int16_t itemexp;
		int32_t vicious;
		int16_t potential1;
		int16_t potential2;
		int16_t potential3;
		int16_t bonusPotential1;
		int16_t bonusPotential2;
		Potential potrank;
		EquipQuality::Id quality;
	};
}