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

#include "Combat/Combat.h"
#include "MapleMap/MapBackgrounds.h"
#include "MapleMap/MapDrops.h"
#include "MapleMap/MapEffect.h"
#include "MapleMap/MapFieldMarkers.h"
#include "MapleMap/MapNpcs.h"
#include "MapleMap/MapPortals.h"
#include "MapleMap/MapTilesObjs.h"

#include "../Timer.h"

#include "../IO/KeyType.h"

#include <chrono>
#include <unordered_map>

namespace ms
{
	class Stage : public Singleton<Stage>
	{
	public:
		Stage();

		void init();

		// Loads the map to display
		void load(int32_t mapid, int8_t portalid);
		// Remove all map objects and graphics
		void clear();

		// Construct the player from a character entry
		void loadplayer(const CharEntry& entry);

		// Call 'draw()' of all objects on stage
		void draw(float alpha) const;
		// Calls 'update()' of all objects on stage
		void update();

		// Show a character effect
		void show_character_effect(int32_t cid, CharEffect::Id effect);

		// Send key input to the stage
		void send_key(KeyType::Id keytype, int32_t keycode, bool pressed);
		// Send mouse input to the stage
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursor_position);

		// Check if the specified id is the player's id
		bool is_player(int32_t cid) const;

		// Returns a reference to the NPCs on the current map
		MapNpcs& get_npcs();
		// Returns a reference to the other characters on the current map
		MapChars& get_chars();
		// Returns a reference to the mobs on the current map
		MapMobs& get_mobs();
		// Returns a reference to the reactors on the current map
		MapReactors& get_reactors();
		// Returns a reference to the drops on the current map
		MapDrops& get_drops();
		// Field placeholders (summon/mist/door/kite/dragon) until full sprites
		MapFieldMarkers& get_field_markers();
		// Returns a reference to the Player
		Player& get_player();
		// Return a reference to the attack and buff component
		Combat& get_combat();
		// Returns the current map ID
		int32_t get_mapid();

		// Return a pointer to a character, possibly the player.
		Optional<Char> get_character(int32_t cid);
		// 按名查找地图上已加载角色的 oid（聊天点名「查看角色信息」用）。找不到返回 -1。
		int32_t find_character_id(const std::string& name) const;

		// Set a map effect
		void add_effect(std::string path);
		// Clear screen map effect
		void clear_effect();
		// Camera (tremble / view)
		Camera& get_camera();

		// Get the total up time of the client
		int64_t get_uptime();
		// Get the level at the start of the client
		uint16_t get_uplevel();
		// Get the EXP at the start of the client
		int64_t get_upexp();

		// Notify the server that the player has moved maps
		void transfer_player();

	private:
		void load_map(int32_t mapid);
		void respawn(int8_t portalid);
		void check_portals(bool allow_hidden = false);
		void check_seats();
		void check_ladders(bool up);
		void check_drops();
		void check_touch_reactors();
		int32_t touching_reactor_oid = 0;

		enum State
		{
			INACTIVE,
			TRANSITION,
			ACTIVE
		};

		Camera camera;
		Physics physics;
		Player player;

		Optional<Playable> playable;
		State state;
		int32_t mapid;

		// Wave 4：AUTO_AGGRO 上报节流 —— 碰撞检测每帧跑，同一怪 1s 内只报
		// 一次（等效原生的"进入接触"事件式上报；本端无接触沿检测，值为近似，
		// NO-NATIVE-EXACT）。Stage::clear() 换图时清空。
		std::unordered_map<int32_t, std::chrono::steady_clock::time_point> aggro_sent;

		// debug.gm_smoke 自驱动（仅 GM 冒烟，生产默认关）：进图后延时 3s 发一次
		// ADMIN_COMMAND 刷怪包，用来在没有输入注入手段的环境里造测试怪。
		// 三态：0=未触发 1=已排队(计时中) 2=已发/本图不再发。
		int smoke_state = 0;
		std::chrono::steady_clock::time_point smoke_armed_at;

		MapInfo mapinfo;
		MapTilesObjs tilesobjs;
		MapBackgrounds backgrounds;
		MapPortals portals;
		MapReactors reactors;
		MapNpcs npcs;
		MapChars chars;
		MapMobs mobs;
		MapDrops drops;
		MapFieldMarkers field_markers;
		MapEffect effect;

		Combat combat;

		std::chrono::time_point<std::chrono::steady_clock> start;
		uint16_t levelBefore;
		int64_t expBefore;
	};
}