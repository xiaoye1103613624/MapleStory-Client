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
#include "UILogin.h"

#include "UILoginNotice.h"
#include "UILoginWait.h"

#include "../UI.h"

#include "../LoginBackground.h"

#include "../Components/MapleButton.h"
#include "../Components/TwoSpriteButton.h"

#include "../../Audio/Audio.h"

#include "../../Net/Packets/LoginPackets.h"

#include <windows.h>
#include <cstring>
#include <cstdint>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UILogin::UILogin() : UIElement(
			Point<int16_t>(0, 0),
			Point<int16_t>(
				Constants::Constants::get().get_viewwidth(),
				Constants::Constants::get().get_viewheight()
			)
		), title_pos(Point<int16_t>(344, 246)), nexon(false)
	{
		std::string LoginMusicNewtro = Configuration::get().get_login_music_newtro();

		Music(LoginMusicNewtro).play();

		std::string version_text = Configuration::get().get_version();
		version = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::LEMONGRASS, "Ver. " + version_text);
		version_pos = nl::nx::UI["Login.img"]["Common"]["version"]["pos"];

		nl::node Login = nl::nx::UI["Login.img"];
		version_pos = Login["Common"]["version"]["pos"];

		// 兼容数据命名：部分 v83 私服（如 BeiDou)把登录面板放在 Login.img/Title 而非 Title_new
		nl::node Title_new = Login["Title_new"];
		if (!Title_new)
			Title_new = Login["Title"];
		capslock = Title_new["capslock"];

		nl::node check_src = Title_new["check"];
		check[false] = check_src["0"];
		check[true] = check_src["1"];

		// 兼容数据命名：不同服务端/版本把登录背景放在不同位置。
		// 优先级：Login.img/Common/frame (BeiDou) -> Map001/Back/UI_login.img -> Map001/Back/login.img
		nl::node backgrd = Login["Common"]["frame"];
		if (!backgrd)
			backgrd = nl::nx::Map001["Back"]["UI_login.img"];
		if (!backgrd)
			backgrd = nl::nx::Map001["Back"]["login.img"];

		nl::node backgrd_src = backgrd["back"] ? backgrd["back"]["0"] : backgrd;

		Point<int16_t> screen_center(
			Constants::Constants::get().get_viewwidth() / 2,
			Constants::Constants::get().get_viewheight() / 2
		);
		nl::node titlebg = Title_new["backgrd"];
		if (!titlebg)
			titlebg = Login["Common"]["backgrd"];
		if (titlebg)
			sprites.emplace_back(titlebg, title_pos);

		// BeiDou 原生布局判定：Login.img/Title 没有 backgrd/mapleID/PW 贴图，
		// 面板与控件坐标需要按原客户端硬编码坐标重建。
		bedou = !titlebg;

		// BeiDou 登录背景（地图 + 外框）由 LoginBackground 统一绘制并跨流程复用，
		// 因此这里不再自行添加背景 sprite。
		bool bg_managed = LoginBackground::get().available();

		if (backgrd_src && !bg_managed)
		{
			sprites.emplace_back(backgrd_src, screen_center);
		}

		Point<int16_t> bpos_login = title_pos;
		Point<int16_t> bpos_save = title_pos;
		Point<int16_t> bpos_lost = title_pos;
		Point<int16_t> bpos_pwlost = title_pos;
		Point<int16_t> bpos_new = title_pos;
		Point<int16_t> bpos_home = title_pos;
		Point<int16_t> bpos_quit = title_pos;

		if (bedou)
		{
			// 告示牌屏幕左上角作为面板基准（与地图中 signboard 绘制锚点一致：
			// screen_topleft = map_pos + viewpos - origin），控件相对它摆放即与告示牌对齐。
			Texture sign_tex(nl::nx::Map["Obj"]["login.img"]["Title"]["signboard"]["0"]["0"]);
			Point<int16_t> sign_origin = sign_tex.get_origin();
			Point<int16_t> signboard_map_pos(137, 21);
			Point<int16_t> sign_topleft = signboard_map_pos + LoginBackground::get().get_viewpos() - sign_origin;

			// 面板原点 = 告示牌左上角，后续按钮偏移以此为基准。
			panel_pos = sign_topleft;

			// 输入框按原版 CUITitle::OnCreate 偏移（账号 67,12 / 密码 67,40）。
			account_pos = sign_topleft + Point<int16_t>(67, 12);
			password_pos = sign_topleft + Point<int16_t>(67, 40);

			// 记住 ID 勾选框（位于保存按钮左侧）。
			check_pos = sign_topleft + Point<int16_t>(12, 73);

			// 按钮按 IDA 真实偏移；叠加贴图 origin 使贴图左上角对齐原窗口坐标。
			auto button_pos = [&](nl::node btn, int16_t x, int16_t y) -> Point<int16_t> {
				Texture tex(btn["normal"]["0"]);
				return sign_topleft + Point<int16_t>(x, y) + tex.get_origin();
			};
			bpos_login = button_pos(Title_new["BtLogin"], 213, -5);
			bpos_save = button_pos(Title_new["BtLoginIDSave"], 37, 74);
			bpos_lost = button_pos(Title_new["BtLoginIDLost"], 144, 73);
			bpos_pwlost = button_pos(Title_new["BtPasswdLost"], 226, 74);
			bpos_new = button_pos(Title_new["BtNew"], 14, 122);
			bpos_home = button_pos(Title_new["BtHomePage"], 114, 120);
			bpos_quit = button_pos(Title_new["BtQuit"], 214, 120);

			// 登录步骤为 0。
			LoginBackground::get().set_step(0);
		}
		else
		{
			panel_pos = title_pos;
			account_pos = title_pos + Point<int16_t>(27, 69);
			password_pos = account_pos;
			check_pos = title_pos;
		}

		nl::node Tab = Title_new["Tab"];
		nl::node TabD = Tab["disabled"];
		nl::node TabE = Tab["enabled"];

		// BeiDou：按钮节点为 BtLoginIDSave/BtLoginIDLost，且每个按钮在面板内
		// 有独立的硬编码坐标（来自原客户端 CUITitle::OnCreate）。
		nl::node node_idsave = bedou ? Title_new["BtLoginIDSave"] : Title_new["BtEmailSave"];
		nl::node node_idlost = bedou ? Title_new["BtLoginIDLost"] : Title_new["BtEmailLost"];

		if (!node_idsave)
			node_idsave = Title_new["BtEmailSave"];

		if (!node_idlost)
			node_idlost = Title_new["BtEmailLost"];

		buttons[Buttons::BtLogin] = std::make_unique<MapleButton>(Title_new["BtLogin"], bpos_login);
		buttons[Buttons::BtEmailSave] = std::make_unique<MapleButton>(node_idsave, bpos_save);
		buttons[Buttons::BtEmailLost] = std::make_unique<MapleButton>(node_idlost, bpos_lost);
		buttons[Buttons::BtPasswdLost] = std::make_unique<MapleButton>(Title_new["BtPasswdLost"], bpos_pwlost);
		buttons[Buttons::BtNew] = std::make_unique<MapleButton>(Title_new["BtNew"], bpos_new);
		buttons[Buttons::BtHomePage] = std::make_unique<MapleButton>(Title_new["BtHomePage"], bpos_home);
		buttons[Buttons::BtQuit] = std::make_unique<MapleButton>(Title_new["BtQuit"], bpos_quit);
		buttons[Buttons::BtMapleID] = std::make_unique<TwoSpriteButton>(TabD["0"], TabE["0"], Point<int16_t>(344, 246));
		buttons[Buttons::BtNexonID] = std::make_unique<TwoSpriteButton>(TabD["1"], TabE["1"], Point<int16_t>(344, 246));

		if (nexon)
		{
			buttons[Buttons::BtNexonID]->set_state(Button::State::PRESSED);
			buttons[Buttons::BtMapleID]->set_state(Button::State::NORMAL);
		}
		else
		{
			buttons[Buttons::BtNexonID]->set_state(Button::State::NORMAL);
			buttons[Buttons::BtMapleID]->set_state(Button::State::PRESSED);
		}

		background = ColorBox(dimension.x(), dimension.y(), Color::Name::BLACK, 1.0f);

		Point<int16_t> textfield_pos = bedou ? account_pos : title_pos + Point<int16_t>(27, 69);

