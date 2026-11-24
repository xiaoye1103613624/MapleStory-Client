//////////////////////////////////////////////////////////////////////////////////
//	Left-side vertical icon dock — SideMenu tree + folder flyout (1A).
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"
#include "../../Util/SideMenuConfig.h"
#include "../../Graphics/Texture.h"

#include <string>
#include <vector>

namespace ms
{
	class UISideIconBar : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::SIDEICONBAR;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UISideIconBar();

		void draw(float inter) const override;
		void update() override;
		void update_screen(int16_t new_width, int16_t new_height) override;

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void remove_cursor() override;

		UIElement::Type get_type() const override;

		void reload_config();

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		// 点击回调运行在 UIElement::send_cursor 遍历 buttons 的循环内；此处若直接
		// rebuild_buttons()（buttons.clear()）会使正在遍历的迭代器失效 → 下一次
		// UIElement.cpp:386 btit.second->bounds() 访问野指针 c0000005（WER 实测）。
		// 因此点击里只改数据+置脏，真正的 clear/重建延迟到 update() 里做。
		void defer_rebuild();

		enum Buttons : uint16_t
		{
			BT_MAIN = 0,
			BT_ROOT0 = 1,
			BT_FLY0 = 200
		};

		// 目录右侧 flyout：复用 192 ShortCutUI 三段底板与格距（贴目录右，非屏幕居中）。
		static constexpr int16_t SLOT_GAP = 4;
		static constexpr int16_t FLY_GAP = 8;
		static constexpr int16_t FLY_PANEL_W = 353;
		static constexpr int16_t FLY_TOP_H = 33;
		static constexpr int16_t FLY_MIDDLE_H = 92;
		static constexpr int16_t FLY_BOTTOM_H = 33;
		static constexpr int16_t FLY_PITCH_X = 56;
		static constexpr int16_t FLY_PITCH_Y = 92;
		static constexpr int16_t FLY_ICON_X_PAD = 10;
		static constexpr int16_t FLY_COLS = 6;
		static constexpr int16_t MARGIN_X = 4;
		static constexpr int16_t MAX_ROOT = 16;
		static constexpr int16_t MAX_FLY = 24;
		static constexpr size_t ATLAS_BASE = 0x534D4E55ull; // 'SMNU'

		void rebuild_buttons();
		void layout_buttons();
		void layout_flyout();
		void clear_flyout_buttons();
		void toggle_folder(const std::string& folder_id);
		void close_flyout();
		void dispatch_item(const SideMenuConfig::Item& item);
		void dispatch_npc_script(const std::string& arg);
		bool try_make_frame_button(uint16_t id, const std::string& icon_key, size_t atlas_slot);
		bool make_btmain_button();
		void load_flyout_panel();

		SideMenuConfig::Document doc;
		std::vector<SideMenuConfig::Item> root;
		std::vector<uint16_t> root_button_ids;
		std::vector<SideMenuConfig::Item> fly_items;
		std::vector<uint16_t> fly_button_ids;
		std::string expanded_folder_id;
		// flyout 底板相对本 element 原点的左上角；lines = middle 重复行数。
		Point<int16_t> fly_panel_rel;
		int16_t fly_panel_lines = 0;
		Texture tex_fly_top;
		Texture tex_fly_middle;
		Texture tex_fly_bottom;
		int16_t view_w = 800;
		int16_t view_h = 600;
		int reload_cooldown = 0;
		bool hover_main_opened = false;
		bool rebuild_pending = false; // defer_rebuild() 置位，下一帧 update() 统一重建
		// 每次 rebuild_buttons() 递增。send_cursor 遍历按钮时若 generation 变化，
		// 说明 dispatch→emplace 间接触发了重建，正在遍历的 unique_ptr 已析构 → 必须中止遍历。
		uint32_t buttons_generation = 0;
	};
}
