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
#include "UIStateCashShop.h"

#include "UITypes/UICashShop.h"

namespace ms
{
	std::vector<CashCommodity> UIStateCashShop::cashshop_commodities;
	bool UIStateCashShop::cash_balances_ready = false;
	int32_t UIStateCashShop::cash_nx_credit = 0;
	int32_t UIStateCashShop::cash_maple_point = 0;
	int32_t UIStateCashShop::cash_nx_prepaid = 0;
	std::vector<int32_t> UIStateCashShop::cash_best_sellers;
	std::vector<CashInvEntry> UIStateCashShop::cash_inventory;
	std::vector<CashGiftEntry> UIStateCashShop::cash_gifts;
	std::array<int32_t, 10> UIStateCashShop::cash_wishlist{};
	int16_t UIStateCashShop::cash_storage_slots = 0;
	int16_t UIStateCashShop::cash_char_slots = 0;
	std::vector<int32_t> UIStateCashShop::local_cart_sns;

	UIStateCashShop::UIStateCashShop()
	{
		focused = UIElement::NONE;

		emplace<UICashShop>();
	}

	void UIStateCashShop::draw(float inter, Point<int16_t> cursor) const
	{
		// EnumMap 按 Type 升序：NOTICE/SOFTKEYBOARD < CASHSHOP，若整表顺序绘
		// 会把购买确认 / SoftKey 盖在商城底板下（可见光标点得到、窗体看不见）。
		// 先画非焦点，最后画 focused（UICashBuyConfirm / SoftKey 均 FOCUSED）。
		UIElement* focused_el = nullptr;

		for (auto iter : elements)
		{
			UIElement* element = iter.second.get();

			if (!element || !element->is_active())
				continue;

			if (iter.first == focused)
				focused_el = element;
			else
				element->draw(inter);
		}

		if (focused_el)
			focused_el->draw(inter);

		// 商城 tooltip 每帧最后绘制（与 UIStateGame 同口径：光标下方 +22px）。
		// 商城态没有装备/技能窗，只有 item/equip/text 三类会被派发。
		if (tooltip)
			tooltip->draw(cursor + Point<int16_t>(0, 22));
	}

	void UIStateCashShop::update()
	{
		for (auto iter : elements)
		{
			UIElement* element = iter.second.get();

			if (element && element->is_active())
			{
				element->refresh_localized();
				element->update();
			}
		}
	}

	void UIStateCashShop::send_close()
	{
		// [退出防护] 商城态点窗口 X / SDL_QUIT 曾为空实现——"点关闭没反应"。
		// 与 ESC 路径一致走 exit_cashshop()：本地即时换图 + 服务端权威刷新，
		// 不用 UI::quit() 直接杀进程（原生行为是退回游戏）。
		if (auto cs = static_cast<UICashShop*>(get(UIElement::Type::CASHSHOP)))
		{
			if (cs->is_active())
				cs->exit_cashshop();
		}
	}

	void UIStateCashShop::send_key(KeyType::Id, int32_t action, bool pressed, bool escape)
	{
		// 须转发 key-up：预览沙盒 send_action(..., false) 依赖松开。
		if (auto focusedelement = get(focused))
		{
			if (focusedelement->is_active())
			{
				focusedelement->send_key(action, pressed, escape);
				return;
			}

			focused = UIElement::Type::NONE;
		}

		if (auto cs = get(UIElement::Type::CASHSHOP))
		{
			if (cs->is_active())
				cs->send_key(action, pressed, escape);
		}
	}

	// ---- tooltip：严格照抄 UIStateGame.cpp:1010-1080 的实现模式 ----
	// 商城态不产生 equip(slot)/skill/map 场景，但 clear/show_item/show_text
	// 三条热路径须与 GAME 态同构，否则 UICashShop::send_cursor 调 UI::show_item
	// 时 UI::state->show_item 落到本 state 的空 stub，tooltip 永远不显示。
	void UIStateCashShop::clear_tooltip(Tooltip::Parent parent)
	{
		if (parent == tooltipparent)
		{
			eqtooltip.set_equip(Tooltip::Parent::NONE, 0);
			ittooltip.set_item(0);
			tetooltip.set_text("");
			tooltip = {};
			tooltipparent = Tooltip::Parent::NONE;
		}
	}

