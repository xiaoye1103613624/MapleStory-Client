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
#pragma once

#include "MapleFrame.h"

#include "../../Graphics/Text.h"

namespace ms
{
	class ChatBalloon
	{
	public:
		ChatBalloon(int8_t type);
		ChatBalloon();

		void draw(Point<int16_t> position) const;
		void update();

		void change_text(const std::string& text);
		void expire();

	private:
		// How long a line stays on screen
		static constexpr int16_t DURATION = 4000; // 4 seconds

		MapleFrame frame;
		Text textlabel;
		Texture arrow;
		int16_t duration;
	};

	class ChatBalloonHorizontal
	{
	public:
		ChatBalloonHorizontal();

		void draw(Point<int16_t> position) const;

		void change_text(const std::string& text);

	private:
		Text textlabel;
		Texture arrow;
		Texture center;
		Texture northeast;
		Texture north;
		Texture northwest;
		Texture west;
		Texture southwest;
		Texture south;
		Texture southeast;
		int16_t xtile;
		int16_t ytile;
	};

	/// 选服频道面板左侧公告：Login.img/WorldNotice/BalloonForLoginProcess + Title「公告」。
	/// 锚点为左上角，便于相对 chBackgrn 左侧摆放。
	class LoginProcessNotice
	{
	public:
		LoginProcessNotice();

		void change_text(const std::string& text);
		void draw(Point<int16_t> topleft) const;

		bool empty() const;
		int16_t width() const;
		int16_t height() const;

	private:
		void relayout();

		Texture title;
		Texture center;
		Texture northeast;
		Texture north;
		Texture northwest;
		Texture west;
		Texture southwest;
		Texture south;
		Texture southeast;
		Texture east;
		Text textlabel;
		int16_t xtile;
		int16_t ytile;
		// 九宫边框厚度（以角件尺寸为准）。BalloonForLoginProcess 实测
		// nw/ne 22x40、sw/se 22x24：上带 40px 容纳标题「公告」，下带 24px
		// 容纳正文——原生静止框 ≈199x68（选中频道页.png 逐像素取证）。
		int16_t top_border;
		int16_t bottom_border;
		int16_t left_border;
		int16_t right_border;
		int16_t box_w;
		int16_t box_h;
	};
}