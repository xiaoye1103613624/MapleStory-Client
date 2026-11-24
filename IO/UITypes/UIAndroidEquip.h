//////////////////////////////////////////////////////////////////////////////////
//	UIAndroidEquip — 192 原生机器人栏（侧栏形态）
//
//	证据：Angel CAndroidTab::InitEquipItem @0x1202630；槽位名 = BodyPart 1200–1206。
//	资源：Equip/backgrnd2（页签色板）+ Equip/Android/backgrnd（人影叠层，非独立窗框）。
//	定位：跟随 UIEquipInventory，顶对齐横排第二位（神秘 | 本窗 | 角色 | 图腾）。
//	心脏 HEART(61)：从 Addon 分流到本窗底栏空格（192 主栏 35 水印贴近 Android 区）。
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
	class UIAndroidEquip : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::ANDROIDEQUIP;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIAndroidEquip(const Inventory& inventory);

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
		// 0..6 = Android 1200–1206；7 = HEART(61) 底栏分流格。
		static constexpr size_t ANDROID_COUNT = 7;
		static constexpr size_t SLOT_COUNT = 8;
		static constexpr size_t HEART_INDEX = 7;

		static constexpr std::array<EquipSlot::Id, SLOT_COUNT> SLOTS = {
			EquipSlot::Id::ANDROID1,
			EquipSlot::Id::ANDROID2,
			EquipSlot::Id::ANDROID3,
			EquipSlot::Id::ANDROID4,
			EquipSlot::Id::ANDROID5,
			EquipSlot::Id::ANDROID6,
			EquipSlot::Id::ANDROID7,
			EquipSlot::Id::HEART
		};

		static constexpr int16_t CELL_W = 40;
		static constexpr int16_t CELL_H = 39;

		class AndroidIcon : public Icon::Type
		{
		public:
			AndroidIcon(EquipSlot::Id source);

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
		Texture shell;			// Equip/backgrnd2
		Texture backgrnd;		// Android/backgrnd 叠层
		Point<int16_t> overlay_off;	// 叠层相对色板 TL
		std::array<Point<int16_t>, SLOT_COUNT> cells{};
		std::array<Texture, SLOT_COUNT> slot_frames;	// Slots/%d；1×1 锚点跳过绘制
		std::array<std::unique_ptr<Icon>, SLOT_COUNT> icons;
		EquipSlot::Id selected_slot = EquipSlot::Id::NONE;
	};
}
