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
			// Angel CUIMacroSysEx::OnMouseButton v43[8]=row：整行宏图标携带宏下标，
			// 拖到快捷格/键位窗时据此构造 KeyType::MACRO 绑定（get_skill_id()==0 的 KEY 型图标）。
			virtual int32_t get_macro_index() const { return -1; }
		// 原生快捷栏「自带功能键」(MENU/ACTION/FACE) 拖拽身份：
		// 技能/道具图标分别由 get_skill_id()/get_item_id() 表达落点绑定，功能键没有这三者，
		// 需一条独立通道把 (KeyType, KeyAction) 透给状态栏 send_icon 完成换位绑定。
		// 带默认实现的虚方法（而非纯虚）：装备/背包/技能等其它 Type 无需改动，
		// 穿戴/卸下装备链路零影响；只有 QsIconType 按需 override。
		// 返回 true 表示 out_type/out_action 有效。
		virtual bool get_binding(uint8_t& out_type, int32_t& out_action) const
		{
			(void)out_type;
			(void)out_action;
			return false;
		}
			// Angel CUIMacroSysEx::OnMouseButton 的 BeginDragDrop 载荷带源坐标；
			// 组合栏内部抓起的图标需要让 UIStateGame 在松手时无条件结算（移动/移除/绑键）。
			virtual bool is_macro_source() const { return false; }
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
		IconType get_type() const;
		int16_t get_inventory_slot() const;
		int32_t get_item_id() const;
		InventoryType::Id get_inventory_tab() const;
		int32_t get_skill_id() const;
		// 见 Type::get_binding：功能键拖拽载荷透出 (KeyType, KeyAction)。
		bool get_binding(uint8_t& out_type, int32_t& out_action) const;
		int32_t get_macro_index() const { return type ? type->get_macro_index() : -1; }
		bool is_macro_source() const { return type && type->is_macro_source(); }

		void draw(Point<int16_t> position) const;
		void dragdraw(Point<int16_t> cursorpos) const;
		void start_drag(Point<int16_t> offset);
		void reset();
		int16_t get_count() const;
		bool get_drag() const;

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

		// 2026-11-22：拖拽落点应取「图标视觉中心」而非原始光标点。
		// dragdraw 画在 cursor-cursoroffset，若命中判定用原始 cursor，则从格子
		// 底部抓起时预览图标在上一格、光标已在下一格（键位窗 PUP/PDN 相邻即复现：
		// 拖到 PUP 却绑到 PDN）。返回预览图标中心，供落点命中统一使用。
		Point<int16_t> get_drag_drop_point(Point<int16_t> cursor) const
		{
			return cursor - cursoroffset + Point<int16_t>(cell_width / 2, cell_height / 2);
		}

	private:
		std::unique_ptr<Type> type;
		bool showcount;
		int16_t count;

		Texture texture;
		bool dragged;
		Point<int16_t> cursoroffset;
		Point<int16_t> cell;
		// 大尺寸贴图（怪物卡 238 的 iconRaw）按格子自适应缩小，渲染时缩放绘制，不改 raw 源。
		// 正常 32x32 图标保持 1.0，行为不变；R27-B3-4 的 cell 几何不受影响。
		float draw_scale = 1.0f;
	};
}