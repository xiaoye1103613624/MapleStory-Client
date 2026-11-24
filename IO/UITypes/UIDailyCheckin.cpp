//////////////////////////////////////////////////////////////////////////////////
// UIDailyCheckin.cpp — 28天签到窗（按 v4 整窗栅格 + cell/v1 状态图还原）
// 整窗 normal.png 513x346 已烧中文"每日签到/第N天"；格子几何来自 manifest:
//   单格 67x71, 原点(7,59), X间距5(即 pitch=72), 行距72。
// 状态叠加: 未解锁=mask 灰蒙版 / 已领取=claimed 绿图 + label "已领取" 角标 / 可领=空态不蒙。
//////////////////////////////////////////////////////////////////////////////////
#include "UIDailyCheckin.h"

#include "../UI.h"
#include "../Components/AreaButton.h"
#include "../Components/MapleButton.h"
#include "../../Net/Packets/DailyCheckinPackets.h"
#include "../../Data/ItemData.h"
#include "../../Util/UiAssetResolve.h"
#include "../../MapleStory.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIDailyCheckin::UIDailyCheckin()
	{
		dimension = Point<int16_t>(WIN_W, WIN_H);
		dragarea = Point<int16_t>(WIN_W, HEADER_H);
		layout_policy = LayoutPolicy::SavedPosClamped;

		hint = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::DARKGREY, "");

#ifdef USE_NX
		nl::node dc = UiAssetResolve::window("DailyCheckin");
		nl::node bg = dc["backgrnd"];
		if (UiAssetResolve::usable(bg))
		{
			backgrnd = Texture(bg);
			has_assets = true;
			dimension = Point<int16_t>(backgrnd.width(), backgrnd.height());
			dragarea = Point<int16_t>(dimension.x(), HEADER_H);
		}
		nl::node cell = dc["cell"];
		if (UiAssetResolve::usable(cell["normal"])) cell_normal = Texture(cell["normal"]);
		if (UiAssetResolve::usable(cell["mask"]))   cell_mask   = Texture(cell["mask"]);
		if (UiAssetResolve::usable(cell["claimed"])) cell_claimed = Texture(cell["claimed"]);
		if (UiAssetResolve::usable(cell["label"]))  cell_label  = Texture(cell["label"]);

		// 关闭钮按需求包 README:306 x=490,y=6,12x12（WIN_W=513，标题栏内右侧）。
		if (nl::node close = UiAssetResolve::close_btn())
			buttons[BtClose] = std::make_unique<MapleButton>(close, Point<int16_t>(490, 6));
		else
			buttons[BtClose] = std::make_unique<AreaButton>(Point<int16_t>(490, 6), Point<int16_t>(12, 12));
#else
		buttons[BtClose] = std::make_unique<AreaButton>(Point<int16_t>(490, 6), Point<int16_t>(12, 12));
