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
#include "UIMerchant.h"

#include "../UI.h"
#include "../Components/MapleButton.h"

#include "../../Net/Packets/PlayerInteractionPackets.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIMerchant::UIMerchant() : owner_label(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::WHITE, "")
	{
		dimension = Point<int16_t>(400, 520);
#ifdef USE_NX
		nl::node panel = nl::nx::UI["CashShop.img"]["CSList"]["Base"];
		if (panel)
		{
			background = panel;
			has_background = true;
			dimension = background.get_dimensions();
		}
		bt_close_node = nl::nx::UI["CashShop.img"]["CSTab"]["BtExit"];
		bt_buy_node = nl::nx::UI["CashShop.img"]["CSList"]["BtBuy"];
#endif
		buttons[Buttons::BtClose] = std::make_unique<MapleButton>(
			bt_close_node,
			Point<int16_t>(dimension.x() - 20, 6)
		);
	}

	void UIMerchant::draw(float inter) const
	{
		if (has_background)
			background.draw(position, inter);

		owner_label.draw(position + Point<int16_t>(10, 8));

		for (size_t i = 0; i < merch_items.size() && i < MAX_SLOTS; i++)
		{
			const MerchItem& mi = merch_items[i];
			Point<int16_t> base = position + Point<int16_t>(20, 40 + static_cast<int16_t>(i) * 30);

			ItemData::get(mi.itemid).get_icon(false).draw(DrawArgument(base, 1.0f, 1.0f));
			price_labels[i].draw(base + Point<int16_t>(40, 4));
		}

		UIElement::draw_buttons(inter);
	}

	Button::State UIMerchant::button_pressed(uint16_t buttonid)
	{
		if (buttonid == Buttons::BtClose)
		{
			deactivate();
			return Button::State::NORMAL;
		}

		if (buttonid >= Buttons::BtBuy)
		{
			size_t index = buttonid - Buttons::BtBuy;
			if (index < merch_items.size())
			{
				// 购买：slot = 物品序号（0 基），quantity=1（单捆购买），与 s9 MERCHANT_BUY(0x22) 对齐。
				HiredMerchantBuyPacket(static_cast<int8_t>(index), 1).dispatch();
			}
			return Button::State::NORMAL;
		}

		return Button::State::DISABLED;
	}

	UIElement::Type UIMerchant::get_type() const
	{
		return TYPE;
	}

	void UIMerchant::set_items(const std::vector<MerchItem>& items)
	{
		merch_items = items;

		for (size_t i = 0; i < MAX_SLOTS; i++)
		{
			if (i < merch_items.size())
			{
				const MerchItem& mi = merch_items[i];
				std::string text = ItemData::get(mi.itemid).get_name() + "  " + std::to_string(mi.price) + "  (" + std::to_string(mi.quantity) + ")";
				price_labels[i].change_text(text);

				buttons[Buttons::BtBuy + static_cast<uint16_t>(i)] = std::make_unique<MapleButton>(
					bt_buy_node,
					position + Point<int16_t>(300, 40 + static_cast<int16_t>(i) * 30)
				);
				buttons[Buttons::BtBuy + static_cast<uint16_t>(i)]->set_active(true);
			}
			else
			{
				price_labels[i].change_text("");
				buttons[Buttons::BtBuy + static_cast<uint16_t>(i)] = nullptr;
			}
		}
	}

	void UIMerchant::set_owner(const std::string& owner)
	{
		owner_label.change_text(owner + " 的商店");
	}
}
