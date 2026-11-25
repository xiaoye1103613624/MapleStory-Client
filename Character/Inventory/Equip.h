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

		// 095 强化（Round 15）：火花 / 宝珠 / 星之力。
		// 新增参数带默认值 → 既有 add_equip 调用点无需改动，旧服务端不下发时全 0。
		Equip(int32_t item_id, int64_t expiration, const std::string& owner, int16_t flags, uint8_t slots, uint8_t level, const EnumMap<EquipStat::Id, uint16_t>& stats, uint8_t itemlevel, int16_t itemexp, int32_t vicious, int16_t p1, int16_t p2, int16_t p3, int16_t bp1, int16_t bp2,
			int16_t spark_id = 0, int16_t soul_id = 0, uint8_t star_force = 0, int16_t bp3 = 0, uint8_t pot_state = 0, uint8_t add_state = 0,
			int64_t limit_break = 0, int64_t add_limit_point = 0, int32_t add_limit_num = 0);

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
		// 095 潜能/魔方：3 条主潜能 + 3 条 bonus 潜能（值为 Potential.img / 197 option id，0 = 空）。
		// 2026-11-25 D2：bonus 扩到第 3 槽，对齐 197 六槽模型。
		int16_t get_potential(size_t index) const;
		int16_t get_bonus_potential(size_t index) const;

		// ⑦ 095 强化三件套（Round 15）。值语义与潜能一致 = 词条/条目 id，0 = 无。
		// 火花（AdditionalOption）：额外属性词条 id。
		int16_t get_spark() const;
		// 宝珠（Soul / 灵魂武器）：灵魂技能条目 id。
		int16_t get_soul() const;
		// 星之力（StarForce）：强化星数 0..25，非词条 id。
		uint8_t get_starforce() const;

		// 197 TMS280 武器级突破上限（方案 C，2026-11-25）：数值加数，非 flag。
		int64_t get_limit_break() const;        // 突破上限卷累计
		int64_t get_add_limit_point() const;    // 极限突破石一次性加成
		int32_t get_add_limit_num() const;      // 0=未用 / 1=已用
		int64_t get_total_limit_break() const;  // 前两者之和

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
		int16_t bonusPotential3;  // 2026-11-25 D2：第 6 槽（附加潜能 3）
		uint8_t potState;         // 197 主潜能品级 state：0=旧id分段回退 / 1..4
		uint8_t addState;         // 197 附加潜能品级 state
		// ⑦ 095 强化（Round 15）
		int16_t spark;
		int16_t soul;
		uint8_t starforce;
		// 197 武器突破上限三字段（追加在装备包尾；旧包不读保持 0）
		int64_t limitBreak;
		int64_t addLimitPoint;
		int32_t addLimitNum;
		Potential potrank;
		EquipQuality::Id quality;
	};
}