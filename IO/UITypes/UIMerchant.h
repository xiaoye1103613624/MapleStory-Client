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

#include "../../Data/ItemData.h"
#include "../../Graphics/Text.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// 个人商店 / 雇佣商人窗口：展示店主上架道具并支持购买。
	// 数据来自 s9 getHiredMerchant(PACKET: PLAYER_INTERACTION action ROOM=5) 与 updateHiredMerchant(action 0x19)。
	class UIMerchant : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MERCHANT;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UIMerchant();

		void draw(float inter) const override;
		void update() override {}
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;

		Button::State button_pressed(uint16_t buttonid) override;

		UIElement::Type get_type() const override;

		// 由 HiredMerchantHandler 调用：用服务端下发的商品列表刷新。
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

		enum Buttons : uint16_t
		{
			BtClose,
			BtBuy
		};

		Sprite background;
		bool has_background = false;
#ifdef USE_NX
		nl::node bt_close_node;
		nl::node bt_buy_node;
#endif
		Text owner_label;
		Text price_labels[MAX_SLOTS];
		std::vector<MerchItem> merch_items;
		int8_t selected_slot = -1;
	};
}
