//////////////////////////////////////////////////////////////////////////////////
//	HudLayout — classic HUD / 多分辨率自适应布局公式（头文件内实现）
//
//	设计网格来自 Angel 083 CUIStatusBar::OnCreate @ 0x8d01b2：CreateLayer 800×578，
//	快捷格层 151×80；Angel 本身无多分辨率（见 docs/features/multi-resolution/evidence/）。
//
//	用户锁定（偏离 Angel 左贴边）：VWIDTH>content_w 时整组 chrome 水平居中；
//	backgrnd 仍通栏 0..VWIDTH。新分辨率禁止再写 switch(VWIDTH==…) 坐标表；
//	扩档只改 ResolutionTiers.h + SysOpt 文案。
//
//	宽 QS（26/32）禁止塞进 800：content_w = 800 + qs_w，QS 贴在 800 条右侧；
//	仅当 VWIDTH >= 800+qs_w 才启用宽格，否则回退 quickSlot08（4 列），避免
//	1024 等中档宽把 QS26 贴右缘仍压住商城/目录钮。
//
//	像素还原权威（2026-11）：1280×720 为布局 oracle；其它档只验本公式
//	（居中差 / 无重叠 / clamp_to_view）。ChatBar 与 StatusBar 必须共用
//	hud_center_x / hud_content_w。见 docs/features/pixel-ui-angel-restore-2026-11/。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Template/Point.h"

#include <algorithm>
#include <cstdint>

namespace ms
{
	namespace HudLayout
	{
		// Angel StatusBar 设计内容宽（不是登录 LOGIN_W=1280）。
		inline constexpr int16_t HUD_DESIGN_W = 800;
		// StatusBar.img/base/quickSlot* 实测面板宽（格几何见 UIStatusBar 顶部注释）。
		inline constexpr int16_t QS_PANEL_W_08 = 151;
		inline constexpr int16_t QS_PANEL_W_26 = 466;
		inline constexpr int16_t QS_PANEL_W_32 = 571;
		// 宽格最低视口：800+qs26=1266（qs32=1371）。旧 QS_WIDE_MIN_W=1024 在
		// 1024 仍选 QS26 并 qs_x=vw-qs_w，会盖住信息栏右侧功能钮。
		inline constexpr int16_t QS_WIDE_MIN_W =
			static_cast<int16_t>(HUD_DESIGN_W + QS_PANEL_W_26);
		// 宽/窄屏按钮簇相对 QS 左缘的间隙（与 UIStatusBar init_classic 同源）。
		// 2026-11-23 用户要求：底部按钮簇往右移、靠近右侧快捷栏。
		// 原 241 在 1280 宽屏下末钮右缘距 QS 左缘约 19px；改 228 右移 13px，
		// 留约 5-6px（4/8 网格容差）。group_x=qs_x-gap，三处（init_classic/
		// update_screen/iconMemo）共用此常量，改一处即同步，避免上下排错位。
		inline constexpr int16_t QS_GROUP_GAP_WIDE = 228;
		inline constexpr int16_t QS_GROUP_GAP_NARROW = 75;

		// 仅当视口装得下「800 信息栏 + 宽 QS」才用 26/32；否则强制 QS08。
		inline bool is_qs_wide(int16_t view_w, bool slots32 = false)
		{
			const int16_t qs_w = slots32 ? QS_PANEL_W_32 : QS_PANEL_W_26;

			return view_w >= static_cast<int16_t>(HUD_DESIGN_W + qs_w);
		}

		// slots32=true 时宽屏取 quickSlot32；否则 quickSlot26。装不下则恒 08。
		inline int16_t qs_panel_w(int16_t view_w, bool slots32)
		{
			if (!is_qs_wide(view_w, slots32))
				return QS_PANEL_W_08;

			return slots32 ? QS_PANEL_W_32 : QS_PANEL_W_26;
		}

		// 组合内容宽：宽 QS 时 = 800+qs（QS 在 800 条右侧，互不水平重叠）；
		// QS08 时仍 = 800（Angel 网格）。居中与 ChatBar/气球必须共用此值。
		inline int16_t hud_content_w(int16_t view_w, bool slots32)
		{
			if (!is_qs_wide(view_w, slots32))
				return HUD_DESIGN_W;

			return static_cast<int16_t>(HUD_DESIGN_W + qs_panel_w(view_w, slots32));
		}

