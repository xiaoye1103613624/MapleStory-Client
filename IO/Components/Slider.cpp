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
#include "Slider.h"

#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	Slider::Slider(int32_t t, Range<int16_t> ver, int16_t xp, int16_t ur, int16_t rm, std::function<void(bool)> om) : type(t), vertical(ver), x(xp), onmoved(om)
	{
		start = Point<int16_t>(x, vertical.first());
		end = Point<int16_t>(x, vertical.second());

		nl::node src;
		std::string base_str = "base";

		if (type == Type::CHATBAR)
		{
			nl::node status_root = UiAssetResolve::status_bar_root();
			src = status_root["chat"]["common"]["scroll"];

			// v83: chat scroll lives under Basic.img VScr (StatusBar.img has no chat/common).
			if (!UiAssetResolve::usable(src))
				src = nl::nx::UI["Basic.img"]["VScr5"];

			if (!UiAssetResolve::usable(src))
				src = nl::nx::UI["Basic.img"]["VScr"];

			if (UiAssetResolve::usable(status_root["chat"]["common"]["scroll"]))
				base_str += "_c";
		}
		else
		{
			std::string VScr = "VScr";

			if (type != Type::LINE_CYAN)
				VScr += std::to_string(type);

			src = nl::nx::UI["Basic.img"][VScr];

			// The enum was copied from a newer asset generation: DEFAULT_SILVER is 9,
			// but this pack only ships VScr..VScr8, so every scrollbar used to resolve
			// to an empty node and disappear.
			// 兜底根由 VScr 改成 VScr5：背包栏（折叠）.png / 任务窗 截图里，原生滚动条是
			// 「白框灰底圆角箭头钮 + 实心浅灰轨道」（VScr5 的 prev0/base 逐块比对一致），
			// 而 VScr 的 base 是一条虚线轨道，属于另一代资产。聊天栏早已单独用 VScr5。
			if (!UiAssetResolve::usable(src))
			{
				UiAssetResolve::diag_once("slider " + VScr, "not in Basic.img, clamped to VScr5");
				src = nl::nx::UI["Basic.img"]["VScr5"];
			}
		}

		nl::node dsrc = src["disabled"];

		dbase = dsrc[base_str];

		dprev = dsrc["prev"];
		dnext = dsrc["next"];

		nl::node esrc = src["enabled"];

		base = esrc[base_str];

		prev = TwoSpriteButton(esrc["prev0"], esrc["prev1"], start);
		next = TwoSpriteButton(esrc["next0"], esrc["next1"], end);

		thumb = TwoSpriteButton(esrc["thumb0"], esrc["thumb1"]);

		// 命中宽度取滑块位图的真实宽度：VScr5 的 thumb 是 15x25，原先写死 8，
		// 于是右半块按不住（表现为"滑块点不动"）。缺资源时退回 8 保持旧行为。
		// 注意 nl::node 没有 get_dimensions()（那是 Texture 的接口），位图尺寸走 get_bitmap()。
		thumbwidth = static_cast<int16_t>(esrc["thumb0"].get_bitmap().width());
		if (thumbwidth <= 0)
			thumbwidth = 8;

		buttonheight = dnext.get_dimensions().y();

		setrows(ur, rm);

		enabled = true;
		scrolling = false;
	}

	Slider::Slider() : Slider(0, {}, 0, 0, 0, {}) {}

	bool Slider::isenabled() const
	{
		return enabled;
	}

	void Slider::setenabled(bool en)
	{
		enabled = en;
	}

	void Slider::setrows(int16_t nr, int16_t ur, int16_t rm)
	{
		rowmax = rm - ur;
		recalc_rowheight();
		row = nr;
	}

	void Slider::recalc_rowheight()
	{
		if (rowmax > 0)
			rowheight = (vertical.length() - buttonheight * 2) / rowmax;
		else
			rowheight = 0;

		// 可滚但步长被整除成 0 时，原先 rowheight==0 会被 draw() 当成"没得滚"，
		// 于是长列表（行数 > 轨道像素）滑块直接消失，只剩两个灰箭头，而且
		// send_cursor 里 yoffset / rowheight 会除零。列表越长越要看得见滑块，
		// 所以这里保底 1px：滑块贴边逐行挪，不再整块不见。
		if (rowmax > 0 && rowheight < 1)
			rowheight = 1;
	}

	void Slider::setrows(int16_t ur, int16_t rm)
	{
		setrows(0, ur, rm);
	}

	void Slider::setvertical(Range<int16_t> ver)
	{
		vertical = ver;
		start = Point<int16_t>(x, vertical.first());
		end = Point<int16_t>(x, vertical.second());
		prev.set_position(start);
		next.set_position(end);

		recalc_rowheight();
	}

	Range<int16_t> Slider::getvertical() const
	{
		return vertical;
	}

	void Slider::draw(Point<int16_t> position) const
	{
		Point<int16_t> base_pos = position + start;
		Point<int16_t> fill = Point<int16_t>(0, vertical.length() + buttonheight - 2);
		DrawArgument base_arg = DrawArgument(Point<int16_t>(base_pos.x(), base_pos.y() + 1), fill);

		int16_t height = dbase.height();

		// 防呆：资源缺失时 dbase 高度可能为 0，原先的 while 会永不前进
		// 形成死循环（技能书 Slider 的 VScr 缺 disabled/base 时即触发），
		// 表现为进图/开技能书后主线程 100% CPU、窗口未响应。高度非正直接跳过。
		if (height > 0)
		{
			int16_t maxheight = vertical.first() + height;

			while (maxheight < vertical.second())
			{
				dbase.draw(position + Point<int16_t>(start.x(), maxheight));

				maxheight += height;
			}
		}

		if (enabled)
		{
			if (rowheight > 0)
			{
				prev.draw(position);
				next.draw(position);
				thumb.draw(position + getthumbpos());
			}
			else
			{
				dprev.draw(position + start);
				dnext.draw(position + end);
			}
		}
		else
		{
			dprev.draw(position + start);
			dnext.draw(position + end);
		}
	}

	void Slider::remove_cursor()
	{
		scrolling = false;

		thumb.set_state(Button::State::NORMAL);
		next.set_state(Button::State::NORMAL);
		prev.set_state(Button::State::NORMAL);
	}

	Point<int16_t> Slider::getthumbpos() const
	{
		int16_t y =
			row < rowmax ?
			vertical.first() + row * rowheight + buttonheight :
			vertical.second() - buttonheight * 2 - 2;

		return Point<int16_t>(x, y);
	}

	Cursor::State Slider::send_cursor(Point<int16_t> cursor, bool pressed)
	{
		Point<int16_t> relative = cursor - start;

		if (scrolling)
		{
			if (pressed)
			{
				int16_t thumby = row * rowheight + buttonheight * 2;
				int16_t delta = relative.y() - thumby;

				if (delta > rowheight / 2 && row < rowmax)
				{
					row++;
					onmoved(false);
				}
				else if (delta < -rowheight / 2 && row > 0)
				{
					row--;
					onmoved(true);
				}

				return Cursor::State::VSCROLLIDLE;
			}
			else
			{
				scrolling = false;
			}
		}
		else if (relative.x() < 0 || relative.y() < 0 || relative.x() > thumbwidth || relative.y() > vertical.second())
		{
			thumb.set_state(Button::State::NORMAL);
			next.set_state(Button::State::NORMAL);
			prev.set_state(Button::State::NORMAL);

			return Cursor::State::IDLE;
		}

		Point<int16_t> thumbpos = getthumbpos();

		if (thumb.bounds(thumbpos).contains(cursor))
		{
			if (pressed)
			{
				scrolling = true;
				thumb.set_state(Button::State::PRESSED);

				return Cursor::State::VSCROLLIDLE;
			}
			else
			{
				thumb.set_state(Button::State::NORMAL);

				return Cursor::State::VSCROLL;
			}
		}
		else
		{
			thumb.set_state(Button::State::NORMAL);
		}

		if (prev.bounds(Point<int16_t>()).contains(cursor))
		{
			if (pressed)
			{
				if (row > 0)
				{
					row--;
					onmoved(true);
				}

				prev.set_state(Button::State::PRESSED);

				return Cursor::State::VSCROLLIDLE;
			}
			else
			{
				prev.set_state(Button::State::MOUSEOVER);

				return Cursor::State::VSCROLL;
			}
		}
		else
		{
			prev.set_state(Button::State::NORMAL);
		}

		if (next.bounds(Point<int16_t>()).contains(cursor))
		{
			if (pressed)
			{
				if (row < rowmax)
				{
					row++;
					onmoved(false);
				}

				next.set_state(Button::State::PRESSED);

				return Cursor::State::VSCROLLIDLE;
			}
			else
			{
				next.set_state(Button::State::MOUSEOVER);

				return Cursor::State::VSCROLL;
			}
		}
		else
		{
			next.set_state(Button::State::NORMAL);
		}

		if (cursor.y() < vertical.second())
		{
			if (pressed)
			{
				auto yoffset = static_cast<double>(relative.y() - buttonheight * 2);
				auto cursorrow = static_cast<int16_t>(std::round(yoffset / rowheight));

				if (cursorrow < 0)
					cursorrow = 0;
				else if (cursorrow > rowmax)
					cursorrow = rowmax;

				int16_t delta = row - cursorrow;

				for (size_t i = 0; i < 2; i++)
				{
					if (delta > 0)
					{
						row--;
						delta--;
						onmoved(true);
					}

					if (delta < 0)
					{
						row++;
						delta++;
						onmoved(false);
					}
				}

				return Cursor::State::VSCROLLIDLE;
			}
		}

		return Cursor::State::VSCROLL;
	}

	void Slider::send_scroll(double yoffset)
	{
		if (yoffset < 0 && row < rowmax)
		{
			row++;
			onmoved(false);
		}

		if (yoffset > 0 && row > 0)
		{
			row--;
			onmoved(true);
		}
	}
}