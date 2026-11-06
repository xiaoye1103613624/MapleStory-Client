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

#include "MapleMap/MapObject.h"

#include "../Graphics/Animation.h"
#include "../Net/Login.h"

#include <memory>

namespace ms
{
	class NpcSpawn
	{
	public:
		NpcSpawn(int32_t oid, int32_t npcid, Point<int16_t> position, bool mirrored, uint16_t fh, bool control = false);

		int32_t get_oid() const;
		std::unique_ptr<MapObject> instantiate(const Physics& physics) const;

	private:
		int32_t oid;
		int32_t id;
		Point<int16_t> position;
		bool flip;
		uint16_t fh;
		bool control;
	};

	class MobSpawn
	{
	public:
		// spawn_effect/parent 来自 SPAWN_MOB 尾段（见 MapObjectHandlers 的
		// read_mob_spawn_tail；s9 PacketCreator:1449-1459/1544-1564）：
		//   spawn_effect >0 是出生特效（波 2 播 Mob/SpecialEff），
		//   parent=true 时 parent_oid 是父怪（扎昆之手/分裂怪），-3 表示
		//   父怪死后才出现。修前这些字节被错位读进 team/newspawn。
		MobSpawn(int32_t oid, int32_t id, int8_t mode, int8_t stance, uint16_t fh, bool newspawn, int8_t team, Point<int16_t> position,
			int8_t spawn_effect = 0, bool parent = false, int32_t parent_oid = 0);

		int8_t get_mode() const;
		int32_t get_oid() const;
		// 同 oid 重新下发时这些字段要覆盖到已有对象上（服务端换控制器发的是整包 spawn）
		int8_t get_stance() const;
		uint16_t get_fh() const;
		int8_t get_team() const;
		bool is_newspawn() const;
		Point<int16_t> get_position() const;
		int8_t get_spawn_effect() const;
		bool is_parented() const;
		int32_t get_parent_oid() const;
		std::unique_ptr<MapObject> instantiate() const;

	private:
		int32_t oid;
		int32_t id;
		int8_t mode;
		int8_t stance;
		uint16_t fh;
		bool newspawn;
		int8_t team;
		Point<int16_t> position;
		int8_t spawn_effect;
		bool parent;
		int32_t parent_oid;
	};

	class ReactorSpawn
	{
	public:
		ReactorSpawn(int32_t oid, int32_t rid, int8_t state, Point<int16_t> position);

		int32_t get_oid() const;
		std::unique_ptr<MapObject> instantiate(const Physics& physics) const;

	private:
		int32_t oid;
		int32_t rid;
		int8_t state;
		Point<int16_t> position;
	};

	class DropSpawn
	{
	public:
		DropSpawn(int32_t oid, int32_t id, bool meso, int32_t owner, Point<int16_t> position, Point<int16_t> destination, int8_t droptype, int8_t mode, bool playerdrop);

		bool is_meso() const;
		int32_t get_itemid() const;
		int32_t get_oid() const;
		std::unique_ptr<MapObject> instantiate(const Animation& icon) const;
		std::unique_ptr<MapObject> instantiate(const Texture& icon) const;

	private:
		int32_t oid;
		int32_t id;
		bool meso;
		int32_t owner;
		Point<int16_t> start;
		Point<int16_t> dest;
		int8_t droptype;
		int8_t mode;
		bool playerdrop;
	};

	class CharSpawn
	{
	public:
		CharSpawn(int32_t cid, const LookEntry& look, uint16_t level, int16_t job, const std::string& name, int8_t stance, Point<int16_t> position);

		int32_t get_cid() const;
		std::unique_ptr<MapObject> instantiate() const;

	private:
		int32_t cid;
		uint16_t level;
		int16_t job;
		std::string name;
		int8_t stance;
		Point<int16_t> position;
		LookEntry look;
	};
}