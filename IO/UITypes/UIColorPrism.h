//////////////////////////////////////////////////////////////////////////////////
// UIColorPrism — 七彩棱镜染色窗（D1 中文包协议 0x11D / 0x184，2026-11-21）。
//
// 资源根：UI/UIWindow.img/ColoringPrism/{backgrnd 300x398, BtOff, BtClose,
// BtCancel, BtOK, enabled}（由父 agent 转 key 入库 live）。
// 布局：300x398；真按钮走 MapleButton（命中区从 dragarea 摘出，参照
// UIDamageRank.cpp:295-309）；3 条滑条 trackTone/Chroma/Bright 176x8 +
// thumb 22x14；预览区画玩家自身；投放槽从装备栏拖入（校验：现金 + 品类
// 104/105/106/110/170 + 已穿戴 slot<0）。
// 文案全部走 Locale（zh-CN/en-US）；窗口可拖拽、ESC 可关。
// 预览升级路径（TintCache 实时改色）沿用既有引擎，不改服务端、不 Sweep。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace ms
{
	class UIColorPrism : public UIDragElement<PosCOLORPRISM>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::COLORPRISM;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		/// 服务端 0x184 DYE_LIST/DYE_MERGE 解出的一条染色（指南 §3.3）。
		struct DyeEntry
		{
			int32_t item_id = 0;
			float hue = 0.0f;    // 度 [-180,180]
			float sat = 0.0f;    // [-1,1]
			float light = 0.0f;  // [-1,1]
		};

		UIColorPrism();

		// —— 收包入口（ColoringPrismHandler 调用）——
		/// OPEN：确保窗口构造并激活（已开则仅 makeactive）。
		static void ensure_open();
		/// DYE_LIST：替换本人染色表；若当前投放的目标在表内，同步滑条值。
		void apply_dye_list(const std::vector<DyeEntry>& entries);
		/// DYE_MERGE：他人染色，仅记录（他人实时预览降级，见报告）。
		void apply_dye_merge(int32_t char_id, const std::vector<DyeEntry>& entries);

		void draw(float inter) const override;
		void update() override;
		// 按钮命中优先于窗口拖拽（BtClose 在标题条内），其余交基类。
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursor) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		// 从装备栏拖入（校验已穿戴现金可染品类）。
		bool send_icon(const Icon& icon, Point<int16_t> cursor_position) override;
		Button::State button_pressed(uint16_t buttonid) override;
		UIElement::Type get_type() const override;

	private:
		enum Buttons : uint16_t
		{
			BtOff = 1000,      // 还原原型（发 CLEAR，不关窗）
			BtCancel = 1001,   // 取消（发 CLEAR 并关窗）
			BtOK = 1002,       // 确定（发 CONFIRM，弹提示并关窗）
			BtClose = 1003,    // 标题条关闭（不发包）
		};

		// —— 窗口几何（300x398）——
		static constexpr int16_t WIN_W = 300;
		static constexpr int16_t WIN_H = 398;
		static constexpr int16_t TITLE_H = 22;              // 可拖动标题条高度
		static constexpr int16_t CLOSE_X = 280;
		static constexpr int16_t CLOSE_Y = 4;
		static constexpr int16_t CLOSE_SIZE = 12;

		// 投放槽（从装备栏拖入；图标 32x32）
		static constexpr int16_t WELL_X = 16;
		static constexpr int16_t WELL_Y = 30;
		static constexpr int16_t WELL_SIZE = 48;
		static constexpr int16_t ICON_SIZE = 32;

		// 预览区（角色脚点大致在窗内 (150, 236)）
		static constexpr int16_t PREVIEW_L = 8;
		static constexpr int16_t PREVIEW_R = 292;
		static constexpr int16_t PREVIEW_T = 84;
		static constexpr int16_t PREVIEW_B = 250;
		static constexpr int16_t AVATAR_X = (PREVIEW_L + PREVIEW_R) / 2;  // 150
		static constexpr int16_t AVATAR_Y = 236;

		// 三条滑条：色相/彩度/明度（均 -100..100）
		static constexpr int16_t ROW_COUNT = 3;
		// 三条标签板顶边由素材实测（亮度突变）：263 / 299 / 331，板高约30。
		// 之前 ROWS_T=258,step=26,ROW_H=24 使轨/thumb 越往下越偏上（截图实证），改为查表精确对齐。
		static constexpr int16_t ROW_TOPS[3] = { 263, 299, 331 };
		static constexpr int16_t ROW_H = 30;
		static constexpr int16_t LABEL_X = 10;
		static constexpr int16_t LABEL_W = 48;
		static constexpr int16_t TRACK_X = 62;
		static constexpr int16_t TRACK_W = 176;
		static constexpr int16_t TRACK_H = 8;
		static constexpr int16_t VALUE_X = 240;
		static constexpr int16_t VALUE_W = 48;
		static constexpr int16_t THUMB_W = 22;
		static constexpr int16_t THUMB_H = 14;
		static constexpr int16_t TRACK_TRAVEL = TRACK_W - THUMB_W;  // 154
		static constexpr int16_t SLIDER_MIN = -100;
		static constexpr int16_t SLIDER_MAX = 100;

		// 底部四颗真按钮
		static constexpr int16_t BTN_T = 360;
		static constexpr int16_t BTN_W = 52;
		static constexpr int16_t BTN_H = 20;

		// —— 逻辑 ——
		void refresh_labels();                 // 按 Locale 重建所有文案
		void preview_refresh();                // 滑条变化 → TintContext 实时预览（SoftRefresh）
		void confirm_and_close();              // 发 CONFIRM + 提示 + 关窗
		void clear_current(bool close_window); // 发 CLEAR（BtOff / BtCancel）
		void reset_sliders();                  // 滑条归零并刷新预览

		static bool is_dyeable_category(int32_t item_id);

		int32_t value(int16_t row) const;
		void set_value(int16_t row, int32_t v);
		void set_value_from_x(int16_t row, int16_t x);
		int16_t thumb_x(int16_t row) const;
		int16_t row_top(int16_t row) const
		{
			return (row >= 0 && row < 3) ? ROW_TOPS[row] : ROW_TOPS[0];
		}
		bool thumb_contains(int16_t row, Point<int16_t> local) const;
		bool button_contains(uint16_t id, Point<int16_t> local) const;
		bool is_in_well(Point<int16_t> local) const;

		// 当前投放目标（已穿戴）—— invType 有符号 -1 = EQUIPPED（与协议一致）
		static constexpr int8_t INVTYPE_EQUIPPED = -1;
		bool has_target = false;
		int8_t target_inv_type = INVTYPE_EQUIPPED;
		int16_t target_slot = 0;      // wire 槽（负数 = 已穿戴）
		int32_t target_item_id = 0;

		int16_t slider[ROW_COUNT] = { 0, 0, 0 };  // -100..100
		int16_t drag_row = -1;
		uint32_t locale_gen = 0;

		// 服务端染色表（itemId → 最新条目），滑条初值/还原用
		std::unordered_map<int32_t, DyeEntry> own_dyes;
		std::unordered_map<int32_t, std::unordered_map<int32_t, DyeEntry>> foreign_dyes;

		Text label_title;
		Text row_labels[ROW_COUNT];
		Text row_values[ROW_COUNT];
		Text drop_hint;
		Text success_hint;

		Texture backgrnd;              // UIWindow.img/ColoringPrism/backgrnd（300x398）
		Texture track[ROW_COUNT];      // trackTone / trackChroma / trackBright
		Texture thumb[3];              // thumbNormal / thumbPressed / thumbMouseOver
		Texture target_icon;           // 投放槽中的装备图标
		bool has_assets = false;
	};
}
