//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.										//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the			//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#include "LoginBackground.h"

#include "../Constants.h"
#include "../Graphics/Texture.h"

#include <cmath>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	LoginBackground::LoginBackground() : enabled(false), has_map(false), has_backs(false), has_forest(false), has_frame(false), step(0), frame_topleft(0, 0), tween_from{0.0, 0.0}, tween_to{0.0, 0.0}, tween_start(0), tween_duration(0), tween_active(false)
	{
	}

	int LoginBackground::step_height(int step)
	{
		// Align with CLogin::GetStepHeight: -8 - 600 * step
		return -8 - 600 * step;
	}

	LoginBackground::Offset LoginBackground::target_offset(int step)
	{
		// Step 5 (other/exit) uses step 1's height in CLogin::ChangeStep.
		if (step == 5)
			step = 1;

		if (step == 0)
			// 登录页沿用既有锚点：告示牌中心对齐书本开口。
			return { 137.0, -20.0 };

		return { 28.0, static_cast<double>(step_height(step)) };
	}

	uint32_t LoginBackground::now_ms()
	{
		using namespace std::chrono;
		return static_cast<uint32_t>(
			duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count()
		);
	}

	LoginBackground::Offset LoginBackground::current_offset() const
	{
		if (!tween_active || tween_duration == 0)
			return target_offset(step);

		uint32_t elapsed = now_ms() - tween_start;

		if (elapsed >= tween_duration)
			return tween_to;

		double t = static_cast<double>(elapsed) / static_cast<double>(tween_duration);
		// 平滑 S 曲线（两端速度为 0），与参考端滚动补间的观感一致。
		t = t * t * (3.0 - 2.0 * t);

		return {
			tween_from.x + (tween_to.x - tween_from.x) * t,
			tween_from.y + (tween_to.y - tween_from.y) * t
		};
	}

	void LoginBackground::set_step(int value)
	{
		if (value == step)
			return;

		Offset from = current_offset();
		int previous = step;

		step = value;
		tween_from = from;
		tween_to = target_offset(value);
		tween_start = now_ms();
		tween_duration = static_cast<uint32_t>(300 * (value > previous ? value - previous : previous - value) + 500);
		tween_active = true;
	}

#ifdef USE_NX
	// Official book frame: UI/Login.img/Common/frame (1280x720).
	// Opening ~1091x581 (~85%x81% of window) so the login map fills the aperture 1:1.
	//
	// MapleEzorsiaV2wzfiles.img/Common/frameNNNN is resolution-named but its opening
	// is only ~611x461 (~60%x60%); use it only when the official node is missing,
	// and only one frame (never stack).
	static nl::node pick_login_frame(int16_t vieww, int16_t viewh)
	{
		nl::node official = nl::nx::UI["Login.img"]["Common"]["frame"];

		if (official)
			return official;

		nl::node common = nl::nx::UI["MapleEzorsiaV2wzfiles.img"]["Common"];

		if (common)
		{
			const char* exact = nullptr;

			if (vieww == 1280 && viewh == 720)
				exact = "frame1280";
			else if (vieww == 1024 && viewh == 768)
				exact = "frame1024";
			else if (vieww == 1366 && viewh == 768)
				exact = "frame1366";
			else if (vieww == 1600 && viewh == 900)
				exact = "frame1600";
			else if (vieww == 1920 && viewh == 1080)
				exact = "frame1920";

			if (exact)
			{
				nl::node node = common[exact];

				if (node)
					return node;
			}
		}

		return {};
	}
