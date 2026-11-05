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
#include "UIStateLogin.h"

#include "UITypes/UICharSelect.h"
#include "UITypes/UILogin.h"
#include "UITypes/UILoginNotice.h"
#include "UITypes/UILogo.h"
#include "UITypes/UIRegion.h"
#include "UITypes/UIWorldSelect.h"

#include "../Configuration.h"
#include "../Constants.h"
#include "../MapleStory.h"

#include "LoginBackground.h"

namespace ms
{
	std::vector<UIStateLogin::ViewAllCharWorld> UIStateLogin::viewall_worlds;
	bool UIStateLogin::viewall_ready = false;
	int32_t UIStateLogin::viewall_expected_worlds = 0;

	UIStateLogin::UIStateLogin()
	{
		focused = UIElement::Type::NONE;

		VWIDTH = Constants::Constants::get().get_viewwidth();
		VHEIGHT = Constants::Constants::get().get_viewheight();

		bool start_shown = Configuration::get().get_start_shown();

		if (!start_shown)
			emplace<UILogo>();
		else
			emplace<UILogin>();
	}

	void UIStateLogin::draw(float inter, Point<int16_t> cursor) const
	{
		// BeiDou 登录流程共用背景（地图 + 书页外框）。Logo 画面不叠加。
		if (LoginBackground::get().available())
		{
			UIElement* logo = const_cast<UIStateLogin*>(this)->get(UIElement::Type::START);

			if (!logo || !logo->is_active())
				LoginBackground::get().draw(inter);
		}

		for (auto iter : elements)
		{
			UIElement* element = iter.second.get();

			if (element && element->is_active())
				element->draw(inter);
		}

		if (tooltip)
			tooltip->draw(cursor + Point<int16_t>(0, 22));
	}

	void UIStateLogin::update()
	{
		if (LoginBackground::get().available())
			LoginBackground::get().update();

		// 请求门闩看护：UILoginWait 取消时只 deactivate（不能在按钮回调里
		// remove(this)），残留实例会让 UIWorldSelect / UICharSelect 的
		// “请求进行中”门闩永久置位（世界点不动、选角点不动）。
		// 这里在下一帧统一移除失效 Wait 并复位各窗口门闩。
		if (UIElement* wait = get(UIElement::Type::LOGINWAIT))
		{
			if (!wait->is_active())
			{
				remove(UIElement::Type::LOGINWAIT);

				if (auto charselect = UI::get().get_element<UICharSelect>())
					charselect->clear_pending();

				if (auto worldselect = UI::get().get_element<UIWorldSelect>())
					worldselect->reset_request_state();
			}
		}

		// VIEWALLCHAR 关闭（BtVAC / ESC / 选角成功）只会 deactivate 自己。
		// 这里恢复弹出时被失活的下层窗口，保持「回到选角流程」的原有语义；
		// 若已切到游戏状态，本 UIStateLogin 会被销毁，不会误恢复。
		if (!viewall_hidden.empty())
		{
			UIElement* viewall = get(UIElement::Type::VIEWALLCHAR);

			if (!viewall || !viewall->is_active())
			{
				for (auto hidden : viewall_hidden)
				{
					if (UIElement* element = get(hidden))
						element->makeactive();
				}

				viewall_hidden.clear();
			}
		}

		// 补间结束：激活被延迟的步进窗口（等价于参考端 OnStepChanged 的窗口切换）。
		if (pending_activation != UIElement::Type::NONE && !LoginBackground::get().is_transitioning())
		{
			if (UIElement* element = get(pending_activation))
				element->makeactive();

			pending_activation = UIElement::Type::NONE;
		}

		bool screen_changed = false;
		const int16_t new_width = Constants::Constants::get().get_viewwidth();
		const int16_t new_height = Constants::Constants::get().get_viewheight();

		if (VWIDTH != new_width || VHEIGHT != new_height)
		{
			screen_changed = true;
			VWIDTH = new_width;
			VHEIGHT = new_height;
			LOG(LOG_INFO, "UIStateLogin resize " << new_width << "x" << new_height);
		}

		for (auto iter : elements)
		{
			UIElement* element = iter.second.get();

			if (element && element->is_active())
			{
				element->refresh_localized();
				element->update();

				if (screen_changed)
					element->update_screen(new_width, new_height);
			}
		}
	}

	void UIStateLogin::doubleclick(Point<int16_t> pos)
	{
		if (auto charselect = UI::get().get_element<UICharSelect>())
			charselect->doubleclick(pos);
	}

	void UIStateLogin::send_key(KeyType::Id type, int32_t action, bool pressed, bool escape)
	{
		if (UIElement* focusedelement = get(focused))
		{
			if (focusedelement->is_active())
			{
				return focusedelement->send_key(action, pressed, escape);
			}
			else
			{
				focused = UIElement::Type::NONE;

				return;
			}
		}
	}

	Cursor::State UIStateLogin::send_cursor(Point<int16_t> cursor_position, Cursor::State cursor_state)
	{
		bool clicked = cursor_state == Cursor::State::CLICKING || cursor_state == Cursor::State::VSCROLLIDLE;

		if (auto focusedelement = get(focused))
		{
			if (focusedelement->is_active())
			{
				remove_cursor(focusedelement->get_type());

				return focusedelement->send_cursor(clicked, cursor_position);
			}

			// 焦点窗已 deactivate（如 Cancel Loading）时清焦点并回落到前台，
			// 勿直接 return，否则当次点击被吞、登录按钮像「没了」。
			focused = UIElement::Type::NONE;
		}

		if (auto front = get_front())
		{
			remove_cursor(front->get_type());

			return front->send_cursor(clicked, cursor_position);
		}

		return Cursor::State::IDLE;
	}

