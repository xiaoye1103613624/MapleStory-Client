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

#include "../UIElement.h"

#include "../Components/Textfield.h"
#include "../Components/TextTooltip.h"

namespace ms
{
	// 虚拟键盘（V17.7 原生 UIWindow.img/SoftKeyboard 宽 QWERTY）。
	// 与 Login.img/Common/SoftKey 是两套原生键盘：后者是 PIC 二级密码竖版随机键盘，
	// 本类曾误用其底板 + Journey 45px/12 列 keypos，键帽漂出底板盖住 Messenger。
	// 现按 UIWindow.img/SoftKeyboard 逐键 origin 网格还原（37 键 + 功能键 + 双皮肤）。
	// Pic：二级密码/PIC —— 密文、最短 6、重复字符校验；Text：自由文本 —— 明文、最短 1、跳过 PIC 校验。
	// 两种模式均可用鼠标点键，也保留物理键盘输入（Text 走 KEY TEXT，见 update()）。
	class UISoftKey : public UIElement
	{
	public:
		using OkCallback = std::function<void(const std::string& entered)>;
		using CancelCallback = std::function<void()>;

		enum class Mode : uint8_t
		{
			Pic,  // 二级密码（默认，保持登录链路行为）
			Text  // 自由文本（Messenger / 密语 / Admin 命令等）
		};

		static constexpr Type TYPE = UIElement::Type::SOFTKEYBOARD;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UISoftKey(OkCallback ok_callback, CancelCallback cancel_callback, std::string tooltip_text, Point<int16_t> tooltip_pos, Mode mode = Mode::Pic);
		UISoftKey(OkCallback ok_callback, CancelCallback cancel_callback, std::string tooltip_text, Mode mode);
		UISoftKey(OkCallback ok_callback, CancelCallback cancel_callback, std::string tooltip_text);
		UISoftKey(OkCallback ok_callback, CancelCallback cancel_callback);
		UISoftKey(OkCallback ok_callback);

		void draw(float inter) const override;
		void update() override;

		void deactivate();

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		void show_text(std::string text, uint16_t maxwidth = 0, bool formatted = true, int16_t line_adj = 0);
		void clear_tooltip();
		void append_key(std::string key);
		bool check_pic();

		enum Buttons : uint16_t
		{
			BtClose,
			BtBackspace,
			BtEnter,
			BtCapsLock,
			BtLShift,
			BtRShift,
			BtKey0
		};

		static constexpr uint16_t KEY_COUNT = 37;
		static constexpr size_t PIC_MIN_TEXT_LEN = 6;
		static constexpr size_t TEXT_MIN_TEXT_LEN = 1;
		static constexpr size_t MAX_TEXT_LEN = 16;

		bool caps_lock;      // CapsLock：持久锁定大写
		bool shift_pending;  // 左/右 Shift：下一个字母后自动复位
		Mode mode;
		size_t min_text_len;

		OkCallback ok_callback;
		CancelCallback cancel_callback;

		Textfield textfield;

		int16_t tooltip_timestep = 0;
		TextTooltip tetooltip;
		Optional<Tooltip> tooltip;
		Point<int16_t> tooltip_pos;
	};
}
