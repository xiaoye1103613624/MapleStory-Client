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
#include "NxFiles.h"
#include "WzFiles.h"

#include <string>

namespace ms
{
	namespace DataFiles
	{
		// 运行时分发器：根据 Configuration::get_data_format() 选择数据源，并委托给
		// 对应的权威加载器。三种加载器各自带「28 个标准根文件齐备性校验 + UI 版本校验 +
		// 语义正确的错误码」，避免在此重复实现导致行为漂移。
		//
		// 此前把任何非 "img"/"nx" 的值都静默当成 "wz"（见旧实现），会把「DataFormat 写错」
		// 伪装成「WZ 数据有问题」，排障方向被带偏。这里改为显式三值匹配，未知值给出
		// 明确的可重试错误。
		Error init()
		{
			const std::string fmt = Configuration::get().get_data_format();

			if (fmt == "img")
				return ImgFiles::init();

			if (fmt == "nx")
				return NxFiles::init();

			if (fmt == "wz")
				return WzFiles::init();

			// 未知取值：明确提示合法取值，且可被既有重试机制重试
			// （用户改好 Settings 后重跑 init 即可重新加载）。
			static const std::string message =
				"Unknown DataFormat \"" + fmt + "\"; expected \"img\", \"nx\" or \"wz\".";
			return Error(Error::Code::DATA_FORMAT, message.c_str());
		}
	}
}
