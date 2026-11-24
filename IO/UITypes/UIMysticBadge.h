//////////////////////////////////////////////////////////////////////////////////
//	UIMysticBadge — 神秘徽章装备栏（2026-11-22 native-engine-full-parity）
//
//	底板：UIWindow.img/MysticBadge/backgrnd（由 assets/ui/mystic_badge/panel_base.png
//	经 orange-wz set_png 入库，187×340）。
//
//	版式（assets/ui/mystic_badge/README.md）：
//	  槽 0..2 上排 (18,149)/(68,149)/(118,149) 约 48×82
//	  槽 3..5 下排 (18,239)/(68,239)/(118,239)
//
//	wire：MYSTIC1..6 = -70..-75（见 EquipSlot.h）。
//	定位：跟随 UIEquipInventory；顶对齐横排最左（神秘|机器人|角色|辅助）。
//	锁图标：assets 有 lock.png，但 panel_base 默认无锁、无「200级可解锁」文案；
//	无官方 Mb 解锁协议证据前不画锁、不弹限制提示。
////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Character/Inventory/Inventory.h"
#include "../../Character/Look/EquipSlot.h"
#include "../../Graphics/Texture.h"

#include <array>
#include <memory>

namespace ms
{
	class UIMysticBadge : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MYSTICBADGE;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIMysticBadge(const Inventory& inventory);

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
		static constexpr size_t SLOT_COUNT = 6;

		static constexpr std::array<EquipSlot::Id, SLOT_COUNT> SLOTS = {
			EquipSlot::Id::MYSTIC1,
			EquipSlot::Id::MYSTIC2,
			EquipSlot::Id::MYSTIC3,
			EquipSlot::Id::MYSTIC4,
			EquipSlot::Id::MYSTIC5,
			EquipSlot::Id::MYSTIC6
		};

		static constexpr std::array<Point<int16_t>, SLOT_COUNT> CELLS = {
			Point<int16_t>(18, 149), Point<int16_t>(68, 149), Point<int16_t>(118, 149),
			Point<int16_t>(18, 239), Point<int16_t>(68, 239), Point<int16_t>(118, 239)
		};

		static constexpr int16_t CELL_W = 48;
		static constexpr int16_t CELL_H = 82;

		class MysticIcon : public Icon::Type
		{
		public:
			MysticIcon(EquipSlot::Id source);

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
