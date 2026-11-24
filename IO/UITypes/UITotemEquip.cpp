//////////////////////////////////////////////////////////////////////////////////
//	UITotemEquip — 192 Equip/totem 三格图腾窗
//////////////////////////////////////////////////////////////////////////////////
#include "UITotemEquip.h"

#include "../UI.h"
#include "UIEquipInventory.h"
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
	UITotemEquip::UITotemEquip(const Inventory& invent)
		: UIElement(Point<int16_t>(0, 0), Point<int16_t>(0, 0), true), inventory(invent)
	{
		set_layout_policy(LayoutPolicy::None);

		nl::node native = UiAssetResolve::equip_native_root();
		nl::node src = native["totem"];
		nl::node bg = src["backgrnd"];

		if (!UiAssetResolve::usable(src) || !UiAssetResolve::usable(bg))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		backgrnd = Texture(bg);
		dimension = backgrnd.get_dimensions();

		if (bg)
			sprites.emplace_back(bg);

		load_icons();
	}

	void UITotemEquip::draw(float alpha) const
	{
		draw_sprites(alpha);

		for (size_t i = 0; i < SLOT_COUNT; ++i)
		{
			if (!icons[i])
				continue;

			icons[i]->set_cell(CELLS[i]);
			icons[i]->draw_at(position);
		}
	}

	void UITotemEquip::load_icons()
	{
		for (auto& icon : icons)
			icon.reset();

		for (EquipSlot::Id slot : SLOTS)
			update_slot(slot);
	}

	void UITotemEquip::update_slot(EquipSlot::Id slot)
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
				std::make_unique<TotemIcon>(slot),
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

	void UITotemEquip::modify(int16_t pos, int8_t mode, int16_t arg)
	{
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

	EquipSlot::Id UITotemEquip::slot_by_position(Point<int16_t> cursorpos) const
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

	Cursor::State UITotemEquip::send_cursor(bool pressed, Point<int16_t> cursorpos)
	{
		EquipSlot::Id slot = slot_by_position(cursorpos);

		if (slot == EquipSlot::Id::NONE)
		{
			clear_tooltip();
			return UIElement::send_cursor(pressed, cursorpos);
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

	void UITotemEquip::close_with_host()
	{
		clear_tooltip();

		if (auto host = UI::get().get_element<UIEquipInventory>())
		{
			if (host->is_active())
				host->toggle_active();
			return;
		}

		UIElement::deactivate();
	}

	void UITotemEquip::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed)
			return;

		if (escape)
		{
			close_with_host();
			return;
		}

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

	void UITotemEquip::doubleclick(Point<int16_t> cursorpos)
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

	bool UITotemEquip::is_in_range(Point<int16_t> cursorpos) const
	{
		return Rectangle<int16_t>(position, position + dimension).contains(cursorpos);
	}

	bool UITotemEquip::send_icon(const Icon& icon, Point<int16_t> cursorpos)
	{
		if (EquipSlot::Id slot = slot_by_position(cursorpos))
			icon.drop_on_equips(slot);

		return true;
	}

	void UITotemEquip::toggle_active()
	{
		if (auto host = UI::get().get_element<UIEquipInventory>())
		{
			host->toggle_active();
			return;
		}

		clear_tooltip();
		UIElement::toggle_active();
	}

	void UITotemEquip::show_equip(EquipSlot::Id slot)
	{
		UI::get().show_equip(Tooltip::Parent::EQUIPINVENTORY, slot);
	}

	void UITotemEquip::clear_tooltip()
	{
		UI::get().clear_tooltip(Tooltip::Parent::EQUIPINVENTORY);
	}

	Button::State UITotemEquip::button_pressed(uint16_t buttonid)
	{
		if (buttonid == 0)
			close_with_host();

		return Button::State::NORMAL;
	}

	UIElement::Type UITotemEquip::get_type() const
	{
		return TYPE;
	}

	UITotemEquip::TotemIcon::TotemIcon(EquipSlot::Id s) : source(s) {}

	void UITotemEquip::TotemIcon::drop_on_equips(EquipSlot::Id slot) const
	{
		if (source == slot)
			Sound(Sound::Name::DRAGEND).play();
	}

	bool UITotemEquip::TotemIcon::drop_on_items(InventoryType::Id tab, EquipSlot::Id eqslot, int16_t slot, bool equip) const
	{
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

	Icon::IconType UITotemEquip::TotemIcon::get_type()
	{
		return Icon::IconType::EQUIP;
	}
}
