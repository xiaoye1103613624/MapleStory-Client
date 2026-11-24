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
#include "UINotice.h"

#include "../UI.h"

#include "../Components/MapleButton.h"

#include "../../Audio/Audio.h"
#include "../../Util/Misc.h"
#include "../../Util/Locale.h"
#include "../../Util/ResolutionTiers.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UINotice::UINotice(std::string message, NoticeType t, Text::Alignment a) : UIDragElement<PosNOTICE>(), type(t), alignment(a)
	{
		// 2026-11-16 弹窗全不显示根因修复：
		// 原实现照抄 Angel 083 的 Basic.img/Notice6（子键 t/c/c_box/box/box2/s_box），
		// 但 live（BeiDou/095）Basic.img 只有 Notice~Notice5，按钮只有 BtOK/2/3 与
		// BtCancel/2/3 —— Notice6 / BtOK4 / BtCancel4 全部缺失，背景与按钮纹理为空，
		// dimension 退化为 0，弹窗不可见。改用 live 存在的 Notice3（提示）/ Notice4
		// （数字输入，含 s_line 白框）三段式 t + N×c + s，口径与 MinMS 一致。
		nl::node basic = nl::nx::UI["Basic.img"];
		nl::node src = basic[type == NoticeType::ENTERNUMBER ? "Notice4" : "Notice3"];

		top = src["t"];
		center = src["c"];
		if (type == NoticeType::ENTERNUMBER && src["s_line"])
			bottom = src["s_line"];
		else
			bottom = src["s"];

		if (type == NoticeType::YESNO)
		{
			position.shift_y(-8);
			question = Text(Text::Font::A11M, alignment, Color::Name::WHITE, message, 200);
		}
		else if (type == NoticeType::ENTERNUMBER)
		{
			position.shift_y(-16);
			question = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, message, 200);
		}
		else if (type == NoticeType::OK)
		{
			// IDA sub_9929DD / Draw sub_99311B：duration==1000 → style19 白字(0xFFFFFFFF)+style20 描边(0xFF355A84)，字号 0x0C=12。
			// α=1.0；本仓字体路径保持思源宋体（不跟原生 Arialdf）。
			uint16_t maxwidth = top.width() - 6;
			std::string tip = message;
			for (size_t i = 0; i + 1 < tip.size();)
			{
				if (tip[i] == '\r' && tip[i + 1] == '\n')
					tip.erase(i, 1);
				else
					++i;
			}

			position.shift_y(-8);
			// OutlinedText 默认无 maxwidth；五路同参 Text 保证折行一致。
			tip_question.inner = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, tip, maxwidth);
			tip_question.l = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::NOTICEOUTLINE, tip, maxwidth);
			tip_question.r = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::NOTICEOUTLINE, tip, maxwidth);
			tip_question.t = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::NOTICEOUTLINE, tip, maxwidth);
			tip_question.b = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::NOTICEOUTLINE, tip, maxwidth);
			question = tip_question.inner;
		}

		height = question.height();
		if (height < 1)
			height = 1;
		int16_t tile_h = center.height() > 0 ? center.height() : 20;
		center_tiles = (height + tile_h - 1) / tile_h;
		if (center_tiles < 1)
			center_tiles = 1;
		int16_t panel_w = top.width() > 0 ? top.width() : 266;
		int16_t panel_h = top.height() + center_tiles * tile_h + bottom.height();
		dimension = Point<int16_t>(panel_w, panel_h);
		position = Point<int16_t>(position.x() - dimension.x() / 2, position.y() - dimension.y() / 2);
		dragarea = Point<int16_t>(dimension.x(), 20);

		if (type != NoticeType::ENTERNUMBER)
			Sound(Sound::Name::DLGNOTICE).play();

		// 居中策略：弹窗一律按当前视口居中（不再依赖 PosNOTICE 的绝对锚点）。
		// 旧实现仅非 1280×720 才 center_on_screen，1280×720 落到 :97 的手动减半 +
		// ENTERNUMBER 的 shift_y(-16) 偏移 → 弹窗偏上、不在正中（用户「弹窗没居中」）。
		// center_on_screen 会用 dimension 重算 position，覆盖上述偏移，四类弹窗统一归位。
		set_layout_policy(LayoutPolicy::Center);
		center_on_screen();
	}

	UINotice::UINotice(std::string message, NoticeType t) : UINotice(message, t, Text::Alignment::CENTER) {}

	void UINotice::update_screen(int16_t new_width, int16_t new_height)
	{
		UIElement::update_screen(new_width, new_height);
	}

	void UINotice::draw(bool textfield) const
	{
		Point<int16_t> start = position;

		top.draw(start);
		start.shift_y(top.height());

		// Notice3/4 三段式：t + N×c + s，替代缺失的 Notice6 专有子键。
		const int16_t tile_h = center.height() > 0 ? center.height() : 20;
		for (int16_t i = 0; i < center_tiles; ++i)
		{
			center.draw(start);
			start.shift_y(tile_h);
		}

		// 文字在 c 平铺区垂直居中；水平沿用原坐标（面板同为 266 宽）。
		// 保留本端 OK 描边字（tip_question）与 LEFT/CENTER 对齐分支，不照搬 MinMS 缩放。
		int16_t text_y = top.height() + (center_tiles * tile_h - height) / 2;
		if (text_y < top.height())
			text_y = top.height();

		if (textfield)
			question.draw(position + Point<int16_t>(13, text_y));
		else if (type == NoticeType::OK)
			tip_question.draw(position + Point<int16_t>(131, text_y));
		else if (type == NoticeType::YESNO && alignment == Text::Alignment::LEFT)
			question.draw(position + Point<int16_t>(31, text_y));
		else
			question.draw(position + Point<int16_t>(131, text_y));

		bottom.draw(start);
	}

	int16_t UINotice::panel_bottom() const
	{
		const int16_t tile_h = center.height() > 0 ? center.height() : 20;
		return static_cast<int16_t>(top.height() + center_tiles * tile_h);
	}

	int16_t UINotice::button_row_y() const
	{
		// BtOK2 / BtCancel2 原始高度 19px；在 s 段内垂直居中。
		constexpr int16_t BTN_H = 19;
		const int16_t band = bottom.height() > BTN_H ? bottom.height() : BTN_H;
		return static_cast<int16_t>(panel_bottom() + (band - BTN_H) / 2);
	}

	int16_t UINotice::box_width() const
	{
		return dimension.x();
	}

	UIYesNo::UIYesNo(std::string message, std::function<void(bool yes)> yh, Text::Alignment alignment) : UINotice(message, NoticeType::YESNO, alignment)
	{
		yesnohandler = yh;

		nl::node basic = nl::nx::UI["Basic.img"];
		const int16_t by = button_row_y();
		const int16_t btns_w = 47;   // BtOK2 / BtCancel2 normal 帧宽
		const int16_t right_margin = 21;
		const int16_t gap = 10;
		const int16_t cancel_x = static_cast<int16_t>(box_width() - right_margin - btns_w);
		const int16_t ok_x = static_cast<int16_t>(cancel_x - gap - btns_w);

		// 用 live 存在的 BtOK2 / BtCancel2 替换缺失的 BtOK4 / BtCancel4。
		buttons[Buttons::YES] = std::make_unique<MapleButton>(basic["BtOK2"], Point<int16_t>(ok_x, by));
		buttons[Buttons::NO] = std::make_unique<MapleButton>(basic["BtCancel2"], Point<int16_t>(cancel_x, by));
	}

	UIYesNo::UIYesNo(std::string message, std::function<void(bool yes)> yesnohandler) : UIYesNo(message, yesnohandler, Text::Alignment::CENTER) {}

	void UIYesNo::draw(float alpha) const
	{
		UINotice::draw(false);
		UIElement::draw(alpha);
	}

	void UIYesNo::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (keycode == KeyAction::Id::RETURN)
		{
			yesnohandler(true);
			deactivate();
		}
		else if (escape)
		{
			yesnohandler(false);
			deactivate();
		}
		// 2026-11-11：移除 S/T/Y/N → SoftKey 残枝。原生 YesNo 只认 RETURN / ESC；
		// 这些字母捷径会把「点确认」劫持成软键盘，破坏正常弹窗逻辑。
	}

	UIElement::Type UIYesNo::get_type() const
	{
		return TYPE;
	}

	Button::State UIYesNo::button_pressed(uint16_t buttonid)
	{
		deactivate();

		switch (buttonid)
		{
			case Buttons::YES:
				yesnohandler(true);
				break;
			case Buttons::NO:
				yesnohandler(false);
				break;
		}

		return Button::State::PRESSED;
	}

	UIEnterNumber::UIEnterNumber(std::string message, std::function<void(int32_t)> nh, int32_t m, int32_t quantity) : UINotice(message, NoticeType::ENTERNUMBER)
	{
		numhandler = nh;
		max = m;

		nl::node basic = nl::nx::UI["Basic.img"];
		const int16_t by = button_row_y();
		const int16_t btns_w = 47;
		const int16_t right_margin = 21;
		const int16_t gap = 10;
		const int16_t cancel_x = static_cast<int16_t>(box_width() - right_margin - btns_w);
		const int16_t ok_x = static_cast<int16_t>(cancel_x - gap - btns_w);

		buttons[Buttons::OK] = std::make_unique<MapleButton>(basic["BtOK2"], Point<int16_t>(ok_x, by));
		buttons[Buttons::CANCEL] = std::make_unique<MapleButton>(basic["BtCancel2"], Point<int16_t>(cancel_x, by));

		// Notice4/s_line 白色输入框：bottom 画在 panel_bottom() 起的水平台，白框内区约 (7,9)-(259,28)。
		// numfield 用「相对弹窗左上角」的局部坐标，draw() 时统一加 position，避免 center_on_screen
		// 重算 position 后 bounds 仍停在旧位置（弹窗居中后输入框飘走/数量不可见的根因）。
		Point<int16_t> numfield_pos = Point<int16_t>(10, panel_bottom() + 10);
		Point<int16_t> numfield_dim = Point<int16_t>(240, 16);

		int16_t numfield_limit = 10;

		numfield = Textfield(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Rectangle<int16_t>(numfield_pos, numfield_pos + numfield_dim), numfield_limit);
		numfield.set_digits_only(true);

		numfield.set_enter_callback(
			[&](std::string numstr)
			{
				handlestring(numstr);
			}
		);

		numfield.set_key_callback(
			KeyAction::Id::ESCAPE,
			[&]()
			{
				deactivate();
			}
		);

		numfield.set_state(Textfield::State::FOCUSED);
		// 初值在 set_state(FOCUSED) 之后设置：focus_textfield 只切 IME 状态不清文本，
		// 但放在最后确保不被任何焦点切换路径覆盖（用户「数量没显示」加固）。
		numfield.change_text(std::to_string(quantity));
	}

	void UIEnterNumber::draw(float alpha) const
	{
		UINotice::draw(true);
		UIElement::draw(alpha);

		// numfield bounds 是相对弹窗的局部坐标，绘制基准统一加 position（与居中/拖动后位置一致）。
		numfield.draw(position);
	}

	void UIEnterNumber::update()
	{
		UIElement::update();

		numfield.update();
	}

	Cursor::State UIEnterNumber::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (numfield.get_state() == Textfield::State::NORMAL)
		{
			// numfield bounds 为相对弹窗局部坐标，命中判定换算到局部系。
			Cursor::State nstate = numfield.send_cursor(cursorpos - position, clicked);

			if (nstate != Cursor::State::IDLE)
				return nstate;
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UIEnterNumber::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (keycode == KeyAction::Id::RETURN)
		{
			handlestring(numfield.get_text());
			deactivate();
		}
		else if (escape)
		{
			deactivate();
		}
		// 2026-11-11：移除 S/N → SoftKey。数字框本身已聚焦接收键盘输入。
	}

	UIElement::Type UIEnterNumber::get_type() const
	{
		return TYPE;
	}

	Button::State UIEnterNumber::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::OK:
				handlestring(numfield.get_text());
				break;
			case Buttons::CANCEL:
				deactivate();
				break;
		}

		return Button::State::NORMAL;
	}

	void UIEnterNumber::handlestring(std::string numstr)
	{
		int num = -1;
		// 空串的 find_first_not_of 返回 npos（vacuous true），会让下面的 stoi("") 抛 invalid_argument：
		// 退格清空数字框后按 RETURN / 点 OK / Textfield enter_callback 都会走到这里，必须显式挡住。
		// 加 !empty() 后空串复用既有 "Only numbers are allowed." 提示，合法数字路径逐字不变。
		bool has_only_digits = !numstr.empty()
			&& (numstr.find_first_not_of("0123456789") == std::string::npos);

		auto okhandler = [&](bool)
		{
			numfield.set_state(Textfield::State::FOCUSED);
			buttons[Buttons::OK]->set_state(Button::State::NORMAL);
		};

		if (!has_only_digits)
		{
			numfield.set_state(Textfield::State::DISABLED);
			UI::get().emplace<UIOk>("Only numbers are allowed.", okhandler);
			return;
		}
		else
		{
			// numfield 允许 10 字符（构造期 numfield_limit = 10），纯数字也可能溢出 int32
			// （例如 9999999999 -> std::stoi 抛 out_of_range）。统一收口到同一提示，不崩交互。
			try
			{
				num = std::stoi(numstr);
			}
			catch (...)
			{
				numfield.set_state(Textfield::State::DISABLED);
				UI::get().emplace<UIOk>("Only numbers are allowed.", okhandler);
				return;
			}
		}

		if (num < 1)
		{
			numfield.set_state(Textfield::State::DISABLED);
			UI::get().emplace<UIOk>(Locale::text("uinotice.num_min1"), okhandler);
			return;
		}
		else if (num > max)
		{
			numfield.set_state(Textfield::State::DISABLED);
			UI::get().emplace<UIOk>(Locale::format("uinotice.num_max", std::to_string(max)), okhandler);
			return;
		}
		else
		{
			numhandler(num);
			deactivate();
		}

		buttons[Buttons::OK]->set_state(Button::State::NORMAL);
	}

	UIEnterString::UIEnterString(std::string message, std::function<void(std::string)> handler,
		size_t limit, bool accept_unicode)
		: UINotice(message, NoticeType::ENTERNUMBER)
	{
		strhandler = std::move(handler);
		maxlen = limit > 0 ? limit : 12;

		nl::node basic = nl::nx::UI["Basic.img"];
		const int16_t by = button_row_y();
		const int16_t btns_w = 47;
		const int16_t right_margin = 21;
		const int16_t gap = 10;
		const int16_t cancel_x = static_cast<int16_t>(box_width() - right_margin - btns_w);
		const int16_t ok_x = static_cast<int16_t>(cancel_x - gap - btns_w);

		buttons[Buttons::OK] = std::make_unique<MapleButton>(basic["BtOK2"], Point<int16_t>(ok_x, by));
		buttons[Buttons::CANCEL] = std::make_unique<MapleButton>(basic["BtCancel2"], Point<int16_t>(cancel_x, by));

		Point<int16_t> field_pos = Point<int16_t>(10, panel_bottom() + 10);
		Point<int16_t> field_dim = Point<int16_t>(240, 16);

		strfield = Textfield(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
			Rectangle<int16_t>(field_pos, field_pos + field_dim), maxlen);
		if (accept_unicode)
			strfield.set_accept_unicode(true);

		strfield.set_enter_callback(
			[&](std::string text)
			{
				handlestring(std::move(text));
			}
		);
		strfield.set_key_callback(
			KeyAction::Id::ESCAPE,
			[&]()
			{
				deactivate();
			}
		);
		strfield.set_state(Textfield::State::FOCUSED);
	}

	void UIEnterString::draw(float alpha) const
	{
		UINotice::draw(true);
		UIElement::draw(alpha);
		strfield.draw(position);
	}

	void UIEnterString::update()
	{
		UIElement::update();
		strfield.update();
	}

	Cursor::State UIEnterString::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (strfield.get_state() == Textfield::State::NORMAL)
		{
			Cursor::State nstate = strfield.send_cursor(cursorpos - position, clicked);
			if (nstate != Cursor::State::IDLE)
				return nstate;
		}
		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UIEnterString::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (keycode == KeyAction::Id::RETURN)
		{
			handlestring(strfield.get_text());
		}
		else if (escape)
		{
			deactivate();
		}
	}

	UIElement::Type UIEnterString::get_type() const
	{
		return TYPE;
	}

	Button::State UIEnterString::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::OK:
				handlestring(strfield.get_text());
				break;
			case Buttons::CANCEL:
				deactivate();
				break;
		}
		return Button::State::NORMAL;
	}

	void UIEnterString::handlestring(std::string text)
	{
		// trim ends
		while (!text.empty() && (text.back() == ' ' || text.back() == '\t'))
			text.pop_back();
		size_t start = 0;
		while (start < text.size() && (text[start] == ' ' || text[start] == '\t'))
			++start;
		if (start > 0)
			text = text.substr(start);

		if (text.empty())
			return;
		if (text.size() > maxlen)
			text.resize(maxlen);

		if (strhandler)
			strhandler(text);
		deactivate();
	}

	UIOk::UIOk(std::string message, std::function<void(bool ok)> oh) : UINotice(message, NoticeType::OK)
	{
		okhandler = oh;

		nl::node basic = nl::nx::UI["Basic.img"];

		buttons[Buttons::OK] = std::make_unique<MapleButton>(basic["BtOK2"],
			Point<int16_t>(box_width() - 21 - 47, button_row_y()));
	}

	void UIOk::draw(float alpha) const
	{
		UINotice::draw(false);
		UIElement::draw(alpha);
	}

	void UIOk::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (keycode == KeyAction::Id::RETURN)
			{
				okhandler(true);
				deactivate();
			}
			else if (escape)
			{
				okhandler(false);
				deactivate();
			}
			// 2026-11-11：移除 S/T/Y/O → SoftKey。原生 UIOk 只认 RETURN / ESC。
		}
	}

	UIElement::Type UIOk::get_type() const
	{
		return TYPE;
	}

	Button::State UIOk::button_pressed(uint16_t buttonid)
	{
		deactivate();

		switch (buttonid)
		{
			case Buttons::OK:
				okhandler(true);
				break;
		}

		return Button::State::NORMAL;
	}
}