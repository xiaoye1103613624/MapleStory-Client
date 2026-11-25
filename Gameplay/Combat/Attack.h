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

#include "../../Template/Rectangle.h"
#include "Damage.h"

#include <unordered_map>
#include <vector>
#include <string>

namespace ms
{
	struct Attack
	{
		enum Type
		{
			CLOSE,
			RANGED,
			MAGIC
		};

		enum DamageType
		{
			DMG_WEAPON,
			DMG_MAGIC,
			DMG_FIXED
		};

		Type type = Type::CLOSE;
		DamageType damagetype = DamageType::DMG_WEAPON;

		double mindamage = 1.0;
		double maxdamage = 1.0;
		float critical = 0.0f;
		// 官方暴击倍率区间：系数 = 1 + (20~50 + 面板暴伤%)/100，区间内随机。
		// 由 Player::get_attack / Skill 按角色暴伤属性填充；这里给官方无暴伤装备的默认值。
		float critical_min = 1.2f;
		float critical_max = 1.5f;
		// 无视防御比例 0..1；作用于怪物 wdef/mdef。
		float ignoredef = 0.0f;
		// BOSS 增伤比例，0.2 = +20%；仅对 boss 生效。
		float bossdamage = 0.0f;
		// 最终伤害比例，0.2 = +20%；各来源独立连乘后由 0x175 折算下发。
		float finaldamage = 0.0f;
		// 魔法基础伤害（MAD 钳到 1999 后），DMG_MAGIC 时替代武器 min/max。
		double magicdamage = 0.0;
		int32_t matk = 0;
		int32_t accuracy = 0;
		int32_t fixdamage = 0;
		int16_t playerlevel = 1;

		uint8_t hitcount = 0;
		uint8_t mobcount = 0;
		uint8_t speed = 0;
		uint8_t stance = 0;
		int32_t skill = 0;
		// 蓄力毫秒数（KeyDown 状态机）：仅 has_charge_field 技能发包时消费。
		int32_t charge = 0;
		int32_t bullet = 0;

		Point<int16_t> origin;
		Rectangle<int16_t> range;
		float hrange = 1.0f;
		// 技能元素属性（WZ elemAttr 首字母 F/I/L/S/H/D；空=无属性）。
		// 四源对照 A1：SkillData 已解析但攻击链此前未消费，补此字段供怪物弱点判定。
		std::string element;
		// 三端原生对齐（MinMS Attack.h）：三蜗牛等固定命中技能强制必中；
		// stepped_range=false 表示显式 lt/rb 命中区保持轴对齐、不走默认锥形。
		bool always_hit = false;
		bool stepped_range = true;
		bool toleft = false;
	};

	struct MobAttack
	{
		Attack::Type type = Attack::Type::CLOSE;
		int32_t watk = 0;
		int32_t matk = 0;
		int32_t mobid = 0;
		int32_t oid = 0;
		Point<int16_t> origin;
		bool valid = false;

		// Create a mob attack for touch damage
		MobAttack() : valid(false) {}
		MobAttack(int32_t watk, Point<int16_t> origin, int32_t mobid, int32_t oid) : type(Attack::Type::CLOSE), watk(watk), origin(origin), mobid(mobid), oid(oid), valid(true) {}

		explicit operator bool() const
		{
			return valid;
		}
	};

	struct MobAttackResult
	{
		int64_t damage;
		int32_t mobid;
		int32_t oid;
		uint8_t direction;

		MobAttackResult(const MobAttack& attack, int64_t damage, uint8_t direction) : damage(damage), direction(direction), mobid(attack.mobid), oid(attack.oid) {}
	};

	struct AttackResult
	{
		AttackResult() {}

		AttackResult(const Attack& attack)
		{
			type = attack.type;
			hitcount = attack.hitcount;
			skill = attack.skill;
			charge = attack.charge;
			speed = attack.speed;
			stance = attack.stance;
			bullet = attack.bullet;
			toleft = attack.toleft;
			origin = attack.origin;
		}

		Attack::Type type;
		int32_t attacker = 0;
		uint8_t mobcount = 0;
		uint8_t hitcount = 1;
		int32_t skill = 0;
		int32_t charge = 0;
		int32_t bullet = 0;
		uint8_t level = 0;
		uint8_t display = 0;
		uint8_t stance = 0;
		uint8_t speed = 0;
		bool toleft = false;
		Point<int16_t> origin;
		std::unordered_map<int32_t, damage::DamageLines> damagelines;
		int32_t first_oid;
		int32_t last_oid;
	};

	struct AttackUser
	{
		int32_t skilllevel;
		uint16_t level;
		bool secondweapon;
		bool flip;
		// 施法者位置（MinMS AttackUser.h:238，Angel 命中记录 user_x+distance
		// @0x95251c 需要）：回马等把怪聚到攻击者身侧的技能依赖此锚点。
		Point<int16_t> position;
	};
}