		// 内容网格宽：过宽用 800，否则跟视口（窄屏不再假装有 800 右缘）。
		// 左 chrome / Angel 网格仍按此宽；宽 QS 超出部分由 hud_content_w 计入居中。
		inline int16_t hud_grid_w(int16_t view_w)
		{
			return view_w > HUD_DESIGN_W ? HUD_DESIGN_W : view_w;
		}

		inline int16_t hud_center_x(int16_t view_w, int16_t content_w)
		{
			return view_w > content_w
				? static_cast<int16_t>((view_w - content_w) / 2)
				: static_cast<int16_t>(0);
		}

		inline int16_t hud_center_x(int16_t view_w, bool slots32)
		{
			return hud_center_x(view_w, hud_content_w(view_w, slots32));
		}

		// 默认按当前 CLASSIC_QS_SLOTS=26（slots32=false）；HudGroup / ChatBar 无槽位上下文时用。
		inline int16_t hud_center_x(int16_t view_w)
		{
			return hud_center_x(view_w, /*slots32=*/false);
		}

		inline int16_t qs_group_gap(int16_t view_w, bool slots32 = false)
		{
			return is_qs_wide(view_w, slots32) ? QS_GROUP_GAP_WIDE : QS_GROUP_GAP_NARROW;
		}

		// HUD 组内快捷格左缘（相对 hud_center_x）。
		// 宽 QS：qs_x = HUD_DESIGN_W（贴在 800 条右侧；is_qs_wide 已保证装得下）。
		// 窄 QS08：Angel 式 qs_x = hud_grid_w - qs08_w。
		inline int16_t qs_x_in_group(int16_t view_w, bool slots32)
		{
			const int16_t qs_w = qs_panel_w(view_w, slots32);

			if (!is_qs_wide(view_w, slots32))
				return static_cast<int16_t>(hud_grid_w(view_w) - qs_w);

			return HUD_DESIGN_W;
		}

		// 按钮簇绝对 x = hud_center_x + qs 左缘 - group_gap（与 StatusBar / 气球同源）。
		inline int16_t classic_group_x(int16_t view_w, bool slots32)
		{
			return static_cast<int16_t>(
				hud_center_x(view_w, slots32)
				+ qs_x_in_group(view_w, slots32)
				- qs_group_gap(view_w, slots32));
		}

		inline int16_t edge_left(int16_t /*view_w*/, int16_t margin)
		{
			return margin;
		}

		inline int16_t edge_right(int16_t view_w, int16_t dim_w, int16_t margin)
		{
			return static_cast<int16_t>(view_w - dim_w - margin);
		}

		inline int16_t edge_top(int16_t /*view_h*/, int16_t margin)
		{
			return margin;
		}

		inline int16_t edge_bottom(int16_t view_h, int16_t dim_h, int16_t margin)
		{
			return static_cast<int16_t>(view_h - dim_h - margin);
		}

		inline Point<int16_t> center_on_screen(int16_t view_w, int16_t view_h, Point<int16_t> dim)
		{
			return Point<int16_t>(
				static_cast<int16_t>((view_w - dim.x()) / 2),
				static_cast<int16_t>((view_h - dim.y()) / 2));
		}

		// 把窗口矩形夹回视口；尺寸大于视口时贴 (0,0)。
		inline Point<int16_t> clamp_to_view(Point<int16_t> pos, Point<int16_t> dim,
			int16_t view_w, int16_t view_h)
		{
			int16_t x = pos.x();
			int16_t y = pos.y();

			if (dim.x() >= view_w)
				x = 0;
			else
				x = std::clamp(x, static_cast<int16_t>(0),
					static_cast<int16_t>(view_w - dim.x()));

			if (dim.y() >= view_h)
				y = 0;
			else
				y = std::clamp(y, static_cast<int16_t>(0),
					static_cast<int16_t>(view_h - dim.y()));

			return Point<int16_t>(x, y);
		}
	}
}
