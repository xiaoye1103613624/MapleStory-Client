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

#include "Template/Singleton.h"

#include <cstdint>

namespace ms
{
	namespace Constants
	{
		// Timestep, i.e., the granularity in which the game advances.
		constexpr uint16_t TIMESTEP = 8;

		class Constants : public Singleton<Constants>
		{
		public:
			Constants()
			{
				// 默认分辨率 1280x720：与 BeiDou 登录 UI 素材的原生尺寸一致
				// （UI/Login.img/Common/frame 即 1280x720），书框可 1:1 绘制，
				// 书页开口约占窗口 85%x81%，即参考客户端的观感。
				VIEWWIDTH = 1280;
				VIEWHEIGHT = 720;
			};

			~Constants() {}

			// Get the window and screen width
			int16_t get_viewwidth()
			{
				return VIEWWIDTH;
			}

			// Set the window and screen width
			void set_viewwidth(int16_t width)
			{
				VIEWWIDTH = width;
			}

			// Get the window and screen height
			int16_t get_viewheight()
			{
				return VIEWHEIGHT;
			}

			// Set the window and screen height
			void set_viewheight(int16_t height)
			{
				VIEWHEIGHT = height;
			}

		private:
			// Window and screen width
			int16_t VIEWWIDTH;
			// Window and screen height
			int16_t VIEWHEIGHT;
		};
	}
}