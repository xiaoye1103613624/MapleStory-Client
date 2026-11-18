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

#include "MapObjects.h"

#include "../Spawn.h"

#include "../Combat/Attack.h"
#include "../Combat/SpecialMove.h"

#include <queue>
#include <vector>

namespace ms
{
	// A collection of mobs on a map.
	class MapMobs
	{
	public:
		// Draw all mobs on a layer.
		void draw(Layer::Id layer, double viewx, double viewy, float alpha) const;
		// Update all mobs.
		void update(const Physics& physics);

		// Spawn a new mob.
		void spawn(MobSpawn&& spawn);
		// Kill a mob.
		void remove(int32_t oid, int8_t effect);
		// Remove all mobs.
		void clear();
		void clear_spawns();	// 切图窗口期残留 spawn 队列清理（不动场上对象）

		// Update who a mob is controlled by.
		void set_control(int32_t oid, bool control);
		// 服务端 MOVE_MONSTER_RESPONSE 下发的下一可用技能，转交控制器怪出招。
		void set_next_skill(int32_t oid, bool use, int8_t skill, int8_t level);
		// Update a mob's hp display.
		void send_mobhp(int32_t oid, int8_t percent, uint16_t playerlevel);
		// Update a mob's movements (s9 moveMonster carries an optional skill header + pOption).
		void send_movement(int32_t oid, Point<int16_t> start, std::vector<Movement>&& movements,
			uint8_t skillbyte = 0, int8_t skillid = 0, int8_t skilllevel = 0, int16_t poption = 0);

		// Calculate the results of an attack.
		void send_attack(AttackResult& result, const Attack& attack, const std::vector<int32_t>& targets, uint8_t mobcount);
		// Applies damage to a mob.
		void apply_damage(int32_t oid, int32_t damage, bool toleft, const AttackUser& user, const SpecialMove& move);

		// Check if the mob with the specified oid exists.
		bool contains(int32_t oid) const;
		// Return the id of the first mob who collides with the object.
		int32_t find_colliding(const MovingObject& moveobj) const;
		// Create an attack by the specified mob.
		MobAttack create_attack(int32_t oid) const;
		// Return the position of a mob.
		Point<int16_t> get_mob_position(int32_t oid) const;
		// Return the head position of a mob.
		Point<int16_t> get_mob_head_position(int32_t oid) const;
		// Return all mob map objects
		MapObjects* get_mobs();

	private:
		MapObjects mobs;

		// 显式回收队列（对齐 Angel CMobPool::OnMobLeaveField 的 ZMap::RemoveKey）：
		// 收 kill 包即登记 {oid, 剩余帧数}，update() 中按死亡表现结束/兜底帧数
		// 调 mobs.remove(oid) 显式删除，死亡表现与对象回收彻底解耦。
		// 兜底帧数 ≈ 3 秒（60fps×3），足够播完 die 动画；动画提前结束则立即回收。
		static constexpr uint16_t MOB_REMOVAL_FRAMES = 180;
		std::vector<std::pair<int32_t, uint16_t>> removal;

		std::queue<MobSpawn> spawns;
	};
}