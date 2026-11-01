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

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	// 现金商城商品（来自服务端 getCSPacket 主商品列表），供 UICashShop 消费。
	struct CashCommodity
	{
		int32_t sn;
		int32_t itemid;
		int32_t price;
		int16_t count;
		std::string name;
	};

	// showCashInventory 条目（精简：cashId / itemId / sn / qty）
	struct CashInvEntry
	{
		int64_t cash_id = 0;
		int32_t itemid = 0;
		int32_t sn = 0;
		int16_t qty = 0;
	};

	struct CashGiftEntry
	{
		int64_t cash_id = 0;
		int32_t itemid = 0;
		std::string from;
	};

	class UIStateCashShop : public UIState
	{
	public:
		UIStateCashShop();

		void draw(float inter, Point<int16_t> cursor) const override;
		void update() override;

		void doubleclick(Point<int16_t>) override {}
		void rightclick(Point<int16_t>) override {}
		void send_key(KeyType::Id, int32_t, bool, bool) override {}
		Cursor::State send_cursor(Point<int16_t> cursor_position, Cursor::State cursor_state) override;
		void send_scroll(double yoffset) override {}
		void send_close() override {}

		void drag_icon(Icon*) override {}
		void clear_tooltip(Tooltip::Parent) override {}
		void show_equip(Tooltip::Parent, int16_t) override {}
		void show_item(Tooltip::Parent, int32_t) override {}
		void show_skill(Tooltip::Parent, int32_t, int32_t, int32_t, int64_t) override {}
		void show_text(Tooltip::Parent, std::string) override {}
		void show_map(Tooltip::Parent, std::string, std::string, int32_t, bool, bool) override {}

		Iterator pre_add(UIElement::Type type, bool toggled, bool focused) override;
		void remove(UIElement::Type type) override;
		UIElement* get(UIElement::Type type) override;
		UIElement* get_front();
		UIElement* get_front(std::list<UIElement::Type>) override { return nullptr; }
		UIElement* get_front(Point<int16_t>) override { return nullptr; }

	private:
		void remove_cursor(UIElement::Type type);

		template <class T, typename...Args>
		void emplace(Args&& ...args);

		EnumMap<UIElement::Type, UIElement::UPtr, UIElement::Type::NUM_TYPES> elements;
		UIElement::Type focused;

	public:
		// 由 SetCashShopHandler 解析 getCSPacket 后填充，UICashShop 构造时消费。
		static std::vector<CashCommodity> cashshop_commodities;

		// 由 QueryCashResultHandler 解析 s9 showCash（QUERY_CASH_RESULT=324）：
		// int NX_CREDIT / int MAPLE_POINT / int NX_PREPAID。
		static bool cash_balances_ready;
		static int32_t cash_nx_credit;
		static int32_t cash_maple_point;
		static int32_t cash_nx_prepaid;

		// CashShopOperation 0x4B / 0x4D / 0x4F|0x55
		static std::vector<CashInvEntry> cash_inventory;
		static std::vector<CashGiftEntry> cash_gifts;
		static std::array<int32_t, 10> cash_wishlist;
		static int16_t cash_storage_slots;
		static int16_t cash_char_slots;
	};
}