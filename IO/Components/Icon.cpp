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
#include "Icon.h"

#include "Charset.h"

#include "../../Audio/Audio.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	Icon::Icon() : Icon(std::make_unique<NullType>(), {}, -1) {}

	Icon::Icon(std::unique_ptr<Type> type, Texture t, int16_t c) : type(std::move(type)), texture(t), count(c)
	{
		// 物品/装备图标落位修正：北斗 .nxcache 的 item/equip 图标贴图实际可见内容
		// 相对格顶整体下移一格（32px = Icon::cell_height）。旧实现曾用 texture.shift(0,32)
		// 抵消，移除后物品/装备图标整体偏上一格（命中区 get_slotpos 不受影响，故悬浮
		// 原位仍正确显示物品信息）。这里仅对 ITEM/EQUIP 类型恢复该下移；技能走
		// StatefulIcon（SKILL）等其它类型不应用，以免三态贴图与基类锚点错开。
		if (this->type && (this->type->get_type() == IconType::ITEM || this->type->get_type() == IconType::EQUIP))
			texture.shift(Point<int16_t>(0, 32));

		showcount = count > -1;
		dragged = false;
	}

	void Icon::draw(Point<int16_t> position) const
	{
		float opacity = dragged ? 0.5f : 1.0f;
		texture.draw(DrawArgument(position, opacity));

		if (showcount)
		{
			static const Charset countset = Charset(nl::nx::UI["Basic.img"]["ItemNo"], Charset::Alignment::LEFT);
			countset.draw(std::to_string(count), position + Point<int16_t>(0, 20));
		}
	}

	void Icon::dragdraw(Point<int16_t> cursorpos) const
	{
		if (dragged)
			texture.draw(DrawArgument(cursorpos - cursoroffset, 0.5f));
	}

	void Icon::drop_on_stage() const
	{
		type->drop_on_stage();
	}

	void Icon::drop_on_equips(EquipSlot::Id eqslot) const
	{
		type->drop_on_equips(eqslot);
	}

	bool Icon::drop_on_items(InventoryType::Id tab, EquipSlot::Id eqslot, int16_t slot, bool equip) const
	{
		bool remove_icon = type->drop_on_items(tab, eqslot, slot, equip);

		if (remove_icon)
			Sound(Sound::Name::DRAGEND).play();

		return remove_icon;
	}

	void Icon::drop_on_bindings(Point<int16_t> cursorposition, bool remove) const
	{
		type->drop_on_bindings(cursorposition, remove);
	}

	void Icon::start_drag(Point<int16_t> offset)
	{
		cursoroffset = offset;
		dragged = true;

		Sound(Sound::Name::DRAGSTART).play();
	}

	void Icon::set_cell(Point<int16_t> cell_position)
	{
		cell = cell_position;
	}

	Rectangle<int16_t> Icon::rect(Point<int16_t> parent) const
	{
		Point<int16_t> left_top = parent + cell;

		return Rectangle<int16_t>(left_top, left_top + Point<int16_t>(cell_width, cell_height));
	}

	bool Icon::contains(Point<int16_t> parent, Point<int16_t> cursor) const
	{
		return rect(parent).contains(cursor);
	}

	void Icon::draw_at(Point<int16_t> parent) const
	{
		draw(parent + cell);
	}

	void Icon::start_drag_at(Point<int16_t> parent, Point<int16_t> cursor)
	{
		start_drag_at(parent, cell, cursor);
	}

	void Icon::start_drag_at(Point<int16_t> parent, Point<int16_t> cell_position, Point<int16_t> cursor)
	{
		// 抓取偏移 = 光标与「本格左上角」的差 ⇒ 拖起来之后图标跟手，不会在按下瞬间跳一格。
		start_drag(cursor - parent - cell_position);
	}

	void Icon::reset()
	{
		dragged = false;
	}

	void Icon::set_count(int16_t c)
	{
		count = c;

		type->set_count(count);
	}

	Icon::IconType Icon::get_type()
	{
		return type->get_type();
	}

	int16_t Icon::get_inventory_slot() const
	{
		return type ? type->get_inventory_slot() : 0;
	}

	int32_t Icon::get_item_id() const
	{
		return type ? type->get_item_id() : 0;
	}

	InventoryType::Id Icon::get_inventory_tab() const
	{
		return type ? type->get_inventory_tab() : InventoryType::Id::NONE;
	}

	int32_t Icon::get_skill_id() const
	{
		return type ? type->get_skill_id() : 0;
	}

	int16_t Icon::get_count() const
	{
		return count;
	}

	bool Icon::get_drag()
	{
		return dragged;
	}
}