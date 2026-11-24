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
#include "UIDailyGift.h"

#include "UISoftKey.h"

#include "../UI.h"
#include "../Components/AreaButton.h"
#include "../Components/MapleButton.h"

#include "../../Audio/Audio.h"
#include "../../Data/ItemData.h"
#include "../../MapleStory.h"
#include "../../Net/Packets/DailyGiftPackets.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#include <algorithm>
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
		progress_visual = 0.0f;
		open_alpha = 0.0f;
		flash_slot = -1;
		flash_remain = 0;
		date_month_start = 10;
		date_day_start = 1;
		date_month_end = 10;
		date_day_end = 31;
		// 真数据未到前的保守初值：全锁、无图标，避免闪出「假可领」误导。
		for (int16_t i = 0; i < kTotalDays; ++i)
			cells[i] = CellVisual::Locked;

		init_layout();
		init_digit_glyphs();
		rebuild_cells();
		refresh_status_texts();
		result_text = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::YELLOW, "");

		// 192 原生资源优先：UiAssetResolve::window4("dailyGift")（节点名小写，与
		// ShortCutUI 范式一致）。资源由 lead 统一入库；有则画整窗 backgrnd，
		// 无则回退既有 ColorBox 灰框。原生节点缺失不应影响协议/交互。
#ifdef USE_NX
		// 192 原生资源经 orange-wz batch_convert_key(192零IV→GMS) 后，以独立
		// Data/UI/DailyGiftStage.img 入库（散装 img 会由 ImgFiles 合并进 UI.nx）。
		// 先查独立 img 根，再回退 window4("dailyGift")，兼容并进 UIWindow4.img 的历史方案。
		nl::node gift = nl::nx::UI["DailyGiftStage.img"]["dailyGift"];
		if (!gift)
			gift = UiAssetResolve::window4("dailyGift");
		nl::node bg = gift["backgrnd"];
		if (UiAssetResolve::usable(bg))
		{
			native_bg = Texture(bg);
			has_native_bg = native_bg.is_valid();
			if (has_native_bg)
			{
				// 原生 741×647 整窗：以贴图真实尺寸为准，保持既有拖动/关闭行为。
				const Point<int16_t> native_dim(native_bg.width(), native_bg.height());
				dimension = native_dim;
				dragarea = Point<int16_t>(native_dim.x(), kTitleH);
			}
		}
		if (!has_native_bg)
			LOG(LOG_WARN, "[DailyGift] 192 native window4(\"dailyGift\")/backgrnd unavailable; gray-box fallback");
