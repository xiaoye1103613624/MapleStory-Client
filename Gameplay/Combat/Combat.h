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

#include "RegularAttack.h"
#include "Damage.h"

#include "../MapleMap/MapChars.h"
#include "../MapleMap/MapMobs.h"
#include "../MapleMap/MapReactors.h"

#include "../../Character/Player.h"
#include "../../Template/TimedQueue.h"

namespace ms
{
	class Combat
	{
	public:
		Combat(Player& player, MapChars& chars, MapMobs& mobs, MapReactors& reactors);

		// Draw bullets, damage numbers etc.
		void draw(double viewx, double viewy, float alpha) const;
		// Poll attacks, damage effects, etc.
		void update();

		// Make the player use a special move
		void use_move(int32_t move_id);
		// Angel CMacroSysMan::Update 语义：宏的三个技能入队，每个等前一个
		// 真正可攻击（动画硬直结束）后再放，间隔 MACRO_GAP 帧防同帧连发。
		void use_macro(int32_t skill1, int32_t skill2, int32_t skill3);

		// Add an attack to the attack queue
		void push_attack(const AttackResult& attack);
		// Show a buff effect
		void show_buff(int32_t cid, int32_t skillid, int8_t level);
		// Show a buff effect
		void show_player_buff(int32_t skillid);
		// Show a damage number from a server-broadcast mob damage (other players / field)
		void show_mob_damage(int32_t oid, int64_t damage, int32_t skin = 0);

	private:
		struct DamageEffect
		{
			AttackUser user;
			DamageNumber number;
			int32_t damage;
			bool toleft;
			int32_t target_oid;
			int32_t move_id;
		};

		struct BulletEffect
		{
			DamageEffect damageeffect;
			Bullet bullet;
			Point<int16_t> target;
		};

		void apply_attack(const AttackResult& attack);
		// 普攻命中后按 WZ finalAttack 链接决定是否延迟触发终极攻击（MinMS Combat.cpp:1157）。
		void maybe_queue_final_attack(const SpecialMove& move, const AttackResult& result);
		// 终极攻击施放（被动 is_attack()=false，走独立攻击路径发包）。
		void cast_final_attack(int32_t fa_id);
		void apply_move(const SpecialMove& move);
		std::vector<int32_t> find_closest(MapObjects* objs, Rectangle<int16_t> range, Point<int16_t> origin, uint8_t objcount, bool use_mobs) const;
		void apply_use_movement(const SpecialMove& move);
		void apply_result_movement(const SpecialMove& move, const AttackResult& result);
		void apply_rush(const AttackResult& result);
		void apply_bullet_effect(const BulletEffect& effect);
		void apply_damage_effect(const DamageEffect& effect);
		void extract_effects(const Char& user, const SpecialMove& move, const AttackResult& result);
		std::vector<DamageNumber> place_numbers(int32_t oid, const damage::DamageLines& damagelines);
		const SpecialMove& get_move(int32_t move_id);

		static constexpr size_t MAX_DAMAGE_NUMBERS = 160;

		Player& player;
		MapChars& chars;
		MapMobs& mobs;
		MapReactors& reactors;

		std::unordered_map<int32_t, Skill> skills;
		RegularAttack regularattack;

		TimedQueue<AttackResult> attackresults;
		TimedQueue<BulletEffect> bulleteffects;
		TimedQueue<DamageEffect> damageeffects;
		// 终极攻击延迟队列（MinMS Combat.cpp:201/204）：普攻命中后按 finalAttack
		// 链接+武器+prop 概率入队，等主攻击动作播完再施放 FA 技能（发攻击包）。
		TimedQueue<int32_t> pending_final_attacks;

		std::list<BulletEffect> bullets;
		std::list<DamageNumber> damagenumbers;

		// H1（2026-11-16）：宏释放队列。旧 Stage MACRO 分支同帧连调三次
		// use_move，第一行 if(!can_attack) return 把第 2、3 个技能全吞掉 ——
		// 「组合技能」实际只出一个。这里按帧泵出，等硬直结束再放下一个。
		std::vector<int32_t> macro_pending;
		uint16_t macro_cooldown = 0;
	};
}