#endif

		for (int16_t d = 1; d <= DAYS; ++d)
		{
			const int16_t col = static_cast<int16_t>((d - 1) % COLS);
			const int16_t row = static_cast<int16_t>((d - 1) / COLS);
			buttons[static_cast<uint16_t>(BtCellBase + d)] = std::make_unique<AreaButton>(
				Point<int16_t>(CELL_X0 + col * (CELL_W + CELL_GAP), CELL_Y0 + row * CELL_H),
				Point<int16_t>(CELL_W, CELL_H - 1));
		}

		center_if_position_unset(dimension);
		DailyCheckinOpenPacket().dispatch();
	}

	void UIDailyCheckin::apply_snapshot(int8_t cd, int32_t mask, int8_t jc,
		const std::vector<int32_t>& icon_ids, const std::vector<std::string>& tooltips)
	{
		current_day = cd;
		claimed_mask = mask;
		just_claimed = jc;
		has_snapshot = true;
		reward_icons = icon_ids;
		reward_tips = tooltips;
		refresh_icon_textures();
		rebuild_cells();
	}

	void UIDailyCheckin::refresh_icon_textures()
	{
		// 每格主 icon：ItemData::get(id).get_icon(NORMAL)，缺资源留空 Texture（不崩）。
		reward_icon_textures.clear();
		reward_icon_textures.resize(reward_icons.size());
		for (size_t i = 0; i < reward_icons.size(); ++i)
		{
			if (reward_icons[i] <= 0) continue;
			reward_icon_textures[i] = ItemData::get(reward_icons[i]).get_icon(false);
		}
	}

	void UIDailyCheckin::rebuild_cells()
	{
		if (just_claimed > 0)
			hint.change_text("已领取第 " + std::to_string(just_claimed) + " 天奖励！");
		else if (current_day > 0)
			hint.change_text("第 " + std::to_string(current_day) + " 天可领取，点击领取。");
		else
			hint.change_text("明日再来领取下一份奖励。");
	}

	int16_t UIDailyCheckin::cell_at(Point<int16_t> cursorpos) const
	{
		const Point<int16_t> local = cursorpos - position;
		for (int16_t d = 1; d <= DAYS; ++d)
		{
			const int16_t col = static_cast<int16_t>((d - 1) % COLS);
			const int16_t row = static_cast<int16_t>((d - 1) / COLS);
			const Point<int16_t> p(CELL_X0 + col * (CELL_W + CELL_GAP), CELL_Y0 + row * CELL_H);
			if (Rectangle<int16_t>(p, p + Point<int16_t>(CELL_W, CELL_H - 1)).contains(local))
				return d;
		}
		return -1;
	}

	void UIDailyCheckin::draw(float inter) const
	{
		const float fade = fade_alpha();
		const Point<int16_t> org = position;
		if (has_assets && backgrnd)
			backgrnd.draw(DrawArgument(org) + fade);

		for (int16_t d = 1; d <= DAYS; ++d)
		{
			const int16_t col = static_cast<int16_t>((d - 1) % COLS);
			const int16_t row = static_cast<int16_t>((d - 1) / COLS);
			const Point<int16_t> p = org + Point<int16_t>(
				CELL_X0 + col * (CELL_W + CELL_GAP), CELL_Y0 + row * CELL_H);
			const bool claimed = (claimed_mask & (1 << (d - 1))) != 0;
			if (claimed && cell_claimed)
			{
				cell_claimed.draw(DrawArgument(p) + fade);
				if (cell_label)
					cell_label.draw(DrawArgument(p + Point<int16_t>((CELL_W - cell_label.width()) / 2, 4)) + fade);
			}
			else if (!claimed && (!has_snapshot || d > current_day) && cell_mask)
			{
				cell_mask.draw(DrawArgument(p) + fade);
			}

			// 主物品 icon：画在粉头"第N天"下方的内容区中央（v4 manifest: 粉头高19，单格67x71）。
			// 之前按整格垂直居中会把 icon 压到粉头文字上（截图实证），这里改内容区居中。
			const size_t idx = static_cast<size_t>(d - 1);
			if (idx < reward_icon_textures.size() && reward_icon_textures[idx])
			{
				const Texture& tex = reward_icon_textures[idx];
				const int16_t body_h = CELL_H - CELL_HEADER_H;   // 52px
				const Point<int16_t> iconp = p + Point<int16_t>(
					(CELL_W - tex.width()) / 2,
					CELL_HEADER_H + (body_h - tex.height()) / 2);
				tex.draw(DrawArgument(iconp) + fade);
			}
		}

		// 引导文字"每天回来领取更多奖励！"已烧进 v4 整窗蓝条，不再重复绘制 Text
		// （之前 draw 在 WIN_H-16 还压住第4行格子底部、被裁切）。
		UIElement::draw_buttons(inter);
	}

	void UIDailyCheckin::update()
	{
		UIElement::update();
	}

	Button::State UIDailyCheckin::button_pressed(uint16_t buttonid)
	{
		if (buttonid == BtClose) { close(); return Button::State::PRESSED; }
		if (buttonid >= BtCellBase + 1 && buttonid <= BtCellBase + DAYS)
		{
			const int16_t d = static_cast<int16_t>(buttonid - BtCellBase);
			const bool claimed = (claimed_mask & (1 << (d - 1))) != 0;
			if (has_snapshot && d == current_day && !claimed)
			{
				DailyCheckinClaimPacket(d).dispatch();
				return Button::State::PRESSED;
			}
		}
		return Button::State::NORMAL;
	}

	Cursor::State UIDailyCheckin::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		// 关闭钮与格子命中区先从 dragarea 摘出（对比 UIDamageRank.cpp:285-299），否则点击被拖拽吞掉。
		const Point<int16_t> local = cursorpos - position;
		if (buttons[BtClose] && buttons[BtClose]->is_active() &&
			buttons[BtClose]->bounds(position).contains(cursorpos))
			return UIElement::send_cursor(clicked, cursorpos);

		const int16_t day = cell_at(cursorpos);
		if (day > 0)
		{
			// hover tooltip：显示服务端该天奖励明细（物品名+数量+mesos，需求包 §6）。
			if (hover_day != day)
			{
				hover_day = day;
				const size_t idx = static_cast<size_t>(day - 1);
				if (idx < reward_tips.size() && !reward_tips[idx].empty())
					UI::get().show_text(Tooltip::Parent::TEXT, reward_tips[idx]);
			}
			return UIElement::send_cursor(clicked, cursorpos);
		}
		if (hover_day != -1)
		{
			hover_day = -1;
			UI::get().clear_tooltip(Tooltip::Parent::TEXT);
		}
		return UIDragElement::send_cursor(clicked, cursorpos);
	}

	void UIDailyCheckin::remove_cursor()
	{
		hover_day = -1;
		UI::get().clear_tooltip(Tooltip::Parent::TEXT);
		UIDragElement::remove_cursor();
	}

	void UIDailyCheckin::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { close(); return; }
	}

	void UIDailyCheckin::close()
	{
		remove_cursor();
		deactivate();
	}

	UIElement::Type UIDailyCheckin::get_type() const
	{
		return TYPE;
	}
}
