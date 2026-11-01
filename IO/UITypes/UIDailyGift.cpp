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
#include "UIDailyGift.h"

#include "UISoftKey.h"

#include "../UI.h"

#include "../Components/AreaButton.h"
#include "../Components/MapleButton.h"

#include "../../Audio/Audio.h"
#include "../../MapleStory.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#include <cmath>
#include <cstdio>
#include <vector>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIDailyGift::UIDailyGift() : UIDragElement<PosDAILYGIFT>()
	{
		hover_slot = -1;
		pressed_slot = -1;
		claim_hover = false;
		claim_pressed = false;
		has_newitem_flash = false;
		fake_kill_current = 120;
		fake_kill_need = 300;
		progress_visual = 0.0f;
		open_alpha = 0.0f;
		flash_slot = -1;
		flash_remain = 0;
		date_month_start = 10;
		date_day_start = 1;
		date_month_end = 10;
		date_day_end = 31;

		init_layout();
		init_digit_glyphs();
		init_fake_data();

		nl::node close = UiAssetResolve::close_btn();

		if (close)
			buttons[Buttons::CLOSE] = std::make_unique<MapleButton>(close, Point<int16_t>(kWinW - 22, 3));

		for (int16_t i = 0; i < kTotalDays; ++i)
		{
			Point<int16_t> origin = slot_origin(i);
			buttons[Buttons::SLOT_BASE + i] = std::make_unique<AreaButton>(
				origin, Point<int16_t>(kSlotW, kSlotH));
		}

		const int16_t claim_x = kWinW - 150;
		const int16_t claim_y = kWinH - 52;
		buttons[Buttons::CLAIM] = std::make_unique<AreaButton>(
			Point<int16_t>(claim_x, claim_y), Point<int16_t>(130, 36));

		nl::node item_new = nl::nx::UI["UIWindow2.img"]["Item"]["New"]["inventory"];
		if (!item_new)
			item_new = nl::nx::UI["UIWindow.img"]["Item"]["New"]["inventory"];

		if (item_new)
		{
			newitem_flash = Animation(item_new);
			has_newitem_flash = true;
		}

		dimension = Point<int16_t>(kWinW, kWinH);
		dragarea = Point<int16_t>(kWinW, kTitleH);

		LOG(LOG_INFO, "UIDailyGift open (grey-box; digits 0-9 composed; no item icons)");
	}

	void UIDailyGift::init_layout()
	{
		window_bg = ColorBox(kWinW, kWinH, Color::Name::MINESHAFT, 0.95f);
		title_bar = ColorBox(kWinW, kTitleH, Color::Name::DARKGREY, 1.0f);
		banner_box = ColorBox(kWinW - 8, kBannerH, Color::Name::JAPANESELAUREL, 1.0f);
		grid_bg = ColorBox(kWinW - 8, kRows * (kSlotH + kSlotGapY) + kGridPadY * 2,
			Color::Name::TUNA, 0.9f);
		footer_box = ColorBox(kWinW - 8, kFooterH - 8, Color::Name::BLACK, 0.92f);

		slot_fill = ColorBox(kSlotW - 2, kSlotH - 2, Color::Name::EMPEROR, 0.95f);
		slot_border = ColorBox(kSlotW, kSlotH, Color::Name::SILVER, 1.0f);
		slot_hover = ColorBox(kSlotW, kSlotH, Color::Name::LIGHTBLUE, 0.35f);
		slot_pressed = ColorBox(kSlotW, kSlotH, Color::Name::BLACK, 0.35f);
		slot_claimed = ColorBox(kSlotW - 2, kSlotH - 2, Color::Name::DARKGREY, 0.7f);
		slot_highlight = ColorBox(kSlotW, kSlotH, Color::Name::SUPERNOVA, 1.0f);
		// 道具槽预留：仅空槽底板，本版不渲染物品
		item_slot_empty = ColorBox(36, 36, Color::Name::MINESHAFT, 0.55f);
		claimed_badge = ColorBox(18, 18, Color::Name::PERSIANGREEN, 1.0f);
		progress_track = ColorBox(160, 10, Color::Name::DARKGREY, 1.0f);
		progress_fill = ColorBox(1, 10, Color::Name::PERSIANGREEN, 1.0f);
		claim_btn = ColorBox(130, 36, Color::Name::PERSIANGREEN, 1.0f);
		flash_box = ColorBox(kSlotW, kSlotH, Color::Name::WHITE, 0.6f);

		title_text = Text(Text::Font::A11B, Text::Alignment::CENTER, Color::Name::YELLOW, "DAILY GIFT");
		banner_sub = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "丰厚礼物，天天相送！");
		banner_main = Text(Text::Font::A18M, Text::Alignment::LEFT, Color::Name::WHITE, "冒险岛每日礼物");

		footer_rules[0] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
			"· 击杀等级范围内怪物达到 300 只可领取当日礼物");
		footer_rules[1] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
			"· 需角色等级 33 级以上");
		footer_rules[2] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
			"· 每个账号每日限领 1 次（本版假数据）");
		footer_rules[3] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
			"· 格子中部道具槽预留，本版不渲染物品");
		footer_rules[4] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
			"· 点击格子即可假领取并显示已领取标识");
		footer_rules[5] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
			"· 无协议：不会真实入包 / 不计真实杀怪");

		progress_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "等级范围怪物");
		claim_label = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE, "领取礼物");
		claimed_mark = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE, "V");
	}

	void UIDailyGift::init_digit_glyphs()
	{
		// 每位独立字形，运行时按位拼数字（对等 docs digits/digit_N.png）
		for (int i = 0; i < 10; ++i)
		{
			const std::string ch(1, static_cast<char>('0' + i));
			digit_glyphs[i] = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, ch);
			digit_glyphs_date[i] = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, ch);
		}

		day_prefix = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "第");
		day_suffix = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "天");
		date_month_unit = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, "月");
		date_day_unit = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, "日");
		date_range_sep = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, " ~ ");
		date_bang = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, "!");
	}

	void UIDailyGift::init_fake_data()
	{
		for (int16_t i = 0; i < kTotalDays; ++i)
		{
			const int16_t day = static_cast<int16_t>(i + 1);
			cells[i] = is_highlight_day(day) ? CellVisual::Highlight : CellVisual::Available;
		}

		const float ratio = fake_kill_need > 0
			? static_cast<float>(fake_kill_current) / static_cast<float>(fake_kill_need)
			: 0.0f;
		progress_visual = ratio;
	}

	bool UIDailyGift::is_highlight_day(int16_t day_1based) const
	{
		for (int16_t d : kHighlightDays)
		{
			if (d == day_1based)
				return true;
		}
		return false;
	}

	Point<int16_t> UIDailyGift::slot_origin(int16_t day_index) const
	{
		const int16_t col = static_cast<int16_t>(day_index % kCols);
		const int16_t row = static_cast<int16_t>(day_index / kCols);
		const int16_t grid_top = kTitleH + kBannerH + 8;

		return Point<int16_t>(
			static_cast<int16_t>(kGridPadX + col * (kSlotW + kSlotGapX)),
			static_cast<int16_t>(grid_top + kGridPadY + row * (kSlotH + kSlotGapY)));
	}

	Rectangle<int16_t> UIDailyGift::slot_bounds(int16_t day_index) const
	{
		Point<int16_t> o = slot_origin(day_index);
		return Rectangle<int16_t>(o, o + Point<int16_t>(kSlotW, kSlotH));
	}

	int16_t UIDailyGift::slot_at(Point<int16_t> cursor_offset) const
	{
		for (int16_t i = 0; i < kTotalDays; ++i)
		{
			if (slot_bounds(i).contains(cursor_offset))
				return i;
		}
		return -1;
	}

	int16_t UIDailyGift::draw_digits(int32_t value, Point<int16_t> origin, float alpha, bool zero_pad2, bool date_style) const
	{
		if (value < 0)
			value = 0;

		std::vector<int> digits;
		if (value == 0)
		{
			digits.push_back(0);
		}
		else
		{
			int32_t v = value;
			while (v > 0)
			{
				digits.push_back(v % 10);
				v /= 10;
			}
			for (size_t i = 0, j = digits.size(); i + 1 < j; ++i, --j)
				std::swap(digits[i], digits[j - 1]);
		}

		if (zero_pad2 && digits.size() < 2)
			digits.insert(digits.begin(), 0);

		const auto& glyphs = date_style ? digit_glyphs_date : digit_glyphs;
		int16_t x = 0;
		for (int d : digits)
		{
			const int idx = (d >= 0 && d <= 9) ? d : 0;
			glyphs[idx].draw(DrawArgument(origin + Point<int16_t>(x, 0), alpha));
			x = static_cast<int16_t>(x + kDigitAdvance);
		}
		return x;
	}

	void UIDailyGift::draw_day_label(int16_t day_1based, Point<int16_t> slot_abs, float alpha) const
	{
		// 「第」+ digits + 「天」居中于格子顶部
		const int16_t num_w = (day_1based >= 10) ? static_cast<int16_t>(2 * kDigitAdvance) : kDigitAdvance;
		const int16_t prefix_w = day_prefix.width();
		const int16_t suffix_w = day_suffix.width();
		const int16_t total_w = static_cast<int16_t>(prefix_w + num_w + suffix_w);
		Point<int16_t> cursor(
			static_cast<int16_t>(slot_abs.x() + (kSlotW - total_w) / 2),
			static_cast<int16_t>(slot_abs.y() + 3));

		day_prefix.draw(DrawArgument(cursor, alpha));
		cursor = cursor + Point<int16_t>(prefix_w, 0);
		const int16_t used = draw_digits(day_1based, cursor, alpha, false);
		cursor = cursor + Point<int16_t>(used, 0);
		day_suffix.draw(DrawArgument(cursor, alpha));
	}

	void UIDailyGift::draw_date_range(Point<int16_t> origin, float alpha) const
	{
		// MM月 DD日 ~ MM月 DD日!
		Point<int16_t> cursor = origin;
		auto draw_md = [&](int16_t month, int16_t day)
		{
			cursor = cursor + Point<int16_t>(draw_digits(month, cursor, alpha, true, true), 0);
			date_month_unit.draw(DrawArgument(cursor, alpha));
			cursor = cursor + Point<int16_t>(date_month_unit.width() + 2, 0);
			cursor = cursor + Point<int16_t>(draw_digits(day, cursor, alpha, true, true), 0);
			date_day_unit.draw(DrawArgument(cursor, alpha));
			cursor = cursor + Point<int16_t>(date_day_unit.width(), 0);
		};

		draw_md(date_month_start, date_day_start);
		date_range_sep.draw(DrawArgument(cursor, alpha));
		cursor = cursor + Point<int16_t>(date_range_sep.width(), 0);
		draw_md(date_month_end, date_day_end);
		date_bang.draw(DrawArgument(cursor, alpha));
	}

	void UIDailyGift::draw(float inter) const
	{
		const float alpha = open_alpha;
		(void)inter;

		window_bg.draw(DrawArgument(position, alpha));
		title_bar.draw(DrawArgument(position, alpha));
		title_text.draw(DrawArgument(position + Point<int16_t>(kWinW / 2, 3), alpha));

		const Point<int16_t> banner_pos = position + Point<int16_t>(4, kTitleH + 2);
		banner_box.draw(DrawArgument(banner_pos, alpha));
		banner_sub.draw(DrawArgument(banner_pos + Point<int16_t>(12, 10), alpha));
		banner_main.draw(DrawArgument(banner_pos + Point<int16_t>(12, 32), alpha));

		ColorBox bubble(170, 28, Color::Name::ENDEAVOUR, 0.95f);
		bubble.draw(DrawArgument(banner_pos + Point<int16_t>(kWinW - 210, 22), alpha));
		draw_date_range(banner_pos + Point<int16_t>(kWinW - 200, 28), alpha);

		const int16_t grid_top = kTitleH + kBannerH + 8;
		grid_bg.draw(DrawArgument(position + Point<int16_t>(4, grid_top), alpha));

		for (int16_t i = 0; i < kTotalDays; ++i)
			draw_slot(i, alpha);

		const int16_t footer_y = kWinH - kFooterH;
		footer_box.draw(DrawArgument(position + Point<int16_t>(4, footer_y + 4), alpha));

		for (int i = 0; i < 6; ++i)
			footer_rules[i].draw(DrawArgument(position + Point<int16_t>(14, footer_y + 10 + i * 14), alpha));

		draw_progress_bar(position + Point<int16_t>(kWinW - 180, footer_y + 18));
		draw_claim_button(position + Point<int16_t>(kWinW - 150, kWinH - 52));

		UIElement::draw_buttons(alpha);
	}

	void UIDailyGift::draw_slot(int16_t day_index, float alpha) const
	{
		const Point<int16_t> abs = position + slot_origin(day_index);
		const bool claimed = cells[day_index] == CellVisual::Claimed;
		const bool highlight = cells[day_index] == CellVisual::Highlight ||
			(cells[day_index] != CellVisual::Claimed && is_highlight_day(static_cast<int16_t>(day_index + 1)));

		if (highlight && !claimed)
			slot_highlight.draw(DrawArgument(abs, alpha));
		else
			slot_border.draw(DrawArgument(abs, alpha));

		slot_fill.draw(DrawArgument(abs + Point<int16_t>(1, 1), alpha));

		if (claimed)
			slot_claimed.draw(DrawArgument(abs + Point<int16_t>(1, 1), alpha * 0.9f));

		if (hover_slot == day_index && !claimed)
			slot_hover.draw(DrawArgument(abs, alpha));
		if (pressed_slot == day_index && !claimed)
			slot_pressed.draw(DrawArgument(abs, alpha));

		draw_day_label(static_cast<int16_t>(day_index + 1), abs, alpha);

		ColorBox mvp(12, 12, highlight ? Color::Name::SUPERNOVA : Color::Name::ORANGE, 1.0f);
		mvp.draw(DrawArgument(abs + Point<int16_t>(3, 3), alpha));

		// 中部道具槽预留：空槽，不填 ItemData / 假道具 / 图标 / 名称
		item_slot_empty.draw(DrawArgument(abs + Point<int16_t>((kSlotW - 36) / 2, 26), alpha));

		if (claimed)
		{
			claimed_badge.draw(DrawArgument(abs + Point<int16_t>(kSlotW - 22, 4), alpha));
			claimed_mark.draw(DrawArgument(abs + Point<int16_t>(kSlotW - 13, 5), alpha));
		}

		if (flash_slot == day_index && flash_remain > 0)
		{
			const float flash_a = alpha * (static_cast<float>(flash_remain) / 18.0f);
			if (has_newitem_flash)
				newitem_flash.draw(DrawArgument(abs + Point<int16_t>(1, 1), flash_a), 1.0f);
			else
				flash_box.draw(DrawArgument(abs, flash_a));
		}
	}

	void UIDailyGift::draw_progress_bar(Point<int16_t> origin) const
	{
		progress_label.draw(origin);
		progress_track.draw(origin + Point<int16_t>(0, 16));

		const int16_t fill_w = static_cast<int16_t>(160.0f * progress_visual);
		if (fill_w > 0)
		{
			ColorBox fill(fill_w, 10, Color::Name::PERSIANGREEN, 1.0f);
			fill.draw(origin + Point<int16_t>(0, 16));
		}

		// 进度数字用 0-9 拼装：CCC / NNN
		Point<int16_t> num_pos = origin + Point<int16_t>(0, 30);
		num_pos = num_pos + Point<int16_t>(draw_digits(fake_kill_current, num_pos, 1.0f, true), 0);
		// 补一位使三位数观感（假进度常 <100）
		Text slash(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::LIGHTGREEN, " / ");
		slash.draw(num_pos);
		num_pos = num_pos + Point<int16_t>(slash.width(), 0);
		draw_digits(fake_kill_need, num_pos, 1.0f, true);
	}

	void UIDailyGift::draw_claim_button(Point<int16_t> origin) const
	{
		Color::Name color = Color::Name::PERSIANGREEN;
		float opc = 1.0f;
		if (claim_pressed)
		{
			color = Color::Name::JAPANESELAUREL;
			opc = 0.85f;
		}
		else if (claim_hover)
		{
			color = Color::Name::LIGHTGREEN;
		}

		ColorBox btn(130, 36, color, opc);
		btn.draw(origin);
		claim_label.draw(origin + Point<int16_t>(65, 10));
	}

	void UIDailyGift::update()
	{
		UIElement::update();

		if (open_alpha < 1.0f)
		{
			open_alpha += 0.08f;
			if (open_alpha > 1.0f)
				open_alpha = 1.0f;
		}

		const float target = fake_kill_need > 0
			? static_cast<float>(fake_kill_current) / static_cast<float>(fake_kill_need)
			: 0.0f;
		const float step = (target - progress_visual) / 12.0f;
		if (std::abs(step) > 0.0001f)
		{
			progress_visual += step;
			if ((step > 0.0f && progress_visual > target) || (step < 0.0f && progress_visual < target))
				progress_visual = target;
		}

		if (flash_remain > 0)
		{
			--flash_remain;
			if (has_newitem_flash)
				newitem_flash.update(6);
			if (flash_remain <= 0)
				flash_slot = -1;
		}
	}

	void UIDailyGift::remove_cursor()
	{
		UIDragElement::remove_cursor();
		hover_slot = -1;
		pressed_slot = -1;
		claim_hover = false;
		claim_pressed = false;
	}

	Cursor::State UIDailyGift::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State drag_state = UIDragElement::send_cursor(clicked, cursorpos);
		if (dragged)
			return drag_state;

		Point<int16_t> cursor_offset = cursorpos - position;

		const int16_t claim_x = kWinW - 150;
		const int16_t claim_y = kWinH - 52;
		Rectangle<int16_t> claim_rc(
			Point<int16_t>(claim_x, claim_y),
			Point<int16_t>(claim_x + 130, claim_y + 36));

		claim_hover = claim_rc.contains(cursor_offset);
		if (!clicked)
			claim_pressed = false;

		int16_t slot = slot_at(cursor_offset);
		hover_slot = slot;

		if (clicked)
		{
			if (claim_hover)
				claim_pressed = true;
			pressed_slot = slot;
		}
		else
		{
			pressed_slot = -1;
		}

		Cursor::State btn_state = UIElement::send_cursor(clicked, cursorpos);
		if (btn_state == Cursor::State::CANCLICK || claim_hover || slot >= 0)
			return Cursor::State::CANCLICK;

		return btn_state;
	}

	void UIDailyGift::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { close(); return; }
		if (!pressed) return;
		if (keycode == 'C' || keycode == 'c' || keycode == 'S' || keycode == 's')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int16_t day = static_cast<int16_t>(std::stoi(s));
					if (day < 0 || day >= kTotalDays) return;
					fake_claim_slot(day);
					LOG(LOG_DEBUG, "UIDailyGift: SoftKey claim day=" << day);
				} catch (...) {}
			}, []() {}, "Gift day");
		}
	}

	UIElement::Type UIDailyGift::get_type() const
	{
		return TYPE;
	}

	Button::State UIDailyGift::button_pressed(uint16_t buttonid)
	{
		if (buttonid == Buttons::CLOSE)
		{
			close();
			return Button::State::NORMAL;
		}

		if (buttonid == Buttons::CLAIM)
		{
			for (int16_t i = 0; i < kTotalDays; ++i)
			{
				if (cells[i] != CellVisual::Claimed)
				{
					fake_claim_slot(i);
					break;
				}
			}
			return Button::State::NORMAL;
		}

		if (buttonid >= Buttons::SLOT_BASE && buttonid < Buttons::SLOT_BASE + kTotalDays)
		{
			const int16_t idx = static_cast<int16_t>(buttonid - Buttons::SLOT_BASE);
			fake_claim_slot(idx);
			return Button::State::NORMAL;
		}

		return Button::State::NORMAL;
	}

	void UIDailyGift::fake_claim_slot(int16_t day_index)
	{
		if (day_index < 0 || day_index >= kTotalDays)
			return;
		if (cells[day_index] == CellVisual::Claimed)
			return;
		try_claim(static_cast<int16_t>(day_index + 1));
	}

	void UIDailyGift::play_claim_flash(int16_t day_index)
	{
		flash_slot = day_index;
		flash_remain = 18;
		if (has_newitem_flash)
			newitem_flash.reset();
		Sound(Sound::Name::BUTTONCLICK).play();
	}

	void UIDailyGift::close()
	{
		LOG(LOG_INFO, "UIDailyGift close");
		deactivate();
	}

	bool UIDailyGift::try_claim(int16_t day)
	{
		const int16_t idx = static_cast<int16_t>(day - 1);
		if (idx < 0 || idx >= kTotalDays)
			return false;
		if (cells[idx] == CellVisual::Claimed)
			return false;

		cells[idx] = CellVisual::Claimed;
		play_claim_flash(idx);
		LOG(LOG_INFO, "UIDailyGift fake claim day=" << day);
		return true;
	}

	void UIDailyGift::on_claim_result(int16_t day, bool success, int32_t result_code)
	{
		(void)day;
		(void)success;
		(void)result_code;
	}

	void UIDailyGift::set_kill_progress(int32_t current, int32_t need)
	{
		fake_kill_current = current;
		fake_kill_need = need > 0 ? need : 300;
	}

	void UIDailyGift::set_account_daily_limit(bool reached)
	{
		(void)reached;
	}

	void UIDailyGift::set_level_gate(int16_t min_level)
	{
		(void)min_level;
	}

	void UIDailyGift::set_mvp_tier(int8_t tier)
	{
		(void)tier;
	}
}
