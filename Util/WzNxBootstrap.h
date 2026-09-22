//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client                    //
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton                      //
//                                                                              //
//	Fyms085 WZ cache bootstrap additions Copyright (C) 2026                    //
//                                                                              //
//	This program is free software: you can redistribute it and/or modify        //
//	it under the terms of the GNU Affero General Public License as published by //
//	the Free Software Foundation, either version 3 of the License, or            //
//	(at your option) any later version.                                         //
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Error.h"

namespace ms
{
	namespace WzNxBootstrap
	{
		// Accepts the original 16-file v085 WZ layout. If an NX cache is not
		// already present, invokes NoLifeWzToNx once and verifies its output.
		Error prepare();
	}
}

