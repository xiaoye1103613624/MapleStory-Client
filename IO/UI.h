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

#include "UIState.h"

#include "Components/ScrollingNotice.h"
#include "Components/Textfield.h"

namespace ms
{
	class UI : public Singleton<UI>
	{
	public:
		enum State
		{
			LOGIN,
			GAME,
			CASHSHOP
		};

		UI();

		void init();
		void draw(float alpha) const;
		void update();

		void enable();
		void disable();
		void change_state(State state);

		void quit();
		bool not_quitted() const;
		bool has_capslocks() const;

		/// 只读光标位置：供各 UIElement::send_scroll 做「光标落在本窗内」门控。
		/// UIStateGame::send_scroll 广播给所有 active 元素，不门控会一次滚多个窗。
		Point<int16_t> get_cursor_position() const { return cursor.get_position(); }

		/// 物理主键（左键）当前是否按住。供「+」长按连升等需要持续按住态的 UI 读取；
		/// 由 send_cursor(bool) 的 down/up 维护，窗口失焦时强制清。
		bool is_primary_down() const { return primary_down; }

		void send_cursor(Point<int16_t> cursor_position);
		void send_cursor(bool pressed);
		void send_cursor(Point<int16_t> cursor_position, Cursor::State cursor_state);
		void send_focus(int focused);
		void send_scroll(double yoffset);
		void send_close();
		void rightclick();
		void doubleclick();
		void send_key(int32_t keycode, bool pressed);
		/// Insert UTF-8 / ASCII from SDL_EVENT_TEXT_INPUT into the focused field.
		void send_text(const std::string& text);

		void set_scrollnotice(const std::string& notice);
		void focus_textfield(Textfield* textfield);
		void remove_textfield();
		void drag_icon(Icon* icon);

		void add_keymapping(uint8_t no, uint8_t type, int32_t action);

		/// 主菜单/底栏/指令共用的"商城"入口。
		/// ui.cashshop.window_mode=true → 打开 UIWindowCashShop（窗口内商城）；
		/// false（默认）或缺配置 → 走原生商城 EnterCashShopPacket。
		void open_cashshop_entry();

		void clear_tooltip(Tooltip::Parent parent);
		void show_equip(Tooltip::Parent parent, int16_t slot);
		void show_item(Tooltip::Parent parent, int32_t item_id);
		void show_skill(Tooltip::Parent parent, int32_t skill_id, int32_t level, int32_t masterlevel, int64_t expiration);
		void show_text(Tooltip::Parent parent, std::string text);
		void show_map(Tooltip::Parent parent, std::string title, std::string description, int32_t mapid, bool bolded, bool portal,
			const std::vector<MapTooltipPlayer>& players = {});

		Keyboard& get_keyboard();

		template <class T, typename...Args>
		Optional<T> emplace(Args&& ...args)
		{
			if (auto iter = state->pre_add(T::TYPE, T::TOGGLED, T::FOCUSED))
			{
				(*iter).second = std::make_unique<T>(
					std::forward<Args>(args)...
					);

				if (UIElement* element = (*iter).second.get())
					state->post_add(T::TYPE, element);
			}

			return state->get(T::TYPE);
		}

		template <class T>
		Optional<T> get_element()
		{
			UIElement::Type type = T::TYPE;
			UIElement* element = state->get(type);

			return static_cast<T*>(element);
		}

		void remove(UIElement::Type type);
		// 登录态：若背景仍在补间，等滚到端点再激活目标窗口。
		void activate_after_transition(UIElement::Type type);
		// 把窗口移到绘制队列末尾（= 最上层）。由 UIElement::makeactive() 在 false→true 跃迁时调用。
		void bring_to_front(UIElement::Type type);
		/// ThemePack runtime switch (GAME state only): rebuild StatusBar + ChatBar.
		void apply_hud_theme();

	private:
		std::unique_ptr<UIState> state;
		Keyboard keyboard;
		Cursor cursor;
		ScrollingNotice scrollingnotice;

		Optional<Textfield> focusedtextfield;
		std::unordered_map<int32_t, bool> is_key_down;

		bool enabled;
		bool quitted;
		bool caps_lock_enabled;
		bool primary_down = false;
	};
}