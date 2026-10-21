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
#include "RootTable.h"

#include <nlnx/nx.hpp>

#include <array>

namespace ms
{
	namespace res
	{
	namespace
	{
		// 与 RootId 顺序严格一致：索引即 RootId。
		//
		// 这里保存的是 nl::nx::* 这些全局变量的**地址**，写入时等价于
		// "nl::nx::Base = node"，从而让既有业务代码无需任何改动。
		std::array<nl::node*, RootTable::SIZE> slots()
		{
			return
			{
				&nl::nx::Base,
				&nl::nx::Character,
				&nl::nx::Effect,
				&nl::nx::Etc,
				&nl::nx::Item,
				&nl::nx::Map,
				&nl::nx::Map001,
				&nl::nx::Map002,
				&nl::nx::Map2,
				&nl::nx::Mob,
				&nl::nx::Mob001,
				&nl::nx::Mob002,
				&nl::nx::Mob2,
				&nl::nx::Morph,
				&nl::nx::Npc,
				&nl::nx::Quest,
				&nl::nx::Reactor,
				&nl::nx::Skill,
				&nl::nx::Skill001,
				&nl::nx::Skill002,
				&nl::nx::Skill003,
				&nl::nx::Sound,
				&nl::nx::Sound001,
				&nl::nx::Sound002,
				&nl::nx::Sound2,
				&nl::nx::String,
				&nl::nx::TamingMob,
				&nl::nx::UI
			};
		}
	}

	void RootTable::bind(RootId id, const nl::node& node)
	{
		const size_t index = static_cast<size_t>(id);

		if (index >= SIZE)
			return;

		*slots()[index] = node;
	}

	nl::node RootTable::root(RootId id)
	{
		const size_t index = static_cast<size_t>(id);

		if (index >= SIZE)
			return {};

		return *slots()[index];
	}

	void RootTable::clear()
	{
		for (nl::node* slot : slots())
			if (slot)
				*slot = nl::node{};
	}

	size_t RootTable::bound_count()
	{
		size_t count = 0;

		for (nl::node* slot : slots())
			if (slot && *slot)
				count++;

		return count;
	}

	std::vector<RootId> RootTable::missing_roots()
	{
		std::vector<RootId> missing;
		const auto table = slots();

		for (size_t i = 0; i < SIZE; i++)
			if (table[i] && !*table[i])
				missing.push_back(static_cast<RootId>(i));

		return missing;
	}
	}
}
