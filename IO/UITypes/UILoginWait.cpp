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
//	You should have received a copy of the GNU Affero General Public License		//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#include "UILoginWait.h"

#include "UISoftKey.h"

#include "../UI.h"
#include "../LoginBackground.h"
#include "../Components/MapleButton.h"

#include "../../Net/Session.h"
#include "../../Graphics/Texture.h"
#include "../../Constants.h"

#include <algorithm>
#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// 选一个真正「多帧」的动效节点：bar / circle / ani / effect。
		nl::node pick_loading_anim(nl::node Loading)
		{
			nl::node bar = Loading["bar"];

			if (bar)
			{
				// bar 本身是帧容器，或 bar/0,1,2…
				if (bar["0"] || bar["1"] || bar.data_type() == nl::node::type::bitmap)
					return bar;
			}

			nl::node circle = Loading["circle"];

			if (circle)
				return circle;

			nl::node ani = Loading["ani"] ? Loading["ani"] : Loading["effect"];

			if (ani)
				return ani;

			return bar;
		}

		// 帧延时：WZ 缺省 100ms（与 Animation::read_frame_timing 一致）。
		uint16_t frame_delay(nl::node src)
		{
			uint16_t delay = src["delay"];

			return delay == 0 ? 100 : delay;
		}
	}

	UILoginWait::UILoginWait() : UILoginWait([]() {}) {}

	UILoginWait::UILoginWait(std::function<void()> okhandler) : okhandler(okhandler)
	{
		nl::node Notice = nl::nx::UI["Login.img"]["Notice"];
		nl::node Loading = Notice["Loading"];
		nl::node load_bg = Loading["backgrnd"];
		const bool bedou = LoginBackground::get().available();

		Point<int16_t> content_origin(0, 0);
		Point<int16_t> load_dim(0, 0);

		if (bedou && load_bg)
		{
			// Loading/backgrnd 已是完整卷轴；勿再叠 Notice/backgrnd。
			// 保持原尺寸居中（参考端即为窗口内居中的卷轴弹窗，非全屏）。
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

		// 进度条：bar/0..10 共 11 帧，每帧多点亮一格（0%..100%）。
		// 2026-11-05 正确端截图（XiaoYe_2026-11-05_00-29-43.png）逐像素定标：
		// 弹窗=Loading/backgrnd 282x144（模板匹配原点=截图(0,9)）；条带 y88..113、
		// x46..253 无内槽；截图仅见第 0 段 8x8 橙块 @ 弹窗内(130..141,98..105)
		// （左侧条带纯白排除 bar@100 段3 的解释）⇒ bar 画在弹窗内 (131,98)。
		static const Point<int16_t> kBeiDouBarOff(131, 98);

		nl::node progress = pick_loading_anim(Loading);

		if (progress)
		{
			std::vector<nl::node> frames;

			if (progress.data_type() == nl::node::type::bitmap)
			{
				frames.push_back(progress);
			}
			else
			{
				for (int i = 0; i < 64; ++i)
				{
					nl::node f = progress[std::to_string(i)];

					if (!f)
						break;

					frames.push_back(f);
				}
			}

			for (nl::node f : frames)
			{
				BarFrame bar_frame;
				bar_frame.texture = Texture(f);
				bar_frame.delay = frame_delay(f);
				bar_frames.push_back(std::move(bar_frame));
			}
		}

		if (!bar_frames.empty())
		{
			const int16_t bar_w = bar_frames.front().texture.width();
			const int16_t bar_h = bar_frames.front().texture.height();

			if (bedou)
			{
				bar_pos = content_origin + kBeiDouBarOff;
			}
			else
			{
				// 非 BeiDou 素材：白槽底部居中兜底。
				bar_pos = content_origin + Point<int16_t>(
					static_cast<int16_t>(std::max(0, (load_dim.x() - bar_w) / 2)),
					static_cast<int16_t>(std::max(0, load_dim.y() - bar_h - 32))
				);
			}

			for (const BarFrame& bar_frame : bar_frames)
				fill_total = static_cast<uint16_t>(fill_total + bar_frame.delay);
		}

		nl::node bt_cancel = Loading["BtCancel"];

		if (bt_cancel)
		{
			Texture cancel_tex(bt_cancel["normal"]["0"]);
			Point<int16_t> cancel_dim = cancel_tex.get_dimensions();

			int16_t cancel_y;
			if (bedou)
			{
				// 2026-11-05 正确端截图逐像素定标：取消钮 65x23 在弹窗内 (188,44)，
				// 与「正在连接到服务器。」文字同行、右缘距弹窗右缘 29px。
				// 旧实现坠到卷轴底部居中（y=113），即用户所见错位。
				cancel_y = 44;
				dimension = load_dim;
			}
			else
			{
				cancel_y = static_cast<int16_t>(bar_pos.y() + 14);
				const int16_t max_y = static_cast<int16_t>(
					std::max(0, dimension.y() - cancel_dim.y() - 10));
				if (cancel_y > max_y)
					cancel_y = max_y;
			}

			Point<int16_t> cancel_pos(
				bedou
					? static_cast<int16_t>(dimension.x() - cancel_dim.x() - 29)
					: static_cast<int16_t>((dimension.x() - cancel_dim.x()) / 2),
				cancel_y
			);
			buttons[Buttons::BtCancel] = std::make_unique<MapleButton>(bt_cancel, cancel_pos);
		}

		center_on_screen();
	}

	void UILoginWait::update_screen(int16_t newwidth, int16_t newheight)
	{
		center_on_screen();
	}

	UIElement::Type UILoginWait::get_type() const
	{
		return TYPE;
	}

	void UILoginWait::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) { close(); return; }
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
			|| keycode == 'C' || keycode == 'c' || keycode == 'X' || keycode == 'x')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				if (t == "c" || t == "cancel" || t == "close" || t == "x" || t == "n")
					close();
			}, []() {}, "cancel");
		}
	}

	void UILoginWait::update()
	{
		UIElement::update();

		if (finished || bar_frames.empty())
			return;

		// 进度条按帧延时往前走；满条后停在 100%，等 begin_finish 收尾。
		if (finish_pending)
		{
			// 收尾阶段：参考端满条后才切页，未满则补满到 100%。
			if (fill_elapsed < fill_total)
			{
				fill_elapsed = static_cast<uint16_t>(
					std::min<uint16_t>(fill_elapsed + Constants::TIMESTEP, fill_total)
				);
			}

			// 服务端已响应且进度条已满 → 收尾跳转（只执行一次）。
			if (fill_elapsed >= fill_total)
			{
				finished = true;
				deactivate();

				if (oncomplete)
					oncomplete();
			}
		}
		else
		{
			// 等待阶段：参考端 LOADING 为「循环增长」的不确定进度动画，
			// 满条后回到 0 重新长；旧逻辑只长一次并停在 100%，服务端未响应时
			// 表现为进度条卡住不动（用户反馈"不是循环增长，会卡一会"）。
			uint16_t next = static_cast<uint16_t>(fill_elapsed + Constants::TIMESTEP);

			if (next >= fill_total)
				next = static_cast<uint16_t>(next - fill_total);

			fill_elapsed = next;
		}
	}

	void UILoginWait::draw(float inter) const
	{
		UIElement::draw(inter);

		if (bar_frames.empty())
			return;

		// 帧索引 = 已播放时长落在哪一帧的延时区间内。
		size_t idx = bar_frames.size() - 1;
		uint16_t acc = 0;

		for (size_t i = 0; i < bar_frames.size(); ++i)
		{
			acc = static_cast<uint16_t>(acc + bar_frames[i].delay);

			if (fill_elapsed < acc)
			{
				idx = i;
				break;
			}
		}

		bar_frames[idx].texture.draw(position + bar_pos);
	}

	void UILoginWait::begin_finish(std::function<void()> oncomplete_)
	{
		if (finished || bar_frames.empty())
		{
			// 已收尾或无进度条素材：立即执行续场逻辑（退化旧行为）。
			if (oncomplete_)
				oncomplete_();

			return;
		}

		oncomplete = std::move(oncomplete_);
		finish_pending = true;
	}

	void UILoginWait::close()
	{
		// 取消：放弃续场逻辑，立即关窗，避免满条后仍执行被取消的跳转。
		finish_pending = false;
		oncomplete = nullptr;

		deactivate();
		okhandler();
	}

	std::function<void()> UILoginWait::get_handler()
	{
		return okhandler;
	}

	Button::State UILoginWait::button_pressed(uint16_t id)
	{
		Session::get().reconnect();
		close();
		return Button::State::NORMAL;
	}
}
