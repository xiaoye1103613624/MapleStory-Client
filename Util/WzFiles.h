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

#ifndef USE_NX
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
		Error init();
	};
}
#endif
