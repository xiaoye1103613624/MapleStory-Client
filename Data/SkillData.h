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

#include "../Graphics/Texture.h"
#include "../Template/Cache.h"

#include "../Character/Inventory/Weapon.h"
#include <vector>

namespace ms
{
	// Contains information about a skill
	class SkillData : public Cache<SkillData>
	{
	public:
		// The stats of one level
		struct Stats
		{
			float damage;
			int32_t matk;
			int32_t fixdamage;
			int32_t mastery;
			uint8_t attackcount;
			uint8_t mobcount;
			uint8_t bulletcount;
			int16_t bulletcost;
			int32_t hpcost;
			int32_t mpcost;
			float chance;
			float critical;
			float ignoredef;
			// 运行时 Skill.nx level 节点 `criticalDamage`（实测值 105/110/.../125，百分比）。
			// 0 = 该技能未配置；v2 公式把它叠加到暴击倍率上。
			float criticaldamage;
			float hrange;
			// WZ level/<lv>/time：ASSASSINATE 潜行秒数倍率上限（MinMS Stats.time）。
			int32_t time;
			Rectangle<int16_t> range;

			// 默认构造：overlay_stats/ stats 的 unordered_map::operator[] 需要可默认构造的 Stats。
			// 全字段零初始化，语义等同「该等级未配置」（get_stats 对缺失等级本就返回默认 object）。
			constexpr Stats() : damage(0.0f), matk(0), fixdamage(0), mastery(0), attackcount(0), mobcount(0), bulletcount(0), bulletcost(0), hpcost(0), mpcost(0), chance(0.0f), critical(0.0f), ignoredef(0.0f), criticaldamage(0.0f), hrange(0.0f), time(0), range() {}
			constexpr Stats(float damage, int32_t matk, int32_t fixdamage, int32_t mastery, uint8_t attackcount, uint8_t mobcount, uint8_t bulletcount, int16_t bulletcost, int32_t hpcost, int32_t mpcost, float chance, float critical, float ignoredef, float hrange, Rectangle<int16_t> range, float criticaldamage = 0.0f, int32_t time = 0) : damage(damage), matk(matk), fixdamage(fixdamage), mastery(mastery), attackcount(attackcount), mobcount(mobcount), bulletcount(bulletcount), bulletcost(bulletcost), hpcost(hpcost), mpcost(mpcost), chance(chance), critical(critical), ignoredef(ignoredef), criticaldamage(criticaldamage), hrange(hrange), time(time), range(range) {}
		};

		// Skill flags, unfortunately these just have to be hard-coded
		enum Flags
		{
			NONE = 0x0000,
			ATTACK = 0x0001,
			RANGED = 0x0002
		};

		// Icon types
		enum Icon
		{
			NORMAL,
			DISABLED,
			MOUSEOVER,
			NUM_ICONS
		};

		// 终极攻击链接：WZ skill/<攻击技>/finalAttack/<FA技能id>/<i>=武器类型。
		// 三端原生对齐（MinMS SkillData.h:128-135，Angel TryDoingMeleeAttack）：
		// 攻击命中后按链接表+武器类型+prop概率，延迟触发对应的 skillType3 被动攻击。
		struct FinalAttackLink
		{
			int32_t skill_id = 0;
			std::vector<int32_t> weapon_types;
		};
		const std::vector<FinalAttackLink>& get_final_attack_links() const;

		// Return whether the skill is passive
		bool is_passive() const;
		// Return whether the skill is an attack skill
		bool is_attack() const;
		// 是否终极攻击被动（WZ skillType==3）：延迟触发的追加攻击。
		bool is_final_attack() const;
		// Return whether this skill is invisible in the skill book UI
		bool is_invisible() const;
		// Return the default masterlevel
		int32_t get_masterlevel() const;
		// Return the required weapon
		Weapon::Type get_required_weapon() const;
		// Return the stats of one level
		// If there are no stats for that level, a default object is returned.
		// 技改覆盖：若该等级有服务端下发的覆盖字段，返回叠加后的 overlay 副本，否则回退 Skill.nx 原值。
		const Stats& get_stats(int32_t level) const;
		// WZ elemAttr 首字母（F/I/L/S/H/D；空=无属性）。四源对照 A1 供攻击链元素克制判定。
		const std::string& get_element() const;

		// 技改覆盖包入口：把 (字段下标 → 值) 叠加到指定等级的 overlay 副本；返回是否实际改动。
		// 字段下标见 SkillStatOverrideCache.h / 服务端 SkillFieldMeta.FIELDS。
		bool apply_overlay(int32_t level, const std::unordered_map<int32_t, int32_t>& fields);
		// 清空本技能全部 overlay，回到 Skill.nx 原值。
		void clear_overlay();

		// Return the name of the skill
		const std::string& get_name() const;
		// Return the description of the skill
		const std::string& get_desc() const;
		// Return the description of a level
		// If there is no description for this level, a warning message is returned.
		const std::string& get_level_desc(int32_t level) const;

		// Return one of the skill icons
		// Cannot fail if type is a valid enumeration
		const Texture& get_icon(Icon icon) const;

		// Return id and level of all required skills
		const std::unordered_map<int32_t, int32_t>& get_reqskills() const;

		// 覆写 Cache<SkillData>::get：见 SkillData.cpp 中 rooted 的说明。
		// Skill 根尚未装载时构造出的是空壳，空壳进缓存会被永久锁死。
		static const SkillData& get(std::int32_t id);

	private:
		// Allow the cache to use the constructor
		friend Cache<SkillData>;
		// Load a skill from the game files
		SkillData(int32_t id);

		// Get some hard-coded information
		int32_t flags_of(int32_t id) const;

		std::unordered_map<int32_t, Stats> stats;
		// 技改覆盖副本：level → 在 Skill.nx 原值上叠加服务端覆盖后的 Stats。
		// 用独立表而非改 stats，避免污染 Cache 里的原始 NX 数据（清覆盖可完整回退）。
		std::unordered_map<int32_t, Stats> overlay_stats;
		std::string element;
		Weapon::Type reqweapon;
		int32_t masterlevel;
		int32_t flags;
		int32_t skill_type = 0; // WZ skill/<id>/skillType（3=终极攻击被动）
		bool passive;
		bool invisible;
		std::vector<FinalAttackLink> final_attack_links;

		std::string name;
		std::string desc;
		std::unordered_map<int32_t, std::string> levels;
		std::unordered_map<int32_t, int32_t> reqskills;

		std::array<Texture, Icon::NUM_ICONS> icons;
		// 构造时 nl::nx::Skill 根是否已装载（只用于决定能否进缓存）
		bool rooted;
	};
}