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
#include "UI.h"

#include "UIStateCashShop.h"
#include "UIStateGame.h"
#include "UIStateLogin.h"
#include "Window.h"

#include "../Configuration.h"
#include "../Util/PerfLog.h"
#include "../Util/ResolutionTiers.h"

#include "UITypes/UIChannel.h"
#include "UITypes/UIChat.h"
#include "UITypes/UIChatBar.h"
#include "UITypes/UIJoypad.h"
#include "UITypes/UINpcTalk.h"
#include "UITypes/UIOptionMenu.h"
#include "UITypes/UIQuit.h"
#include "UITypes/UIQuickslotConfig.h"
#include "UITypes/UIShop.h"
#include "UITypes/UIStatusBar.h"
#include "UITypes/UITemporaryStatView.h"
#include "UITypes/UIWorldMap.h"

namespace ms
{
	UI::UI()
	{
		state = std::make_unique<UIStateNull>();
		enabled = true;
	}

	void UI::init()
	{
		caps_lock_enabled = Configuration::get().get_caps_lock_enabled();
	
		cursor.init();

		change_state(State::LOGIN);
	}

	void UI::draw(float alpha) const
	{
		state->draw(alpha, cursor.get_position());

		scrollingnotice.draw(alpha);

		cursor.draw(alpha);
	}

	void UI::update()
	{
		state->update();

		scrollingnotice.update();

		cursor.update();
	}

	void UI::enable()
	{
		enabled = true;
	}

	void UI::disable()
	{
		enabled = false;
	}

	void UI::change_state(State id)
	{
		// 从游戏/商城返回登录或选角时恢复登录视口 1280×720（UI/Login.img/Common/frame
		// 原生尺寸）。UIStateLogin 在下一行构造，读到的已是登录尺寸；实际建窗由下一帧
		// Window::check_events 完成。启动时 UI::init() 也走这里，值相同不会重复建窗。
		if (id == State::LOGIN)
			ResolutionTiers::apply_viewsize(
				ResolutionTiers::LOGIN_W, ResolutionTiers::LOGIN_H);

		switch (id)
		{
			case State::LOGIN:
				state = std::make_unique<UIStateLogin>();
				break;
			case State::GAME:
				state = std::make_unique<UIStateGame>();
				break;
			case State::CASHSHOP:
				state = std::make_unique<UIStateCashShop>();
				break;
		}
	}

	void UI::quit()
	{
		quitted = true;
	}

	bool UI::not_quitted() const
	{
		return !quitted;
	}

	bool UI::has_capslocks() const
	{
		return caps_lock_enabled;
	}

	void UI::send_cursor(Point<int16_t> cursor_position, Cursor::State cursor_state)
	{
		Cursor::State next_state = state->send_cursor(cursor_position, cursor_state);

		cursor.set_state(next_state);
		cursor.set_position(cursor_position);
	}

	void UI::send_focus(int focused)
	{
		if (focused)
		{
			// The window gained input focus
			uint8_t sfxvolume = Setting<SFXVolume>::get().load();
			Sound::set_sfxvolume(sfxvolume);

			uint8_t bgmvolume = Setting<BGMVolume>::get().load();
			Music::set_bgmvolume(bgmvolume);
		}
		else
		{
			// The window lost input focus
			Sound::set_sfxvolume(0);
			Music::set_bgmvolume(0);
		}
	}

	void UI::send_scroll(double yoffset)
	{
		state->send_scroll(yoffset);
	}

	void UI::send_close()
	{
		state->send_close();
	}

	void UI::send_cursor(bool pressed)
	{
		Cursor::State cursorstate = (pressed && enabled) ? Cursor::State::CLICKING : Cursor::State::IDLE;
		Point<int16_t> cursorpos = cursor.get_position();
		send_cursor(cursorpos, cursorstate);

		if (focusedtextfield && pressed)
		{
			Cursor::State tstate = focusedtextfield->send_cursor(cursorpos, pressed);

			switch (tstate)
			{
				case Cursor::State::IDLE:
					focusedtextfield = {};
					break;
			}
		}
	}

