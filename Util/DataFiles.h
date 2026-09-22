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
#pragma once

#include "../Error.h"

namespace ms
{
	namespace DataFiles
	{
		// Runtime backend dispatcher.
		//
		// Selects the data source based on Configuration::get_data_format():
		//   "img" -> ImgFiles  (classic standalone .img directory)
		//   "nx"  -> NoLifeNx native .nx files (nl::nx::load_all)
		//   "wz"  -> direct .wz archives via the WZ runtime (nl::wz::load_all)
		//
		// A single binary can therefore load any supported format without a
		// recompile; the game logic consumes the resulting nl::node view
		// identically regardless of which backend was chosen.
		Error init();
	}
}
