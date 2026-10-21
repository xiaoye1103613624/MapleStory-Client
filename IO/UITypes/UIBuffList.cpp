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

#include "../../Configuration.h"
#include "../../Data/ItemData.h"
#include "../../Util/Misc.h"

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
			nl::node src = nl::nx::Skill[strid.substr(0, 3) + ".img"]["skill"][strid];
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
		show_timer(Configuration::get().get_buff_timer())
	{
		int16_t height = Constants::Constants::get().get_viewheight();
		int16_t width = Constants::Constants::get().get_viewwidth();

		update_screen(width, height);
	}

	void UIBuffList::draw(float alpha) const
	{
		Point<int16_t> icpos = position;

		for (auto& icon : icons)
		{
			icon.second.draw(icpos, alpha);
			icpos.shift_x(-32);
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
	}

	void UIBuffList::update_screen(int16_t new_width, int16_t)
	{
		position = Point<int16_t>(new_width - 35, 55);
		dimension = Point<int16_t>(position.x(), 32);
	}

	Cursor::State UIBuffList::send_cursor(bool pressed, Point<int16_t> cursorposition)
	{
		return UIElement::send_cursor(pressed, cursorposition);
	}

	UIElement::Type UIBuffList::get_type() const
	{
		return TYPE;
	}

	void UIBuffList::add_buff(int32_t buffid, int32_t duration)
	{
		icons.emplace(
			std::piecewise_construct,
			std::forward_as_tuple(buffid),
			std::forward_as_tuple(buffid, duration, show_timer)
		);
	}
}