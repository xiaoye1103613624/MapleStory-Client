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
#include "MapMobs.h"
#include "Mob.h"

#include "../Physics/Physics.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

namespace ms
{
	void MapMobs::draw(Layer::Id layer, double viewx, double viewy, float alpha) const
	{
		mobs.draw(layer, viewx, viewy, alpha);
	}

	void MapMobs::update(const Physics& physics)
	{
		for (; !spawns.empty(); spawns.pop())
		{
			const MobSpawn& spawn = spawns.front();

			if (Optional<Mob> mob = mobs.get(spawn.get_oid()))
			{
				// 同 oid 的整包重下发（换控制器 / 脚本重 spawn）要连落位一起吃：
				// 原来只 makeactive() 复用旧对象，新坐标、姿态、队伍全被丢掉。
				mob->refresh(spawn.get_mode(), spawn.get_stance(), spawn.get_fh(),
					spawn.get_team(), spawn.is_newspawn(), spawn.get_position(),
					spawn.get_spawn_effect(), spawn.is_parented(), spawn.get_parent_oid());
			}
			else
			{
				mobs.add(spawn.instantiate());
			}
		}

		// 显式回收：对齐 Angel CMobPool::OnMobLeaveField 的 ZMap::RemoveKey。
		// 死亡表现与对象回收解耦——此处按动画结束 / 兜底帧数从池显式删除死体，
		// 不再依赖 Mob::update() 返回 -1 作为唯一删除路径。
		for (auto iter = removal.begin(); iter != removal.end();)
		{
			int32_t oid = iter->first;
			uint16_t& frames = iter->second;

			bool done = false;

			if (Optional<Mob> mob = mobs.get(oid))
				done = mob->is_dead() || --frames == 0;
			else
				done = true; // 已被别处（换图/重置）移除

			if (done)
			{
				mobs.remove(oid);
				iter = removal.erase(iter);
			}
			else
			{
				++iter;
			}
		}

		mobs.update(physics);
	}

	void MapMobs::spawn(MobSpawn&& spawn)
	{
		spawns.emplace(std::move(spawn));
	}

	void MapMobs::remove(int32_t oid, int8_t animation)
	{
		if (Optional<Mob> mob = mobs.get(oid))
		{
			// 收包即改变活体语义：kill() 立即停用移动/攻击/受伤/碰撞并启动死亡表现；
			// is_alive() 随后返回 false，怪不再移动/攻击/显示血条。死体回收交池层显式完成。
			mob->kill(animation);

			if (animation == 0)
			{
				// 立即消失（无动画可播）：直接显式从池删除，对齐 Angel OnMobLeaveField。
				mobs.remove(oid);
			}
			else
			{
				// 有死亡表现（die/fade）：登记到显式回收队列，等动画结束或兜底帧数再删。
				removal.emplace_back(oid, MOB_REMOVAL_FRAMES);
			}
		}

		// 分裂怪跟随父怪（spawn 尾段 effect==-3 雪藏）：父怪在这里被击杀 ⇒ 唤醒子怪。
		// 服务端对 -3 怪不再补发 SPAWN_MOB（s9 spawnMonsterInternal:1554-1558 只写
		// -3+parentOid，客户端职责就是本地雪藏/唤醒），不接这一步扎昆之手类怪永不出现。
		for (auto& mmo : mobs)
		{
			Mob* m = static_cast<Mob*>(mmo.second.get());

			if (m && m->is_dormant() && m->get_parent_oid() == oid)
				m->wake();
		}
	}

	void MapMobs::clear_spawns()
	{
		// 与 MapDrops/MapNpcs 同理，待生成 mob spawn 队列必须清空，否则旧图
		// 怪物包在新图 foothold 上重播 movepath，视觉残留/越界。
		std::queue<MobSpawn>().swap(spawns);
	}

	void MapMobs::reanchor_spawns(const Physics& physics)
	{
		// 对齐 MapNpcs::reanchor_spawns（MapNpcs.cpp:72-93）：
		// MobSpawn::instantiate 不查物理，旧图残留包会带旧 fh/坐标实体化到新图。
		// 这里按当前图 foothold 重锚：fh 有效则保留（新图合法包），当前图 x 处无
		// 落脚点则丢弃（旧图残留）。与 NPC 同款，不另起炉灶。
		std::queue<MobSpawn> kept;
		while (!spawns.empty())
		{
			MobSpawn spawn = std::move(spawns.front());
			spawns.pop();

			SpawnPoint resolved = physics.resolve_spawn_point(spawn.get_position(), spawn.get_fh());
			if (resolved.fhid == 0)
				continue; // 旧图残留包：当前图无落脚点，丢弃

			spawn.set_fh(resolved.fhid);
			kept.push(std::move(spawn));
		}
		spawns.swap(kept);
	}

