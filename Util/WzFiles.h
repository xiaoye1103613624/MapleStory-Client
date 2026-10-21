////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../MapleStory.h"

// 说明：此处原为 `#ifndef USE_NX` 的编译期开关（旧设计是「NX 与 WZ 二选一」）。
// 现在数据源改为**运行时**由 DataFormat 选择（见 Util/DataFiles.cpp），
// NX 与 WZ 两套后端都必须可用，故移除该开关，让 WzFiles 始终参与编译。
#include "../Error.h"

#include <array>
#include <string>

// The direct WZ backend (NoLifeWzToNx, embedded via Util/WzRuntime.cpp) exposes
// the same nl::nx roots the NX backend does, so game code keeps using
// nl::nx::<file>["x.img"] unchanged. Include the real node API here so every
// translation unit that pulls in WzFiles.h gets the working nl::node type.
#include <nlnx/nx.hpp>
#include <nlnx/wz.hpp>

namespace ms
{
	namespace WzFiles
	{
		// Number of needed files (single + split volumes, matching the NX layout)
		constexpr uint8_t NUM_FILES = 28;

		// Names of the needed game files (.wz variant)
		constexpr std::array<const char*, NUM_FILES> filenames =
		{
			"Base.wz",
			"Character.wz",
			"Effect.wz",
			"Etc.wz",
			"Item.wz",
			"Map.wz",
			"Map001.wz",
			"Map002.wz",
			"Map2.wz",
			"Mob.wz",
			"Mob001.wz",
			"Mob002.wz",
			"Mob2.wz",
			"Morph.wz",
			"Npc.wz",
			"Quest.wz",
			"Reactor.wz",
			"Skill.wz",
			"Skill001.wz",
			"Skill002.wz",
			"Skill003.wz",
			"Sound.wz",
			"Sound001.wz",
			"Sound002.wz",
			"Sound2.wz",
			"String.wz",
			"TamingMob.wz",
			"UI.wz"
		};

		// Initialize the WZ backend (NoLifeWzToNx) and verify the UI file version.
		//
		// @base_dir 非空时，所有 .wz 归档都在此目录下解析；为空则保持既有行为
		//           （相对当前工作目录）。由资源门面在定位到数据目录后传入。
		// Initialize the WZ backend; @base_dir optionally relocates the archives.
		Error init(const std::string& base_dir = std::string());
	};
}
