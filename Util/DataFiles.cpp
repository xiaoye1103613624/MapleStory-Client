////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	Fyms085 data dispatcher additions Copyright (C) 2026						//
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
#include "DataFiles.h"
#include "../Configuration.h"
#include "ImgFiles.h"

#include <nlnx/nx.hpp>
#include <nlnx/wz.hpp>

#include <fstream>
#include <stdexcept>
#include <string>

namespace ms
{
	namespace DataFiles
	{
		Error init()
		{
			const std::string fmt = Configuration::get().get_data_format();

			// 调试日志（默认关闭，避免启动期刷屏）。
			std::ofstream dbg;

			if (fmt == "img")
				return ImgFiles::init();

			try
			{
				if (fmt == "nx")
					nl::nx::load_all();
				else
					nl::wz::load_all(); // "wz" (and any unknown value) is the default
			}
			catch (const std::exception& ex)
			{
				static const std::string message = ex.what();
				return Error(Error::Code::NLNX, message.c_str());
			}

			return Error::Code::NONE;
		}
	}
}
