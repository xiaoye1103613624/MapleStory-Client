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

#include <utility>

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
		// rooted：此刻 nl::nx::Skill 根有没有绑上。选角阶段与登录界面是并行
		// 预载的（ResourceService::prewarm_game_roots → ImgSource::ensure_char_select
		// 只装外观相关根），Skill 要到进图后的 ensure_map_roots / ensure_remaining
		// 才绑。登录封包里的 SkillBook::collect_passives 早于这一刻，构造出来的
		// 就是空壳。
		rooted = nl::nx::Skill.size() > 0;
		nl::node src = nl::nx::Skill[jobid + ".img"]["skill"][strid];
		nl::node strsrc = nl::nx::String["Skill.img"][strid];

		/// Load icons
		icons = { src["icon"], src["iconDisabled"], src["iconMouseOver"] };

		// 本包技能图标的 origin 恒为 (0,32)（WZ 的「底边锚点」写法），真机抓帧实测
		// 过：行内绘制点的 origin=0,32。而 UISkillBook 的行内
		// 偏移是按原生像素量出来的「图标左上 = 绘制点」写的
		// （CLASSIC_SKILL_OFFSET(7,99) + SKILL_META_OFFSET(2,2)，原生图标左上 =
		// 行背景左上 + (2,2)，pitch 40）。不补偿就把每行图标整体上抬 32px ——
		// 真机表现为「第 i 行的图标落在第 i-1 行的格子里、最后一行空着」。
		// 上游是在 Icon 构造里统一 shift(0,32)，但那会把 origin 只有几像素的
		// 道具图标整体下推（见 Icon.cpp 注释），所以补偿收在技能这一族。
		for (Texture& icon : icons)
			icon.shift(icon.get_origin());

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
			// 运行时 Skill.nx 只存在 `criticalDamage`（百分比，如 105 = +5% 暴伤），
			// 不存在 ignoreDef/bossDamage 节点；缺失时这两个值保持 0（R6：精简数据需兜底）。
			float criticaldamage = (float)sub["criticalDamage"].get_integer(0);
			ignoredef = (float)sub["ignoreDef"].get_integer(0) / 100;
			float hrange = (float)sub["range"].get_real(100.0) / 100;
			Rectangle<int16_t> range = sub;
			int32_t level = string_conversion::or_default<int32_t>(sub.name(), -1);

			stats.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(level),
				std::forward_as_tuple(damage, matk, fixdamage, mastery, attackcount, mobcount, bulletcount, bulletcost, hpcost, mpcost, chance, critical, ignoredef, hrange, range, criticaldamage)
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

		// 三端原生对齐（MinMS SkillData.cpp:422-455，live WZ 实证）：
		// skill/<攻击技>/finalAttack/<FA技能id>/<i>=武器类型（30/40/31…），
		// 只有装备武器 Maple 类型命中其一才延迟触发该终极攻击。
		for (nl::node fa : src["finalAttack"])
		{
			const int32_t fa_id = string_conversion::or_zero<int32_t>(fa.name());
			if (fa_id <= 0)
				continue;
			FinalAttackLink link;
			link.skill_id = fa_id;
			for (nl::node wt : fa)
			{
				const int32_t weapon_type = static_cast<int32_t>(wt.get_integer(0));
				if (weapon_type > 0)
					link.weapon_types.push_back(weapon_type);
			}
			final_attack_links.push_back(std::move(link));
		}

		// skillType==3 + hit 的被动是终极攻击：虽为 passive，仍按攻击包下发。
		skill_type = static_cast<int32_t>(src["skillType"].get_integer(0));
		if (passive && skill_type == 3 && src["hit"].size() > 0)
		{
			for (const auto& entry : stats)
			{
				if (entry.second.damage > 0.0f)
				{
					inferred |= ATTACK; // 合并进下面统一的 flags 赋值，避免读未初始化值
					break;
				}
			}
		}

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

	const SkillData& SkillData::get(int32_t id)
	{
		auto iter = cache.find(id);

		if (iter == cache.end())
		{
			SkillData built(id);

			// 根还没装载 → 这次构造的是空壳。空壳进缓存就永久锁死：真机上
			// 新手 0000012「精灵的祝福」由登录封包的 collect_passives 首次触发，
			// 那一刻 Skill 根未绑，于是技能窗里它永远没有图标、没有等级表。
			// 这里不写缓存，返回同一个「未装载」实例，等根就绪后的访问重新构造。
			if (!built.rooted)
			{
				static const SkillData unloaded(0);

				return unloaded;
			}

			iter = cache.emplace(id, std::move(built)).first;
		}

		return iter->second;
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
		// 保持既有语义（!passive && ATTACK）：当前端没有 MinMS 那套
		// crash/seal/summon 的 flags&=~ATTACK 排除，贸然放开 passive 会回归。
		// 终极攻击被动由 is_final_attack() 单独识别并在 Combat 里走攻击路径。
		return !passive && (flags & ATTACK);
	}

	bool SkillData::is_final_attack() const
	{
		return passive && skill_type == 3 && (flags & ATTACK);
	}

	const std::vector<SkillData::FinalAttackLink>& SkillData::get_final_attack_links() const
	{
		return final_attack_links;
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
		// 技改覆盖优先：有 overlay 副本则返回它，否则回退 Skill.nx 原值。
		auto ov = overlay_stats.find(level);
		if (ov != overlay_stats.end())
			return ov->second;

		auto iter = stats.find(level);

		if (iter == stats.end())
		{
			static constexpr Stats null_stats = Stats(0.0f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f, Rectangle<int16_t>());

			return null_stats;
		}

		return iter->second;
	}

	const std::string& SkillData::get_element() const
	{
		return element;
	}

	bool SkillData::apply_overlay(int32_t level, const std::unordered_map<int32_t, int32_t>& fields)
	{
		auto base = stats.find(level);
		if (base == stats.end() || fields.empty())
			return false;

		// 以当前有效值（可能已有 overlay）为底再叠加，保证多次下发同等级是累进覆盖。
		Stats s = get_stats(level);
		for (const auto& kv : fields)
		{
			switch (kv.first)
			{
			case 0:  s.damage = static_cast<float>(kv.second) / 100.0f; break; // damage WZ 是百分整数
			case 1:  s.attackcount = static_cast<uint8_t>(kv.second); break;
			case 2:  s.mobcount = static_cast<uint8_t>(kv.second); break;
			case 3:  s.mpcost = kv.second; break;   // mpCon
			case 4:  s.hpcost = kv.second; break;   // hpCon
			case 9:  s.matk = kv.second; break;     // mad
			case 11: s.fixdamage = kv.second; break;
			case 12: s.bulletcount = static_cast<uint8_t>(kv.second); break;
			case 13: s.bulletcost = static_cast<int16_t>(kv.second); break;
			default: break; // 5 time / 6 cooltime / 7 x / 8 y / 10 watk：客户端 Stats 无对应字段，忽略
			}
		}
		overlay_stats[level] = s;
		return true;
	}

	void SkillData::clear_overlay()
	{
		overlay_stats.clear();
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