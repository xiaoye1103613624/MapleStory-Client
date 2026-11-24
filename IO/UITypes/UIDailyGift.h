//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																						//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.												//
//																						//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of					//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the					//
//	GNU Affero General Public License for more details.							//
//																						//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"

#include "../../Graphics/Animation.h"
#include "../../Graphics/Geometry.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <array>
#include <cstdint>
#include <string>

namespace ms
{
	// 每日礼物 / Daily Gift —— 192 原生资源 + 私有协议 0x3721/0x3722 真窗。
	// 构造时发 0x3721 OPEN 请求快照；服务端 0x3722/respType=0 落 apply_snapshot，
	// 点击可领格发 0x3721 CLAIM，真实结果由 0x3722/respType=1 → on_claim_result 驱动。
	// 28 格用服务端下发的 reward_item_ids 经 ItemData::get(id).get_icon(false) 画真图标；
	// 已领格灰蒙+对勾，可领格高亮，未解锁灰。进度条 killCount/300、等级门 levelMin、
	// premiumLeft 均为真数据。资源优先 UiAssetResolve::window4("dailyGift")（192 原生，
	// 节点名小写），无则回退原有灰框。
	class UIDailyGift : public UIDragElement<PosDAILYGIFT>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::DAILYGIFT;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		static constexpr int16_t kTotalDays = 28;
		static constexpr int16_t kCols = 7;
		static constexpr int16_t kRows = 4;
		static constexpr int32_t kKillTarget = 300; // 进度条分母（与规则文案一致）

		UIDailyGift();

		void draw(float inter) const override;
		void update() override;

		void remove_cursor() override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		// --- 协议入口 ---
		// 0x3722/respType=0：配置快照。数组定长 28（与协议 int[28] 对齐）。
		void apply_snapshot(int8_t current_day, int32_t claimed_mask,
			int8_t level_min, int8_t quest_done, int32_t kill_count, int8_t premium_left,
			const std::array<int32_t, kTotalDays>& reward_item_ids,
			const std::array<int32_t, kTotalDays>& reward_counts);

		// 0x3722/respType=1：领奖结果。result_code 0成功/1等级不足/2杀怪不足/3已领过/4非当前日/5月限尽。
		void on_claim_result(int8_t result_code, int8_t day, int32_t item_id, int32_t count);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum class CellVisual : uint8_t
		{
			Locked,    // 未解锁（day > current_day 且未领）：灰
			Available, // 可领（day <= current_day 且未领）：高亮
			Claimed    // 已领：灰蒙 + 对勾
		};

		enum Buttons : uint16_t
		{
			CLOSE,
			CLAIM,
			SLOT_BASE // SLOT_BASE + 0..27
		};

		void close();
		void init_layout();
		void init_digit_glyphs();
		void rebuild_cells();
		void refresh_icon_textures();
		void request_claim(int16_t day_index);
		void play_claim_flash(int16_t day_index);
		bool is_highlight_day(int16_t day_1based) const; // premium 日金色角标（视觉装饰）
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
		void refresh_status_texts();

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

		// 假日期范围（顶栏拼装用，服务端未下发区间，保留既有表现）
		int16_t date_month_start;
		int16_t date_day_start;
		int16_t date_month_end;
		int16_t date_day_end;

		// --- 服务端真数据（0x3722/0） ---
		bool has_snapshot = false;
		int8_t current_day = 0;
		int32_t claimed_mask = 0;
		int8_t level_min = 0;
		int8_t quest_done = 0;
		int32_t kill_count = 0;
		int8_t premium_left = 0;
		std::array<int32_t, kTotalDays> reward_item_ids{};
		std::array<int32_t, kTotalDays> reward_counts{};
		std::array<Texture, kTotalDays> reward_icons{};
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
		Text status_text;  // 等级门 + premiumLeft（动态，不进 locale 绑定表）
		Text result_text;  // 最近一次领奖结果提示（事件驱动，不进绑定表）

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

		// 192 原生资源根（window4("dailyGift")，节点名小写）；无则走灰框。
		bool has_native_bg = false;
		Texture native_bg;

		Animation newitem_flash;
		bool has_newitem_flash;

		int16_t hover_slot;
		int16_t pressed_slot;
		bool claim_hover;
		bool claim_pressed;

		float progress_visual;

		float open_alpha;
		int16_t flash_slot;
		int16_t flash_remain;
	};
}
