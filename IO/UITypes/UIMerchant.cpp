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
#include "UICharInfo.h"
#include "UISoftKey.h"
#include "UIItemInventory.h"

#include "../UI.h"
#include "../Components/MapleButton.h"

#include "../../Net/Packets/PlayerInteractionPackets.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

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
		// CashShop.img/CSTab 下只有 Tab 一个子节点（nx_inspect 实测），没有 BtExit
		// ⇒ 关闭钮与其它功能窗共用同一颗描边 X（UiAssetResolve::close_btn() → BtClose2）。
		bt_close_node = UiAssetResolve::close_btn();
		bt_buy_node = nl::nx::UI["CashShop.img"]["CSList"]["BtBuy"];
#endif
		if (bt_close_node)
		{
			buttons[Buttons::BtClose] = std::make_unique<MapleButton>(
				bt_close_node,
				Point<int16_t>(dimension.x() - 20, 6)
			);
		}
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
			MinigameExitPacket().dispatch();
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

	Cursor::State UIMerchant::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State st = UIElement::send_cursor(clicked, cursorpos);
		if (!clicked || !active)
			return st;
		Point<int16_t> rel = cursorpos - position;
		if (rel.x() < 16 || rel.x() > 360)
			return st;
		int16_t row = static_cast<int16_t>((rel.y() - 40) / 30);
		if (row >= 0 && row < static_cast<int16_t>(merch_items.size()) && row < MAX_SLOTS)
		{
			selected_slot = static_cast<int8_t>(row);
			LOG(LOG_DEBUG, "UIMerchant: select slot=" << static_cast<int>(selected_slot));
		}
		return st;
	}

	void UIMerchant::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) {
			MinigameExitPacket().dispatch();
			deactivate();
			return;
		}
		if (!pressed) return;
		if (keycode == 'X' || keycode == 'x') {
			MinigameExitPacket().dispatch();
			deactivate();
			return;
		}
		if (keycode == 'B' || keycode == 'b') {
			if (selected_slot >= 0 && static_cast<size_t>(selected_slot) < merch_items.size()) {
				int8_t slot = selected_slot;
				UI::get().emplace<UISoftKey>([slot](const std::string& s) {
					int qty = 1;
					if (!s.empty()) {
						try { qty = std::stoi(s); } catch (...) { return; }
					}
					if (qty > 0) {
						HiredMerchantBuyPacket(slot, static_cast<int16_t>(qty)).dispatch();
						LOG(LOG_DEBUG, "UIMerchant: SoftKey Buy slot=" << static_cast<int>(slot) << " qty=" << qty);
					}
				}, []() {}, "Buy qty (selected)");
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					int slot = 0, qty = 1;
					size_t c = s.find(',');
					try {
						if (c == std::string::npos) slot = std::stoi(s);
						else {
							slot = std::stoi(s.substr(0, c));
							qty = std::stoi(s.substr(c + 1));
						}
					} catch (...) { return; }
					if (slot >= 0 && qty > 0) {
						selected_slot = static_cast<int8_t>(slot);
						HiredMerchantBuyPacket(static_cast<int8_t>(slot), static_cast<int16_t>(qty)).dispatch();
						LOG(LOG_DEBUG, "UIMerchant: SoftKey Buy slot=" << slot << " qty=" << qty);
					}
				}, []() {}, "slot[,qty]");
			}
		} else if (keycode == 'A' || keycode == 'a') {
			// SoftKey：背包选中格时 qty[,merchSlot]；否则 iv,pos,qty,slot
			int16_t inv_slot = 0;
			int8_t inv_type = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				inv_slot = inv->get_selected_slot();
				if (inv_slot > 0)
					inv_type = static_cast<int8_t>(inv->get_tab());
			}
			if (inv_slot > 0 && inv_type > 0) {
				int8_t iv = inv_type;
				int16_t pos = inv_slot;
				UI::get().emplace<UISoftKey>([iv, pos](const std::string& s) {
					int16_t qty = 1;
					int8_t mslot = 0;
					if (!s.empty()) {
						size_t c = s.find(',');
						try {
							qty = static_cast<int16_t>(std::stoi(c == std::string::npos ? s : s.substr(0, c)));
							if (c != std::string::npos)
								mslot = static_cast<int8_t>(std::stoi(s.substr(c + 1)));
						} catch (...) { return; }
					}
					if (qty < 1) return;
					HiredMerchantSetItemPacket(iv, pos, qty, mslot).dispatch();
					LOG(LOG_DEBUG, "UIMerchant: SetItem from inv iv=" << (int)iv
						<< " pos=" << pos << " qty=" << qty);
				}, []() {}, "qty[,merchSlot]");
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& spec) {
					int vals[4] = { 2, 1, 1, 0 };
					size_t start = 0; int vi = 0;
					while (vi < 4 && start <= spec.size()) {
						size_t c = spec.find(',', start);
						std::string part = (c == std::string::npos) ? spec.substr(start) : spec.substr(start, c - start);
						try { vals[vi] = std::stoi(part); } catch (...) {}
						++vi;
						if (c == std::string::npos) break;
						start = c + 1;
					}
					HiredMerchantSetItemPacket(
						static_cast<int8_t>(vals[0]), static_cast<int16_t>(vals[1]),
						static_cast<int16_t>(vals[2]), static_cast<int8_t>(vals[3])
					).dispatch();
					LOG(LOG_DEBUG, "UIMerchant: SoftKey SetItem " << spec);
				}, []() {}, "iv,pos,qty,slot");
			}
		} else if (keycode == 'T' || keycode == 't') {
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				set_owner(pre);
				LOG(LOG_DEBUG, "UIMerchant: owner from CharInfo len=" << pre.size());
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& tip) {
					if (tip.empty()) return;
					set_owner(tip);
					LOG(LOG_DEBUG, "UIMerchant: SoftKey owner len=" << tip.size());
				}, []() {}, "Owner name");
			}
		} else if (keycode == 'O' || keycode == 'o') {
			HiredMerchantOpenPacket().dispatch();
			LOG(LOG_DEBUG, "UIMerchant: SoftKey Open");
		}
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
