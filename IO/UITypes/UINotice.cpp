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

#include "UISoftKey.h"

#include "../UI.h"

#include "../Components/MapleButton.h"

#include "../../Audio/Audio.h"
#include "../../Util/Misc.h"
#include "../../Util/ResolutionTiers.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UINotice::UINotice(std::string message, NoticeType t, Text::Alignment a) : UIDragElement<PosNOTICE>(), type(t), alignment(a)
	{
		nl::node src = nl::nx::UI["Basic.img"]["Notice6"];

		top = src["t"];
		center = src["c"];
		centerbox = src["c_box"];
		box = src["box"];
		box2 = src["box2"];
		bottom = src["s"];
		bottombox = src["s_box"];

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
		dimension = Point<int16_t>(top.width(), top.height() + height + bottom.height());
		position = Point<int16_t>(position.x() - dimension.x() / 2, position.y() - dimension.y() / 2);
		dragarea = Point<int16_t>(dimension.x(), 20);

		if (type != NoticeType::ENTERNUMBER)
			Sound(Sound::Name::DLGNOTICE).play();

		// 多分辨率：PosNOTICE(400,285) 是 800×600 时代按中心点记的常量，在其它档位
		// 会明显偏左上。非 1280×720 视口下按视口居中；1280×720 保留原生绝对位置。
		if (Constants::Constants::get().get_viewwidth() != ResolutionTiers::LOGIN_W
			|| Constants::Constants::get().get_viewheight() != ResolutionTiers::LOGIN_H)
			center_on_screen();
	}

	UINotice::UINotice(std::string message, NoticeType t) : UINotice(message, t, Text::Alignment::CENTER) {}

	void UINotice::update_screen(int16_t, int16_t)
	{
		// 弹窗族（UINotice / UIYesNo / UIEnterNumber / UIOk）在实时切换分辨率时居中重锚。
		center_on_screen();
	}

	void UINotice::draw(bool textfield) const
	{
		Point<int16_t> start = position;

		top.draw(start);
		start.shift_y(top.height());

		if (textfield)
		{
			center.draw(start);
			start.shift_y(center.height());
			centerbox.draw(start);
			start.shift_y(centerbox.height() - 1);
			box2.draw(start);
			start.shift_y(box2.height());
			box.draw(DrawArgument(start, Point<int16_t>(0, 29)));
			start.shift_y(29);

			question.draw(position + Point<int16_t>(13, 13));
		}
		else
		{
			int16_t pos_y = height >= 32 ? height : 32;

			center.draw(DrawArgument(start, Point<int16_t>(0, pos_y)));
			start.shift_y(pos_y);
			centerbox.draw(start);
			start.shift_y(centerbox.height());
			box.draw(start);
			start.shift_y(box.height());

			if (type == NoticeType::OK)
				tip_question.draw(position + Point<int16_t>(131, 14));
			else if (type == NoticeType::YESNO && alignment == Text::Alignment::LEFT)
				question.draw(position + Point<int16_t>(31, 14));
			else
				question.draw(position + Point<int16_t>(131, 14));
		}

		bottombox.draw(start);
	}

	int16_t UINotice::box2offset(bool textfield) const
	{
		int16_t offset = top.height() + centerbox.height() + box.height() + height - (textfield ? 0 : 16);

		if (type == NoticeType::OK)
			if (height < 34)
				offset += 15;

		return offset;
	}

	UIYesNo::UIYesNo(std::string message, std::function<void(bool yes)> yh, Text::Alignment alignment) : UINotice(message, NoticeType::YESNO, alignment)
	{
		yesnohandler = yh;

		int16_t belowtext = box2offset(false);

		nl::node src = nl::nx::UI["Basic.img"];

		buttons[Buttons::YES] = std::make_unique<MapleButton>(src["BtOK4"], Point<int16_t>(156, belowtext));
		buttons[Buttons::NO] = std::make_unique<MapleButton>(src["BtCancel4"], Point<int16_t>(198, belowtext));
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
		else if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
			|| keycode == 'Y' || keycode == 'y' || keycode == 'N' || keycode == 'n')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				if (t == "y" || t == "yes" || t == "ok" || t == "1")
					button_pressed(Buttons::YES);
				else if (t == "n" || t == "no" || t == "0")
					button_pressed(Buttons::NO);
			}, []() {}, "y|n");
		}
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

		int16_t belowtext = box2offset(true) - 21;
		int16_t pos_y = belowtext + 35;

		nl::node src = nl::nx::UI["Basic.img"];

		buttons[Buttons::OK] = std::make_unique<MapleButton>(src["BtOK4"], 156, pos_y);
		buttons[Buttons::CANCEL] = std::make_unique<MapleButton>(src["BtCancel4"], 198, pos_y);

		Point<int16_t> numfield_pos = position + Point<int16_t>(22, belowtext + 5);
		Point<int16_t> numfield_dim = Point<int16_t>(217, 16);

		int16_t numfield_limit = 10;

		numfield = Textfield(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::LIGHTGREY, Rectangle<int16_t>(numfield_pos, numfield_pos + numfield_dim), numfield_limit);
		numfield.change_text(std::to_string(quantity));

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
	}

	void UIEnterNumber::draw(float alpha) const
	{
		UINotice::draw(true);
		UIElement::draw(alpha);

		numfield.draw(Point<int16_t>(2, -4));
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
			Cursor::State nstate = numfield.send_cursor(cursorpos, clicked);

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
		else if (keycode == 'S' || keycode == 's' || keycode == 'N' || keycode == 'n')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				if (s.empty()) return;
				numfield.change_text(s);
				handlestring(s);
				deactivate();
				LOG(LOG_DEBUG, "UIEnterNumber: SoftKey number=" << s);
			}, []() {}, "Enter number");
		}
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
			UI::get().emplace<UIOk>("You may only enter a number equal to or higher than 1.", okhandler);
			return;
		}
		else if (num > max)
		{
			numfield.set_state(Textfield::State::DISABLED);
			UI::get().emplace<UIOk>("You may only enter a number equal to or lower than " + std::to_string(max) + ".", okhandler);
			return;
		}
		else
		{
			numhandler(num);
			deactivate();
		}

		buttons[Buttons::OK]->set_state(Button::State::NORMAL);
	}

	UIOk::UIOk(std::string message, std::function<void(bool ok)> oh) : UINotice(message, NoticeType::OK)
	{
		okhandler = oh;

		nl::node src = nl::nx::UI["Basic.img"];

		buttons[Buttons::OK] = std::make_unique<MapleButton>(src["BtOK4"], 197, box2offset(false));
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
			else if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
				|| keycode == 'Y' || keycode == 'y' || keycode == 'O' || keycode == 'o')
			{
				UI::get().emplace<UISoftKey>([this](const std::string& tip) {
					if (tip.empty()) return;
					std::string t = tip;
					for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
					if (t == "y" || t == "yes" || t == "ok" || t == "1")
						button_pressed(Buttons::OK);
					else if (t == "n" || t == "no" || t == "0" || t == "cancel") {
						okhandler(false);
						deactivate();
					}
				}, []() {}, "ok|cancel");
			}
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