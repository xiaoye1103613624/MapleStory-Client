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

#include "../Graphics/Text.h"

#include <iostream>
#include <unordered_map>
#include <list>
#include <vector>
#include <string>

namespace ms
{
	namespace string_conversion
	{
		template<typename T>
		inline T or_default(const std::string& str, T def)
		{
			try
			{
				int32_t intval = std::stoi(str);
				return static_cast<T>(intval);
			}
			catch (const std::exception& ex)
			{
				LOG(LOG_ERROR, __func__ << ": " << ex.what());

				return def;
			}
		}

		template<typename T>
		inline T or_zero(const std::string& str)
		{
			return or_default<T>(str, T(0));
		}
	};

	namespace string_format
	{
		// Format a number string so that each three decimal points are separated by a comma
		void split_number(std::string& input);

		// Prefix an id with zeros so that it has the minimum specified length
		std::string extend_id(int32_t id, size_t length);

		// Skill.nx / Skill.wz 的命名空间目录名 = 技能 id 去掉末 4 位（不足 3 位补零）。
		// 7 位 id 得到 3 位（冒险家 110.img、211.img），8 位 id 得到 4 位
		// （骑士团 1000.img、战神 2112.img），4 位的新手 id 得到 000.img。
		// 证据：BeiDou-Server_s9 client/Skill.java:37 `this.job = id / 10000`
		//       + wz/Skill.wz 目录清单（100..522 三位、1000..2112 四位、000 新手）。
		std::string skill_namespace(int32_t skillid);

		// Pad a string with leading zeros to a specified length
		std::string pad_string(std::string str, size_t length);

		// Cut off a string at a specified width with an ellipsis
		void format_with_ellipsis(Text& input, int16_t width, uint16_t ellipsis_width = 2U, bool word_break = false);

		// Convert a string to lowercase letters
		std::string tolower(std::string str);
	};

	namespace bytecode
	{
		// Check if a bit mask contains the specified value
		bool compare(int32_t mask, int32_t value);
	}

	namespace NxHelper
	{
		namespace Map
		{
			struct MapInfo
			{
				std::string description;
				std::string name;
				std::string street_name;
				std::string full_name;
			};

			// Returns all relative map info
			MapInfo get_map_info_by_id(int32_t mapid);

			// Returns the category of a map
			std::string get_map_category(int32_t mapid);

			// Returns a list of all life on a map (Mobs and NPCs)
			std::unordered_map<int64_t, std::pair<std::string, std::string>> get_life_on_map(int32_t mapid);

			// Structured mob spawn info for world-map side panels (deduped, sorted).
			struct MapMobInfo
			{
				int32_t id = 0;
				std::string name;
				int16_t level = 0;
				// maxHP from Mob.img/info; 0 when the node is missing.
				int32_t max_hp = 0;
			};

			// Unique mobs on a map (life type "m"), sorted by level then id.
			std::vector<MapMobInfo> get_mobs_on_map(int32_t mapid);

			// Returns the name of the node, under which the argument map id is in
			nl::node get_map_node_name(int32_t mapid);
		}
	}

	namespace single_console
	{
		void log_message(std::string message);

		static std::list<std::string> log_history;
	}
}