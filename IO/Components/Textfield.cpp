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
#include "Textfield.h"

#include "../UI.h"

#include <sstream>
#include <iostream>
#include <algorithm>

namespace ms
{
	Textfield::Textfield()
	{
		text = "";
	}

	Textfield::Textfield(Text::Font font, Text::Alignment alignment, Color::Name text_color, Rectangle<int16_t> b, size_t limit, int16_t marker_height) : bounds(b), limit(limit), text_shift(0, 0)
	{
		textlabel = Text(font, alignment, text_color, "", 0, false);
		marker = ColorLine(marker_height, text_color, 1.0f, true);

#if LOG_LEVEL >= LOG_UI
		boundsoutline = ColorBox(bounds.width(), bounds.height(), Color::Name::RED, 0.5f);
#endif

		text = "";
		markerpos = 0;
		crypt = 0;
		state = State::NORMAL;
	}

	void Textfield::draw(Point<int16_t> position) const
	{
		draw(position, Point<int16_t>(0, 0));
	}

	void Textfield::draw(Point<int16_t> position, Point<int16_t> marker_adjust) const
	{
		Point<int16_t> absp = bounds.get_left_top();

#if LOG_LEVEL >= LOG_UI
		boundsoutline.draw(absp);
#endif

		if (state == State::DISABLED)
			return;

		absp.shift(position);

		// FreeType 以 y 为基线：矮输入槽若不下移，字形大半画在槽外，槽内只剩竖线/白线伪影。
		Point<int16_t> text_pos = absp + text_shift;

		if (text.size() > 0)
			textlabel.draw(text_pos);

		if (state == State::FOCUSED && showmarker)
		{
			// FreeType：draw 原点之上 linespace 处是基线，字形升部在基线上方。
			// 旧逻辑用 height-4 把竖杠顶到基线附近再往下画 → 整条偏到槽底。
			// 竖杠从槽内偏上起笔，高度由构造时的 marker_height 决定。
			constexpr int16_t marker_y = 2;
			Point<int16_t> mpos = text_pos + Point<int16_t>(textlabel.advance(markerpos) - 1, marker_y) + marker_adjust;

			if (crypt > 0)
				mpos.shift(1, 0);

			marker.draw(mpos);
		}
	}

	void Textfield::update()
	{
		if (state == State::DISABLED)
			return;

		elapsed += Constants::TIMESTEP;

		if (elapsed > 256)
		{
			showmarker = !showmarker;
			elapsed = 0;
		}
	}

	void Textfield::update(Point<int16_t> position, Point<int16_t> dimensions)
	{
		if (state == State::DISABLED)
			return;

		bounds = Rectangle<int16_t>(position, position + dimensions);

#if LOG_LEVEL >= LOG_UI
		boundsoutline.setwidth(bounds.width());
		boundsoutline.setheight(bounds.height());
#endif

		elapsed += Constants::TIMESTEP;

		if (elapsed > 256)
		{
			showmarker = !showmarker;
			elapsed = 0;
		}
	}

	void Textfield::set_state(State st)
	{
		if (state != st)
		{
			const State previous = state;
			state = st;

			if (state != State::DISABLED)
			{
				elapsed = 0;
				showmarker = true;
			}
			else
			{
				UI::get().remove_textfield();
			}

			if (state == State::FOCUSED)
			{
				UI::get().focus_textfield(this);
			}
			else if (previous == State::FOCUSED && state == State::NORMAL)
			{
				// Click-away / explicit blur: drop SDL text input as well.
				UI::get().remove_textfield();
			}
		}
	}

	void Textfield::set_enter_callback(std::function<void(std::string)> onr)
	{
		onreturn = onr;
	}

	void Textfield::set_key_callback(KeyAction::Id key, std::function<void(void)> action)
	{
		callbacks[key] = action;
	}

	void Textfield::set_text_callback(std::function<void(void)> action)
	{
		ontext = action;
	}

