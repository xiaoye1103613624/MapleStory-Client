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
#pragma once

#include "../Configuration.h"

#ifdef _WIN32
#include <windef.h>
#include <WinUser.h>
#endif

namespace ms
{
	class ScreenResolution
	{
	public:
#ifdef _WIN32
		ScreenResolution()
		{
			RECT desktop;

			// Get a handle to the desktop window
			const HWND hDesktop = GetDesktopWindow();

			// Get the size of screen to the variable desktop
			GetWindowRect(hDesktop, &desktop);

			// The top left corner will have coordinates (0, 0) and the bottom right corner will have coordinates (horizontal, vertical)
			Configuration::get().set_max_width(desktop.right);
			Configuration::get().set_max_height(desktop.bottom);

			// Get the state of the Caps Lock key
			int nVirtKey = GetKeyState(20);

			Configuration::get().set_caps_lock_enabled(nVirtKey);
		}
#else
		// POSIX / Android: there is no desktop window. The SDL window layer
		// (IO/Window.cpp) overrides max width/height with the real display
		// mode after initialization, so only a sane placeholder is set here.
		// Caps Lock state is not available without a keyboard-state API.
		ScreenResolution()
		{
			Configuration::get().set_max_width(1920);
			Configuration::get().set_max_height(1080);
			Configuration::get().set_caps_lock_enabled(false);
		}
#endif
	};
}