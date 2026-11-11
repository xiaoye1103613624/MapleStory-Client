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

	UIEquipInventory::UIEquipInventory(const Inventory& invent) : UIDragElement<PosEQINV>(), inventory(invent)
	{
		nl::node close = basic_close_btn();
		nl::node Equip = UiAssetResolve::window4("Equip");

		if (!UiAssetResolve::usable(Equip))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		// 槽位格逐格实测（evidence/r27-b3-equip/）：v83 Equip/backgrnd（175x304），
		// 列 x=5/38/71/104/137（步进 33），行 y=32/65/98/131/164/197/230（步进 33）。
		// 每格标签烘焙在精灵里，实际版式：
		//   行1 帽子(3列)；行2 勋章/额饰/戒指/戒指；
		//   行3 眼饰/耳饰；行4 披风/上衣/项链/武器/盾牌；
		//   行5 手套/裤·裙/腰带/指环/指环；行6 鞋子(3列)；行7 骑兽/鞍子。
		// 这张表是「绘制点 / 命中区 / 调色矩形」三件事的唯一真源。以前底图按
		// classic_flat（Equip/Equip 子壳在不在）选、坐标按 is_classic_hud()
		// （StatusBar.img 在不在）选，两个谓词不同源，一旦一个成立一个不成立就是
		// 「底图 v83 单张、坐标另一套 6 列」的整窗错位；那套 6 列分支与后面的
		// 页签/totem 分支在本包里永不可达，已按裁决删除（单一版式、单一路径）。
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

		nl::node backgrnd = Equip["backgrnd"];

		backgrnd_tex = Texture(backgrnd);
		bg_origin = backgrnd_tex.get_origin();

		const Point<int16_t> bg_dimensions = backgrnd_tex.get_dimensions();

		if (backgrnd)
			sprites.emplace_back(backgrnd);

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

		// round78 / Angel 083 依据链（纯注释，0 行为字面量）：
		// - 资源根：orange-wz MCP 128B WzAESConstant.DEFAULT_KEY 实证
		//   UIWindow.img/Equip 10 真节点全在（backgrnd / pet / FullBackgrnd /
		//   BtDetail / BtPet1-3 / BtPetEquipShow / BtPetEquipHide / BtCashshop）。
		//   旧 _equip_probe_stdout.txt 的 "IMG 解析失败" 是 65536B NoLife 全量表
		//   错规格假阻塞（round85 4b2452c4 已翻案），非 MCP 故障、非重名。
		// - 083 活 IDA：CUIEquip::OnButtonClicked @0x7feb72 case 3000 商城 /
		//   3001 ToggleDragonEquip / 3002 TogglePetEquip；
		//   CUIEquip::TogglePetEquip @0x7ffa84 ZAlloc 1456B 独立 CUIPetEquip
		//   子窗（父窗偏移 +123）；CUIPetEquip::OnButtonClicked @0x8019f1
		//   case 2000 收回 / 2001-2003 三页签 InvalidateRect。
		// - 083 无 CUIEquip / CUIPetEquip 的 OnCreate 符号（sym2_*OnCreate total=0）
		//   ⇒ 无布局立即数，坐标按门禁 18 走截图 SAD 备选（合法）。
		// - 定性：合法混合面反例（真资源根 + 截图坐标 + 083 独立子窗架构差异），
		//   不是虚构根同族病。本端内嵌宠物面板是 Journey 架构差异、交互语义对齐
		//   （BT_PET_SHOW/HIDE 开合 + BT_PET_TAB0/1/2 三页签）；按
		//   「校准≠改回原生 / s9 优先」不重写架构，本轮 0 行为改动。
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

			// 实测 BtPet1/2/3 四态 origin 都是 (15,13)（不是旧注释写的 (15,0)，见
			// evidence/r27-b3-equip/README.md §1）。这三格在现有原生帧上 agree 仅 .322
			// —— 那张帧没画宠物页签，落位没有尺，故本轮**只记不改**：按 (15,0) 推出来的
			// 现有 y 可能整体高 13px，等一张展开宠物面板的原生帧再定（列入用户取证）。
			// 间距 34 沿用 evidence/w7-equip/coords.md 的模板匹配结果。
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

		build_disabled_cells();
		load_icons();
	}

	void UIEquipInventory::draw(float alpha) const
	{
		draw_sprites(alpha);

		// 宠物装备面板底图（Equip/pet，槽名已烤进贴图，不再用 Text 重画）
		if (pet_panel_open)
			pet_backgrnd.draw(position + pet_panel_pos);

		draw_buttons(alpha);

		// 不可用槽：原生没有第二张 disabled 美术，而是对同一张 backgrnd 的该格逐像素
		// 改色（evidence/r27-b3-equip §6）。改色只由 slot_available() 的运行期判据决定
		// —— 哪个格子变粉是算出来的，不是写死的格子表。
		for (size_t i = 0; i < TINTABLE_SLOTS.size(); i++)
		{
			EquipSlot::Id slot = TINTABLE_SLOTS[i];

			if (slot_available(slot) || i >= disabled_cells.size() || !disabled_cells[i].is_valid())
				continue;

			Point<int16_t> ip = iconpositions[slot];

			if (ip.x() == 0 && ip.y() == 0)
				continue;

			// 调色矩形 = 格子 + (0,3)（底图凹槽相对 iconpositions 实测下移 3px）。
			// 裁出来的就是底图上这一块，所以落位 = 底图左上角落点 + 矩形偏移，
			// 与 draw_sprites 里那张 backgrnd 逐像素重合。
			disabled_cells[i].draw(position - bg_origin + Point<int16_t>(ip.x(), ip.y() + 3));
		}

		for (auto iter : icons)
		{
			if (!iter.second)
				continue;

			Point<int16_t> ip = iconpositions[iter.first];

			if (ip.x() == 0 && ip.y() == 0)
				continue;

			// 与 slot_by_position() 同源：格左上角 = iconpositions，格大小 = Icon 本格。
			// 图标自身的 texture origin 由 DrawArgument 负责补偿。原先这里额外 +（4,4），
			// 而底图凹槽相位实测已经是 (5,32) 起、格 32x32 —— 那 4px 会把每件装备
			// 压向右下、盖住下一格的标签（用户报的「装备 ui 位置偏移」）。
			iter.second->set_cell(ip);
			iter.second->draw_at(position);
		}
	}

	Button::State UIEquipInventory::button_pressed(uint16_t id)
	{
		switch (id)
		{
		case Buttons::BT_CLOSE:
			toggle_active();
			break;
		case Buttons::BT_SHOP:
			EnterCashShopPacket().dispatch();
			LOG(LOG_DEBUG, "UIEquipInventory: EnterCashShop");
			return Button::State::NORMAL;
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
				icon->start_drag_at(position, cursorpos);

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
		// 命中区 = 当前显示尺寸（收起 = backgrnd，展开 = 含宠物面板的并集），
		// 与 dimension 同源；旧 totem 分支用的 EquipGL/Totem 美术在本包已不加载。
		Rectangle<int16_t> bounds = Rectangle<int16_t>(position, position + dimension);

		return bounds.contains(cursorpos);
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
		EquipSlot::Id unoccupied = EquipSlot::Id::NONE;

		for (auto iter : iconpositions)
		{
			if (iter.second.x() == 0 && iter.second.y() == 0)
				continue;

			// 命中区与绘制同源：有图标的格直接问 Icon 自己刚被画出去的那格；
			// 空格也要能被认出（拖放目标、双手武器共用格），所以回退到同一把尺
			// ——格左上角 = iconpositions，格大小 = Icon 本格（底图凹槽 32x33 里的 32）。
			const Icon* occupied = icons[iter.first].get();
			Rectangle<int16_t> iconrect = occupied
				? occupied->rect(position)
				: Rectangle<int16_t>(position + iter.second,
					position + iter.second + Point<int16_t>(Icon::cell_width, Icon::cell_height));

			if (!iconrect.contains(cursorpos))
				continue;

			// 双手武器同时占 WEAPON 与 SUBWEAPON 两格（(137,131) 是共用格，原生语义），
			// 取槽时必须先给「这一格真有装备」的那个 id，否则 tooltip 会串到空的另一 id 上。
			if (icons[iter.first])
				return iter.first;

			if (unoccupied == EquipSlot::Id::NONE)
				unoccupied = iter.first;
		}

		return unoccupied;
	}

	void UIEquipInventory::build_disabled_cells()
	{
		if (!backgrnd_tex.is_valid())
			return;

		std::vector<TintedRect> rects;

		rects.reserve(TINTABLE_SLOTS.size());

		for (EquipSlot::Id slot : TINTABLE_SLOTS)
		{
			Point<int16_t> ip = iconpositions[slot];

			// 凹槽相对 iconpositions 下移 3px（backgrnd 上实测的改色带是 y=233..264，
			// 而格顶在 y=230），格 32x32。
			rects.push_back(TintedRect{ ip.x(), static_cast<int16_t>(ip.y() + 3), 32, 32 });
		}

		if (!build_tinted_regions(backgrnd_tex, rects, disabled_cells))
		{
			disabled_cells.clear();
			UiAssetResolve::diag_once("equip tint cells", "Equip/backgrnd pixel copy failed -> tint disabled");
		}
	}

	bool UIEquipInventory::slot_available(EquipSlot::Id slot) const
	{
		// 只有拿到运行期判据的格子才参与改色；其余一律返回 true（保持底图原样），
		// 判据缺口见 evidence/r27-b3-equip/README.md §7 —— 不做「看着像就写死」的格子表。
		switch (slot)
		{
		case EquipSlot::Id::TAMEDMOB:
		{
			// 骑兽技能（0001004，invisible=1，max 1）：没学过 → 骑兽格不可用。
			return Stage::get().get_player().get_skilllevel(TAMING_SKILL) > 0;
		}
		case EquipSlot::Id::SADDLE:
		{
			// 鞍子挂在已装备的骑兽上：没装备骑兽 → 鞍子格不可用。
			return inventory.get_item_id(InventoryType::Id::EQUIPPED, EquipSlot::Id::TAMEDMOB) != 0;
		}
		case EquipSlot::Id::SHIELD:
		{
			// 双手武器占手且原生不给盾牌位：Char::is_twohanded() 判的是已装备集合。
			return !Stage::get().get_player().is_twohanded();
		}
		default:
			return true;
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