	void UI::send_cursor(Point<int16_t> cursor_position)
	{
		send_cursor(cursor_position, cursor.get_state());
	}

	void UI::rightclick()
	{
		Point<int16_t> pos = cursor.get_position();
		state->rightclick(pos);
	}

	void UI::doubleclick()
	{
		Point<int16_t> pos = cursor.get_position();
		state->doubleclick(pos);
	}

	void UI::send_key(int32_t keycode, bool pressed)
	{
		if ((is_key_down[GLFW_KEY_LEFT_ALT] || is_key_down[GLFW_KEY_RIGHT_ALT]) && (is_key_down[GLFW_KEY_ENTER] || is_key_down[GLFW_KEY_KP_ENTER]))
		{
			// 「系统设置 · 画面模式 · 允许 ALT+回车切换」为假时不响应（原生同义：
			// 那一项就是这组合键的授权开关）。仍吃掉本次按键，避免落到快捷槽。
			if (Setting<SysOptAltEnter>::get().load())
				Window::get().toggle_fullscreen();

			is_key_down[GLFW_KEY_LEFT_ALT] = false;
			is_key_down[GLFW_KEY_RIGHT_ALT] = false;
			is_key_down[GLFW_KEY_ENTER] = false;
			is_key_down[GLFW_KEY_KP_ENTER] = false;

			return;
		}

		// 全局截图：登录/选角/游戏均可；登录期 KEYMAP 未下发时 SCROLL_LOCK 仍应生效。
		// SoftKey 尾巴「登录遮罩截图」验收依赖此路径（勿仅挂在 UIStateGame）。
		if (pressed)
		{
			// PRINT_SCREEN is an additional screenshot key: SendKeys automation
			// cannot deliver VK_SCROLL on this host; low-level keybd_event can.
			const bool scroll_lock = (keycode == GLFW_KEY_SCROLL_LOCK
				|| keycode == GLFW_KEY_PRINT_SCREEN);
			const bool mapped_shot =
				keyboard.get_mapping(keycode).action == KeyAction::Id::SCREENSHOT;
			if (scroll_lock || mapped_shot)
			{
				perf::mark("SHOT", "request kc=" + std::to_string(keycode));
				Window::get().request_screenshot();
				is_key_down[keycode] = pressed;
				return;
			}
		}

		if (is_key_down[keyboard.capslockcode()])
			caps_lock_enabled = !caps_lock_enabled;

		// quickslotConfig 捕获态：需要 GLFW→maple 键码，不能走 mapping.action。
		if (auto qs = get_element<UIQuickslotConfig>())
		{
			if (qs->is_active() && qs->is_capturing() && pressed)
			{
				if (keycode == GLFW_KEY_ESCAPE)
				{
					qs->send_key(0, true, true);
				}
				else
				{
					uint8_t mk = keyboard.maple_keycode_from_glfw(keycode);
					if (mk)
						qs->capture_maple_key(mk);
				}
				is_key_down[keycode] = pressed;
				return;
			}
			if (qs->is_active() && keycode == GLFW_KEY_ESCAPE && pressed)
			{
				qs->send_key(0, true, true);
				is_key_down[keycode] = pressed;
				return;
			}
		}

		if (focusedtextfield)
		{
			bool ctrl = is_key_down[keyboard.leftctrlcode()] || is_key_down[keyboard.rightctrlcode()];

			if (ctrl && pressed)
			{
				KeyAction::Id action = keyboard.get_ctrl_action(keycode);

				if (action == KeyAction::Id::COPY || action == KeyAction::Id::PASTE)
				{
					if (focusedtextfield->can_copy_paste())
					{
						switch (action)
						{
							case KeyAction::Id::COPY:
								Window::get().setclipboard(focusedtextfield->get_text());
								break;
							case KeyAction::Id::PASTE:
								focusedtextfield->add_string(Window::get().getclipboard());
								break;
						}
					}
				}
			}
			else
			{
				bool shift = is_key_down[keyboard.leftshiftcode()] || is_key_down[keyboard.rightshiftcode()] || caps_lock_enabled;
				Keyboard::Mapping mapping = keyboard.get_text_mapping(keycode, shift);

				// Android soft-keyboard: chars arrive via TEXT_INPUT; skip KEY TEXT
				// to avoid double-insert. Desktop uses KEY_DOWN ASCII (IME disabled).
				if (mapping.type == KeyType::Id::TEXT && Window::get().text_input_active())
					;
				else
					focusedtextfield->send_key(mapping.type, mapping.action, pressed);
			}
		}
		else
		{
			Keyboard::Mapping mapping = keyboard.get_mapping(keycode);
			// SoftKey 字母捷径（UIAdmin / UIBuffList / UIShortCut / UIStatusBar / UIGameMenu…）
			// 用 ASCII 比较 'A'/'S'/'D'/'4'。未绑定键 get_mapping → NONE/0，捷径永不触发。
			// GLFW 可打印键码与 ASCII 同值（A=65…Z=90，'4'=52），未绑定时把 keycode
			// 当作 action 下传；已绑定键仍走 KEYMAP，不受影响。
			KeyType::Id map_type = mapping.type;
			int32_t map_action = mapping.action;
			if (map_type == KeyType::Id::NONE
				&& keycode >= GLFW_KEY_SPACE && keycode <= GLFW_KEY_Z)
			{
				map_action = keycode;
			}

			bool sent = false;
			std::list<UIElement::Type> types;

			bool escape = keycode == GLFW_KEY_ESCAPE;
			bool tab = keycode == GLFW_KEY_TAB;
			bool enter = keycode == GLFW_KEY_ENTER || keycode == GLFW_KEY_KP_ENTER;
			bool up_down = keycode == GLFW_KEY_UP || keycode == GLFW_KEY_DOWN;
			bool left_right = keycode == GLFW_KEY_LEFT || keycode == GLFW_KEY_RIGHT;
			bool arrows = up_down || left_right;

			auto statusbar = UI::get().get_element<UIStatusBar>();
			auto channel = UI::get().get_element<UIChannel>();
			auto worldmap = UI::get().get_element<UIWorldMap>();
			auto optionmenu = UI::get().get_element<UIOptionMenu>();
			auto shop = UI::get().get_element<UIShop>();
			auto joypad = UI::get().get_element<UIJoypad>();
			auto rank = UI::get().get_element<UIRank>();
			auto quit = UI::get().get_element<UIQuit>();
			auto npctalk = UI::get().get_element<UINpcTalk>();
			//auto report = UI::get().get_element<UIReport>();
			//auto whisper = UI::get().get_element<UIWhisper>();

			if (npctalk && npctalk->is_active())
			{
				npctalk->send_key(map_action, pressed, escape);
				sent = true;
			}
			else if (statusbar && statusbar->is_menu_active())
			{
				statusbar->send_key(map_action, pressed, escape);
				sent = true;
			}
			else if (channel && channel->is_active() && map_action != KeyAction::Id::CHANGECHANNEL)
			{
				channel->send_key(map_action, pressed, escape);
				sent = true;
			}
			else if (worldmap && worldmap->is_active() && map_action != KeyAction::Id::WORLDMAP)
			{
				worldmap->send_key(map_action, pressed, escape);
				sent = true;
			}
			else if (optionmenu && optionmenu->is_active())
			{
				optionmenu->send_key(map_action, pressed, escape);
				sent = true;
			}
			else if (shop && shop->is_active())
			{
				shop->send_key(map_action, pressed, escape);
				sent = true;
			}
			else if (joypad && joypad->is_active())
			{
				joypad->send_key(map_action, pressed, escape);
				sent = true;
			}
			else if (rank && rank->is_active())
			{
				rank->send_key(map_action, pressed, escape);
				sent = true;
			}
			else if (quit && quit->is_active())
			{
				quit->send_key(map_action, pressed, escape);
				sent = true;
			}
			else
			{
				// All
				if (escape || tab || enter || arrows)
				{
					// Login
					types.emplace_back(UIElement::Type::WORLDSELECT);
					types.emplace_back(UIElement::Type::CHARSELECT);
					types.emplace_back(UIElement::Type::RACESELECT);			// No tab
					types.emplace_back(UIElement::Type::CLASSCREATION);			// No tab (No arrows, but shouldn't send else where)
					types.emplace_back(UIElement::Type::LOGINNOTICE);			// No tab (No arrows, but shouldn't send else where)
					types.emplace_back(UIElement::Type::LOGINNOTICE_CONFIRM);	// No tab (No arrows, but shouldn't send else where)
					types.emplace_back(UIElement::Type::LOGINWAIT);				// No tab (No arrows, but shouldn't send else where)
				}

				if (escape)
				{
					// Login
					types.emplace_back(UIElement::Type::SOFTKEYBOARD);

					// Game
					types.emplace_back(UIElement::Type::NOTICE);
					types.emplace_back(UIElement::Type::QUICKSLOTCONFIG);
					types.emplace_back(UIElement::Type::KEYCONFIG);
					types.emplace_back(UIElement::Type::CHAT);
					types.emplace_back(UIElement::Type::EVENT);
					types.emplace_back(UIElement::Type::DAILYGIFT);
					types.emplace_back(UIElement::Type::STATSINFO);
					types.emplace_back(UIElement::Type::ITEMINVENTORY);
					types.emplace_back(UIElement::Type::EQUIPINVENTORY);
					types.emplace_back(UIElement::Type::SKILLBOOK);
					types.emplace_back(UIElement::Type::QUESTLOG);
					types.emplace_back(UIElement::Type::USERLIST);
					types.emplace_back(UIElement::Type::NPCTALK);
					types.emplace_back(UIElement::Type::CHARINFO);

					// 游戏设置 / 系统设置（R27-B7-4）。这两个是本轮新拆出来的独立窗，
					// 原生按 ESC 会先关掉最上层的设置窗再轮到目录；漏加的话 ESC 会直接
					// 落到 MAINMENU，把目录气球盖在还开着的设置窗上。
					types.emplace_back(UIElement::Type::GAMEOPT);
					types.emplace_back(UIElement::Type::SYSOPT);
				}
				else if (enter)
				{
					// Login
					types.emplace_back(UIElement::Type::SOFTKEYBOARD);

					// Game
					types.emplace_back(UIElement::Type::NOTICE);
				}
				else if (tab)
				{
					// Game
					types.emplace_back(UIElement::Type::ITEMINVENTORY);
					types.emplace_back(UIElement::Type::EQUIPINVENTORY);
					types.emplace_back(UIElement::Type::SKILLBOOK);
					types.emplace_back(UIElement::Type::QUESTLOG);
					types.emplace_back(UIElement::Type::USERLIST);
				}

				if (types.size() > 0)
				{
					auto element = state->get_front(types);

					if (element && element != nullptr)
					{
						element->send_key(map_action, pressed, escape);
						sent = true;
					}
				}
			}

			if (!sent)
			{
				// SoftKey D：TemporaryStatView 开关。UIBuffList SoftKey D 因 FOCUSED=false
				// 键进不去；未绑定 D 时在此兜底（与 BuffList::send_key SoftKey D 同语义）。
				if (pressed && !escape && map_type == KeyType::Id::NONE
					&& (map_action == 'D' || map_action == 'd'))
				{
					auto tsv = UI::get().get_element<UITemporaryStatView>();
					if (!tsv)
						tsv = UI::get().emplace<UITemporaryStatView>();
					else if (!tsv->is_active())
						tsv->makeactive();
					else
						tsv->deactivate();
					sent = true;
				}
				else if (escape)
				{
					state->send_key(map_type, map_action, pressed, escape);
				}
				else if (enter)
				{
					if (auto chatbar = UI::get().get_element<UIChatBar>())
						chatbar->send_key(map_action, pressed, escape);
					else
						state->send_key(map_type, map_action, pressed, escape);
				}
				else
				{
					state->send_key(map_type, map_action, pressed, escape);
				}
			}
		}

		is_key_down[keycode] = pressed;
	}

