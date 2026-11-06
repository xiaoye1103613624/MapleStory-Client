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
#include "UISoftKey.h"

#include "../UI.h"
#include "../Window.h"

#include "../Components/MapleButton.h"

#include "../../Audio/Audio.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UISoftKey::UISoftKey(OkCallback ok_callback, CancelCallback cancel_callback, std::string tooltip_text, Point<int16_t> tooltip_pos, Mode mode)
		: UIElement(Point<int16_t>(216, 224), Point<int16_t>(0, 0)),
		  highCase(false),
		  mode(mode),
		  min_text_len(mode == Mode::Pic ? PIC_MIN_TEXT_LEN : TEXT_MIN_TEXT_LEN),
		  ok_callback(ok_callback),
		  cancel_callback(cancel_callback),
		  tooltip_pos(tooltip_pos)
	{
		nl::node SoftKey = nl::nx::UI["Login.img"]["Common"]["SoftKey"];
		nl::node backgrnd = SoftKey["backgrnd"];

		sprites.emplace_back(backgrnd);

		Point<int16_t> button_adj = Point<int16_t>(1, 0);

		buttons[Buttons::BtCancel] = std::make_unique<MapleButton>(SoftKey["BtCancel"], button_adj);
		buttons[Buttons::BtDel] = std::make_unique<MapleButton>(SoftKey["BtDel"], button_adj);
		buttons[Buttons::BtOK] = std::make_unique<MapleButton>(SoftKey["BtOK"], button_adj);
		if (SoftKey["BtShift"])
			buttons[Buttons::BtShift] = std::make_unique<MapleButton>(SoftKey["BtShift"], button_adj);

#pragma region BtNum
#pragma region Row 1
		std::string row1KeysMap[ROW1_KEYS];

		row1KeysMap[0] = "1";
		row1KeysMap[1] = "2";
		row1KeysMap[2] = "3";
		row1KeysMap[3] = "4";
		row1KeysMap[4] = "5";
		row1KeysMap[5] = "6";
		row1KeysMap[6] = "7";
		row1KeysMap[7] = "8";
		row1KeysMap[8] = "9";
		row1KeysMap[9] = "0";

		uint16_t row1r1 = random.next_int(ROW_MAX);
		uint16_t row1r2 = random.next_int(ROW_MAX);

		while (row1r1 == row1r2)
			row1r2 = random.next_int(ROW_MAX);

		row1keys[row1r1] = "Blank";
		row1keys[row1r2] = "Blank";

		uint16_t keyIndex = 0;

		for (std::string& key : row1keys)
		{
			if (key != "Blank" && keyIndex < ROW1_KEYS)
			{
				key = row1KeysMap[keyIndex];
				keyIndex++;
			}
		}

		for (uint16_t i = 0; i < ROW_MAX; i++)
		{
			const std::string& key = row1keys[i];

			if (key == "Blank")
			{
				if (SoftKey["BtblankNum"])
					buttons[Buttons::BtNum0 + i] = std::make_unique<MapleButton>(SoftKey["BtblankNum"], keypos(i, 0));
			}
			else
				buttons[Buttons::BtNum0 + i] = std::make_unique<MapleButton>(SoftKey["BtNum"][key], keypos(i, 0));
		}
#pragma endregion
#pragma endregion

#pragma region BtLowCase / BtHighCase
#pragma region Row 2
		std::string row2KeysMap[ROW2_KEYS];

		row2KeysMap[0] = get_key_map_index("Q");
		row2KeysMap[1] = get_key_map_index("W");
		row2KeysMap[2] = get_key_map_index("E");
		row2KeysMap[3] = get_key_map_index("R");
		row2KeysMap[4] = get_key_map_index("T");
		row2KeysMap[5] = get_key_map_index("Y");
		row2KeysMap[6] = get_key_map_index("U");
		row2KeysMap[7] = get_key_map_index("I");
		row2KeysMap[8] = get_key_map_index("O");
		row2KeysMap[9] = get_key_map_index("P");

		uint16_t row2r1 = random.next_int(ROW_MAX);
		uint16_t row2r2 = random.next_int(ROW_MAX);

		while (row2r1 == row2r2)
			row2r2 = random.next_int(ROW_MAX);

		row2keys[row2r1] = "Blank";
		row2keys[row2r2] = "Blank";

		keyIndex = 0;

		for (std::string& key : row2keys)
		{
			if (key != "Blank" && keyIndex < ROW2_KEYS)
			{
				key = row2KeysMap[keyIndex];
				keyIndex++;
			}
		}

		uint16_t caseKeyIndex = 0;

		for (uint16_t i = 0; i < ROW_MAX; i++, caseKeyIndex++)
		{
			const std::string& key = row2keys[i];

			if (key == "Blank")
			{
				if (SoftKey["BtblankCase"])
				{
					BtCaseKeys[caseKeyIndex][true] = std::make_unique<MapleButton>(SoftKey["BtblankCase"], keypos(i, 1));
					BtCaseKeys[caseKeyIndex][false] = std::make_unique<MapleButton>(SoftKey["BtblankCase"], keypos(i, 1));
				}
			}
			else
			{
				BtCaseKeys[caseKeyIndex][true] = std::make_unique<MapleButton>(SoftKey["BtHighCase"][key], keypos(i, 1));
				BtCaseKeys[caseKeyIndex][false] = std::make_unique<MapleButton>(SoftKey["BtLowCase"][key], keypos(i, 1));
			}
		}
#pragma endregion

#pragma region Row 3
		std::string row3KeysMap[ROW3_KEYS];

		row3KeysMap[0] = get_key_map_index("A");
		row3KeysMap[1] = get_key_map_index("S");
		row3KeysMap[2] = get_key_map_index("D");
		row3KeysMap[3] = get_key_map_index("F");
		row3KeysMap[4] = get_key_map_index("G");
		row3KeysMap[5] = get_key_map_index("H");
		row3KeysMap[6] = get_key_map_index("J");
		row3KeysMap[7] = get_key_map_index("K");
		row3KeysMap[8] = get_key_map_index("L");

		uint16_t row3r1 = random.next_int(ROW_MAX);
		uint16_t row3r2 = random.next_int(ROW_MAX);

		while (row3r1 == row3r2)
			row3r2 = random.next_int(ROW_MAX);

		uint16_t row3r3 = random.next_int(ROW_MAX);

		while (row3r1 == row3r3 || row3r2 == row3r3)
			row3r3 = random.next_int(ROW_MAX);

		row3keys[row3r1] = "Blank";
		row3keys[row3r2] = "Blank";
		row3keys[row3r3] = "Blank";

		keyIndex = 0;

		for (std::string& key : row3keys)
		{
			if (key != "Blank" && keyIndex < ROW3_KEYS)
			{
				key = row3KeysMap[keyIndex];
				keyIndex++;
			}
		}

		for (uint16_t i = 0; i < ROW_MAX; i++, caseKeyIndex++)
		{
			const std::string& key = row3keys[i];

			if (key == "Blank")
			{
				if (SoftKey["BtblankCase"])
				{
					BtCaseKeys[caseKeyIndex][true] = std::make_unique<MapleButton>(SoftKey["BtblankCase"], keypos(i, 2));
					BtCaseKeys[caseKeyIndex][false] = std::make_unique<MapleButton>(SoftKey["BtblankCase"], keypos(i, 2));
				}
			}
			else
			{
				BtCaseKeys[caseKeyIndex][true] = std::make_unique<MapleButton>(SoftKey["BtHighCase"][key], keypos(i, 2));
				BtCaseKeys[caseKeyIndex][false] = std::make_unique<MapleButton>(SoftKey["BtLowCase"][key], keypos(i, 2));
			}
		}
#pragma endregion

#pragma region Row 4
		std::string row4KeysMap[ROW4_KEYS];

		row4KeysMap[0] = get_key_map_index("Z");
		row4KeysMap[1] = get_key_map_index("X");
		row4KeysMap[2] = get_key_map_index("C");
		row4KeysMap[3] = get_key_map_index("V");
		row4KeysMap[4] = get_key_map_index("B");
		row4KeysMap[5] = get_key_map_index("N");
		row4KeysMap[6] = get_key_map_index("M");

		uint16_t row4r1 = random.next_int(ROW4_MAX);

		row4keys[row4r1] = "Blank";

		keyIndex = 0;

		for (std::string& key : row4keys)
		{
			if (key != "Blank" && keyIndex < ROW4_KEYS)
			{
				key = row4KeysMap[keyIndex];
				keyIndex++;
			}
		}

		for (uint16_t i = 0; i < ROW4_MAX; i++, caseKeyIndex++)
		{
			const std::string& key = row4keys[i];

			if (key == "Blank")
			{
				if (SoftKey["BtblankCase"])
				{
					BtCaseKeys[caseKeyIndex][true] = std::make_unique<MapleButton>(SoftKey["BtblankCase"], keypos(i, 3));
					BtCaseKeys[caseKeyIndex][false] = std::make_unique<MapleButton>(SoftKey["BtblankCase"], keypos(i, 3));
				}
			}
			else
			{
				BtCaseKeys[caseKeyIndex][true] = std::make_unique<MapleButton>(SoftKey["BtHighCase"][key], keypos(i, 3));
				BtCaseKeys[caseKeyIndex][false] = std::make_unique<MapleButton>(SoftKey["BtLowCase"][key], keypos(i, 3));
			}
		}
#pragma endregion
#pragma endregion

		Point<int16_t> textfield_topleft = position + Point<int16_t>(202, 61);

		textfield = Textfield(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Rectangle<int16_t>(textfield_topleft, textfield_topleft + Point<int16_t>(188, 21)), MAX_TEXT_LEN);

		// Pic：二级密码密文 + 拦截物理键盘字母数字（原生「只能用鼠标」）。
		// Text：明文 + 不设 text_callback，物理键盘可直接写入 Textfield。
		if (mode == Mode::Pic)
			textfield.set_cryptchar('*');

		textfield.set_enter_callback(
			[&](std::string)
			{
				button_pressed(Buttons::BtOK);
			}
		);

		textfield.set_key_callback(
			KeyAction::Id::ESCAPE,
			[&]() {
				button_pressed(Buttons::BtCancel);
			}
		);

		if (mode == Mode::Pic)
		{
			textfield.set_text_callback(
				[&]()
				{
					clear_tooltip();
					show_text("and the numbers and letters can only be entered using a mouse.", 175, true, 1);
				}
			);
		}

		// Text tip 禁止 formatted：# 色码会吞邀请提示明文（联调误判 SoftKey 文案缺失）。
		if (mode == Mode::Text)
			show_text(tooltip_text, 0, false, 0);
		else
			show_text(tooltip_text);

		dimension = Texture(backgrnd).get_dimensions();
		LOG(LOG_DEBUG, "UISoftKey: mode=" << (mode == Mode::Pic ? "Pic" : "Text")
			<< " min_len=" << min_text_len);
	}

	UISoftKey::UISoftKey(OkCallback ok_callback, CancelCallback cancel_callback, std::string tooltip_text, Mode mode)
		: UISoftKey(ok_callback, cancel_callback, tooltip_text, Point<int16_t>(0, 0), mode) {}
	UISoftKey::UISoftKey(OkCallback ok_callback, CancelCallback cancel_callback, std::string tooltip_text)
		: UISoftKey(ok_callback, cancel_callback, tooltip_text, Point<int16_t>(0, 0), Mode::Pic) {}
	UISoftKey::UISoftKey(OkCallback ok_callback, CancelCallback cancel_callback)
		: UISoftKey(ok_callback, cancel_callback, "") {}
	UISoftKey::UISoftKey(OkCallback ok_callback)
		: UISoftKey(ok_callback, []() {}) {}

	void UISoftKey::draw(float inter) const
	{
		UIElement::draw(inter);

		for (uint16_t i = 0; i < CASE_KEYS; i++)
		{
			const Button* btn = BtCaseKeys[i][highCase].get();

			// BeiDou 的 SoftKey.img 缺 BtblankCase：Blank 槽位不会创建按钮，
			// 此处保持为空 unique_ptr，必须跳过（0xC0000005 @ UISoftKey::draw+0x42，
			// mov rax,[rcx] 虚调用 rcx=NULL，2026-10-29 事件日志实证）。
			if (!btn)
				continue;

			btn->draw(position);
		}

		textfield.draw(Point<int16_t>(44, 0));

		if (tooltip)
			tooltip->draw(position + Point<int16_t>(419, 50) + tooltip_pos);
	}

	void UISoftKey::update()
	{
		UIElement::update();

		textfield.update();
		textfield.set_state(Textfield::State::FOCUSED);

		// Text 模式：邀请名 / Admin 命令等为 ASCII。
		// focus_textfield → begin_ascii_text_input 后 UI::send_key 会在
		// text_input_active 时跳过 KeyType::TEXT，只等 SDL_EVENT_TEXT_INPUT。
		// 自动化扫码与部分宿主不产 TEXT_INPUT ⇒ SoftKey 框永远空、邀请发不出，
		// 联调会误判「Messenger/协议坏了」。Text 关掉 StopTextInput，走 KEY TEXT。
		// Pic（二级密码）保持 StartTextInput + 仅鼠标点键，行为不变。
		if (mode == Mode::Text)
			Window::get().end_ascii_text_input();

		if (tooltip)
		{
			if (tooltip_timestep > 0)
				tooltip_timestep -= Constants::TIMESTEP;
			else
				clear_tooltip();
		}
	}

	void UISoftKey::deactivate()
	{
		UI::get().remove(UIElement::Type::SOFTKEYBOARD);
	}

	Cursor::State UISoftKey::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (Cursor::State new_state = textfield.send_cursor(cursorpos, clicked))
			return new_state;

		for (uint16_t i = 0; i < CASE_KEYS; i++)
		{
			auto& btn = BtCaseKeys[i][highCase];

			// 同 draw：BtblankCase 缺失的 Blank 槽位为空，跳过防崩。
			if (!btn)
				continue;

			if (btn->is_active() && btn->bounds(position).contains(cursorpos))
			{
				if (btn->get_state() == Button::State::NORMAL)
				{
					Sound(Sound::Name::BUTTONOVER).play();

					btn->set_state(Button::State::MOUSEOVER);
					return Cursor::State::CANCLICK;
				}
				else if (btn->get_state() == Button::State::MOUSEOVER)
				{
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();

						btn->set_state(case_pressed(i));

						return Cursor::State::IDLE;
					}
					else
					{
						return Cursor::State::CANCLICK;
					}
				}
			}
			else if (btn->get_state() == Button::State::MOUSEOVER)
			{
				btn->set_state(Button::State::NORMAL);
			}
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UISoftKey::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
				button_pressed(Buttons::BtCancel);
			else if (keycode == KeyAction::Id::RETURN)
				button_pressed(Buttons::BtOK);
		}
	}

	UIElement::Type UISoftKey::get_type() const
	{
		return TYPE;
	}

	Button::State UISoftKey::button_pressed(uint16_t buttonid)
	{
		std::string pic = textfield.get_text();
		size_t size = pic.size();

		if (buttonid == Buttons::BtCancel)
		{
			deactivate();

			if (cancel_callback)
				cancel_callback();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtDel)
		{
			if (size > 0)
			{
				pic.pop_back();
				textfield.change_text(pic);
			}

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtOK)
		{
			if (size >= min_text_len)
			{
				// Text 模式跳过 PIC 重复字符等门禁；Pic 仍走 check_pic。
				if (mode == Mode::Text || check_pic())
				{
					deactivate();

					if (ok_callback)
						ok_callback(pic);
				}
			}
			else
			{
				clear_tooltip();
				if (mode == Mode::Pic)
					show_text("The PIC needs to be at least 6 characters long.");
				else
					show_text("Please enter at least 1 character.");
			}

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtShift)
		{
			highCase = !highCase;

			return Button::State::NORMAL;
		}
		else if (buttonid >= Buttons::BtNum0)
		{
			std::string key = row1keys[buttonid - Buttons::BtNum0];

			if (key != "Blank")
				append_key(key);

			return Button::State::NORMAL;
		}
		else
		{
			return Button::State::DISABLED;
		}
	}

	void UISoftKey::show_text(std::string text, uint16_t maxwidth, bool formatted, int16_t line_adj)
	{
		tetooltip.set_text(text, maxwidth, formatted, line_adj);

		if (!text.empty())
		{
			tooltip = tetooltip;
			tooltip_timestep = 7 * 1000;
		}
	}

	void UISoftKey::clear_tooltip()
	{
		tooltip_pos = Point<int16_t>(0, 0);
		tetooltip.set_text("");
		tooltip = Optional<Tooltip>();
	}

	void UISoftKey::append_key(std::string key)
	{
		std::string pic = textfield.get_text();
		size_t size = pic.size();

		if (size < MAX_TEXT_LEN)
		{
			pic.append(key);
			textfield.change_text(pic);
		}
	}

	Point<int16_t> UISoftKey::keypos(uint16_t index, uint16_t row) const
	{
		int16_t x = index * 45;
		int16_t y = row * 43;

		// Third row starts at the third character position
		if (row == 3)
			x += 45 * 2;

		return Point<int16_t>(27 + x, 95 + y);
	}

	Button::State UISoftKey::case_pressed(uint16_t buttonid)
	{
		// Row 2
		if (buttonid >= 0 && buttonid < ROW_MAX)
		{
			std::string string_index = row2keys[buttonid];

			if (string_index != "Blank")
			{
				uint16_t index = std::stoi(string_index);
				std::string key = get_key_from_index(index);

				append_key(key);
			}

			return Button::State::NORMAL;
		}
		// Row 3
		else if (buttonid >= ROW_MAX && buttonid < ROW_MAX * 2)
		{
			std::string string_index = row3keys[buttonid - ROW_MAX];

			if (string_index != "Blank")
			{
				uint16_t index = std::stoi(string_index);
				std::string key = get_key_from_index(index);

				append_key(key);
			}

			return Button::State::NORMAL;
		}
		// Row 4
		else if (buttonid >= ROW_MAX * 2 && buttonid < ROW_MAX * 2 + ROW4_MAX)
		{
			std::string string_index = row4keys[buttonid - ROW_MAX * 2];

			if (string_index != "Blank")
			{
				uint16_t index = std::stoi(string_index);
				std::string key = get_key_from_index(index);

				append_key(key);
			}

			return Button::State::NORMAL;
		}

		return Button::State::DISABLED;
	}

	std::string UISoftKey::get_key_map_index(std::string key)
	{
		for (auto map : KeyMap)
			if (map.second == key)
				return std::to_string(map.first);

		LOG(LOG_DEBUG, "Could not find index for key [" << key << "] in KeyMap.");

		return "Blank";
	}

	std::string UISoftKey::get_key_from_index(uint16_t index)
	{
		auto iter = KeyMap.find(index);

		if (iter != KeyMap.end())
		{
			std::string key = KeyMap.at(index);

			return highCase ? key : string_format::tolower(key);
		}

		LOG(LOG_DEBUG, "Could not find key at index [" << index << "] in KeyMap.");

		return "Blank";
	}

	bool UISoftKey::check_pic()
	{
		const char* pStr = textfield.get_text().c_str();

		if (pStr == NULL)
			return false;

		int count = 0;
		char m = ' ';
		bool reptitive = false;

		while (*pStr)
		{
			if (*pStr == m)
			{
				count++;
			}
			else
			{
				count = 0;
				m = *pStr;
			}

			if (count > 2)
			{
				reptitive = true;
				break;
			}

			pStr++;
		}

		if (reptitive)
		{
			clear_tooltip();
			show_text("Your 2nd password cannot contain three of the same character in a row.", 220, true, 1);

			return false;
		}

		bool requirements = false;

		// TODO: Add check for required amount of characters

		if (requirements)
		{
			clear_tooltip();
			show_text("Your 2nd password must have at least two of the following: uppercase letters, lowercase letters, numbers, and special characters.", 242, true, 1);

			return false;
		}

		return true;
	}
}