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
#pragma once

// 根绑定表（Bridge）。
//
// 业务层（Character / IO / Gameplay）到处直接使用 nl::nx::Map、nl::nx::UI 等
// 全局变量。抽象层不打算一次性改动它们，因此由本表把"28 个标准根"抽象成
// RootId，并在装载时把后端产出的根节点回写到这些全局变量：
//
//     后端 parser  ->  RootTable::bind(RootId, node)  ->  nl::nx::<Root>
//
// 这样后端可替换（策略模式），而上层零改动。

#include "IDataSource.h"

namespace ms
{
	namespace res
	{
	class RootTable
	{
	public:
		// 根的总数（与 RootId::Count 一致）。
		static constexpr size_t SIZE = static_cast<size_t>(RootId::Count);

		// 把某个根绑定为指定节点（同时写入对应的 nl::nx 全局变量）。
		static void bind(RootId id, const nl::node& node);

		// 取某个根的当前节点；未绑定时为空 node。
		static nl::node root(RootId id);

		// 清空所有根（数据源切换/关闭时使用）。
		static void clear();

		// 统计已绑定的根数量，供诊断输出。
		static size_t bound_count();

		// 收集所有当前为空的根，供诊断输出（缺失根是允许的，仅提示）。
		static std::vector<RootId> missing_roots();
	};
	}
}
