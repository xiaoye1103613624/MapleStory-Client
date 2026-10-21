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
#include "UILoginWait.h"

#include "../UI.h"
#include "../LoginBackground.h"
#include "../Components/MapleButton.h"

#include "../../Net/Session.h"

#include <algorithm>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// BeiDou Loading/backgrnd(282×144) 内白色槽实测：LOADING 字形约 (53,98)-(94,105)，
	// 槽区约 x46 起；bar 帧 109×8，原点脏值由 Texture sanitize 清零。
	static constexpr Point<int16_t> kBeiDouBarOff(46, 98);

	UILoginWait::UILoginWait() : UILoginWait([]() {}) {}

	UILoginWait::UILoginWait(std::function<void()> okhandler) : okhandler(okhandler)
	{
		nl::node Notice = nl::nx::UI["Login.img"]["Notice"];
		nl::node Loading = Notice["Loading"];
		nl::node load_bg = Loading["backgrnd"];
		const bool bedou = LoginBackground::get().available();

		Point<int16_t> content_origin(0, 0);
		Point<int16_t> load_dim(0, 0);
		Point<int16_t> progress_pos(0, 0);
		bool have_progress = false;

		if (bedou && load_bg)
		{
			// Loading/backgrnd 已是完整卷轴（文案+LOADING 槽）。再叠 Notice/backgrnd
			// 会变成「两个重叠弹窗」。
			sprites.emplace_back(load_bg);
			load_dim = Texture(load_bg).get_dimensions();
			dimension = load_dim;
		}
		else
		{
			nl::node frame = Notice["backgrnd"]["0"];

			if (frame)
			{
				sprites.emplace_back(frame);
				dimension = Texture(frame).get_dimensions();

				if (load_bg)
				{
					load_dim = Texture(load_bg).get_dimensions();
					content_origin = Point<int16_t>(
						static_cast<int16_t>(std::max(0, (dimension.x() - load_dim.x()) / 2)),
						static_cast<int16_t>(std::max(8, (dimension.y() - load_dim.y()) / 2 - 20))
					);
					sprites.emplace_back(load_bg, content_origin);
				}
			}
			else if (load_bg)
			{
				sprites.emplace_back(load_bg);
				dimension = Texture(load_bg).get_dimensions();
				load_dim = dimension;
			}
		}

		nl::node progress = Loading["bar"] ? Loading["bar"] : Loading["circle"];

		if (progress)
		{
			if (load_dim.x() <= 0)
				load_dim = load_bg ? Texture(load_bg).get_dimensions() : dimension;

			if (bedou)
			{
				progress_pos = content_origin + kBeiDouBarOff;
			}
			else
			{
				constexpr int16_t bar_w = 109;
				progress_pos = Point<int16_t>(
					static_cast<int16_t>(content_origin.x() + std::max(0, (load_dim.x() - bar_w) / 2)),
					static_cast<int16_t>(content_origin.y() + std::max(0, load_dim.y() - 52))
				);
			}

			sprites.emplace_back(progress, progress_pos);
			have_progress = true;
		}

		nl::node bt_cancel = Loading["BtCancel"];

		if (bt_cancel)
		{
			Texture cancel_tex(bt_cancel["normal"]["0"]);
			Point<int16_t> cancel_dim = cancel_tex.get_dimensions();

			int16_t cancel_y;
			if (bedou)
			{
				// 取消键放在卷轴下方，避免压住 LOADING / bar。
				cancel_y = static_cast<int16_t>(load_dim.y() + 2);
				dimension = Point<int16_t>(
					std::max(dimension.x(), load_dim.x()),
					static_cast<int16_t>(cancel_y + cancel_dim.y())
				);
			}
			else if (have_progress)
			{
				cancel_y = static_cast<int16_t>(progress_pos.y() + 14);
			}
			else if (Notice["backgrnd"]["0"])
			{
				cancel_y = static_cast<int16_t>(content_origin.y() + load_dim.y() + 4);
			}
			else
			{
				cancel_y = static_cast<int16_t>(70);
			}

			if (!bedou)
			{
				const int16_t max_y = static_cast<int16_t>(
					std::max(0, dimension.y() - cancel_dim.y() - 10));
				if (cancel_y > max_y)
					cancel_y = max_y;
			}

			Point<int16_t> cancel_pos(
				static_cast<int16_t>((dimension.x() - cancel_dim.x()) / 2),
				cancel_y
			);
			buttons[Buttons::BtCancel] = std::make_unique<MapleButton>(bt_cancel, cancel_pos);
		}

		center_on_screen();
	}

	void UILoginWait::update_screen(int16_t, int16_t)
	{
		center_on_screen();
	}

	UIElement::Type UILoginWait::get_type() const
	{
		return TYPE;
	}

	void UILoginWait::close()
	{
		// 不可在 button_pressed 栈内 remove(this)：会销毁自身后仍写按钮状态。
		// deactivate + 恢复输入即可；UIStateLogin::send_cursor 已对失效焦点回落前台。
		// 下次 emplace<UILoginWait> / 显式 remove 会清掉残留实例。
		deactivate();
		okhandler();
	}

	std::function<void()> UILoginWait::get_handler()
	{
		return okhandler;
	}

	Button::State UILoginWait::button_pressed(uint16_t id)
	{
		// 中止本次登录握手；失败也继续关窗，避免卡在 Wait。
		Session::get().reconnect();

		close();

		return Button::State::NORMAL;
	}
}
