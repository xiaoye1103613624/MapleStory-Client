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
		// 上游在这里做 texture.shift(Point(0, 32))，用于抵消 WZ 图标 origin 的
		// 「锚点在底边」写法；本包（北斗 .nxcache）所有 icon 的 origin 都是小值
		// （实测 Coat/Cap (-2,-1)、Glove (-4,-1)、Weapon (0,0)、Consume (-3,-1)、
		// Skill 全部 (0,0)），再减 32 会把每个图标整体下移 32px。
		// 依据（参考截图 现有道具+tooltip.png e75687e9 逐像素实测，见
		// docs/features/ui-v83-window-restore/evidence/w0-icon/coords.md）：
		// 背包格线 x=32/68/104/140（距 36）、y=46/80/114（距 34），
		// 与 UIItemInventory 回退 slot_pos=(8,50)+步进 36/34 完全对齐（格顶 50 由
		// R27-B2 按展开态 72 颗 Item/disabled 逐像素定案，尺子 = r27-oracle/ItemFull.json）；
		// 白棉T恤图标墨迹左上 = 格左上 + (4,1)，而 -origin = (2,1) ⇒ 原生没有 32px 下移。
		// 所以此处不再 shift，图标左上 = 绘制点 - origin。
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