	void UIStateLogin::send_close()
	{
		auto logo = UI::get().get_element<UILogo>();
		auto login = UI::get().get_element<UILogin>();
		auto region = UI::get().get_element<UIRegion>();

		if (logo && logo->is_active() || login && login->is_active() || region && region->is_active())
			UI::get().quit();
		else
			UI::get().emplace<UIQuitConfirm>();
	}

	void UIStateLogin::clear_tooltip(Tooltip::Parent parent)
	{
		if (parent == tooltipparent)
		{
			tetooltip.set_text("");
			tooltip = {};
			tooltipparent = Tooltip::Parent::NONE;
		}
	}

	void UIStateLogin::show_text(Tooltip::Parent parent, std::string text)
	{
		tetooltip.set_text(text);

		if (!text.empty())
		{
			tooltip = tetooltip;
			tooltipparent = parent;
		}
	}

	template <class T, typename...Args>
	void UIStateLogin::emplace(Args&& ...args)
	{
		if (auto iter = pre_add(T::TYPE, T::TOGGLED, T::FOCUSED))
		{
			(*iter).second = std::make_unique<T>(
				std::forward<Args>(args)...
			);

			post_add(T::TYPE, (*iter).second.get());
		}
	}

	void UIStateLogin::post_add(UIElement::Type type, UIElement* element)
	{
		if (!element)
			return;

		// 「查看所有角色」是独占整屏窗口。draw()/update() 会遍历所有 active 元素，
		// 若选角页、建角页、世界页等下层窗口仍 active，就会与它同屏叠影
		// （左侧木牌、吊牌、按钮重影）。这里只失活、不销毁实例，关闭后由
		// update() 按 viewall_hidden 恢复；不改木牌数量与位置，也不破坏返回选角流程。
		if (type == UIElement::Type::VIEWALLCHAR)
		{
			viewall_hidden.clear();

			for (auto iter : elements)
			{
				UIElement* other = iter.second.get();

				if (other && other->is_active() && other->get_type() != type)
				{
					other->deactivate();
					viewall_hidden.push_back(other->get_type());
				}
			}

			return;
		}

		if (!is_step_window(type))
			return;

		// 参考端 CLogin::OnStepChanged 在补间结束后才显示新窗口。
		// 构造期已把世界列表/角色数据装好，这里只推迟可见与输入。
		if (LoginBackground::get().available() && LoginBackground::get().is_transitioning())
		{
			element->deactivate();
			pending_activation = type;
		}
	}

	void UIStateLogin::activate_after_transition(UIElement::Type type)
	{
		if (LoginBackground::get().available() && LoginBackground::get().is_transitioning())
		{
			pending_activation = type;
			return;
		}

		if (UIElement* element = get(type))
			element->makeactive();
	}

	bool UIStateLogin::is_step_window(UIElement::Type type)
	{
		switch (type)
		{
			case UIElement::Type::LOGIN:
			case UIElement::Type::WORLDSELECT:
			case UIElement::Type::CHARSELECT:
			case UIElement::Type::RACESELECT:
			case UIElement::Type::CLASSCREATION:
				return true;
			default:
				return false;
		}
	}

	UIState::Iterator UIStateLogin::pre_add(UIElement::Type type, bool, bool is_focused)
	{
		remove(type);

		if (is_focused)
			focused = type;

		return elements.find(type);
	}

	void UIStateLogin::remove(UIElement::Type type)
	{
		if (focused == type)
			focused = UIElement::Type::NONE;

		if (pending_activation == type)
			pending_activation = UIElement::Type::NONE;

		if (auto& element = elements[type])
		{
			element->deactivate();
			element.release();
		}
	}

	UIElement* UIStateLogin::get(UIElement::Type type)
	{
		return elements[type].get();
	}

	UIElement* UIStateLogin::get_front()
	{
		UIElement* front = nullptr;

		for (auto iter : elements)
		{
			auto& element = iter.second;

			if (element && element->is_active())
				front = element.get();
		}

		return front;
	}

	UIElement* UIStateLogin::get_front(std::list<UIElement::Type> types)
	{
		auto begin = types.rbegin();
		auto end = types.rend();

		for (auto iter = begin; iter != end; ++iter)
		{
			auto& element = elements[*iter];

			if (element && element->is_active())
				return element.get();
		}

		return nullptr;
	}

	UIElement* UIStateLogin::get_front(Point<int16_t> cursor_position)
	{
		auto begin = elements.values().rbegin();
		auto end = elements.values().rend();

		for (auto iter = begin; iter != end; ++iter)
		{
			auto& element = *iter;

			if (element && element->is_active() && element->is_in_range(cursor_position))
				return element.get();
		}

		return nullptr;
	}

	void UIStateLogin::remove_cursor(UIElement::Type type)
	{
		for (auto iter : elements)
		{
			auto& element = iter.second;

			if (element && element->is_active() && element->get_type() != type)
				element->remove_cursor();
		}
	}
}