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
#include "SkillData.h"

#include "../Character/SkillId.h"
#include "../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	SkillData::SkillData(int32_t id)
	{
		/// Locate sources
		std::string strid = string_format::extend_id(id, 7);
		std::string jobid = string_format::skill_namespace(id);
		nl::node src = nl::nx::Skill[jobid + ".img"]["skill"][strid];
		nl::node strsrc = nl::nx::String["Skill.img"][strid];

		/// Load icons
		icons = { src["icon"], src["iconDisabled"], src["iconMouseOver"] };

		/// Load strings
		name = strsrc["name"].get_string();
		desc = strsrc["desc"].get_string();

		for (int32_t level = 1; nl::node sub = strsrc["h" + std::to_string(level)]; level++)
			levels.emplace(level, sub);

		/// Load stats
		nl::node levelsrc = src["level"];

		for (auto sub : levelsrc)
		{
			float damage = (float)sub["damage"] / 100;
			int32_t matk = sub["mad"];
			int32_t fixdamage = sub["fixdamage"];
			int32_t mastery = sub["mastery"];
			uint8_t attackcount = (uint8_t)sub["attackCount"].get_integer(1);
			uint8_t mobcount = (uint8_t)sub["mobCount"].get_integer(1);
			uint8_t bulletcount = (uint8_t)sub["bulletCount"].get_integer(1);
			int16_t bulletcost = (int16_t)sub["bulletConsume"].get_integer(bulletcount);
			int32_t hpcost = sub["hpCon"];
			int32_t mpcost = sub["mpCon"];
			float chance = (float)sub["prop"].get_real(100.0) / 100;
			float critical = 0.0f;
			float ignoredef = 0.0f;
			float hrange = (float)sub["range"].get_real(100.0) / 100;
			Rectangle<int16_t> range = sub;
			int32_t level = string_conversion::or_default<int32_t>(sub.name(), -1);

			stats.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(level),
				std::forward_as_tuple(damage, matk, fixdamage, mastery, attackcount, mobcount, bulletcount, bulletcost, hpcost, mpcost, chance, critical, ignoredef, hrange, range)
			);
		}

		element = src["elemAttr"].get_string();

		if (jobid == "900" || jobid == "910")
			reqweapon = Weapon::Type::NONE;
		else
			reqweapon = Weapon::by_value(100 + (int32_t)src["weapon"]);

		masterlevel = static_cast<int32_t>(stats.size());
		passive = (id % 10000) / 1000 == 0;

		// 攻击位优先按节点属性推断：带 hit（受击帧）或 ball（弹道）的技能一定造成伤害，
		// 这与 s9 client/SkillFactory.java 判定 buff 的取反条件 `effect != null
		// && hit == null && ball == null` 完全对应。原来的硬编码表只覆盖到冒险家战士/法师
		// 的一部分，弓箭手（21x/22x/31x/32x）、海盗（51x/52x）、骑士团（1xxx）与战神（2xxx）
		// 全部落到 NONE —— is_attack() 为假后走 UseSkillPacket 分支，攻击动作与伤害整条
		// 链路都不执行，表现为这些职业的技能「按了没反应」。
		// 表仍保留，兜住 Slow/Seal 这类没有 hit/ball 却要靠攻击包下发减益的例外技能。
		int32_t inferred = NONE;

		if (src["hit"].size() > 0 || src["ball"].size() > 0)
			inferred = ATTACK;

		flags = inferred | flags_of(id);
		invisible = src["invisible"].get_bool();

		/// Load required skills
		nl::node reqsrc = src["req"];

		for (auto sub : reqsrc)
		{
			int32_t skillid = string_conversion::or_default<int32_t>(sub.name(), -1);
			int32_t reqlv = sub.get_integer();

			reqskills.emplace(skillid, reqlv);
		}
	}

	int32_t SkillData::flags_of(int32_t id) const
	{
		static const std::unordered_map<int32_t, int32_t> skill_flags =
		{
			// Beginner
			{ SkillId::THREE_SNAILS, ATTACK },
			// Warrior
			{ SkillId::POWER_STRIKE, ATTACK },
			{ SkillId::SLASH_BLAST, ATTACK },
			// Fighter
			// Page
			// Crusader
			{ SkillId::SWORD_PANIC, ATTACK },
			{ SkillId::AXE_PANIC, ATTACK },
			{ SkillId::SWORD_COMA, ATTACK },
			{ SkillId::AXE_COMA, ATTACK },
			// Hero
			{ SkillId::RUSH_HERO, ATTACK },
			{ SkillId::BRANDISH, ATTACK },
			// Page
			// White Knight
			{ SkillId::CHARGE, ATTACK },
			// Paladin
			{ SkillId::RUSH_PALADIN, ATTACK },
			{ SkillId::BLAST, ATTACK },
			{ SkillId::HEAVENS_HAMMER, ATTACK },
			// Spearman
			// Dragon Knight
			{ SkillId::DRAGON_BUSTER, ATTACK },
			{ SkillId::DRAGON_FURY, ATTACK },
			{ SkillId::PA_BUSTER, ATTACK },
			{ SkillId::PA_FURY, ATTACK },
			{ SkillId::SACRIFICE, ATTACK },
			{ SkillId::DRAGONS_ROAR, ATTACK },
			// Dark Knight
			{ SkillId::RUSH_DK, ATTACK },
			// Mage
			{ SkillId::ENERGY_BOLT, ATTACK | RANGED },
			{ SkillId::MAGIC_CLAW, ATTACK | RANGED },
			// F/P Mage
			{ SkillId::SLOW_FP, ATTACK },
			{ SkillId::FIRE_ARROW, ATTACK | RANGED },
			{ SkillId::POISON_BREATH, ATTACK | RANGED },
			// F/P ArchMage
			{ SkillId::EXPLOSION, ATTACK },
			{ SkillId::POISON_MIST, ATTACK },
			{ SkillId::SEAL_FP, ATTACK },
			{ SkillId::ELEMENT_COMPOSITION_FP, ATTACK | RANGED },
			// TODO: Blank?
			{ SkillId::FIRE_DEMON, ATTACK },
			{ SkillId::PARALYZE, ATTACK | RANGED },
			{ SkillId::METEOR_SHOWER, ATTACK }
		};

		auto iter = skill_flags.find(id);

		if (iter == skill_flags.end())
			return NONE;

		return iter->second;
	}

	bool SkillData::is_passive() const
	{
		return passive;
	}

	bool SkillData::is_attack() const
	{
		return !passive && (flags & ATTACK);
	}

	bool SkillData::is_invisible() const
	{
		return invisible;
	}

	int32_t SkillData::get_masterlevel() const
	{
		return masterlevel;
	}

	Weapon::Type SkillData::get_required_weapon() const
	{
		return reqweapon;
	}

	const SkillData::Stats& SkillData::get_stats(int32_t level) const
	{
		auto iter = stats.find(level);

		if (iter == stats.end())
		{
			static constexpr Stats null_stats = Stats(0.0f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f, Rectangle<int16_t>());

			return null_stats;
		}

		return iter->second;
	}

	const std::string& SkillData::get_name() const
	{
		return name;
	}

	const std::string& SkillData::get_desc() const
	{
		return desc;
	}

	const std::string& SkillData::get_level_desc(int32_t level) const
	{
		auto iter = levels.find(level);

		if (iter == levels.end())
		{
			static const std::string null_level = "Missing level description.";

			return null_level;
		}
		else
		{
			return iter->second;
		}
	}

	const Texture& SkillData::get_icon(Icon icon) const
	{
		return icons[icon];
	}

	const std::unordered_map<int32_t, int32_t>& SkillData::get_reqskills() const
	{
		return reqskills;
	}
}