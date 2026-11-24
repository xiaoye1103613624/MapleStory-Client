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

#include <algorithm>

namespace ms
{
	NpcSpawn::NpcSpawn(int32_t o, int32_t i, Point<int16_t> p, bool fl, uint16_t f, bool cnt,
		int16_t rx0, int16_t rx1)
		: oid(o), id(i), position(p), flip(fl), fh(f), control(cnt),
		rx_min(0), rx_max(0)
	{
		// s9 写的是 rx0=x+50(右)/rx1=x-50(左)，方向相反；归一化后 min=左界 max=右界。
		// 两值相等（包未给范围）时保持 0/0，Npc 侧据此关闭范围夹取。
		if (rx0 != rx1)
		{
			int16_t lo = std::min(rx0, rx1);
			int16_t hi = std::max(rx0, rx1);
			// 兜底：绝对范围必须包含 spawn 的 x；若不包含，说明 rx 是相对偏移/脏数据，
			// 宁可关闭范围（仍有 TURNATEDGES 台阶转向兜底），也不要把 NPC 钉到错误坐标。
			if (lo <= p.x() && p.x() <= hi)
			{
				rx_min = lo;
				rx_max = hi;
			}
		}
	}

	int32_t NpcSpawn::get_oid() const
	{
		return oid;
	}

	uint16_t NpcSpawn::get_fh() const
	{
		return fh;
	}

	void NpcSpawn::set_fh(uint16_t nfhid)
	{
		fh = nfhid;
	}

	Point<int16_t> NpcSpawn::get_position() const
	{
		return position;
	}

	int16_t NpcSpawn::get_rx_min() const
	{
		return rx_min;
	}

	int16_t NpcSpawn::get_rx_max() const
	{
		return rx_max;
	}

	std::unique_ptr<MapObject> NpcSpawn::instantiate(const Physics& physics) const
	{
		// 走服务端声明的 fh，而不是按坐标反查：NPC 包 y 槽是 wz 的 cy（实测比平台线低 5~17px），
		// get_y_below 会挑到下面一层平台，NPC 就"站到路下面"去了；反查不到时被丢到地图边界外，
		// 表现为 NPC 不见了。怪物包传的是真 y，所以一直是好的——这里跟怪物对齐。
		// 切图重锚：resolve 返回的 fhid 是落点解析后的真实 fh——fh 有效则保留（新图包），
		// fh 指向旧图则按 x 就近重锚并回写，保证 phobj.fhid 与落点平台一致，首帧不再坠落。
		SpawnPoint resolved = physics.resolve_spawn_point(position, fh);
		if (resolved.fhid == 0)
			return nullptr; // 当前图在包 x 处无任何平台 = 旧图残留包，丢弃而非掉进边界
		return std::make_unique<Npc>(id, oid, flip, resolved.fhid, control, resolved.position, rx_min, rx_max);
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

	void MobSpawn::set_fh(uint16_t nfhid)
	{
		// 切图重锚：把 spawn 声明的 fh 改成当前图 foothold 解析后的真实值。
		fh = nfhid;
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

	DropSpawn::DropSpawn(int32_t o, int32_t i, bool ms, int32_t ow, Point<int16_t> p, Point<int16_t> d, int8_t t, int8_t m, bool pd, int8_t g) : oid(o), id(i), meso(ms), owner(ow), start(p), dest(d), droptype(t), mode(m), playerdrop(pd), grade(g) {}

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

	int8_t DropSpawn::get_grade() const
	{
		return grade;
	}

	Point<int16_t> DropSpawn::get_destination() const
	{
		return dest;
	}

	void DropSpawn::set_destination(Point<int16_t> ndest)
	{
		dest = ndest;
	}

	std::unique_ptr<MapObject> DropSpawn::instantiate(const Animation& icon) const
	{
		return std::make_unique<MesoDrop>(oid, owner, start, dest, droptype, mode, playerdrop, icon, grade);
	}

	std::unique_ptr<MapObject> DropSpawn::instantiate(const Texture& icon) const
	{
		return std::make_unique<ItemDrop>(oid, owner, start, dest, droptype, mode, id, playerdrop, icon, grade);
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