	void UI::set_scrollnotice(const std::string& notice)
	{
		scrollingnotice.setnotice(notice);
	}

	void UI::send_text(const std::string& text)
	{
		if (!focusedtextfield || text.empty())
			return;

		// Composed text (IME / clipboard) may be UTF-8 (CJK). The Textfield
		// applies its own allowlist: unicode-capable fields keep it, ASCII-only
		// fields (account/password) still drop it.
		focusedtextfield->add_string(text);
	}

	void UI::focus_textfield(Textfield* tofocus)
	{
		if (focusedtextfield && focusedtextfield.get() != tofocus)
			focusedtextfield->set_state(Textfield::State::NORMAL);

		focusedtextfield = tofocus;

		if (focusedtextfield)
		{
			// 桌面端 SDL3 在部分宿主/输入法环境下不产生 SDL_EVENT_TEXT_INPUT，
			// 而 UI::send_key 会在 text_input_active() 时丢弃 KeyType::TEXT 的
			// KEY_DOWN。两者相抵消会让 ASCII 输入框（账号/密码/聊天）永远收不到字符。
			// 与 UISoftKey(Mode::Text) 的既有做法一致：ASCII-only 字段关掉 SDL
			// 文本输入，改走 KEY_DOWN ASCII 通路；需要中文 IME 的字段仍保留
			// StartTextInput，由 send_text() 送 UTF-8。
			if (focusedtextfield->accepts_unicode())
				Window::get().begin_ascii_text_input();
			else
				Window::get().end_ascii_text_input();
		}
	}

