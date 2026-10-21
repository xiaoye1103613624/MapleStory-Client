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

// 资源定位器（Chain of Responsibility）。
//
// 以前每个后端都只认"当前工作目录"：把 .wz / .nx 放在别处就启动不了，
// 而报错信息也只说"找不到文件"，不带任何线索。
//
// 定位器把"去哪儿找数据"变成一条候选链，按优先级依次自省（probe），
// 取分数最高者；并把**尝试过的全部候选**记录下来，
// 用于在失败时给出可排障的错误信息。

#include "IDataSource.h"

#include <string>
#include <vector>

namespace ms
{
	namespace res
	{
		// 一次定位的结论。
		struct Layout
		{
			// 是否找到了可用数据源。
			bool found = false;
			// 命中的格式；未找到时为 Unknown。
			Format format = Format::Unknown;
			// 命中的数据根目录（传给后端的 base）。
			std::string base;
			// 命中的后端名字（诊断用）。
			std::string backend;
			// 命中分数（根命中数量），用于比较多个候选。
			int score = 0;
			// 尝试过的全部候选目录，按尝试顺序（失败时用于排障）。
			std::vector<std::string> tried;
		};

		class ResourceLocator
		{
		public:
			// 候选目录链，按优先级从高到低、已去重。
			//
			//   1. 环境变量 XY_DATA_DIR（运维/联调临时指定）
			//   2. 配置 data.dir（部署与用户配置）
			//   3. 可执行文件所在目录
			//   4. 可执行文件目录下的 Data/
			//   5. 可执行文件目录的父目录
			//   6. 父目录下的 Data/
			//   7. 当前工作目录
			//   8. 当前工作目录下的 Data/
			static std::vector<std::string> candidates();

			// 在候选链中定位数据源。
			// @wanted 为 Unknown 时自动挑选分数最高者（DataFormat=auto）；
			//         否则只考虑该格式，但仍会在候选链中寻找它的位置。
			static Layout locate(Format wanted = Format::Unknown);

			// 可执行文件所在目录；失败时返回空串。
			static std::string executable_dir();

			// 把尝试过的候选拼成一行文本，供错误信息与诊断使用。
			static std::string describe_tried(const Layout& layout);
		};
	}
}
