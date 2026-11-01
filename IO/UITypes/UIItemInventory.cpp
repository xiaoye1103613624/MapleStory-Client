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
#include "UIItemInventory.h"

#include "UIEquipInventory.h"
#include "UINotice.h"
#include "UIViciousHammer.h"
#include "UIVegaSpell.h"
#include "UIMiscShells.h"
#include "UIMegaphone.h"
#include "UICashGachapon.h"
#include "UIRemoteGachapon.h"
#include "UIMaker.h"
#include "UIPlant.h"
#include "UISoftKey.h"
#include "../../Net/Packets/PlayerInteractionPackets.h"

#include "../UI.h"
#include "UIStorage.h"
#include "UITrade.h"
#include "../../Net/Packets/CashItemPackets.h"
#include "../../Net/Packets/MakerPackets.h"
#include "../../Net/Packets/SolomonPackets.h"
#include "../../Net/Packets/StoragePackets.h"
#include "../../Net/Packets/TradePackets.h"
#include "../UIStateCashShop.h"

#include "../Components/MapleButton.h"
#include "../Components/TwoSpriteButton.h"
#include "../UITypes/UIKeyConfig.h"

#include "../../Data/EquipData.h"
#include "../../Gameplay/Stage.h"

#include "../../Net/Packets/GameplayPackets.h"
#include "../../Net/Packets/InventoryPackets.h"
#include "../../Net/Packets/PlayerPackets.h"
#include "../../Configuration.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		nl::node basic_close_btn()
		{
			// 见 UiAssetResolve::close_btn() —— 本包 BtClose 是实心方块，
			// 原生标题栏用的是描边 X（BtClose2）。
			return UiAssetResolve::close_btn();
		}

		// v83 Item/Tab has exactly indices 0..4 (装备/消耗/设置/其它/特殊). The baked labels
		// fix the index->category order; DEC has no tab and index "5" resolves to nothing.
		const char* const tab_keys[5] = { "0", "1", "2", "3", "4" };

		InventoryType::Id tab_type_at(int i)
		{
			switch (i)
			{
				case 0: return InventoryType::Id::EQUIP;
				case 1: return InventoryType::Id::USE;
				case 2: return InventoryType::Id::SETUP;
				case 3: return InventoryType::Id::ETC;
				case 4: return InventoryType::Id::CASH;
			}
			return InventoryType::Id::NONE;
		}

		int tab_index_of(InventoryType::Id t)
		{
			for (int i = 0; i < 5; i++)
				if (tab_type_at(i) == t)
					return i;

			return -1;
		}

		// Plate strip measured off 背包栏（折叠）.png: 5 contiguous 34px New/Tab plates,
		// dark separators at x=6/39/73/107/141/175 -> left edge 5 + i*34, top y=24.
		Point<int16_t> tab_plate_pos(int i)
		{
			return Point<int16_t>(static_cast<int16_t>(5 + i * 34), 24);
		}

		Button* find_btn(std::map<uint16_t, std::unique_ptr<Button>>& buttons, uint16_t id)
		{
			auto it = buttons.find(id);
			return (it != buttons.end()) ? it->second.get() : nullptr;
		}
	}

	// sort_enabled 初值必须是 false：`set_sort(false)` 走的是「显示 BT_GATHER」那一支，
	// 与切 tab 时的复位（:666 `set_sort(false)`）和两张参考截图一致；旧初值 true
	// 会让窗口刚打开时显示 BtSort（三条横杠），与原生不符。
	UIItemInventory::UIItemInventory(const Inventory& invent) : UIDragElement<PosINV>(), inventory(invent), ignore_tooltip(false), tab(InventoryType::Id::EQUIP), sort_enabled(false)
	{
		nl::node Item = UiAssetResolve::window("Item");

		if (!UiAssetResolve::usable(Item))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		nl::node pos = Item["pos"];

		if (pos)
		{
			slot_col = pos["slot_col"];
			slot_pos = pos["slot_pos"];
			slot_row = pos["slot_row"];
			slot_space_x = pos["slot_space_x"];
			slot_space_y = pos["slot_space_y"];
		}
		else
		{
			// v83 BeiDou Item：无 pos 节点，按 backgrnd（175x289）槽位格实测
			// （r05-ui-layout.md §4）：列 x=8/45/81/117（步进 36）、
			// 行 y=51/85/119/153/187/221（步进 34）、4 列 6 行、格 32px。
			// 旧回退值行步进取 36（space_y=4），与背景格错位累计最多 10px，
			// 导致道具图标越靠下越偏离格线。
			slot_col = 4;
			slot_row = 6;
			slot_space_x = 4;
			slot_space_y = 2;
			slot_pos = Point<int16_t>(8, 51);
		}

		max_slots = slot_row * slot_col;
		max_full_slots = slot_col * max_slots;

		nl::node AutoBuild = Item["AutoBuild"];
		nl::node FullAutoBuild = Item["FullAutoBuild"];

		backgrnd = Item["productionBackgrnd"] ? Item["productionBackgrnd"] : Item["backgrnd"];
		backgrnd2 = Item["productionBackgrnd2"] ? Item["productionBackgrnd2"] : Item["backgrnd2"];
		backgrnd3 = Item["productionBackgrnd3"] ? Item["productionBackgrnd3"] : Item["backgrnd3"];

		full_backgrnd = Item["FullBackgrnd"];
		full_backgrnd2 = Item["FullBackgrnd2"];
		full_backgrnd3 = Item["FullBackgrnd3"];

		bg_dimensions = backgrnd.get_dimensions();
		bg_full_dimensions = full_backgrnd.is_valid() ? full_backgrnd.get_dimensions() : bg_dimensions;

		nl::node New = Item["New"];
		newitemslot = New["inventory"];
		newitemtabdis = New["Tab0"];
		newitemtaben = New["Tab1"];

		projectile = Item["activeIcon"];
		disabled = Item["disabled"];

		Point<int16_t> icon_dimensions = disabled.is_valid() ? disabled.get_dimensions() : Point<int16_t>(32, 32);
		icon_width = icon_dimensions.x() ? icon_dimensions.x() : 32;
		icon_height = icon_dimensions.y() ? icon_dimensions.y() : 32;

		nl::node Tab = Item["Tab"];
		nl::node taben = Tab["enabled"];
		nl::node tabdis = Tab["disabled"];
		nl::node tabplate_en = New["Tab1"];   // pink plate = selected
		nl::node tabplate_dis = New["Tab0"];  // gray plate = unselected

		const bool classic_item = !static_cast<bool>(pos) && UiAssetResolve::is_classic_hud();

		// The button carries the coloured plate (whole 34px tab is clickable and hover/press
		// gives the pink feedback); draw() overlays the matching text sprite. Baked labels fix
		// the index->category order (0 EQUIP, 1 USE, 2 SETUP, 3 ETC, 4 CASH).
		auto add_tab = [&](uint16_t id, int tab_index)
		{
			const char* key = tab_keys[tab_index];

			if (!tabplate_dis[key] || !tabplate_en[key])
				return;

			buttons[id] = std::make_unique<TwoSpriteButton>(
				tabplate_dis[key], tabplate_en[key], tab_plate_pos(tab_index));

			if (tabdis[key])
				tabtext_dis[tab_index] = Texture(tabdis[key]);
			if (taben[key])
				tabtext_en[tab_index] = Texture(taben[key]);
		};

		add_tab(Buttons::BT_TAB_EQUIP, 0);
		add_tab(Buttons::BT_TAB_USE, 1);
		add_tab(Buttons::BT_TAB_SETUP, 2);
		add_tab(Buttons::BT_TAB_ETC, 3);
		add_tab(Buttons::BT_TAB_CASH, 4);

		if (nl::node close = basic_close_btn())
			try_maple_btn(buttons, Buttons::BT_CLOSE, close, Point<int16_t>(bg_dimensions.x() - 15, 3));

		if (AutoBuild)
		{
			try_maple_btn(buttons, Buttons::BT_COIN, AutoBuild["button:Coin"]);
			try_maple_btn(buttons, Buttons::BT_POINT, AutoBuild["button:Point"]);
			try_maple_btn(buttons, Buttons::BT_GATHER, AutoBuild["button:Gather"]);
			try_maple_btn(buttons, Buttons::BT_SORT, AutoBuild["button:Sort"]);
			try_maple_btn(buttons, Buttons::BT_FULL, AutoBuild["button:Full"]);
			try_maple_btn(buttons, Buttons::BT_UPGRADE, AutoBuild["button:Upgrade"]);
			try_maple_btn(buttons, Buttons::BT_APPRAISE, AutoBuild["button:Appraise"]);
			try_maple_btn(buttons, Buttons::BT_EXTRACT, AutoBuild["button:Extract"]);
			try_maple_btn(buttons, Buttons::BT_DISASSEMBLE, AutoBuild["button:Disassemble"]);
			try_maple_btn(buttons, Buttons::BT_TOAD, AutoBuild["anibutton:Toad"]);
		}
		else if (classic_item)
		{
			// Top-right tool row is right-aligned to the frame width and repositioned by
			// set_full on expand. BtFull/BtSmall share the toggle slot (only one active per
			// state); BtSort/BtGather share the slot behind it (set_sort picks which shows).
			// BtCashshop ("扩充道具栏") belongs to the EXPANDED frame's bottom-right -- the
			// old (80,285) sat below the 289px collapsed frame ("button outside the window").
			const int16_t W = bg_dimensions.x();
			try_maple_btn(buttons, Buttons::BT_COIN, Item["BtCoin"], Point<int16_t>(7, 266));
			try_maple_btn(buttons, Buttons::BT_FULL, Item["BtFull"], Point<int16_t>(W - 51, 4));
			try_maple_btn(buttons, Buttons::BT_SMALL, Item["BtSmall"], Point<int16_t>(W - 51, 4));
			try_maple_btn(buttons, Buttons::BT_GATHER, Item["BtGather"], Point<int16_t>(W - 31, 4));
			try_maple_btn(buttons, Buttons::BT_SORT, Item["BtSort"], Point<int16_t>(W - 31, 4));
			try_maple_btn(buttons, Buttons::BT_CASHSHOP, Item["BtCashshop"], Point<int16_t>(bg_full_dimensions.x() - 113, 266));
		}
		else
		{
			try_maple_btn(buttons, Buttons::BT_COIN, Item["BtCoin"]);
			try_maple_btn(buttons, Buttons::BT_GATHER, Item["BtGather"]);
			try_maple_btn(buttons, Buttons::BT_SORT, Item["BtSort"]);
			try_maple_btn(buttons, Buttons::BT_FULL, Item["BtFull"]);
			try_maple_btn(buttons, Buttons::BT_SMALL, Item["BtSmall"]);
			try_maple_btn(buttons, Buttons::BT_CASHSHOP, Item["BtCashshop"]);
		}

		if (FullAutoBuild)
		{
			try_maple_btn(buttons, Buttons::BT_SMALL, FullAutoBuild["button:Small"]);
			try_maple_btn(buttons, Buttons::BT_COIN_SM, FullAutoBuild["button:Coin"]);
			try_maple_btn(buttons, Buttons::BT_POINT_SM, FullAutoBuild["button:Point"]);
			try_maple_btn(buttons, Buttons::BT_GATHER_SM, FullAutoBuild["button:Gather"]);
			try_maple_btn(buttons, Buttons::BT_SORT_SM, FullAutoBuild["button:Sort"]);
			try_maple_btn(buttons, Buttons::BT_UPGRADE_SM, FullAutoBuild["button:Upgrade"]);
			try_maple_btn(buttons, Buttons::BT_APPRAISE_SM, FullAutoBuild["button:Appraise"]);
			try_maple_btn(buttons, Buttons::BT_EXTRACT_SM, FullAutoBuild["button:Extract"]);
			try_maple_btn(buttons, Buttons::BT_DISASSEMBLE_SM, FullAutoBuild["button:Disassemble"]);
			try_maple_btn(buttons, Buttons::BT_TOAD_SM, FullAutoBuild["anibutton:Toad"]);
			try_maple_btn(buttons, Buttons::BT_CASHSHOP, FullAutoBuild["button:Cashshop"]);
		}

		// Extract/Disassemble 已接线 Maker（r227）；不再强制 DISABLED
		if (auto* b = find_btn(buttons, button_by_tab(tab)))
			b->set_state(Button::State::PRESSED);

		mesolabel = Text(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::BLACK);
		maplepointslabel = Text(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::BLACK);
		{
			// 优先 s9 showCash 写入的 maple_point；未进过商城则用本地 Setting
			uint64_t raw = Setting<MaplePointsLocal>::get().load();
			if (UIStateCashShop::cash_balances_ready && UIStateCashShop::cash_maple_point >= 0)
				raw = static_cast<uint64_t>(UIStateCashShop::cash_maple_point);
			std::string pts = std::to_string(raw);
			string_format::split_number(pts);
			maplepointslabel.change_text(pts);
		}

		for (size_t i = 0; i < InventoryType::Id::LENGTH; i++)
		{
			InventoryType::Id id = InventoryType::by_value(i);
			slotrange[id] = std::pair<int16_t, int16_t>(1, 24);
		}

		// Scrollbar strip measured off 背包栏（折叠）.png: 12px VScr at window-x 157, spanning
		// the 6-row slot area (y 46..250). The track is fixed height; only the travel grows
		// with the slot count, so an expanded inventory scrolls instead of the old
		// slotmax-scaled range overflowing the 289px frame.
		int16_t totalrows = inventory.get_slotmax(tab) / slot_col;

		slider = Slider(
			Slider::Type::DEFAULT_SILVER, Range<int16_t>(46, 250), 157, slot_row, totalrows,
			[&](bool upwards)
			{
				int16_t shift = upwards ? -slot_col : slot_col;

				slotrange[tab].first += shift;
				slotrange[tab].second += shift;
			}
		);

#if LOG_LEVEL >= LOG_UI
		for (size_t i = 0; i < max_full_slots; i++)
			slot_labels[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, std::to_string(i + 1));
#endif

		set_full(false);
		clear_new();
		load_icons();
	}

	void UIItemInventory::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		// 金币数值锚点（RIGHT 对齐，两张底图共用同一常量）。
		// 取证方式：把 UIWindow.img/Item/FullBackgrnd(603x289) 与参考截图
		// 背包展开(603x293) 逐行做暗度差分，先用「金币」标签行对齐确认两者
		// 纵向偏移为 0（截图未裁顶），于是截图坐标即窗口坐标。
		// 差分出来的数字墨迹：y 284..288、x 77..141（右边缘 141）。
		// A11M 的字形顶 = draw_y + 6（GraphicsGL.cpp 的 ay = linespace），
		// 所以 draw_y = 284 - 6 = 278，右边界取 141。
		// 上一轮误判成"掉出下边框"把常量从 275 抬到 260，数字整体上浮约 15px
		// —— 就是用户看到的"金币值位置偏了"。
		Point<int16_t> mesolabel_pos = position + Point<int16_t>(141, 278);

		if (full_enabled)
		{
			full_backgrnd.draw(position);
			full_backgrnd2.draw(position);
			full_backgrnd3.draw(position);

			mesolabel.draw(mesolabel_pos);
		}
		else
		{
			backgrnd.draw(position);
			backgrnd2.draw(position);
			backgrnd3.draw(position);

			slider.draw(position);

			mesolabel.draw(mesolabel_pos);
		}

		auto& range = slotrange.at(tab);

		size_t numslots = inventory.get_slotmax(tab);
		size_t firstslot = full_enabled ? 1 : range.first;
		size_t lastslot = full_enabled ? max_full_slots : range.second;

		for (size_t i = 0; i <= max_full_slots; i++)
		{
			Point<int16_t> slotpos = get_slotpos(i);

			if (icons.find(i) != icons.end())
			{
				auto& icon = icons.at(i);

				if (icon && i >= firstslot && i <= lastslot)
					icon->draw(position + slotpos);
			}
			else
			{
				if (i > numslots && i <= lastslot)
					disabled.draw(position + slotpos);
			}

#if LOG_LEVEL >= LOG_UI
			if (i <= lastslot && i < max_full_slots)
				slot_labels[i].draw(position + get_slotpos(i + 1) - Point<int16_t>(0, 5));
#endif
		}

		int16_t bulletslot = inventory.get_bulletslot();

		if (tab == InventoryType::Id::USE && is_visible(bulletslot))
			projectile.draw(position + get_slotpos(bulletslot));

		UIElement::draw_buttons(alpha);

		if (newtab != InventoryType::Id::NONE)
		{
			if (newtab == tab)
			{
				newitemtaben.draw(position + get_tabpos(newtab), alpha);

				if (is_visible(newslot))
					newitemslot.draw(position + get_slotpos(newslot) + Point<int16_t>(1, 1), alpha);
			}
			else
			{
				newitemtabdis.draw(position + get_tabpos(newtab), alpha);
			}
		}

		// Tab labels (装备/消耗/设置/其它/特殊) are text-only sprites; draw them on top of the
		// plate buttons. Selected tab -> enabled(white) text on the pink plate, others ->
		// disabled(gray) text on the gray plate.
		for (int i = 0; i < 5; i++)
		{
			bool selected = (tab == tab_type_at(i));
			Point<int16_t> text_pos = position + tab_plate_pos(i) + Point<int16_t>(3, 1);

			(selected ? tabtext_en[i] : tabtext_dis[i]).draw(text_pos);
		}
	}

	void UIItemInventory::update()
	{
		UIElement::update();

		newitemtaben.update(6);
		newitemtabdis.update(6);
		newitemslot.update(6);

		std::string meso_str = std::to_string(inventory.get_meso());
		string_format::split_number(meso_str);

		mesolabel.change_text(meso_str);
	}

	void UIItemInventory::update_slot(int16_t slot)
	{
		if (int32_t item_id = inventory.get_item_id(tab, slot))
		{
			int16_t count;

			if (tab == InventoryType::Id::EQUIP)
				count = -1;
			else
				count = inventory.get_item_count(tab, slot);

			const bool untradable = ItemData::get(item_id).is_untradable();
			const bool cashitem = ItemData::get(item_id).is_cashitem();
			const Texture& texture = ItemData::get(item_id).get_icon(false);
			EquipSlot::Id eqslot = inventory.find_equipslot(item_id);

			icons[slot] = std::make_unique<Icon>(
				std::make_unique<ItemIcon>(*this, tab, eqslot, slot, item_id, count, untradable, cashitem),
				texture, count
			);
		}
		else if (icons.count(slot))
		{
			icons.erase(slot);
		}
	}

	void UIItemInventory::load_icons()
	{
		icons.clear();

		uint8_t numslots = inventory.get_slotmax(tab);

		for (size_t i = 0; i <= max_full_slots; i++)
			if (i <= numslots)
				update_slot(i);
	}

	Button::State UIItemInventory::button_pressed(uint16_t buttonid)
	{
		InventoryType::Id oldtab = tab;

		switch (buttonid)
		{
			case Buttons::BT_CLOSE:
			{
				toggle_active();

				return Button::State::NORMAL;
			}
			case Buttons::BT_TAB_EQUIP:
			{
				tab = InventoryType::Id::EQUIP;
				break;
			}
			case Buttons::BT_TAB_USE:
			{
				tab = InventoryType::Id::USE;
				break;
			}
			case Buttons::BT_TAB_SETUP:
			{
				tab = InventoryType::Id::SETUP;
				break;
			}
			case Buttons::BT_TAB_ETC:
			{
				tab = InventoryType::Id::ETC;
				break;
			}
			case Buttons::BT_TAB_CASH:
			{
				tab = InventoryType::Id::CASH;
				break;
			}
			case Buttons::BT_TAB_DEC:
			{
				tab = InventoryType::Id::DEC;
				break;
			}
			case Buttons::BT_GATHER:
			case Buttons::BT_GATHER_SM:
			{
				GatherItemsPacket(tab).dispatch();
				break;
			}
			case Buttons::BT_SORT:
			case Buttons::BT_SORT_SM:
			{
				SortItemsPacket(tab).dispatch();
				break;
			}
			case Buttons::BT_FULL:
			{
				set_full(true);

				return Button::State::NORMAL;
			}
			case Buttons::BT_SMALL:
			{
				set_full(false);

				return Button::State::NORMAL;
			}
			case Buttons::BT_COIN:
			case Buttons::BT_COIN_SM:
			{
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t amt = std::stoi(s);
						if (amt > 0) {
							MesoDropPacket(amt).dispatch();
							LOG(LOG_DEBUG, "UIItemInventory: MesoDrop amt=" << amt);
						}
					} catch (...) {}
				}, []() {}, "Drop meso");
				return Button::State::NORMAL;
			}
			case Buttons::BT_EXTRACT:
			case Buttons::BT_EXTRACT_SM:
			{
				auto maker = UI::get().get_element<UIMaker>();
				if (!maker)
					maker = UI::get().emplace<UIMaker>();
				else if (!maker->is_active())
					maker->makeactive();
				LOG(LOG_DEBUG, "UIItemInventory: Extract → UIMaker");
				return Button::State::NORMAL;
			}
			case Buttons::BT_DISASSEMBLE:
			case Buttons::BT_DISASSEMBLE_SM:
			{
				UI::get().emplace<UISoftKey>([](const std::string& tip) {
					try {
						int32_t id = std::stoi(tip);
						if (id > 0) {
							MakerCreatePacket(id).dispatch();
							LOG(LOG_DEBUG, "UIItemInventory: Disassemble SoftKey id=" << id);
						}
					} catch (...) {}
				}, []() {}, "Maker itemId");
				return Button::State::NORMAL;
			}
			case Buttons::BT_POINT:
			case Buttons::BT_POINT_SM:
			{
				// SoftKey 数字 → 本地刷新点券标签；空/非数字 → 进商城
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					if (s.empty()) {
						EnterCashShopPacket().dispatch();
						LOG(LOG_DEBUG, "UIItemInventory: Point → EnterCashShop");
						return;
					}
					try {
						long long pts = std::stoll(s);
						if (pts < 0) pts = 0;
						Setting<MaplePointsLocal>::get().save(static_cast<uint64_t>(pts));
						std::string t = std::to_string(pts);
						string_format::split_number(t);
						maplepointslabel.change_text(t);
						LOG(LOG_DEBUG, "UIItemInventory: maplepoints label=" << pts);
					} catch (...) {
						EnterCashShopPacket().dispatch();
					}
				}, []() {}, "MaplePoints|empty=CS");
				return Button::State::NORMAL;
			}
			case Buttons::BT_UPGRADE:
			case Buttons::BT_UPGRADE_SM:
			{
				auto ask_equip = [](int16_t scroll) {
					if (auto equi = UI::get().get_element<UIEquipInventory>()) {
						EquipSlot::Id target = equi->get_selected_eqslot();
						if (target != EquipSlot::Id::NONE) {
							ScrollEquipPacket(scroll, target).dispatch();
							LOG(LOG_DEBUG, "UIItemInventory: Upgrade scroll=" << scroll
								<< " eq=" << static_cast<int>(target));
							return;
						}
					}
					UI::get().emplace<UISoftKey>([scroll](const std::string& dst) {
						try {
							int eq = std::stoi(dst);
							if (eq <= 0) return;
							EquipSlot::Id target = EquipSlot::by_id(static_cast<size_t>(eq));
							if (target == EquipSlot::Id::NONE) return;
							ScrollEquipPacket(scroll, target).dispatch();
							LOG(LOG_DEBUG, "UIItemInventory: Upgrade scroll=" << scroll << " eq=" << eq);
						} catch (...) {}
					}, []() {}, "Equip slot");
				};
				int16_t from_sel = 0;
				if (tab == InventoryType::Id::USE && selected_slot > 0)
					from_sel = selected_slot;
				if (from_sel > 0) {
					ask_equip(from_sel);
				} else {
					UI::get().emplace<UISoftKey>([ask_equip](const std::string& src) {
						try {
							int16_t scroll = static_cast<int16_t>(std::stoi(src));
							if (scroll <= 0) return;
							ask_equip(scroll);
						} catch (...) {}
					}, []() {}, "Scroll slot");
				}
				return Button::State::NORMAL;
			}
			case Buttons::BT_APPRAISE:
			case Buttons::BT_APPRAISE_SM:
			{
				auto ui = UI::get().get_element<UIVegaSpell>();
				if (!ui) ui = UI::get().emplace<UIVegaSpell>();
				else if (!ui->is_active()) ui->makeactive();
				LOG(LOG_DEBUG, "UIItemInventory: Appraise → UIVegaSpell");
				return Button::State::NORMAL;
			}
			case Buttons::BT_TOAD:
			case Buttons::BT_TOAD_SM:
			{
				auto ui = UI::get().get_element<UIPlant>();
				if (!ui) ui = UI::get().emplace<UIPlant>();
				else if (!ui->is_active()) ui->makeactive();
				LOG(LOG_DEBUG, "UIItemInventory: Toad → UIPlant");
				return Button::State::NORMAL;
			}
			case Buttons::BT_CASHSHOP:
			{
				EnterCashShopPacket().dispatch();
				LOG(LOG_DEBUG, "UIItemInventory: EnterCashShop");
				return Button::State::NORMAL;
			}
		}

		if (tab != oldtab)
		{
			uint16_t row = slotrange.at(tab).first / slot_col;
			slider.setrows(row, slot_row, inventory.get_slotmax(tab) / slot_col);

			if (auto* b = find_btn(buttons, button_by_tab(oldtab)))
				b->set_state(Button::State::NORMAL);
			if (auto* b = find_btn(buttons, button_by_tab(tab)))
				b->set_state(Button::State::PRESSED);

			load_icons();
			set_sort(false);
		}

		return Button::State::IDENTITY;
	}

	void UIItemInventory::doubleclick(Point<int16_t> cursorpos)
	{
		int16_t slot = slot_by_position(cursorpos - position);

		if (icons.count(slot) && is_visible(slot))
		{
			if (int32_t item_id = inventory.get_item_id(tab, slot))
			{
				switch (tab)
				{
					case InventoryType::Id::EQUIP:
					{
						if (can_wear_equip(slot))
						{
							EquipSlot::Id equipslot = inventory.find_equipslot(item_id);

							if (equipslot == EquipSlot::Id::NONE)
							{
								LOG(LOG_DEBUG, "Could not find appropriate EquipSlot::Id for item [" << item_id << "]. Equip would be dropped.");
								break;
							}

							EquipItemPacket(slot, equipslot).dispatch();
						}

						break;
					}
					case InventoryType::Id::USE:
					{
						// 强化卷轴类：先开 EnchantSkill 壳再发包（可拖装备）
						if (item_id / 10000 == 204) {
							auto ui = UI::get().get_element<UIEnchantSkill>();
							if (!ui) ui = UI::get().emplace<UIEnchantSkill>();
							else if (!ui->is_active()) ui->makeactive();
						}
						UseItemPacket(slot, item_id).dispatch();
						break;
					}
					case InventoryType::Id::CASH:
					{
						// 现金道具双击：开对应壳（拖装备由各 UI 自己处理）
						int32_t prefix = item_id / 10000;
						if (prefix == 557) {
							auto ui = UI::get().get_element<UIViciousHammer>();
							if (!ui) ui = UI::get().emplace<UIViciousHammer>();
							else if (!ui->is_active()) ui->makeactive();
						} else if (prefix == 561) {
							auto ui = UI::get().get_element<UIVegaSpell>();
							if (!ui) ui = UI::get().emplace<UIVegaSpell>();
							else if (!ui->is_active()) ui->makeactive();
						} else if (prefix == 559) {
							auto ui = UI::get().get_element<UIItemProtector>();
							if (!ui) ui = UI::get().emplace<UIItemProtector>();
							else if (!ui->is_active()) ui->makeactive();
						} else if (prefix == 552) {
							auto ui = UI::get().get_element<UIKarmaScissors>();
							if (!ui) ui = UI::get().emplace<UIKarmaScissors>();
							else if (!ui->is_active()) ui->makeactive();
						} else if (prefix == 545) {
							// 545* 远程雇佣商人：开 EntrustedShop + OPEN_STORE
							auto ui = UI::get().get_element<UIEntrustedShop>();
							if (!ui) ui = UI::get().emplace<UIEntrustedShop>();
							else if (!ui->is_active()) ui->makeactive();
							HiredMerchantOpenPacket().dispatch();
						} else if (prefix == 506) {
							auto ui = UI::get().get_element<UIIncubator>();
							if (!ui) ui = UI::get().emplace<UIIncubator>();
							else if (!ui->is_active()) ui->makeactive();
						} else if (prefix == 522) {
							auto ui = UI::get().get_element<UICashGachapon>();
							if (!ui) ui = UI::get().emplace<UICashGachapon>();
							else if (!ui->is_active()) ui->makeactive();
						} else if (prefix == 515 || prefix == 519 || prefix == 523 || prefix == 524 || prefix == 525) {
							// 美发/整形/其它简易现金券：SoftKey 确认后 USE_CASH basic
							int16_t cash_slot = slot;
							int32_t cash_id = item_id;
							UI::get().emplace<UISoftKey>([cash_slot, cash_id](const std::string&) {
								UseCashItemBasicPacket(cash_slot, cash_id).dispatch();
							}, []() {}, "Cash coupon");
						} else if (prefix == 543) {
							auto ui = UI::get().get_element<UIRemoteGachapon>();
							if (!ui) ui = UI::get().emplace<UIRemoteGachapon>();
							else if (!ui->is_active()) ui->makeactive();
						} else if (prefix == 505) {
							auto ui = UI::get().get_element<UIReset>();
							if (!ui) ui = UI::get().emplace<UIReset>();
							else if (!ui->is_active()) ui->makeactive();
						} else if (prefix == 517) {
							auto ui = UI::get().get_element<UIUtilDlgExPet>();
							if (!ui) ui = UI::get().emplace<UIUtilDlgExPet>();
							else if (!ui->is_active()) ui->makeactive();
						} else if (prefix == 504) {
							auto ui = UI::get().get_element<UITeleport>();
							if (!ui) ui = UI::get().emplace<UITeleport>();
							else if (!ui->is_active()) ui->makeactive();
						} else if (prefix == 507) {
							int32_t sub = (item_id / 1000) % 10;
							if (sub == 6) {
								auto ui = UI::get().get_element<UIItemMegaphone>();
								if (!ui) ui = UI::get().emplace<UIItemMegaphone>();
								else if (!ui->is_active()) ui->makeactive();
							} else if (sub == 3 || sub == 7) {
								// 5073*/5077* → ArtMegaphone 壳
								auto ui = UI::get().get_element<UIArtMegaphone>();
								if (!ui) ui = UI::get().emplace<UIArtMegaphone>();
								else if (!ui->is_active()) ui->makeactive();
							} else {
								auto ui = UI::get().get_element<UIMegaphoneBasic>();
								if (!ui) ui = UI::get().emplace<UIMegaphoneBasic>();
								else if (!ui->is_active()) ui->makeactive();
							}
						} else if (prefix == 539) {
							auto ui = UI::get().get_element<UIAvatarMegaphone>();
							if (!ui) ui = UI::get().emplace<UIAvatarMegaphone>();
							else if (!ui->is_active()) ui->makeactive();
							int16_t cash_slot = slot;
							int32_t cash_id = item_id;
							UI::get().emplace<UISoftKey>([cash_slot, cash_id](const std::string& text) {
								if (text.empty()) return;
								UseAvatarMegaphonePacket(cash_slot, cash_id, text, false).dispatch();
							}, []() {}, "Avatar Mega");
						} else if (prefix == 537) {
							int16_t cash_slot = slot;
							int32_t cash_id = item_id;
							UI::get().emplace<UISoftKey>([cash_slot, cash_id](const std::string& text) {
								if (text.empty()) return;
								UseChalkboardPacket(cash_slot, cash_id, text).dispatch();
							}, []() {}, "Chalkboard");
						} else if (prefix == 508) {
							int16_t cash_slot = slot;
							int32_t cash_id = item_id;
							UI::get().emplace<UISoftKey>([cash_slot, cash_id](const std::string& text) {
								if (text.empty()) return;
								UseKitePacket(cash_slot, cash_id, text).dispatch();
							}, []() {}, "Kite");
						} else if (prefix == 512) {
							int16_t cash_slot = slot;
							int32_t cash_id = item_id;
							UI::get().emplace<UISoftKey>([cash_slot, cash_id](const std::string& text) {
								if (text.empty()) return;
								UseMapEffectPacket(cash_slot, cash_id, text).dispatch();
							}, []() {}, "Map effect");
						} else {
							// 未映射前缀：SoftKey 确认后简易 USE_CASH（便于实机试未知现金）
							int16_t cash_slot = slot;
							int32_t cash_id = item_id;
							UI::get().emplace<UISoftKey>([cash_slot, cash_id](const std::string&) {
								UseCashItemBasicPacket(cash_slot, cash_id).dispatch();
								LOG(LOG_DEBUG, "UIItemInventory: generic USE_CASH id=" << cash_id);
							}, []() {}, "Cash use");
						}
						break;
					}
					default:
					{
						// 仓库/交易打开时，双击非装备/消耗类物品 = 存入仓库 / 加入交易
						int16_t qty = inventory.get_item_count(tab, slot);
						if (qty > 0)
						{
							if (auto uitrade = UI::get().get_element<UITrade>())
								TradeAddItemPacket(static_cast<uint8_t>(tab), slot, qty, uitrade->next_slot()).dispatch();
							else if (auto uistorage = UI::get().get_element<UIStorage>())
								StorageStorePacket(slot, item_id, qty).dispatch();
						}
						break;
					}
				}
			}
		}
	}

	bool UIItemInventory::send_icon(const Icon& icon, Point<int16_t> cursorpos)
	{
		int16_t slot = slot_by_position(cursorpos - position);

		if (slot > 0)
		{
			int32_t item_id = inventory.get_item_id(tab, slot);
			EquipSlot::Id eqslot;
			bool equip;

			if (item_id && tab == InventoryType::Id::EQUIP)
			{
				eqslot = inventory.find_equipslot(item_id);
				equip = true;
			}
			else
			{
				eqslot = EquipSlot::Id::NONE;
				equip = false;
			}

			ignore_tooltip = true;

			return icon.drop_on_items(tab, eqslot, slot, equip);
		}

		return true;
	}

	Cursor::State UIItemInventory::send_cursor(bool pressed, Point<int16_t> cursorpos)
	{
		Cursor::State dstate = UIDragElement::send_cursor(pressed, cursorpos);

		if (dragged)
		{
			clear_tooltip();

			return dstate;
		}

		Point<int16_t> cursor_relative = cursorpos - position;

		if (!full_enabled && slider.isenabled())
		{
			Cursor::State sstate = slider.send_cursor(cursor_relative, pressed);

			if (sstate != Cursor::State::IDLE)
			{
				clear_tooltip();

				return sstate;
			}
		}

		int16_t slot = slot_by_position(cursor_relative);
		Icon* icon = get_icon(slot);
		bool is_icon = icon && is_visible(slot);

		if (is_icon)
		{
			selected_slot = slot;
			if (pressed)
			{
				Point<int16_t> slotpos = get_slotpos(slot);
				icon->start_drag(cursor_relative - slotpos);

				UI::get().drag_icon(icon);

				clear_tooltip();

				return Cursor::State::GRABBING;
			}
			else if (!ignore_tooltip)
			{
				show_item(slot);

				return Cursor::State::CANGRAB;
			}
			else
			{
				ignore_tooltip = false;

				return Cursor::State::CANGRAB;
			}
		}
		else
		{
			clear_tooltip();

			return UIElement::send_cursor(pressed, cursorpos);
		}
	}

	void UIItemInventory::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				toggle_active();
			}
			else if (keycode == KeyAction::Id::TAB)
			{
				clear_tooltip();

				InventoryType::Id newtab;

				switch (tab)
				{
					case InventoryType::Id::EQUIP:
						newtab = InventoryType::Id::USE;
						break;
					case InventoryType::Id::USE:
						newtab = InventoryType::Id::ETC;
						break;
					case InventoryType::Id::ETC:
						newtab = InventoryType::Id::SETUP;
						break;
					case InventoryType::Id::SETUP:
						newtab = InventoryType::Id::CASH;
						break;
					case InventoryType::Id::CASH:
						newtab = InventoryType::Id::DEC;
						break;
					case InventoryType::Id::DEC:
						newtab = InventoryType::Id::EQUIP;
						break;
				}

				button_pressed(button_by_tab(newtab));
			}
			else if (keycode == 'G' || keycode == 'g')
			{
				GatherItemsPacket(tab).dispatch();
				LOG(LOG_DEBUG, "UIItemInventory: Gather tab=" << static_cast<int>(tab));
			}
			else if (keycode == 'O' || keycode == 'o')
			{
				SortItemsPacket(tab).dispatch();
				LOG(LOG_DEBUG, "UIItemInventory: Sort tab=" << static_cast<int>(tab));
			}
			else if (keycode == 'M' || keycode == 'm')
			{
				button_pressed(Buttons::BT_COIN);
			}
			else if (keycode == 'I' || keycode == 'i')
			{
				auto ui = UI::get().get_element<UIItemMove>();
				if (!ui) ui = UI::get().emplace<UIItemMove>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'U' || keycode == 'u')
			{
				auto use_slot = [this](int16_t slot) {
					int32_t itemid = inventory.get_item_id(tab, slot);
					if (itemid <= 0) return;
					// Solomon 系列（2022xxx）走专用 opcode，而非普通 USE_ITEM
					if (itemid / 1000 == 2022) {
						UseSolomonPacket(slot, itemid).dispatch();
						LOG(LOG_DEBUG, "UIItemInventory: Solomon slot=" << slot << " id=" << itemid);
					} else {
						UseItemPacket(slot, itemid).dispatch();
						LOG(LOG_DEBUG, "UIItemInventory: Use slot=" << slot << " id=" << itemid);
					}
				};
				if (selected_slot > 0 && inventory.get_item_id(tab, selected_slot) > 0) {
					use_slot(selected_slot);
				} else {
					UI::get().emplace<UISoftKey>([this, use_slot](const std::string& s) {
						try {
							int16_t slot = static_cast<int16_t>(std::stoi(s));
							if (slot <= 0) return;
							selected_slot = slot;
							use_slot(slot);
						} catch (...) {}
					}, []() {}, "Use slot");
				}
			}
			else if (keycode == 'G' || keycode == 'g')
			{
				UseGachaExpPacket().dispatch();
				LOG(LOG_DEBUG, "UIItemInventory: UseGachaExp");
			}
			else if (keycode == 'D' || keycode == 'd')
			{
				if (selected_slot > 0 && inventory.get_item_id(tab, selected_slot) > 0) {
					int16_t slot = selected_slot;
					UI::get().emplace<UISoftKey>([this, slot](const std::string& s) {
						int16_t qty = 1;
						if (!s.empty()) {
							try { qty = static_cast<int16_t>(std::stoi(s)); } catch (...) { return; }
						}
						if (qty < 1) return;
						MoveItemPacket(tab, slot, 0, qty).dispatch();
						LOG(LOG_DEBUG, "UIItemInventory: SoftKey drop slot=" << slot << " qty=" << qty);
					}, []() {}, "Drop qty (selected)");
				} else {
					UI::get().emplace<UISoftKey>([this](const std::string& s) {
						try {
							size_t comma = s.find(',');
							int16_t slot = static_cast<int16_t>(std::stoi(comma == std::string::npos ? s : s.substr(0, comma)));
							int16_t qty = 1;
							if (comma != std::string::npos)
								qty = static_cast<int16_t>(std::stoi(s.substr(comma + 1)));
							if (slot <= 0 || qty < 1) return;
							selected_slot = slot;
							MoveItemPacket(tab, slot, 0, qty).dispatch();
							LOG(LOG_DEBUG, "UIItemInventory: SoftKey drop slot=" << slot << " qty=" << qty);
						} catch (...) {}
					}, []() {}, "Drop slot,qty");
				}
			}
			else if (keycode == 'S' || keycode == 's')
			{
				if (selected_slot > 0) {
					int16_t src = selected_slot;
					UI::get().emplace<UISoftKey>([this, src](const std::string& s) {
						try {
							int16_t dst = static_cast<int16_t>(std::stoi(s));
							if (dst <= 0) return;
							MoveItemPacket(tab, src, dst, 1).dispatch();
							LOG(LOG_DEBUG, "UIItemInventory: SoftKey move " << src << "->" << dst);
						} catch (...) {}
					}, []() {}, "Move dst");
				} else {
					UI::get().emplace<UISoftKey>([this](const std::string& s) {
						try {
							size_t comma = s.find(',');
							if (comma == std::string::npos) return;
							int16_t src = static_cast<int16_t>(std::stoi(s.substr(0, comma)));
							int16_t dst = static_cast<int16_t>(std::stoi(s.substr(comma + 1)));
							if (src <= 0 || dst <= 0) return;
							MoveItemPacket(tab, src, dst, 1).dispatch();
							LOG(LOG_DEBUG, "UIItemInventory: SoftKey move " << src << "->" << dst);
						} catch (...) {}
					}, []() {}, "Move src,dst");
				}
			}
			else if (keycode == 'Y' || keycode == 'y')
			{
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t amt = std::stoi(s);
						if (amt > 0) {
							MesoDropPacket(amt).dispatch();
							LOG(LOG_DEBUG, "UIItemInventory: SoftKey meso drop=" << amt);
						}
					} catch (...) {}
				}, []() {}, "Meso amt");
			}
			else if (keycode == 'F' || keycode == 'f')
			{
				auto ui = UI::get().get_element<UIItemSearch>();
				if (!ui) ui = UI::get().emplace<UIItemSearch>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'E' || keycode == 'e')
			{
				if (selected_slot > 0 && tab == InventoryType::Id::EQUIP
					&& inventory.get_item_id(tab, selected_slot) > 0 && can_wear_equip(selected_slot)) {
					int16_t slot = selected_slot;
					int32_t itemid = inventory.get_item_id(tab, slot);
					EquipSlot::Id dest = inventory.find_equipslot(itemid);
					if (dest != EquipSlot::Id::NONE) {
						EquipItemPacket(slot, dest).dispatch();
						LOG(LOG_DEBUG, "UIItemInventory: Equip selected slot=" << slot);
					}
				} else {
					UI::get().emplace<UISoftKey>([this](const std::string& s) {
						try {
							int16_t slot = static_cast<int16_t>(std::stoi(s));
							if (slot <= 0 || tab != InventoryType::Id::EQUIP) return;
							int32_t itemid = inventory.get_item_id(tab, slot);
							if (itemid <= 0 || !can_wear_equip(slot)) return;
							EquipSlot::Id dest = inventory.find_equipslot(itemid);
							if (dest == EquipSlot::Id::NONE) return;
							selected_slot = slot;
							EquipItemPacket(slot, dest).dispatch();
							LOG(LOG_DEBUG, "UIItemInventory: SoftKey equip slot=" << slot);
						} catch (...) {}
					}, []() {}, "Equip slot");
				}
			}
			else if (keycode == 'C' || keycode == 'c')
			{
				// SoftKey：slot,itemid,text → chalk / kite / mapfx（前缀 k, / m,）
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					std::string body = s;
					enum { Chalk, Kite, MapFx } mode = Chalk;
					if (body.size() >= 2 && (body[0] == 'k' || body[0] == 'K') && body[1] == ',') {
						mode = Kite; body = body.substr(2);
					} else if (body.size() >= 2 && (body[0] == 'm' || body[0] == 'M') && body[1] == ',') {
						mode = MapFx; body = body.substr(2);
					}
					size_t c1 = body.find(',');
					if (c1 == std::string::npos) return;
					size_t c2 = body.find(',', c1 + 1);
					if (c2 == std::string::npos) return;
					try {
						int16_t slot = static_cast<int16_t>(std::stoi(body.substr(0, c1)));
						int32_t id = std::stoi(body.substr(c1 + 1, c2 - c1 - 1));
						std::string text = body.substr(c2 + 1);
						if (slot <= 0 || id <= 0) return;
						if (mode == Kite)
							UseKitePacket(slot, id, text).dispatch();
						else if (mode == MapFx)
							UseMapEffectPacket(slot, id, text).dispatch();
						else
							UseChalkboardPacket(slot, id, text).dispatch();
						LOG(LOG_DEBUG, "UIItemInventory: SoftKey cash text mode=" << mode);
					} catch (...) {}
				}, []() {}, "chalk|k,|m, slot,id,text");
			}
		}
	}

	UIElement::Type UIItemInventory::get_type() const
	{
		return TYPE;
	}

	void UIItemInventory::modify(InventoryType::Id type, int16_t slot, int8_t mode, int16_t arg)
	{
		if (slot <= 0)
			return;

		if (type == tab)
		{
			switch (mode)
			{
				case Inventory::Modification::ADD:
				{
					update_slot(slot);

					newtab = type;
					newslot = slot;
					break;
				}
				case Inventory::Modification::CHANGECOUNT:
				case Inventory::Modification::ADDCOUNT:
				{
					if (auto icon = get_icon(slot))
						icon->set_count(arg);

					break;
				}
				case Inventory::Modification::SWAP:
				{
					if (arg != slot)
					{
						update_slot(slot);
						update_slot(arg);
					}

					break;
				}
				case Inventory::Modification::REMOVE:
				{
					update_slot(slot);
					break;
				}
			}
		}

		switch (mode)
		{
			case Inventory::Modification::ADD:
			case Inventory::Modification::ADDCOUNT:
			{
				newtab = type;
				newslot = slot;
				break;
			}
			case Inventory::Modification::CHANGECOUNT:
			case Inventory::Modification::SWAP:
			case Inventory::Modification::REMOVE:
			{
				if (newslot == slot && newtab == type)
					clear_new();

				break;
			}
		}
	}

	void UIItemInventory::set_sort(bool enabled)
	{
		sort_enabled = enabled;

		auto set_act = [&](uint16_t id, bool act)
		{
			if (auto* b = find_btn(buttons, id))
				b->set_active(act);
		};

		// classic 包没有 AutoBuild/FullAutoBuild ⇒ BT_SORT_SM / BT_GATHER_SM 这两颗**根本不会被创建**
		// （见构造函数 :250 的 `if (FullAutoBuild)` 分支）。展开态原来只点亮 _SM 那颗，
		// 于是 W-31 槽位整颗消失；而两张参考（`背包栏（折叠）.png` / `背包栏展开.png`）
		// 中间那颗红色 `+`（= `Item/BtGather`，12×12，已用 nx_inspect 导出比对）在两个态下都在。
		// ⇒ _SM 对缺席时回退到与非展开态共用的同一对按钮。
		const bool sm_pair = find_btn(buttons, Buttons::BT_SORT_SM)
			|| find_btn(buttons, Buttons::BT_GATHER_SM);

		if (full_enabled && sm_pair)
		{
			if (sort_enabled)
			{
				set_act(Buttons::BT_SORT, false);
				set_act(Buttons::BT_SORT_SM, true);
				set_act(Buttons::BT_GATHER, false);
				set_act(Buttons::BT_GATHER_SM, false);
			}
			else
			{
				set_act(Buttons::BT_SORT, false);
				set_act(Buttons::BT_SORT_SM, false);
				set_act(Buttons::BT_GATHER, false);
				set_act(Buttons::BT_GATHER_SM, true);
			}
		}
		else
		{
			if (sort_enabled)
			{
				set_act(Buttons::BT_SORT, true);
				set_act(Buttons::BT_SORT_SM, false);
				set_act(Buttons::BT_GATHER, false);
				set_act(Buttons::BT_GATHER_SM, false);
			}
			else
			{
				set_act(Buttons::BT_SORT, false);
				set_act(Buttons::BT_SORT_SM, false);
				set_act(Buttons::BT_GATHER, true);
				set_act(Buttons::BT_GATHER_SM, false);
			}
		}
	}

	void UIItemInventory::change_tab(InventoryType::Id type)
	{
		button_pressed(button_by_tab(type));
	}

	void UIItemInventory::clear_new()
	{
		newtab = InventoryType::Id::NONE;
		newslot = 0;
	}

	void UIItemInventory::toggle_active()
	{
		UIElement::toggle_active();

		if (!active)
		{
			clear_new();
			clear_tooltip();
		}
	}

	void UIItemInventory::remove_cursor()
	{
		UIDragElement::remove_cursor();

		slider.remove_cursor();
	}

	void UIItemInventory::show_item(int16_t slot)
	{
		if (tab == InventoryType::Id::EQUIP)
		{
			UI::get().show_equip(Tooltip::Parent::ITEMINVENTORY, slot);
		}
		else
		{
			int32_t item_id = inventory.get_item_id(tab, slot);
			UI::get().show_item(Tooltip::Parent::ITEMINVENTORY, item_id);
		}
	}

	void UIItemInventory::clear_tooltip()
	{
		UI::get().clear_tooltip(Tooltip::Parent::ITEMINVENTORY);
	}

	bool UIItemInventory::is_visible(int16_t slot) const
	{
		return !is_not_visible(slot);
	}

	bool UIItemInventory::is_not_visible(int16_t slot) const
	{
		auto& range = slotrange.at(tab);

		if (full_enabled)
			return slot < 1 || slot > max_full_slots;
		else
			return slot < range.first || slot > range.second;
	}

	bool UIItemInventory::can_wear_equip(int16_t slot) const
	{
		const Player& player = Stage::get().get_player();
		const CharStats& stats = player.get_stats();
		const CharLook& look = player.get_look();
		const bool alerted = look.get_alerted();

		if (alerted)
		{
			UI::get().emplace<UIOk>("You cannot complete this action right now.\\nEvade the attack and try again.", [](bool) {});
			return false;
		}

		const int32_t item_id = inventory.get_item_id(InventoryType::Id::EQUIP, slot);
		const EquipData& equipdata = EquipData::get(item_id);
		const ItemData& itemdata = equipdata.get_itemdata();

		const int8_t reqGender = itemdata.get_gender();
		const bool female = stats.get_female();

		switch (reqGender)
		{
			// Male
			case 0:
			{
				if (female)
					return false;

				break;
			}
			// Female
			case 1:
			{
				if (!female)
					return false;

				break;
			}
			// Unisex
			case 2:
			default:
			{
				break;
			}
		}

		const std::string jobname = stats.get_jobname();

		if (jobname == "GM" || jobname == "SuperGM")
			return true;

		// TODO: Remove from EquipTooltip and move into Job?
		bool can_wear = false;

		uint16_t job = stats.get_stat(MapleStat::Id::JOB) / 100;
		int16_t reqJOB = equipdata.get_reqstat(MapleStat::Id::JOB);

		switch (reqJOB)
		{
			case 0: // Common
			{
				can_wear = true;
				break;
			}
			case 1: // Warrior
			{
				if (job == 1 || job >= 20)
					can_wear = true;

				break;
			}
			case 2: // Magician
			{
				if (job == 2)
					can_wear = true;

				break;
			}
			case 3: // Magician, Warrior
			{
				if (job == 1 || job >= 20 || job == 2)
					can_wear = true;

				break;
			}
			case 4: // Bowman
			{
				if (job == 3)
					can_wear = true;

				break;
			}
			case 8: // Thief
			{
				if (job == 4)
					can_wear = true;

				break;
			}
			case 16: // Pirate
			{
				if (job == 5)
					can_wear = true;

				break;
			}
			default:
			{
				can_wear = false;
			}
		}

		if (!can_wear)
		{
			UI::get().emplace<UIOk>("Your current job\\ncannot equip the selected item.", [](bool) {});
			return false;
		}
		// End of TODO

		int16_t reqLevel = equipdata.get_reqstat(MapleStat::Id::LEVEL);
		int16_t reqDEX = equipdata.get_reqstat(MapleStat::Id::DEX);
		int16_t reqSTR = equipdata.get_reqstat(MapleStat::Id::STR);
		int16_t reqLUK = equipdata.get_reqstat(MapleStat::Id::LUK);
		int16_t reqINT = equipdata.get_reqstat(MapleStat::Id::INT);
		int16_t reqFAME = equipdata.get_reqstat(MapleStat::Id::FAME);

		int8_t i = 0;

		if (reqLevel > stats.get_stat(MapleStat::Id::LEVEL))
			i++;
		else if (reqDEX > stats.get_total(EquipStat::Id::DEX))
			i++;
		else if (reqSTR > stats.get_total(EquipStat::Id::STR))
			i++;
		else if (reqLUK > stats.get_total(EquipStat::Id::LUK))
			i++;
		else if (reqINT > stats.get_total(EquipStat::Id::INT))
			i++;
		else if (reqFAME > stats.get_honor())
			i++;

		if (i > 0)
		{
			UI::get().emplace<UIOk>("Your stats are too low to equip this item\\nor you do not meet the job requirement.", [](bool) {});
			return false;
		}

		return true;
	}

	int16_t UIItemInventory::slot_by_position(Point<int16_t> cursorpos) const
	{
		Point<int16_t> cursor_offset = cursorpos - slot_pos;

		int16_t xoff = cursor_offset.x();
		int16_t yoff = cursor_offset.y();

		int16_t cur_x = cursorpos.x();
		int16_t slot_x = slot_pos.x();
		int16_t xmin = slot_x;
		int16_t xmax = (icon_width + slot_space_x) * (full_enabled ? slot_col * 4 : slot_col) - (full_enabled ? slot_space_x : 0);

		int16_t cur_y = cursorpos.y();
		int16_t slot_y = slot_pos.y();
		int16_t ymin = slot_y;
		int16_t ymax = (icon_height + slot_space_y) * (full_enabled ? slot_row + 1 : slot_row - 1) - (full_enabled ? slot_space_y : 0);

		int16_t slot = 0;
		int16_t absslot = full_enabled ? 1 : slotrange.at(tab).first;

		int16_t col = cur_x / (icon_width + slot_space_x);
		int16_t row = cur_y / (icon_height + slot_space_y) - 1;

		div_t div = std::div(col, 4);
		slot = col + absslot + (4 * row) + (div.quot * 28);

		if (cur_x < xmin || cur_x > xmax || cur_y < ymin || cur_y > ymax)
			slot = 0;

		LOG(LOG_UI,
			"Slot: " << slot << " Col: " << col << " Row: " << row << " "
			<< cur_x << " < (" << xmin << ") || "
			<< cur_x << " > (" << xmax << ") || "
			<< cur_y << " < (" << ymin << ") && "
			<< cur_y << " > (" << ymax << ")");

		if (is_visible(slot))
			return slot;

		return 0;
	}

	Point<int16_t> UIItemInventory::get_slotpos(int16_t slot) const
	{
		int16_t absslot = slot - (full_enabled ? 1 : slotrange.at(tab).first);

		int16_t step_x = static_cast<int16_t>(icon_width + slot_space_x);
		int16_t step_y = static_cast<int16_t>(icon_height + slot_space_y);

		if (step_x <= 0)
			step_x = 42;
		if (step_y <= 0)
			step_y = 42;

		if (full_enabled)
		{
			// 展开态（对照正确端截图 背包栏展开.png 602x291 逐像素实测）：
			// 4 页 × 4 列 × 6 行 = 96 槽；页内列距 36，行距 34，首行 y=52。
			// 页 0 列 0 槽位 x=8、页 1 x=158、页 2 x=307、页 3 x=456
			// → 相对 slot_pos.x=8 的页偏移 {0,150,299,448}。
			// 旧实现按 3 页 × 8 行（absslot/32）布局，第 7/8 行溢出面板底边=花屏。
			constexpr int16_t PAGE_COLS = 4;
			constexpr int16_t PAGE_ROWS = 6;
			static const int16_t page_dx[4] = { 0, 150, 299, 448 };

			int16_t page = static_cast<int16_t>(absslot / (PAGE_COLS * PAGE_ROWS));
			int16_t rem = static_cast<int16_t>(absslot % (PAGE_COLS * PAGE_ROWS));
			int16_t row = static_cast<int16_t>(rem / PAGE_COLS);
			int16_t col = static_cast<int16_t>(rem % PAGE_COLS);

			if (page < 0)
				page = 0;
			else if (page > 3)
				page = 3;

			return slot_pos + Point<int16_t>(
				static_cast<int16_t>(col * step_x + page_dx[page]),
				static_cast<int16_t>(row * step_y));
		}

		div_t div4 = std::div(absslot, 4);

		int16_t row = div4.quot;
		int16_t col = div4.rem;

		return slot_pos + Point<int16_t>(static_cast<int16_t>(col * step_x), static_cast<int16_t>(row * step_y));
	}

	Point<int16_t> UIItemInventory::get_tabpos(InventoryType::Id tb) const
	{
		int i = tab_index_of(tb);

		if (i < 0)
			i = 0;

		return tab_plate_pos(i);
	}

	uint16_t UIItemInventory::button_by_tab(InventoryType::Id tb) const
	{
		switch (tb)
		{
			case InventoryType::Id::EQUIP:
				return Buttons::BT_TAB_EQUIP;
			case InventoryType::Id::USE:
				return Buttons::BT_TAB_USE;
			case InventoryType::Id::SETUP:
				return Buttons::BT_TAB_SETUP;
			case InventoryType::Id::ETC:
				return Buttons::BT_TAB_ETC;
			case InventoryType::Id::CASH:
				return Buttons::BT_TAB_CASH;
			default:
				return Buttons::BT_TAB_DEC;
		}
	}

	Icon* UIItemInventory::get_icon(int16_t slot)
	{
		auto iter = icons.find(slot);

		if (iter != icons.end())
			return iter->second.get();
		else
			return nullptr;
	}

	void UIItemInventory::set_full(bool enabled)
	{
		full_enabled = enabled;

		auto set_act = [&](uint16_t id, bool act)
		{
			if (auto* b = find_btn(buttons, id))
				b->set_active(act);
		};

		// Place a sprite by its top-left (buttons store the anchor, so add the current-state
		// origin back).
		auto place = [&](uint16_t id, Point<int16_t> topleft)
		{
			if (auto* b = find_btn(buttons, id))
				b->set_position(topleft + b->origin());
		};

		dimension = full_enabled ? bg_full_dimensions : bg_dimensions;

		const int16_t W = dimension.x();

		// Right-aligned top row: [toggle] [sort/gather] [close]. The toggle and the sort/gather
		// pair each occupy one slot (only the active button of each pair shows), so BtFull and
		// BtSmall no longer collide at a fixed (109,6); both track the frame width and stay
		// inside it in either state.
		place(Buttons::BT_CLOSE, Point<int16_t>(W - 15, 3));
		place(Buttons::BT_GATHER, Point<int16_t>(W - 31, 4));
		place(Buttons::BT_SORT, Point<int16_t>(W - 31, 4));
		place(Buttons::BT_FULL, Point<int16_t>(W - 51, 4));
		place(Buttons::BT_SMALL, Point<int16_t>(W - 51, 4));

		// "扩充道具栏" (BtCashshop) exists only in the expanded frame, bottom-right.
		place(Buttons::BT_CASHSHOP, Point<int16_t>(bg_full_dimensions.x() - 113, 266));

		set_act(Buttons::BT_FULL, !full_enabled);
		set_act(Buttons::BT_SMALL, full_enabled);
		set_act(Buttons::BT_CASHSHOP, full_enabled);
		// Classic layout has a single coin button that serves both states.
		set_act(Buttons::BT_COIN, true);

		// AutoBuild/FullAutoBuild split (non-classic generations) keeps its own toggles.
		set_act(Buttons::BT_POINT, !full_enabled);
		set_act(Buttons::BT_UPGRADE, !full_enabled);
		set_act(Buttons::BT_APPRAISE, !full_enabled);
		set_act(Buttons::BT_EXTRACT, !full_enabled);
		set_act(Buttons::BT_DISASSEMBLE, !full_enabled);
		set_act(Buttons::BT_TOAD, !full_enabled);
		set_act(Buttons::BT_COIN_SM, full_enabled);
		set_act(Buttons::BT_POINT_SM, full_enabled);
		set_act(Buttons::BT_UPGRADE_SM, full_enabled);
		set_act(Buttons::BT_APPRAISE_SM, full_enabled);
		set_act(Buttons::BT_EXTRACT_SM, full_enabled);
		set_act(Buttons::BT_DISASSEMBLE_SM, full_enabled);
		set_act(Buttons::BT_TOAD_SM, full_enabled);

		dragarea = Point<int16_t>(dimension.x(), 20);

		set_sort(sort_enabled);
		load_icons();
	}

	void UIItemInventory::ItemIcon::set_count(int16_t c)
	{
		count = c;
	}

	Icon::IconType UIItemInventory::ItemIcon::get_type()
	{
		return Icon::IconType::ITEM;
	}

	UIItemInventory::ItemIcon::ItemIcon(const UIItemInventory& parent, InventoryType::Id st, EquipSlot::Id eqs, int16_t s, int32_t iid, int16_t c, bool u, bool cash) : parent(parent)
	{
		sourcetab = st;
		eqsource = eqs;
		source = s;
		item_id = iid;
		count = c;
		untradable = u;
		cashitem = cash;
	}

	void UIItemInventory::ItemIcon::drop_on_stage() const
	{
		constexpr const char* dropmessage = "How many will you drop?";
		constexpr const char* untradablemessage = "This item can't be taken back once thrown away.\\nWill you still drop it?";
		constexpr const char* cashmessage = "You can't drop this item.";

		if (cashitem)
		{
			UI::get().emplace<UIOk>(cashmessage, [](bool) {});
		}
		else
		{
			if (untradable)
			{
				auto onok = [&, dropmessage](bool ok)
				{
					if (ok)
					{
						if (count <= 1)
						{
							MoveItemPacket(sourcetab, source, 0, 1).dispatch();
						}
						else
						{
							auto onenter = [&](int32_t qty)
							{
								MoveItemPacket(sourcetab, source, 0, qty).dispatch();
							};

							UI::get().emplace<UIEnterNumber>(dropmessage, onenter, count, count);
						}
					}
				};

				UI::get().emplace<UIYesNo>(untradablemessage, onok);
			}
			else
			{
				if (count <= 1)
				{
					MoveItemPacket(sourcetab, source, 0, 1).dispatch();
				}
				else
				{
					auto onenter = [&](int32_t qty)
					{
						MoveItemPacket(sourcetab, source, 0, qty).dispatch();
					};

					UI::get().emplace<UIEnterNumber>(dropmessage, onenter, count, count);
				}
			}
		}
	}

	void UIItemInventory::ItemIcon::drop_on_equips(EquipSlot::Id eqslot) const
	{
		switch (sourcetab)
		{
			case InventoryType::Id::EQUIP:
			{
				if (eqsource == eqslot)
					if (parent.can_wear_equip(source))
						EquipItemPacket(source, eqslot).dispatch();

				Sound(Sound::Name::DRAGEND).play();
				break;
			}
			case InventoryType::Id::USE:
			{
				ScrollEquipPacket(source, eqslot).dispatch();
				break;
			}
		}
	}

	bool UIItemInventory::ItemIcon::drop_on_items(InventoryType::Id tab, EquipSlot::Id, int16_t slot, bool) const
	{
		if (tab != sourcetab || slot == source)
			return true;

		MoveItemPacket(tab, source, slot, 1).dispatch();

		return true;
	}

	void UIItemInventory::ItemIcon::drop_on_bindings(Point<int16_t> cursorposition, bool remove) const
	{
		if (sourcetab == InventoryType::Id::USE || sourcetab == InventoryType::Id::SETUP)
		{
			auto keyconfig = UI::get().get_element<UIKeyConfig>();
			Keyboard::Mapping mapping = Keyboard::Mapping(KeyType::ITEM, item_id);

			if (remove)
				keyconfig->unstage_mapping(mapping);
			else
				keyconfig->stage_mapping(cursorposition, mapping);
		}
	}
}