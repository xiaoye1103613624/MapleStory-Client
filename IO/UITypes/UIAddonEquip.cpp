//////////////////////////////////////////////////////////////////////////////////
//	UIAddonEquip — 扩展（辅助）装备独立子窗（2026-11-12 装备槽位扩展 ④）
//////////////////////////////////////////////////////////////////////////////////
#include "UIAddonEquip.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "UIItemInventory.h"

#include "../../Audio/Audio.h"
#include "../../Data/ItemData.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/InventoryPackets.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIAddonEquip::UIAddonEquip(const Inventory& invent)
		: UIDragElement<PosEQINV>(), inventory(invent)
	{
		// 底板经 orange-wz MCP 从 docs/features/addon-equipment/ui/window/v5/normal.png
		// 入库为 UIWindow.img/AddonEquip/backgrnd（152×105 ARGB4444）。
		// 节点缺失时优雅降级：不创建窗、不落 (0,0) 空壳。
		nl::node src = UiAssetResolve::window4("AddonEquip");

		if (!UiAssetResolve::usable(src))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		nl::node bg = src["backgrnd"];

		if (!UiAssetResolve::usable(bg))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		backgrnd = Texture(bg);
		bg_origin = backgrnd.get_origin();
		dimension = backgrnd.get_dimensions();
		dragarea = Point<int16_t>(dimension.x(), 20);

		if (bg)
			sprites.emplace_back(bg);

		load_icons();
	}

	void UIAddonEquip::draw(float alpha) const
	{
		draw_sprites(alpha);

		for (size_t i = 0; i < SLOT_COUNT; ++i)
		{
			if (!icons[i])
				continue;

			// 与 slot_by_position() 同源：格左上角 = CELLS[i]，格大小 = CELL_W×CELL_H。
			// 图标自身 texture origin 由 DrawArgument 负责补偿。
			icons[i]->set_cell(CELLS[i]);
			icons[i]->draw_at(position);
		}
	}

	void UIAddonEquip::load_icons()
	{
		for (auto& icon : icons)
			icon.reset();

		for (EquipSlot::Id slot : SLOTS)
			update_slot(slot);
	}

	void UIAddonEquip::update_slot(EquipSlot::Id slot)
	{
		size_t index = 0;

		while (index < SLOT_COUNT && SLOTS[index] != slot)
			++index;

		if (index >= SLOT_COUNT)
			return;

		if (int32_t item_id = inventory.get_item_id(InventoryType::Id::EQUIPPED, slot))
		{
			const Texture& texture = ItemData::get(item_id).get_icon(false);

			icons[index] = std::make_unique<Icon>(
				std::make_unique<AddonIcon>(slot),
				texture,
				-1
				);
		}
		else
		{
			icons[index].reset();
		}

		clear_tooltip();
	}

	void UIAddonEquip::modify(int16_t pos, int8_t mode, int16_t arg)
	{
		// pos/arg 是 wire BodyPart 绝对值（下行包字段），先还原成逻辑槽位。
		EquipSlot::Id eqpos = EquipSlot::from_wire_bodypart(pos);
		EquipSlot::Id eqarg = EquipSlot::from_wire_bodypart(arg);

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
		default:
			break;
		}
	}

	EquipSlot::Id UIAddonEquip::slot_by_position(Point<int16_t> cursorpos) const
	{
		for (size_t i = 0; i < SLOT_COUNT; ++i)
		{
			Point<int16_t> lt = position + CELLS[i];
			Rectangle<int16_t> rect(lt, lt + Point<int16_t>(CELL_W, CELL_H));

			if (rect.contains(cursorpos))
				return SLOTS[i];
		}

		return EquipSlot::Id::NONE;
	}

	Cursor::State UIAddonEquip::send_cursor(bool pressed, Point<int16_t> cursorpos)
	{
		Cursor::State dstate = UIDragElement::send_cursor(pressed, cursorpos);

		if (dragged)
		{
			clear_tooltip();
			return dstate;
		}

		EquipSlot::Id slot = slot_by_position(cursorpos);

		if (slot == EquipSlot::Id::NONE)
		{
			clear_tooltip();
			return Cursor::State::IDLE;
		}

		selected_slot = slot;

		size_t index = 0;

		while (index < SLOT_COUNT && SLOTS[index] != slot)
			++index;

		if (index >= SLOT_COUNT)
			return Cursor::State::IDLE;

		if (auto icon = icons[index].get())
		{
			if (pressed)
			{
				icon->start_drag_at(position, cursorpos);
				UI::get().drag_icon(icon);
				clear_tooltip();
				return Cursor::State::GRABBING;
			}

			show_equip(slot);
			return Cursor::State::CANGRAB;
		}

		clear_tooltip();
		return Cursor::State::IDLE;
	}

	void UIAddonEquip::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed)
			return;

		if (escape)
		{
			toggle_active();
			return;
		}

		// 与 UIEquipInventory 一致：U/S 卸下选中槽，I 开背包。
		if (keycode == 'U' || keycode == 'u' || keycode == 'S' || keycode == 's')
		{
			if (selected_slot != EquipSlot::Id::NONE
				&& inventory.get_item_id(InventoryType::Id::EQUIPPED, selected_slot) > 0)
			{
				if (int16_t freeslot = inventory.find_free_slot(InventoryType::Id::EQUIP))
					UnequipItemPacket(selected_slot, freeslot).dispatch();
			}
		}
		else if (keycode == 'I' || keycode == 'i')
		{
			if (auto ui = UI::get().get_element<UIItemInventory>())
			{
				if (!ui->is_active())
					ui->makeactive();
			}
		}
	}

	void UIAddonEquip::doubleclick(Point<int16_t> cursorpos)
	{
		EquipSlot::Id slot = slot_by_position(cursorpos);

		if (slot == EquipSlot::Id::NONE)
			return;

		size_t index = 0;

		while (index < SLOT_COUNT && SLOTS[index] != slot)
			++index;

		if (index < SLOT_COUNT && icons[index])
			if (int16_t freeslot = inventory.find_free_slot(InventoryType::Id::EQUIP))
				UnequipItemPacket(slot, freeslot).dispatch();
	}

	bool UIAddonEquip::is_in_range(Point<int16_t> cursorpos) const
	{
		return Rectangle<int16_t>(position, position + dimension).contains(cursorpos);
	}

	bool UIAddonEquip::send_icon(const Icon& icon, Point<int16_t> cursorpos)
	{
		if (EquipSlot::Id slot = slot_by_position(cursorpos))
			icon.drop_on_equips(slot);

		return true;
	}

	void UIAddonEquip::toggle_active()
	{
		clear_tooltip();
		UIElement::toggle_active();
	}

	void UIAddonEquip::show_equip(EquipSlot::Id slot)
	{
		UI::get().show_equip(Tooltip::Parent::EQUIPINVENTORY, slot);
	}

	void UIAddonEquip::clear_tooltip()
	{
		UI::get().clear_tooltip(Tooltip::Parent::EQUIPINVENTORY);
	}

	Button::State UIAddonEquip::button_pressed(uint16_t buttonid)
	{
		if (buttonid == 0)
			toggle_active();

		return Button::State::NORMAL;
	}

	UIElement::Type UIAddonEquip::get_type() const
	{
		return TYPE;
	}

	UIAddonEquip::AddonIcon::AddonIcon(EquipSlot::Id s) : source(s) {}

	void UIAddonEquip::AddonIcon::drop_on_equips(EquipSlot::Id slot) const
	{
		// 从本窗拖到主装备窗（或本窗另一格）：语义由 UIItemInventory 的
		// ItemIcon::drop_on_items 处理，这里只给音效反馈。
		if (source == slot)
			Sound(Sound::Name::DRAGEND).play();
	}

	bool UIAddonEquip::AddonIcon::drop_on_items(InventoryType::Id tab, EquipSlot::Id eqslot, int16_t slot, bool equip) const
	{
		// 与 UIEquipInventory::EquipIcon 同语义：
		//   equip=true  → 从背包拖进本窗槽（eqslot），发 EquipItemPacket；
		//   equip=false → 从本窗拖回背包，发 UnequipItemPacket。
		if (tab != InventoryType::Id::EQUIP)
			return false;

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

	Icon::IconType UIAddonEquip::AddonIcon::get_type()
	{
		return Icon::IconType::EQUIP;
	}
}
