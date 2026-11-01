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
#include "UIEquipInventory.h"

#include "UIMiscShells.h"
#include "UISoftKey.h"
#include "UIViciousHammer.h"

#include "../UI.h"

#include "../Components/MapleButton.h"
#include "../Components/TwoSpriteButton.h"
#include "../UITypes/UIItemInventory.h"

#include "../../Audio/Audio.h"
#include "../../Data/ItemData.h"
#include "../../Gameplay/Stage.h"

#include "../../Net/Packets/CashItemPackets.h"
#include "../../Net/Packets/GameplayPackets.h"
#include "../../Net/Packets/InventoryPackets.h"
#include "../../Net/Packets/PlayerPackets.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

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

		Button* find_btn(std::map<uint16_t, std::unique_ptr<Button>>& buttons, uint16_t id)
		{
			auto it = buttons.find(id);
			return (it != buttons.end()) ? it->second.get() : nullptr;
		}

		// 页签专用：TwoSpriteButton 的 normal 态取 nsrc、选中态（mouseOver/pressed）取 ssrc，
		// 位置是 anchor（左上角 + origin），所以这里按「贴图左上角」给坐标再补 origin —— 与
		// ms::try_maple_btn 同一口径。缺件 → 不创建 + 一行 diag，绝不静默落 (0,0)。
		bool try_pet_tab_btn(std::map<uint16_t, std::unique_ptr<Button>>& buttons, uint16_t id,
			nl::node src, Point<int16_t> topleft, const std::string& site)
		{
			// 实测（evidence/w7-equip §7）：参考截图里未选页 = disabled 美术（mae 0.0，灰底灰数字），
			// 所以未选页用 disabled 美术、当前页用 normal 美术（彩底红数字）。
			nl::node unselected = src["disabled"]["0"];
			nl::node selected = src["normal"]["0"];

			if (!UiAssetResolve::usable(unselected) || !UiAssetResolve::usable(selected))
			{
				UiAssetResolve::diag_once("pet tab missing " + site + " id=" + std::to_string(id),
					"node=" + src.name() + " -> skipped");
				return false;
			}

			Point<int16_t> origin = Texture(unselected).get_origin();

			buttons[id] = std::make_unique<TwoSpriteButton>(unselected, selected,
				Point<int16_t>(static_cast<int16_t>(topleft.x() + origin.x()),
					static_cast<int16_t>(topleft.y() + origin.y())));
			return true;
		}
	}

	UIEquipInventory::UIEquipInventory(const Inventory& invent) : UIDragElement<PosEQINV>(), inventory(invent), tab(Buttons::BT_TAB1), hasPendantSlot(false), hasPocketSlot(false)
	{
		const bool classic_layout = UiAssetResolve::is_classic_hud();

		if (classic_layout)
		{
			// v83 Equip/backgrnd（175x304）槽位格逐格实测（r05-ui-layout.md §2）：
			// 列 x=5/38/71/104/137（步进 33），行 y=32/65/98/131/164/197/230（步进 33）。
			// 每格标签烘焙在精灵里，实际版式：
			//   行1 帽子(3列)；行2 勋章/额饰/戒指/戒指；
			//   行3 眼饰/耳饰；行4 披风/上衣/项链/武器/盾牌；
			//   行5 手套/裤·裙/腰带/指环/指环；行6 鞋子(3列)；行7 骑兽/鞍子。
			// 旧表 HAT 落在第 2 列 (38,35)——帽子标签实际在第 3 列，
			// 导致帽子图标压到额饰格、帽子格空置。
			iconpositions[EquipSlot::Id::HAT] = Point<int16_t>(71, 32);
			iconpositions[EquipSlot::Id::MEDAL] = Point<int16_t>(5, 65);
			iconpositions[EquipSlot::Id::FACE] = Point<int16_t>(38, 65);
			iconpositions[EquipSlot::Id::RING3] = Point<int16_t>(104, 65);
			iconpositions[EquipSlot::Id::RING4] = Point<int16_t>(137, 65);
			iconpositions[EquipSlot::Id::EYEACC] = Point<int16_t>(71, 98);
			iconpositions[EquipSlot::Id::EARACC] = Point<int16_t>(104, 98);
			iconpositions[EquipSlot::Id::CAPE] = Point<int16_t>(5, 131);
			iconpositions[EquipSlot::Id::TOP] = Point<int16_t>(38, 131);
			iconpositions[EquipSlot::Id::PENDANT1] = Point<int16_t>(71, 131);
			iconpositions[EquipSlot::Id::WEAPON] = Point<int16_t>(104, 131);
			iconpositions[EquipSlot::Id::SHIELD] = Point<int16_t>(137, 131);
			iconpositions[EquipSlot::Id::SUBWEAPON] = Point<int16_t>(137, 131);
			iconpositions[EquipSlot::Id::GLOVES] = Point<int16_t>(5, 164);
			iconpositions[EquipSlot::Id::BOTTOM] = Point<int16_t>(38, 164);
			iconpositions[EquipSlot::Id::BELT] = Point<int16_t>(71, 164);
			iconpositions[EquipSlot::Id::RING1] = Point<int16_t>(104, 164);
			iconpositions[EquipSlot::Id::RING2] = Point<int16_t>(137, 164);
			iconpositions[EquipSlot::Id::SHOES] = Point<int16_t>(71, 197);
			iconpositions[EquipSlot::Id::TAMEDMOB] = Point<int16_t>(5, 230);
			iconpositions[EquipSlot::Id::SADDLE] = Point<int16_t>(38, 230);
		}
		else
		{
			// Column 1
			iconpositions[EquipSlot::Id::RING1] = Point<int16_t>(14, 50);
			iconpositions[EquipSlot::Id::RING2] = Point<int16_t>(14, 91);
			iconpositions[EquipSlot::Id::RING3] = Point<int16_t>(14, 132);
			iconpositions[EquipSlot::Id::RING4] = Point<int16_t>(14, 173);
			iconpositions[EquipSlot::Id::POCKET] = Point<int16_t>(14, 214);
			iconpositions[EquipSlot::Id::BOOK] = Point<int16_t>(14, 255);

			// Column 2
			iconpositions[EquipSlot::Id::PENDANT2] = Point<int16_t>(55, 91);
			iconpositions[EquipSlot::Id::PENDANT1] = Point<int16_t>(55, 132);
			iconpositions[EquipSlot::Id::WEAPON] = Point<int16_t>(55, 173);
			iconpositions[EquipSlot::Id::BELT] = Point<int16_t>(55, 214);

			// Column 3
			iconpositions[EquipSlot::Id::HAT] = Point<int16_t>(96, 50);
			iconpositions[EquipSlot::Id::FACE] = Point<int16_t>(96, 91);
			iconpositions[EquipSlot::Id::EYEACC] = Point<int16_t>(96, 132);
			iconpositions[EquipSlot::Id::TOP] = Point<int16_t>(96, 173);
			iconpositions[EquipSlot::Id::BOTTOM] = Point<int16_t>(96, 214);
			iconpositions[EquipSlot::Id::SHOES] = Point<int16_t>(96, 255);

			// Column 4
			iconpositions[EquipSlot::Id::EARACC] = Point<int16_t>(137, 132);
			iconpositions[EquipSlot::Id::SHOULDER] = Point<int16_t>(137, 173);
			iconpositions[EquipSlot::Id::GLOVES] = Point<int16_t>(137, 214);
			iconpositions[EquipSlot::Id::ANDROID_SLOT] = Point<int16_t>(137, 255);

			// Column 5
			iconpositions[EquipSlot::Id::EMBLEM] = Point<int16_t>(178, 50);
			iconpositions[EquipSlot::Id::BADGE] = Point<int16_t>(178, 91);
			iconpositions[EquipSlot::Id::MEDAL] = Point<int16_t>(178, 132);
			iconpositions[EquipSlot::Id::SUBWEAPON] = Point<int16_t>(178, 173);
			iconpositions[EquipSlot::Id::CAPE] = Point<int16_t>(178, 214);
			iconpositions[EquipSlot::Id::HEART] = Point<int16_t>(178, 255);
		}

		tab_source[Buttons::BT_TAB0] = "Equip";
		tab_source[Buttons::BT_TAB1] = "Cash";
		tab_source[Buttons::BT_TAB2] = "Pet";
		tab_source[Buttons::BT_TAB3] = "Android";

		nl::node close = basic_close_btn();
		nl::node Equip = UiAssetResolve::window4("Equip");

		if (!UiAssetResolve::usable(Equip))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		bool classic_flat = !UiAssetResolve::usable(Equip[tab_source[Buttons::BT_TAB0]]);

		this->classic_flat = classic_flat;

		nl::node backgrnd = Equip["backgrnd"];
		Point<int16_t> bg_dimensions = Texture(backgrnd).get_dimensions();

		if (classic_flat)
		{
			if (backgrnd)
				sprites.emplace_back(backgrnd);

			background[Buttons::BT_TAB0] = Texture();
			totem_dimensions = Point<int16_t>(0, 0);
			totem_adj = Point<int16_t>(0, 0);

			if (close)
				buttons[Buttons::BT_CLOSE] = std::make_unique<MapleButton>(close, Point<int16_t>(bg_dimensions.x() - 19, 5));

			// BtDetail/BtCashshop 原生默认不显示：参考截图里两钮全图模板匹配最优 mae 55.8 / 61.1
			// （对照同截图内实际显示的钮 mae ≤0.8）→ 截图内不存在。坐标按 backgrnd 底栏左段兜底落位，
			// 构造后立即收起，绝不再叠在 (0,0)。
			try_maple_btn(buttons, Buttons::BT_SLOT, Equip["BtDetail"],
				Point<int16_t>(4, static_cast<int16_t>(bg_dimensions.y() - 25)), "equip BtDetail");
			try_maple_btn(buttons, Buttons::BT_SHOP, Equip["BtCashshop"],
				Point<int16_t>(54, static_cast<int16_t>(bg_dimensions.y() - 21)), "equip BtCashshop");

			if (auto* b = find_btn(buttons, Buttons::BT_SLOT))
				b->set_active(false);
			if (auto* b = find_btn(buttons, Buttons::BT_SHOP))
				b->set_active(false);

			// 宠物装备区：以窗体右下角为锚，坐标逐像素模板匹配 正确客户端截图/角色装备.png 量得
			// （窗口原点 = Equip/backgrnd 左上角，见 evidence/w7-equip/coords.md）。
			pet_backgrnd = Texture(Equip["pet"]);

			if (pet_backgrnd.is_valid())
			{
				const int16_t right = bg_dimensions.x();
				const int16_t bottom = bg_dimensions.y();

				// pet[177x181] 压住窗口右边 3px，底边与窗口底边齐平
				pet_panel_pos = Point<int16_t>(static_cast<int16_t>(right - 3),
					static_cast<int16_t>(bottom - pet_backgrnd.height()));

				try_maple_btn(buttons, Buttons::BT_PET_SHOW, Equip["BtPetEquipShow"],
					Point<int16_t>(static_cast<int16_t>(right - 73), static_cast<int16_t>(bottom - 24)), "equip BtPetEquipShow");

				// 页签 origin=(15,0)（四态同 origin），间距 34，贴图左上角 (right+7, bottom-30) = (182/216/250, 274)
				static const char* const pet_tabs[3] = { "BtPet1", "BtPet2", "BtPet3" };

				for (uint16_t i = 0; i < 3; i++)
					try_pet_tab_btn(buttons, Buttons::BT_PET_TAB0 + i, Equip[pet_tabs[i]],
						Point<int16_t>(static_cast<int16_t>(right + 7 + 34 * i), static_cast<int16_t>(bottom - 30)), "equip pet tab");

				try_maple_btn(buttons, Buttons::BT_PET_HIDE, Equip["BtPetEquipHide"],
					Point<int16_t>(static_cast<int16_t>(right + 152), static_cast<int16_t>(bottom - 22)), "equip BtPetEquipHide");
			}

			base_dimension = bg_dimensions;

			const int16_t expanded_x = static_cast<int16_t>(pet_panel_pos.x() + pet_backgrnd.width());
			const int16_t expanded_y = static_cast<int16_t>(pet_panel_pos.y() + pet_backgrnd.height());

			expanded_dimension = Point<int16_t>(
				expanded_x > bg_dimensions.x() ? expanded_x : bg_dimensions.x(),
				expanded_y > bg_dimensions.y() ? expanded_y : bg_dimensions.y());

			set_pet_panel(false);

			dimension = bg_dimensions;
			dragarea = Point<int16_t>(bg_dimensions.x(), 20);

			tab = Buttons::BT_TAB0;
			load_icons();
			return;
		}

		background[Buttons::BT_TAB0] = Equip[tab_source[Buttons::BT_TAB0]]["backgrnd"];
		background[Buttons::BT_TAB1] = Equip[tab_source[Buttons::BT_TAB1]]["backgrnd"];
		background[Buttons::BT_TAB2] = Equip[tab_source[Buttons::BT_TAB2]]["backgrnd"];
		background[Buttons::BT_TAB3] = Equip[tab_source[Buttons::BT_TAB3]]["backgrnd"];

		for (uint16_t i = Buttons::BT_TAB0; i < Buttons::BT_TABE; i++)
			for (auto slot : Equip[tab_source[i]]["Slots"])
				if (slot.name().find("_") == std::string::npos)
					Slots[i].emplace_back(slot);

		nl::node EquipGL = nl::nx::UI["UIWindowGL.img"]["Equip"];
		nl::node totem_backgrnd = EquipGL["Totem"]["backgrnd"];

		totem_dimensions = Texture(totem_backgrnd).get_dimensions();
		totem_adj = Point<int16_t>(-totem_dimensions.x() + 4, 0);

		if (totem_backgrnd)
			sprites.emplace_back(totem_backgrnd, totem_adj);
		if (backgrnd)
			sprites.emplace_back(backgrnd);

		tabbar = Equip["tabbar"];
		disabled = Equip[tab_source[Buttons::BT_TAB0]]["disabled"];
		disabled2 = Equip[tab_source[Buttons::BT_TAB0]]["disabled2"];

		if (close)
			buttons[Buttons::BT_CLOSE] = std::make_unique<MapleButton>(close, Point<int16_t>(bg_dimensions.x() - 19, 5));

		try_maple_btn(buttons, Buttons::BT_SLOT, Equip[tab_source[Buttons::BT_TAB0]]["BtSlot"]);
		try_maple_btn(buttons, Buttons::BT_EFFECT, EquipGL["Equip"]["btEffect"]);
		try_maple_btn(buttons, Buttons::BT_SALON, EquipGL["Equip"]["btSalon"]);
		try_maple_btn(buttons, Buttons::BT_CONSUMESETTING, Equip[tab_source[Buttons::BT_TAB2]]["BtConsumeSetting"]);
		try_maple_btn(buttons, Buttons::BT_EXCEPTION, Equip[tab_source[Buttons::BT_TAB2]]["BtException"]);
		try_maple_btn(buttons, Buttons::BT_SHOP, Equip[tab_source[Buttons::BT_TAB3]]["BtShop"]);

		// BT_EXCEPTION / BT_SHOP / BT_CONSUMESETTING 已接线（r228/r229）

		nl::node Tab = Equip["Tab"];

		for (uint16_t i = Buttons::BT_TAB0; i < Buttons::BT_TABE; i++)
		{
			if (Tab["disabled"][i] && Tab["enabled"][i])
				buttons[Buttons::BT_TAB0 + i] = std::make_unique<TwoSpriteButton>(Tab["disabled"][i], Tab["enabled"][i], Point<int16_t>(0, 3));
		}

		dimension = bg_dimensions;
		dragarea = Point<int16_t>(bg_dimensions.x(), 20);

		load_icons();
		change_tab(Buttons::BT_TAB0);
	}

	void UIEquipInventory::draw(float alpha) const
	{
		draw_sprites(alpha);

		// 宠物装备面板底图（Equip/pet，槽名已烤进贴图，不再用 Text 重画）
		if (pet_panel_open)
			pet_backgrnd.draw(position + pet_panel_pos);

		draw_buttons(alpha);

		background[tab].draw(position);
		tabbar.draw(position);

		for (auto slot : Slots[tab])
			slot.draw(position);

		if (tab == Buttons::BT_TAB0)
		{
			if (!hasPendantSlot && disabled.is_valid() && (iconpositions[EquipSlot::Id::PENDANT2].x() || iconpositions[EquipSlot::Id::PENDANT2].y()))
				disabled.draw(position + iconpositions[EquipSlot::Id::PENDANT2]);

			if (!hasPocketSlot && disabled.is_valid() && (iconpositions[EquipSlot::Id::POCKET].x() || iconpositions[EquipSlot::Id::POCKET].y()))
				disabled.draw(position + iconpositions[EquipSlot::Id::POCKET]);

			for (auto iter : icons)
			{
				if (!iter.second)
					continue;

				Point<int16_t> ip = iconpositions[iter.first];

				if (ip.x() == 0 && ip.y() == 0)
					continue;

				iter.second->draw(position + ip + Point<int16_t>(4, 4));
			}
		}
		else if (tab == Buttons::BT_TAB2)
		{
			if (disabled2.is_valid())
			{
				disabled2.draw(position + Point<int16_t>(113, 57));
				disabled2.draw(position + Point<int16_t>(113, 106));
				disabled2.draw(position + Point<int16_t>(113, 155));
			}
		}
	}

	Button::State UIEquipInventory::button_pressed(uint16_t id)
	{
		switch (id)
		{
		case Buttons::BT_CLOSE:
			toggle_active();
			break;
		case Buttons::BT_TAB0:
		case Buttons::BT_TAB1:
		case Buttons::BT_TAB2:
		case Buttons::BT_TAB3:
			change_tab(id);

			return Button::State::IDENTITY;
		case Buttons::BT_SHOP:
			EnterCashShopPacket().dispatch();
			LOG(LOG_DEBUG, "UIEquipInventory: EnterCashShop");
			return Button::State::NORMAL;
		case Buttons::BT_EXCEPTION:
		{
			auto ex = UI::get().get_element<UIExceptionItemSearch>();
			if (!ex)
				ex = UI::get().emplace<UIExceptionItemSearch>();
			else if (!ex->is_active())
				ex->makeactive();
			return Button::State::NORMAL;
		}
		case Buttons::BT_CONSUMESETTING:
		{
			int32_t from_inv = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				int16_t slot = inv->get_selected_slot();
				if (slot > 0)
					from_inv = Stage::get().get_player().get_inventory().get_item_id(inv->get_tab(), slot);
			}
			if (from_inv > 0) {
				UseItemEffectPacket(from_inv).dispatch();
				LOG(LOG_DEBUG, "UIEquipInventory: ConsumeSetting effect=" << from_inv);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& tip) {
					try {
						int32_t id = std::stoi(tip);
						if (id > 0) {
							UseItemEffectPacket(id).dispatch();
							LOG(LOG_DEBUG, "UIEquipInventory: ConsumeSetting effect=" << id);
						}
					} catch (...) {
						if (!tip.empty())
							LOG(LOG_DEBUG, "UIEquipInventory: ConsumeSetting tip=" << tip);
					}
				}, []() {}, "Effect itemid");
			}
			return Button::State::NORMAL;
		}
		case Buttons::BT_SLOT:
		{
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				if (inv->get_tab() == InventoryType::Id::CASH) {
					int16_t slot = inv->get_selected_slot();
					if (slot > 0) {
						int32_t itemid = Stage::get().get_player().get_inventory()
							.get_item_id(InventoryType::Id::CASH, slot);
						if (itemid > 0) {
							UseCashItemBasicPacket(slot, itemid).dispatch();
							LOG(LOG_DEBUG, "UIEquipInventory: Slot expand from inv cash=" << slot << " id=" << itemid);
							return Button::State::NORMAL;
						}
					}
				}
			}
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					// cash_slot,itemid — 扩展栏现金道具
					auto comma = s.find(',');
					if (comma == std::string::npos) return;
					int16_t cash_slot = static_cast<int16_t>(std::stoi(s.substr(0, comma)));
					int32_t itemid = std::stoi(s.substr(comma + 1));
					if (cash_slot > 0 && itemid > 0) {
						UseCashItemBasicPacket(cash_slot, itemid).dispatch();
						LOG(LOG_DEBUG, "UIEquipInventory: Slot expand cash=" << cash_slot << " id=" << itemid);
					}
				} catch (...) {}
			}, []() {}, "cashSlot,itemId");
			return Button::State::NORMAL;
		}
		case Buttons::BT_EFFECT:
		{
			int32_t from_inv = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				int16_t slot = inv->get_selected_slot();
				if (slot > 0)
					from_inv = Stage::get().get_player().get_inventory().get_item_id(inv->get_tab(), slot);
			}
			if (from_inv > 0) {
				UseItemEffectPacket(from_inv).dispatch();
				LOG(LOG_DEBUG, "UIEquipInventory: Effect itemid=" << from_inv);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t id = std::stoi(s);
						if (id > 0) {
							UseItemEffectPacket(id).dispatch();
							LOG(LOG_DEBUG, "UIEquipInventory: Effect itemid=" << id);
						}
					} catch (...) {}
				}, []() {}, "Effect itemid");
			}
			return Button::State::NORMAL;
		}
		case Buttons::BT_SALON:
		{
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				if (inv->get_tab() == InventoryType::Id::CASH) {
					int16_t slot = inv->get_selected_slot();
					if (slot > 0) {
						int32_t itemid = Stage::get().get_player().get_inventory()
							.get_item_id(InventoryType::Id::CASH, slot);
						if (itemid > 0) {
							UseCashItemBasicPacket(slot, itemid).dispatch();
							LOG(LOG_DEBUG, "UIEquipInventory: Salon from inv cash=" << slot << " id=" << itemid);
							return Button::State::NORMAL;
						}
					}
				}
			}
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					auto comma = s.find(',');
					if (comma == std::string::npos) return;
					int16_t cash_slot = static_cast<int16_t>(std::stoi(s.substr(0, comma)));
					int32_t itemid = std::stoi(s.substr(comma + 1));
					if (cash_slot > 0 && itemid > 0) {
						UseCashItemBasicPacket(cash_slot, itemid).dispatch();
						LOG(LOG_DEBUG, "UIEquipInventory: Salon cash=" << cash_slot << " id=" << itemid);
					}
				} catch (...) {}
			}, []() {}, "cashSlot,itemId");
			return Button::State::NORMAL;
		}
		case Buttons::BT_PET_SHOW:
			set_pet_panel(true);
			return Button::State::NORMAL;
		case Buttons::BT_PET_HIDE:
			set_pet_panel(false);
			return Button::State::NORMAL;
		case Buttons::BT_PET_TAB0:
		case Buttons::BT_PET_TAB1:
		case Buttons::BT_PET_TAB2:
			change_pet_tab(id);
			return Button::State::IDENTITY;
		default:
			break;
		}

		return Button::State::NORMAL;
	}

	void UIEquipInventory::update_slot(EquipSlot::Id slot)
	{
		if (int32_t item_id = inventory.get_item_id(InventoryType::Id::EQUIPPED, slot))
		{
			const Texture& texture = ItemData::get(item_id).get_icon(false);

			icons[slot] = std::make_unique<Icon>(
				std::make_unique<EquipIcon>(slot),
				texture,
				-1
				);
		}
		else if (icons[slot])
		{
			icons[slot].release();
		}

		clear_tooltip();
	}

	void UIEquipInventory::load_icons()
	{
		icons.clear();

		for (auto iter : EquipSlot::values)
			update_slot(iter);
	}

	Cursor::State UIEquipInventory::send_cursor(bool pressed, Point<int16_t> cursorpos)
	{
		Cursor::State dstate = UIDragElement::send_cursor(pressed, cursorpos);

		if (dragged)
		{
			clear_tooltip();

			return dstate;
		}

		EquipSlot::Id slot = slot_by_position(cursorpos);

		if (auto icon = icons[slot].get())
		{
			selected_eqslot = slot;
			if (pressed)
			{
				icon->start_drag(cursorpos - position - iconpositions[slot]);

				UI::get().drag_icon(icon);

				clear_tooltip();

				return Cursor::State::GRABBING;
			}
			else
			{
				show_equip(slot);

				return Cursor::State::CANGRAB;
			}
		}
		else
		{
			clear_tooltip();

			return Cursor::State::IDLE;
		}
	}

	void UIEquipInventory::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				toggle_active();
			}
			else if (keycode == KeyAction::Id::TAB)
			{
				uint16_t newtab = tab + 1;

				if (newtab >= Buttons::BT_TABE)
					newtab = Buttons::BT_TAB0;

				change_tab(newtab);
			}
			else if (keycode == 'U' || keycode == 'u' || keycode == 'S' || keycode == 's')
			{
				if (selected_eqslot != EquipSlot::Id::NONE
					&& inventory.get_item_id(InventoryType::Id::EQUIPPED, selected_eqslot) > 0) {
					if (int16_t freeslot = inventory.find_free_slot(InventoryType::Id::EQUIP))
					{
						UnequipItemPacket(selected_eqslot, freeslot).dispatch();
						LOG(LOG_DEBUG, "UIEquipInventory: Unequip selected="
							<< static_cast<int>(selected_eqslot));
					}
				} else {
					UI::get().emplace<UISoftKey>([this](const std::string& s) {
						try {
							int16_t slot = static_cast<int16_t>(std::stoi(s));
							if (slot <= 0) return;
							EquipSlot::Id eq = EquipSlot::by_id(static_cast<size_t>(slot));
							if (eq == EquipSlot::Id::NONE) return;
							selected_eqslot = eq;
							if (int16_t freeslot = inventory.find_free_slot(InventoryType::Id::EQUIP))
							{
								UnequipItemPacket(eq, freeslot).dispatch();
								LOG(LOG_DEBUG, "UIEquipInventory: SoftKey unequip slot=" << slot);
							}
						} catch (...) {}
					}, []() {}, "Unequip slot");
				}
			}
			else if (keycode == 'I' || keycode == 'i')
			{
				auto ui = UI::get().get_element<UIItemInventory>();
				if (!ui)
					ui = UI::get().emplace<UIItemInventory>(inventory);
				else if (!ui->is_active())
					ui->makeactive();
			}
			else if (keycode == 'V' || keycode == 'v')
			{
				auto ui = UI::get().get_element<UIViciousHammer>();
				if (!ui) ui = UI::get().emplace<UIViciousHammer>();
				else if (!ui->is_active()) ui->makeactive();
			}
		}
	}

	UIElement::Type UIEquipInventory::get_type() const
	{
		return TYPE;
	}

	void UIEquipInventory::doubleclick(Point<int16_t> cursorpos)
	{
		EquipSlot::Id slot = slot_by_position(cursorpos);

		if (icons[slot])
			if (int16_t freeslot = inventory.find_free_slot(InventoryType::Id::EQUIP))
				UnequipItemPacket(slot, freeslot).dispatch();
	}

	bool UIEquipInventory::is_in_range(Point<int16_t> cursorpos) const
	{
		Rectangle<int16_t> bounds = Rectangle<int16_t>(position, position + dimension);

		Rectangle<int16_t> totem_bounds = Rectangle<int16_t>(position, position + totem_dimensions);
		totem_bounds.shift(totem_adj);

		return bounds.contains(cursorpos) || totem_bounds.contains(cursorpos);
	}

	bool UIEquipInventory::send_icon(const Icon& icon, Point<int16_t> cursorpos)
	{
		if (EquipSlot::Id slot = slot_by_position(cursorpos))
			icon.drop_on_equips(slot);

		return true;
	}

	void UIEquipInventory::toggle_active()
	{
		clear_tooltip();

		UIElement::toggle_active();
	}

	void UIEquipInventory::modify(int16_t pos, int8_t mode, int16_t arg)
	{
		EquipSlot::Id eqpos = EquipSlot::by_id(pos);
		EquipSlot::Id eqarg = EquipSlot::by_id(arg);

		switch (mode)
		{
		case 0:
		case 3:
			update_slot(eqpos);
			break;
		case 2:
			update_slot(eqpos);
			update_slot(eqarg);
			break;
		}
	}

	void UIEquipInventory::show_equip(EquipSlot::Id slot)
	{
		UI::get().show_equip(Tooltip::Parent::EQUIPINVENTORY, slot);
	}

	void UIEquipInventory::clear_tooltip()
	{
		UI::get().clear_tooltip(Tooltip::Parent::EQUIPINVENTORY);
	}

	EquipSlot::Id UIEquipInventory::slot_by_position(Point<int16_t> cursorpos) const
	{
		if (tab != Buttons::BT_TAB0)
			return EquipSlot::Id::NONE;

		for (auto iter : iconpositions)
		{
			if (iter.second.x() == 0 && iter.second.y() == 0)
				continue;

			Rectangle<int16_t> iconrect = Rectangle<int16_t>(
				position + iter.second,
				position + iter.second + Point<int16_t>(32, 32)
				);

			if (iconrect.contains(cursorpos))
				return iter.first;
		}

		return EquipSlot::Id::NONE;
	}

	void UIEquipInventory::change_tab(uint16_t tabid)
	{
		uint8_t oldtab = tab;
		tab = tabid;

		if (oldtab != tab)
		{
			clear_tooltip();

			if (auto* b = find_btn(buttons, oldtab))
				b->set_state(Button::State::NORMAL);
			if (auto* b = find_btn(buttons, tab))
				b->set_state(Button::State::PRESSED);

			// v83 flat 版式没有页签：BtDetail/BtCashshop 保持构造时的默认隐藏
			if (classic_flat)
				return;

			if (auto* b = find_btn(buttons, Buttons::BT_SLOT))
				b->set_active(tab == Buttons::BT_TAB0);

			if (auto* b = find_btn(buttons, Buttons::BT_CONSUMESETTING))
				b->set_active(tab == Buttons::BT_TAB2);
			if (auto* b = find_btn(buttons, Buttons::BT_EXCEPTION))
				b->set_active(tab == Buttons::BT_TAB2);

			if (auto* b = find_btn(buttons, Buttons::BT_SHOP))
				b->set_active(tab == Buttons::BT_TAB3);
		}
	}

	void UIEquipInventory::set_pet_panel(bool open)
	{
		pet_panel_open = open && pet_backgrnd.is_valid();

		if (auto* b = find_btn(buttons, Buttons::BT_PET_SHOW))
			b->set_active(!pet_panel_open);
		if (auto* b = find_btn(buttons, Buttons::BT_PET_HIDE))
			b->set_active(pet_panel_open);

		for (uint16_t i = Buttons::BT_PET_TAB0; i < Buttons::BT_PET_TABE; i++)
			if (auto* b = find_btn(buttons, i))
				b->set_active(pet_panel_open);

		if (pet_panel_open)
			change_pet_tab(pet_tab);

		dimension = pet_panel_open ? expanded_dimension : base_dimension;
	}

	void UIEquipInventory::change_pet_tab(uint16_t tabid)
	{
		if (tabid < Buttons::BT_PET_TAB0 || tabid >= Buttons::BT_PET_TABE)
			return;

		pet_tab = tabid;

		for (uint16_t i = Buttons::BT_PET_TAB0; i < Buttons::BT_PET_TABE; i++)
			if (auto* b = find_btn(buttons, i))
				// TwoSpriteButton：NORMAL → disabled 美术（灰），PRESSED/mouseOver → normal 美术（彩）。
				// 不能用 Button::State::DISABLED 表示未选页，因为 Button::is_active() 排除 DISABLED → 点不中。
				b->set_state(i == pet_tab ? Button::State::PRESSED : Button::State::NORMAL);

		// 预留：BtPet1/2/3 三页宠物装备槽的穿戴/卸下（Equip/pet 下无 Slots 子节点，槽位数据待协议批次）
	}

	UIEquipInventory::EquipIcon::EquipIcon(int16_t s)
	{
		source = s;
	}

	void UIEquipInventory::EquipIcon::drop_on_stage() const
	{
		Sound(Sound::Name::DRAGEND).play();
	}

	void UIEquipInventory::EquipIcon::drop_on_equips(EquipSlot::Id slot) const
	{
		if (source == slot)
			Sound(Sound::Name::DRAGEND).play();
	}

	bool UIEquipInventory::EquipIcon::drop_on_items(InventoryType::Id tab, EquipSlot::Id eqslot, int16_t slot, bool equip) const
	{
		if (tab != InventoryType::Id::EQUIP)
		{
			if (auto iteminventory = UI::get().get_element<UIItemInventory>())
			{
				if (iteminventory->is_active())
				{
					iteminventory->change_tab(InventoryType::Id::EQUIP);
					return false;
				}
			}
		}

		if (equip)
		{
			if (eqslot == source)
				EquipItemPacket(slot, eqslot).dispatch();
		}
		else
		{
			UnequipItemPacket(source, slot).dispatch();
		}

		return true;
	}

	Icon::IconType UIEquipInventory::EquipIcon::get_type()
	{
		return Icon::IconType::EQUIP;
	}
}