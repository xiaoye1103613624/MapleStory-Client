////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	Fyms085 IMG backend additions Copyright (C) 2026							//
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

#include "../Error.h"

#include <string>

namespace ms
{
	namespace ImgFiles
	{
		// IMG data backend.
		//
		// Reads a directory of classic standalone .img files. Each standard
		// MapleStory root (Base, Character, Effect, Etc, Item, Map, Mob, Morph,
		// Npc, Quest, Reactor, Skill, Sound, String, TamingMob, UI) is expected
		// as a subdirectory of the configured data directory; every .img file
		// inside it becomes a child node named "<file>.img" (so the game keeps
		// accessing data exactly as it does through the NX backend, e.g.
		// nl::nx::Map["Map001.img"]).
		//
		// The .img files are converted into a cached set of .nx files via
		// nl::convert_img_files_to_nx and then loaded through nl::nx::load_all,
		// giving the game a single, unchanged nl::node view regardless of the
		// underlying data source. The conversion result is cached in a
		// ".nxcache" subfolder so subsequent launches skip re-parsing.
		//
		// The data directory is taken from Configuration::get_data_dir().
		//
		// @base_dir 非空时优先使用该目录；为空时回退到配置中的 data.dir。
		//          由资源门面在定位到数据目录后传入。
		Error init(const std::string& base_dir = std::string());
	}
}
