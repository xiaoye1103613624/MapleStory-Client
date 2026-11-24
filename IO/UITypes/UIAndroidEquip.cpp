//////////////////////////////////////////////////////////////////////////////////
//	UIAndroidEquip — 192 原生机器人栏
//////////////////////////////////////////////////////////////////////////////////
#include "UIAndroidEquip.h"

#include "../UI.h"
#include "UIEquipInventory.h"
#include "UIItemInventory.h"

#include "../../Audio/Audio.h"
#include "../../Data/ItemData.h"
#include "../../Gameplay/Stage.h"
#include "../../Graphics/DrawArgument.h"
#include "../../Net/Packets/InventoryPackets.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <string>

namespace ms
{
	UIAndroidEquip::UIAndroidEquip(const Inventory& invent)
		: UIElement(Point<int16_t>(0, 0), Point<int16_t>(0, 0), true), inventory(invent)
	{
		set_layout_policy(LayoutPolicy::None);

		nl::node native = UiAssetResolve::equip_native_root();
		nl::node src = native["Android"];
		nl::node shell_n = native["backgrnd2"];
		nl::node bg = src["backgrnd"];

		// Android/backgrnd 是低透明人影叠层（与 192 同源），须落在 Equip/backgrnd2 色板上。
		if (!UiAssetResolve::usable(src) || !UiAssetResolve::usable(bg)
			|| !UiAssetResolve::usable(shell_n))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		shell = Texture(shell_n);
		backgrnd = Texture(bg);
		const Point<int16_t> shell_o = shell.get_origin();
		const Point<int16_t> bg_o = backgrnd.get_origin();
		dimension = shell.get_dimensions();

		// 原生同窗原点系：shell_tl=P-shell_o，android_tl=P-bg_o → 相对色板 TL 偏移。
		overlay_off = Point<int16_t>(
			static_cast<int16_t>(-bg_o.x() + shell_o.x()),
			static_cast<int16_t>(-bg_o.y() + shell_o.y())
		);

		// DrawArgument(origin) 钉 TL：色板 TL=position，叠层 TL=position+overlay_off。
		sprites.emplace_back(shell_n, DrawArgument(shell_o));
		sprites.emplace_back(bg, DrawArgument(overlay_off + bg_o));

		nl::node slots_node = src["Slots"];

		for (size_t i = 0; i < ANDROID_COUNT; ++i)
		{
			const int16_t bp = static_cast<int16_t>(1200 + i);
			nl::node slotn = slots_node[std::to_string(bp)];
			slot_frames[i] = Texture(slotn);
			const Point<int16_t> so = slot_frames[i].get_origin();
			// 槽位相对色板 TL = overlay_off + (android 局部格点)
			cells[i] = Point<int16_t>(
				static_cast<int16_t>(overlay_off.x() - so.x() + bg_o.x()),
				static_cast<int16_t>(overlay_off.y() - so.y() + bg_o.y())
			);
		}

		// 心脏：色板底栏左侧空格（Addon 分流；192 主栏 35 水印贴近机器人区）。
		cells[HEART_INDEX] = Point<int16_t>(
			12,
			static_cast<int16_t>(dimension.y() - CELL_H - 8)
		);

		load_icons();
	}

	void UIAndroidEquip::draw(float alpha) const
	{
		draw_sprites(alpha);

		// 192/live：Slots/1200..1205 多为 1×1 原点锚；仅 >1×1 的画布（如 1206=40×39）绘制。
		for (size_t i = 0; i < ANDROID_COUNT; ++i)
		{
			if (!slot_frames[i].is_valid())
				continue;

			const Point<int16_t> dim = slot_frames[i].get_dimensions();

			if (dim.x() <= 1 && dim.y() <= 1)
				continue;

			slot_frames[i].draw(DrawArgument(position + cells[i], alpha * fade_alpha()));
		}

		for (size_t i = 0; i < SLOT_COUNT; ++i)
		{
			if (!icons[i])
				continue;

			icons[i]->set_cell(cells[i]);
			icons[i]->draw_at(position);
		}

		draw_buttons(alpha);
	}

	void UIAndroidEquip::load_icons()
	{
		for (auto& icon : icons)
			icon.reset();

		for (EquipSlot::Id slot : SLOTS)
			update_slot(slot);
	}

	void UIAndroidEquip::update_slot(EquipSlot::Id slot)
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
				std::make_unique<AndroidIcon>(slot),
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

	void UIAndroidEquip::modify(int16_t pos, int8_t mode, int16_t arg)
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

	EquipSlot::Id UIAndroidEquip::slot_by_position(Point<int16_t> cursorpos) const
	{
		for (size_t i = 0; i < SLOT_COUNT; ++i)
		{
			Point<int16_t> lt = position + cells[i];
			Rectangle<int16_t> rect(lt, lt + Point<int16_t>(CELL_W, CELL_H));

			if (rect.contains(cursorpos))
				return SLOTS[i];
		}

		return EquipSlot::Id::NONE;
	}

	Cursor::State UIAndroidEquip::send_cursor(bool pressed, Point<int16_t> cursorpos)
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

	void UIAndroidEquip::close_with_host()
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

	void UIAndroidEquip::send_key(int32_t keycode, bool pressed, bool escape)
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

	void UIAndroidEquip::doubleclick(Point<int16_t> cursorpos)
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

	bool UIAndroidEquip::is_in_range(Point<int16_t> cursorpos) const
	{
		return Rectangle<int16_t>(position, position + dimension).contains(cursorpos);
	}

	bool UIAndroidEquip::send_icon(const Icon& icon, Point<int16_t> cursorpos)
	{
		if (EquipSlot::Id slot = slot_by_position(cursorpos))
			icon.drop_on_equips(slot);

		return true;
	}

	void UIAndroidEquip::toggle_active()
	{
		if (auto host = UI::get().get_element<UIEquipInventory>())
		{
			host->toggle_active();
			return;
		}

		clear_tooltip();
		UIElement::toggle_active();
	}

	void UIAndroidEquip::show_equip(EquipSlot::Id slot)
	{
		UI::get().show_equip(Tooltip::Parent::EQUIPINVENTORY, slot);
	}

	void UIAndroidEquip::clear_tooltip()
	{
		UI::get().clear_tooltip(Tooltip::Parent::EQUIPINVENTORY);
	}

	Button::State UIAndroidEquip::button_pressed(uint16_t buttonid)
	{
		if (buttonid == 0)
			close_with_host();

		return Button::State::NORMAL;
	}

	UIElement::Type UIAndroidEquip::get_type() const
	{
		return TYPE;
	}

	UIAndroidEquip::AndroidIcon::AndroidIcon(EquipSlot::Id s) : source(s) {}

	void UIAndroidEquip::AndroidIcon::drop_on_equips(EquipSlot::Id slot) const
	{
		if (source == slot)
			Sound(Sound::Name::DRAGEND).play();
	}

	bool UIAndroidEquip::AndroidIcon::drop_on_items(InventoryType::Id tab, EquipSlot::Id eqslot, int16_t slot, bool equip) const
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

	Icon::IconType UIAndroidEquip::AndroidIcon::get_type()
	{
		return Icon::IconType::EQUIP;
	}
}