#endif

	void LoginBackground::init()
	{
#ifdef USE_NX
		nl::node loginFrame = nl::nx::UI["Login.img"]["Common"]["frame"];
		nl::node mapNode = nl::nx::UI["MapLogin.img"];

		nl::node titleNew = nl::nx::UI["Login.img"]["Title_new"];

		if (!titleNew)
			titleNew = nl::nx::UI["Login.img"]["Title"];

		// BeiDou: Title has no backgrd, and login frame + MapLogin exist.
		bool bei = !titleNew["backgrd"] && loginFrame && mapNode;

		if (!bei)
			return;

		int16_t vieww = Constants::Constants::get().get_viewwidth();
		int16_t viewh = Constants::Constants::get().get_viewheight();

		// Forest far layers (Map001/Back/login.img/back/11..13) as fallback.
		nl::node backSet = nl::nx::Map001["Back"]["login.img"]["back"];
		if (backSet)
		{
			for (int16_t no = 11; no <= 13; no++)
			{
				nl::node layer = backSet[std::to_string(no)];

				if (layer)
					bg_layers.emplace_back(layer);
			}

			has_forest = !bg_layers.empty();
		}

		if (mapNode)
		{
			map = MapTilesObjs(mapNode);
			has_map = true;

			// Prefer MapLogin.img/back (same camera as map objects).
			if (mapNode["back"]["0"])
			{
				backgrounds = MapBackgrounds(mapNode["back"]);
				has_backs = true;
			}
		}

		// Draw exactly one frame. Sprite rect is always (0,0)-(vieww,viewh)
		// (Texture::draw get_rectangle); at 1280x720 the bitmap is 1:1.
		nl::node frameNode = pick_login_frame(vieww, viewh);

		if (frameNode)
		{
			Texture frame_tex(frameNode);
			Point<int16_t> origin = frame_tex.get_origin();

			frame = Sprite(frameNode, DrawArgument(
				origin,
				Point<int16_t>(vieww, viewh)
			));
			frame_topleft = Point<int16_t>(vieww / 2, viewh / 2) - origin;
			has_frame = true;
		}

		enabled = true;
#endif
}


	Point<int16_t> LoginBackground::get_viewpos() const
	{
		Point<int16_t> center(
			Constants::Constants::get().get_viewwidth() / 2,
			Constants::Constants::get().get_viewheight() / 2
		);

		Offset offset = current_offset();

		return Point<int16_t>(
			static_cast<int16_t>(std::lround(center.x() - offset.x)),
			static_cast<int16_t>(std::lround(center.y() - offset.y))
		);
	}

	void LoginBackground::draw(float alpha) const
	{
		if (!enabled)
			return;

		Point<int16_t> viewpos = get_viewpos();

		if (has_backs)
			backgrounds.drawbackgrounds(viewpos.x(), viewpos.y(), alpha);
		else if (has_forest)
		{
			int16_t vieww = Constants::Constants::get().get_viewwidth();
			int16_t viewh = Constants::Constants::get().get_viewheight();

			int32_t scroll = 8 + 600 * step;

			for (const auto& sprite : bg_layers)
			{
				Point<int16_t> dim = sprite.get_dimensions();
				Point<int16_t> origin = sprite.get_origin();

				int16_t posx = (vieww - dim.x()) / 2;
				int16_t posy = (viewh - dim.y()) - static_cast<int16_t>(scroll);

				sprite.draw(Point<int16_t>(posx + origin.x(), posy + origin.y()), alpha);
			}
		}

		if (has_map)
		{
			static const Layer::Id BG_LAYERS[] =
			{
				Layer::ZERO, Layer::ONE, Layer::TWO, Layer::THREE,
				Layer::FOUR, Layer::FIVE, Layer::SIX, Layer::SEVEN
			};

			for (Layer::Id layer : BG_LAYERS)
				map.draw(layer, viewpos, alpha);
		}

		if (has_backs)
			backgrounds.drawforegrounds(viewpos.x(), viewpos.y(), alpha);

		// Draw the single selected border exactly once.
		if (has_frame)
			frame.draw(Point<int16_t>(0, 0), alpha);
	}

	void LoginBackground::update()
	{
		if (tween_active && now_ms() - tween_start >= tween_duration)
			tween_active = false;

		if (has_backs)
			backgrounds.update();

		for (auto& sprite : bg_layers)
			sprite.update();

		if (has_map)
			map.update();

		if (has_frame)
			frame.update();
	}
}
