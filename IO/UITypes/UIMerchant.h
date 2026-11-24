////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"
#include "../Components/Textfield.h"

#include "../../Data/ItemData.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// 个人商店 / 雇佣商人货架窗。
	// 资源：UIWindow.img/PersonalShop（对齐 Angel CPersonalShopDlg / s9 ROOM）。
	// 数据：PLAYER_INTERACTION ROOM sub=4|5 与 updateHiredMerchant(0x19)。
	class UIMerchant : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MERCHANT;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UIMerchant();

		void draw(float inter) const override;
		void update() override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;

		Button::State button_pressed(uint16_t buttonid) override;

		UIElement::Type get_type() const override;

		struct MerchItem
		{
			int32_t itemid;
			int16_t quantity;
			int32_t price;
			int16_t bundles;
		};
		void set_items(const std::vector<MerchItem>& items);
		void set_owner(const std::string& owner);

	private:
		static constexpr uint8_t MAX_SLOTS = 16u;
		// 货架列表区：select 162×35；Angel GetItemIndexFromPoint x<396。
		static constexpr int16_t ITEM_X = 14;
		static constexpr int16_t ITEM_Y0 = 76;
		static constexpr int16_t ITEM_ROW = 35;
		static constexpr int16_t ITEM_W = 162;
		static constexpr int16_t VISIBLE_ROWS = 12;

		enum Buttons : uint16_t
		{
			BtClose,
			BtExit,
			BtBuy,
			BtStart,
			BtInfo,
			BtItem
		};

		void request_buy();
		void request_put_item();
		void request_open_store();
		void send_exit();
		int8_t hit_slot(Point<int16_t> rel) const;

		Texture background;
		Texture select_tex;
		Texture soldout_tex;
		bool has_background = false;
		bool has_select = false;
		bool has_soldout = false;

		Text owner_label;
		Text tip_label;
		Text price_labels[MAX_SLOTS];
		std::vector<MerchItem> merch_items;
		int8_t selected_slot = -1;
		int16_t scroll = 0;
	};
}