	void MapMobs::clear()
	{
		mobs.clear();
		clear_spawns();
		removal.clear();	// 补齐漏清的显式回收队列（MapMobs.h:85），零风险
	}

	void MapMobs::set_control(int32_t oid, bool control)
	{
		int8_t mode = control ? 1 : 0;

		if (Optional<Mob> mob = mobs.get(oid))
			mob->set_control(mode);
	}

	void MapMobs::set_next_skill(int32_t oid, bool use, int8_t skill, int8_t level)
	{
		if (Optional<Mob> mob = mobs.get(oid))
			mob->set_next_skill(use, skill, level);
	}

	void MapMobs::send_mobhp(int32_t oid, int8_t percent, uint16_t playerlevel)
	{
		if (Optional<Mob> mob = mobs.get(oid))
			mob->show_hp(percent, playerlevel);
	}

	void MapMobs::send_movement(int32_t oid, Point<int16_t> start, std::vector<Movement>&& movements,
		uint8_t skillbyte, int8_t skillid, int8_t skilllevel, int16_t poption)
	{
		if (Optional<Mob> mob = mobs.get(oid))
			mob->send_movement(start, std::move(movements), skillbyte, skillid, skilllevel, poption);
	}

	void MapMobs::send_attack(AttackResult& result, const Attack& attack, const std::vector<int32_t>& targets, uint8_t mobcount)
	{
		for (auto& target : targets)
		{
			if (Optional<Mob> mob = mobs.get(target))
			{
				result.damagelines[target] = mob->calculate_damage(attack);
				result.mobcount++;

				if (result.mobcount == 1)
					result.first_oid = target;

				if (result.mobcount == mobcount)
					result.last_oid = target;
			}
		}
	}

	void MapMobs::apply_damage(int32_t oid, int32_t damage, bool toleft, const AttackUser& user, const SpecialMove& move, bool lethal)
	{
		if (Optional<Mob> mob = mobs.get(oid))
		{
			mob->apply_damage(damage, toleft, lethal);

			// TODO: Maybe move this into the method above too?
			move.apply_hiteffects(user, *mob);
		}
	}

	bool MapMobs::contains(int32_t oid) const
	{
		return mobs.contains(oid);
	}

	int32_t MapMobs::find_colliding(const MovingObject& moveobj) const
	{
		Range<int16_t> horizontal = Range<int16_t>(moveobj.get_last_x(), moveobj.get_x());
		Range<int16_t> vertical = Range<int16_t>(moveobj.get_last_y(), moveobj.get_y());

		Rectangle<int16_t> player_rect = {
			horizontal.smaller(),
			horizontal.greater(),
			static_cast<int16_t>(vertical.smaller() - 50),
			vertical.greater()
		};

		auto iter = std::find_if(
			mobs.begin(),
			mobs.end(),
			[&player_rect](auto& mmo)
			{
				Optional<Mob> mob = mmo.second.get();
				return mob && mob->is_alive() && mob->is_in_range(player_rect);
			}
		);

		if (iter == mobs.end())
			return 0;

		return iter->second->get_oid();
	}

	MobAttack MapMobs::create_attack(int32_t oid) const
	{
		if (Optional<const Mob> mob = mobs.get(oid))
			return mob->create_touch_attack();
		else
			return {};
	}

	Point<int16_t> MapMobs::get_mob_position(int32_t oid) const
	{
		if (auto mob = mobs.get(oid))
			return mob->get_position();
		else
			return Point<int16_t>(0, 0);
	}

	Point<int16_t> MapMobs::get_mob_head_position(int32_t oid) const
	{
		if (Optional<const Mob> mob = mobs.get(oid))
			return mob->get_head_position();
		else
			return Point<int16_t>(0, 0);
	}

	MapObjects* MapMobs::get_mobs()
	{
		return &mobs;
	}
}