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
#include "Equip.h"

namespace ms
{
	Equip::Equip(int32_t item_id, int64_t expiration, const std::string& owner, int16_t flags, uint8_t slots, uint8_t level, const EnumMap<EquipStat::Id, uint16_t>& stats, uint8_t itemlevel, int16_t itemexp, int32_t vicious, int16_t p1, int16_t p2, int16_t p3, int16_t bp1, int16_t bp2) : item_id(item_id), expiration(expiration), owner(owner), flags(flags), slots(slots), level(level), stats(stats), itemlevel(itemlevel), itemexp(itemexp), vicious(vicious), potential1(p1), potential2(p2), potential3(p3), bonusPotential1(bp1), bonusPotential2(bp2)
	{
		// 095 潜能/魔方：无显式 tier 字段时，按主潜能是否存在推导展示等级（POT_RARE 占位，后续可由 Potential.img 细分级）。
		potrank = (p1 != 0 || p2 != 0 || p3 != 0) ? Equip::Potential::POT_RARE : Equip::Potential::POT_NONE;
		quality = EquipQuality::check_quality(item_id, level > 0, stats);
	}

	int32_t Equip::get_item_id() const
	{
		return item_id;
	}

	int64_t Equip::get_expiration() const
	{
		return expiration;
	}

	const std::string& Equip::get_owner() const
	{
		return owner;
	}

	int16_t Equip::get_flags() const
	{
		return flags;
	}

	uint8_t Equip::get_slots() const
	{
		return slots;
	}

	uint8_t Equip::get_level() const
	{
		return level;
	}

	uint8_t Equip::get_itemlevel() const
	{
		return itemlevel;
	}

	int16_t Equip::get_itemexp() const
	{
		return itemexp;
	}

	uint16_t Equip::get_stat(EquipStat::Id type) const
	{
		return stats[type];
	}

	int32_t Equip::get_vicious() const
	{
		return vicious;
	}

	Equip::Potential Equip::get_potrank() const
	{
		return potrank;
	}

	int16_t Equip::get_potential(size_t index) const
	{
		switch (index)
		{
			case 0: return potential1;
			case 1: return potential2;
			case 2: return potential3;
			default: return 0;
		}
	}

	int16_t Equip::get_bonus_potential(size_t index) const
	{
		switch (index)
		{
			case 0: return bonusPotential1;
			case 1: return bonusPotential2;
			default: return 0;
		}
	}

	EquipQuality::Id Equip::get_quality() const
	{
		return quality;
	}
}