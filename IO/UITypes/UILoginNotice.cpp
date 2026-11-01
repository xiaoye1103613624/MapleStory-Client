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
#include "UILoginNotice.h"

#include "UISoftKey.h"

#include "../UI.h"

#include "../Components/MapleButton.h"

#include "../../Audio/Audio.h"
#include "../../Graphics/Text.h"
#include "../../Util/Locale.h"
#include "../../Util/LoginUiAssets.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#include <algorithm>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UILoginNotice::UILoginNotice(uint16_t message, std::function<void()> okhandler, std::function<void()> cancelhandler) : okhandler(okhandler), cancelhandler(cancelhandler)
	{
		multiple = false;

		nl::node Notice = nl::nx::UI["Login.img"]["Notice"];
		nl::node backgrnd;

		switch (message)
		{
			case Message::NAME_IN_USE:
			case Message::ILLEGAL_NAME:
			case Message::BLOCKED_ID:
			case Message::INCORRECT_PIC:
				backgrnd = Notice["backgrnd"]["1"];
				break;
			default:
				backgrnd = Notice["backgrnd"]["0"];
				break;
		}

		Texture bg_tex(backgrnd);

		// 参考端渲染 = 羊皮纸 backgrnd(362×219，左上角烘焙蘑菇印章) + 文案板。
		// 原生布局（上游 continued-journey 权威坐标，写死于原生客户端）：
		//   文案板 text/N(221×109) 固定贴 (17,13)；
		//   单按钮 BtYes(75×32) 在 (100,106)，双按钮 BtYes(70,106)/BtNo(130,106)。
		// 旧改法把文案板水平居中 + y=68、按钮贴底计算（y≈163）——文案板盖住印章、
		// 按钮坠到卷角上，整体错位（密码错误弹窗 UI 不对的根因）。
		dimension = bg_tex.get_dimensions();

		sprites.emplace_back(backgrnd);
		sprites.emplace_back(Notice["text"][message], Point<int16_t>(17, 13));

		if (message == Message::DELETE_CONFIRMATION || message == Message::CASH_ITEMS_CONFIRM_DELETION)
		{
			multiple = true;

			buttons[Buttons::YES] = std::make_unique<MapleButton>(Notice["BtYes"], Point<int16_t>(70, 106));
			buttons[Buttons::NO] = std::make_unique<MapleButton>(Notice["BtNo"], Point<int16_t>(130, 106));
		}
		else
		{
			buttons[Buttons::YES] = std::make_unique<MapleButton>(Notice["BtYes"], Point<int16_t>(100, 106));
		}

		center_on_screen();
	}

	UILoginNotice::UILoginNotice(uint16_t message, std::function<void()> okhandler) : UILoginNotice(message, okhandler, []() {}) {}
	UILoginNotice::UILoginNotice(uint16_t message) : UILoginNotice(message, []() {}) {}

	void UILoginNotice::update_screen(int16_t, int16_t)
	{
		center_on_screen();
	}

	void UILoginNotice::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				if (!multiple)
					okhandler();
				else
					cancelhandler();

				deactivate();
			}
			else if (keycode == KeyAction::RETURN)
			{
				okhandler();
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
	}

	UIElement::Type UILoginNotice::get_type() const
	{
		return TYPE;
	}

	Button::State UILoginNotice::button_pressed(uint16_t buttonid)
	{
		if (buttonid == Buttons::YES)
			okhandler();
		else if (buttonid == Buttons::NO)
			cancelhandler();

		deactivate();

		return Button::State::NORMAL;
	}

	UIQuitConfirm::UIQuitConfirm()
	{
		nl::node notice = nl::nx::UI["Login.img"]["Notice"];
		nl::node backgrnd = notice["backgrnd"]["0"];
		nl::node bt_yes = notice["BtYes"];
		nl::node bt_no = notice["BtNo"];

		Texture bg_tex(backgrnd);
		Texture yes_tex(bt_yes["normal"]["0"]);

		dimension = bg_tex.get_dimensions();
		const int16_t bg_w = dimension.x();
		const int16_t bg_h = dimension.y();

		// 居中布局（用户反馈：文字与按钮偏左下）。板 362x219，
		// BtYes/BtNo 各 75x32——旧坐标 (68,106)/(129,106) 还互相重叠 14px。
		constexpr int16_t btn_w = 75;
		constexpr int16_t btn_gap = 13;
		constexpr int16_t btn_y = 122;
		const int16_t btn_pair_x = static_cast<int16_t>((bg_w - btn_w * 2 - btn_gap) / 2);

		confirm_text = Text(
			Text::Font::A12M,
			Text::Alignment::CENTER,
			Color::Name::DARKGREY,
			Locale::text("login.quit_confirm"),
			300
		);

		// 文案水平居中于整板、略高于中线，给下方按钮留空间。
		const int16_t text_h = confirm_text.height();
		text_pos = Point<int16_t>(
			static_cast<int16_t>(bg_w / 2),
			static_cast<int16_t>((bg_h - text_h) / 2 - 22)
		);

		sprites.emplace_back(backgrnd);

		buttons[Buttons::BT_OK] = std::make_unique<MapleButton>(
			bt_yes, Point<int16_t>(btn_pair_x, btn_y));
		buttons[Buttons::BT_CANCEL] = std::make_unique<MapleButton>(
			bt_no, Point<int16_t>(static_cast<int16_t>(btn_pair_x + btn_w + btn_gap), btn_y));

		center_on_screen();
	}

	void UIQuitConfirm::draw(float alpha) const
	{
		UIElement::draw(alpha);
		confirm_text.draw(position + text_pos);
	}

	void UIQuitConfirm::update_screen(int16_t, int16_t)
	{
		center_on_screen();
	}

	void UIQuitConfirm::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				deactivate();
			}
			else if (keycode == KeyAction::RETURN)
			{
				UI::get().quit();
				deactivate();
			}
			else if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
				|| keycode == 'Y' || keycode == 'y' || keycode == 'N' || keycode == 'n')
			{
				UI::get().emplace<UISoftKey>([this](const std::string& tip) {
					if (tip.empty()) return;
					std::string t = tip;
					for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
					if (t == "y" || t == "yes" || t == "ok" || t == "1" || t == "quit")
						button_pressed(BT_OK);
					else if (t == "n" || t == "no" || t == "0" || t == "cancel")
						deactivate();
				}, []() {}, "quit|cancel");
			}
		}
	}

	UIElement::Type UIQuitConfirm::get_type() const
	{
		return TYPE;
	}

	Button::State UIQuitConfirm::button_pressed(uint16_t buttonid)
	{
		if (buttonid == BT_OK)
			UI::get().quit();

		deactivate();

		return Button::PRESSED;
	}

	UIClassConfirm::UIClassConfirm(uint16_t selected_class, bool unavailable, std::function<void()> okhandler) : okhandler(okhandler)
	{
		nl::node RaceSelect = nl::nx::UI["Login.img"]["RaceSelect_new"];
		nl::node type = unavailable ? RaceSelect["deny"] : RaceSelect["confirm"];
		nl::node backgrnd = type["backgrnd"];

		// BeiDou has no RaceSelect_new confirm/deny — fall back to Login Notice board.
		if (!backgrnd)
		{
			LoginUiAssets::log_info("[ClassConfirm] RaceSelect_new missing; using Notice fallback");

			nl::node notice = nl::nx::UI["Login.img"]["Notice"];
			backgrnd = notice["backgrnd"]["0"];
			sprites.emplace_back(backgrnd);

			buttons[Buttons::OK] = std::make_unique<MapleButton>(notice["BtYes"], Point<int16_t>(68, 106));
			if (!unavailable)
				buttons[Buttons::CANCEL] = std::make_unique<MapleButton>(notice["BtNo"], Point<int16_t>(129, 106));

			dimension = Texture(backgrnd).get_dimensions();
			center_on_screen();
			return;
		}

		nl::node race = type["race"][selected_class];

		int16_t backgrnd_x = Texture(backgrnd).get_dimensions().x();
		int16_t race_x = Texture(race).get_dimensions().x();

		int16_t pos_x = (backgrnd_x - race_x) / 2;

		sprites.emplace_back(backgrnd);
		sprites.emplace_back(race, Point<int16_t>(pos_x, 10) + (Point<int16_t>)race["origin"]);

		if (unavailable)
		{
			buttons[Buttons::OK] = std::make_unique<MapleButton>(type["BtOK"]);
		}
		else
		{
			buttons[Buttons::OK] = std::make_unique<MapleButton>(type["BtOK"], Point<int16_t>(62, 107));
			buttons[Buttons::CANCEL] = std::make_unique<MapleButton>(type["BtCancel"], Point<int16_t>(137, 107));
		}

		dimension = Texture(backgrnd).get_dimensions();
		center_on_screen();
	}

	void UIClassConfirm::update_screen(int16_t, int16_t)
	{
		center_on_screen();
	}

	Cursor::State UIClassConfirm::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		for (auto& btit : buttons)
		{
			if (btit.second->is_active() && btit.second->bounds(position).contains(cursorpos))
			{
				if (btit.second->get_state() == Button::State::NORMAL)
				{
					Sound(Sound::Name::BUTTONOVER).play();

					btit.second->set_state(Button::State::MOUSEOVER);
				}
				else if (btit.second->get_state() == Button::State::MOUSEOVER)
				{
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();

						btit.second->set_state(button_pressed(btit.first));
					}
				}
			}
			else if (btit.second->get_state() == Button::State::MOUSEOVER)
			{
				btit.second->set_state(Button::State::NORMAL);
			}
		}

		return Cursor::State::LEAF;
	}

	void UIClassConfirm::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
				deactivate();
			else if (keycode == KeyAction::Id::RETURN)
				button_pressed(Buttons::OK);
			else if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
				|| keycode == 'Y' || keycode == 'y' || keycode == 'N' || keycode == 'n')
			{
				UI::get().emplace<UISoftKey>([this](const std::string& tip) {
					if (tip.empty()) return;
					std::string t = tip;
					for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
					if (t == "y" || t == "yes" || t == "ok" || t == "1")
						button_pressed(Buttons::OK);
					else if (t == "n" || t == "no" || t == "0")
						deactivate();
				}, []() {}, "y|n");
			}
		}
	}

	UIElement::Type UIClassConfirm::get_type() const
	{
		return TYPE;
	}

	Button::State UIClassConfirm::button_pressed(uint16_t buttonid)
	{
		deactivate();

		if (buttonid == Buttons::OK)
			okhandler();

		return Button::State::NORMAL;
	}

	UIKeySelect::UIKeySelect(std::function<void(bool)> oh, bool l) : okhandler(oh), login(l)
	{
		nl::node KeyType = UiAssetResolve::window("KeyConfig")["KeyType"];
		nl::node backgrnd = KeyType["backgrnd"];

		if (!UiAssetResolve::usable(KeyType) || !backgrnd)
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		sprites.emplace_back(backgrnd);

		if (KeyType["btClose"])
			buttons[Buttons::CLOSE] = std::make_unique<MapleButton>(KeyType["btClose"]);
		if (KeyType["btTypeA"])
			buttons[Buttons::TYPEA] = std::make_unique<MapleButton>(KeyType["btTypeA"]);
		if (KeyType["btTypeB"])
			buttons[Buttons::TYPEB] = std::make_unique<MapleButton>(KeyType["btTypeB"], Point<int16_t>(1, 1));

		if (login)
			if (auto it = buttons.find(Buttons::CLOSE); it != buttons.end() && it->second)
				it->second->set_active(false);

		dimension = Texture(backgrnd).get_dimensions();
		center_on_screen();
	}

	void UIKeySelect::update_screen(int16_t, int16_t)
	{
		center_on_screen();
	}

	void UIKeySelect::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (!login && (escape || keycode == KeyAction::Id::RETURN))
		{
			deactivate();
			return;
		}
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
			|| keycode == 'A' || keycode == 'a' || keycode == 'B' || keycode == 'b')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				if (t == "a" || t == "typea" || t == "0" || t == "default")
					button_pressed(Buttons::TYPEA);
				else if (t == "b" || t == "typeb" || t == "1" || t == "alternate")
					button_pressed(Buttons::TYPEB);
			}, []() {}, "a|b");
		}
	}

	UIElement::Type UIKeySelect::get_type() const
	{
		return TYPE;
	}

	Button::State UIKeySelect::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			default:
			case Buttons::CLOSE:
				deactivate();
				break;
			case Buttons::TYPEA:
			case Buttons::TYPEB:
			{
				bool alternate = (buttonid == Buttons::TYPEA) ? false : true;

				if (alternate)
				{
					if (auto it = buttons.find(Buttons::TYPEA); it != buttons.end() && it->second)
						it->second->set_state(Button::State::DISABLED);
				}
				else
				{
					if (auto it = buttons.find(Buttons::TYPEB); it != buttons.end() && it->second)
						it->second->set_state(Button::State::DISABLED);
				}

				auto onok = [&, alternate]()
				{
					okhandler(alternate);
					deactivate();
				};

				UI::get().emplace<UIKeyConfirm>(alternate, onok, login);
				break;
			}
		}

		return Button::State::DISABLED;
	}

	UIKeyConfirm::UIKeyConfirm(bool alternate, std::function<void()> oh, bool l) : okhandler(oh), login(l)
	{
		nl::node alert = UiAssetResolve::window("KeyConfig")["KeyType"]["alert"];
		nl::node background = alternate ? alert["alternate"] : alert["default"];

		if (!UiAssetResolve::usable(alert) || !background)
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		sprites.emplace_back(background);

		if (alert["btOk"])
			buttons[Buttons::OK] = std::make_unique<MapleButton>(alert["btOk"]);

		dimension = Texture(background).get_dimensions();
		center_on_screen();
	}

	void UIKeyConfirm::update_screen(int16_t, int16_t)
	{
		center_on_screen();
	}

	void UIKeyConfirm::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (keycode == KeyAction::Id::RETURN)
			{
				confirm();
			}
			else if (!login && escape)
			{
				deactivate();

				UI::get().remove(UIElement::Type::LOGINNOTICE);
			}
			else if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
				|| keycode == 'Y' || keycode == 'y')
			{
				UI::get().emplace<UISoftKey>([this](const std::string& tip) {
					if (tip.empty()) return;
					std::string t = tip;
					for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
					if (t == "y" || t == "yes" || t == "ok" || t == "1")
						confirm();
				}, []() {}, "y|ok");
			}
		}
	}

	UIElement::Type UIKeyConfirm::get_type() const
	{
		return TYPE;
	}

	Button::State UIKeyConfirm::button_pressed(uint16_t buttonid)
	{
		confirm();

		return Button::State::NORMAL;
	}

	void UIKeyConfirm::confirm()
	{
		okhandler();
		deactivate();

		UI::get().remove(UIElement::Type::LOGINNOTICE);
	}
}