	void UI::remove_textfield()
	{
		// Clear first so set_state(DISABLED) → remove_textfield cannot bounce
		// the same field back to NORMAL (that used to undo DISABLED).
		focusedtextfield = {};
		Window::get().end_ascii_text_input();
	}

	void UI::drag_icon(Icon* icon)
	{
		state->drag_icon(icon);
	}

	void UI::add_keymapping(uint8_t no, uint8_t type, int32_t action)
	{
		keyboard.assign(no, type, action);
	}

	void UI::clear_tooltip(Tooltip::Parent parent)
	{
		state->clear_tooltip(parent);
	}

	void UI::show_equip(Tooltip::Parent parent, int16_t slot)
	{
		state->show_equip(parent, slot);
	}

	void UI::show_item(Tooltip::Parent parent, int32_t item_id)
	{
		state->show_item(parent, item_id);
	}

	void UI::show_skill(Tooltip::Parent parent, int32_t skill_id, int32_t level, int32_t masterlevel, int64_t expiration)
	{
		state->show_skill(parent, skill_id, level, masterlevel, expiration);
	}

	void UI::show_text(Tooltip::Parent parent, std::string text)
	{
		state->show_text(parent, text);
	}

	void UI::show_map(Tooltip::Parent parent, std::string title, std::string description, int32_t mapid, bool bolded, bool portal)
	{
		state->show_map(parent, title, description, mapid, bolded, portal);
	}

	Keyboard& UI::get_keyboard()
	{
		return keyboard;
	}

	void UI::remove(UIElement::Type type)
	{
		focusedtextfield = {};

		state->remove(type);
	}

	void UI::activate_after_transition(UIElement::Type type)
	{
		state->activate_after_transition(type);
	}

	void UI::bring_to_front(UIElement::Type type)
	{
		state->bring_to_front(type);
	}
}