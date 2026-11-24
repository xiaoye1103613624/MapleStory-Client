//////////////////////////////////////////////////////////////////////////////////
//	UITotemEquip — 192 Equip/totem 三格图腾窗（侧栏）
//
//	资源：UIWindow4.img/Equip/totem/backgrnd（141×70 ARGB4444；orange-wz 实证）。
//	槽位：TOTEM1..3 = BP55–57；TOTEM4 废弃新穿（本窗不绑第 4 格）。
//	定位：跟随 UIEquipInventory，贴 base_dimension 右缘（原 Addon 位）。
//	开/关由宿主 sync_side_panels 驱动；D-TOTEM4 / D-ADDON 锁定后取代 UIAddonEquip。
////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/Texture.h"

#include <array>
#include <memory>

namespace ms
{
	class UITotemEquip : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::TOTEMEQUIP;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UITotemEquip(const Inventory& inventory);

		void draw(float inter) const override;
		void toggle_active() override;
		bool send_icon(const Icon& icon, Point<int16_t> position) override;
		void doubleclick(Point<int16_t> position) override;
		bool is_in_range(Point<int16_t> cursorpos) const override;
		Cursor::State send_cursor(bool pressed, Point<int16_t> position) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void modify(int16_t pos, int8_t mode, int16_t arg);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		static constexpr size_t SLOT_COUNT = 3;

		static constexpr std::array<EquipSlot::Id, SLOT_COUNT> SLOTS = {
			EquipSlot::Id::TOTEM1,
			EquipSlot::Id::TOTEM2,
			EquipSlot::Id::TOTEM3
		};

		// 141×70 三格：步进 40，顶区留给「图腾」水印行。
		static constexpr std::array<Point<int16_t>, SLOT_COUNT> CELLS = {
			Point<int16_t>(11, 28),
			Point<int16_t>(51, 28),
			Point<int16_t>(91, 28)
		};

		static constexpr int16_t CELL_W = 39;
		static constexpr int16_t CELL_H = 38;

		class TotemIcon : public Icon::Type
		{
		public:
			TotemIcon(EquipSlot::Id source);

			void drop_on_stage() const override {}
			void drop_on_equips(EquipSlot::Id slot) const override;
			bool drop_on_items(InventoryType::Id tab, EquipSlot::Id eqslot, int16_t slot, bool equip) const override;
			void drop_on_bindings(Point<int16_t>, bool) const override {}
			void set_count(int16_t) override {}
			Icon::IconType get_type() override;

		private:
			EquipSlot::Id source;
		};

		void load_icons();
		void update_slot(EquipSlot::Id slot);
		EquipSlot::Id slot_by_position(Point<int16_t> position) const;
		void show_equip(EquipSlot::Id slot);
		void clear_tooltip();
		void close_with_host();

		const Inventory& inventory;
		Texture backgrnd;
		std::array<std::unique_ptr<Icon>, SLOT_COUNT> icons;
		EquipSlot::Id selected_slot = EquipSlot::Id::NONE;
	};
}
