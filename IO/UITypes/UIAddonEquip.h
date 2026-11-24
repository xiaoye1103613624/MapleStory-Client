//////////////////////////////////////////////////////////////////////////////////
//	UIAddonEquip — 扩展（辅助）装备独立子窗（2026-11-12 装备槽位扩展 ④）
//
//	底板：UIWindow.img/AddonEquip/backgrnd（由
//	  docs/features/addon-equipment/ui/window/v5/normal.png 经 orange-wz MCP 入库，
//	  152×105 ARGB4444；文案烤进位图，不再用 Text 重画）。
//
//	版式（v5 manifest，1x）：
//	  上排 图腾×4      (11,32)/(44,32)/(77,32)/(110,32)
//	  下排 纹章/机器人/心脏/徽章 (11,65)/(44,65)/(77,65)/(110,65)
//	  槽位内区约 31×30，水印偏移 (2,2)。
//
//	官方 wire 编号见 docs/features/equip-slot-expansion-2026-11/official-slot-mapping.md：
//	  TOTEM1..4 = -55..-58 / EMBLEM = -59 / ANDROID_SLOT = -60 /
//	  HEART = -61 / BADGE = -54。
//	穿戴包由 EquipItemPacket 统一走 EquipSlot::wire_bodypart() 转换，本类不碰 wire。
//
//	定位：跟随者窗。贴角色装备 base_dimension 右侧、顶边齐（宠物栏上方）；
//	非 192 原生「辅助」面板（192 无 AddonEquip，仅有 Equip/totem 页签）。
//	开/关由宿主 sync_side_panels 驱动。
////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/Texture.h"

#include <array>
#include <memory>

namespace ms
{
	class UIAddonEquip : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::ADDONEQUIP;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIAddonEquip(const Inventory& inventory);

		void draw(float inter) const override;
		void toggle_active() override;
		bool send_icon(const Icon& icon, Point<int16_t> position) override;
		void doubleclick(Point<int16_t> position) override;
		bool is_in_range(Point<int16_t> cursorpos) const override;
		Cursor::State send_cursor(bool pressed, Point<int16_t> position) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		/// 装备栏数据变化时由 UIStateGame 转发（与 UIEquipInventory::modify 同语义）。
		void modify(int16_t pos, int8_t mode, int16_t arg);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		// 机器人已迁 UIAndroidEquip（BP1200）；本窗保留图腾/纹章/心脏/徽章。
		static constexpr size_t SLOT_COUNT = 7;

		static constexpr std::array<EquipSlot::Id, SLOT_COUNT> SLOTS = {
			EquipSlot::Id::TOTEM1,
			EquipSlot::Id::TOTEM2,
			EquipSlot::Id::TOTEM3,
			EquipSlot::Id::TOTEM4,
			EquipSlot::Id::EMBLEM,
			EquipSlot::Id::HEART,
			EquipSlot::Id::BADGE
		};

		/// 位图内格左上角（v5）；(44,65) 原「机器人」水印格空出不绑槽（穿戴走 UIAndroidEquip）。
		static constexpr std::array<Point<int16_t>, SLOT_COUNT> CELLS = {
			Point<int16_t>(11, 32), Point<int16_t>(44, 32),
			Point<int16_t>(77, 32), Point<int16_t>(110, 32),
			Point<int16_t>(11, 65),
			Point<int16_t>(77, 65), Point<int16_t>(110, 65)
		};

		static constexpr int16_t CELL_W = 31;
		static constexpr int16_t CELL_H = 30;

		class AddonIcon : public Icon::Type
		{
		public:
			AddonIcon(EquipSlot::Id source);

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
		/// ESC / 假关闭钮：转宿主同关，禁止单独关辅助。
		void close_with_host();

		const Inventory& inventory;
		Texture backgrnd;
		Point<int16_t> bg_origin;
		std::array<std::unique_ptr<Icon>, SLOT_COUNT> icons;
		EquipSlot::Id selected_slot = EquipSlot::Id::NONE;
	};
}
