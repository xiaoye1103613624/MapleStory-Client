////////////////////////////////////////////////////////////////////////////////
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
//	You should have received a copy of the GNU Affero General Public License		//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Template/Singleton.h"

#include "../Gameplay/MapleMap/MapBackgrounds.h"
#include "../Gameplay/MapleMap/MapTilesObjs.h"
#include "../Graphics/Sprite.h"

#include <vector>

namespace ms
{
	// BeiDou 原生登录背景：远景森林（Map001/Back/login.img）+ 纵向地图
	// （UI/MapLogin.img）+ 书页外框（UI/Login.img/Common/frame）。
	// 登录流程（登录/大区选择/角色选择）共用同一套背景，仅通过 step 改变相机。
	class LoginBackground : public Singleton<LoginBackground>
	{
	public:
		LoginBackground();

		// 载入地图与外框；仅当数据为 BeiDou 风格时才启用。
		void init();

		bool available() const
		{
			return enabled;
		}

		// step: 0=登录, 1=大区选择, 2=角色选择（地图相机高度 -8-600*step）
		void set_step(int value)
		{
			step = value;
		}

		int get_step() const
		{
			return step;
		}

		// 外框左上角（供各界面按同一基准摆放控件）
		Point<int16_t> get_frame_topleft() const
		{
			return frame_topleft;
		}

		// 当前 step 的地图相机偏移。
		Point<int16_t> get_viewpos() const;

		void draw(float alpha) const;
		void update();

	private:
		static int step_height(int step);

		MapTilesObjs map;
		// 登录地图自己的 back 层（含 rx/ry 视差、tiling、动画）。
		// 用它取代手搓的森林层，天然与地图同相机、且随分辨率自适应。
		MapBackgrounds backgrounds;
		std::vector<Sprite> bg_layers;
		Sprite frame;
		Point<int16_t> frame_topleft;
		bool enabled;
		bool has_map;
		bool has_backs;
		bool has_forest;
		bool has_frame;
		int step;
	};
}
