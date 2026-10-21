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

#include "../../Graphics/Animation.h"
#include "../../Graphics/Geometry.h"
#include "../../Graphics/Text.h"

#include <array>
#include <string>

namespace ms
{
	// 每日礼物 / Daily Gift（灰框 UI 骨架）
	// 本版：纯客户端假数据 + 视觉动画；领取/杀怪/账号日限/等级·MVP 仅预留接口。
	// 道具槽预留：格子中部不渲染 ItemData / 假道具 ID / 图标 Texture（后期对接再摆）。
	// 数字：用 digit_glyphs[0..9] 逐位拼装「第 N 天」与顶栏日期，不写死整句位图。
	// HD 切片归档：docs/features/daily-gift-hd/resources/（本版可不加载 PNG）。
	class UIDailyGift : public UIDragElement<PosDAILYGIFT>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::DAILYGIFT;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		static constexpr int16_t kTotalDays = 28;
		static constexpr int16_t kCols = 7;
		static constexpr int16_t kRows = 4;

		UIDailyGift();

		void draw(float inter) const override;
		void update() override;

		void remove_cursor() override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		// --- 逻辑预留（阶段 3+ 接服务端；本版空实现 / 本地假态）---
		bool try_claim(int16_t day);
		void on_claim_result(int16_t day, bool success, int32_t result_code);
		void set_kill_progress(int32_t current, int32_t need = 300);
		void set_account_daily_limit(bool reached);
		void set_level_gate(int16_t min_level);
		void set_mvp_tier(int8_t tier);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum class CellVisual : uint8_t
		{
			Available,
			Claimed,
			Highlight
		};

		enum Buttons : uint16_t
		{
			CLOSE,
			CLAIM,
			SLOT_BASE // SLOT_BASE + 0..27
		};

		void close();
		void init_layout();
		void init_fake_data();
		void init_digit_glyphs();
		void fake_claim_slot(int16_t day_index);
		void play_claim_flash(int16_t day_index);
		bool is_highlight_day(int16_t day_1based) const;
		Point<int16_t> slot_origin(int16_t day_index) const;
		Rectangle<int16_t> slot_bounds(int16_t day_index) const;
		int16_t slot_at(Point<int16_t> cursor_offset) const;
		void draw_slot(int16_t day_index, float alpha) const;
		void draw_progress_bar(Point<int16_t> origin) const;
		void draw_claim_button(Point<int16_t> origin) const;
		// 用 0-9 字形数组从左到右拼数字；返回绘制后光标 x 偏移
		int16_t draw_digits(int32_t value, Point<int16_t> origin, float alpha, bool zero_pad2, bool date_style = false) const;
		void draw_day_label(int16_t day_1based, Point<int16_t> slot_abs, float alpha) const;
		void draw_date_range(Point<int16_t> origin, float alpha) const;

		static constexpr int16_t kWinW = 560;
		static constexpr int16_t kWinH = 520;
		static constexpr int16_t kTitleH = 20;
		static constexpr int16_t kBannerH = 72;
		static constexpr int16_t kFooterH = 130;
		static constexpr int16_t kGridPadX = 16;
		static constexpr int16_t kGridPadY = 8;
		static constexpr int16_t kSlotW = 72;
		static constexpr int16_t kSlotH = 78;
		static constexpr int16_t kSlotGapX = 4;
		static constexpr int16_t kSlotGapY = 4;
		static constexpr int16_t kDigitAdvance = 10;

		static constexpr int16_t kHighlightDays[5] = { 9, 12, 17, 24, 26 };

		// 假日期范围（顶栏拼装用，可后期换成服务端区间）
		int16_t date_month_start;
		int16_t date_day_start;
		int16_t date_month_end;
		int16_t date_day_end;

		std::array<CellVisual, kTotalDays> cells{};
		// 可复用数字资源：每位独立「0」…「9」（本版 Text 字形；HD 可换 PNG 贴图）
		std::array<Text, 10> digit_glyphs{};
		std::array<Text, 10> digit_glyphs_date{};
		Text day_prefix;   // 「第」
		Text day_suffix;   // 「天」
		Text date_month_unit; // 「月」
		Text date_day_unit;   // 「日」
		Text date_range_sep;  // 「 ~ 」
		Text date_bang;       // 「!」

		Text title_text;
		Text banner_sub;
		Text banner_main;
		Text footer_rules[6];
		Text progress_label;
		Text claim_label;
		Text claimed_mark;

		ColorBox window_bg;
		ColorBox title_bar;
		ColorBox banner_box;
		ColorBox grid_bg;
		ColorBox footer_box;
		ColorBox slot_fill;
		ColorBox slot_border;
		ColorBox slot_hover;
		ColorBox slot_pressed;
		ColorBox slot_claimed;
		ColorBox slot_highlight;
		ColorBox item_slot_empty;
		ColorBox claimed_badge;
		ColorBox progress_track;
		ColorBox progress_fill;
		ColorBox claim_btn;
		ColorBox flash_box;

		Animation newitem_flash;
		bool has_newitem_flash;

		int16_t hover_slot;
		int16_t pressed_slot;
		bool claim_hover;
		bool claim_pressed;

		int32_t fake_kill_current;
		int32_t fake_kill_need;
		float progress_visual;

		float open_alpha;
		int16_t flash_slot;
		int16_t flash_remain;
	};
}
