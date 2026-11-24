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
#include "UIBuffList.h"
#include "UITemporaryStatView.h"

#include "UISoftKey.h"

#include "../../Weather/WeatherState.h"

#include "../UI.h"
#include "../../Configuration.h"
#include "../../Data/ItemData.h"
#include "../../Net/Packets/CommonPackets.h"
#include "../../Net/Packets/ItemAClassPackets.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/Misc.h"
#include "../../Util/PerfLog.h"

#include <cstdio>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	BuffIcon::BuffIcon(int32_t buff, int32_t dur, bool timer) :
		cover(IconCover::BUFF, dur - FLASH_TIME),
		buffid(buff),
		duration(dur),
		shown_seconds(-1),
		opcstep(-0.05f),
		show_timer(timer),
		timelabel(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW),
		timeoutline(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK)
	{
		opacity.set(1.0f);

		if (buffid >= 0)
		{
			std::string strid = string_format::extend_id(buffid, 7);
			nl::node src = nl::nx::Skill[string_format::skill_namespace(buffid) + ".img"]["skill"][strid];
			icon = src["icon"];
		}
		else
		{
			icon = ItemData::get(-buffid).get_icon(true);
		}

		refresh_label();
	}

	void BuffIcon::draw(Point<int16_t> position, float alpha) const
	{
		icon.draw(DrawArgument(position, opacity.get(alpha)));
		cover.draw(position + Point<int16_t>(1, -31), alpha);

		if (timelabel.empty())
			return;

		// 图标精灵以底边对齐 position，故画布坐标 y=21 对应屏幕 y = position.y - 32 + 21；
		// 水平方向按参考端规则在 32px 内居中。
		Point<int16_t> labelpos = position + Point<int16_t>(
			static_cast<int16_t>((ICON_SIZE - timelabel.width()) / 2),
			LABEL_TOP - ICON_SIZE
		);

		float opc = opacity.get(alpha);

		// 黑色描边：先铺 8 个方向的偏移副本，再叠黄色正文（对齐参考端 9 次黑色 + 1 次黄色）。
		for (int16_t dy = -1; dy <= 1; dy++)
		{
			for (int16_t dx = -1; dx <= 1; dx++)
			{
				if (dx == 0 && dy == 0)
					continue;

				timeoutline.draw(DrawArgument(labelpos + Point<int16_t>(dx, dy), opc));
			}
		}

		timelabel.draw(DrawArgument(labelpos, opc));
	}

	bool BuffIcon::update()
	{
		if (duration <= FLASH_TIME)
		{
			opacity += opcstep;

			bool fadedout = opcstep < 0.0f && opacity.last() <= 0.0f;
			bool fadedin = opcstep > 0.0f && opacity.last() >= 1.0f;

			if (fadedout || fadedin)
				opcstep = -opcstep;
		}

		cover.update();

		duration -= Constants::TIMESTEP;

		refresh_label();

		return duration < Constants::TIMESTEP;
	}

	void BuffIcon::refresh_label()
	{
		if (!show_timer)
			return;

		int32_t seconds = (duration + 999) / 1000;

		if (seconds < 1)
			seconds = 1;

		if (seconds == shown_seconds)
			return;

		shown_seconds = seconds;

		std::string text = format_time(duration);
		timelabel.change_text(text);
		timeoutline.change_text(text);
	}

	std::string BuffIcon::format_time(int32_t milliseconds)
	{
		int32_t seconds = (milliseconds + 999) / 1000;

		if (seconds < 1)
			seconds = 1;

		char buffer[24];

		if (seconds >= 3600)
			std::snprintf(buffer, sizeof(buffer), "%dh%dm", seconds / 3600, (seconds % 3600) / 60);
		else if (seconds >= 60)
			std::snprintf(buffer, sizeof(buffer), "%dm%ds", seconds / 60, seconds % 60);
		else
			std::snprintf(buffer, sizeof(buffer), "%ds", seconds);

		return buffer;
	}

	UIBuffList::UIBuffList() :
		show_timer(Configuration::get().get_buff_timer()),
		// 2026-11-23 用户要求时间从 44px 缩小一半：A44M(44px) → A22M(22px)。
		// 放大/缩小用真实字号而非 DrawArgument 缩放（drawtext 不读 xscale/yscale）。
		timeclock_label(Text::Font::A22M, Text::Alignment::RIGHT, Color::Name::YELLOW),
		timeclock_outline(Text::Font::A22M, Text::Alignment::RIGHT, Color::Name::BLACK),
		// 天气图标标签：CJK 单字，与时间同右对齐基线，放时间右侧。
		weatherglyph_label(Text::Font::A22M, Text::Alignment::LEFT, Color::Name::YELLOW),
		weatherglyph_outline(Text::Font::A22M, Text::Alignment::LEFT, Color::Name::BLACK),
		// 天气元素提示用小号 A11M，右对齐，与时间大字同色系。
		elementhint_label(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::YELLOW),
		elementhint_outline(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::BLACK)
	{
		// Edge 右上：图标条从右缘向左铺；dimension.x 历史语义见 update_screen。
		set_layout_edge(LayoutEdge::TopRight, Point<int16_t>(35, 55));

		int16_t height = Constants::Constants::get().get_viewheight();
		int16_t width = Constants::Constants::get().get_viewwidth();

		update_screen(width, height);
	}

	void UIBuffList::draw(float alpha) const
	{
		Point<int16_t> icpos = position;

		// 顺序快照：右键命中索引用同一顺序（unordered_map 迭代序在 erase 后可变）
		draw_order.clear();

		for (auto& icon : icons)
		{
			draw_order.push_back(icon.first);
			icon.second.draw(icpos, alpha);
			icpos.shift_x(-32);
		}

		// 右上 buff 下方的游戏内时间 hh:mm（2026-11-23）。无 0x373D 状态不画。
		// 44px 大字（4 倍）：右缘仍与 buff 右缘对齐；字模墨迹顶约在绘制点下方
		// ink_offset 处，y 取 position.y+2 让大字紧接图标带下方而不压到小地图。
		const weather::State& wst = weather::State::get();
		if (wst.has_state() && !timeclock_label.empty())
		{
			// 22px 时间（缩小一半）。天气标签贴右缘，时间右对齐点左移一个标签宽，
			// 保证「天气在时间右侧」且不越出屏幕右缘。
			const int16_t glyph_w = weatherglyph_label.empty() ? 0 : weatherglyph_label.width();
			const int16_t gap = glyph_w > 0 ? 8 : 0;
			const Point<int16_t> tpos(position.x() - glyph_w - gap, position.y() + 2);
			// 黑描边（复用 BuffIcon 的 8 方向描边口径），黄字在上。
			constexpr int16_t OUTLINE = 2;
			for (int16_t dy = -OUTLINE; dy <= OUTLINE; dy++)
			{
				for (int16_t dx = -OUTLINE; dx <= OUTLINE; dx++)
				{
					if (dx == 0 && dy == 0)
						continue;
					timeclock_outline.draw(DrawArgument(tpos + Point<int16_t>(dx, dy), alpha));
				}
			}
			timeclock_label.draw(DrawArgument(tpos, alpha));
		}

		// 天气图标（CJK 单字，贴右缘、在时间右侧），同样 2px 描边。
		if (wst.has_state() && !weatherglyph_label.empty())
		{
			const Point<int16_t> gpos(position.x(), position.y() + 2);
			constexpr int16_t OUTLINE = 2;
			for (int16_t dy = -OUTLINE; dy <= OUTLINE; dy++)
			{
				for (int16_t dx = -OUTLINE; dx <= OUTLINE; dx++)
				{
					if (dx == 0 && dy == 0)
						continue;
					weatherglyph_outline.draw(DrawArgument(gpos + Point<int16_t>(dx, dy), alpha));
				}
			}
			weatherglyph_label.draw(DrawArgument(gpos, alpha));
		}

		// 天气元素提示：时间大字下方，A11M 小字，同样 1px 黑描边。
		if (wst.has_state() && !elementhint_label.empty())
		{
			const Point<int16_t> epos(position.x(), position.y() + 52);
			for (int16_t dy = -1; dy <= 1; dy++)
			{
				for (int16_t dx = -1; dx <= 1; dx++)
				{
					if (dx == 0 && dy == 0)
						continue;
					elementhint_outline.draw(DrawArgument(epos + Point<int16_t>(dx, dy), alpha));
				}
			}
			elementhint_label.draw(DrawArgument(epos, alpha));
		}
	}

	void UIBuffList::update()
	{
		for (auto iter = icons.begin(); iter != icons.end();)
		{
			bool expired = iter->second.update();

			if (expired)
				iter = icons.erase(iter);
			else
				iter++;
		}

		const weather::State& wst = weather::State::get();
		const bool has_state = wst.has_state();

		// 2026-11-23 V2：游戏时间 MM月DD日 hh:mm（日期来自 0x3742 gameDate，
		// 与游戏季节同步）。分钟或日期变化时重排文本，无 0x373D 时清空。
		const int minute = has_state ? wst.minute_of_day() : -1;
		const int game_date = has_state ? wst.game_date() : 0;
		if (minute != timeclock_shown_minute || game_date != timeclock_shown_date)
		{
			timeclock_shown_minute = static_cast<int16_t>(minute);
			timeclock_shown_date = static_cast<int16_t>(game_date);
			if (minute < 0)
			{
				timeclock_label.change_text("");
				timeclock_outline.change_text("");
			}
			else
			{
				char buf[24];
				if (game_date > 0)
				{
					const int month = game_date / 100;
					const int day = game_date % 100;
					std::snprintf(buf, sizeof(buf), "%02d月%02d日 %02d:%02d",
						month, day, minute / 60, minute % 60);
				}
				else
				{
					// 旧服务端未发 0x3742：退化为原 hh:mm（不显示日期）。
					std::snprintf(buf, sizeof(buf), "%02d:%02d", minute / 60, minute % 60);
				}
				timeclock_label.change_text(buf);
				timeclock_outline.change_text(buf);
			}
		}

		// 2026-11-23 天气元素提示：本地 sky→元素映射（与服务端 DB 种子表一致），
		// 只显示元素名不显示倍率（倍率由服务端权威且可配）。夜晚时叠加"夜·暗"。
		// 只在 key 变化时重排文本。
		std::string hint_key;
		if (has_state)
		{
			switch (wst.sky())
			{
				case weather::Sky::RAIN: case weather::Sky::STORM: hint_key = "雷"; break;
				case weather::Sky::SNOW: case weather::Sky::BLIZZARD: hint_key = "冰"; break;
				case weather::Sky::CLEAR: hint_key = "火/圣"; break;
				case weather::Sky::OVERCAST: case weather::Sky::SANDSTORM: hint_key = "毒"; break;
				default: break;
			}
			if (wst.minute_of_day() >= 19 * 60 || wst.minute_of_day() < 4 * 60)
				hint_key += hint_key.empty() ? "暗" : "·暗";
		}
		if (hint_key != elementhint_shown_key)
		{
			elementhint_shown_key = hint_key;
			const std::string text = hint_key.empty() ? "" : ("天气·" + hint_key + " 增强");
			elementhint_label.change_text(text);
			elementhint_outline.change_text(text);
		}

		// 天气图标（CJK 单字，emoji 码点在 SourceHanSerifSC 缺字会变方块，故不用 emoji）。
		// RAIN/STORM 再按是否雷暴区分小雨/中雨/大雨：wire 无雨强字段，统一显示「雨」，
		// 强度由服务端权威不在此细分；SNOW/BLIZZARD→雪，OVERCAST/SANDSTORM→阴/雾。
		std::string glyph;
		if (has_state)
		{
			switch (wst.sky())
			{
				case weather::Sky::CLEAR:     glyph = u8"晴"; break;
				case weather::Sky::RAIN:      glyph = u8"雨"; break;
				case weather::Sky::SNOW:      glyph = u8"雪"; break;
				case weather::Sky::OVERCAST:  glyph = u8"阴"; break;
				case weather::Sky::STORM:     glyph = u8"雷"; break;
				case weather::Sky::BLIZZARD:  glyph = u8"雪"; break;
				case weather::Sky::LEAVES:    glyph = u8"叶"; break;
				case weather::Sky::BLOSSOM:   glyph = u8"花"; break;
				case weather::Sky::SANDSTORM: glyph = u8"雾"; break;
				default: break;
			}
		}
		if (glyph != weatherglyph_shown_key)
		{
			weatherglyph_shown_key = glyph;
			weatherglyph_label.change_text(glyph);
			weatherglyph_outline.change_text(glyph);
		}
	}

	void UIBuffList::update_screen(int16_t new_width, int16_t)
	{
		// 锚点是「右缘内侧 35px」处的单格原点，不是窗口左上；故不用基类 Edge 的 dim 减法。
		position = Point<int16_t>(
			static_cast<int16_t>(new_width - layout_margin.x()),
			layout_margin.y());
		dimension = Point<int16_t>(position.x(), 32);
	}

	Cursor::State UIBuffList::send_cursor(bool pressed, Point<int16_t> cursorposition)
	{
		return UIElement::send_cursor(pressed, cursorposition);
	}

	// Wave 3：命中框 = 条带并集，几何与 draw() 严格同构：
	// 第 i 格锚点 icpos_i = (position.x - 32*i, position.y)，图标 origin(0,32)
	// 底边对齐锚点 → 可见框 [x-32i, x-32i+32) × [y-32, y]。
	// 并集 = [position.x - 32*(n-1), position.x + 32) × [position.y - 32, position.y]。
	// 基类矩形（position..position+dimension）在此窗不成立（见头注释）。
	bool UIBuffList::is_in_range(Point<int16_t> cursor_position) const
	{
		if (icons.empty())
			return false;

		const int32_t w = cursor_position.x() - position.x();
		const int32_t left = -32 * (static_cast<int32_t>(icons.size()) - 1);

		return w >= left && w < 32
			&& cursor_position.y() >= position.y() - 32 && cursor_position.y() <= position.y();
	}

	void UIBuffList::rightclick(Point<int16_t> cursorpos)
	{
		const int32_t w = cursorpos.x() - position.x();

		if (w >= 32 || !is_in_range(cursorpos))
			return;

		// 槽号 = draw 顺序快照里的第几格（与上一帧实际渲染一致）。
		// w∈[-32(n-1), 32) → slot = (31 - w)/32 ∈ [0, n)。
		size_t slot = static_cast<size_t>((31 - w) / 32);

		if (slot >= draw_order.size())
			return;

		const int32_t skillid = draw_order[slot];

		auto it = icons.find(skillid);
		if (it == icons.end())
			return;	// 快照滞后一帧（项刚过期）：静默丢弃

		// icons 的 key 编码见 BuffIcon::BuffIcon：正数=技能id，负数=-itemid。
		// 旧判据 `skillid >= 1000000` 把 7 位技能 id 误当道具 → 发 0x49，
		// 服务端 CancelItemEffectHandler 取负得 -skillid，getItemEffect(-skillid)
		// 为 null → cancelEffect(null) → extractCurrentBuffStats 空指针崩溃
		// （out1.log 自 23:20:28 起 37 次 CancelItemEffectHandler 出错， CancelBuffHandler 0 次）。
		// 正确判据按**正负号**：负数 key 本身就是 -itemid，服务端会再取负还原道具id。
		if (skillid < 0)
			CancelItemEffectPacket(skillid).dispatch();
		else
			CancelBuffPacket(skillid).dispatch();
		perf::mark("BUFF", "cancel request skillid=" + std::to_string(skillid));
	}

	UIElement::Type UIBuffList::get_type() const
	{
		return TYPE;
	}

	void UIBuffList::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) return;
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't')
		{
			
		}
		else if (keycode == 'C' || keycode == 'c' || keycode == 'X' || keycode == 'x')
		{
			
		}
		else if (keycode == 'D' || keycode == 'd')
		{
			// W5：打开 TemporaryStatView 明细板
			auto ui = UI::get().get_element<UITemporaryStatView>();
			if (!ui) ui = UI::get().emplace<UITemporaryStatView>();
			else if (!ui->is_active()) ui->makeactive();
			else ui->deactivate();
		}
	}

	void UIBuffList::add_buff(int32_t buffid, int32_t duration)
	{
		icons.emplace(
			std::piecewise_construct,
			std::forward_as_tuple(buffid),
			std::forward_as_tuple(buffid, duration, show_timer)
		);
	}

	void UIBuffList::remove_buff(int32_t buffid)
	{
		icons.erase(buffid);
	}
}