#pragma region Account
		Texture account_src = Texture(Title_new["mapleID"]);
		account_src_dim = account_src.get_dimensions();

		// BeiDou 数据缺少输入框贴图，按原版 CUITitle 输入区尺寸兜底 (132x15)。
		if (bedou)
			account_src_dim = Point<int16_t>(132, 15);

		account = Textfield(Text::Font::A13M, Text::Alignment::LEFT, Color::Name::JAMBALAYA, Rectangle<int16_t>(textfield_pos, textfield_pos + account_src_dim), TEXTFIELD_LIMIT);

		account.set_key_callback
		(
			KeyAction::Id::TAB, [&]
			{
				account.set_state(Textfield::State::NORMAL);
				password.set_state(Textfield::State::FOCUSED);
			}
		);

		account.set_enter_callback
		(
			[&](std::string msg)
			{
				login();
			}
		);

		account_bg[false] = account_src;
		account_bg[true] = Title_new["nexonID"];
#pragma endregion

#pragma region Password
		if (bedou)
			textfield_pos = password_pos;
		else
			textfield_pos.shift_y(account_src_dim.y() + 1);

		Texture password_src = Title_new["PW"];
		password_src_dim = password_src.get_dimensions();

		if (bedou)
			password_src_dim = Point<int16_t>(120, 15);

		password = Textfield(Text::Font::A13M, Text::Alignment::LEFT, Color::Name::JAMBALAYA, Rectangle<int16_t>(textfield_pos, textfield_pos + password_src_dim), TEXTFIELD_LIMIT);

		password.set_key_callback
		(
			KeyAction::Id::TAB, [&]
			{
				account.set_state(Textfield::State::FOCUSED);
				password.set_state(Textfield::State::NORMAL);
			}
		);

		password.set_enter_callback
		(
			[&](std::string msg)
			{
				login();
			}
		);

		password.set_cryptchar('*');
		password_bg = password_src;
