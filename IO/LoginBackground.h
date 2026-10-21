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
	// BeiDou login background: MapLogin map + one book border.
	// Prefer official UI/Login.img/Common/frame (full-window opening at 1280x720).
	// Fallback to one MapleEzorsiaV2wzfiles.img/Common/frameNNNN only if missing;
	// never stack multiple frames (causes double book pages).
	class LoginBackground : public Singleton<LoginBackground>
	{
	public:
		LoginBackground();

		// Load map + single border; only when BeiDou-style data is present.
		void init();

		bool available() const
		{
			return enabled;
		}

		// step: 0=login, 1=world select, 2=char select (camera -8-600*step)
		void set_step(int value)
		{
			step = value;
		}

		int get_step() const
		{
			return step;
		}

		Point<int16_t> get_frame_topleft() const
		{
			return frame_topleft;
		}

		Point<int16_t> get_viewpos() const;

		void draw(float alpha) const;
		void update();

	private:
		static int step_height(int step);

		MapTilesObjs map;
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
