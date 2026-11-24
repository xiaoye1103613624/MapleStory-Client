//////////////////////////////////////////////////////////////////////////////////
// UIWitchForest — 192 魔女森林抽奖窗（UIWindowCN.img/CN_50100）· 全链路接入版
// 主窗：backgrnd / button:50|50X10|100|100X10 / Help|Close|Notice
//       / Number(次数) / Timer(倒计时) / layer:miracleTime / 5 奖励底槽 / Grade|Type
//       / specialRewardCover0|1 / popUp(confirm|cancel|confirm2) / Notice(底板+滚动)
// 结果窗(Result)：backgrnd / layer:frame / title50|title100 / rewardSlot+layer:rewardBg0..10
//       / button:OK / Effect 65 帧 / EffectSS 65 帧。
// 协议 WITCH_FOREST_ACTION C 0x3719 / S 0x371A。
// 资源在构建阶段已把占位 link 内联为实体位图；运行时按 WZ 中心原点换算锚点。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	class UIWitchForest : public UIDragElement<PosWITCHFOREST>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::WITCHFOREST;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIWitchForest();

		void draw(float inter) const override;
		void update() override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Button::State button_pressed(uint16_t buttonid) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void remove_cursor() override;
		UIElement::Type get_type() const override;

		// 0x371A handler：respType=0 状态(nx,draws,secs,miracle)；=1 抽奖结果。
		void apply_state(int64_t nxcredit, int32_t totaldraws, int32_t secondsleft, int8_t miracle);
		void apply_result(int8_t drawtype, int32_t count,
			const std::vector<int32_t>& itemids, const std::vector<int32_t>& qtys,
			const std::vector<int8_t>& grades, int64_t nxafter, bool special);

	private:
		enum Buttons : uint16_t {
			Bt50 = 0, Bt50x10 = 1, Bt100 = 2, Bt100x10 = 3,
			BtHelp = 4, BtNotice = 5, BtClose = 6,
			BtPopConfirm = 7, BtPopCancel = 8, BtPopConfirm2 = 9,
			BtResultOK = 10
		};

		static constexpr int16_t WIN_W = 803;
		static constexpr int16_t WIN_H = 713;
		static constexpr int16_t FX_FRAMES = 65;

		bool has_assets = false;
		Texture backgrnd;
		Texture reward_bg_normal[5];   // layer:reward50Bg0..4
		Texture reward_bg_rare[5];     // layer:reward100Bg0..4
		Texture grade_icon[5];
		Texture type_icon[5];
		Texture number_glyph[10];      // Number 0..9（次数）
		Texture timer_glyph[11];       // Timer 0..9 + ':'
		Texture miracle_time;          // layer:miracleTime
		Texture special_cover[2];      // specialRewardCover0/1

		// popUp
		Texture popup_bg;
		// Notice
		Texture notice_bg;

		// Result 演出窗
		Texture result_bg;
		Texture result_frame;
		Texture result_title[2];       // title50 / title100
		Texture result_slot[5];        // rewardSlot 0..4
		Texture result_rewardbg[3];    // layer:rewardBg0/1/2（3/4/5.. 复用）
		Texture result_fx[FX_FRAMES];       // Effect 0..64
		Texture result_fxss[FX_FRAMES];     // EffectSS 0..64

		Text info;
		Text result_text;

		int64_t nxcredit = 0;
		int32_t totaldraws = 0;
		int32_t secondsleft = 0;
		int8_t miracle = 0;
		bool has_snapshot = false;
		bool show_popup = false;
		bool show_notice = false;
		bool show_result = false;
		bool result_special = false;
		int8_t result_drawtype = 0;
		int8_t pending_drawtype = -1;

		std::vector<int32_t> result_itemids;
		std::vector<int32_t> result_qtys;
		std::vector<int8_t> result_grades;

		// 动画帧状态
		size_t fx_frame = 0;
		uint32_t fx_delay = 0;
		uint32_t draw_tick = 0;        // update 里倒计时用

		void close();
		void refresh_result_text();
		void draw_timer(const Point<int16_t>& org, float fade) const;
		void draw_number(const Point<int16_t>& org, int32_t value, float fade) const;
	};
}
