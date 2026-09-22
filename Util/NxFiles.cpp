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
#include "NxFiles.h"

#ifdef USE_NX
#include <fstream>

#include <nlnx/node.hpp>
#include <nlnx/nx.hpp>

namespace ms
{
	namespace NxFiles
	{
		Error init()
		{
			// 宽松校验：NoLifeNx 在根文件缺失时生成空节点而非致命错误
			// （见 Util/ImgFiles.cpp 注释：BeiDou 数据不带 Base 目录，缺失根应作为
			// 空节点而非加载失败）。故不再要求 28 个根全齐，仅当「一个都没有」时
			// 才判定为路径配错 / 数据缺失，给出明确错误。
			bool any_present = false;

			for (auto filename : filenames)
				if (std::ifstream{ filename }.good())
					any_present = true;

			if (!any_present)
				return Error(Error::Code::MISSING_FILE, "no NX root files found; expected the .nx set in the working directory");

			try
			{
				nl::nx::load_all();
			}
			catch (const std::exception& ex)
			{
				static const std::string message = ex.what();

				return Error(Error::Code::NLNX, message.c_str());
			}

			constexpr const char* POSTCHAOS_BITMAP = "Login.img/WorldSelect/BtChannel/layer:bg";

			if (nl::nx::UI.resolve(POSTCHAOS_BITMAP).data_type() != nl::node::type::bitmap)
				return Error::Code::WRONG_UI_FILE;

			return Error::Code::NONE;
		}
	}
}
#endif