	void Textfield::send_key(KeyType::Id type, int32_t action, bool pressed)
	{
		if (pressed)
		{
			if (type == KeyType::Id::ACTION)
			{
				switch (action)
				{
					case KeyAction::Id::LEFT:
					{
						if (markerpos > 0)
							markerpos--;

						break;
					}
					case KeyAction::Id::RIGHT:
					{
						if (markerpos < text.size())
							markerpos++;

						break;
					}
					case KeyAction::Id::BACK:
					{
						if (text.size() > 0 && markerpos > 0)
						{
							// 按 UTF-8 码点删除：回退到光标前最近的非续字节处，
							// 避免把多字节汉字拆成半截导致乱码。
							size_t start = static_cast<size_t>(markerpos) - 1;
							while (start > 0 && (static_cast<unsigned char>(text[start]) & 0xC0) == 0x80)
								start--;

							text.erase(start, static_cast<size_t>(markerpos) - start);
							markerpos = static_cast<int32_t>(start);

							modifytext(text);
						}

						break;
					}
					case KeyAction::Id::RETURN:
					{
						if (onreturn)
							onreturn(text);

						break;
					}
					case KeyAction::Id::SPACE:
					{
						add_string(" ");
						break;
					}
					case KeyAction::Id::HOME:
					{
						markerpos = 0;
						break;
					}
					case KeyAction::Id::END:
					{
						markerpos = text.size();
						break;
					}
					case KeyAction::Id::DELETE:
					{
						if (text.size() > 0 && markerpos < text.size())
						{
							size_t end = static_cast<size_t>(markerpos) + 1;
							while (end < text.size() && (static_cast<unsigned char>(text[end]) & 0xC0) == 0x80)
								end++;

							text.erase(static_cast<size_t>(markerpos), end - static_cast<size_t>(markerpos));

							modifytext(text);
						}

						break;
					}
					default:
					{
						if (callbacks.count(action))
							callbacks.at(action)();

						break;
					}
				}
			}
			else if (type == KeyType::Id::TEXT)
			{
				if (ontext)
				{
					if (isdigit(action) || isalpha(action))
					{
						ontext();
						return;
					}
				}

				// Printable ASCII only — signed int8_t cast used to corrupt
				// high bytes into negative chars (mojibake / OOB font index).
				if (action >= 32 && action < 128)
				{
					char c = static_cast<char>(static_cast<unsigned char>(action));
					add_string(std::string(1, c));
				}
			}
		}
	}

	void Textfield::add_string(const std::string& str)
	{
		for (unsigned char c : str)
		{
			// Unicode-capable fields (e.g. map search) accept full UTF-8 so
			// IME / clipboard CJK commits render via the FreeType glyph cache.
			// ASCII-only fields (account/password) keep the legacy allowlist.
			if (accept_unicode)
			{
				if (c < 32)
					continue;
			}
			else
			{
				if (c < 32 || c >= 128)
					continue;
			}

			if (belowlimit())
			{
				text.insert(markerpos, 1, static_cast<char>(c));

				markerpos++;

				modifytext(text);
			}
		}
	}

	void Textfield::modifytext(const std::string& t)
	{
		// 与 add_string 一致：FreeType atlas 仅 ASCII 32–127。
		// 从 Settings 回填账号时若含 UTF-8/GBK，不经过 add_string 会画出竖线/白线伪影。
		std::string filtered;
		filtered.reserve(t.size());

		for (unsigned char c : t)
		{
			if (accept_unicode)
			{
				if (c >= 32)
					filtered.push_back(static_cast<char>(c));
			}
			else
			{
				if (c >= 32 && c < 128)
					filtered.push_back(static_cast<char>(c));
			}
		}

		if (crypt > 0)
		{
			std::string crypted;
			crypted.insert(0, filtered.size(), static_cast<char>(crypt));
			textlabel.change_text(crypted);
		}
		else
		{
			textlabel.change_text(filtered);
		}

		text = filtered;
	}

	Cursor::State Textfield::send_cursor(Point<int16_t> cursorpos, bool clicked)
	{
		if (state == State::DISABLED)
			return Cursor::State::IDLE;

		if (bounds.contains(cursorpos))
		{
			if (clicked)
			{
				if (state == State::NORMAL)
					set_state(State::FOCUSED);

				return Cursor::State::CLICKING;
			}
			else
			{
				return Cursor::State::CANCLICK;
			}
		}
		else
		{
#if LOG_LEVEL >= LOG_UI
			Point<int16_t> lt = bounds.get_left_top();
			int16_t lt_x = lt.x();
			int16_t lt_y = lt.y();

			Point<int16_t> rb = bounds.get_right_bottom();
			int16_t rb_x = rb.x();
			int16_t rb_y = rb.y();

			int16_t cur_x = cursorpos.x();
			int16_t cur_y = cursorpos.y();

			LOG(LOG_UI,
				"(" << lt_x << ", " << lt_y << ") != (" << rb_x << ", " << rb_y << ") && "
				<< cur_x << " >= (" << lt_x << ") && "
				<< cur_x << " <= (" << rb_x << ") && "
				<< cur_y << " >= (" << lt_y << ") && "
				<< cur_y << " <= (" << rb_y << ")");
#endif

			if (clicked && state == State::FOCUSED)
				set_state(State::NORMAL);

			return Cursor::State::IDLE;
		}
	}

	void Textfield::change_text(const std::string& t)
	{
		modifytext(t);

		if (markerpos > text.size())
			markerpos = text.size();
		else
			markerpos = text.size();
	}

	void Textfield::set_cryptchar(int8_t character)
	{
		crypt = character;
	}

	void Textfield::set_limit(size_t l)
	{
		limit = l;
	}

	bool Textfield::belowlimit() const
	{
		if (limit > 0)
		{
			return text.size() < limit;
		}
		else
		{
			uint16_t advance = textlabel.advance(text.size());

			return (advance + 50) < bounds.get_horizontal().length();
		}
	}

	const std::string& Textfield::get_text() const
	{
		return text;
	}

	bool Textfield::can_copy_paste() const
	{
		if (ontext)
		{
			ontext();

			return false;
		}
		else
		{
			return true;
		}
	}

	bool Textfield::empty() const
	{
		return text.empty();
	}

	Textfield::State Textfield::get_state() const
	{
		return state;
	}
}