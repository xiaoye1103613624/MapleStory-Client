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

#include "../Constants.h"
#include "../Gameplay/MapleMap/MapBackgrounds.h"
#include "../Gameplay/MapleMap/MapTilesObjs.h"
#include "../Graphics/Sprite.h"

#include <chrono>
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
		void set_step(int value);

		int get_step() const
		{
			return step;
		}

		Point<int16_t> get_frame_topleft() const
		{
			return frame_topleft;
		}

		Point<int16_t> get_viewpos() const;

		// 目标步进的视角位置。构造登录窗口时补间可能刚开始，
		// 必须用端点而不是当前插值位置，否则窗口会落在半途。
		Point<int16_t> get_target_viewpos() const
		{
			Point<int16_t> center(
				Constants::Constants::get().get_viewwidth() / 2,
				Constants::Constants::get().get_viewheight() / 2
			);
			Offset offset = target_offset(step);

			return Point<int16_t>(
				static_cast<int16_t>(center.x() - offset.x),
				static_cast<int16_t>(center.y() - offset.y)
			);
		}

		// 参考端 CLogin::OnStepChanged 只在补间结束后切换窗口；
		// 登录态据此延迟激活新窗口，避免新窗口提前悬在终点。
		bool is_transitioning() const
		{
			return tween_active;
		}

		void draw(float alpha) const;
		void update();

	private:
		static int step_height(int step);

		// 屏幕空间偏移：viewpos = 屏幕中心 - offset。
		// 参考端 CLogin::ChangeStep 用 IWzGr2D 中心补间在旧/新 step 高度之间滚动，
		// 时长 300*|new-old|+500 ms；这里沿用同一时长与端点，缓动取平滑 S 曲线。
		struct Offset
		{
			double x;
			double y;
		};

		static Offset target_offset(int step);
		static uint32_t now_ms();
		Offset current_offset() const;

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

		Offset tween_from;
		Offset tween_to;
		uint32_t tween_start;
		uint32_t tween_duration;
		bool tween_active;
	};
}
