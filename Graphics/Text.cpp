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
#include "Text.h"

#include "GraphicsGL.h"

#include "../Data/ItemData.h"

namespace ms
{
	Text::Text(Font f, Alignment a, Color::Name c, Background b, const std::string& t, uint16_t mw, bool fm, int16_t la) : font(f), alignment(a), color(c), background(b), maxwidth(mw), formatted(fm), line_adj(la)
	{
		change_text(t);
	}

	Text::Text(Font f, Alignment a, Color::Name c, const std::string& t, uint16_t mw, bool fm, int16_t la) : Text(f, a, c, Background::NONE, t, mw, fm, la) {}
	Text::Text() : Text(Font::A11M, Alignment::LEFT, Color::BLACK) {}

	void Text::reset_layout()
	{
		if (text.empty())
			return;

		layout = GraphicsGL::get().createlayout(text, font, alignment, color, maxwidth, formatted, line_adj);
	}

	void Text::change_text(const std::string& t)
	{
		if (text == t)
			return;

		text = t;

		reset_layout();
	}

	void Text::change_color(Color::Name c)
	{
		if (color == c)
			return;

		color = c;

		reset_layout();
	}

	void Text::set_background(Background b)
	{
		background = b;
	}

	void Text::draw(const DrawArgument& args) const
	{
		draw(args, Range<int16_t>(0, 0));
	}

	void Text::draw(const DrawArgument& args, const Range<int16_t>& vertical) const
	{
		GraphicsGL::get().drawtext(args, vertical, text, layout, font, color, background);

		// #i/#v 行内图标：布局期已占位；此处按 IDA 底对齐基线画 canvas
		// （UtilDlgEx 元素 y = line_pos - height，blit 左上角）。
		const Point<int16_t> base = args.getpos();
		const bool clip_v = vertical.first() > 0 || vertical.second() > 0;

		for (const Text::Layout::Icon& icon : layout.get_icons())
		{
			size_t li = 0;
			Point<int16_t> linepos;
			bool found = false;

			for (const Text::Layout::Line& line : layout)
			{
				if (li == icon.line)
				{
					linepos = line.position;
					found = true;
					break;
				}

				++li;
			}

			if (!found)
				continue;

			const int16_t draw_x = static_cast<int16_t>(base.x() + linepos.x() + icon.ax);
			const int16_t draw_y = static_cast<int16_t>(base.y() + linepos.y() - icon.height);

			if (clip_v)
			{
				const int16_t vmin = vertical.first() > 0 ? vertical.first() : draw_y;
				const int16_t vmax = vertical.second() > 0 ? vertical.second() : static_cast<int16_t>(draw_y + icon.height);

				if (draw_y + icon.height < vmin || draw_y > vmax)
					continue;
			}

			const Texture& tex = ItemData::get(icon.item_id).get_icon(false);

			if (!tex)
				continue;

			// Texture::draw 会扣 origin；原生 blit 按左上角落点，故先加回 origin。
			tex.draw(DrawArgument(Point<int16_t>(
				static_cast<int16_t>(draw_x + tex.get_origin().x()),
				static_cast<int16_t>(draw_y + tex.get_origin().y()))));
		}
	}

	uint16_t Text::advance(size_t pos) const
	{
		return layout.advance(pos);
	}

	bool Text::empty() const
	{
		return text.empty();
	}

	size_t Text::length() const
	{
		return text.size();
	}

	int16_t Text::width() const
	{
		return layout.width();
	}

	int16_t Text::height() const
	{
		return layout.height();
	}

	Point<int16_t> Text::dimensions() const
	{
		return layout.get_dimensions();
	}

	Point<int16_t> Text::endoffset() const
	{
		return layout.get_endoffset();
	}

	const std::string& Text::get_text() const
	{
		return text;
	}

	Point<int16_t> Text::position_of(size_t pos) const
	{
		return layout.position_of(pos);
	}

	int16_t Text::linespace(Font f)
	{
		return GraphicsGL::get().get_linespace(f);
	}

	int16_t Text::ink_offset() const
	{
		return GraphicsGL::get().get_ink_offset(font, text);
	}

	int16_t Text::y_for_ink_top(int16_t ink_top) const
	{
		return static_cast<int16_t>(ink_top - ink_offset());
	}

	Text::Layout::Layout(const std::vector<Layout::Line>& l, const std::vector<int16_t>& a, int16_t w, int16_t h, int16_t ex, int16_t ey)
		: Layout(l, a, std::vector<Icon>(), w, h, ex, ey) {}

	Text::Layout::Layout(const std::vector<Layout::Line>& l, const std::vector<int16_t>& a, const std::vector<Icon>& ic, int16_t w, int16_t h, int16_t ex, int16_t ey)
		: lines(l), advances(a), icons(ic), dimensions(w, h), endoffset(ex, ey) {}

	Text::Layout::Layout() : Layout(std::vector<Layout::Line>(), std::vector<int16_t>(), 0, 0, 0, 0) {}

	int16_t Text::Layout::width() const
	{
		return dimensions.x();
	}

	int16_t Text::Layout::height() const
	{
		return dimensions.y();
	}

	int16_t Text::Layout::advance(size_t index) const
	{
		return index < advances.size() ? advances[index] : 0;
	}

	Point<int16_t> Text::Layout::get_dimensions() const
	{
		return dimensions;
	}

	Point<int16_t> Text::Layout::get_endoffset() const
	{
		return endoffset;
	}

	const std::vector<Text::Layout::Icon>& Text::Layout::get_icons() const
	{
		return icons;
	}

	Point<int16_t> Text::Layout::position_of(size_t pos) const
	{
		for (const Line& line : lines)
		{
			if (line.words.empty())
				continue;

			const size_t first = line.words.front().first;
			const size_t last = line.words.back().last;

			// 严格小于：上一行的末边界和下一行的首边界是同一个字节，取「起点」时
			// 正确答案属于下一行，先撞上上一行会拿到行末的 x。
			if (pos < first || pos >= last)
				continue;

			return Point<int16_t>(line.position.x() + advance(pos), line.position.y());
		}

		return Point<int16_t>();
	}

	Text::Layout::iterator Text::Layout::begin() const
	{
		return lines.begin();
	}

	Text::Layout::iterator Text::Layout::end() const
	{
		return lines.end();
	}
}