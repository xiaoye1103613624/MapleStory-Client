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
#include "Spawn.h"

#include "MapleMap/ItemDrop.h"
#include "MapleMap/MesoDrop.h"
#include "MapleMap/Mob.h"
#include "MapleMap/Npc.h"
#include "MapleMap/Reactor.h"

#include "../Character/OtherChar.h"

namespace ms
{
	NpcSpawn::NpcSpawn(int32_t o, int32_t i, Point<int16_t> p, bool fl, uint16_t f) : oid(o), id(i), position(p), flip(fl), fh(f) {}

	int32_t NpcSpawn::get_oid() const
	{
		return oid;
	}

	std::unique_ptr<MapObject> NpcSpawn::instantiate(const Physics& physics) const
	{
		// 走服务端声明的 fh，而不是按坐标反查：NPC 包 y 槽是 wz 的 cy（实测比平台线低 5~17px），
		// get_y_below 会挑到下面一层平台，NPC 就"站到路下面"去了；反查不到时被丢到地图边界外，
		// 表现为 NPC 不见了。怪物包传的是真 y，所以一直是好的——这里跟怪物对齐。
		auto spawnposition = physics.get_spawn_point(position, fh);
		return std::make_unique<Npc>(id, oid, flip, fh, false, spawnposition);
	}

	MobSpawn::MobSpawn(int32_t o, int32_t i, int8_t m, int8_t st, uint16_t f, bool ns, int8_t t, Point<int16_t> p,
		int8_t se, bool par, int32_t poid) : oid(o), id(i), mode(m), stance(st), fh(f), newspawn(ns), team(t), position(p),
		spawn_effect(se), parent(par), parent_oid(poid) {}

	int8_t MobSpawn::get_mode() const
	{
		return mode;
	}

	int32_t MobSpawn::get_oid() const
	{
		return oid;
	}

	std::unique_ptr<MapObject> MobSpawn::instantiate() const
	{
		return std::make_unique<Mob>(oid, id, mode, stance, fh, newspawn, team, position,
			spawn_effect, parent, parent_oid);
	}

	int8_t MobSpawn::get_spawn_effect() const
	{
		return spawn_effect;
	}

	bool MobSpawn::is_parented() const
	{
		return parent;
	}

	int32_t MobSpawn::get_parent_oid() const
	{
		return parent_oid;
	}

	int8_t MobSpawn::get_stance() const
	{
		return stance;
	}

	uint16_t MobSpawn::get_fh() const
	{
		return fh;
	}

	int8_t MobSpawn::get_team() const
	{
		return team;
	}

	bool MobSpawn::is_newspawn() const
	{
		return newspawn;
	}

	Point<int16_t> MobSpawn::get_position() const
	{
		return position;
	}

	ReactorSpawn::ReactorSpawn(int32_t o, int32_t r, int8_t s, Point<int16_t> p) : oid(o), rid(r), state(s), position(p) {}

	int32_t ReactorSpawn::get_oid() const
	{
		return oid;
	}

	std::unique_ptr<MapObject> ReactorSpawn::instantiate(const Physics& physics) const
	{
		auto spawnposition = physics.get_y_below(position);
		return std::make_unique<Reactor>(oid, rid, state, spawnposition);
	}

	DropSpawn::DropSpawn(int32_t o, int32_t i, bool ms, int32_t ow, Point<int16_t> p, Point<int16_t> d, int8_t t, int8_t m, bool pd) : oid(o), id(i), meso(ms), owner(ow), start(p), dest(d), droptype(t), mode(m), playerdrop(pd) {}

	bool DropSpawn::is_meso() const
	{
		return meso;
	}

	int32_t DropSpawn::get_itemid() const
	{
		return id;
	}

	int32_t DropSpawn::get_oid() const
	{
		return oid;
	}

	std::unique_ptr<MapObject> DropSpawn::instantiate(const Animation& icon) const
	{
		return std::make_unique<MesoDrop>(oid, owner, start, dest, droptype, mode, playerdrop, icon);
	}

	std::unique_ptr<MapObject> DropSpawn::instantiate(const Texture& icon) const
	{
		return std::make_unique<ItemDrop>(oid, owner, start, dest, droptype, mode, id, playerdrop, icon);
	}

	CharSpawn::CharSpawn(int32_t cid, const LookEntry& look, uint16_t level, int16_t job, const std::string& name, int8_t stance, Point<int16_t> position) : cid(cid), look(look), level(level), job(job), name(name), stance(stance), position(position) {}

	int32_t CharSpawn::get_cid() const
	{
		return cid;
	}

	std::unique_ptr<MapObject> CharSpawn::instantiate() const
	{
		return std::make_unique<OtherChar>(cid, look, level, job, name, stance, position);
	}
}