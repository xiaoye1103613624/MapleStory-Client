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

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	LoginBackground::LoginBackground() : enabled(false), has_map(false), has_backs(false), has_forest(false), has_frame(false), step(0), frame_topleft(0, 0)
	{
	}

	int LoginBackground::step_height(int step)
	{
		// 对齐原客户端 CLogin::GetStepHeight：-8 - 600 * step
		// （其中 step==4 时另有 *(this+133) 偏移，登录流程用不到）。
		return -8 - 600 * step;
	}

	void LoginBackground::init()
	{
#ifdef USE_NX
		nl::node frameNode = nl::nx::UI["Login.img"]["Common"]["frame"];
		nl::node mapNode = nl::nx::UI["MapLogin.img"];

		nl::node titleNew = nl::nx::UI["Login.img"]["Title_new"];

		if (!titleNew)
			titleNew = nl::nx::UI["Login.img"]["Title"];

		// BeiDou 判定：Title 无 backgrd 贴图，且同时具备 Common/frame 与 MapLogin 地图。
		bool bei = !titleNew["backgrd"] && frameNode && mapNode;

		if (!bei)
			return;

		int16_t vieww = Constants::Constants::get().get_viewwidth();
		int16_t viewh = Constants::Constants::get().get_viewheight();

		// 北斗登录远景：完整森林背景是 Map001/Back/login.img/back/11（一张很高的纵向
		// 拼图，登录木牌在底部，登录后选频道/选角色/创建角色依次上移）。back/12、back/13
		// 为同套背景的额外景深层（若存在）。直接作为纵向滚动背景绘制。
		nl::node backSet = nl::nx::Map001["Back"]["login.img"]["back"];
		if (backSet)
		{
			// 从 11 开始按序加载景深层（11 最远，13 最近）。
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

			// 登录地图自带的 back 层（与主地图同一套 MapBackgrounds 渲染：
			// 内含 rx/ry 视差、tiling、ani，且视差基准是 VWIDTH/2 / VHEIGHT/2），
			// 因此各 step 共用同一相机、任意分辨率都自洽。
			// 优先使用它；缺失时才退回下面手搓的森林层。
			if (mapNode["back"]["0"])
			{
				backgrounds = MapBackgrounds(mapNode["back"]);
				has_backs = true;
			}
		}

		if (frameNode)
		{
			Texture frame_tex(frameNode);
			Point<int16_t> origin = frame_tex.get_origin();

			// frame 为 1280x720，与 1024x768 窗口比例不符：拉伸铺满整屏。
			frame = Sprite(frameNode, DrawArgument(
				origin,
				Point<int16_t>(vieww, viewh)
			));
			// 供各界面按同一基准摆放控件：以 800x600 -> 当前分辨率 的居中偏移。
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

		if (step == 0)
			// 登录页把告示牌居中：告示牌 obj pos = (137, 21)。
			// 相对原 800x600 上调 +40，避免登录面板整体偏上压住书页标题区。
			return center - Point<int16_t>(137, -20);

		return center - Point<int16_t>(28, step_height(step));
	}

	void LoginBackground::draw(float alpha) const
	{
		if (!enabled)
			return;

			Point<int16_t> viewpos = get_viewpos();

		// 优先走地图自带的 back 层：与地图物件共用同一相机（viewpos），
		// 因此换 step 或换分辨率都不会与地图物件产生相对漂移。
		if (has_backs)
			backgrounds.drawbackgrounds(viewpos.x(), viewpos.y(), alpha);
		else if (has_forest)
		{
			int16_t vieww = Constants::Constants::get().get_viewwidth();
			int16_t viewh = Constants::Constants::get().get_viewheight();

			// 背景大图很高：登录场景在底部，逐 step 上移（与地图相机 8+600*step 一致）。
			int32_t scroll = 8 + 600 * step;

			for (const auto& sprite : bg_layers)
			{
				Point<int16_t> dim = sprite.get_dimensions();
				Point<int16_t> origin = sprite.get_origin();

				int16_t posx = (vieww - dim.x()) / 2;
				int16_t posy = (viewh - dim.y()) - static_cast<int16_t>(scroll);

				// Sprite::draw 以 origin 为锚点，故传入 topleft + origin。
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

		// 前景 back 层（front=1）画在地图物件之后。
		if (has_backs)
			backgrounds.drawforegrounds(viewpos.x(), viewpos.y(), alpha);

		if (has_frame)
			frame.draw(Point<int16_t>(0, 0), alpha);
	}

	void LoginBackground::update()
	{
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
