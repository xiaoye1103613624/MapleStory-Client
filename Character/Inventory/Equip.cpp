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
	// 装备潜能等级推导（2026-11-25 197 同步）：
	// 优先用服务端下发的独立 state（197 Equip.getState()），值域 1..4 = B/A/S/SS；
	// state=0 时回退旧 id 分段（1..50，4 段等分），保证旧服/旧存档平滑兼容。
	static Equip::Potential potential_rank_of(int16_t first_potential, uint8_t state = 0)
	{
		if (state >= 1 && state <= 4) {
			switch (state) {
				case 1: return Equip::Potential::POT_RARE;      // B
				case 2: return Equip::Potential::POT_EPIC;      // A
				case 3: return Equip::Potential::POT_UNIQUE;    // S
				case 4: return Equip::Potential::POT_LEGENDARY; // SS
				default: break;
			}
		}
		if (first_potential <= 0)
			return Equip::Potential::POT_NONE;
		if (first_potential <= 12)
			return Equip::Potential::POT_RARE;      // B - 旧 id 段 1-12
		if (first_potential <= 24)
			return Equip::Potential::POT_EPIC;      // A - 旧 id 段 13-24
		if (first_potential <= 36)
			return Equip::Potential::POT_UNIQUE;    // S - 旧 id 段 25-36
		return Equip::Potential::POT_LEGENDARY;     // SS - 旧 id 段 37-50
	}

	Equip::Equip(int32_t item_id, int64_t expiration, const std::string& owner, int16_t flags, uint8_t slots, uint8_t level, const EnumMap<EquipStat::Id, uint16_t>& stats, uint8_t itemlevel, int16_t itemexp, int32_t vicious, int16_t p1, int16_t p2, int16_t p3, int16_t bp1, int16_t bp2, int16_t spark_id, int16_t soul_id, uint8_t star_force, int16_t bp3, uint8_t pot_state, uint8_t add_state, int64_t limit_break, int64_t add_limit_point, int32_t add_limit_num) : item_id(item_id), expiration(expiration), owner(owner), flags(flags), slots(slots), level(level), stats(stats), itemlevel(itemlevel), itemexp(itemexp), vicious(vicious), potential1(p1), potential2(p2), potential3(p3), bonusPotential1(bp1), bonusPotential2(bp2), bonusPotential3(bp3), potState(pot_state), addState(add_state), spark(spark_id), soul(soul_id), starforce(star_force), limitBreak(limit_break), addLimitPoint(add_limit_point), addLimitNum(add_limit_num)
	{
		// 095 潜能等级推导（2026-11-20 据 BWIKI + 095 getState 字节码还原）：
		// 原生 equip.getState() 从 5 个潜能 id 的数值段位推导等级（零服务端字段），
		// BWIKI 规则「第一条潜能 = 装备潜能等级」，且协议只发 5 个 short、无 rank 字段。
		// 本端 id 空间是 1..50 简化（服务端 r.nextInt(50)+1），按第一条潜能 p1 分段定级：
		//   0 → NONE / 1-12 → B(RARE) / 13-24 → A(EPIC) / 25-36 → S(UNIQUE) / 37-50 → SS(LEGENDARY)。
		// 与 locale equip.potline.* 文案分段（固定值/+3%/+6%/+9%）一一对应。
		potrank = potential_rank_of(p1, pot_state);

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
			case 2: return bonusPotential3;  // 2026-11-25 D2 第6槽
			default: return 0;
		}
	}

	int16_t Equip::get_spark() const
	{
		return spark;
	}

	int16_t Equip::get_soul() const
	{
		return soul;
	}

	uint8_t Equip::get_starforce() const
	{
		return starforce;
	}

	int64_t Equip::get_limit_break() const
	{
		return limitBreak;
	}

	int64_t Equip::get_add_limit_point() const
	{
		return addLimitPoint;
	}

	int32_t Equip::get_add_limit_num() const
	{
		return addLimitNum;
	}

	int64_t Equip::get_total_limit_break() const
	{
		return limitBreak + addLimitPoint;
	}

	EquipQuality::Id Equip::get_quality() const
	{
		return quality;
	}
}