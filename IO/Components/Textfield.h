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

#include "../Cursor.h"
#include "../Keyboard.h"

#include "../../Graphics/Geometry.h"

#include <functional>

namespace ms
{
	class Textfield
	{
	public:
		enum State
		{
			NORMAL,
			DISABLED,
			FOCUSED
		};

		Textfield();
		Textfield(Text::Font font, Text::Alignment alignment, Color::Name text_color, Rectangle<int16_t> bounds, size_t limit, int16_t marker_height = 12);

		void draw(Point<int16_t> position) const;
		void draw(Point<int16_t> position, Point<int16_t> marker_adjust) const;
		void update();
		void update(Point<int16_t> position, Point<int16_t> dimensions);
		void send_key(KeyType::Id type, int32_t action, bool pressed);
		void add_string(const std::string& str);

		void set_state(State state);
		void change_text(const std::string& text);
		void set_cryptchar(int8_t character);
		void set_limit(size_t limit);
		/// 文字相对 bounds 左上角的绘制偏移（FreeType 基线在 y，木牌矮槽需下移）。
		void set_text_shift(Point<int16_t> shift) { text_shift = shift; }
		/// 允许接收 UTF-8（IME/剪贴板中文）。默认 false（账号/密码等仍限 ASCII）。
		void set_accept_unicode(bool accept) { accept_unicode = accept; }
		/// UI::focus_textfield 据此决定开/关系统 IME 文本输入。
		bool accepts_unicode() const { return accept_unicode; }

		void set_enter_callback(std::function<void(std::string)> onreturn);
		void set_key_callback(KeyAction::Id key, std::function<void(void)> action);
		void set_text_callback(std::function<void(void)> action);

		Cursor::State send_cursor(Point<int16_t> cursorpos, bool clicked);

		bool empty() const;
		State get_state() const;
		const std::string& get_text() const;
		bool can_copy_paste() const;

	private:
		void modifytext(const std::string& t);
		bool belowlimit() const;

		Text textlabel;
		std::string text;
		ColorLine marker;
		bool showmarker;
		uint16_t elapsed;
		size_t markerpos;
		Rectangle<int16_t> bounds;
		size_t limit;
		int8_t crypt;
		bool accept_unicode = false;
		State state;
		Point<int16_t> text_shift;

		std::function<void(std::string)> onreturn;
		std::map<int32_t, std::function<void(void)>> callbacks;
		std::function<void(void)> ontext;

#if LOG_LEVEL >= LOG_UI
		ColorBox boundsoutline;
#endif
	};
}