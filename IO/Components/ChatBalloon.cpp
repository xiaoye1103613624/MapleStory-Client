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
#include "ChatBalloon.h"

#include "../../Constants.h"

#include <algorithm>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	ChatBalloon::ChatBalloon(int8_t type)
	{
		std::string typestr;

		if (type < 0)
		{
			switch (type)
			{
				case -1:
					typestr = "dead";
					break;
			}
		}
		else
		{
			typestr = std::to_string(type);
		}

		nl::node src = nl::nx::UI["ChatBalloon.img"][typestr];

		arrow = src["arrow"];
		frame = src;

		textlabel = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK, "", 80);

		duration = 0;
	}

	ChatBalloon::ChatBalloon() : ChatBalloon(0) {}

	void ChatBalloon::change_text(const std::string& text)
	{
		textlabel.change_text(text);

		duration = DURATION;
	}

	void ChatBalloon::draw(Point<int16_t> position) const
	{
		if (duration == 0)
			return;

		int16_t width = textlabel.width();
		int16_t height = textlabel.height();

		frame.draw(position, width, height);
		arrow.draw(position);
		textlabel.draw(position - Point<int16_t>(0, height + 4));
	}

	void ChatBalloon::update()
	{
		duration -= Constants::TIMESTEP;

		if (duration < 0)
			duration = 0;
	}

	void ChatBalloon::expire()
	{
		duration = 0;
	}

	ChatBalloonHorizontal::ChatBalloonHorizontal()
	{
		nl::node Balloon = nl::nx::UI["Login.img"]["WorldNotice"]["Balloon"];

		arrow = Balloon["arrow"];
		center = Balloon["c"];
		northeast = Balloon["ne"];
		north = Balloon["n"];
		northwest = Balloon["nw"];
		west = Balloon["w"];
		southwest = Balloon["sw"];
		south = Balloon["s"];
		southeast = Balloon["se"];

		xtile = std::max<int16_t>(north.width(), 1);
		ytile = std::max<int16_t>(west.height(), 1);

		textlabel = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::BLACK);
	}

	void ChatBalloonHorizontal::draw(Point<int16_t> position) const
	{
		// 空文案不画边框，避免与 BalloonForLoginProcess 公告叠成「两个弹窗」。
		if (textlabel.get_text().empty())
			return;

		int16_t width = textlabel.width() + 9;
		int16_t height = textlabel.height() - 1;

		int16_t left = position.x() - width / 2;
		int16_t top = position.y() - height;
		int16_t right = left + width;
		int16_t bottom = top + height;

		northwest.draw(DrawArgument(left, top));
		southwest.draw(DrawArgument(left, bottom));

		for (int16_t y = top; y < bottom; y += ytile)
		{
			west.draw(DrawArgument(left, y));
		}

		center.draw(DrawArgument(Point<int16_t>(left, top), Point<int16_t>(width + 1, height)));

		for (int16_t x = left; x < right; x += xtile)
		{
			north.draw(DrawArgument(x, top));
			south.draw(DrawArgument(x, bottom));
		}

		northeast.draw(DrawArgument(right, top));
		southeast.draw(DrawArgument(right, bottom));

		arrow.draw(DrawArgument(right + 1, top));
		textlabel.draw(DrawArgument(left + 6, top - 4));
	}

	void ChatBalloonHorizontal::change_text(const std::string& text)
	{
		textlabel.change_text(text);
	}

	LoginProcessNotice::LoginProcessNotice() : box_w(0), box_h(0)
	{
		// 优先整板用 BalloonForLoginProcess 九宫（决策 9.7）；缺件时回退 Balloon 小九宫 + Title。
		nl::node loginproc = nl::nx::UI["Login.img"]["WorldNotice"]["BalloonForLoginProcess"];
		nl::node balloon = nl::nx::UI["Login.img"]["WorldNotice"]["Balloon"];
		nl::node frame = loginproc;

		if (!loginproc["nw"] && !loginproc["c"])
			frame = balloon;

		title = loginproc["Title"] ? loginproc["Title"] : Texture();
		center = frame["c"] ? frame["c"] : balloon["c"];
		northeast = frame["ne"] ? frame["ne"] : balloon["ne"];
		north = frame["n"] ? frame["n"] : balloon["n"];
		northwest = frame["nw"] ? frame["nw"] : balloon["nw"];
		west = frame["w"] ? frame["w"] : balloon["w"];
		east = frame["e"] ? frame["e"] : balloon["e"];
		southwest = frame["sw"] ? frame["sw"] : balloon["sw"];
		south = frame["s"] ? frame["s"] : balloon["s"];
		southeast = frame["se"] ? frame["se"] : balloon["se"];

		xtile = std::max<int16_t>(north.width(), 1);
		ytile = std::max<int16_t>(west.height(), 1);

		textlabel = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::BLACK);
	}

	void LoginProcessNotice::change_text(const std::string& text)
	{
		textlabel.change_text(text);
		relayout();
	}

	void LoginProcessNotice::relayout()
	{
		if (textlabel.get_text().empty())
		{
			box_w = 0;
			box_h = 0;
			return;
		}

		constexpr int16_t pad_x = 18;
		constexpr int16_t pad_y = 8;
		constexpr int16_t gap = 4;

		const int16_t inner_w = std::max(title.width(), static_cast<int16_t>(textlabel.width() + pad_x));
		const int16_t inner_h = static_cast<int16_t>(title.height() + gap + textlabel.height() + pad_y);

		// 与 ChatBalloonHorizontal 一致：按 tile 对齐，避免边框与填充错缝。
		const int16_t numhor = inner_w / xtile + 2;
		const int16_t numver = std::max<int16_t>(inner_h / ytile, 1) + 1;

		box_w = static_cast<int16_t>(numhor * xtile);
		box_h = static_cast<int16_t>(numver * ytile);
	}

	bool LoginProcessNotice::empty() const
	{
		return textlabel.get_text().empty();
	}

	int16_t LoginProcessNotice::width() const
	{
		return box_w;
	}

	int16_t LoginProcessNotice::height() const
	{
		return box_h;
	}

	void LoginProcessNotice::draw(Point<int16_t> topleft) const
	{
		if (empty() || box_w <= 0 || box_h <= 0)
			return;

		const int16_t left = topleft.x();
		const int16_t top = topleft.y();
		const int16_t right = static_cast<int16_t>(left + box_w);
		const int16_t bottom = static_cast<int16_t>(top + box_h);

		northwest.draw(DrawArgument(left, top));
		northeast.draw(DrawArgument(right, top));
		southwest.draw(DrawArgument(left, bottom));
		southeast.draw(DrawArgument(right, bottom));

		for (int16_t y = top; y < bottom; y += ytile)
		{
			west.draw(DrawArgument(left, y));
			east.draw(DrawArgument(right, y));
		}

		center.draw(DrawArgument(Point<int16_t>(left, top), Point<int16_t>(box_w + 1, box_h)));

		for (int16_t x = left; x < right; x += xtile)
		{
			north.draw(DrawArgument(x, top));
			south.draw(DrawArgument(x, bottom));
		}

		const int16_t title_x = static_cast<int16_t>(left + (box_w - title.width()) / 2);
		title.draw(Point<int16_t>(title_x, static_cast<int16_t>(top + 4)));

		const int16_t text_x = static_cast<int16_t>(left + box_w / 2);
		const int16_t text_y = static_cast<int16_t>(top + title.height() + 10);
		textlabel.draw(Point<int16_t>(text_x, text_y));
	}
}