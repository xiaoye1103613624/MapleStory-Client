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

#include "../UI.h"
#include "../LoginBackground.h"

#include "../Components/MapleButton.h"

#include "../../Audio/Audio.h"
#include "../../Util/LoginUiAssets.h"
#include "../../Util/UiAssetResolve.h"

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
		Texture text_tex(Notice["text"][message]);
		Texture yes_tex(Notice["BtYes"]["normal"]["0"]);

		// BeiDou Notice/text/* 多为不透明整板（羊皮纸+文案），再叠 backgrnd 会像「两个提示框」。
		// 有不透明文案板时：以外框 backgrnd 定尺寸，文案板居中叠在框内；若文案板几乎铺满则只画文案板。
		dimension = bg_tex.get_dimensions();
		const int16_t bg_w = dimension.x();
		const int16_t bg_h = dimension.y();
		const int16_t text_w = text_tex.get_dimensions().x();
		const int16_t text_h = text_tex.get_dimensions().y();
		const int16_t btn_w = yes_tex.get_dimensions().x();
		const int16_t btn_h = yes_tex.get_dimensions().y();
		constexpr int16_t btn_gap = 12;
		constexpr int16_t text_top = 16;
		constexpr int16_t btn_bottom_pad = 24;

		// BeiDou Notice/text/* ≈ 221×109 全不透明羊皮纸+文案；backgrnd/0 ≈ 362×219。
		// 旧阈值 70%×50% 对 61%×49.8% 判失败 → 叠两层像「两个弹窗」。
		const bool text_is_full_plate = text_w > 0 && text_h > 0
			&& (
				(text_w * 100 >= bg_w * 55 && text_h * 100 >= bg_h * 45)
				|| (LoginBackground::get().available() && text_w >= 180 && text_h >= 90)
			);

		if (text_is_full_plate)
		{
			// 文案本身已是完整提示板：只画文案，避免双羊皮纸。
			dimension = text_tex.get_dimensions();
			sprites.emplace_back(Notice["text"][message]);
		}
		else
		{
			const int16_t text_x = static_cast<int16_t>((bg_w - text_w) / 2);
			sprites.emplace_back(backgrnd);
			sprites.emplace_back(Notice["text"][message], Point<int16_t>(text_x, text_top));
		}

		const int16_t panel_w = dimension.x();
		const int16_t panel_h = dimension.y();
		const int16_t btn_y = static_cast<int16_t>(panel_h - btn_h - btn_bottom_pad);

		if (message == Message::DELETE_CONFIRMATION || message == Message::CASH_ITEMS_CONFIRM_DELETION)
		{
			multiple = true;

			const int16_t pair_w = static_cast<int16_t>(btn_w + btn_gap + btn_w);
			const int16_t btn_x0 = static_cast<int16_t>((panel_w - pair_w) / 2);

			buttons[Buttons::YES] = std::make_unique<MapleButton>(Notice["BtYes"], Point<int16_t>(btn_x0, btn_y));
			buttons[Buttons::NO] = std::make_unique<MapleButton>(
				Notice["BtNo"],
				Point<int16_t>(static_cast<int16_t>(btn_x0 + btn_w + btn_gap), btn_y)
			);
		}
		else
		{
			const int16_t btn_x = static_cast<int16_t>((panel_w - btn_w) / 2);
			buttons[Buttons::YES] = std::make_unique<MapleButton>(Notice["BtYes"], Point<int16_t>(btn_x, btn_y));
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
		nl::node text_node = notice["text"][UILoginNotice::Message::CONFIRM_EXIT];
		nl::node bt_yes = notice["BtYes"];
		nl::node bt_no = notice["BtNo"];

		Texture bg_tex(backgrnd);
		Texture text_tex(text_node);
		Texture yes_tex(bt_yes["normal"]["0"]);
		Texture no_tex(bt_no["normal"]["0"]);

		dimension = bg_tex.get_dimensions();
		const int16_t bg_w = dimension.x();
		const int16_t bg_h = dimension.y();
		const int16_t text_w = text_tex.get_dimensions().x();
		const int16_t btn_w = yes_tex.get_dimensions().x();
		const int16_t btn_h = yes_tex.get_dimensions().y();
		constexpr int16_t btn_gap = 12;
		constexpr int16_t text_top = 16;
		constexpr int16_t btn_bottom_pad = 24;

		// 相对羊皮纸居中：印章已烘焙在底图左侧，文字/按钮按底板宽高排布。
		const int16_t text_x = static_cast<int16_t>((bg_w - text_w) / 2);
		const int16_t pair_w = static_cast<int16_t>(btn_w + btn_gap + btn_w);
		const int16_t btn_x0 = static_cast<int16_t>((bg_w - pair_w) / 2);
		const int16_t btn_y = static_cast<int16_t>(bg_h - btn_h - btn_bottom_pad);

		sprites.emplace_back(backgrnd);
		sprites.emplace_back(text_node, Point<int16_t>(text_x, text_top));

		buttons[Buttons::BT_OK] = std::make_unique<MapleButton>(bt_yes, Point<int16_t>(btn_x0, btn_y));
		buttons[Buttons::BT_CANCEL] = std::make_unique<MapleButton>(
			bt_no,
			Point<int16_t>(static_cast<int16_t>(btn_x0 + btn_w + btn_gap), btn_y)
		);

		center_on_screen();
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
		if (pressed && !login)
			if (escape || keycode == KeyAction::Id::RETURN)
				deactivate();
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