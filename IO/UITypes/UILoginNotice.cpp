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
#include <cstdint>
#include <vector>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UILoginNotice::UILoginNotice(uint16_t message, std::function<void()> okhandler, std::function<void()> cancelhandler) : okhandler(okhandler), cancelhandler(cancelhandler), use_localized_text(false)
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

		// 羊皮纸 backgrnd(运行时 362×219，左上角烘焙蘑菇印章) + 文案板 + 按钮。
		// 修复（2026-11-15）：原硬编码文案 (17,13)/按钮 (100,106) 相对背景板整体偏左
		// （文案偏左 53px、按钮偏左 43px，与实机"文案和按钮都偏左"吻合）。
		// 改为按背景板尺寸动态计算水平居中坐标，坐标由已部署 img 几何推导
		// （backgrnd=362×219、text=221×109、BtYes/BtNo=75×32），不写死偏移。
		dimension = bg_tex.get_dimensions();
		const int16_t bg_w = dimension.x();
		const int16_t bg_h = dimension.y();

		sprites.emplace_back(backgrnd);

		// 文案位图内的墨迹并非居中——各 text/N 都是左对齐排版（墨迹起点 x≈2-6），
		// 宽度随文案长短变化（实测 cx 52~110 不等），整块画布居中后文字依然偏左
		// 且压到左上角蘑菇印章。改为运行时解压位图扫描墨迹包围盒（暗色像素，
		// 阈值 max(R,G,B)<140，列/行 ≥2 个墨迹点滤除孤立抗锯齿噪点），
		// 把墨迹中心水平对齐到背景板中心；y 维持顶部 13（避开左下卷角）。
		// 密码错误/未知错误：原 WZ 位图是英文字图，改 Text+Locale 渲染中文。
		// 参照同文件 UIQuitConfirm 的范式；文案走 locale，缺键由 Locale::tr 回退。
		const bool localized = (message == Message::WRONG_PASSWORD) || (message == Message::UNKNOWN_ERROR);

		if (localized)
		{
			const std::string key = (message == Message::WRONG_PASSWORD)
				? "login.err_wrong_password"
				: "login.err_unknown_error";

			// A12M/12 号字，居中，深灰；宽 300 与 UIQuitConfirm 一致，适配 362 宽背景板。
			notice_text = Text(
				Text::Font::A12M,
				Text::Alignment::CENTER,
				Color::Name::DARKGREY,
				Locale::text(key),
				300
			);
			notice_text_pos = Point<int16_t>(
				static_cast<int16_t>(bg_w / 2),
				static_cast<int16_t>((bg_h - notice_text.height()) / 2 - 22)
			);
			use_localized_text = true;
		}

		const Texture text_tex(Notice["text"][message]);
		int16_t text_x = static_cast<int16_t>((bg_w - text_tex.get_dimensions().x()) / 2);

		// 中文分支不需要位图墨迹扫描（文案由 Text 渲染）；仅其它 message 走原扫描逻辑。
		if (!localized)
		{
		if (const nl::bitmap tbmp = Notice["text"][message])
		{
			if (const uint8_t* pixels = static_cast<const uint8_t*>(tbmp.data()))
			{
				const int32_t tw = tbmp.width();
				const int32_t th = tbmp.height();

				std::vector<int32_t> colcount(tw, 0);
				std::vector<int32_t> rowcount(th, 0);

				for (int32_t y = 0; y < th; ++y)
				{
					const uint8_t* row = pixels + static_cast<size_t>(y) * tw * 4;

					for (int32_t x = 0; x < tw; ++x)
					{
						const uint8_t* bgra = row + static_cast<size_t>(x) * 4;
						const uint8_t b = bgra[0];
						const uint8_t g = bgra[1];
						const uint8_t r = bgra[2];
						const uint8_t bright = (b > g) ? ((b > r) ? b : r) : ((g > r) ? g : r);

						if (bright < 140)
						{
							++colcount[x];
							++rowcount[y];
						}
					}
				}

				int32_t minx = -1;
				int32_t maxx = -1;

				for (int32_t x = 0; x < tw; ++x)
				{
					if (colcount[x] >= 2)
					{
						if (minx < 0)
							minx = x;

						maxx = x;
					}
				}

				if (minx >= 0 && maxx > minx)
				{
					const int32_t ink_w = maxx - minx + 1;
					text_x = static_cast<int16_t>((bg_w - ink_w) / 2 - minx);
				}
			}
		}
		}

		// 中文分支不叠加 WZ 英文字图，避免中英双语重叠。
		if (!localized)
			sprites.emplace_back(Notice["text"][message], Point<int16_t>(text_x, 13));

		if (message == Message::DELETE_CONFIRMATION || message == Message::CASH_ITEMS_CONFIRM_DELETION)
		{
			multiple = true;

			// 双按钮：两键各 75 宽、间距 13，整体居中。
			constexpr int16_t dbtn_w = 75;
			constexpr int16_t dbtn_gap = 13;
			const int16_t dstart = static_cast<int16_t>((bg_w - (dbtn_w * 2 + dbtn_gap)) / 2);

			buttons[Buttons::YES] = std::make_unique<MapleButton>(Notice["BtYes"], Point<int16_t>(dstart, 106));
			buttons[Buttons::NO] = std::make_unique<MapleButton>(Notice["BtNo"], Point<int16_t>(static_cast<int16_t>(dstart + dbtn_w + dbtn_gap), 106));
		}
		else
		{
			// 单按钮"确定"：相对背景板水平居中（75×32）。
			constexpr int16_t sbtn_w = 75;
			const int16_t sbtn_x = static_cast<int16_t>((bg_w - sbtn_w) / 2);

			buttons[Buttons::YES] = std::make_unique<MapleButton>(Notice["BtYes"], Point<int16_t>(sbtn_x, 106));
		}

		center_on_screen();
	}

	UILoginNotice::UILoginNotice(uint16_t message, std::function<void()> okhandler) : UILoginNotice(message, okhandler, []() {}) {}
	UILoginNotice::UILoginNotice(uint16_t message) : UILoginNotice(message, []() {}) {}

	void UILoginNotice::draw(float alpha) const
	{
		UIElement::draw(alpha);

		if (use_localized_text)
			notice_text.draw(position + notice_text_pos);
	}

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

			// 与 UILoginNotice 一致：按钮相对背景板水平居中（BtYes/BtNo 各 75×32）。
			dimension = Texture(backgrnd).get_dimensions();
			const int16_t cbg_w = dimension.x();

			if (unavailable)
			{
				constexpr int16_t cbtn_w = 75;
				const int16_t cbtn_x = static_cast<int16_t>((cbg_w - cbtn_w) / 2);

				buttons[Buttons::OK] = std::make_unique<MapleButton>(notice["BtYes"], Point<int16_t>(cbtn_x, 106));
			}
			else
			{
				constexpr int16_t cbtn_w = 75;
				constexpr int16_t cbtn_gap = 13;
				const int16_t cstart = static_cast<int16_t>((cbg_w - (cbtn_w * 2 + cbtn_gap)) / 2);

				buttons[Buttons::OK] = std::make_unique<MapleButton>(notice["BtYes"], Point<int16_t>(cstart, 106));
				buttons[Buttons::CANCEL] = std::make_unique<MapleButton>(notice["BtNo"], Point<int16_t>(static_cast<int16_t>(cstart + cbtn_w + cbtn_gap), 106));
			}

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