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

#include "../UIDragElement.h"

namespace ms
{
	// The Equip inventory
	class UIEquipInventory : public UIDragElement<PosEQINV>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::EQUIPINVENTORY;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIEquipInventory(const Inventory& inventory);

		void draw(float inter) const override;

		void toggle_active() override;
		bool send_icon(const Icon& icon, Point<int16_t> position) override;

		void doubleclick(Point<int16_t> position) override;
		bool is_in_range(Point<int16_t> cursorpos) const override;
		Cursor::State send_cursor(bool pressed, Point<int16_t> position) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		void modify(int16_t pos, int8_t mode, int16_t arg);
		EquipSlot::Id get_selected_eqslot() const { return selected_eqslot; }

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		void show_equip(EquipSlot::Id slot);
		void clear_tooltip();
		void load_icons();
		void update_slot(EquipSlot::Id slot);
		EquipSlot::Id slot_by_position(Point<int16_t> position) const;
		void change_tab(uint16_t tabid);
		void set_pet_panel(bool open);
		void change_pet_tab(uint16_t tabid);

		class EquipIcon : public Icon::Type
		{
		public:
			EquipIcon(int16_t source);

			void drop_on_stage() const override;
			void drop_on_equips(EquipSlot::Id slot) const override;
			bool drop_on_items(InventoryType::Id tab, EquipSlot::Id eqslot, int16_t slot, bool equip) const override;
			void drop_on_bindings(Point<int16_t>, bool) const override {}
			void set_count(int16_t) override {}
			Icon::IconType get_type() override;

		private:
			int16_t source;
		};

		enum Buttons : uint16_t
		{
			BT_TAB0,
			BT_TAB1,
			BT_TAB2,
			BT_TAB3,
			BT_TABE,
			BT_CLOSE,
			BT_SLOT,
			BT_EFFECT,
			BT_SALON,
			BT_CONSUMESETTING,
			BT_EXCEPTION,
			BT_SHOP,
			BT_PET_SHOW,
			BT_PET_HIDE,
			BT_PET_TAB0,
			BT_PET_TAB1,
			BT_PET_TAB2,
			BT_PET_TABE
		};

		const Inventory& inventory;

		EnumMap<EquipSlot::Id, Point<int16_t>> iconpositions;
		EnumMap<EquipSlot::Id, std::unique_ptr<Icon>> icons;

		uint16_t tab;
		std::string tab_source[Buttons::BT_TABE];
		Texture tabbar;
		Texture background[Buttons::BT_TABE];
		Texture disabled;
		Texture disabled2;
		std::vector<Texture> Slots[Buttons::BT_TABE];

		Point<int16_t> totem_dimensions;
		Point<int16_t> totem_adj;

		bool hasPendantSlot;
		bool hasPocketSlot;
		EquipSlot::Id selected_eqslot = EquipSlot::Id::NONE;

		// v83（classic flat）装备窗只有一个页面、没有 tab 条
		bool classic_flat = false;
		// 宠物装备面板（Equip/pet）显隐 + 溢出后的命中区
		bool pet_panel_open = false;
		// 实测（evidence/w7-equip §7）：参考截图里三页签全是 disabled 美术（灰底灰数字，mae 0.0），
		// 该帧应为「未召唤宠物」态 → 默认不高亮，点击后才把当前页换成 normal 美术（彩底红数字）。
		uint16_t pet_tab = 0xFFFF;
		Texture pet_backgrnd;
		Point<int16_t> pet_panel_pos;
		Point<int16_t> base_dimension;
		Point<int16_t> expanded_dimension;
	};
}