#endif

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

		if (!has_native_bg)
		{
			dimension = Point<int16_t>(kWinW, kWinH);
			dragarea = Point<int16_t>(kWinW, kTitleH);
		}

		// 构造即请求配置快照（侧栏点击发 OPEN → 0x3722/0 → handler emplace 本窗）。
		// 若 handler 是 emplace 本窗后到达的快照，本次 OPEN 回包时本窗已存在，
		// 直接进入 apply_snapshot，不会重复开窗。
		DailyGiftOpenPacket().dispatch();
		LOG(LOG_INFO, "[DailyGift] open; native_bg=" << has_native_bg
			<< "; 0x3721 OPEN dispatched");
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
		// 道具槽底板：真图标先画在其上，再叠底板透明蒙层统一观感。
		item_slot_empty = ColorBox(36, 36, Color::Name::MINESHAFT, 0.55f);
		claimed_badge = ColorBox(18, 18, Color::Name::PERSIANGREEN, 1.0f);
		progress_track = ColorBox(160, 10, Color::Name::DARKGREY, 1.0f);
		progress_fill = ColorBox(1, 10, Color::Name::PERSIANGREEN, 1.0f);
		claim_btn = ColorBox(130, 36, Color::Name::PERSIANGREEN, 1.0f);
		flash_box = ColorBox(kSlotW, kSlotH, Color::Name::WHITE, 0.6f);

		// 静态文案进 locale 绑定表（切语言后已开窗立刻换字）；动态值另走 change_text。
		title_text = Text(Text::Font::A11B, Text::Alignment::CENTER, Color::Name::YELLOW);
		bind_text(title_text, "dailygift.title");
		banner_sub = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE);
		bind_text(banner_sub, "dailygift.banner_sub");
		banner_main = Text(Text::Font::A18M, Text::Alignment::LEFT, Color::Name::WHITE);
		bind_text(banner_main, "dailygift.banner_main");

		for (int i = 0; i < 6; ++i)
		{
			footer_rules[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE);
			bind_text(footer_rules[i], "dailygift.rule_" + std::to_string(i + 1));
		}

		progress_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE);
		bind_text(progress_label, "dailygift.progress");
		claim_label = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE);
		bind_text(claim_label, "dailygift.claim");
		claimed_mark = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE, "V");

		status_text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "");
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

		day_prefix = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE);
		bind_text(day_prefix, "dailygift.day_prefix");
		day_suffix = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE);
		bind_text(day_suffix, "dailygift.day_suffix");
		date_month_unit = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE);
		bind_text(date_month_unit, "dailygift.date_month");
		date_day_unit = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE);
		bind_text(date_day_unit, "dailygift.date_day");
		date_range_sep = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, " ~ ");
		date_bang = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, "!");
	}

	void UIDailyGift::apply_snapshot(int8_t cd, int32_t mask, int8_t lmin, int8_t qdone,
		int32_t kills, int8_t premium, const std::array<int32_t, kTotalDays>& ids,
		const std::array<int32_t, kTotalDays>& counts)
	{
		current_day = cd;
		claimed_mask = mask;
		level_min = lmin;
		quest_done = qdone;
		kill_count = kills;
		premium_left = premium;
		reward_item_ids = ids;
		reward_counts = counts;
		has_snapshot = true;

		refresh_icon_textures();
		rebuild_cells();
		refresh_status_texts();

		LOG(LOG_INFO, "[DailyGift] apply_snapshot day=" << static_cast<int>(cd)
			<< " mask=" << mask << " levelMin=" << static_cast<int>(lmin)
			<< " questDone=" << static_cast<int>(qdone) << " kills=" << kills
			<< " premiumLeft=" << static_cast<int>(premium));
	}

	void UIDailyGift::refresh_icon_textures()
	{
		// 28 格真图标：ItemData::get(id).get_icon(false)（NORMAL，非 cash 变体）。
		// 服务端未下发 / id<=0 的格子留空 Texture，draw_slot 走空槽底板，不崩。
		for (int16_t i = 0; i < kTotalDays; ++i)
		{
			reward_icons[i] = Texture();
			if (reward_item_ids[i] > 0)
				reward_icons[i] = ItemData::get(reward_item_ids[i]).get_icon(false);
		}
	}

	void UIDailyGift::rebuild_cells()
	{
		for (int16_t i = 0; i < kTotalDays; ++i)
		{
			const int16_t day = static_cast<int16_t>(i + 1);
			const bool claimed = (claimed_mask & (1 << i)) != 0;
			if (claimed)
				cells[i] = CellVisual::Claimed;
			else if (has_snapshot && day <= current_day)
				cells[i] = CellVisual::Available;
			else
				cells[i] = CellVisual::Locked;
		}

		// 进度条动画目标（真 killCount / 300）。
		const float target = kKillTarget > 0
			? std::min(1.0f, static_cast<float>(std::max(0, kill_count)) / static_cast<float>(kKillTarget))
			: 0.0f;
		progress_visual = target;
	}

	void UIDailyGift::refresh_status_texts()
	{
		// 等级门 + premiumLeft 均来自快照，事件驱动更新，故不进 locale 绑定表。
		const std::string gate = Locale::format("dailygift.level_gate", std::to_string(static_cast<int>(level_min)));
		const std::string prem = Locale::format("dailygift.premium_left", std::to_string(static_cast<int>(premium_left)));
		status_text.change_text(gate + "   " + prem);
	}

	bool UIDailyGift::is_highlight_day(int16_t day_1based) const
	{
		// premium 日的金色角标仅作视觉装饰；可领/已领/锁定仍以 cells 为准。
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

		if (has_native_bg)
		{
			// 192 原生整窗 backgrnd（741×647）：整窗一张贴图，格子/文字仍按其上布局绘制。
			native_bg.draw(DrawArgument(position, alpha));
		}
		else
		{
			window_bg.draw(DrawArgument(position, alpha));
			title_bar.draw(DrawArgument(position, alpha));
		}
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

		// 等级门 / premiumLeft 与最近领奖结果提示（动态文案）。
		status_text.draw(DrawArgument(position + Point<int16_t>(14, footer_y + 4), alpha));
		result_text.draw(DrawArgument(position + Point<int16_t>(kWinW / 2, kWinH - 72), alpha));

		UIElement::draw_buttons(alpha);
	}

	void UIDailyGift::draw_slot(int16_t day_index, float alpha) const
	{
		const Point<int16_t> abs = position + slot_origin(day_index);
		const CellVisual state = cells[day_index];
		const bool claimed = state == CellVisual::Claimed;
		const bool locked = state == CellVisual::Locked;
		const bool available = state == CellVisual::Available;

		if (available)
			slot_highlight.draw(DrawArgument(abs, alpha)); // 可领格高亮
		else
			slot_border.draw(DrawArgument(abs, alpha));

		slot_fill.draw(DrawArgument(abs + Point<int16_t>(1, 1), alpha));

		// 真图标：服务端下发 rewardItemIds[day] → ItemData 图标画在格子内容区中央。
		// 锁定格整体压暗（灰蒙），已领格另叠灰蒙 + 对勾。
		const Texture& icon = reward_icons[day_index];
		if (icon.is_valid())
		{
			const Point<int16_t> icon_pos(
				static_cast<int16_t>((kSlotW - icon.width()) / 2),
				26);
			icon.draw(DrawArgument(abs + icon_pos, locked ? alpha * 0.35f : alpha));
		}
		else
		{
			item_slot_empty.draw(DrawArgument(abs + Point<int16_t>((kSlotW - 36) / 2, 26), alpha));
		}

		if (locked)
		{
			// 未解锁：整格灰蒙，不带对勾。
			ColorBox lock( kSlotW - 2, kSlotH - 2, Color::Name::DARKGREY, 0.65f);
			lock.draw(DrawArgument(abs + Point<int16_t>(1, 1), alpha));
		}
		else if (claimed)
		{
			// 已领：灰蒙 + 对勾。
			slot_claimed.draw(DrawArgument(abs + Point<int16_t>(1, 1), alpha * 0.9f));
			claimed_badge.draw(DrawArgument(abs + Point<int16_t>(kSlotW - 22, 4), alpha));
			claimed_mark.draw(DrawArgument(abs + Point<int16_t>(kSlotW - 13, 5), alpha));
		}

		if (hover_slot == day_index && !claimed && !locked)
			slot_hover.draw(DrawArgument(abs, alpha));
		if (pressed_slot == day_index && !claimed && !locked)
			slot_pressed.draw(DrawArgument(abs, alpha));

		draw_day_label(static_cast<int16_t>(day_index + 1), abs, alpha);

		// premium 日金色角标（视觉装饰，与领取状态无关）。
		if (is_highlight_day(static_cast<int16_t>(day_index + 1)))
		{
			ColorBox mvp(12, 12, Color::Name::SUPERNOVA, 1.0f);
			mvp.draw(DrawArgument(abs + Point<int16_t>(3, 3), alpha));
		}

		if (flash_slot == day_index && flash_remain > 0)
		{
			// 视觉闪光仅由本地点击触发；格子是否变「已领」完全由 0x3722 结果驱动。
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

		// 进度数字用 0-9 拼装：真 killCount / 300。
		Point<int16_t> num_pos = origin + Point<int16_t>(0, 30);
		num_pos = num_pos + Point<int16_t>(draw_digits(kill_count, num_pos, 1.0f, true), 0);
		Text slash(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::LIGHTGREEN, " / ");
		slash.draw(num_pos);
		num_pos = num_pos + Point<int16_t>(slash.width(), 0);
		draw_digits(kKillTarget, num_pos, 1.0f, true);
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

		const float target = kKillTarget > 0
			? std::min(1.0f, static_cast<float>(std::max(0, kill_count)) / static_cast<float>(kKillTarget))
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

	Cursor::State UIDailyGift::send_cursor(bool clicked, Point<int16_t> cursor_offset)
	{
		Cursor::State drag_state = UIDragElement::send_cursor(clicked, cursor_offset);
		if (dragged)
			return drag_state;

		Point<int16_t> local = cursor_offset - position;

		const int16_t claim_x = kWinW - 150;
		const int16_t claim_y = kWinH - 52;
		Rectangle<int16_t> claim_rc(
			Point<int16_t>(claim_x, claim_y),
			Point<int16_t>(claim_x + 130, claim_y + 36));

		claim_hover = claim_rc.contains(local);
		if (!clicked)
			claim_pressed = false;

		int16_t slot = slot_at(local);
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

		Cursor::State btn_state = UIElement::send_cursor(clicked, cursor_offset);
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
			// 保留既有空行为（关闭/Layout 等不变）。
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
			// 底部「领取礼物」：领当前日（若可领）。
			if (has_snapshot && current_day > 0 && current_day <= kTotalDays)
			{
				const int16_t idx = static_cast<int16_t>(current_day - 1);
				if (cells[idx] == CellVisual::Available)
					request_claim(current_day);
			}
			return Button::State::NORMAL;
		}

		if (buttonid >= Buttons::SLOT_BASE && buttonid < Buttons::SLOT_BASE + kTotalDays)
		{
			const int16_t idx = static_cast<int16_t>(buttonid - Buttons::SLOT_BASE);
			request_claim(static_cast<int16_t>(idx + 1));
			return Button::State::NORMAL;
		}

		return Button::State::NORMAL;
	}

	void UIDailyGift::request_claim(int16_t day)
	{
		const int16_t idx = static_cast<int16_t>(day - 1);
		if (!has_snapshot || idx < 0 || idx >= kTotalDays)
			return;
		if (cells[idx] != CellVisual::Available)
			return; // 已领 / 未解锁不可发领取包

		// 真实领取只发 0x3721 CLAIM；本地仅保留闪光动画，不预置「已领」态。
		DailyGiftClaimPacket(day).dispatch();
		play_claim_flash(idx);
		LOG(LOG_INFO, "[DailyGift] 0x3721 CLAIM day=" << day);
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
		LOG(LOG_INFO, "[DailyGift] close");
		deactivate();
	}

	void UIDailyGift::on_claim_result(int8_t result_code, int8_t day, int32_t item_id, int32_t count)
	{
		// 真实结果由服务端驱动：仅成功码把对应格置为已领；失败码给出可见提示。
		static const char* kFailHint[] = {
			"", "等级不足", "杀怪不足", "已领过", "非当前日", "月限已尽"
		};

		if (result_code == 0)
		{
			const int16_t idx = static_cast<int16_t>(day - 1);
			if (idx >= 0 && idx < kTotalDays)
			{
				claimed_mask |= (1 << idx);
				cells[idx] = CellVisual::Claimed;
			}
			result_text.change_text(Locale::format("dailygift.claim_ok",
				std::to_string(static_cast<int>(day)) + " x" + std::to_string(count)));
			LOG(LOG_INFO, "[DailyGift] claim OK day=" << static_cast<int>(day)
				<< " item=" << item_id << " count=" << count);
		}
		else
		{
			const int code = static_cast<int>(result_code);
			const std::string hint = (code >= 1 && code <= 5)
				? kFailHint[code] : "领取失败";
			result_text.change_text(Locale::format("dailygift.claim_fail", hint));
			// 失败不推进格子状态；闪光结束后自然消失，不残留假已领。
			LOG(LOG_WARN, "[DailyGift] claim FAIL code=" << code << " day=" << static_cast<int>(day));
		}
	}
}
