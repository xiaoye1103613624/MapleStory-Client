//////////////////////////////////////////////////////////////////////////////////
// UIDamageRank — 个人伤害统计窗（按 Tengutake/Tengutake 参考实现完整还原）
//
// 参考：E:/MXD/扩展改动/个人伤害统计/统计1/Tengutake/src/uiDamageRank.cpp
//   :25-60 布局契约；:510-560 素材 UOL；:1333-1515 按钮矩形/四态；
//   :2090-2145 gauge 分段绘制；:2398-2482 按钮行为；:994-1080 滚动条。
//
// 数据：Character/DamageRank.h 单例 snapshot_players()/snapshot_skills()，
//   由 DptTrackerHandler (0x3714) 喂入；收包链路不在本文件改动。
// 发包：控制包 0x3713（open=1/reset=2/close=3）在 cpp 内用 OutPacket 直发，
//   不触碰 Net/Packets 与 OutPacket 枚举（约束：不改协议与其它文件）。
//
// 资源契约（probe 已确认 live 存在）：
//   UIWindow.img/DamageRank/{backgrndmax,backgrndmin,backgrndcenter,backgrndbottom,
//     backgrndbottomover,title1,title2,iconCommonAtk,iconUnknownSkill}
// 尚待补导入（缺失时优雅降级，不崩）：
//   DamageRank/gauge/{commonl,commonc,commonr,selfl,selfc,selfr}
//   DamageRank/Bt{Reset,Switch,Auto}/{normal,mouseOver,pressed,disabled}/0
//   Basic.img/Bt{Min,Max,Close} 与 Basic.img/VScr4/enabled/*
//
// 布局常量全部来自参考源码，禁止凭观感改动。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Character/DamageRank.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Geometry.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ms
{
	class UIDamageRank : public UIDragElement<PosDAMAGERANK>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::DAMAGERANK;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIDamageRank();

		void draw(float inter) const override;
		void update() override;
		Button::State button_pressed(uint16_t buttonid) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void remove_cursor() override;
		void send_scroll(double yoffset) override;
		UIElement::Type get_type() const override;
		// ESC 关闭：UI.cpp 的 ESC 白名单已含 DAMAGERANK，这里只处理按下态。
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		// 数据源变化后由 DptTrackerHandler 调用；active 时立即刷新行缓存。
		void reload();

	private:
		// ── 页签 ──────────────────────────────────────────────────────────────
		enum class Mode : uint8_t { Player = 0, Skill = 1 };

		// 5 颗头部按钮（顺序与参考 ButtonId 一致）。
		enum BtnId : uint8_t
		{
			kBtnReset = 0,
			kBtnSwitch,
			kBtnAuto,
			kBtnMinMax,
			kBtnClose,
			kBtnNone = 0xFF
		};

		// 滚动条可命中部位。
		enum class ScrollPart : uint8_t { None, Prev, Next, Thumb, Track };

		// ── 布局常量（参考 :25-59） ───────────────────────────────────────────
		static constexpr int16_t kLayerWidth = 223;
		static constexpr int16_t kLayerHeightExpanded = 223;
		static constexpr int16_t kLayerHeightMinimized = 20;
		static constexpr int16_t kLayerBottomHeight = 2;

		static constexpr int16_t kMaxPlayerRows = 30;
		static constexpr int16_t kMaxSkillRows = 15;

		static constexpr int16_t kHeaderHeight = 26;
		static constexpr int16_t kTitleY = 26;
		static constexpr int16_t kRowsStartY = 52;

		static constexpr int16_t kPlayerRowHeight = 21;
		static constexpr int16_t kPlayerRowGap = 2;
		static constexpr int16_t kPlayerRowPitch = kPlayerRowHeight + kPlayerRowGap;
		static constexpr int16_t kSkillRowHeight = 42;

		// 垂直落位：文字墨迹顶按「行高中线」求 —— ink_top = row_y + (row_h - ink_h)/2。
		//   ink_h 是 A11 单行字形的实际墨迹高（含 CJK 约 11px，不是 linespace，也不是
		//   ink_offset）；行数中线只定「目标墨迹顶」，真正的绘制点反解统一交给
		//   Text::y_for_ink_top（Graphics/Text.h:164 = ink_top - ink_offset()），
		//   与 UIStatsInfo/UIStatusBar 的整改口径一致，禁止再写裸 y+1/y+5 偏移。
		static constexpr int16_t kA11InkHeight = 11;   // A11 单行墨迹高（含 CJK，实测 10~11，取 11 居中）

		static constexpr int16_t kResetX = 5;
		static constexpr int16_t kSwitchX = 137;
		static constexpr int16_t kAutoX = 169;
		static constexpr int16_t kMinX = 192;
		static constexpr int16_t kCloseX = 206;

		static constexpr int16_t kScrollX = 205;
		static constexpr int16_t kScrollWidth = 15;
		static constexpr int16_t kScrollButtonHeight = 13;
		static constexpr int16_t kScrollThumbMinHeight = 12;

		// ── 资源加载 ──────────────────────────────────────────────────────────
		void load_assets();
		bool load_tex(Texture& out, nl::node n);

		// ── 布局/几何 ─────────────────────────────────────────────────────────
		int16_t current_height() const;
		void refresh_dimension();     // 行数/页签/最小化变化后同步画布高度
		int16_t item_count() const;
		int16_t row_pitch() const;
		int16_t row_height() const;
		int16_t visible_rows() const;
		int16_t max_scroll() const;
		bool has_scroll() const;
		void clamp_scroll();

		// 按行高中线求「目标墨迹顶」：row_y + (row_height - ink_h)/2，
		// 调用方再把它喂给 Text::y_for_ink_top 反解绘制点。
		int16_t centered_ink_top(int16_t row_y, int16_t row_h) const;

		// 技能图标取数：skill_id<0(DoT) 不画；0 → 通用攻击；
		// >0 → SkillData::get(id).get_icon(NORMAL)，miss 回退 icon_unknown_skill。
		// 有效结果进 cache 避免每帧查表；miss 不缓存以便 Skill 根装载后重试。
		const Texture* skill_icon(int32_t skill_id) const;

		Rectangle<int16_t> button_rect(BtnId id) const;
		bool hit_test_button(int16_t x, int16_t y, BtnId& out) const;

		Rectangle<int16_t> scroll_bar_rect() const;
		Rectangle<int16_t> scroll_thumb_rect() const;
		ScrollPart hit_test_scroll(int16_t x, int16_t y) const;
		void set_scroll_from_thumb_y(int16_t y);

		// ── 绘制 ──────────────────────────────────────────────────────────────
		void draw_background(Point<int16_t> org, float fade) const;
		void draw_buttons_manual(Point<int16_t> org, float fade) const;
		void draw_title_band(Point<int16_t> org, float fade) const;
		void draw_player_page(Point<int16_t> org, float fade) const;
		void draw_skill_page(Point<int16_t> org, float fade) const;
		void draw_scrollbar(Point<int16_t> org, float fade) const;
		void draw_gauge(
			Point<int16_t> org,
			int16_t x,
			int16_t y,
			int16_t width,
			bool self_style,
			float ratio,
			float fade) const;

		Texture button_texture(BtnId id, Button::State state) const;

		// ── 交互 ──────────────────────────────────────────────────────────────
		void on_button(BtnId id);
		void rebuild_rows();

		// 直接发 0x3713 控制包（action: 1=open / 2=reset / 3=close）。
		static void send_control(uint8_t action);

		// ── 状态 ──────────────────────────────────────────────────────────────
		Mode mode = Mode::Player;
		bool auto_mode = true;      // 参考：Auto 开时不显示滚动条
		bool minimized = false;
		int32_t scroll = 0;         // 当前首行偏移（玩家/技能共用，切页时清零）

		BtnId hovered_btn = kBtnNone;
		BtnId pressed_btn = kBtnNone;   // 仅用于 pressed 态的瞬时视觉
		uint8_t pressed_flash = 0;      // pressed 态残留帧数（每帧 update 递减）
		ScrollPart hovered_scroll = ScrollPart::None;
		bool thumb_dragging = false;

		bool assets_load_tried = false;
		bool gauge_ready = false;
		bool scroll_ready = false;

		// 背景 4 段 + over 底段。
		Texture bg_max;         // 223x52
		Texture bg_min;         // 223x20
		Texture bg_center;      // 223x21
		Texture bg_bottom;      // 223x3
		Texture bg_bottom_over; // 223x3

		// 页签条（玩家页 title1 / 技能页 title2），画在 (8, kTitleY)。
		Texture title1;
		Texture title2;

		// gauge 六种分段（缺失 → 纯色条降级）。
		Texture gauge_common_l;
		Texture gauge_common_c;
		Texture gauge_common_r;
		Texture gauge_self_l;
		Texture gauge_self_c;
		Texture gauge_self_r;

		// 技能图标（已导入 live）。
		Texture icon_common_atk;
		Texture icon_unknown_skill;
		// 按 skill_id 缓存已解析的技能图标（Texture 值副本，跨帧安全）。
		// 只缓存有效图标：Skill 根未装载时的 miss 不缓存，待根就绪后可重试出图。
		mutable std::unordered_map<int32_t, Texture> skill_icon_cache;

		// 5 颗按钮的四态纹理；[BtnId][Button::State] 只取前 4 态。
		Texture btn_tex[5][Button::State::NUM_STATES];
		// 最小化时 MinMax 显示 BtMax 图标（三态，无 disabled）。
		Texture btn_max_tex[Button::State::NUM_STATES];

		// 滚动条 VScr4：prev/next/thumb 各三态 + base。
		Texture scr_prev[3];
		Texture scr_next[3];
		Texture scr_thumb[3];
		Texture scr_base;

		// 空数据提示（行文本由 draw_* 依数据快照即时生成）。
		Text empty_hint;
	};
}
