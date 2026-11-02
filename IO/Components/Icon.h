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
#pragma once

#include "../../Graphics/Texture.h"

#include "../../Character/Inventory/Inventory.h"
#include "../../Template/Rectangle.h"

#include <memory>

namespace ms
{
	class Icon
	{
	public:
		enum IconType : uint8_t
		{
			NONE,
			SKILL,
			EQUIP,
			ITEM,
			KEY,
			NUM_TYPES
		};

		class Type
		{
		public:
			virtual ~Type() {}

			virtual void drop_on_stage() const = 0;
			virtual void drop_on_equips(EquipSlot::Id eqslot) const = 0;
			virtual bool drop_on_items(InventoryType::Id tab, EquipSlot::Id eqslot, int16_t slot, bool equip) const = 0;
			virtual void drop_on_bindings(Point<int16_t> cursorposition, bool remove) const = 0;
			virtual void set_count(int16_t) = 0;
			virtual IconType get_type() = 0;
			// Optional inventory identity for drop targets (Hammer / Vega / etc.)
			virtual int16_t get_inventory_slot() const { return 0; }
			virtual int32_t get_item_id() const { return 0; }
			virtual InventoryType::Id get_inventory_tab() const { return InventoryType::Id::NONE; }
			virtual int32_t get_skill_id() const { return 0; }
		};

		class NullType : public Type
		{
			void drop_on_stage() const override {}
			void drop_on_equips(EquipSlot::Id) const override {}
			bool drop_on_items(InventoryType::Id, EquipSlot::Id, int16_t, bool) const override { return true; }
			void drop_on_bindings(Point<int16_t>, bool) const override {}
			void set_count(int16_t) override {}
			IconType get_type() override { return IconType::NONE; }
		};

		Icon();
		Icon(std::unique_ptr<Type> type, Texture texture, int16_t count);

		void drop_on_stage() const;
		void drop_on_equips(EquipSlot::Id eqslot) const;
		bool drop_on_items(InventoryType::Id tab, EquipSlot::Id eqslot, int16_t slot, bool equip) const;
		void drop_on_bindings(Point<int16_t> cursorposition, bool remove) const;
		void set_count(int16_t count);
		IconType get_type();
		int16_t get_inventory_slot() const;
		int32_t get_item_id() const;
		InventoryType::Id get_inventory_tab() const;
		int32_t get_skill_id() const;

		void draw(Point<int16_t> position) const;
		void dragdraw(Point<int16_t> cursorpos) const;
		void start_drag(Point<int16_t> offset);
		void reset();
		int16_t get_count() const;
		bool get_drag();

		//———————————————— 本格几何（R27-B3-4）————————————————
		// 原生图标只有一种落法：画在哪一格，点哪一格就算命中，抓起来就按那一点偏移。
		// 上游的 Icon 不带任何几何，持有者各写各的：绘制点一个式子、命中矩形另一个式子、
		// 拖拽偏移第三个式子 —— 本仓库里这三处实测分叉过三回（装备窗 +4,+4；
		// 键盘设定 -adjust；技能窗 SKILL_META_OFFSET），而且「键盘设定按键图偏上」
		// 就是绘制点单独往下修留下的痕迹。
		// 现在几何归 Icon 所有：持有者的绘制循环在画之前 set_cell 一次（窗口相对的
		// 格左上角），draw_at / rect / contains / start_drag_at 全部从它推导，
		// 所以「画在哪」和「点哪算」在结构上不可能再分叉。
		// 单一写入者 = 绘制循环；读取者 = send_cursor / doubleclick / send_icon。
		// 一个图标画在两格（键盘设定的 Ctrl/Alt/Shift）时，抓取偏移用带格子的
		// 显式重载，别依赖「最后一次 set_cell 留下的那格」。
		// 命名用 cell 而不是 slot：持有者侧的 `slot` 一律是**槽号**（int16_t），
		// 而这里存的是**格左上角**（Point），装备/背包窗口里两者同框出现，不能混。
		static constexpr int16_t cell_width = 32;
		static constexpr int16_t cell_height = 32;

		void set_cell(Point<int16_t> cell_position);
		// parent = 持有者窗口的 position；返回绝对命中矩形。
		Rectangle<int16_t> rect(Point<int16_t> parent) const;
		bool contains(Point<int16_t> parent, Point<int16_t> cursor) const;
		void draw_at(Point<int16_t> parent) const;
		void start_drag_at(Point<int16_t> parent, Point<int16_t> cursor);
		void start_drag_at(Point<int16_t> parent, Point<int16_t> cell_position, Point<int16_t> cursor);

	private:
		std::unique_ptr<Type> type;
		bool showcount;
		int16_t count;

		Texture texture;
		bool dragged;
		Point<int16_t> cursoroffset;
		Point<int16_t> cell;
	};
}