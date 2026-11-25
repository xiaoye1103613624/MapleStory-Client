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
#include "../../Character/Player.h"
#include "../../Data/SkillData.h"
#include "../../Util/Misc.h"
#include "../../Util/Randomizer.h"
#include "../../Configuration.h"
#include <algorithm>

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

	// Tier-0 被动增幅：WZ req 指向 base 技能，被动自身从不被 cast。
	// 三端原生对齐 MinMS Skill.cpp:541-549，s9 以客户端 damage 权威结算（不吃双算）。
	static const SkillData::Stats* passive_upgrade(const Char& user, int32_t passive_id)
	{
		const int32_t lv = user.get_skilllevel(passive_id);
		if (lv <= 0)
			return nullptr;
		return &SkillData::get(passive_id).get_stats(lv);
	}

	// 属性蓄力码（MinMS Skill.cpp:50-69 / Angel sub_7908E7）：火2 冰1 雷3 圣5。
	static int32_t charge_element_code(int32_t charge_skill)
	{
		switch (charge_skill)
		{
			case SkillId::SWORD_FIRE_CHARGE: case SkillId::BW_FIRE_CHARGE: return 2;
			case SkillId::SWORD_ICE_CHARGE: case SkillId::BW_ICE_CHARGE: return 1;
			case SkillId::SWORD_LIT_CHARGE: case SkillId::BW_LIT_CHARGE: return 3;
			case SkillId::SWORD_HOLY_CHARGE: case SkillId::BW_HOLY_CHARGE: return 5;
			default: return 0;
		}
	}

	static char element_letter(int32_t code)
	{
		switch (code)
		{
			case 1: return 'I';
			case 2: return 'F';
			case 3: return 'L';
			case 4: return 'S';
			case 5: return 'H';
			case 6: return 'D';
			default: return 0;
		}
	}

	void Skill::apply_stats(const Char& user, Attack& attack) const
	{
		int32_t level = user.get_skilllevel(skillid);
		const SkillData::Stats stats = SkillData::get(skillid).get_stats(level);

		// 被动增幅族（MinMS Skill.cpp:551-575）：命中技能被对应被动升级时，
		// 用被动等级的 damage% 替换基础 ratio；BULLSEYE 还要把攻击包 skillid
		// 报成被动自身，否则 s9 damage check 390% vs 148% 超 1.5x 被判失败。
		const SkillData::Stats* burst = nullptr;
		bool report_upgrade = false;
		if (skillid == SkillId::PIRATE_DOUBLE_SHOT)
			burst = passive_upgrade(user, SkillId::BURST_FIRE);
		else if (skillid == SkillId::CHARGE)
			burst = passive_upgrade(user, SkillId::ADVANCED_CHARGE);
		else if (skillid == SkillId::HOMING_BEACON_SKILL)
		{
			burst = passive_upgrade(user, SkillId::BULLSEYE);
			report_upgrade = burst != nullptr;
		}
		attack.skill = report_upgrade ? SkillId::BULLSEYE : skillid;

		// 元素属性透传（A1）；CHARGE 无 WZ elemAttr 时按当前属性蓄力回退。
		attack.element = SkillData::get(skillid).get_element();
		if (attack.element.empty() && skillid == SkillId::CHARGE)
		{
			const Player* player = dynamic_cast<const Player*>(&user);
			const char letter = element_letter(player
				? charge_element_code(player->get_buff_skillid(Buffstat::Id::WK_CHARGE)) : 0);
			if (letter != 0)
				attack.element = std::string(1, letter);
		}

		if (stats.fixdamage)
		{
			attack.fixdamage = stats.fixdamage;
			attack.mindamage = static_cast<double>(stats.fixdamage);
			attack.maxdamage = static_cast<double>(stats.fixdamage);
			attack.damagetype = Attack::DMG_FIXED;
		}
		else if (stats.matk)
		{
			// pre-BB 魔法公式（MinMS CharStats.cpp:797-824 / Angel MDamage@0x791617）：
			// min/max 用 MAD² 二次项 + skill mastery 下限，非旧的线性式。
			const auto& cs = static_cast<const Player&>(user).get_stats();
			const float skill_mastery = std::max(cs.get_mastery(), 0.1f + static_cast<float>(stats.mastery) * 0.05f);
			attack.matk += stats.matk;
			attack.mindamage = cs.get_magic_mindamage(stats.matk, skill_mastery);
			attack.maxdamage = cs.get_magic_maxdamage(stats.matk);
			attack.damagetype = Attack::DMG_MAGIC;
			attack.type = Attack::MAGIC;
		}
		else
		{
			float ratio = burst ? burst->damage : stats.damage;
			// ELEMENTAL_BOOST 对烈焰/寒冰喷射是加算（Angel PDamage 0x79006f）。
			if (skillid == SkillId::FLAME_THROWER || skillid == SkillId::ICE_SPLITTER)
				if (const SkillData::Stats* boost = passive_upgrade(user, SkillId::ELEMENTAL_BOOST))
					ratio += boost->damage;
			attack.mindamage *= ratio;
			attack.maxdamage *= ratio;
			attack.damagetype = Attack::DMG_WEAPON;
		}

		attack.critical += stats.critical;
		attack.ignoredef += stats.ignoredef;

		// SACRIFICE 强制无视防御（Angel PDamage hard-skip PDD）。
		if (skillid == SkillId::SACRIFICE)
			attack.ignoredef = 1.0f;

		// ASSASSINATE 潜行秒数倍率：min(stealth_seconds, WZ time)/3+1（整数除法，
		// 满 3 秒才跳档；Angel PDamage 0x79028F）。
		if (skillid == SkillId::ASSASSINATE)
		{
			const int32_t seconds = std::min(
				static_cast<const Player&>(user).stealth_seconds(), stats.time);
			const double factor = static_cast<double>(seconds / 3 + 1);
			attack.mindamage *= factor;
			attack.maxdamage *= factor;
		}

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