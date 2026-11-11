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
#include "../../Graphics/DrawArgument.h"
#include "../../Graphics/Texture.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// V17.7 原生 UIWindow.img/SoftKeyboard 的 37 键编号→字符，
		// 由运行时 UI.nx 位图逐张导出后视觉识读（keys_sheet.png）。
		constexpr char KEY_CHARS[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-";

		// 逐键 window-local top-left。来源 origins.json 的 normal/0/origin：
		//   T = ANCHOR_WINDOW(283, 30) - raw_origin。
		// k36 '-' 与 k35 'Z' 共用 origin(207,-76)，按原生实拍手工错位到数字行第 11 位。
		constexpr int16_t KEY_X[37] =
		{
			232, 25, 48, 71, 94, 117, 140, 163, 186, 209,
			60, 168, 122, 106, 94, 129, 152, 175, 209, 198,
			221, 244, 214, 191, 232, 255, 48, 117, 83, 140,
			186, 145, 71, 99, 163, 76, 255
		};

		constexpr int16_t KEY_Y[37] =
		{
			37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
			83, 106, 106, 83, 60, 83, 83, 83, 60, 83,
			83, 83, 106, 106, 60, 60, 60, 60, 83, 60,
			60, 106, 60, 106, 60, 106, 37
		};
	}

	UISoftKey::UISoftKey(OkCallback ok_callback, CancelCallback cancel_callback, std::string tooltip_text, Point<int16_t> tooltip_pos, Mode mode)
		: UIElement(Point<int16_t>(216, 224), Point<int16_t>(0, 0)),
		  caps_lock(false),
		  shift_pending(false),
		  mode(mode),
		  min_text_len(mode == Mode::Pic ? PIC_MIN_TEXT_LEN : TEXT_MIN_TEXT_LEN),
		  ok_callback(ok_callback),
		  cancel_callback(cancel_callback),
		  tooltip_pos(tooltip_pos)
	{
		nl::node SoftKey = nl::nx::UI["UIWindow.img"]["SoftKeyboard"];

		// 背景三层：卷轴底板 / 标题条 / 键区托盘。
		// 三层位图的 origin 都是画布中心，Sprite 按 origin 回减，必须用
		// DrawArgument(origin + 期望TL) 才能把它们精确摆到窗口 local 坐标
		// （同 UIKeyConfig.cpp:85-95 的处理）。tray 内嵌于 frame 的 (18,30)。
		nl::node backgrnd = SoftKey["backgrnd"]["1"];
		nl::node frame = backgrnd["back"];     // 315×127
		nl::node title = backgrnd["1"];        // 147×34
		nl::node tray = backgrnd["keyboard"];  // 278×95

		if (frame)
			sprites.emplace_back(frame, DrawArgument(Texture(frame).get_origin()));

		if (title)
			sprites.emplace_back(title, DrawArgument(Texture(title).get_origin()));

		if (tray)
			sprites.emplace_back(tray, DrawArgument(Texture(tray).get_origin() + Point<int16_t>(18, 30)));

		// 标题栏右上关闭钮（皮肤 1，14×14）。
		try_maple_btn(buttons, Buttons::BtClose, SoftKey["Bt"]["1"]["BtClose"], Point<int16_t>(299, 2), "softkey close");

		// 功能键 top-left = _preview_layout.py 的 FUNC_TL + TRAY_TL(18,30)。
		nl::node funckey = SoftKey["funckey"];

		try_maple_btn(buttons, Buttons::BtBackspace, funckey["backspace"], Point<int16_t>(274, 36), "softkey backspace");
		try_maple_btn(buttons, Buttons::BtEnter, funckey["enter"], Point<int16_t>(263, 83), "softkey enter");
		try_maple_btn(buttons, Buttons::BtCapsLock, funckey["capslock"], Point<int16_t>(20, 83), "softkey capslock");
		try_maple_btn(buttons, Buttons::BtLShift, funckey["lshift"], Point<int16_t>(20, 106), "softkey lshift");
		try_maple_btn(buttons, Buttons::BtRShift, funckey["rshift"], Point<int16_t>(249, 106), "softkey rshift");

		// 37 键：编号→字符映射与逐键 origin 网格来自 UI.nx 位图实证。
		// try_maple_btn 的 position 语义是期望 top-left，内部自洽抵消 sanitized
		// origin，故键帽恒落在 KEY_X/KEY_Y，键帽 21×22 全装进 315×127 底板。
		nl::node keys = SoftKey["key"];

		for (uint16_t n = 0; n < KEY_COUNT; n++)
			try_maple_btn(buttons, static_cast<uint16_t>(Buttons::BtKey0 + n), keys[std::to_string(n)],
				Point<int16_t>(KEY_X[n], KEY_Y[n]), "softkey key");

		// 输入缓冲：窗口下方紧凑条，取代旧实现漂到 x=419 的散落文本框。
		Point<int16_t> textfield_topleft = position + Point<int16_t>(10, 130);

		textfield = Textfield(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK,
			Rectangle<int16_t>(textfield_topleft, textfield_topleft + Point<int16_t>(200, 18)), MAX_TEXT_LEN);
		// 软键盘输入框支持中文（含"角色名"等需中文的入口）；Pic 密文场景仍按 set_cryptchar 掩码。
		textfield.set_accept_unicode(true);

		// Pic：二级密码密文 + 拦截物理键盘字母数字（原生「只能用鼠标」）。
		// Text：明文 + 不设 text_callback，物理键盘可直接写入 Textfield。
		if (mode == Mode::Pic)
			textfield.set_cryptchar('*');

		textfield.set_enter_callback(
			[&](std::string)
			{
				button_pressed(Buttons::BtEnter);
			}
		);

		textfield.set_key_callback(
			KeyAction::Id::ESCAPE,
			[&]() {
				button_pressed(Buttons::BtClose);
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

		dimension = frame ? Texture(frame).get_dimensions() : Point<int16_t>(315, 127);

		LOG(LOG_DEBUG, "UISoftKey: UIWindow.img/SoftKeyboard mode=" << (mode == Mode::Pic ? "Pic" : "Text")
			<< " min_len=" << min_text_len << " buttons=" << buttons.size());
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
		// 背景 Sprite + 全部键帽/功能键按钮由基类统一绘制（顺序：sprites → buttons）。
		UIElement::draw(inter);

		textfield.draw(Point<int16_t>(0, 0));

		if (tooltip)
			tooltip->draw(position + Point<int16_t>(10, 150) + tooltip_pos);
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

		// 37 键 + 功能键 + 关闭钮全在 buttons 里，hover/click/音效/回调
		// 由基类统一处理（button_pressed 分发到 append_key / 大小写 / 提交）。
		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UISoftKey::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
				button_pressed(Buttons::BtClose);
			else if (keycode == KeyAction::Id::RETURN)
				button_pressed(Buttons::BtEnter);
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

		if (buttonid == Buttons::BtClose)
		{
			deactivate();

			if (cancel_callback)
				cancel_callback();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtBackspace)
		{
			if (size > 0)
			{
				pic.pop_back();
				textfield.change_text(pic);
			}

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtEnter)
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
		else if (buttonid == Buttons::BtCapsLock)
		{
			// CapsLock 持久锁定大写（再点一次解锁）。
			caps_lock = !caps_lock;

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtLShift || buttonid == Buttons::BtRShift)
		{
			// 左/右 Shift 一次性：下一个字母后由 append 分支复位。
			shift_pending = !shift_pending;

			return Button::State::NORMAL;
		}
		else if (buttonid >= Buttons::BtKey0)
		{
			uint16_t n = static_cast<uint16_t>(buttonid - Buttons::BtKey0);

			if (n < KEY_COUNT)
			{
				char c = KEY_CHARS[n];

				// 键帽是大写字形；默认（无 Caps/Shift）输入小写，与旧
				// get_key_from_index 的 highCase?key:tolower(key) 口径一致。
				if (!caps_lock && !shift_pending && c >= 'A' && c <= 'Z')
					c = static_cast<char>(c - 'A' + 'a');

				append_key(std::string(1, c));
			}

			shift_pending = false;

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

	bool UISoftKey::check_pic()
	{
		// 保留旧校验：仅拦「连续 3 个相同字符」。
		std::string text = textfield.get_text();
		const char* pStr = text.c_str();

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