#pragma endregion

		// BeiDou 数据缺少输入框贴图：用不透明白底 + 深色边框补齐，保证可见可点。
		if (bedou)
		{
			account_border = ColorBox(account_src_dim.x() + 2, account_src_dim.y() + 2, Color::Name::DARKGREY, 1.0f);
			account_field = ColorBox(account_src_dim.x(), account_src_dim.y(), Color::Name::WHITE, 1.0f);
			password_border = ColorBox(password_src_dim.x() + 2, password_src_dim.y() + 2, Color::Name::DARKGREY, 1.0f);
			password_field = ColorBox(password_src_dim.x(), password_src_dim.y(), Color::Name::WHITE, 1.0f);
		}

		saveid = Setting<SaveLogin>::get().load();

		if (saveid)
		{
			account.change_text(Setting<DefaultAccount>::get().load());
			password.set_state(Textfield::State::FOCUSED);
		}
		else
		{
			account.set_state(Textfield::State::FOCUSED);
		}

		if (Configuration::get().get_auto_login())
		{
			UI::get().emplace<UILoginWait>([]() {});

			auto loginwait = UI::get().get_element<UILoginWait>();

			if (loginwait && loginwait->is_active())
				LoginPacket(
					Configuration::get().get_auto_acc(),
					Configuration::get().get_auto_pass()
				).dispatch();
		}
	}

	void UILogin::draw(float alpha) const
	{
		// BeiDou 下背景由 LoginBackground 统一绘制，这里只画面板控件。
		if (!LoginBackground::get().available())
			background.draw(position + Point<int16_t>(0, 7));

		UIElement::draw(alpha);

		version.draw(position + version_pos - Point<int16_t>(0, 5));

		if (bedou)
		{
			account_border.draw(position + account_pos - Point<int16_t>(1, 1));
			account_field.draw(position + account_pos);
			password_border.draw(position + password_pos - Point<int16_t>(1, 1));
			password_field.draw(position + password_pos);

			account.draw(position);
			password.draw(position);
		}
		else
		{
			account.draw(position + Point<int16_t>(5, 10));
			password.draw(position + Point<int16_t>(5, 13));
		}

		// BeiDou 无标题背景图，告示牌已自带账号/密码提示，故跳过单独的 ID/PW 标签绘制。
		if (!bedou && account.get_state() == Textfield::State::NORMAL && account.empty() && account_bg[nexon].is_valid())
			account_bg[nexon].draw(position + title_pos);

		if (!bedou && password.get_state() == Textfield::State::NORMAL && password.empty() && password_bg.is_valid())
			password_bg.draw(position + title_pos);

		bool has_capslocks = UI::get().has_capslocks();

		if (check[saveid].is_valid())
			check[saveid].draw(position + (bedou ? check_pos : title_pos));

		if (has_capslocks && account.get_state() == Textfield::State::FOCUSED)
			capslock.draw(position + (bedou ? account_pos : title_pos) - Point<int16_t>(0, account_src_dim.y()));

		if (has_capslocks && password.get_state() == Textfield::State::FOCUSED)
			capslock.draw(position + (bedou ? password_pos : title_pos) + Point<int16_t>(password_src_dim.x() - account_src_dim.x(), 0));
	}

	void UILogin::update()
	{
		UIElement::update();

		account.update();
		password.update();
	}

	void UILogin::login()
	{
		account.set_state(Textfield::State::DISABLED);
		password.set_state(Textfield::State::DISABLED);

		std::string account_text = account.get_text();
		std::string password_text = password.get_text();

		std::function<void()> okhandler = [&, password_text]()
		{
			account.set_state(Textfield::State::NORMAL);
			password.set_state(Textfield::State::NORMAL);

			if (!password_text.empty())
				password.set_state(Textfield::State::FOCUSED);
			else
				account.set_state(Textfield::State::FOCUSED);
		};

		if (account_text.empty())
		{
			UI::get().emplace<UILoginNotice>(UILoginNotice::Message::NOT_REGISTERED, okhandler);
			return;
		}

		if (password_text.length() <= 4)
		{
			UI::get().emplace<UILoginNotice>(UILoginNotice::Message::WRONG_PASSWORD, okhandler);
			return;
		}

		UI::get().emplace<UILoginWait>(okhandler);

		auto loginwait = UI::get().get_element<UILoginWait>();

		if (loginwait && loginwait->is_active())
		{
			// TODO: Implement login with email
			if (nexon)
				LoginEmailPacket(account_text, password_text).dispatch();
			else
				LoginPacket(account_text, password_text).dispatch();
		}
	}

	void UILogin::open_url(uint16_t id)
	{
		std::string url;

		switch (id)
		{
			case Buttons::BtNew:
				url = Configuration::get().get_joinlink();
				break;
			case Buttons::BtHomePage:
				url = Configuration::get().get_website();
				break;
			case Buttons::BtPasswdLost:
				url = Configuration::get().get_findpass();
				break;
			case Buttons::BtEmailLost:
				url = Configuration::get().get_findid();
				break;
			default:
				return;
		}

		ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

	Button::State UILogin::button_pressed(uint16_t id)
	{
		switch (id)
		{
			case Buttons::BtLogin:
			{
				login();

				return Button::State::NORMAL;
			}
			case Buttons::BtNew:
			case Buttons::BtHomePage:
			case Buttons::BtPasswdLost:
			case Buttons::BtEmailLost:
			{
				open_url(id);

				return Button::State::NORMAL;
			}
			case Buttons::BtEmailSave:
			{
				saveid = !saveid;

				Setting<SaveLogin>::get().save(saveid);

				return Button::State::MOUSEOVER;
			}
			case Buttons::BtQuit:
			{
				UI::get().quit();

				return Button::State::PRESSED;
			}
			case Buttons::BtMapleID:
			{
				nexon = false;

				buttons[Buttons::BtNexonID]->set_state(Button::State::NORMAL);

				account.change_text("");
				password.change_text("");

				account.set_limit(TEXTFIELD_LIMIT);

				return Button::State::PRESSED;
			}
			case Buttons::BtNexonID:
			{
				nexon = true;

				buttons[Buttons::BtMapleID]->set_state(Button::State::NORMAL);

				account.change_text("");
				password.change_text("");
				
				account.set_limit(72);

				return Button::State::PRESSED;
			}
			default:
			{
				return Button::State::DISABLED;
			}
		}
	}

	Cursor::State UILogin::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (Cursor::State new_state = account.send_cursor(cursorpos, clicked))
			return new_state;

		if (Cursor::State new_state = password.send_cursor(cursorpos, clicked))
			return new_state;

		return UIElement::send_cursor(clicked, cursorpos);
	}

	UIElement::Type UILogin::get_type() const
	{
		return TYPE;
	}
}