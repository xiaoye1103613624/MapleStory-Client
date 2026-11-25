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
#include "../Net/Packets/CashShopPackets.h"
#include "../Net/Packets/GameplayPackets.h"   // EnterCashShopPacket（open_cashshop_entry 原生分支）
#include "../Net/Handlers/CashShopWindowSync.h"
#include "../Util/Misc.h"
#include "../Util/PerfLog.h"
#include "../Util/ResolutionTiers.h"

#include "UITypes/UIChannel.h"
#include "UITypes/UIChat.h"
#include "UITypes/UIChatBar.h"
#include "UITypes/UIWindowCashShop.h"
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

		// 窗口内商城（方案 C）：收包线程只入队，这里在主线程每帧消费并
		// 转发给 UIWindowCashShop（包括 OPEN 时 emplace 窗口）。
		CashShopWindowSync::pump();

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
		// [退出防护] 联动窗口门闩：此前 close_requested 恒 false，running() 只靠
		// 本标志退出；两处一起置位，避免"UI 已退但窗口还认为开着"的不一致态。
		Window::get().request_close();
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
		// 失焦时收不到 mouse-up：强制清按住态，避免长按连升卡在按下态。
		if (!focused)
			primary_down = false;

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
		// 物理主键按住态：down(true) 置位、up(false) 清位。长按连升等 per-frame
		// 逻辑读 is_primary_down()；失焦在 send_focus(false) 里强制清。
		primary_down = pressed;

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

			// 第二轮 P1 修复：道具/技能/宏热键是 one-shot，长按出现的 repeat
			// （SDL KEY_DOWN 的 event.key.repeat，本端按 pressed 直传）必须吞掉，
			// 否则按住药水键会连发清空整叠。对照 MinMS IO/UI.cpp:401-408。
			// 注意 is_key_down[keycode] 在本帧末(:553)才更新，故此处为真表示"上一帧已按下"。
			if (pressed && is_key_down[keycode]
				&& (mapping.type == KeyType::Id::ITEM
					|| mapping.type == KeyType::Id::SKILL
					|| mapping.type == KeyType::Id::MACRO))
			{
				return;
			}

			// SoftKey 字母捷径（UIAdmin / UIBuffList / UIShortCut / UIStatusBar / UIGameMenu…）
			// 用 ASCII 比较 'A'/'S'/'D'/'4'。未绑定键 get_mapping → NONE/0，捷径永不触发。
			// GLFW 可打印键码与 ASCII 同值（A=65…Z=90，'4'=52），未绑定时把 keycode
			// 当作 action 下传；已绑定键仍走 KEYMAP，不受影响。
			//
			// 软键盘总闸（ui.softkey.hotkeys，默认 false）：原生 v83 不启用软键盘，
			// 这些"裸字母 → action → 各窗口 emplace<UISoftKey>"是历史过度实现。
			// 关闭时不再把可打印字母当 action 下传，从源头掐断所有裸键软键盘捷径；
			// 方向键/回车/小键盘回车仍单独兜底（焦点气球目录/列表依赖），不受影响。
			// 选角 PIC 软键盘走 UICharSelect/UIViewAllChar 的服务端状态驱动分支，
			// 不经这里的 ASCII 兜底，故完全不受此开关影响。
			// 静态缓存：send_key 为高频路径，配置只在首帧读取一次。
			static const bool softkey_hotkeys = Configuration::get().get_softkey_hotkeys();
			const bool printable = keycode >= GLFW_KEY_SPACE && keycode <= GLFW_KEY_Z;
			KeyType::Id map_type = mapping.type;
			int32_t map_action = mapping.action;
			if (map_type == KeyType::Id::NONE
				&& ((softkey_hotkeys && printable)
					// 方向键 / 回车默认不在 KEYMAP 里（get_mapping → NONE,0），
					// 不兜底传真实键码的话，焦点气球（目录/界面列表）的
					// send_key 收到的 action 恒为 0 → 上下键/回车永远无效。
					|| keycode == GLFW_KEY_UP || keycode == GLFW_KEY_DOWN
					|| keycode == GLFW_KEY_LEFT || keycode == GLFW_KEY_RIGHT
					|| keycode == GLFW_KEY_ENTER || keycode == GLFW_KEY_KP_ENTER))
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
				// X 键：举报窗/右键菜单的 send_key 用 'X'/'x' 关闭。
				// 归入 get_front 层级派发，SoftKey 打开时先关 SoftKey 再关宿主窗。
				bool close_x = keycode == GLFW_KEY_X;

				// All
				if (escape || tab || enter || arrows || close_x)
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

					// Game — 举报窗/右键菜单同样支持 Esc 关闭。
					// 二者 FOCUSED=true 且 send_key 已实现 escape→deactivate，
					// 但此前不在白名单，Esc 直接落到全局兜底去开目录（关不掉）。
					// get_front 按层级从后往前取：SoftKey 由 Report 打开时位于更上层，
					// 会先收到 Esc 关闭，下一次 Esc 才轮到 Report —— 天然层级优先。
					types.emplace_back(UIElement::Type::CONTEXTMENU);
					types.emplace_back(UIElement::Type::REPORT);
					types.emplace_back(UIElement::Type::NOTICE);
					types.emplace_back(UIElement::Type::QUICKSLOTCONFIG);
					types.emplace_back(UIElement::Type::KEYCONFIG);
					types.emplace_back(UIElement::Type::CHAT);
					types.emplace_back(UIElement::Type::EVENT);
					types.emplace_back(UIElement::Type::DAILYGIFT);
					types.emplace_back(UIElement::Type::STATSINFO);
					types.emplace_back(UIElement::Type::ITEMINVENTORY);
					types.emplace_back(UIElement::Type::EQUIPINVENTORY);
					types.emplace_back(UIElement::Type::TOTEMEQUIP);
					types.emplace_back(UIElement::Type::ANDROIDEQUIP);
					types.emplace_back(UIElement::Type::MYSTICBADGE);
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

					// 侧栏扩展三窗（2026-11-21）：ESC 也要能关，补齐白名单。
					// 三个窗口均已实现 send_key(escape→deactivate)，此前漏登记导致 ESC 落到全局开目录。
					types.emplace_back(UIElement::Type::DAILYCHECKIN);
					types.emplace_back(UIElement::Type::DAMAGERANK);
					types.emplace_back(UIElement::Type::BATTLESTATS);
					types.emplace_back(UIElement::Type::COLORPRISM);
					// 192 ShortCutUI 横向弹层：ESC 交由其 send_key 关闭。
					types.emplace_back(UIElement::Type::CONTENTSSHORTCUT);
				// 窗口内商城（方案 C）：FOCUSED 窗，ESC 交由其 send_key 关闭。
				types.emplace_back(UIElement::Type::WINDOWCASHSHOP);
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
					types.emplace_back(UIElement::Type::TOTEMEQUIP);
					types.emplace_back(UIElement::Type::ANDROIDEQUIP);
					types.emplace_back(UIElement::Type::MYSTICBADGE);
					types.emplace_back(UIElement::Type::SKILLBOOK);
					types.emplace_back(UIElement::Type::QUESTLOG);
					types.emplace_back(UIElement::Type::USERLIST);
				}
				else if (close_x)
				{
					// X 键关闭：SoftKey 在最上层，先关它；否则关举报窗/右键菜单。
					types.emplace_back(UIElement::Type::SOFTKEYBOARD);
					types.emplace_back(UIElement::Type::CONTEXTMENU);
					types.emplace_back(UIElement::Type::REPORT);
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
				// 临时属性明细板（UITemporaryStatView）默认隐藏，仅在 UIStateGame 建窗时
				// deactivate()。旧实现在这里加了一条全局 D 兜底开关：只要 D 未绑键，
				// 每次按 D（冲刺/走位/连带 buff 的高频键）都会 toggle 出「临时属性」黑底
				// 面板，并吞掉这次按键 —— 正是用户报的「用 buffer 后会弹临时属性」真因。
				// 该兜底还是一条调试/管理入口，不该出现在正常游戏链路，整块移除：
				// 面板保持隐藏态，需要时仍可由管理面板/专门入口显式打开。
				if (escape)
				{
					state->send_key(map_type, map_action, pressed, escape);
				}
				else if (enter)
				{
					// 回车优先派发给 active 的焦点窗（目录/界面气球的「回车激活当前项」）；
					// chatbar 先拦截会让气球永远收不到回车（用户实测「回车没效果」根因）。
					if (state && state->has_active_focused())
						state->send_key(map_type, map_action, pressed, escape);
					else if (auto chatbar = UI::get().get_element<UIChatBar>())
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

		// 空格双写修复（!item 命令因双空格解析失败，2026-11-18 实机）：
		// 聊天框 set_accept_unicode(true) → focus 时 begin_ascii_text_input，
		// 按空格键有两条插入路径同时命中：
		//   ① SDL_EVENT_TEXT_INPUT → send_text(" ") → add_string（本函数）
		//   ② KEY_DOWN 空格在 textactions 里映射成 ACTION/SPACE，UI::send_key
		//      的 TEXT_INPUT 跳过条件只覆盖 KeyType::TEXT、不覆盖 ACTION，
		//      于是 Textfield::send_key(ACTION,SPACE) 又 add_string 一次。
		// ASCII 空格由 ② 无条件可靠插入（输入法/宿主即便不发 TEXT_INPUT 也有），
		// 因此这里过滤"纯 ASCII 空格"的 TEXT_INPUT，保留 ② 单次插入。
		// 中文 IME 选字/上屏是汉字而非纯空格，不受影响；全角空格 U+3000 亦不命中。
		if (text.size() == 1 && text[0] == ' ')
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

	void UI::show_map(Tooltip::Parent parent, std::string title, std::string description, int32_t mapid, bool bolded, bool portal,
		const std::vector<MapTooltipPlayer>& players)
	{
		state->show_map(parent, title, description, mapid, bolded, portal, players);
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

	void UI::apply_hud_theme()
	{
		if (auto* game = dynamic_cast<UIStateGame*>(state.get()))
			game->apply_hud_theme();
		else
			LOG(LOG_WARN, "UI::apply_hud_theme: not in GAME state");
	}

	void UI::open_cashshop_entry()
	{
		if (Setting<WindowCashShopEnabled>::get().load())
		{
			// 方案 C：窗口内商城。清空上一个会话残留的收包事件，再打开窗口；
			// 窗口构造完成后会自行发 REQUEST_CATALOG 拉取分类索引/余额。
			CashShopWindowSync::clear();
			emplace<UIWindowCashShop>();
			LOG(LOG_INFO, "UI::open_cashshop_entry: window cash shop (ui.cashshop.window_mode=true)");
		}
		else
		{
			// 默认路径：原生商城（0x28），行为与改动前完全一致。
			EnterCashShopPacket().dispatch();
			LOG(LOG_DEBUG, "UI::open_cashshop_entry: native cash shop (default)");
		}
	}
}