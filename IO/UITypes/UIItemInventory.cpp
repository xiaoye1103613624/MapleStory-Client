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
#include "UIEnhance.h"
#include "UIMiscShells.h"
#include "UICharInfo.h"
#include "UIChatBar.h"
#include "UIWedding.h"
#include "../../Character/MarriageInfo.h"
#include "../../Net/Packets/WeddingPackets.h"
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
#include "../../Net/Packets/CashShopPackets.h"
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
#include "../../Net/Packets/PetPackets.h"
#include "../../Net/Packets/ItemAClassPackets.h"
#include "../../Net/Packets/MapInteractPackets.h"
#include "../../Gameplay/MapleMap/Mob.h"
#include "../../Net/Packets/PlayerPackets.h"
#include "../../Configuration.h"
#include "../../Util/Locale.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#include <algorithm>
#include <cstdlib>
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

	// 开窗默认全页签 nState=0 → 显示 BtGather（红底 +）。
	// 证据：Angel 083 CUIItem::OnCreate @ 0x81c6c9 逐字
	//   `memset(this + 378, 0, 0x18u);`（m_anSortBtState[0..5] 全 0）
	//   `CUIItem::SetArrangeButton(this + 377, this + *(this + 377) + 378);`
	// → nState=0 走 SetArrangeButton @ 0x81e3a6 的 BtGather 分支（SP_1852 是 nState=1 的 BtSort）。
	UIItemInventory::UIItemInventory(const Inventory& invent) : UIDragElement<PosINV>(), inventory(invent), ignore_tooltip(false), tab(InventoryType::Id::EQUIP), sort_enabled(false)
	{
		for (int i = 0; i < InventoryType::Id::LENGTH; ++i)
			arrange_bt_state[i] = false;

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

			// 防御性守卫：子节点缺失/非整数时 nlnx get_integer() 返回默认 0
			// （includes/NoLifeNx/nlnx/node.cpp:161-179），slot_col / slot_row 为 0
			// 会让 :310 / :689 / :690 三处整数除法除零、max_slots 归零，构造或切
			// 页签直接崩。整组回退到与下方 else 相同的 v83 BeiDou 位图实测常量；
			// 合法带 pos 的资源包（col>0 且 row>0）逐字保留、不触发本分支。
			if (slot_col <= 0 || slot_row <= 0)
			{
				slot_col = 4;
				slot_row = 6;
				slot_space_x = 4;
				slot_space_y = 2;
				slot_pos = Point<int16_t>(8, 50);
			}
		}
		else
		{
			// v83 BeiDou Item：无 pos 节点，槽位格按位图实测。尺子来自**展开态**原生帧：
			// 把 32x32 的 Item/disabled 在整帧上逐像素求差，命中 72 格（12 列 x 6 行）且
			// 逐格 SAD=0.000，列=窗口相对 8/44/80/116 | 157/193/229/265 | …（页距 149）、
			// 行=50/84/118/152/186/220（行距 34）⇒ 4 列 6 行、格 32px、space_x=4 / space_y=2。
			// 格顶与 (204,204,187) 那条格线**同一行**（覆盖层顶边正压在格线上），所以 y 是 50
			// 不是 51：旧值 (8,51) 是当年从紧裁原生帧的**帧坐标**抄来的，没减锚点 dy=1。
			// 见 evidence/r27-oracle/ItemFull.json 的 note 与 notes/open-issues.md B15。
			slot_col = 4;
			slot_row = 6;
			slot_space_x = 4;
			slot_space_y = 2;
			slot_pos = Point<int16_t>(8, 50);
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

		// 展开态翻页三件套：锚定 BtCashshop（"扩充道具栏"）左侧，位置固定、
		// 不受其是否显示影响。素材 Basic.img/BtHide3(左←上一页) / BtHide2(右→下一页)，
		// 与 BtCashshop(93x18 @ y266) 按垂直中线水平对齐。
		{
			nl::node basic = nl::nx::UI["Basic.img"];
			const int16_t cash_x = bg_full_dimensions.x() - 113;
			const int16_t cash_y = 266;
			const int16_t cash_h = 18;    // Item/BtCashshop normal 实测
			const int16_t arrow_w = 12;   // BtHide2/BtHide3 实测
			const int16_t arrow_h = 12;
			const int16_t gap_edge = 8;   // 与 cashshop 的间距
			const int16_t gap_inner = 3;  // 三件套两两间距
			const int16_t center_y = static_cast<int16_t>(cash_y + cash_h / 2);
			const int16_t arrow_y = static_cast<int16_t>(center_y - arrow_h / 2); // 269
			page_input_dim = Point<int16_t>(24, 16);
			const int16_t input_y = static_cast<int16_t>(center_y - page_input_dim.y() / 2); // 267
			// 从 cashshop 左边缘往左排：下一页 → 输入框 → 上一页，互不重叠。
			const int16_t next_x = cash_x - gap_edge - arrow_w;
			const int16_t input_x = next_x - gap_inner - page_input_dim.x();
			const int16_t prev_x = input_x - gap_inner - arrow_w;
			try_maple_btn(buttons, Buttons::BT_PAGE_PREV, basic["BtHide3"], Point<int16_t>(prev_x, arrow_y));
			try_maple_btn(buttons, Buttons::BT_PAGE_NEXT, basic["BtHide2"], Point<int16_t>(next_x, arrow_y));

			// 页码输入框兼作页码显示：仅数字、默认 1；浅灰底板让黑字可见（Release 也画）。
			page_input_pos = Point<int16_t>(input_x, input_y);
			page_input = Textfield(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK,
				Rectangle<int16_t>(page_input_pos, page_input_pos + page_input_dim), 3);
			page_input.set_digits_only(true);
			page_input.change_text("1");
			// 页码数字垂直居中于 24x16 浅灰框：Textfield 默认 text_shift=(0,0)，
			// FreeType 按行顶/基线绘字，16px 矮槽不补偿就把「1」画到框外下方
			// （用户截图实证）。用 y_for_ink_top 按当前字模现取墨迹度量，不手抄常数。
			page_input.center_text_vertically(page_input_dim.y());
			page_input_frame = ColorBox(page_input_dim.x(), page_input_dim.y(), Color(0xDDDDDDFFu), 1.0f);
			page_input.set_enter_callback([&](std::string s) {
				if (s.empty())
				{
					sync_page_chrome();
					return;
				}
				apply_page(static_cast<int16_t>(std::atoi(s.c_str())));
			});
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
		// R27-B1b 按原生帧重量（evidence/r27-oracle/Item.json 的 meso_* 探针）：
		// 以白框上边框为公共基准 —— Item/backgrnd 边框 y266 / FullBackgrnd y266，
		// 折叠帧量到数字「0」墨迹 y268、展开帧 y270，各自扣掉本帧窗口偏移后
		// **都等于窗口 y269**（右缘同理均为窗口 x137）。⇒ 墨迹顶 269、右缘 137。
		// 上一版 (141, 284) 的 284 是拿 603x293 截图按「暗度差分」读的，差分的其实是
		// 白框下边框（y281）以下那一条，数字被甩到框外 16px —— 就是用户报的
		// 「金币值位置偏下了，没有在框内」。
		const int16_t meso_y = mesolabel.y_for_ink_top(269);
		Point<int16_t> mesolabel_pos = position + Point<int16_t>(137, meso_y);

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
		size_t firstrel = full_enabled ? 1 : range.first;
		size_t lastrel = full_enabled ? max_full_slots : range.second;

		for (size_t rel = firstrel; rel <= lastrel; rel++)
		{
			// 几何用 rel（get_slotpos 只认相对格 1..96），数据用 abs（服务端槽位）。
			size_t abs = full_enabled ? static_cast<size_t>(full_page_offset) + rel : rel;
			Point<int16_t> slotpos = get_slotpos(static_cast<int16_t>(rel));

			auto it = icons.find(static_cast<int16_t>(abs));
			if (it != icons.end())
			{
				auto& icon = it->second;

				if (icon && abs <= numslots)
				{
					icon->set_cell(slotpos);
					icon->draw_at(position);
				}
			}
			else
			{
				if (abs > numslots)
					disabled.draw(position + slotpos);
			}

#if LOG_LEVEL >= LOG_UI
			if (rel < lastrel)
				slot_labels[rel].draw(position + get_slotpos(static_cast<int16_t>(rel) + 1) - Point<int16_t>(0, 5));
#endif
		}

		if (full_enabled)
		{
			page_input_frame.draw(position + page_input_pos);
			page_input.draw(position);
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
			const Texture& texture = ItemData::get(item_id).get_icon(item_id / 10000 == 238);
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

		// 上限×2 后必须加载全部绝对槽 1..numslots（可到 192），
		// 否则翻到第 2 页 (abs 97..192) 无图标可画。
		for (size_t i = 0; i <= numslots; i++)
			update_slot(i);
	}

	Button::State UIItemInventory::button_pressed(uint16_t buttonid)
	{
		InventoryType::Id oldtab = tab;

		switch (buttonid)
		{
			case Buttons::BT_PAGE_PREV:
				change_page(-1);
				return Button::State::NORMAL;
			case Buttons::BT_PAGE_NEXT:
				change_page(1);
				return Button::State::NORMAL;
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
			// 点击不切钮的依据（083/08v83 口径）：
			//   Angel 083 `CUIItem::OnChildNotify` @ 0x81d01f 的 switch 只覆盖
			//   0x7D4(Sort)/0x7D5(Expand)/0x7D6(Collapse)/0x7D7 的**悬停提示文案**，
			//   按钮切换只由 `CWvsContext::OnGatherItemResult` @ 0xa1e943 /
			//   `OnSortItemResult` @ 0xa1e96d 内部 SetArrangeButton(ui, ti, 1/0) 触发。
			//   （旧注释曾引 095 线地址，在 083 IDB 里指向无关函数，已剔除。）
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
				// 083 CUIItem::OnDropMoney @0x81d996 → CUtilDlgEx 数字框 → MESO_DROP(94)。
				// 历史上的 SoftKey 是 softkey 波次临时接线，会弹 PIC 软键盘而非数量输入。
				int32_t meso = static_cast<int32_t>(inventory.get_meso());

				if (meso <= 0)
					return Button::State::NORMAL;

				auto onenter = [](int32_t amt)
				{
					if (amt > 0)
					{
						MesoDropPacket(amt).dispatch();
						LOG(LOG_DEBUG, "UIItemInventory: MesoDrop amt=" << amt);
					}
				};

				UI::get().emplace<UIEnterNumber>(
					Locale::text("inventory.drop_meso"),
					onenter,
					meso,
					meso);
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
				
				return Button::State::NORMAL;
			}
			case Buttons::BT_POINT:
			case Buttons::BT_POINT_SM:
			{
				// SoftKey 数字 → 本地刷新点券标签；空/非数字 → 进商城
				
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
					
				};
				int16_t from_sel = 0;
				if (tab == InventoryType::Id::USE && selected_slot > 0)
					from_sel = selected_slot;
				if (from_sel > 0) {
					ask_equip(from_sel);
				} else {
					
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

			apply_page(1);   // 切页签页码归 1（U3），整理态保持不变
			load_icons();
			// 原生 OnTabChanged @ 0x7CF040：SetArrangeButton(m_nItemTI, m_anSortBtState[m_nItemTI])
			// —— 恢复该页签上次状态，而不是一律复位 Gather。
			set_arrange_button(tab, arrange_bt_state[tab]);
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
				// 面交窗打开时，双击任意背包道具 = 加入交易。必须放在 switch 之前，
				// 否则 EQUIP/USE/CASH 会走道具自身用途（魔方重洗 / 使用 / 现金效果）。
				if (auto uitrade = UI::get().get_element<UITrade>())
				{
					if (uitrade->is_active())
					{
						uint8_t trade_slot = uitrade->next_slot();
						if (trade_slot > 0)
						{
							int16_t qty = inventory.get_item_count(tab, slot);
							if (qty < 1)
								qty = 1;
							TradeAddItemPacket(static_cast<uint8_t>(tab), slot, qty, trade_slot).dispatch();
						}
						return;
					}
				}

				switch (tab)
				{
					case InventoryType::Id::EQUIP:
					{
						// 原生双击 = 穿戴（can_wear_equip 内含性别/职业/等级/属性
						// 校验，不满足时弹 UIOk 提示；这是 item6「双击不可穿戴装备
						// 无提示」的正解）。095 魔方重洗入口收回到 CASH 页签的
						// 243* 魔方道具双击（见下面 CASH 分支），不再劫持装备双击。
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
						// 订婚戒指盒（2240000..2240015，s9 ENGAGEMENT_BOX_MIN/MAX）：
						// 原生双击 = 输入对象名 → RING_ACTION mode0 求婚。
						// 先开婚礼窗（内含对端提示位），再走 SoftKey 取对象名。
						if (WeddingItem::is_proposal_box(item_id)) {
							int32_t box_id = item_id;
							std::string pre;
							if (auto ci = UI::get().get_element<UICharInfo>())
								pre = ci->get_target_name();
							auto ask_target = [box_id](const std::string& name) {
								if (name.empty()) return;
								WeddingProposalPacket(name, box_id).dispatch();
								LOG(LOG_DEBUG, "UIItemInventory: proposal name=" << name << " box=" << box_id);
							};
							if (!pre.empty())
								ask_target(pre);
							else
								;
							// 求婚不消费 USE 道具本体（s9 校验 haveItem 后由 mode2 接受时 removeById）。
							break;
						}
						// 强化卷轴类：原生（Angel OnDoubleClicked @0x4efd25 按 prefix 分发，
						// 204 不在普通 UseItem 之列）双击只开强化壳，由壳内确认再走强化协议。
						// 旧实现此处漏 break，继续落到 :853 UseItemPacket 把卷轴当普通消耗品
						// 发给服务端 → 服务端 back ItemEff → Sound::Sound(204xxxx) 查不到
						// prefix 02040000 回退 02000000（药水音）=> "卷轴用药水音效"真因。
						// 加 break 截断误发；音效资源缺 204 的问题随之不再触发。
						// UI_004 协同机族(16/17)：20484xx 是「2合1道具融合机」，不是砸卷。
						// 必须抢在通用 204 砸卷分支之前精确拦截，否则会被误开 UIEnchantSkill。
						if (item_id / 1000 == 2048 && item_id / 10000 == 204) {
							uint8_t act = (item_id == 2048403) ? 17 : 16;  // 2048403=卷轴协同,其余=协同机
							UI::get().remove(UIElement::Type::ENHANCE);
							UI::get().emplace<UIEnhance>(act);
							LOG(LOG_DEBUG, "[DBG-ENH] open synergy action=" << static_cast<int>(act)
								<< " id=" << item_id << " slot=" << slot);
							break;
						}
						if (item_id / 10000 == 204) {
							auto ui = UI::get().get_element<UIEnchantSkill>();
							if (!ui) ui = UI::get().emplace<UIEnchantSkill>();
							else if (!ui->is_active()) ui->makeactive();
							break;
						}
						// W3：捕捉球 227* → USE_CATCH_ITEM + 最近怪 oid
						if (item_id / 10000 == 227) {
							int32_t mob_oid = 0;
							int best = 400 * 400;
							Point<int16_t> ppos = Stage::get().get_player().get_position();
							if (auto* list = Stage::get().get_mobs().get_mobs()) {
								for (auto& mk : *list) {
									auto* mob = dynamic_cast<Mob*>(mk.second.get());
									if (!mob || !mob->is_active()) continue;
									Point<int16_t> mp = mob->get_position();
									int ddx = mp.x() - ppos.x();
									int ddy = mp.y() - ppos.y();
									int d2 = ddx * ddx + ddy * ddy;
									if (d2 < best) { best = d2; mob_oid = mob->get_oid(); }
								}
							}
							if (mob_oid > 0)
								UseCatchItemPacket(slot, item_id, mob_oid).dispatch();
							else
								LOG(LOG_DEBUG, "UIItemInventory: CATCH no mob nearby");
							break;
						}
						// W4：物品奖励盒（无右键菜单时 SoftKey 确认走 USE_ITEM_REWARD）
						if (item_id / 10000 == 202) {
							// W4: 物品奖励盒 -> USE_ITEM_REWARD(112); s9 ItemRewardHandler 按 itemId 开奖.
							// 原实现只缓存 use_slot/use_id 就 break (死代码), 包类与 handler 齐备漏接线.
							UseItemRewardPacket(slot, item_id).dispatch();
							LOG(LOG_DEBUG, "UIItemInventory: USE_ITEM_REWARD slot=" << slot << " id=" << item_id);
							break;
						}
						// Wave 2a/3：路由（技能书/召唤袋/脚本）已下沉到 UseItemPacket。
						UseItemPacket(slot, item_id).dispatch();
						break;
					}
					case InventoryType::Id::CASH:
					{
						// 现金道具双击：开对应壳（拖装备由各 UI 自己处理）
						int32_t prefix = item_id / 10000;
						// UI_004 强化族：双击魔方/印章/石净化等道具 → 通用强化壳 UIEnhance。
						// 映射依据 CONTRACT.md 族 id 表；锤(557)/维加(561)/保护(559)走各自既有壳。
						// UI_004 药剂罐(18)：5820000(永久)/5820001(7天) 双击开罐。
						if (item_id == 5820000 || item_id == 5820001) {
							UI::get().remove(UIElement::Type::ENHANCE);
							UI::get().emplace<UIEnhance>(18);
							break;
						}
						uint8_t enh_action = 0;
						if (item_id == 5062006 || item_id == 5062005) enh_action = 3;       // 白金/惊人魔方
						else if (item_id == 5062010 || item_id == 5062090) enh_action = 2;    // 黑/记忆魔方(Before/After)
						else if (item_id == 5062503) enh_action = 5;                          // 白附加魔方
						else if (item_id == 5062500 || item_id == 5062501) enh_action = 4;    // 附加魔方
						else if (item_id == 5530111 || item_id == 5530124 || item_id == 5530341) enh_action = 12; // 潜能印章
						else if (item_id == 5530268 || item_id == 5530269 || item_id == 5530342) enh_action = 13; // 附加印章
						else if (item_id == 5062400) enh_action = 19;                         // 神奇铁砧/大师作品
						else if (item_id == 5060023) enh_action = 14;                         // 混沌净化石(石净化)
						else if (item_id / 1000 == 5062) enh_action = 6;                      // 其余 5062xxx → 通用神奇魔方
						if (enh_action != 0) {
							// 单例 TYPE 只能一个实例：换族先 remove 再 emplace，保证 action 正确。
							if (auto old = UI::get().get_element<UIEnhance>()) {
								if (old->get_action() == enh_action && old->is_active()) {
									old->makeactive();
									break;
								}
							}
							UI::get().remove(UIElement::Type::ENHANCE);
							UI::get().emplace<UIEnhance>(enh_action);
							LOG(LOG_DEBUG, "[DBG-ENH] open action=" << static_cast<int>(enh_action)
								<< " id=" << item_id << " slot=" << slot);
							break;
						}
						if (Stage::get().get_player().get_stats().get_stat(MapleStat::Id::HP) == 0) {
							// W3：死亡态现金外观/效果 → USE_DEATHITEM
							UseDeathItemPacket(item_id).dispatch();
							break;
						}
						if (prefix == 500) {
							// W1：宠物召唤/收回 — SPAWN_PET(0x62)
							bool lead = !Stage::get().get_player().has_pet();
							SpawnPetPacket(static_cast<int8_t>(slot), lead).dispatch();
							LOG(LOG_DEBUG, "UIItemInventory: SPAWN_PET slot=" << slot
								<< " id=" << item_id << " lead=" << lead);
						} else if (item_id == 5451000) {
							auto ui = UI::get().get_element<UIRemoteGachapon>();
							if (!ui) ui = UI::get().emplace<UIRemoteGachapon>();
							else if (!ui->is_active()) ui->makeactive();
						} else if (prefix == 557) {
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
						} else if (prefix == 514) {
							// 514* 个人商店许可证：开 PersonalShop（ctor SoftKey 店名 → CREATE，需 FM）
							auto ui = UI::get().get_element<UIPersonalShop>();
							if (!ui) ui = UI::get().emplace<UIPersonalShop>();
							else if (!ui->is_active()) ui->makeactive();
							LOG(LOG_DEBUG, "UIItemInventory: PersonalShop permit id=" << item_id);
						} else if (prefix == 503) {
							// 503* 雇佣商人许可证：开 EntrustedShop（SoftKey 店名 → CREATE type5）
							auto ui = UI::get().get_element<UIEntrustedShop>();
							if (!ui) ui = UI::get().emplace<UIEntrustedShop>();
							else if (!ui->is_active()) ui->makeactive();
							LOG(LOG_DEBUG, "UIItemInventory: EntrustedShop permit id=" << item_id);
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
							
						} else if (prefix == 537) {
							int16_t cash_slot = slot;
							int32_t cash_id = item_id;
							
						} else if (prefix == 508) {
							int16_t cash_slot = slot;
							int32_t cash_id = item_id;
							
						} else if (prefix == 243) {
							// 095 魔方（243*）：原生双击魔方 = 重洗「选中的装备」。
							// 本端 UICube 未建，先按装备背包槽位收口（服务端
							// CashOperationHandler action 0x40 读 EQUIP 槽位）。
							int16_t cube_slot = slot;
							int32_t cube_id = item_id;
							
						} else if (prefix == 512) {
							int16_t cash_slot = slot;
							int32_t cash_id = item_id;
							
						} else {
							// 未映射前缀：SoftKey 确认后简易 USE_CASH（便于实机试未知现金）
							int16_t cash_slot = slot;
							int32_t cash_id = item_id;
							
						}
						break;
					}
					case InventoryType::Id::ETC:
					{
						// 婚礼请柬（4031377/4031395，待寄出）：原生 desc「双击邀请函输入您要邀请的角色名」
						// ⇒ RING_ACTION mode5（guest + marriageId + 本道具槽位）。
						if (WeddingItem::is_send_invitation(item_id)) {
							const MarriageInfo& m = Stage::get().get_player().get_marriage();
							if (m.relationship_id <= 0) {
								if (auto chatbar = UI::get().get_element<UIChatBar>())
									chatbar->show_message(Locale::text("wedding.no_invitation").c_str(),
										UIChatBar::MessageType::YELLOW);
								break;
							}
							int16_t inv_slot = slot;
							const int32_t rel_id = m.relationship_id;
							std::string pre;
							if (auto ci = UI::get().get_element<UICharInfo>())
								pre = ci->get_target_name();
							auto ask_guest = [inv_slot, rel_id](const std::string& name) {
								if (name.empty()) return;
								WeddingInvitePacket(name, rel_id, inv_slot).dispatch();
								LOG(LOG_DEBUG, "UIItemInventory: invitation guest=" << name
									<< " rel=" << rel_id << " slot=" << inv_slot);
							};
							if (!pre.empty())
								ask_guest(pre);
							else
								;
							break;
						}
						// 已收到的请柬（4031406/4031407）：双击 → mode6 查看新人
						if (WeddingItem::is_received_invitation(item_id)) {
							WeddingOpenInvitationPacket(slot, item_id).dispatch();
							LOG(LOG_DEBUG, "UIItemInventory: open invitation slot=" << slot
								<< " id=" << item_id);
							auto ui = UI::get().get_element<UIWedding>();
							if (!ui) ui = UI::get().emplace<UIWedding>();
							else if (!ui->is_active()) ui->makeactive();
							break;
						}
						// 订婚凭据 4031357..4031364（s9 EMPTY_ENGAGEMENT_BOX_* / ENGAGEMENT_RING_*）：
						// 双击 = 解除订婚（RING_ACTION mode3 → breakMarriageRing → breakEngagement）。
						// s9 判据 itemId/10000==403 且 ETC 栏真的持有该道具。
						// 婚礼戒指 11128xx 属 EQUIP 栏，不在本页签，其解约入口走婚礼窗 B 键。
						if (WeddingItem::is_engagement_token(item_id)) {
							UI::get().emplace<UIYesNo>(Locale::text("wedding.break_confirm"),
								[item_id](bool yes) {
									if (!yes) return;
									WeddingBreakPacket(item_id).dispatch();
									LOG(LOG_DEBUG, "UIItemInventory: break token=" << item_id);
								});
							break;
						}
						// W4：金/银宝箱
						if (item_id == 4280000 || item_id == 4280001)
							UseTreasureChestPacket(slot, item_id).dispatch();
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

		if (full_enabled)
		{
			Cursor::State tstate = page_input.send_cursor(cursorpos, pressed);
			if (tstate != Cursor::State::IDLE)
			{
				clear_tooltip();
				return tstate;
			}
		}

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
				// 本格几何是 draw() 里刚写进 Icon 的那一份 ⇒ 抓取偏移和绘制点必然同格。
				icon->start_drag_at(position, cursorpos);

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
			// 083 CUIItem::OnChildNotify @0x81d01f a3==101：整理/扩展/收起悬停文案。
			// 0x7D7 现金扩槽与 2003 槽位合并没有对应按钮，文案键保留、不发明控件。
			Cursor::State bstate = UIElement::send_cursor(pressed, cursorpos);

			const char* tip_key = nullptr;
			struct Tip { uint16_t id; const char* key; };
			static const Tip tips[] = {
				{ Buttons::BT_SORT, "inventory.tip_sort" },
				{ Buttons::BT_SORT_SM, "inventory.tip_sort" },
				{ Buttons::BT_GATHER, "inventory.tip_gather" },
				{ Buttons::BT_GATHER_SM, "inventory.tip_gather" },
				{ Buttons::BT_FULL, "inventory.tip_expand" },
				{ Buttons::BT_SMALL, "inventory.tip_collapse" },
			};

			for (const Tip& tip : tips)
			{
				auto it = buttons.find(tip.id);

				if (it == buttons.end() || !it->second || !it->second->is_active())
					continue;

				if (it->second->bounds(position).contains(cursorpos))
				{
					tip_key = tip.key;
					break;
				}
			}

			if (tip_key)
				UI::get().show_text(Tooltip::Parent::ITEMINVENTORY, Locale::text(tip_key));
			else
				clear_tooltip();

			return bstate;
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
					
				}
			}
			// 原为 'G'/'g'，与 :1182 的 GatherItems 同键；同一 else-if 链一命中即跳，
			// 本分支从未可达，USE_GACHA_EXP(158) 空包等于「补了包没给入口」。
			// r803 笔记的「SoftKey G → UseGachaExp」是旧账口径；G 归 Gather（父版本 df31fb1c^ 既有 +
			// BT_GATHER 按钮同源）。改用链内空位 X/x 让两个出站包都可达；SoftKey 是本端调试键，
			// 不涉协议/包体/原生键位映射。
			else if (keycode == 'X' || keycode == 'x')
			{
				UseGachaExpPacket().dispatch();
				LOG(LOG_DEBUG, "UIItemInventory: UseGachaExp");
			}
			else if (keycode == 'D' || keycode == 'd')
			{
				if (selected_slot > 0 && inventory.get_item_id(tab, selected_slot) > 0) {
					int16_t slot = selected_slot;
					
				} else {
					
				}
			}
			else if (keycode == 'S' || keycode == 's')
			{
				if (selected_slot > 0) {
					int16_t src = selected_slot;
					
				} else {
					
				}
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
					
				}
			}
			else if (keycode == 'C' || keycode == 'c')
			{
				// SoftKey：slot,itemid,text → chalk / kite / mapfx（前缀 k, / m,）
				
			}
		}
	}

	void UIItemInventory::send_scroll(double yoffset)
	{
		if (!is_in_range(UI::get().get_cursor_position()))
			return;

		if (full_enabled)
			change_page(yoffset > 0 ? -1 : 1);
		else
			slider.send_scroll(yoffset);
	}

	int16_t UIItemInventory::page_count() const
	{
		int16_t slotmax = static_cast<int16_t>(inventory.get_slotmax(tab));
		return static_cast<int16_t>((slotmax + max_full_slots - 1) / max_full_slots);
	}

	void UIItemInventory::apply_page(int16_t p)
	{
		int16_t pages = page_count();
		if (pages < 1)
			pages = 1;
		if (p < 1)
			p = 1;
		else if (p > pages)
			p = pages;
		full_page_offset = static_cast<int16_t>((p - 1) * max_full_slots);
		page_input.change_text(std::to_string(p));
		sync_page_chrome();
		load_icons();
	}

	void UIItemInventory::change_page(int16_t delta)
	{
		int16_t cur = static_cast<int16_t>(full_page_offset / max_full_slots) + 1;
		apply_page(static_cast<int16_t>(cur + delta));
	}

	void UIItemInventory::sync_page_chrome()
	{
		int16_t pages = page_count();
		if (pages < 1)
			pages = 1;
		int16_t cur = static_cast<int16_t>(full_page_offset / max_full_slots) + 1;
		// 展开态两颗箭头常显（active=true），边界用 DISABLED 变灰而非隐藏；
		// 折叠态 active=false 隐藏。对齐 UICashShop.cpp:1390-1393 样板。
		auto set_btn = [&](uint16_t id, bool usable) {
			if (auto* b = find_btn(buttons, id))
			{
				b->set_active(full_enabled);
				b->set_state(usable ? Button::State::NORMAL : Button::State::DISABLED);
			}
		};
		set_btn(Buttons::BT_PAGE_PREV, cur > 1);
		set_btn(Buttons::BT_PAGE_NEXT, cur < pages);
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

	void UIItemInventory::set_arrange_button(InventoryType::Id type, bool show_sort)
	{
		// 对齐 Angel 083 `CUIItem::SetArrangeButton(nTI, nState)` @ 0x81e3a6：
		//   `*(this + nTI + 378) = nState;`（m_anSortBtState[nTI]=nState）
		//   `if (nTI == *(this + 377))` 才替换 m_pBtArrange
		//   nState==0 → BtGather(0x7D3)；nState==1 → SP_1852 = BtSort(0x7D4)
		if (type < InventoryType::Id::EQUIP || type > InventoryType::Id::DEC)
			return;

		arrange_bt_state[type] = show_sort;

		if (type != tab)
			return;

		sort_enabled = show_sort;

		auto set_act = [&](uint16_t id, bool act)
		{
			if (auto* b = find_btn(buttons, id))
				b->set_active(act);
		};

		// classic 包没有 AutoBuild/FullAutoBuild ⇒ BT_SORT_SM / BT_GATHER_SM 这两颗**根本不会被创建**
		// （见构造函数 FullAutoBuild 分支）。展开态原来只点亮 _SM 那颗，
		// 于是 W-31 槽位整颗消失；参考图中间红 `+`（Item/BtGather）两态都在。
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

	void UIItemInventory::set_sort(bool enabled)
	{
		set_arrange_button(tab, enabled);
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
		page_input.set_state(Textfield::State::NORMAL);
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
		{
			int16_t lo = full_page_offset + 1;
			int16_t hi = std::min<int16_t>(full_page_offset + max_full_slots,
				static_cast<int16_t>(inventory.get_slotmax(tab)));
			return slot < lo || slot > hi;
		}
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
			UI::get().emplace<UIOk>(Locale::text("uiiteminventory.alerted"), [](bool) {});
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
		// 命中 = get_slotpos 的逆，逐格问版式。
		// 旧实现是另一套除法：col = x / (32+4)、row = y / (32+2) - 1，再加
		// div.quot * 28 的页补偿，和展开态版式（每页 4 列 + page_dx 149 跳列）根本不同源
		// ——格宽 32、列距 36，格子正好跨在除法边界上，每格右侧 4px 会算到邻格，
		// 页边界附近偏得更多（收起态的行距也不同源）。既然绘制走 get_slotpos，
		// 命中就必须也只认 get_slotpos，否则「看着能点、点了是别格」。
		const auto& range = slotrange.at(tab);

		int16_t firstslot = full_enabled ? 1 : range.first;
		int16_t lastslot = full_enabled ? max_full_slots : range.second;

		for (int16_t rel = firstslot; rel <= lastslot; rel++)
		{
			// 几何按 rel 命中，返回的却是绝对槽 abs（服务端槽位）。
			int16_t abs = full_enabled ? static_cast<int16_t>(full_page_offset) + rel : rel;
			if (!is_visible(abs))
				continue;

			Point<int16_t> slotpos = get_slotpos(rel);
			Rectangle<int16_t> cell = Rectangle<int16_t>(slotpos,
				slotpos + Point<int16_t>(Icon::cell_width, Icon::cell_height));

			if (cell.contains(cursorpos))
			{
				LOG(LOG_UI, "Slot: " << abs << " Cell: (" << slotpos.x() << "," << slotpos.y()
					<< ") Cursor: (" << cursorpos.x() << "," << cursorpos.y() << ")");

				return abs;
			}
		}

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
			// 展开态尺子（原生帧 602x291，锚点 (0,1) 换算成窗口相对）：把 Item/disabled
			// 整张 32x32 在帧上逐像素求差，命中 72 格 = 12 列 x 6 行、逐格 SAD=0.000：
			//   列 x = 157/193/229/265 | 306/342/378/414 | 455/491/527/563
			//   行 y = 50/84/118/152/186/220
			// ⇒ 页内列距 36、行距 34、**页距 149**（跨页首列差 41 = 36+5，页边界是双线）。
			// 旧值 {0,150,299,448} 是把帧坐标（158/307/456）直接减 slot_pos.x 得来的，
			// 少减了锚点 dy/dx —— 于是第 2/3/4 页整体右移 1px。见 ItemFull.json note。
			// 旧实现按 3 页 x 8 行（absslot/32）布局，第 7/8 行溢出面板底边=花屏。
			constexpr int16_t PAGE_COLS = 4;
			constexpr int16_t PAGE_ROWS = 6;
			static const int16_t page_dx[4] = { 0, 149, 298, 447 };

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
		if (full_enabled)
			apply_page(1);   // 展开归第 1 页（apply_page 内部已 sync 显隐/按钮态）
		else
			sync_page_chrome();  // 折叠态隐藏翻页三件套按钮
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
		// 丢弃弹窗文案走 i18n（原硬编码英文，中文客户端下不跟随语言设置）。
		const std::string dropmessage = Locale::text("inventory.drop_quantity");
		const std::string untradablemessage = Locale::text("inventory.drop_untradable");
		const std::string cashmessage = Locale::text("inventory.drop_cash");

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