	void UIStateCashShop::show_equip(Tooltip::Parent parent, int16_t slot)
	{
		eqtooltip.set_equip(parent, slot);

		if (slot)
		{
			tooltip = eqtooltip;
			tooltipparent = parent;
		}
	}

	void UIStateCashShop::show_item(Tooltip::Parent parent, int32_t itemid)
	{
		// 与 UIStateGame::show_item 同口径：装备号段（1xxxxxx）走 EquipTooltip，
		// 消耗/安装/etc/普通 cash 走 ItemTooltip。不臆造新判定。
		if (itemid && itemid / 1000000 == 1)
		{
			eqtooltip.set_equip_itemid(itemid);
			tooltip = eqtooltip;
			tooltipparent = parent;
			return;
		}

		ittooltip.set_item(itemid);

		if (itemid)
		{
			tooltip = ittooltip;
			tooltipparent = parent;
		}
	}

	void UIStateCashShop::show_text(Tooltip::Parent parent, std::string text)
	{
		tetooltip.set_text(text);

		if (!text.empty())
		{
			tooltip = tetooltip;
			tooltipparent = parent;
		}
	}

	Cursor::State UIStateCashShop::send_cursor(Point<int16_t> cursor_position, Cursor::State cursor_state)
	{
		bool clicked = cursor_state == Cursor::State::CLICKING || cursor_state == Cursor::State::VSCROLLIDLE;

		// 与 UIStateGame::send_cursor 同口径：每帧派发入口先清 tooltip，
		// UICashShop::send_cursor 命中商品/热销图标时同帧重挂，移出即消失。
		if (tooltipparent != Tooltip::Parent::NONE)
			clear_tooltip(tooltipparent);

		if (auto focusedelement = get(focused))
		{
			if (focusedelement->is_active())
			{
				remove_cursor(focusedelement->get_type());

				return focusedelement->send_cursor(clicked, cursor_position);
			}
			else
			{
				focused = UIElement::NONE;

				return cursor_state;
			}
		}
		else
		{
			if (auto front = get_front())
			{
				remove_cursor(front->get_type());

				return front->send_cursor(clicked, cursor_position);
			}
			else
			{
				return Cursor::State::IDLE;
			}
		}
	}

	UIState::Iterator UIStateCashShop::pre_add(UIElement::Type type, bool toggled, bool is_focused)
	{
		remove(type);

		if (is_focused)
			focused = type;

		return elements.find(type);
	}

	void UIStateCashShop::remove(UIElement::Type type)
	{
		if (focused == type)
			focused = UIElement::Type::NONE;

		if (auto& element = elements[type])
		{
			element->deactivate();
			element.release();
		}
	}

	UIElement* UIStateCashShop::get(UIElement::Type type)
	{
		return elements[type].get();
	}

	UIElement* UIStateCashShop::get_front()
	{
		if (auto focusedelement = get(focused))
		{
			if (focusedelement->is_active())
				return focusedelement;
		}

		UIElement* front = nullptr;

		for (auto iter : elements)
		{
			auto& element = iter.second;

			if (element && element->is_active())
				front = element.get();
		}

		return front;
	}

	bool UIStateCashShop::has_active_focused()
	{
		if (auto focusedelement = get(focused))
			return focusedelement->is_active();

		return false;
	}

	void UIStateCashShop::remove_cursor(UIElement::Type type)
	{
		for (auto iter : elements)
		{
			auto& element = iter.second;

			if (element && element->is_active() && element->get_type() != type)
				element->remove_cursor();
		}
	}

	template <class T, typename...Args>
	void UIStateCashShop::emplace(Args&& ...args)
	{
		if (auto iter = pre_add(T::TYPE, T::TOGGLED, T::FOCUSED))
		{
			(*iter).second = std::make_unique<T>(
				std::forward<Args>(args)...
				);
		}
	}
}