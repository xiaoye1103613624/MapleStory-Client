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
#include "WzFiles.h"

// 与 WzFiles.h 一致：移除 `#ifndef USE_NX`，让 WZ 后端始终参与编译（运行时按 DataFormat 选择）。
#include <fstream>

#include <nlnx/nx.hpp>
#include <nlnx/wz.hpp>

// WZ 后端复用了 NX 后端同款的 UI 版本校验（nl::nx::UI.resolve(...)），
// 该表达式依赖 nl::node::type::bitmap，因此需要显式引入 node.hpp
// （nx.hpp 在本构建配置下不会把它带入作用域）。
#include <nlnx/node.hpp>

namespace ms
{
	namespace WzFiles
	{
		Error init()
		{
			// 宽松校验：与 NxFiles::init() 一致 —— 容忍缺失根（BeiDou 类数据不保证 28 个根全齐），
			// 仅当「一个 .wz 根都没有」时才判定为数据缺失，给出明确错误。
			bool any_present = false;

			for (auto filename : filenames)
				if (std::ifstream{ filename }.good())
					any_present = true;

			if (!any_present)
				return Error(Error::Code::MISSING_FILE, "no WZ root files found; expected the .wz set in the working directory");

			try
			{
				nl::wz::load_all();
			}
			catch (const std::exception& ex)
			{
				static const std::string message = ex.what();

				return Error(Error::Code::WZ, message.c_str());
			}

			// Verify the UI archive is the expected version before launching.
			constexpr const char* POSTCHAOS_BITMAP = "Login.img/WorldSelect/BtChannel/layer:bg";

			if (nl::nx::UI.resolve(POSTCHAOS_BITMAP).data_type() != nl::node::type::bitmap)
				return Error::Code::WRONG_UI_FILE;

			return Error::Code::NONE;
		}
	}
}