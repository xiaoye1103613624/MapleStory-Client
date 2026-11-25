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
#include "Skill.h"

#include "../../Character/SkillId.h"
#include "../../Data/SkillData.h"
#include "../../Util/Misc.h"
#include "../../Util/Randomizer.h"
#include "../../Configuration.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	Skill::Skill(int32_t id) : skillid(id)
	{
		const SkillData& data = SkillData::get(skillid);

		std::string strid;

		if (skillid < 10000000)
			strid = string_format::extend_id(skillid, 7);
		else
			strid = std::to_string(skillid);

		nl::node src = nl::nx::Skill[string_format::skill_namespace(skillid) + ".img"]["skill"][strid];

		projectile = true;
		overregular = false;

		sound = std::make_unique<SingleSkillSound>(strid);

		bool byleveleffect = src["CharLevel"]["10"]["effect"].size() > 0;
		bool multieffect = src["effect0"].size() > 0;

		if (byleveleffect)
		{
			useeffect = std::make_unique<ByLevelUseEffect>(src);
		}
		else if (multieffect)
		{
			useeffect = std::make_unique<MultiUseEffect>(src);
		}
		else
		{
			bool isanimation = src["effect"]["0"].data_type() == nl::node::type::bitmap;
			bool haseffect1 = src["effect"]["1"].size() > 0;

			if (isanimation)
			{
				useeffect = std::make_unique<SingleUseEffect>(src);
			}
			else if (haseffect1)
			{
				useeffect = std::make_unique<TwoHandedUseEffect>(src);
			}
			else
			{
				switch (skillid)
				{
				case SkillId::IRON_BODY:
				case SkillId::MAGIC_ARMOR:
					useeffect = std::make_unique<IronBodyUseEffect>();
					break;
				default:
					useeffect = std::make_unique<NoUseEffect>();
					break;
				}
			}
		}

		bool bylevelhit = src["CharLevel"]["10"]["hit"].size() > 0;
		bool byskilllevelhit = src["level"]["1"]["hit"].size() > 0;
		bool hashit0 = src["hit"]["0"].size() > 0;
		bool hashit1 = src["hit"]["1"].size() > 0;

		if (bylevelhit)
		{
			if (hashit0 && hashit1)
				hiteffect = std::make_unique<ByLevelTwoHHitEffect>(src);
			else
				hiteffect = std::make_unique<ByLevelHitEffect>(src);
		}
		else if (byskilllevelhit)
		{
			hiteffect = std::make_unique<BySkillLevelHitEffect>(src);
		}
		else if (hashit0 && hashit1)
		{
			hiteffect = std::make_unique<TwoHandedHitEffect>(src);
		}
		else if (hashit0)
		{
			hiteffect = std::make_unique<SingleHitEffect>(src);
		}
		else
		{
			hiteffect = std::make_unique<NoHitEffect>();
		}

		bool hasaction0 = src["action"]["0"].data_type() == nl::node::type::string;
		bool hasaction1 = src["action"]["1"].data_type() == nl::node::type::string;

		if (hasaction0 && hasaction1)
		{
			action = std::make_unique<TwoHandedAction>(src);
		}
		else if (hasaction0)
		{
			action = std::make_unique<SingleAction>(src);
		}
		else if (data.is_final_attack())
		{
			// 三端原生对齐（MinMS Skill.cpp:331-336）：终极攻击被动播 OF 动作池，
			// 不用常规 O1/O2/O3。
			action = std::make_unique<FinalAttackAction>();
			overregular = true;
		}
		else if (data.is_attack())
		{
			bool bylevel = src["level"]["1"]["action"].data_type() == nl::node::type::string;

			if (bylevel)
			{
				action = std::make_unique<ByLevelAction>(src, skillid);
			}
			else
			{
				action = std::make_unique<RegularAction>();
				overregular = true;
			}
		}
		else
		{
			action = std::make_unique<NoAction>();
		}

		bool hasball = src["ball"].size() > 0;
		bool bylevelball = src["level"]["1"]["ball"].size() > 0;

		if (bylevelball)
		{
			bullet = std::make_unique<BySkillLevelBullet>(src, skillid);
		}
		else if (hasball)
		{
			bullet = std::make_unique<SingleBullet>(src);
		}
		else
		{
			bullet = std::make_unique<RegularBullet>();
			projectile = false;
		}

		// 2026-11-23 校准：瞬移 / 二段跳的施放动画原生直接挂在 Effect/BasicEff.img，
		// Skill.img 的 <skill>/effect 节点里没有对应条目 —— 上面 else 分支必然落到
		// NoUseEffect，这正是用户报「没有动效」的根因。这里在 useeffect 已确定之后
		// 统一覆盖（不改动前面 effect 选择逻辑，零回归面）：
		//   TryRegisterTeleport@0x957b74 播 BasicEff.img/Teleport
		//   DoActiveSkill_BoundJump@0x96be38 播 BasicEff.img/Flying
		// 走 BasicEffUseEffect -> show_attack_effect，尊重 SysOptSkillEffects 画面开关。
		if (SkillId::is_teleport(skillid))
		{
			useeffect = std::make_unique<BasicEffUseEffect>("Teleport");
			LOG(LOG_DEBUG, "[Skill] teleport useeffect=BasicEff/Teleport skillid=" << skillid);
		}
		else if (SkillId::is_flash_jump(skillid))
		{
			useeffect = std::make_unique<BasicEffUseEffect>("Flying");
			LOG(LOG_DEBUG, "[Skill] flashjump useeffect=BasicEff/Flying skillid=" << skillid);
		}

		// 第二轮补齐：orange-wz 跨 img 实证 live Skill/*.img 含 6 类额外特效节点
		// （prepare/keydown/keydownend/screen/special/tile，多为 CANVAS 帧动画）。
		// 首轮只探 210.img 误判为"资源缺源"，此处按节点存在逐个挂接，原地逐帧播放。
		// receiver/finish 全库无源，不接；special/tile 的多段落点物理本端 EffectLayer
		// 不支持，降级为原地动画（诚实的可见近似，真机观感待验）。
		{
			auto composite = std::make_unique<CompositeUseEffect>();
			static const char* extra_nodes[] = {
				"prepare", "keydown", "keydownend", "screen", "special", "tile", "finish"
			};
			int added = 0;
			for (const char* node_name : extra_nodes)
			{
				nl::node extra = src[node_name];
				if (!extra || extra.size() == 0)
					continue;
				// tile/sub 节点是 LIST（含 0/1/2 子帧），其余取第 0 帧动画。
				// z 由 Effect(src) 内部读 anim["z"]（默认 0）。
				nl::node anim = extra["0"].size() > 0 ? extra["0"] : extra;
				composite->add(std::make_unique<ExtraNodeUseEffect>(anim));
				++added;
			}
			if (added > 0)
			{
				extrauseeffect = std::move(composite);
				LOG(LOG_DEBUG, "[Skill] extra vfx nodes=" << added << " skillid=" << skillid);
			}
		}
	}

	void Skill::apply_useeffects(Char& user) const
	{
		useeffect->apply(user);
		if (extrauseeffect)
			extrauseeffect->apply(user);

		sound->play_use();
	}

	void Skill::apply_actions(Char& user, Attack::Type type) const
	{
		// 三端原生对齐（MinMS Skill.cpp:491-525，Angel SendSkillUseRequest 指定action）：
		// 爬梯/绳施法时，对「WZ 显式指定 action」的技能改播 ladder2/rope2 帧；
		// RegularAction（无 WZ action）不重定向。
		if (user.is_climbing() && action->is_appointed())
		{
			user.attack(user.get_char_state() == Char::LADDER ? "ladder2" : "rope2");
			return;
		}

		int32_t level = user.get_skilllevel(skillid);
		const SkillData& data = SkillData::get(skillid);
		// 双飞斩/三连环光击破 body 必须匹配飞镖常规投掷(SWINGO1/O2 via RANGED)，
		// 不能被陈旧 wire type 复活成 magic1/2/3 施法帧。
		if (skillid == SkillId::LUCKY_SEVEN || skillid == SkillId::TRIPLE_THROW)
			type = Attack::RANGED;
		else if (skillid == SkillId::THREE_SNAILS)
			type = Attack::MAGIC; // 三蜗牛走魔法攻击
		else if (data.get_stats(level).matk || data.get_stats(1).matk)
			type = Attack::MAGIC;

		action->apply(user, type);
	}

	void Skill::apply_stats(const Char& user, Attack& attack) const
	{
		attack.skill = skillid;

		int32_t level = user.get_skilllevel(skillid);
		const SkillData::Stats stats = SkillData::get(skillid).get_stats(level);

		if (stats.fixdamage)
		{
			attack.fixdamage = stats.fixdamage;
			attack.damagetype = Attack::DMG_FIXED;
		}
		else if (stats.matk)
		{
			attack.matk += stats.matk;
			attack.damagetype = Attack::DMG_MAGIC;

			// 魔法技能用独立魔法基础值（MAD/INT/LUK），而不是武器 min/max。
			if (attack.magicdamage > 0.0)
			{
				double multiplier = stats.damage > 0.0f ? stats.damage : 1.0f;
				double base = attack.magicdamage * multiplier + stats.matk;
				attack.mindamage = base;
				attack.maxdamage = base;
			}
		}
		else
		{
			attack.mindamage *= stats.damage;
			attack.maxdamage *= stats.damage;
			attack.damagetype = Attack::DMG_WEAPON;
		}

		attack.critical += stats.critical;

		attack.ignoredef += stats.ignoredef;

		// 三端原生对齐（MinMS Skill.cpp:638-641，Angel CalcDamage::PDamage 对
		// SACRIFICE hard-skip PDD）：WZ 无 ignoredef 键，强制完全无视物理防御。
		if (skillid == SkillId::SACRIFICE)
			attack.ignoredef = 1.0f;

		// 四源对照 A1：技能元素属性透传到攻击链，供 Mob 弱点/免疫判定。
		attack.element = SkillData::get(skillid).get_element();

		// criticalDamage 是百分比（105 = +5% 暴伤），叠加到官方暴击倍率区间（基准 1.35）。
		if (stats.criticaldamage > 100.0f)
		{
			float bonus = (stats.criticaldamage - 100.0f) / 100.0f;
			attack.critical_min += bonus;
			attack.critical_max += bonus;
		}
		attack.mobcount = stats.mobcount;
		attack.hrange = stats.hrange;

		// 三端原生对齐（MinMS Skill.cpp:660-665）：三蜗牛固定伤害+必中，
		// 走魔法攻击而非弓箭 RANGED。
		if (skillid == SkillId::THREE_SNAILS)
			attack.always_hit = true;

		switch (attack.type)
		{
		case Attack::RANGED:
			attack.hitcount = stats.bulletcount;
			break;
		default:
			attack.hitcount = stats.attackcount;
			break;
		}

		if (!stats.range.empty())
		{
			// 三端原生对齐（MinMS Skill.cpp:680-686）：WZ 显式 lt/rb（治疗/呐喊等）
			// 保持轴对齐命中区，不叠加默认投射物的锥形 stepped 标记。
			attack.range = stats.range;
			attack.stepped_range = false;
		}

		if (projectile && !attack.bullet)
		{
			switch (skillid)
			{
			case SkillId::THREE_SNAILS:
				switch (level)
				{
				case 1:
					attack.bullet = 4000019;
					break;
				case 2:
					attack.bullet = 4000000;
					break;
				case 3:
					attack.bullet = 4000016;
					break;
				}
				break;
			default:
				attack.bullet = skillid;
				break;
			}
		}

		if (overregular)
		{
			attack.stance = user.get_look().get_stance();

			if (attack.type == Attack::CLOSE && !projectile)
				attack.range = user.get_afterimage().get_range();
		}
	}

	void Skill::apply_hiteffects(const AttackUser& user, Mob& target) const
	{
		hiteffect->apply(user, target);

		sound->play_hit();

		// 三端原生对齐（MinMS Skill.cpp:735-768，Angel TryDoingMeleeAttack@0x952453）：
		// 忍者冲击 4121008：prop 概率把目标击退，1-10级 100px、11级+ 200px。
		Randomizer randomizer;
		if (skillid == SkillId::NINJA_STORM)
		{
			const SkillData::Stats& st = SkillData::get(skillid).get_stats(user.skilllevel);
			if (st.chance > 0.0f && randomizer.below(st.chance))
				target.push_back(user.skilllevel >= 11 ? 200 : 100);
		}
		// 回马 5101002：把命中怪聚到「攻击者 + 15+rand(0..19)」同一锚点。
		else if (skillid == SkillId::BACK_SPIN_BLOW)
		{
			target.gather(user.position, 15 + static_cast<int32_t>(randomizer.next_int(20)));
		}
	}

	Animation Skill::get_bullet(const Char& user, int32_t bulletid) const
	{
		return bullet->get(user, bulletid);
	}

	uint16_t Skill::get_bullet_rotate_period(const Char& user) const
	{
		// 三端原生对齐（MinMS Skill.cpp:776-779）：委托给弹道资源读 rotatePeriod。
		return bullet ? bullet->get_rotate_period(user) : 0;
	}

	bool Skill::is_attack() const
	{
		return SkillData::get(skillid).is_attack();
	}

	bool Skill::is_final_attack() const
	{
		return SkillData::get(skillid).is_final_attack();
	}

	bool Skill::is_skill() const
	{
		return true;
	}

	int32_t Skill::get_id() const
	{
		return skillid;
	}

	SpecialMove::ForbidReason Skill::can_use(int32_t level, Weapon::Type weapon, const Job& job, uint16_t hp, uint16_t mp, uint16_t bullets) const
	{
		if (level <= 0)
			return FBR_OTHER;

		// 脚本/道具习得技能在 WZ 未装载或 level 表为空时 masterlevel=0，
		// 旧判 level>0 会误拒；仅在 WZ 有有效上限时才钳等级。
		const int32_t wz_master = SkillData::get(skillid).get_masterlevel();
		if (wz_master > 0 && level > wz_master)
			return FBR_OTHER;

		// 技能书 level>0（含 help/脚本/GM 授予）即以持有为准；不再用 job.can_use
		// 二次拦截——跨职学来的瞬移/二段跳否则永远 FBR_OTHER。
		(void)job;

		const SkillData::Stats stats = SkillData::get(skillid).get_stats(level);

		if (hp <= stats.hpcost)
			return FBR_HPCOST;

		if (mp < stats.mpcost)
			return FBR_MPCOST;

		const bool is_atk = SkillData::get(skillid).is_attack();
		Weapon::Type reqweapon = SkillData::get(skillid).get_required_weapon();

		// 攻击技仍校验武器；瞬移/二段跳等非攻击技允许脚本跨职持有后直接用。
		if (is_atk && weapon != reqweapon && reqweapon != Weapon::NONE)
			return FBR_WEAPONTYPE;

		// 非攻击位移技不耗飞镖；旧逻辑对 CLAW 一律查子弹会误拒二段跳。
		if (!is_atk || stats.bulletcost <= 0)
			return FBR_NONE;

		switch (weapon)
		{
		case Weapon::BOW:
		case Weapon::CROSSBOW:
		case Weapon::CLAW:
		case Weapon::GUN:
			return (bullets >= stats.bulletcost) ? FBR_NONE : FBR_BULLETCOST;
		default:
			return FBR_NONE;
		}
	}
}