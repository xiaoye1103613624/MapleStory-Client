//////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton					//
//																			//
//	Fyms085 resource abstraction additions Copyright (C) 2026				//
//																			//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.										//
//																			//
//	This program is distributed in the hope that it will be useful,			//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.						//
//																			//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.	//
//////////////////////////////////////////////////////////////////////////////
#include "IDataSource.h"

namespace ms
{
	namespace res
	{
	namespace
	{
		// 与 RootId 顺序严格一致；改动顺序会破坏根绑定。
		constexpr const char* ROOT_NAMES[static_cast<size_t>(RootId::Count)] =
		{
			"Base", "Character", "Effect", "Etc", "Item", "Map", "Map001", "Map002", "Map2",
			"Mob", "Mob001", "Mob002", "Mob2", "Morph", "Npc", "Quest", "Reactor", "Skill",
			"Skill001", "Skill002", "Skill003", "Sound", "Sound001", "Sound002", "Sound2",
			"String", "TamingMob", "UI"
		};
	}

	const char* to_string(Format format) noexcept
	{
		switch (format)
		{
		case Format::Img:
			return "img";
		case Format::Nx:
			return "nx";
		case Format::Wz:
			return "wz";
		default:
			return "unknown";
		}
	}

	const char* to_string(RootId id) noexcept
	{
		const size_t index = static_cast<size_t>(id);

		if (index >= static_cast<size_t>(RootId::Count))
			return "?";

		return ROOT_NAMES[index];
	}

	const char* root_filename(RootId id) noexcept
	{
		// 标准根的名字同时也是其文件名（各后端自行补 .img/.nx/.wz 后缀）。
		return to_string(id);
	}
	}
}
