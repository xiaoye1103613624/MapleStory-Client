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
#include "UIStatusBar.h"

#include <fstream>

#include "../../Util/HudLayout.h"
#include "../../Util/ResolutionTiers.h"

#include <cstdlib>

#include "UIAdmin.h"
#include "UIChannel.h"
#include "UICharInfo.h"
#include "UIChat.h"
#include "UIMessenger.h"
#include "UIDelivery.h"
#include "UIEquipInventory.h"
#include "UIDailyGift.h"
#include "UIEvent.h"
#include "UIFamily.h"
#include "UIGuild.h"
#include "UIItemInventory.h"
#include "UIJoypad.h"
#include "UIKeyConfig.h"
#include "UIMaker.h"
#include "UIMonsterBook.h"
#include "UIAuction.h"   // 2026-11-25 拍卖行入口（BT_MENU_AUCTION → UIAuction）
#include "UIMonsterCarnival.h"
#include "UIBattleStats.h"
#include "UIOptionMenu.h"
#include "UIMiscShells.h"
#include "UIPlant.h"
#include "UIQuestLog.h"
#include "UIQuit.h"
#include "UISkillBook.h"
#include "UISoftKey.h"
#include "UIStatsInfo.h"
#include "UIUserList.h"
#include "UIWorldMap.h"

#include "../UI.h"

// B4-5 HP/MP 警报阈值取自 ui.sysopt.hp_warn/mp_warn（Setting<SysOptHpWarn>）。
#include "../../Configuration.h"

#include "../Components/MapleButton.h"

#include "../../Character/ExpTable.h"
#include "../../Util/Misc.h"
#include "../../Gameplay/Stage.h"

#include "../../Data/ItemData.h"
#include "../../Data/SkillData.h"
#include "../../Character/SkillMacros.h"
#include "../../Net/Packets/GameplayPackets.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Net/Packets/MiscOutPackets.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Net/Packets/PlayerPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/UiWindowOpen.h"
#include "../../Util/Misc.h"
#include "../KeyConfig.h"
#include "../KeyType.h"
#include "UIChatBar.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <algorithm>

// 经典底条的分段裁框使用 Range<int16_t>（Texture::draw 的水平裁剪参数）。
#include "../../Template/Range.h"

namespace ms
{
	// classic 三联量规图集（gauge/bar、gauge/graduation 均 340x31）。
	// 图集内三段是「同行 1:1 排列」，参考端也按图集原间距绘制（不重新拉开间距）：
	//   HP 边框 x0..107 / MP x109..215 / EXP x222..339（实测见 evidence/w1w3-hud/coords.md，
	//   参考截图 graduation 暗框整体跨 x218..557 = 340px，与 GAUGE_X=218 + 1:1 完全吻合）。
	// 因此 SEG_*（图集内左裁起点）同时就是绘制时的目标偏移，无需第二套 DST_*。
	constexpr int16_t SEG_HP = 0;
	constexpr int16_t SEG_MP = 109;
	constexpr int16_t SEG_EXP = 222;
	constexpr int16_t SEG_W_HP = 108;
	constexpr int16_t SEG_W_MP = 107;
	constexpr int16_t SEG_W_EXP = 118;
	// 经典底栏三联量规整体框（graduation）位置：init_classic 参考截图判定值，
	// draw_classic 的「百分比叠条」也需要同一几何，故上移为文件级常量。
	constexpr int16_t GAUGE_X = 218;
	constexpr int16_t GAUGE_Y = 38;

	// classic 快捷格几何（由 base/quickSlot08|26|32 图集自身反推，非截图量）：
	//   151 = 9 + 4*28 + 3*7 + 9   → quickSlot08 4 列
	//   466 = 9 + 13*28 + 12*7 + 9 → quickSlot26 13 列
	//   571 = 9 + 16*28 + 15*7 + 9 → quickSlot32 16 列
	//   80  = 9 + 2*28 + 1*6 + 9   → 2 行，行距 34
	constexpr int16_t QS_SLOT = 28;
	constexpr int16_t QS_PITCH_X = 35;
	constexpr int16_t QS_PITCH_Y = 34;
	constexpr int16_t QS_FIRST_X = 9;
	constexpr int16_t QS_FIRST_Y = 9;
	// WZ 图标锚点在贴图下方（origin.y = +32），裸 Texture 须同样下移才落到格内。
	constexpr int16_t QS_ICON_SHIFT_Y = 32;
	// 键帽带：key/0「Shift」宽 28 = 格宽，故贴纸以格左上角为锚点。
	// 数字/字母键帽字身（参考截图量得 #AABBCC 像素包围盒）左上角 = 格左上角 + (4, 4)。
	constexpr int16_t QS_CAP_X = 4;
	constexpr int16_t QS_CAP_Y = 4;
	// 中文绑定名落在格下半带（A11M 行高约 11，14 + 11 ≤ 28）。
	constexpr int16_t QS_LABEL_Y = 14;
	// 快捷格拖拽换位阈值：光标离开起点超过该像素数才起拖，否则按单击使用。
	constexpr int16_t QS_DRAG_THRESHOLD = 8;

	// classic 快捷格键位表（13 列 x2 行，参考端 KeyConfig 槽序）。
	constexpr uint8_t CLASSIC_QS_KEYS[26] = {
		KeyConfig::Key::LEFT_SHIFT, KeyConfig::Key::INSERT, KeyConfig::Key::HOME, KeyConfig::Key::PAGE_UP,
		KeyConfig::Key::NUM1, KeyConfig::Key::NUM2, KeyConfig::Key::NUM3, KeyConfig::Key::NUM4, KeyConfig::Key::NUM5,
		KeyConfig::Key::A, KeyConfig::Key::S, KeyConfig::Key::D, KeyConfig::Key::F,
		KeyConfig::Key::LEFT_CONTROL, KeyConfig::Key::DELETE, KeyConfig::Key::END, KeyConfig::Key::PAGE_DOWN,
		KeyConfig::Key::Q, KeyConfig::Key::W, KeyConfig::Key::E, KeyConfig::Key::R, KeyConfig::Key::T,
		KeyConfig::Key::Z, KeyConfig::Key::X, KeyConfig::Key::C, KeyConfig::Key::V
	};

	// 2026-11-20 IDA 实证（MapleStoryT.exe s_ptShortKeyPos@0x279c390，28 项运行时常量）：
	//   列 x = 7, 40, 73, 106 | 150, 183, 216, 249 | 282, 315, 348, 381 | 414, 447
	//   行 y = 15, 48
	//   → 列距 33、行距 33、**每 4 列一组**、组间 gap 11（106→150）。
	//   索引顺序 = 逐组「上行4格 → 下行4格」，共 4+4+4+2 = 14 列 × 2 行 = 28 格。
	//   这同时解释了用户「按键映射参照 083 的 13 格、多余按规律往后延续」：
	//   083 是 13 列×2=26 格，192 在其后续到 28 格，坐标规律完全相同（pitch33）。
	// 旧表 pitch35/无组间隔是错的（图标偏移的根因之一），此处按 IDA 实表生成。
	constexpr int16_t QS_COL_X[14] = {
		7, 40, 73, 106,
		150, 183, 216, 249,
		282, 315, 348, 381,
		414, 447
	};
	constexpr int16_t QS_ROW_Y[2] = { 15, 48 };
	constexpr int16_t QS_GRID_SLOTS = 28; // 14 列 × 2 行（IDA m_nQuickSlotDisplayNo 上限）

	// 按 IDA 索引顺序生成格子相对坐标：每组 4 列走完上下两行再进下一组。
	constexpr auto make_qs_pos()
	{
		std::array<Point<int16_t>, QS_GRID_SLOTS> pos{};
		size_t idx = 0;
		for (int group = 0; group < 4; ++group)
		{
			for (int row = 0; row < 2; ++row)
			{
				for (int c = 0; c < 4; ++c)
				{
					const int col = group * 4 + c;
					if (col >= 14 || idx >= QS_GRID_SLOTS)
						continue;
					pos[idx++] = Point<int16_t>(QS_COL_X[col], QS_ROW_Y[row]);
				}
			}
		}
		return pos;
	}
	constexpr auto MODERN_QS_POS = make_qs_pos();

	// 键序 = 083 CLASSIC_QS_KEYS[26] 全 26 个 + 按同规律续 2 个（Z/X 之后接 C/V 已用尽，
	// 续 Q/W 的下一组：NUM6/NUM7），保证 28 格都有确定键位。
	constexpr uint8_t MODERN_QS_KEYS[QS_GRID_SLOTS] = {
		KeyConfig::Key::LEFT_SHIFT, KeyConfig::Key::INSERT, KeyConfig::Key::HOME, KeyConfig::Key::PAGE_UP,
		KeyConfig::Key::NUM1, KeyConfig::Key::NUM2, KeyConfig::Key::NUM3, KeyConfig::Key::NUM4,
		KeyConfig::Key::NUM5, KeyConfig::Key::A, KeyConfig::Key::S, KeyConfig::Key::D,
		KeyConfig::Key::F, KeyConfig::Key::LEFT_CONTROL, KeyConfig::Key::DELETE, KeyConfig::Key::END,
		KeyConfig::Key::PAGE_DOWN, KeyConfig::Key::Q, KeyConfig::Key::W, KeyConfig::Key::E,
		KeyConfig::Key::R, KeyConfig::Key::T, KeyConfig::Key::Z, KeyConfig::Key::X,
		KeyConfig::Key::C, KeyConfig::Key::V,
		// 28 格比 083 的 26 格多 2 个，按同规律往后延续（数字键序 6、7）。
		KeyConfig::Key::NUM6, KeyConfig::Key::NUM7
	};
	// key/0..6 贴纸索引（Shift/Ins/Hm/Pup/Ctrl/Del/End）；-1 = 无贴纸，走 KEYNAME。
	constexpr int8_t CLASSIC_QS_KEYCAP[26] = {
		0, 1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		4, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	};
	// NUM1-5 用 FontMemo 位图数字；字母与 Pdn（本包无贴纸，key/7 是 28x20 红箭头）用描边文字回退。
	constexpr const char* CLASSIC_QS_KEYNAME[26] = {
		"", "", "", "", "1", "2", "3", "4", "5", "A", "S", "D", "F",
		"", "", "", "Pdn", "Q", "W", "E", "R", "T", "Z", "X", "C", "V"
	};

	// StatusBar.img/number 字库：0-9 与 slash/percent 高 7，Lbracket/Rbracket 高 9，
	// 全部 origin = (0,0)（nx 实测），且无 '.' 字形。
	// 推进模型由参考截图逐列还原（HP「[50/50]」/ MP「[50/50]」/ EXP「[0.00%]」全列吻合）：
	//   '[' ']' → getw + 2      其余字形 → getw + 1      '.'（字库缺，用 1x1 白点补）→ 4
	// 垂直：以 9px 括号行为基准，7px 字形下移 1px 居中；小数点下移 7px 贴基线。
	int16_t hud_advance(const Charset& set, char c)
	{
		if (c == '.')
			return 4;

		int16_t width = set.getw(c);

		return (c == '[' || c == ']') ? width + 2 : width + 1;
	}

	void draw_hud_number(const Charset& set, const ColorBox& dot, const std::string& text,
		const DrawArgument& args)
	{
		int16_t shift = 0;

		for (char c : text)
		{
			if (c == '.')
			{
				dot.draw(args + Point<int16_t>(static_cast<int16_t>(shift + 1), 7));
			}
			else
			{
				const bool bracket = (c == '[' || c == ']');

				set.draw(c, args + Point<int16_t>(shift, bracket ? 0 : 1));
			}

			shift += hud_advance(set, c);
		}
	}

	UIStatusBar::UIStatusBar(const CharStats& st) : stats(st)
	{
		quickslot_active = false;
		quickslot_adj = Point<int16_t>(QUICKSLOT_MAX, 0);
		VWIDTH = Constants::Constants::get().get_viewwidth();
		VHEIGHT = Constants::Constants::get().get_viewheight();

		menu_active = false;
		setting_active = false;
		community_active = false;
		character_active = false;
		event_active = false;
		classic_mode = UiAssetResolve::is_classic_hud();

		if (classic_mode)
		{
			LOG(LOG_DEBUG, "UIStatusBar: using StatusBar.img classic HUD");
			init_classic();
			return;
		}

		init_modern();
	}

	void UIStatusBar::update_screen(int16_t new_width, int16_t new_height)
	{
		if (!classic_mode)
			return;

		// QS 宽/窄翻转：禁止在本函数半切换贴图（会一帧双绘/钮空洞）。
		// 交由 UIStateGame 卸 STATUSBAR 再 emplace（先卸命中再换纹理），见
		// UIStateGame.cpp qs_flip 分支；此处仅同档重锚。
		if (HudLayout::is_qs_wide(new_width, CLASSIC_QS_SLOTS == 32) != classic_qs_wide)
			return;

		VWIDTH = new_width;
		VHEIGHT = new_height;

		const bool slots32 = (CLASSIC_QS_SLOTS == 32);
		const int16_t content_w = HudLayout::hud_content_w(VWIDTH, slots32);
		const int16_t hud_cx = HudLayout::hud_center_x(VWIDTH, content_w);
		// 2026-11-21：多分辨率重锚同步 backgrnd 内容宽/中心。
		classic_content_w = content_w;
		classic_content_cx = hud_cx;

		quickslot_pos = Point<int16_t>(
			HudLayout::qs_x_in_group(VWIDTH, slots32),
			-8);
		const int16_t group_x = static_cast<int16_t>(
			quickslot_pos.x() - HudLayout::qs_group_gap(VWIDTH));

		auto move_btn = [this](Buttons id, int16_t x, int16_t y)
		{
			if (Button* b = button_at(id))
				b->set_position(Point<int16_t>(x, y));
		};

		move_btn(Buttons::BT_CLASSIC_SHOP, group_x + 0, 36);
		move_btn(Buttons::BT_CLASSIC_NPT, group_x + 56, 36);
		move_btn(Buttons::BT_CLASSIC_MENU, group_x + 112, 36);
		move_btn(Buttons::BT_CLASSIC_SHORT, group_x + 168, 36);
		move_btn(Buttons::BT_CLASSIC_EQUIP, group_x + 45, 8);
		move_btn(Buttons::BT_CLASSIC_INV, group_x + 75, 8);
		move_btn(Buttons::BT_CLASSIC_STAT, group_x + 105, 8);
		move_btn(Buttons::BT_CLASSIC_SKILL, group_x + 135, 8);
		move_btn(Buttons::BT_CLASSIC_KEYSET, group_x + 165, 8);
		move_btn(Buttons::BT_CLASSIC_QS, group_x + 195, 8);
		move_btn(Buttons::BT_CLASSIC_QSD, group_x + 195, 8);
		move_btn(Buttons::BT_CLASSIC_CLAIM, group_x + 0, 8);

		position = Point<int16_t>(hud_cx, static_cast<int16_t>(VHEIGHT - CLASSIC_BAR_H));
		position_x = position.x();
		position_y = static_cast<int16_t>(position.y() - 9);
		dimension = Point<int16_t>(VWIDTH, 80);

		LOG(LOG_DEBUG, "[MR] StatusBar classic reanchor cx=" << hud_cx
			<< " content_w=" << content_w << " qs_x=" << quickslot_pos.x()
			<< " group_x=" << group_x << " vw=" << VWIDTH << " vh=" << VHEIGHT);
	}

	void UIStatusBar::init_modern()
	{
		// ThemePack / multi-res: snap layout constants to nearest StatusBar3 enum width
		// so non-enumerated view sizes (1440/1600/…) keep chrome aligned with the
		// EXPBar asset we actually load (instead of pos_adj=0 + 1280 textures).
		static constexpr int16_t kModernW[] = { 800, 1024, 1280, 1366, 1920 };
		auto nearest_modern_w = [](int16_t vw) -> int16_t
		{
			int16_t best = kModernW[0];
			int32_t best_d = std::abs(static_cast<int32_t>(vw) - kModernW[0]);

			for (int16_t cand : kModernW)
			{
				const int32_t d = std::abs(static_cast<int32_t>(vw) - cand);

				if (d < best_d)
				{
					best_d = d;
					best = cand;
				}
			}

			return best;
		};

		const int16_t layout_w = nearest_modern_w(VWIDTH);

		std::string stat = "status";

		if (layout_w == 800)
			stat += "800";

		nl::node mainBar = UiAssetResolve::status_bar_root()["mainBar"];
		nl::node status = mainBar[stat];
		nl::node EXPBar = mainBar["EXPBar"];
		nl::node EXPBarRes = EXPBar[layout_w];

		// 多分辨率兜底：modern HUD 的 EXPBar 只有五套分件。
		if (!UiAssetResolve::usable(EXPBarRes))
			EXPBarRes = EXPBar[1280];
		nl::node menu = mainBar["menu"];
		nl::node quickSlot = mainBar["quickSlot"];
		nl::node submenu = mainBar["submenu"];

		// ============ 2026-11-20 多分辨自适应底部布局（用户裁决版） ============
		// 用户要求：
		//  ① 血量条**中轴居中**于视口（不是左缘居中）：status_x = (VWIDTH - 198)/2
		//  ② 主功能菜单移到**等级行上方**（血条框之上，水平居中）
		//  ③ 快捷键贴右、**按整格截断**（不允许半格在视口内）；宽度够全部显示
		//  ④ 底部整体（聊天/信息/菜单/快捷键）自适应；宽度过大时整体居中
		// IDA 复用：s_ptShortKeyPos 28 格坐标（pitch33、4列一组 gap11）、格数 8/28 规则。
		constexpr int16_t MENU_PITCH = 35;       // 菜单六钮步距（素材 origin 级差）
		constexpr int16_t MENU_W_TOTAL = 6 * 35; // 六钮总宽
		// 2026-11-23 用户反馈：底部两排功能菜单离经验条太近，整体右移一点。
		// 只平移主功能菜单条（下排），血条/经验条/快捷栏锚点不变。
		constexpr int16_t MENU_X_SHIFT = 48;
		constexpr int16_t STATUS_W = 198;        // status/backgrnd 素材宽
		constexpr int16_t QS_PITCH_X = 33;       // IDA 列距（s_ptShortKeyPos）
		constexpr int16_t QS_CELL = 32;          // 格边长
		constexpr int16_t QS_PAD_R = 4;          // 底板右留白

		// 经验条顶（相对底栏 position）。
		constexpr int16_t EXP_BOTTOM = 87;
		// 素材尺寸固定（不随视口缩放）：status/cover 204×70、menu 钮 37、quickSlot/backgrnd 557×67。
		constexpr int16_t STATUS_H = 70;        // mainBar/status/layer:cover
		constexpr int16_t MENU_H = 37;          // mainBar/menu/button:* normal/0
		constexpr int16_t QS_H = 67;            // mainBar/quickSlot/backgrnd

		// ① 血条中轴居中：让 198 宽素材的**中心**落在视口中心。
		const int16_t status_x = static_cast<int16_t>((VWIDTH - STATUS_W) / 2);

		// ③ 快捷键：贴右缘，按整列计算可见列数（不出现半格）。
		//    QS 底板结构：首列 x=7、列距 33、末列右缘 = 7+(cols-1)*33+32。
		//    先按 IDA 上限（小分辨率 8 / 其余 28）取需求格数，再按实际可用宽向下取整到整列。
		auto qs_span_w = [QS_PITCH_X, QS_CELL, QS_PAD_R](int16_t cols) -> int16_t
		{
			// 可见宽度 = 首列偏移 7 + (cols-1)*33 + 格宽 32 + 右留白 4
			return static_cast<int16_t>(7 + (cols - 1) * QS_PITCH_X + QS_CELL + QS_PAD_R);
		};
		// 上限列数：28 格 = 14 列；小分辨率 8 格 = 4 列（IDA m_nQuickSlotDisplayNo）。
		const bool qs_small = (VWIDTH <= 1024 && VHEIGHT <= 768);
		int16_t qs_max_cols = qs_small ? 4 : 14;
		// 让 QS 起点不压到血条（血条右缘 = status_x + 198），可用宽 = VWIDTH - (status_x+198) - gap。
		const int16_t qs_avail_min_x = static_cast<int16_t>(status_x + STATUS_W + 8);
		int16_t qs_cols = qs_max_cols;
		while (qs_cols > 1 && qs_avail_min_x + qs_span_w(qs_cols) > VWIDTH)
			--qs_cols;
		const int16_t qs_visible_w = qs_span_w(qs_cols);
		const int16_t qs_x = static_cast<int16_t>(VWIDTH - qs_visible_w - 2);


		// ② 主功能菜单移到等级行上方、水平居中（血条框顶 = EXP_BOTTOM - STATUS_H）。
		const int16_t menu_x = static_cast<int16_t>((VWIDTH - MENU_W_TOTAL) / 2 + MENU_X_SHIFT);
		const int16_t menu_y = static_cast<int16_t>(EXP_BOTTOM - STATUS_H - MENU_H - 6);

		// 经验条：**关键** EXPBar/<w>/layer:back 的 origin=(0,-23)（orange-wz 实测），
		// draw(pos) 视觉左上 = pos - origin，故 pos.y 必须 = 期望视觉顶 + origin.y。
		// 此前直接用 EXP_BOTTOM 当 pos.y → 视觉顶被推到 EXP_BOTTOM+23，1280x720 下
		// 落到视口最底边之外，用户报「底部经验条不见了」的真因。
		constexpr int16_t EXP_ORIGIN_Y = -23; // EXPBar/1920/layer:back origin.y
		exp_pos = Point<int16_t>(0, static_cast<int16_t>(EXP_BOTTOM + EXP_ORIGIN_Y + 14));

		sprites.emplace_back(EXPBar["backgrnd"], DrawArgument(Point<int16_t>(0, EXP_BOTTOM), Point<int16_t>(VWIDTH, 0)));
		sprites.emplace_back(EXPBarRes["layer:back"], exp_pos);

		int16_t exp_max = VWIDTH - 16;

		expbar = Gauge(
			Gauge::Type::DEFAULT,
			EXPBarRes.resolve("layer:gauge"),
			EXPBarRes.resolve("layer:cover"),
			EXPBar.resolve("layer:effect"),
			exp_max, 0.0f
		);

		int16_t pos_adj = 0;

		if (layout_w == 1280)
			pos_adj = 87;
		else if (layout_w == 1366)
			pos_adj = 171;
		else if (layout_w == 1920)
			pos_adj = 448;

		if (layout_w == 1024)
			quickslot_min = 1;
		else
			quickslot_min = 0;

		{
			// 全档统一「从右往左链式」坐标：QS 贴右缘 → 菜单在其左 → 血条再左。
			// 任意视口宽（800/1024/1280/1366/1920）都不重叠，不再维护分档硬编码。
			hpmp_pos = Point<int16_t>(status_x, EXP_BOTTOM - STATUS_H);
			// 相对偏移按 status/layer:cover(204x70) 内部布局（素材原坐标，不缩放）：
			// 「等级」二字是 backgrnd 上烤好的（约 x2..44），数字紧跟其后 → +65；
			// HP 条 y≈26..40、MP 条 y≈42..56，数值居中于条内。
			levelset_pos = Point<int16_t>(static_cast<int16_t>(hpmp_pos.x() + 65), static_cast<int16_t>(hpmp_pos.y() + 6));
			hpset_pos = Point<int16_t>(static_cast<int16_t>(hpmp_pos.x() + 96), static_cast<int16_t>(hpmp_pos.y() + 30));
			mpset_pos = Point<int16_t>(static_cast<int16_t>(hpmp_pos.x() + 90), static_cast<int16_t>(hpmp_pos.y() + 46));
			// 经验条文字「<exp>[<pct>%]」：与 layer:back 绿条同步（绿条已下移 14px 贴底），
			// 文字在绿条内垂直居中（绿条 10px 高、字模 9px → +1）。
			statset_pos = Point<int16_t>(static_cast<int16_t>(VWIDTH * 21 / 100 + 120), static_cast<int16_t>(EXP_BOTTOM + 15));
			namelabel_pos = Point<int16_t>(static_cast<int16_t>(hpmp_pos.x() + 112), hpmp_pos.y());
			quickslot_pos = Point<int16_t>(qs_x, EXP_BOTTOM - QS_H);

			// Menu：六钮横排，子菜单窗口在其上方。
			menu_pos = Point<int16_t>(menu_x, EXP_BOTTOM - MENU_H);
			setting_pos = menu_pos + Point<int16_t>(0, 168);
			community_pos = menu_pos + Point<int16_t>(-26, 196);
			character_pos = menu_pos + Point<int16_t>(-61, 168);
			event_pos = menu_pos + Point<int16_t>(-94, 252);
		}


		hpmp_sprites.emplace_back(status["backgrnd"], hpmp_pos - Point<int16_t>(1, 0));
		hpmp_sprites.emplace_back(status["layer:cover"], hpmp_pos - Point<int16_t>(1, 0));

		if (layout_w == 800)
			hpmp_sprites.emplace_back(status["layer:Lv"], hpmp_pos);
		else
			hpmp_sprites.emplace_back(status["layer:Lv"], hpmp_pos - Point<int16_t>(1, 0));

		int16_t hpmp_max = 139;

		if (layout_w > 800)
			hpmp_max += 30;

		hpbar = Gauge(Gauge::Type::DEFAULT, status.resolve("gauge/hp/layer:0"), hpmp_max, 0.0f);
		mpbar = Gauge(Gauge::Type::DEFAULT, status.resolve("gauge/mp/layer:0"), hpmp_max, 0.0f);

		statset = Charset(EXPBar["number"], Charset::Alignment::RIGHT);
		hpmpset = Charset(status["gauge"]["number"], Charset::Alignment::RIGHT);
		levelset = Charset(status["lvNumber"], Charset::Alignment::LEFT);

		// 名字在血条背景(198宽)内水平居中 → CENTER 对齐，x=背景中心。
		namelabel = OutlinedText(Text::Font::A13M, Text::Alignment::CENTER, Color::Name::GALLERY, Color::Name::TUNA);

		quickslot[0] = quickSlot["backgrnd"];
		quickslot[1] = quickSlot["layer:cover"];
		// 192 CLayoutMan::AddSingleLayerRemoveOrigin：底板/遮罩去掉 canvas origin。
		// StatusBar3 backgrnd origin=(-12,-3)、cover=(-10,0)；不清零则
		// s_ptShortKeyPos 相对控件 TL 的槽相对底板偏左上（空/偏移）。
		if (quickslot[0])
			quickslot[0].shift(quickslot[0].get_origin());
		if (quickslot[1])
			quickslot[1].shift(quickslot[1].get_origin());

		// menu 六钮：WZ 用共享 CreateCtrl + origin.x 横排，但 Texture::sanitize 会清掉
		// |ox|>132（Menu=-174 / Setting=-139）→ 叠成「缺失」。这里按语义序显式 TL。
		// 2026-11-20 用户裁决：主功能菜单移到**等级行上方**（menu_y），水平居中（menu_x）。
		constexpr int16_t MENU_COUNT = 6;
		constexpr int16_t MENU_BTN_H = 37; // orange-wz: menu/normal/0 高 36~37
		constexpr int16_t STATUS_BAR_W = 198; // orange-wz: mainBar/status/backgrnd 198×37
		constexpr int16_t STATUS_BAR_H = 37;
		Point<int16_t> menu_strip = Point<int16_t>(menu_x, menu_y);

		auto place_menu = [&](Buttons id, nl::node src, int16_t idx, const char* tag) {
			const Point<int16_t> tl = menu_strip + Point<int16_t>(static_cast<int16_t>(idx * MENU_PITCH), 0);
			if (!try_maple_btn(buttons, id, src, tl, tag))
			{
				UiAssetResolve::diag_once(tag, "mainBar/menu button missing");
				return;
			}
			if (Button* b = button_at(id))
				b->set_active(true);
		};
		place_menu(Buttons::BT_CASHSHOP, menu["button:CashShop"], 0, "statusbar.modern.menu.CashShop");
		place_menu(Buttons::BT_MENU, menu["button:Menu"], 1, "statusbar.modern.menu.Menu");
		place_menu(Buttons::BT_OPTIONS, menu["button:Setting"], 2, "statusbar.modern.menu.Setting");
		place_menu(Buttons::BT_CHARACTER, menu["button:Character"], 3, "statusbar.modern.menu.Character");
		place_menu(Buttons::BT_COMMUNITY, menu["button:Community"], 4, "statusbar.modern.menu.Community");
		place_menu(Buttons::BT_EVENT, menu["button:Event"], 5, "statusbar.modern.menu.Event");

		std::string fold = "button:Fold";
		std::string extend = "button:Extend";

		if (layout_w == 800)
		{
			fold += "800";
			extend += "800";
		}

		if (layout_w == 1366)
			quickslot_qs_adj = Point<int16_t>(213, 0);
		else
			quickslot_qs_adj = Point<int16_t>(211, 0);

		// 展开/收起（切换快捷键尺寸）钮：图4 实测为 quickSlot/button:Fold|Extend 竖条
		// （12/13×71，origin(-3,-1)/(-2,-1)），贴在**底板左缘外侧**、底边与底板齐平。
		// 旧实现放在 quickslot_pos-(2,0) 且绘制在底板之前 → 被底板整块盖住，
		// 用户报「左侧切换尺寸的 UI 没实现」的真因（按钮存在但看不见）。
		{
			constexpr int16_t BAR_W = 13;  // button:Extend 宽 13（Fold 12）
			constexpr int16_t BAR_H = 71;  // origin(-3,-1) → 视觉高 71
			const Point<int16_t> quickslot_qs(
				static_cast<int16_t>(quickslot_pos.x() - BAR_W),
				static_cast<int16_t>(EXP_BOTTOM - BAR_H));
			buttons[Buttons::BT_FOLD_QS] = std::make_unique<MapleButton>(quickSlot[fold], quickslot_qs);
			buttons[Buttons::BT_EXTEND_QS] = std::make_unique<MapleButton>(quickSlot[extend], quickslot_qs);
		}

		if (buttons[Buttons::BT_FOLD_QS] && buttons[Buttons::BT_EXTEND_QS])
		{
			if (quickslot_active)
				buttons[Buttons::BT_EXTEND_QS]->set_active(false);
			else
				buttons[Buttons::BT_FOLD_QS]->set_active(false);
		}

#pragma region Menu
		menubackground[0] = submenu["backgrnd"]["0"];
		menubackground[1] = submenu["backgrnd"]["1"];
		menubackground[2] = submenu["backgrnd"]["2"];

		buttons[Buttons::BT_MENU_ACHIEVEMENT] = std::make_unique<MapleButton>(submenu["menu"]["button:achievement"], menu_pos);
		buttons[Buttons::BT_MENU_AUCTION] = std::make_unique<MapleButton>(submenu["menu"]["button:auction"], menu_pos);
		buttons[Buttons::BT_MENU_BATTLE] = std::make_unique<MapleButton>(submenu["menu"]["button:battleStats"], menu_pos);
		buttons[Buttons::BT_MENU_CLAIM] = std::make_unique<MapleButton>(submenu["menu"]["button:Claim"], menu_pos);
		buttons[Buttons::BT_MENU_FISHING] = std::make_unique<MapleButton>(submenu["menu"]["button:Fishing"], menu_pos + Point<int16_t>(3, 1));
		buttons[Buttons::BT_MENU_HELP] = std::make_unique<MapleButton>(submenu["menu"]["button:Help"], menu_pos);
		buttons[Buttons::BT_MENU_MEDAL] = std::make_unique<MapleButton>(submenu["menu"]["button:medal"], menu_pos);
		buttons[Buttons::BT_MENU_MONSTER_COLLECTION] = std::make_unique<MapleButton>(submenu["menu"]["button:monsterCollection"], menu_pos);
		buttons[Buttons::BT_MENU_MONSTER_LIFE] = std::make_unique<MapleButton>(submenu["menu"]["button:monsterLife"], menu_pos);
		buttons[Buttons::BT_MENU_QUEST] = std::make_unique<MapleButton>(submenu["menu"]["button:quest"], menu_pos);
		buttons[Buttons::BT_MENU_UNION] = std::make_unique<MapleButton>(submenu["menu"]["button:union"], menu_pos);

		buttons[Buttons::BT_SETTING_CHANNEL] = std::make_unique<MapleButton>(submenu["setting"]["button:channel"], setting_pos);
		buttons[Buttons::BT_SETTING_QUIT] = std::make_unique<MapleButton>(submenu["setting"]["button:GameQuit"], setting_pos);
		buttons[Buttons::BT_SETTING_JOYPAD] = std::make_unique<MapleButton>(submenu["setting"]["button:JoyPad"], setting_pos);
		buttons[Buttons::BT_SETTING_KEYS] = std::make_unique<MapleButton>(submenu["setting"]["button:keySetting"], setting_pos);
		buttons[Buttons::BT_SETTING_OPTION] = std::make_unique<MapleButton>(submenu["setting"]["button:option"], setting_pos);

		buttons[Buttons::BT_COMMUNITY_PARTY] = std::make_unique<MapleButton>(submenu["community"]["button:bossParty"], community_pos);
		buttons[Buttons::BT_COMMUNITY_FRIENDS] = std::make_unique<MapleButton>(submenu["community"]["button:friends"], community_pos);
		buttons[Buttons::BT_COMMUNITY_GUILD] = std::make_unique<MapleButton>(submenu["community"]["button:guild"], community_pos);
		buttons[Buttons::BT_COMMUNITY_MAPLECHAT] = std::make_unique<MapleButton>(submenu["community"]["button:mapleChat"], community_pos);

		buttons[Buttons::BT_CHARACTER_INFO] = std::make_unique<MapleButton>(submenu["character"]["button:character"], character_pos);
		buttons[Buttons::BT_CHARACTER_EQUIP] = std::make_unique<MapleButton>(submenu["character"]["button:Equip"], character_pos);
		buttons[Buttons::BT_CHARACTER_ITEM] = std::make_unique<MapleButton>(submenu["character"]["button:Item"], character_pos);
		buttons[Buttons::BT_CHARACTER_SKILL] = std::make_unique<MapleButton>(submenu["character"]["button:Skill"], character_pos);
		buttons[Buttons::BT_CHARACTER_STAT] = std::make_unique<MapleButton>(submenu["character"]["button:Stat"], character_pos);

		buttons[Buttons::BT_EVENT_DAILY] = std::make_unique<MapleButton>(submenu["event"]["button:dailyGift"], event_pos);
		buttons[Buttons::BT_EVENT_SCHEDULE] = std::make_unique<MapleButton>(submenu["event"]["button:schedule"], event_pos);

		for (size_t i = Buttons::BT_MENU_QUEST; i <= Buttons::BT_EVENT_DAILY; i++)
			buttons[i]->set_active(false);

		menutitle[0] = submenu["title"]["character"];
		menutitle[1] = submenu["title"]["community"];
		menutitle[2] = submenu["title"]["event"];
		menutitle[3] = submenu["title"]["menu"];
		menutitle[4] = submenu["title"]["setting"];
#pragma endregion

		// Y/chrome use layout_w (nearest enum); width still follows real VWIDTH.
		if (layout_w == 800)
		{
			// 2026-11-19 图2校准：整组底栏贴视口底（不是悬空）。经验条高24、
			// exp_pos.y=87 → position.y = VHEIGHT - 87 - 24 = VHEIGHT-111，
			// 经验条底正好落在 VHEIGHT。血条/菜单/QS 相对 EXP_BOTTOM=87 底对齐不变。
			position = Point<int16_t>(0, static_cast<int16_t>(VHEIGHT - 111));
			position_x = 410;
			position_y = position.y();
			dimension = Point<int16_t>(VWIDTH - position_x, 140);
		}
		else if (layout_w == 1024)
		{
			position = Point<int16_t>(0, static_cast<int16_t>(VHEIGHT - 111)); // 底栏贴视口底
			position_x = 410;
			position_y = position.y() + 42;
			dimension = Point<int16_t>(VWIDTH - position_x, 75);
		}
		else if (layout_w == 1280)
		{
			position = Point<int16_t>(0, static_cast<int16_t>(VHEIGHT - 111)); // 底栏贴视口底
			position_x = 500;
			position_y = position.y() + 42;
			dimension = Point<int16_t>(VWIDTH - position_x, 75);
		}
		else if (layout_w == 1366)
		{
			position = Point<int16_t>(0, static_cast<int16_t>(VHEIGHT - 111)); // 底栏贴视口底
			position_x = 585;
			position_y = position.y() + 42;
			dimension = Point<int16_t>(VWIDTH - position_x, 75);
		}
		else // 1920
		{
			position = Point<int16_t>(0, static_cast<int16_t>(VHEIGHT - 111)); // 底栏贴视口底
			position_x = 860;
			position_y = position.y() + 40;
			dimension = Point<int16_t>(VWIDTH - position_x, 80);
		}

		if (layout_w != VWIDTH)
			LOG(LOG_INFO, "[MR] StatusBar modern layout_w=" << layout_w
				<< " for VWIDTH=" << VWIDTH);

		// 2026-11-20 格数与布局同源（避免两处算法不一致导致画到视口外）：
		//   qs_cols 已在布局段按「IDA 上限 8/28 → 向下取整到整列 → 不压血条」算出，
		//   这里直接换算格数 = 列数 × 2（上下两行），保证最后一列完整、不出现半格。
		//   IDA 依据：CQuickSlot::ChangeScreenResolution@0x169b710 的
		//   m_nQuickSlotDisplayNo（800×600/1024×768→8，其余→28）。
		{
			modern_qs_count = std::min(
				static_cast<size_t>(MODERN_QS_SLOTS),
				static_cast<size_t>(qs_cols) * 2);
		}

		// 键帽：优先 StatusBar3.img/KeyConfig/key（本包已入库），再回退 UIWindow。
#ifdef USE_NX
		nl::node qskey = UiAssetResolve::status_bar_root()["KeyConfig"]["key"];
		if (!qskey)
			qskey = nl::nx::UI["UIWindow.img"]["KeyConfig"]["quickslotConfig"]["key"];
		if (!qskey)
			qskey = nl::nx::UI["UIWindow2.img"]["KeyConfig"]["quickslotConfig"]["key"];
		if (qskey)
		{
			for (int id = 1; id <= 88; ++id)
			{
				nl::node n = qskey[std::to_string(id)];
				if (n)
					classic_qs_cap[id] = Texture(n);
			}
		}
		{
			nl::node itemno = nl::nx::UI["Basic.img"]["ItemNo"];
			if (itemno)
			{
				classic_qs_itemno = Charset(itemno, Charset::Alignment::LEFT);
				classic_qs_itemno_ok = true;
			}
		}
#endif
		classic_qs_keytext = OutlinedText(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::DARKGREY);
		quickslot_active = true;
		quickslot_adj = Point<int16_t>(0, 0);
		if (buttons[Buttons::BT_FOLD_QS] && buttons[Buttons::BT_EXTEND_QS])
		{
			buttons[Buttons::BT_EXTEND_QS]->set_active(false);
			buttons[Buttons::BT_FOLD_QS]->set_active(true);
		}
		refresh_modern_qs_icons();
		LOG(LOG_INFO, "UIStatusBar modern quickSlot slots=" << modern_qs_count
			<< " caps=" << (classic_qs_cap[2] ? 1 : 0));
	}

	void UIStatusBar::draw(float alpha) const
	{
		if (classic_mode)
		{
			draw_classic(alpha);
			return;
		}

		UIElement::draw_sprites(alpha);

		hpmp_sprites[0].draw(position, alpha);

		expbar.draw(position + exp_pos);
		hpbar.draw(position + hpmp_pos);
		mpbar.draw(position + hpmp_pos);

		hpmp_sprites[1].draw(position, alpha);
		hpmp_sprites[2].draw(position, alpha);

		int32_t level = stats.get_stat(MapleStat::Id::LEVEL);
		int32_t hp = stats.get_stat(MapleStat::Id::HP);
		int32_t mp = stats.get_stat(MapleStat::Id::MP);
		int32_t maxhp = stats.get_total(EquipStat::Id::HP);
		int32_t maxmp = stats.get_total(EquipStat::Id::MP);
		int64_t exp = stats.get_exp();

		std::string expstring = std::to_string(100 * getexppercent());

		statset.draw(
			std::to_string(exp) + "[" + expstring.substr(0, expstring.find('.') + 3) + "%]",
			position + statset_pos
		);

		hpmpset.draw(
			"[" + std::to_string(hp) + "/" + std::to_string(maxhp) + "]",
			position + hpset_pos
		);

		hpmpset.draw(
			"[" + std::to_string(mp) + "/" + std::to_string(maxmp) + "]",
			position + mpset_pos
		);

		levelset.draw(
			std::to_string(level),
			position + levelset_pos
		);

		// 2026-11-19 图1校准：角色名与「等级 N」同一行（图1「等级200 yyy天堂2」）。
		// Text::draw(pos) 的 pos.y 是**行顶**（含 ink_offset 上空白），必须经
		// y_for_ink_top 把「等级行墨迹顶」换算成行顶，否则名字下沉压血条。
		{
			const int16_t ink_top = levelset_pos.y(); // 与等级同一墨迹顶
			namelabel.draw(position + Point<int16_t>(
				namelabel_pos.x(), namelabel.y_for_ink_top(ink_top)));
		}

		{
			const Point<int16_t> qs_draw = position + quickslot_pos
				+ ((VWIDTH > 800 && VWIDTH < 1366) ? Point<int16_t>(-1, 0) : Point<int16_t>(0, 0))
				+ quickslot_adj;
			// 底板裁剪精确到「最后一格右缘」：由**实际格数**反算，保证右侧不出现半格/残边
			// （用户要求：截断必须落在格边界）。cols = modern_qs_count / 2（上下两行）。
			const int16_t cols = static_cast<int16_t>((modern_qs_count + 1) / 2);
			const int16_t avail_w = static_cast<int16_t>(7 + (cols - 1) * 33 + 32 + 4);
			const int16_t bw = quickslot[0].width();
			const int16_t crop_r = static_cast<int16_t>(std::max(0, bw - avail_w));
			quickslot[0].draw(qs_draw, Range<int16_t>(0, 0), Range<int16_t>(0, crop_r));
			const int16_t cw = quickslot[1].width();
			const int16_t crop_r2 = static_cast<int16_t>(std::max(0, cw - avail_w));
			quickslot[1].draw(qs_draw, Range<int16_t>(0, 0), Range<int16_t>(0, crop_r2));
		}

		draw_modern_quickslot(alpha);

		// 切换快捷键尺寸钮（Fold/Extend 竖条）必须在底板之后画，否则被背板整块盖住
		// —— 这是「左侧切换尺寸 UI 看不见」的修复点。位置在底板左缘外侧，不与底板重叠。
		if (const auto fold_it = buttons.find(Buttons::BT_FOLD_QS);
			fold_it != buttons.end() && fold_it->second)
			fold_it->second->draw(position + quickslot_adj);

		if (const auto extend_it = buttons.find(Buttons::BT_EXTEND_QS);
			extend_it != buttons.end() && extend_it->second)
			extend_it->second->draw(position + quickslot_adj);

		// 六钮后画，避免被 quickSlot backgrnd/cover 盖住（800 档尤甚）。
		for (uint16_t bid : { Buttons::BT_CASHSHOP, Buttons::BT_MENU, Buttons::BT_OPTIONS,
			Buttons::BT_CHARACTER, Buttons::BT_COMMUNITY, Buttons::BT_EVENT })
		{
			if (const auto it = buttons.find(bid); it != buttons.end() && it->second)
				it->second->draw(position);
		}

#pragma region Menu
		Point<int16_t> pos_adj = Point<int16_t>(0, 0);

		if (quickslot_active)
		{
			if (VWIDTH == 800)
				pos_adj += Point<int16_t>(0, -73);
			else
				pos_adj += Point<int16_t>(0, -31);
		}

		Point<int16_t> pos;
		uint8_t button_count, menutitle_index;

		if (character_active)
		{
			pos = character_pos;
			button_count = 5;
			menutitle_index = 0;
		}
		else if (community_active)
		{
			pos = community_pos;
			button_count = 4;
			menutitle_index = 1;
		}
		else if (event_active)
		{
			pos = event_pos;
			button_count = 2;
			menutitle_index = 2;
		}
		else if (menu_active)
		{
			pos = menu_pos;
			button_count = 11;
			menutitle_index = 3;
		}
		else if (setting_active)
		{
			pos = setting_pos;
			button_count = 5;
			menutitle_index = 4;
		}
		else
		{
			return;
		}

		Point<int16_t> mid_pos = Point<int16_t>(0, 29);

		uint16_t end_y = std::floor(28.2 * button_count);

		if (menu_active)
			end_y -= 1;

		uint16_t mid_y = end_y - mid_pos.y();

		menubackground[0].draw(position + pos + pos_adj);
		menubackground[1].draw(DrawArgument(position + pos + pos_adj) + DrawArgument(mid_pos, Point<int16_t>(0, mid_y)));
		menubackground[2].draw(position + pos + pos_adj + Point<int16_t>(0, end_y));

		menutitle[menutitle_index].draw(position + pos + pos_adj);

		for (size_t i = Buttons::BT_MENU_QUEST; i <= Buttons::BT_EVENT_DAILY; i++)
			buttons.at(i)->draw(position);
#pragma endregion
	}

	void UIStatusBar::update()
	{
		// 2026-11-22 兜底：起拖后图标交给 UIStateGame 的 draggedicon，后续
		// send_cursor 不再派发到本窗；若松手落在空白处（drop_on_stage，未走
		// send_icon），瞬态就无人复位。检测到主键已松开即清掉，避免快捷格
		// 被 :2964/:2969 的 qs_dragging 守卫永久吞掉单击/拖拽。
		if (qs_dragging && !UI::get().is_primary_down())
		{
			qs_dragging = false;
			qs_drag_slot = -1;
			qs_pressed = false;
		}

		if (classic_mode)
		{
			update_classic();
			return;
		}

		UIElement::update();

		for (Sprite sprite : hpmp_sprites)
			sprite.update();

		expbar.update(getexppercent());
		hpbar.update(gethppercent());
		mpbar.update(getmppercent());

		namelabel.change_text(stats.get_name());

		Point<int16_t> pos_adj = get_quickslot_pos();

		if (quickslot_active)
		{
			if (quickslot_adj.x() > quickslot_min)
			{
				int16_t new_x = quickslot_adj.x() - Constants::TIMESTEP;

				if (new_x < quickslot_min)
					quickslot_adj.set_x(quickslot_min);
				else
					quickslot_adj.shift_x(-Constants::TIMESTEP);
			}
		}
		else
		{
			if (quickslot_adj.x() < QUICKSLOT_MAX)
			{
				int16_t new_x = quickslot_adj.x() + Constants::TIMESTEP;

				if (new_x > QUICKSLOT_MAX)
					quickslot_adj.set_x(QUICKSLOT_MAX);
				else
					quickslot_adj.shift_x(Constants::TIMESTEP);
			}
		}

		for (size_t i = Buttons::BT_MENU_QUEST; i <= Buttons::BT_MENU_CLAIM; i++)
		{
			Point<int16_t> menu_adj = Point<int16_t>(0, 0);

			if (i == Buttons::BT_MENU_FISHING)
				menu_adj = Point<int16_t>(3, 1);

			buttons[i]->set_position(menu_pos + menu_adj + pos_adj);
		}

		for (size_t i = Buttons::BT_SETTING_CHANNEL; i <= Buttons::BT_SETTING_QUIT; i++)
			buttons[i]->set_position(setting_pos + pos_adj);

		for (size_t i = Buttons::BT_COMMUNITY_FRIENDS; i <= Buttons::BT_COMMUNITY_MAPLECHAT; i++)
			buttons[i]->set_position(community_pos + pos_adj);

		for (size_t i = Buttons::BT_CHARACTER_INFO; i <= Buttons::BT_CHARACTER_ITEM; i++)
			buttons[i]->set_position(character_pos + pos_adj);

		for (size_t i = Buttons::BT_EVENT_SCHEDULE; i <= Buttons::BT_EVENT_DAILY; i++)
			buttons[i]->set_position(event_pos + pos_adj);

		// 绑定变化后刷新 modern 快捷格图标（每秒一次足够）。
		static int modern_qs_refresh_ms = 0;
		modern_qs_refresh_ms += Constants::TIMESTEP;
		if (modern_qs_refresh_ms >= 1000)
		{
			modern_qs_refresh_ms = 0;
			refresh_modern_qs_icons();
		}
	}

	Button::State UIStatusBar::button_pressed(uint16_t id)
	{
		switch (id)
		{
			case Buttons::BT_CLASSIC_SHOP:
			{
				// 窗口内商城（方案 C）总开关：true→UIWindowCashShop，false→原生商城。
				// helper 内部对 false 路径等价于旧的 EnterCashShopPacket().dispatch()。
				UI::get().open_cashshop_entry();
				break;
			}
			case Buttons::BT_CASHSHOP:
			{
				// 与 BT_CLASSIC_SHOP 共用同一入口 helper（配置开关决定窗口/原生）。
				UI::get().open_cashshop_entry();
				break;
			}
			case Buttons::BT_CLASSIC_MENU:
			case Buttons::BT_MENU:
			{
				if (classic_mode)
				{
					// BtMenu（目录）：只开主菜单（UIGameMenu）。
					// 旧行为同时弹 UIOptionMenu（游戏设置），与参考端不符（用户实测反馈）；
					// 游戏设置由系统按键/主菜单内入口打开。
					// 互斥：开主菜单时先收起界面列表（UIShortCut）。
					if (auto sc = UI::get().get_element<UIShortCut>(); sc && sc->is_active())
						sc->deactivate();

					auto gm = UI::get().get_element<UIGameMenu>();
					if (!gm)
						gm = UI::get().emplace<UIGameMenu>();
					else if (!gm->is_active())
					{
						gm->reset_selection();
						gm->makeactive();
					}
					else
						gm->deactivate();
					break;
				}

				toggle_menu();
				break;
			}
			case Buttons::BT_CLASSIC_EQUIP:
			{
				if (!UiAssetResolve::usable(UiAssetResolve::window("Equip")))
					break;

				UI::get().emplace<UIEquipInventory>(
					Stage::get().get_player().get_inventory()
				);
				stop_button_blink(id);
				break;
			}
			case Buttons::BT_CLASSIC_INV:
			{
				if (!UiAssetResolve::usable(UiAssetResolve::window("Item")))
					break;

				UI::get().emplace<UIItemInventory>(
					Stage::get().get_player().get_inventory()
				);
				stop_button_blink(id);
				break;
			}
			case Buttons::BT_CLASSIC_STAT:
			{
				if (!UiAssetResolve::usable(UiAssetResolve::window("Stat")))
					break;

				UI::get().emplace<UIStatsInfo>(
					Stage::get().get_player().get_stats()
				);
				stop_button_blink(id);
				break;
			}
			case Buttons::BT_CLASSIC_SKILL:
			{
				if (!UiAssetResolve::usable(UiAssetResolve::window("Skill")))
					break;

				UI::get().emplace<UISkillBook>(
					Stage::get().get_player().get_stats(),
					Stage::get().get_player().get_skills()
				);
				stop_button_blink(id);
				break;
			}
			case Buttons::BT_CLASSIC_KEYSET:
			{
				UI::get().emplace<UIKeyConfig>(
					Stage::get().get_player().get_inventory(),
					Stage::get().get_player().get_skills()
				);
				break;
			}
			case Buttons::BT_CLASSIC_QS:
			case Buttons::BT_CLASSIC_QSD:
			{
				quickslot_active = !quickslot_active;
				sync_classic_qs_buttons();
				break;
			}
			case Buttons::BT_CLASSIC_NPT:
			{
				// BtNPT 位图为「帮助」，但帮助内容不在客户端：原生发 ENTER_MTS(0x9C)，
				// 由服务端 use_mts 决定进拍卖行 UI 还是 npc/9900001.js 脚本中心对话。
				EnterMtsPacket().dispatch();
				LOG(LOG_INFO, "UIStatusBar: Help(BtNPT) -> ENTER_MTS");
				break;
			}
			case Buttons::BT_CLASSIC_SHORT:
			{
				// BtShort「界面」= 只开 UIShortCut 面板（UIWindow.img/ShortCut 的 11 钮气球）。
				// 旧行为同时 emplace UIKeyConfig，导致点一次弹两窗（键盘设定另有
				// BT_CLASSIC_KEYSET 独立入口，见上）。
				// 互斥：开界面列表时先收起主菜单（UIGameMenu）。
				if (auto gm = UI::get().get_element<UIGameMenu>(); gm && gm->is_active())
					gm->deactivate();

				auto sc = UI::get().get_element<UIShortCut>();
				if (!sc)
					sc = UI::get().emplace<UIShortCut>();
				else if (!sc->is_active())
				{
					sc->reset_selection();
					sc->makeactive();
				}
				else
					// 再点一次「界面」必须收起。旧实现在这里什么都不做，
					// 于是气球展开后只能靠 Esc/X 关，鼠标点同一颗按钮关不掉。
					sc->deactivate();
				break;
			}
			case Buttons::BT_CLASSIC_WHISPER:
			{
				if (auto chatbar = UI::get().get_element<UIChatBar>())
					chatbar->focus_whisper();
				break;
			}
			case Buttons::BT_CLASSIC_CLAIM:
			{
				// BtClaim 旁路：打开 SlideMenu（展开快捷入口）
				auto slide = UI::get().get_element<UISlideMenu>();
				if (!slide)
					slide = UI::get().emplace<UISlideMenu>();
				else if (!slide->is_active())
					slide->makeactive();
				if (slide)
					slide->toggle_expand();
				break;
			}
			case Buttons::BT_OPTIONS:
			{
				toggle_setting();
				break;
			}
			case Buttons::BT_CHARACTER:
			{
				toggle_character();
				break;
			}
			case Buttons::BT_COMMUNITY:
			{
				toggle_community();
				break;
			}
			case Buttons::BT_EVENT:
			{
				toggle_event();
				break;
			}
			case Buttons::BT_FOLD_QS:
			{
				toggle_qs(false);
				break;
			}
			case Buttons::BT_EXTEND_QS:
			{
				toggle_qs(true);
				break;
			}
			case Buttons::BT_MENU_QUEST:
			{
				UI::get().emplace<UIQuestLog>(
					Stage::get().get_player().get_quests()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_MENU_MONSTER_COLLECTION:
			{
				UI::get().emplace<UIMonsterBook>();
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_CLAIM:
			{
				open_window_gated<UIClaim>("Claim", "Claim");
				open_window_gated<UIDelivery>("Delivery", "Delivery");
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_MEDAL:
			{
				// DailyGift 真根在 DailyGiftStage.img（非 UIWindow.img/DailyGift），
				// ctor 自带灰底回退；勿用 window() 门控误杀。
				auto ui = UI::get().get_element<UIDailyGift>();
				if (!ui) ui = UI::get().emplace<UIDailyGift>();
				else if (!ui->is_active()) ui->makeactive();
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_ACHIEVEMENT:
			{
				open_window_gated<UIQuestAlarm>("QuestAlarm", "QuestAlarm");
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_UNION:
			{
				// round85 D-G：v83「家族联盟」= UIUserList 五页签 Tab::UNION，
				// 菜单联盟钮同样收敛到共用窗，不再开独立 UIGuild 浮窗。
				auto userlist = UI::get().get_element<UIUserList>();
				if (!userlist)
					UI::get().emplace<UIUserList>(UIUserList::Tab::UNION);
				else if (!userlist->is_active())
					userlist->makeactive();
				else if (userlist->get_tab() != UIUserList::Tab::UNION)
					userlist->change_tab(UIUserList::Tab::UNION);
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_AUCTION:
			{
				// 2026-11-25 197 拍卖行闭环：点 StatusBar 拍卖按钮 → 开原生 UIAuction 窗
				// （读 UIAuction.img + 走 AUCTION_ACTION 协议，对端 s9 AuctionHandler）。
				// 旧 ENTER_MTS 走脚本中心/MTS，与 192 原生拍卖行不是同一功能，已替换。
				UI::get().remove(UIElement::Type::AUCTION);
				UI::get().emplace<UIAuction>();
				LOG(LOG_INFO, "UIStatusBar: Auction -> UIAuction (197 auction protocol)");
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_MONSTER_LIFE:
			{
				UI::get().emplace<UIMonsterBook>();
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_BATTLE:
			{
				// 战斗统计（192 Battle Analysis）：此前误接怪物嘉年华，修正为真正的战斗统计窗。
				// 窗口单例由 UI::emplace 管理（已开则 makeactive），侧栏入口走 FeatureRegistry 同一路径。
				auto bs = UI::get().get_element<UIBattleStats>();
				if (!bs)
					bs = UI::get().emplace<UIBattleStats>();
				else if (!bs->is_active())
					bs->makeactive();
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_FISHING:
			{
				open_window_gated<UIPlant>("plant", "plant");
				remove_menus();
				break;
			}
			case Buttons::BT_MENU_HELP:
			{
				EnterMtsPacket().dispatch();
				LOG(LOG_INFO, "UIStatusBar: Help(menu) -> ENTER_MTS");
				remove_menus();
				break;
			}
			case Buttons::BT_SETTING_CHANNEL:
			{
				UI::get().emplace<UIChannel>();

				remove_menus();
				break;
			}
			case Buttons::BT_SETTING_OPTION:
			{
				UI::get().emplace<UIOptionMenu>();

				remove_menus();
				break;
			}
			case Buttons::BT_SETTING_KEYS:
			{
				UI::get().emplace<UIKeyConfig>(
					Stage::get().get_player().get_inventory(),
					Stage::get().get_player().get_skills()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_SETTING_JOYPAD:
			{
				if (UiAssetResolve::usable(UiAssetResolve::window("JoyPad")))
					UI::get().emplace<UIJoypad>();
				else
					UiAssetResolve::diag_once("statusbar JoyPad",
						"JoyPad missing — SoftKey shell 预留");

				remove_menus();
				break;
			}
			case Buttons::BT_SETTING_QUIT:
			{
				UI::get().emplace<UIQuit>(stats);

				remove_menus();
				break;
			}
			case Buttons::BT_COMMUNITY_FRIENDS:
			case Buttons::BT_COMMUNITY_PARTY:
			{
				auto userlist = UI::get().get_element<UIUserList>();
				auto tab = (id == Buttons::BT_COMMUNITY_FRIENDS) ? UIUserList::Tab::FRIEND : UIUserList::Tab::PARTY;

				if (!userlist)
				{
					UI::get().emplace<UIUserList>(tab);
				}
				else
				{
					auto cur_tab = userlist->get_tab();
					auto is_active = userlist->is_active();

					if (cur_tab == tab)
					{
						if (is_active)
							userlist->deactivate();
						else
							userlist->makeactive();
					}
					else
					{
						if (!is_active)
							userlist->makeactive();

						userlist->change_tab(tab);
					}
				}

				remove_menus();
				break;
			}
			case Buttons::BT_COMMUNITY_GUILD:
			{
				// round85 D-G（按 Angel 083 收口）：家族 = UIUserList 第 3 页签
				// Tab::GUILD，与上方 FRIENDS/PARTY 走完全相同的共用窗 toggle 路径，
				// 不再实例化 GuildUI.img 虚构根的独立 UIGuild 浮窗。
				auto userlist = UI::get().get_element<UIUserList>();
				auto tab = UIUserList::Tab::GUILD;
				if (!userlist)
					UI::get().emplace<UIUserList>(tab);
				else if (userlist->get_tab() == tab)
				{
					if (userlist->is_active())
						userlist->deactivate();
					else
						userlist->makeactive();
				}
				else
				{
					if (!userlist->is_active())
						userlist->makeactive();
					userlist->change_tab(tab);
				}
				remove_menus();
				break;
			}
			case Buttons::BT_COMMUNITY_MAPLECHAT:
			{
				// 社区 mapleChat 槽 → Messenger（出站建房）
				if (UiAssetResolve::usable(UiAssetResolve::window("Messenger")))
				{
					UI::get().emplace<UIMessenger>();

					if (auto m = UI::get().get_element<UIMessenger>())
					{
						if (m->is_active())
							m->try_create_room();
					}
				}
				else
				{
					UI::get().emplace<UIChat>();
				}

				remove_menus();
				break;
			}
			case Buttons::BT_CHARACTER_INFO:
			{
				UI::get().emplace<UICharInfo>(
					Stage::get().get_player().get_oid()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_CHARACTER_STAT:
			{
				UI::get().emplace<UIStatsInfo>(
					Stage::get().get_player().get_stats()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_CHARACTER_SKILL:
			{
				UI::get().emplace<UISkillBook>(
					Stage::get().get_player().get_stats(),
					Stage::get().get_player().get_skills()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_CHARACTER_EQUIP:
			{
				UI::get().emplace<UIEquipInventory>(
					Stage::get().get_player().get_inventory()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_CHARACTER_ITEM:
			{
				UI::get().emplace<UIItemInventory>(
					Stage::get().get_player().get_inventory()
					);

				remove_menus();
				break;
			}
			case Buttons::BT_EVENT_SCHEDULE:
			{
				// 与 SoftKey/键盘 V 同入口：有 EventList → UIEvent，否则回退 UISpecial。
				open_event_window();
				auto ew = UI::get().get_element<UIEventWindow>();
				if (!ew)
					ew = UI::get().emplace<UIEventWindow>();
				else if (!ew->is_active())
					ew->makeactive();

				remove_menus();
				break;
			}
			case Buttons::BT_EVENT_DAILY:
			{
				auto ui = UI::get().get_element<UIDailyGift>();
				if (!ui) ui = UI::get().emplace<UIDailyGift>();
				else if (!ui->is_active()) ui->makeactive();
				remove_menus();
				break;
			}
		}

		return Button::State::NORMAL;
	}

	void UIStatusBar::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				if (!menu_active && !setting_active && !community_active && !character_active && !event_active)
					toggle_setting();
				else
					remove_menus();
			}
			else if (keycode == KeyAction::Id::RETURN)
			{
				// 修复：buttons 可能有缺失条目，禁止 operator[] 只读访问（会插入空 unique_ptr 再解引用）。
				auto btn_at = [this](uint16_t id) -> Button*
				{
					auto it = buttons.find(id);
					return (it != buttons.end()) ? it->second.get() : nullptr;
				};

				for (size_t i = Buttons::BT_MENU_QUEST; i <= Buttons::BT_EVENT_DAILY; i++)
					if (btn_at(static_cast<uint16_t>(i))
						&& btn_at(static_cast<uint16_t>(i))->get_state() == Button::State::MOUSEOVER)
						button_pressed(static_cast<uint16_t>(i));
			}
			else if (keycode == KeyAction::Id::UP || keycode == KeyAction::Id::DOWN)
			{
				uint16_t min_id, max_id;

				if (menu_active)
				{
					min_id = Buttons::BT_MENU_QUEST;
					max_id = Buttons::BT_MENU_CLAIM;
				}
				else if (setting_active)
				{
					min_id = Buttons::BT_SETTING_CHANNEL;
					max_id = Buttons::BT_SETTING_QUIT;
				}
				else if (community_active)
				{
					min_id = Buttons::BT_COMMUNITY_FRIENDS;
					max_id = Buttons::BT_COMMUNITY_MAPLECHAT;
				}
				else if (character_active)
				{
					min_id = Buttons::BT_CHARACTER_INFO;
					max_id = Buttons::BT_CHARACTER_ITEM;
				}
				else if (event_active)
				{
					min_id = Buttons::BT_EVENT_SCHEDULE;
					max_id = Buttons::BT_EVENT_DAILY;
				}

				uint16_t id = min_id;

				auto btn_at = [this](uint16_t bid) -> Button*
				{
					auto it = buttons.find(bid);
					return (it != buttons.end()) ? it->second.get() : nullptr;
				};

				for (size_t i = min_id; i <= max_id; i++)
				{
					Button* b = btn_at(static_cast<uint16_t>(i));
					if (b && b->get_state() != Button::State::NORMAL)
					{
						id = static_cast<uint16_t>(i);

						b->set_state(Button::State::NORMAL);
						break;
					}
				}

				if (keycode == KeyAction::Id::DOWN)
				{
					if (id < max_id)
						id++;
					else
						id = min_id;
				}
				else if (keycode == KeyAction::Id::UP)
				{
					if (id > min_id)
						id--;
					else
						id = max_id;
				}

				if (Button* b = btn_at(id))
					b->set_state(Button::State::MOUSEOVER);
			}
			else if (keycode == 'W' || keycode == 'w')
			{
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				if (!pre.empty()) {
					std::string to = pre;
					
				} else {
					
				}
			}
			else if (keycode == 'S' || keycode == 's')
			{
				
			}
			else if (keycode == 'E' || keycode == 'e')
			{
				
			}
			else if (keycode == 'C' || keycode == 'c')
			{
				
			}
			else if (keycode == 'H' || keycode == 'h')
			{
				int32_t from_inv = 0;
				if (auto inv = UI::get().get_element<UIItemInventory>()) {
					int16_t slot = inv->get_selected_slot();
					if (slot > 0)
						from_inv = Stage::get().get_player().get_inventory()
							.get_item_id(inv->get_tab(), slot);
				}
				if (from_inv > 0) {
					UseChairPacket(from_inv).dispatch();
					LOG(LOG_DEBUG, "UIStatusBar: SoftKey chair=" << from_inv);
				} else {
					
				}
			}
			else if (keycode == 'X' || keycode == 'x')
			{
				CancelChairPacket().dispatch();
				LOG(LOG_DEBUG, "UIStatusBar: CancelChair");
			}
			else if (keycode == 'D' || keycode == 'd')
			{
				// USE_DOOR 语义修正（同 UIGameMenu 'O'）：真门踩线自动上行走
				// MapFieldMarkers::check_doors；本键 SoftKey 输 ownerid 调试兜底。
				
				LOG(LOG_DEBUG, "UIStatusBar: UseDoor prompt");
			}
			else if (keycode == 'K' || keycode == 'k')
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				auto loot = Stage::get().get_drops().find_loot_at(pos);
				if (loot.first > 0) {
					PickupItemPacket(loot.first, loot.second).dispatch();
					LOG(LOG_DEBUG, "UIStatusBar: Pickup nearest oid=" << loot.first);
				} else {
					
				}
			}
			else if (keycode == 'R' || keycode == 'r')
			{
				int32_t near = 0;
				if (MapObjects* objs = Stage::get().get_reactors().get_reactors()) {
					Point<int16_t> p = Stage::get().get_player().get_position();
					int64_t bestd = -1;
					for (auto it = objs->begin(); it != objs->end(); ++it) {
						if (!it->second) continue;
						Point<int16_t> np = it->second->get_position();
						int64_t dx = static_cast<int64_t>(np.x()) - p.x();
						int64_t dy = static_cast<int64_t>(np.y()) - p.y();
						int64_t d = dx * dx + dy * dy;
						if (bestd < 0 || d < bestd) { bestd = d; near = it->second->get_oid(); }
					}
				}
				Point<int16_t> pos = Stage::get().get_player().get_position();
				if (near > 0) {
					DamageReactorPacket(near, pos, 0, 0).dispatch();
					LOG(LOG_DEBUG, "UIStatusBar: DamageReactor nearest oid=" << near);
				} else {
					
				}
			}
			else if (keycode == 'I' || keycode == 'i')
			{
				int32_t from_inv = 0;
				if (auto uii = UI::get().get_element<UIItemInventory>()) {
					int16_t slot = uii->get_selected_slot();
					if (slot > 0)
						from_inv = Stage::get().get_player().get_inventory().get_item_id(uii->get_tab(), slot);
				}
				if (from_inv > 0) {
					UseItemEffectPacket(from_inv).dispatch();
					LOG(LOG_DEBUG, "UIStatusBar: effect id=" << from_inv);
				} else {
					
				}
			}
			else if (keycode == 'B' || keycode == 'b')
			{
				CloseChalkboardPacket().dispatch();
				LOG(LOG_DEBUG, "UIStatusBar: CloseChalkboard");
			}
			else if (keycode == 'N' || keycode == 'n')
			{
				int32_t near = 0;
				if (MapObjects* npcs = Stage::get().get_npcs().get_npcs()) {
					Point<int16_t> p = Stage::get().get_player().get_position();
					int64_t bestd = -1;
					for (auto it = npcs->begin(); it != npcs->end(); ++it) {
						if (!it->second) continue;
						Point<int16_t> np = it->second->get_position();
						int64_t dx = static_cast<int64_t>(np.x()) - p.x();
						int64_t dy = static_cast<int64_t>(np.y()) - p.y();
						int64_t d = dx * dx + dy * dy;
						if (bestd < 0 || d < bestd) { bestd = d; near = it->second->get_oid(); }
					}
				}
				if (near > 0) {
					TalkToNPCPacket(near).dispatch();
					LOG(LOG_DEBUG, "UIStatusBar: TalkToNPC nearest oid=" << near);
				} else {
					
				}
			}
			else if (keycode == 'T' || keycode == 't')
			{
				auto ui = UI::get().get_element<UIItemSearch>();
				if (!ui) ui = UI::get().emplace<UIItemSearch>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'P' || keycode == 'p')
			{
				auto ui = UI::get().get_element<UISkillUp>();
				if (!ui) ui = UI::get().emplace<UISkillUp>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'F' || keycode == 'f')
			{
				auto ui = UI::get().get_element<UIFadeYesNo>();
				if (!ui) ui = UI::get().emplace<UIFadeYesNo>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'G' || keycode == 'g')
			{
				auto ui = UI::get().get_element<UIGMBoard>();
				if (!ui) ui = UI::get().emplace<UIGMBoard>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'Q' || keycode == 'q')
			{
				auto ui = UI::get().get_element<UIQuestLog>();
				if (!ui)
					ui = UI::get().emplace<UIQuestLog>(
						Stage::get().get_player().get_quests());
				else if (!ui->is_active())
					ui->makeactive();
			}
			else if (keycode == 'Z' || keycode == 'z')
			{
				auto ui = UI::get().get_element<UIWorldMap>();
				if (!ui) ui = UI::get().emplace<UIWorldMap>();
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'L' || keycode == 'l')
			{
				auto ui = UI::get().get_element<UIUserList>();
				if (!ui) ui = UI::get().emplace<UIUserList>(0);
				else if (!ui->is_active()) ui->makeactive();
			}
			else if (keycode == 'A' || keycode == 'a')
			{
				open_window_gated<UIAdmin>("Admin", "Admin");
			}
			else if (keycode == 'Y' || keycode == 'y')
			{
				open_window_gated<UIFamily>("Family", "Family");
			}
			else if (keycode == 'V' || keycode == 'v')
			{
				open_window_gated<UICoupon>("Coupon", "Coupon");
			}
			else if (keycode == 'M' || keycode == 'm')
			{
				open_window_gated<UIMaker>("Maker", "Maker");
			}
		}
	}

	bool UIStatusBar::is_in_range(Point<int16_t> cursor_position) const
	{
		Point<int16_t> pos;
		Rectangle<int16_t> bounds;

		if (!character_active && !community_active && !event_active && !menu_active && !setting_active)
		{
			if (!classic_mode)
			{
				// modern：底栏全宽可点（含 QS 左侧 menu 六钮与 quickSlot 顶带）。
				bounds = Rectangle<int16_t>(
					Point<int16_t>(0, position.y()),
					Point<int16_t>(VWIDTH, static_cast<int16_t>(VHEIGHT)));
			}
			else
			{
				pos = Point<int16_t>(position_x, position_y);
				bounds = Rectangle<int16_t>(pos, pos + dimension);
			}
		}
		else
		{
			uint8_t button_count;
			int16_t pos_y_adj;

			if (character_active)
			{
				pos = character_pos;
				button_count = 5;
				pos_y_adj = 248;
			}
			else if (community_active)
			{
				pos = community_pos;
				button_count = 4;
				pos_y_adj = 301;
			}
			else if (event_active)
			{
				pos = event_pos;
				button_count = 2;
				pos_y_adj = 417;
			}
			else if (menu_active)
			{
				pos = menu_pos;
				button_count = 11;
				pos_y_adj = -90;
			}
			else if (setting_active)
			{
				pos = setting_pos;
				button_count = 5;
				pos_y_adj = 248;
			}

			pos_y_adj += VHEIGHT - 600;

			Point<int16_t> pos_adj = get_quickslot_pos();
			pos = Point<int16_t>(pos.x(), std::abs(pos.y()) + pos_y_adj) + pos_adj;

			uint16_t end_y = std::floor(28.2 * button_count);

			bounds = Rectangle<int16_t>(pos, pos + Point<int16_t>(113, end_y + 35));
		}

		return bounds.contains(cursor_position);
	}

	UIElement::Type UIStatusBar::get_type() const
	{
		return TYPE;
	}

	void UIStatusBar::toggle_qs()
	{
		if (classic_mode)
		{
			quickslot_active = !quickslot_active;
			sync_classic_qs_buttons();
			return;
		}

		if (!menu_active && !setting_active && !community_active && !character_active && !event_active)
			toggle_qs(!quickslot_active);
	}

	void UIStatusBar::toggle_qs(bool enabled)
	{
		if (classic_mode)
		{
			quickslot_active = enabled;
			sync_classic_qs_buttons();
			return;
		}

		// 2026-11-19 真机反馈「点展开布局没变」根因：旧代码参数名 quick_slot_active
		// 遮蔽成员 quickslot_active，本行 `if (quickslot_active == quick_slot_active)`
		// 恒为真 → 永远 return，展开/收缩逻辑从未执行。参数改名 enabled 后正确比较。
		if (quickslot_active == enabled)
			return;

		quickslot_active = enabled;

		// IDA CQuickSlot::Toggle@0x16972a0：切换 m_bShowSlide，整层
		// RelMove(m_nInitX, m_nInitY + (收起?67:0))——不是改槽数，是整层下移 67px
		// 让第二行滑出可视区。modern_qs_count 保持恒定。
		quickslot_adj = Point<int16_t>(0, enabled ? 0 : 67);

		// 修复：禁止 operator[] 只读访问（可能插入空 unique_ptr 再解引用）。
		auto btn_at = [this](uint16_t id) -> Button*
		{
			auto it = buttons.find(id);
			return (it != buttons.end()) ? it->second.get() : nullptr;
		};

		if (Button* b = btn_at(Buttons::BT_FOLD_QS))
			b->set_active(quickslot_active);
		if (Button* b = btn_at(Buttons::BT_EXTEND_QS))
			b->set_active(!quickslot_active);

		// menu 六钮始终可见（横排在 QS 左侧），不随 QS 折叠互斥。
		for (uint16_t bid : { Buttons::BT_CASHSHOP, Buttons::BT_MENU, Buttons::BT_OPTIONS,
			Buttons::BT_CHARACTER, Buttons::BT_COMMUNITY, Buttons::BT_EVENT })
		{
			if (Button* b = btn_at(bid))
				b->set_active(true);
		}
	}

	Button* UIStatusBar::button_at(uint16_t id) const
	{
		auto it = buttons.find(id);
		return (it != buttons.end()) ? it->second.get() : nullptr;
	}

	int32_t UIStatusBar::blink_index(uint16_t id) const
	{
		switch (id)
		{
			case Buttons::BT_CLASSIC_EQUIP:  return 0;
			case Buttons::BT_CLASSIC_INV:    return 1;
			case Buttons::BT_CLASSIC_STAT:   return 2;
			case Buttons::BT_CLASSIC_SKILL:  return 3;
			default: return -1;
		}
	}

	// 083 a3==1（启动）：OnStatChanged 低等级<=10 升级 / CUtilDlgEx::Draw 任务指引。
	// 记启动时刻（此处用逻辑帧累计毫秒归零）+ 复位真 KEY_ANI 两帧动画。
	void UIStatusBar::start_button_blink(uint16_t id)
	{
		int32_t idx = blink_index(id);
		if (idx < 0)
			return;
		blink_elapsed_ms[idx] = 0;
		blink_ani[idx].reset();
	}

	void UIStatusBar::start_guide_blink(uint8_t idx)
	{
		static constexpr uint16_t kGuideBtns[4] = {
			Buttons::BT_CLASSIC_EQUIP, Buttons::BT_CLASSIC_INV,
			Buttons::BT_CLASSIC_STAT, Buttons::BT_CLASSIC_SKILL
		};
		if (idx >= 4)
			return;
		start_button_blink(kGuideBtns[idx]);
		LOG(LOG_DEBUG, "UIStatusBar: guide KEY_ANI start idx=" << static_cast<int>(idx));
	}

	void UIStatusBar::stop_guide_blink(uint8_t idx)
	{
		static constexpr uint16_t kGuideBtns[4] = {
			Buttons::BT_CLASSIC_EQUIP, Buttons::BT_CLASSIC_INV,
			Buttons::BT_CLASSIC_STAT, Buttons::BT_CLASSIC_SKILL
		};
		if (idx >= 4)
			return;
		stop_button_blink(kGuideBtns[idx]);
	}

	void UIStatusBar::stop_all_guide_blinks()
	{
		for (uint8_t i = 0; i < 4; ++i)
			stop_guide_blink(i);
	}

	// 083 a3==0（清除）：OnButtonClicked 2000-2003 开子窗 / OnChildNotify a3==100 /
	// Update 的 +90000ms 超时。-1 对齐 083 的 v8 = -1（未启动）。
	void UIStatusBar::stop_button_blink(uint16_t id)
	{
		int32_t idx = blink_index(id);
		if (idx < 0)
			return;
		blink_elapsed_ms[idx] = -1;
	}

	void UIStatusBar::set_buttons_active(std::initializer_list<uint16_t> ids, bool active)
	{
		for (uint16_t id : ids)
		{
			if (Button* b = button_at(id))
				b->set_active(active);
		}
	}

	void UIStatusBar::toggle_menu()
	{
		remove_active_menu(MenuType::MENU);

		menu_active = !menu_active;

		set_buttons_active({ Buttons::BT_MENU_ACHIEVEMENT, Buttons::BT_MENU_AUCTION,
			Buttons::BT_MENU_BATTLE, Buttons::BT_MENU_CLAIM, Buttons::BT_MENU_FISHING,
			Buttons::BT_MENU_HELP, Buttons::BT_MENU_MEDAL, Buttons::BT_MENU_MONSTER_COLLECTION,
			Buttons::BT_MENU_MONSTER_LIFE, Buttons::BT_MENU_QUEST, Buttons::BT_MENU_UNION }, menu_active);

		if (menu_active)
		{
			if (Button* b = button_at(Buttons::BT_MENU_QUEST))
				b->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::toggle_setting()
	{
		remove_active_menu(MenuType::SETTING);

		setting_active = !setting_active;

		set_buttons_active({ Buttons::BT_SETTING_CHANNEL, Buttons::BT_SETTING_QUIT,
			Buttons::BT_SETTING_JOYPAD, Buttons::BT_SETTING_KEYS, Buttons::BT_SETTING_OPTION }, setting_active);

		if (setting_active)
		{
			if (Button* b = button_at(Buttons::BT_SETTING_CHANNEL))
				b->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::toggle_community()
	{
		remove_active_menu(MenuType::COMMUNITY);

		community_active = !community_active;

		set_buttons_active({ Buttons::BT_COMMUNITY_PARTY, Buttons::BT_COMMUNITY_FRIENDS,
			Buttons::BT_COMMUNITY_GUILD, Buttons::BT_COMMUNITY_MAPLECHAT }, community_active);

		if (community_active)
		{
			if (Button* b = button_at(Buttons::BT_COMMUNITY_FRIENDS))
				b->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::toggle_character()
	{
		remove_active_menu(MenuType::CHARACTER);

		character_active = !character_active;

		set_buttons_active({ Buttons::BT_CHARACTER_INFO, Buttons::BT_CHARACTER_EQUIP,
			Buttons::BT_CHARACTER_ITEM, Buttons::BT_CHARACTER_SKILL, Buttons::BT_CHARACTER_STAT }, character_active);

		if (character_active)
		{
			if (Button* b = button_at(Buttons::BT_CHARACTER_INFO))
				b->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::toggle_event()
	{
		remove_active_menu(MenuType::EVENT);

		event_active = !event_active;

		set_buttons_active({ Buttons::BT_EVENT_DAILY, Buttons::BT_EVENT_SCHEDULE }, event_active);

		if (event_active)
		{
			if (Button* b = button_at(Buttons::BT_EVENT_SCHEDULE))
				b->set_state(Button::State::MOUSEOVER);

			Sound(Sound::Name::DLGNOTICE).play();
		}
	}

	void UIStatusBar::remove_menus()
	{
		if (menu_active)
			toggle_menu();
		else if (setting_active)
			toggle_setting();
		else if (community_active)
			toggle_community();
		else if (character_active)
			toggle_character();
		else if (event_active)
			toggle_event();
	}

	void UIStatusBar::remove_active_menu(MenuType type)
	{
		for (size_t i = Buttons::BT_MENU_QUEST; i <= Buttons::BT_EVENT_DAILY; i++)
		{
			if (Button* b = button_at(static_cast<uint16_t>(i)))
				b->set_state(Button::State::NORMAL);
		}

		if (menu_active && type != MenuType::MENU)
			toggle_menu();
		else if (setting_active && type != MenuType::SETTING)
			toggle_setting();
		else if (community_active && type != MenuType::COMMUNITY)
			toggle_community();
		else if (character_active && type != MenuType::CHARACTER)
			toggle_character();
		else if (event_active && type != MenuType::EVENT)
			toggle_event();
	}

	void UIStatusBar::sync_classic_qs_buttons()
	{
		if (!classic_mode)
			return;

		auto btn_at = [this](uint16_t id) -> Button*
		{
			auto it = buttons.find(id);
			return (it != buttons.end()) ? it->second.get() : nullptr;
		};

		// 方向语义（用户实测正确端 + WZ 解码双证）：StatusBar.img/QuickSlot = ▲ 上箭头，
		// QuickSlotD = ▼ 下箭头。功能框「隐藏」(quickslot_active=false) 时显示 ▲
		// （点击向上展开）；「显示」(true) 时显示 ▼（点击向下收起）。
		// 旧实现把两颗按钮的 active 条件整体写反，导致展开态显示 ▲、收起态显示 ▼；
		// 源码 :2215 的注释语义本是对的，注释与实现矛盾，以正确端实测为准。
		if (Button* b = btn_at(Buttons::BT_CLASSIC_QS))
			b->set_active(!quickslot_active);

		if (Button* b = btn_at(Buttons::BT_CLASSIC_QSD))
			b->set_active(quickslot_active);
	}

	Point<int16_t> UIStatusBar::get_quickslot_pos() const
	{
		if (quickslot_active)
		{
			if (VWIDTH == 800)
				return Point<int16_t>(0, -73);
			else
				return Point<int16_t>(0, -31);
		}

		return Point<int16_t>(0, 0);
	}

	bool UIStatusBar::is_menu_active()
	{
		return menu_active || setting_active || community_active || character_active || event_active;
	}

	float UIStatusBar::getexppercent() const
	{
		int32_t level = stats.get_stat(MapleStat::Id::LEVEL);
		// s9 999 级场景：满级后 required_exp 仍是超大值，直接相除会让 EXP 条错显。
		// MinMS UIStatusBar.cpp:1061-1073 满级直接返回 0。
		if (level >= ExpTable::LEVELCAP)
			return 0.0f;
		int64_t exp = stats.get_exp();
		int64_t need = ExpTable::required_exp(level);
		if (need <= 0)
			return 0.0f;
		float ratio = static_cast<float>(static_cast<double>(exp) / static_cast<double>(need));
		if (ratio < 0.0f) ratio = 0.0f;
		if (ratio > 1.0f) ratio = 1.0f;
		return ratio;
	}

	float UIStatusBar::gethppercent() const
	{
		int32_t hp = stats.get_stat(MapleStat::Id::HP);
		int32_t maxhp = stats.get_total(EquipStat::Id::HP);
		// 防 max<=0 除零（inf/NaN 会污染进度条绘制）。
		if (maxhp <= 0)
			return 0.0f;
		float ratio = static_cast<float>(hp) / static_cast<float>(maxhp);
		if (ratio < 0.0f) ratio = 0.0f;
		if (ratio > 1.0f) ratio = 1.0f;
		return ratio;
	}

	float UIStatusBar::getmppercent() const
	{
		int32_t mp = stats.get_stat(MapleStat::Id::MP);
		int32_t maxmp = stats.get_total(EquipStat::Id::MP);
		if (maxmp <= 0)
			return 0.0f;
		float ratio = static_cast<float>(mp) / static_cast<float>(maxmp);
		if (ratio < 0.0f) ratio = 0.0f;
		if (ratio > 1.0f) ratio = 1.0f;
		return ratio;
	}

	void UIStatusBar::init_classic()
	{
		nl::node sb = UiAssetResolve::status_bar_root();
		nl::node base = sb["base"];
		nl::node gauge = sb["gauge"];

		classic_backgrnd = Texture(base["backgrnd"]);
		classic_backgrnd2 = Texture(base["backgrnd2"]);
		classic_icon_memo = Texture(base["iconMemo"]);
		classic_graduation = Texture(gauge["graduation"]);
		classic_gauge_bar = Texture(gauge["bar"]);
		classic_gauge_gray = Texture(gauge["gray"]);

		// 快捷栏面板在下方按格数选型后赋值，此处不提前占位。

		// nxcache 脏 origin 会把底条整块抬离屏幕底边 / 左缘，统一清零为 TL 锚点。
		auto zero_origin = [](Texture& tex)
		{
			if (!tex.is_valid())
				return;

			Point<int16_t> o = tex.get_origin();

			if (o.x() != 0 || o.y() != 0)
				tex.shift(o);
		};

		zero_origin(classic_backgrnd);
		zero_origin(classic_backgrnd2);
		zero_origin(classic_icon_memo);
		zero_origin(classic_graduation);
		zero_origin(classic_gauge_bar);
		zero_origin(classic_gauge_gray);

		// ============ 经典底条布局（800 设计网格 + 可选水平居中）============
		// 权威来源：
		//  a) Angel 083 CUIStatusBar::OnCreate @ 0x8d01b2 逐字 CreateCtrl(caller, this, id, x, y, type)：
		//       BtShop=1000/(573,543)  1001/(629,543)  BtMenu=1002/(685,543)
		//       BtShort=1007/(741,543) EquipKey=2000/(618,515) InvenKey=2001/(648,515)
		//       StatKey=2002/(678,515) SkillKey=2003/(708,515) KeySet=1005/(738,515)
		//       QuickSlot=1006/(768,515) （1009/(573,515) = BtClaim，见 SP_1976_UI_STATUSBARIMG_BTCLAIM）
		//     创建窗体高 578、top≈22 → 底条相对 y = CreateY - 507：543→36、515→8。
		//     x 末端 741+54=795 / 768+28=796，证明设计网格宽 HUD_DESIGN_W=800。
		//  b) StatusBar.img 节点实测：base/backgrnd 817x71、quickSlot08/26/32 = 151/466/571。
		//  c) Angel 无多分辨率（左贴边）。本端用户锁定：按 content_w 居中——
		//     宽 QS 时 content_w=800+qs（QS 在 800 条右侧，禁止塞进 800）；QS08 仍 800 内右上；
		//     backgrnd 仍通栏。公式见 Util/HudLayout.h，禁止再扩 modern 分宽坐标表。

		// 参考截图（进入游戏页.png，1280x720，底栏 position 顶 = 649）逐像素判定：
		//   graduation 暗边框整幅跨 x218..557（= 340，与图集同宽）→ GAUGE_X=218、
		//   三段间距就是图集打包间距 0/109/222（旧「128/255」是把商城红按钮当成了段界）。
		//   边框 y700..717（18 行）、HP 红填充 y702..715（14 行）→ 图集 y0 = 687 → GAUGE_Y=38。
		//   （GAUGE_X/GAUGE_Y 已上移为文件级常量，draw_classic 的「百分比叠条」共用。）

		hpmp_pos = Point<int16_t>(GAUGE_X, GAUGE_Y);
		exp_pos = Point<int16_t>(GAUGE_X, GAUGE_Y);
		// number 字库锚点 = 参考截图 Lbracket 实测位（底栏 rel 坐标，数字基线偏差 0..1px）：
		// HP '[' 屏 x237 y690 / MP '[' x349 / EXP '[' x472 → 各段起点 + 烘焙标签宽度。
		hpset_pos = Point<int16_t>(static_cast<int16_t>(GAUGE_X + SEG_HP + 19), 41);
		mpset_pos = Point<int16_t>(static_cast<int16_t>(GAUGE_X + SEG_MP + 22), 41);
		statset_pos = Point<int16_t>(static_cast<int16_t>(GAUGE_X + SEG_EXP + 32), 41);
		// 等级徽章：Basic.img/LevelNo 橙盒数字 11x13，参考实测屏 x50..60 / y696..708
		// → rel (50,47)。多位数按 11+1 推进并以单字中心 x=55 居中。
		// 「LV.」前缀与右侧方括号：参考底板上没有可画的 Bracket（x61..84 全暗），
		// ShowLevel 仅作为 LevelNo 缺失时的回退字库。
		lvbadge_pos = Point<int16_t>(50, 47);
		// 芯片内两行（参考实测**墨迹顶** rel y：职业名 40、角色名 56；旧值 21/39
		// 落在聊天输入行 → 白字白底「职业没有显示」）。
		// 这里 y 槽存的是量到的墨迹顶，不是绘制点：绘制点在 draw_classic 里按
		// 本行文字自己的度量反解（R27-B1b Text::y_for_ink_top，SimHei 12px 的
		// CJK bitmap_top 实测 10 而非 r15 手抄的 12 ⇒ 此前两行各低 2/3px，
		// 就是用户说的「底栏职业/名称偏下」）。
		joblabel_pos = Point<int16_t>(87, 40);
		namelabel_pos = Point<int16_t>(87, 56);

		// 三联填充：同一图集按分段裁剪，HP/MP/EXP 各绘在各自分段起点。
		hpbar = Gauge(Gauge::Type::CLIP, classic_gauge_bar, SEG_W_HP, 0.0f);
		hpbar.set_clip_left(SEG_HP);
		mpbar = Gauge(Gauge::Type::CLIP, classic_gauge_bar, SEG_W_MP, 0.0f);
		mpbar.set_clip_left(SEG_MP);
		expbar = Gauge(Gauge::Type::CLIP, classic_gauge_bar, SEG_W_EXP, 0.0f);
		expbar.set_clip_left(SEG_EXP);

		// 闪光覆盖层对齐各分段可视框：图集边框自 y13 起（参考实测 700-687），
		// 109x18 再向左外扩 1px。（此前误用 GAUGE_Y，闪光画到了亮板上。）
		// 传 hpFlash/mpFlash 根而非 ["0"]：Animation 只有在拿到多帧根时才建 5 帧动画。
		if (gauge["hpFlash"]["0"])
		{
			classic_hp_flash_sprite = Sprite(gauge["hpFlash"], hpmp_pos + Point<int16_t>(-1, 13));
			classic_hp_flash = 0;
		}

		if (gauge["mpFlash"]["0"])
		{
			classic_mp_flash_sprite = Sprite(gauge["mpFlash"], hpmp_pos + Point<int16_t>(static_cast<int16_t>(SEG_MP - 1), 13));
			classic_mp_flash = 0;
		}

		if (sb["number"])
		{
			statset = Charset(sb["number"], Charset::Alignment::LEFT);
			hpmpset = Charset(sb["number"], Charset::Alignment::LEFT);
		}
#ifdef USE_NX
		{
			nl::node basic = nl::nx::UI["Basic.img"];

			if (basic["LevelNo"])
				classic_levelno = Charset(basic["LevelNo"], Charset::Alignment::LEFT);

			nl::node showlevel = basic["ShowLevel"];

			if (showlevel)
			{
				classic_showlevel = Charset(showlevel["0"], Charset::Alignment::LEFT);
				classic_showlevel_bracket = Texture(showlevel["Bracket"]["right"]);
			}
		}
#endif

		namelabel = OutlinedText(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::DARKGREY);

		// MapleButton 锚点 = CreateCtrl TL + canvas origin（StatusBar.img 各按钮 origin 实测均为 (0,0)，
		// 故锚点即网格坐标本身；保留 origin 相加以防更换资源后 origin 非零）。
		auto add_btn = [&](Buttons id, nl::node node, int16_t x, int16_t y)
		{
			if (!node)
				return;

			Point<int16_t> origin;
			if (node["normal"]["0"])
				origin = Texture(node["normal"]["0"]).get_origin();

			buttons[id] = std::make_unique<MapleButton>(
				node,
				Point<int16_t>(static_cast<int16_t>(x + origin.x()), static_cast<int16_t>(y + origin.y()))
			);
		};

		// 快捷栏选型：仅当 VWIDTH >= 800+qs_panel 才用 26/32，否则 quickSlot08。
		// 宽 QS 落在 800 条右侧（content_w=800+qs）；QS08 仍在 800 内右上。
		const bool slots32 = (CLASSIC_QS_SLOTS == 32);
		const bool wide = HudLayout::is_qs_wide(VWIDTH, slots32);
		classic_qs_wide = wide;
		if (wide)
		{
			classic_qs_count = CLASSIC_QS_SLOTS;
			classic_qs_cols = classic_qs_count == 32 ? 16 : 13;
			classic_quickslot = Texture(base[slots32 ? "quickSlot32" : "quickSlot26"]);
		}
		else
		{
			classic_qs_count = 8;
			classic_qs_cols = 4;
			classic_quickslot = Texture(base["quickSlot08"]);
		}
		zero_origin(classic_quickslot);

		const int16_t qs_panel_w = classic_quickslot.is_valid()
			? classic_quickslot.get_dimensions().x()
			: HudLayout::qs_panel_w(VWIDTH, slots32);
		const int16_t content_w = HudLayout::hud_content_w(VWIDTH, slots32);
		const int16_t hud_cx = HudLayout::hud_center_x(VWIDTH, content_w);
		// 2026-11-21：backgrnd 只覆盖整组内容宽（含快捷格），居中于 hud_cx。
		classic_content_w = content_w;
		classic_content_cx = hud_cx;

		// QS 组内 x：宽屏 = 800（或紧视口 vw-qs_w）；窄屏 = grid-qs08。y=-8 见历史截图取证。
		quickslot_pos = Point<int16_t>(
			HudLayout::qs_x_in_group(VWIDTH, slots32),
			-8
		);
		const int16_t group_x = static_cast<int16_t>(
			quickslot_pos.x() - HudLayout::qs_group_gap(VWIDTH));

		// IDA CreateCtrl：y=543 → 底条 36；y=515 → 底条 8。按钮簇整体定位于 group_x。
		add_btn(Buttons::BT_CLASSIC_SHOP, sb["BtShop"], group_x + 0, 36);
		add_btn(Buttons::BT_CLASSIC_NPT, sb["BtNPT"], group_x + 56, 36);
		add_btn(Buttons::BT_CLASSIC_MENU, sb["BtMenu"], group_x + 112, 36);
		add_btn(Buttons::BT_CLASSIC_SHORT, sb["BtShort"], group_x + 168, 36);

		add_btn(Buttons::BT_CLASSIC_EQUIP, sb["EquipKey"], group_x + 45, 8);
		add_btn(Buttons::BT_CLASSIC_INV, sb["InvenKey"], group_x + 75, 8);
		add_btn(Buttons::BT_CLASSIC_STAT, sb["StatKey"], group_x + 105, 8);
		add_btn(Buttons::BT_CLASSIC_SKILL, sb["SkillKey"], group_x + 135, 8);

		// D9 · 083 SetButtonBlink 的 SP_1986..1989_..._KEY_ANI 四组真动画：
		// .nx 实测 StatusBar.img/<Key>/ani/{0,1} 两帧、delay 500ms（1Hz 常态<->发光）。
		// 外部程序曾误判该资源缺失、改用手绘叠层，此处按 .nx 实测改回 083 真帧。
		blink_ani[0] = Animation(sb["EquipKey"]["ani"]);
		blink_ani[1] = Animation(sb["InvenKey"]["ani"]);
		blink_ani[2] = Animation(sb["StatKey"]["ani"]);
		blink_ani[3] = Animation(sb["SkillKey"]["ani"]);
		add_btn(Buttons::BT_CLASSIC_KEYSET, sb["KeySet"], group_x + 165, 8);
		// 快捷栏收起(上箭头 QuickSlot)/展开(下箭头 QuickSlotD)为同位单按钮：
		// 图标随 quickslot_active 上下切换，而非两颗按钮并存（用户实测反馈）。
		add_btn(Buttons::BT_CLASSIC_QS, sb["QuickSlot"], group_x + 195, 8);
		add_btn(Buttons::BT_CLASSIC_QSD, sb["QuickSlotD"], group_x + 195, 8);

		// 举报钮 BtClaim：Angel 083 CUIStatusBar::OnCreate 的 1009/(573,515)，与
		// BtShop 1000/(573,543) 同 x 不同 y（参考全幅图 1x 模板匹配 BtClaim x576 /
		// BtShop x576 双证）。旧值 group_x-15 把它挪到屏幕 x558，与 ChatBar 的
		// BtMin(x543) 叠成「红头盔压蓝方块」。按 IDA 真值改回 group_x+0。
		// 参考端该簇没有第二颗深蓝 ▶ —— 密语箭头是 ChatBar 在 chatTarget 框左端
		// 画的 BtWhisper(0,4)，故不再建 StatusBar 的 BT_CLASSIC_WHISPER，避免多一颗。
		if (sb["BtClaim"])
			add_btn(Buttons::BT_CLASSIC_CLAIM, sb["BtClaim"], group_x + 0, 8);

		// 量规数值改画 number 位图字库（W1）；标签 HP/MP/EXP 用 gauge/bar 图集烘焙，不建 Text。
		classic_exp_dot = ColorBox(1, 1, Color::Name::WHITE, 1.0f);
		classic_exp_pct_dot = ColorBox(1, 1, Color(0x8A2BE2FF), 1.0f);
		joblabel = OutlinedText(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::DARKGREY);
		classic_exp_text = OutlinedText(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::DARKGREY);

		// 键帽层：原生 Angel 在每格左上角画 UIWindow.img/KeyConfig/quickslotConfig/key
		// 的淡蓝斜体白框切图（用户 m01201 指定路径）。底板 base/quickSlot26 是空白格，
		// 键帽单独画；UIQuickslotConfig.cpp:41-78 已用同一资源做「变更快捷键」窗。
		// 取证：orange-wz 解出 68 张 key/* 位图（tools/_qs_montage_white.png），正确端
		// 图2 逐格 masked 模板 SAD 4.5~18.8，旧 StatusBar.img/key 灰贴纸 + 描边文字
		// SAD 96~108 ⇒ 不是同一套绘制。W3 coords.md §2.3「Basic.img 无字母位图⇒
		// 只能文字回退」只约束 Basic.img，不能外推到 UIWindow.img 这套资源。
		// 节点号与 KeyConfig::Key 枚举逐字相等，故按 maple key 编号直接索引。
#ifdef USE_NX
		nl::node qskey = nl::nx::UI["UIWindow.img"]["KeyConfig"]["quickslotConfig"]["key"];
		if (!qskey)
			qskey = nl::nx::UI["UIWindow2.img"]["KeyConfig"]["quickslotConfig"]["key"];
		if (qskey)
		{
			for (int id = 2; id <= 88; ++id)
			{
				nl::node n = qskey[std::to_string(id)];
				if (n)
					classic_qs_cap[id] = Texture(n);
			}
		}
#endif

		// 缺切图时的回退：旧 StatusBar.img/key/0..6 灰贴纸 + 描边文字（中文名仍走 Text）。
		for (size_t i = 0; i < 7; i++)
			classic_key_label[i] = Texture(sb["key"][static_cast<int16_t>(i)]);
		classic_qs_keytext = OutlinedText(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::DARKGREY);
		classic_qs_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "", 28);
		// 道具数量字模：与背包 Icon::draw 同源（Basic.img/ItemNo），原生药水格右下显示叠数。
		{
			nl::node itemno = nl::nx::UI["Basic.img"]["ItemNo"];
			if (itemno)
			{
				classic_qs_itemno = Charset(itemno, Charset::Alignment::LEFT);
				classic_qs_itemno_ok = true;
			}
		}

		quickslot[0] = classic_quickslot;
		quickslot_active = true;
		quickslot_adj = Point<int16_t>(0, 0);

		// HudGroup：x = hud_center_x(vw, content_w)；y 贴底。宽 QS 时 content_w=800+qs。
		position = Point<int16_t>(hud_cx, static_cast<int16_t>(VHEIGHT - CLASSIC_BAR_H));
		position_x = position.x();
		position_y = static_cast<int16_t>(position.y() - 9);
		dimension = Point<int16_t>(VWIDTH, 80);

		LOG(LOG_DEBUG, "[MR] StatusBar classic HudGroup cx=" << hud_cx
			<< " content_w=" << content_w << " qs_x=" << quickslot_pos.x()
			<< " qs_w=" << qs_panel_w << " group_x=" << group_x
			<< " vw=" << VWIDTH << " vh=" << VHEIGHT);

		refresh_classic_qs_icons();
		// init 时两颗箭头刚 add_btn 完都还是 MapleButton 默认态，必须按当前
		// quickslot_active 同步一次，否则进图首帧方向错 / 两颗箭头重叠双显。
		sync_classic_qs_buttons();
	}

	Point<int16_t> UIStatusBar::modern_qs_cell_at(size_t slot) const
	{
		const Point<int16_t> qs_nudge =
			(VWIDTH > 800 && VWIDTH < 1366) ? Point<int16_t>(-1, 0) : Point<int16_t>(0, 0);
		const Point<int16_t> base = position + quickslot_pos + quickslot_adj + qs_nudge;

		if (slot >= MODERN_QS_SLOTS)
			return base;

		return base + MODERN_QS_POS[slot];
	}

	// 2026-11-19 新增：modern(192) 快捷格命中测试（拖放 / 点击共用）。
	// 返回命中格索引，-1 = 未命中。格 32×32，与 draw_modern_quickslot 同一几何。
	int16_t UIStatusBar::modern_qs_slot_at(Point<int16_t> cursor_position) const
	{
		const size_t count = std::min(modern_qs_count, MODERN_QS_SLOTS);

		for (size_t i = 0; i < count; ++i)
		{
			const Point<int16_t> cell = modern_qs_cell_at(i);
			if (Rectangle<int16_t>(cell, cell + Point<int16_t>(32, 32)).contains(cursor_position))
				return static_cast<int16_t>(i);
		}

		return -1;
	}

	bool UIStatusBar::cursor_on_interactive(Point<int16_t> cursor_position) const
	{
		if (classic_mode)
			return false;

		for (const auto& btit : buttons)
		{
			if (!btit.second || !btit.second->is_active())
				continue;
			if (btit.second->bounds(position).contains(cursor_position))
				return true;
		}

		if (quickslot_active)
		{
			const size_t count = std::min(modern_qs_count, MODERN_QS_SLOTS);
			for (size_t i = 0; i < count; ++i)
			{
				const Point<int16_t> cell = modern_qs_cell_at(i);
				if (Rectangle<int16_t>(cell, cell + Point<int16_t>(32, 32)).contains(cursor_position))
					return true;
			}
		}

		return false;
	}

	void UIStatusBar::refresh_modern_qs_icons()
	{
		const Keyboard& keyboard = UI::get().get_keyboard();

#ifdef USE_NX
		nl::node key_icons = UiAssetResolve::status_bar_root()["KeyConfig"]["icon"];
		if (!key_icons)
			key_icons = nl::nx::UI["UIWindow.img"]["KeyConfig"]["icon"];
		if (!key_icons)
			key_icons = nl::nx::UI["UIWindow2.img"]["KeyConfig"]["icon"];
#endif

		const size_t count = std::min(modern_qs_count, MODERN_QS_SLOTS);
		for (size_t i = 0; i < MODERN_QS_SLOTS; i++)
		{
			classic_qs_icons[i] = Texture();
			classic_qs_names[i].clear();
			classic_qs_counts[i].clear();
			if (i >= count)
				continue;

			// 2026-11-19 用户裁决：去掉「前8槽走玩家可改绑定」，20 格统一按
			// 083 同构键位表 MODERN_QS_KEYS 显示（192 2×10 与 083 前20同构）。
			const uint8_t maple_key = MODERN_QS_KEYS[i];

			Keyboard::Mapping mapping = keyboard.get_maple_mapping(maple_key);

			if (mapping.type == KeyType::Id::SKILL)
			{
				const SkillData& skill = SkillData::get(mapping.action);
				const Texture& icon = skill.get_icon(SkillData::Icon::NORMAL);
				if (icon.is_valid())
					classic_qs_icons[i] = icon;
				classic_qs_names[i] = skill.get_name();
			}
			else if (mapping.type == KeyType::Id::ITEM)
			{
				const ItemData& item = ItemData::get(mapping.action);
				if (item)
				{
					classic_qs_icons[i] = item.get_icon(false);
					int16_t qty = Stage::get().get_player().get_inventory().get_total_item_count(mapping.action);
					if (qty > 1)
						classic_qs_counts[i] = std::to_string(qty);
				}
			}
#ifdef USE_NX
			else if (mapping.type == KeyType::Id::MACRO)
			{
				nl::node mi = nl::nx::UI["UIWindow.img"]["SkillMacro"]["Macroicon"][mapping.action];
				if (mi["icon"])
				{
					Texture mtex = mi["icon"];
					mtex.shift(mtex.get_origin());
					classic_qs_icons[i] = mtex;
				}
			}
			else if (mapping.type == KeyType::Id::MENU || mapping.type == KeyType::Id::ACTION
				|| mapping.type == KeyType::Id::FACE)
			{
				nl::node n = key_icons[std::to_string(mapping.action)];
				if (!n)
					n = key_icons[mapping.action];
				if (n)
					classic_qs_icons[i] = Texture(n);
			}
#endif
		}
	}

	void UIStatusBar::draw_modern_quickslot(float) const
	{
		if (!quickslot_active)
			return;

		const size_t count = std::min(modern_qs_count, MODERN_QS_SLOTS);
		const int16_t right_limit = static_cast<int16_t>(VWIDTH - 34); // 格右缘不得超出客户端

		for (size_t i = 0; i < count; i++)
		{
			const Point<int16_t> cell_pos = modern_qs_cell_at(i);
			// 展开态格数可能超出右缘 → 跳过，避免 UI 溢出客户端。
			if (cell_pos.x() + 32 > right_limit)
				continue;

			// 2026-11-19 图1校准：图标整格居中（origin 抵消后视觉左上 = cell + (32-dim)/2）。
			// 键帽本就压在图标左上角（图1 正确端同样如此），不是错误重叠。
			// 此前「图标在键帽下方区居中」的 CAP_H 方案反而让图标下移出格，已回退。
			if (classic_qs_icons[i])
			{
				const Point<int16_t> org = classic_qs_icons[i].get_origin();
				const Point<int16_t> dim = classic_qs_icons[i].get_dimensions();
				const Point<int16_t> lt = cell_pos + Point<int16_t>(
					static_cast<int16_t>((32 - dim.x()) / 2),
					static_cast<int16_t>((32 - dim.y()) / 2));
				classic_qs_icons[i].draw(lt + org);
			}

			// 与图标一致：键帽也统一按 083 同构表显示，不再用玩家可改绑定。
			const uint8_t mk = MODERN_QS_KEYS[i];

			int16_t cap_id = static_cast<int16_t>(mk);
			switch (mk)
			{
			case KeyConfig::Key::RIGHT_CONTROL: cap_id = KeyConfig::Key::LEFT_CONTROL; break;
			case KeyConfig::Key::RIGHT_SHIFT: cap_id = KeyConfig::Key::LEFT_SHIFT; break;
			case KeyConfig::Key::RIGHT_ALT: cap_id = KeyConfig::Key::LEFT_ALT; break;
			default: break;
			}

			// StatusBar3 KeyConfig/key/* 为顶带小条（约 28×9）；画在格内顶部偏下 1px，
			// 避免压住格子上边框（图1 键帽在格内、不越界）。
			constexpr Point<int16_t> CAP_TOP(2, 1);
			if (cap_id > 0 && cap_id < static_cast<int16_t>(classic_qs_cap.size()) && classic_qs_cap[cap_id])
			{
				classic_qs_cap[cap_id].draw(cell_pos + CAP_TOP);
			}
			else
			{
				// 切图缺失时用短名回退，避免空格完全无键位提示。
				const char* name = "";
				switch (mk)
				{
				case KeyConfig::Key::LEFT_SHIFT: case KeyConfig::Key::RIGHT_SHIFT: name = "Sft"; break;
				case KeyConfig::Key::LEFT_CONTROL: case KeyConfig::Key::RIGHT_CONTROL: name = "Ctl"; break;
				case KeyConfig::Key::INSERT: name = "Ins"; break;
				case KeyConfig::Key::HOME: name = "Hm"; break;
				case KeyConfig::Key::PAGE_UP: name = "Pup"; break;
				case KeyConfig::Key::DELETE: name = "Del"; break;
				case KeyConfig::Key::END: name = "End"; break;
				case KeyConfig::Key::PAGE_DOWN: name = "Pdn"; break;
				case KeyConfig::Key::NUM1: name = "1"; break;
				case KeyConfig::Key::NUM2: name = "2"; break;
				case KeyConfig::Key::NUM3: name = "3"; break;
				case KeyConfig::Key::NUM4: name = "4"; break;
				case KeyConfig::Key::NUM5: name = "5"; break;
				case KeyConfig::Key::NUM6: name = "6"; break;
				case KeyConfig::Key::NUM7: name = "7"; break;
				case KeyConfig::Key::NUM8: name = "8"; break;
				case KeyConfig::Key::Q: name = "Q"; break;
				case KeyConfig::Key::W: name = "W"; break;
				case KeyConfig::Key::E: name = "E"; break;
				case KeyConfig::Key::R: name = "R"; break;
				case KeyConfig::Key::A: name = "A"; break;
				case KeyConfig::Key::S: name = "S"; break;
				case KeyConfig::Key::D: name = "D"; break;
				case KeyConfig::Key::F: name = "F"; break;
				case KeyConfig::Key::Z: name = "Z"; break;
				case KeyConfig::Key::X: name = "X"; break;
				case KeyConfig::Key::C: name = "C"; break;
				case KeyConfig::Key::V: name = "V"; break;
				default: break;
				}
				if (name[0])
				{
					classic_qs_keytext.change_text(name);
					classic_qs_keytext.draw(cell_pos + Point<int16_t>(2, 0));
				}
			}

			if (classic_qs_itemno_ok && !classic_qs_counts[i].empty())
			{
				classic_qs_itemno.draw(
					classic_qs_counts[i],
					DrawArgument(cell_pos + Point<int16_t>(16, 18)));
			}
		}
	}

	void UIStatusBar::refresh_classic_qs_icons()
	{
		const Keyboard& keyboard = UI::get().get_keyboard();

#ifdef USE_NX
		nl::node key_icons = nl::nx::UI["UIWindow.img"]["KeyConfig"]["icon"];
		if (!key_icons)
			key_icons = nl::nx::UI["UIWindow2.img"]["KeyConfig"]["icon"];
#endif

		for (size_t i = 0; i < CLASSIC_QS_SLOTS; i++)
		{
			classic_qs_icons[i] = Texture();
			classic_qs_names[i].clear();
			classic_qs_counts[i].clear();
			// 8 槽快捷栏：触发键走 Keyboard::quickslots（可被 CHANGE_QUICKSLOT 改）。
			// 26/32 宽格仍用 CLASSIC_QS_KEYS 固定表。
			uint8_t maple_key = (classic_qs_count == 8 && i < 8)
				? static_cast<uint8_t>(keyboard.get_quickslot_key(static_cast<uint8_t>(i)))
				: CLASSIC_QS_KEYS[i];
			Keyboard::Mapping mapping = keyboard.get_maple_mapping(maple_key);

			if (mapping.type == KeyType::Id::SKILL)
			{
				const SkillData& skill = SkillData::get(mapping.action);
				const Texture& icon = skill.get_icon(SkillData::Icon::NORMAL);

				if (icon.is_valid())
					classic_qs_icons[i] = icon;

				classic_qs_names[i] = skill.get_name();
			}
			else if (mapping.type == KeyType::Id::ITEM)
			{
				const ItemData& item = ItemData::get(mapping.action);

				if (item)
				{
					classic_qs_icons[i] = item.get_icon(false);
					// 原生快捷格道具显示「数量」而非名称（用户 bug 2）：
					// 名称走 classic_qs_names（中文 Text 会溢出/错位），数量走 classic_qs_counts + ItemNo 字模。
					int16_t qty = Stage::get().get_player().get_inventory().get_total_item_count(mapping.action);
					if (qty > 1)
						classic_qs_counts[i] = std::to_string(qty);
				}
			}
#ifdef USE_NX
			else if (mapping.type == KeyType::Id::MACRO)
			{
				// V2：整行宏绑到快捷格时画 Macroicon/<idx>/icon，与键位窗同一素材。
				nl::node mi = nl::nx::UI["UIWindow.img"]["SkillMacro"]["Macroicon"][mapping.action];
				if (mi["icon"])
				{
					Texture mtex = mi["icon"];
					mtex.shift(mtex.get_origin());
					classic_qs_icons[i] = mtex;
				}
				classic_qs_names[i] = SkillMacros::get().get_slot(static_cast<size_t>(mapping.action)).name;
			}
			else if (mapping.type == KeyType::Id::MENU || mapping.type == KeyType::Id::ACTION
				|| mapping.type == KeyType::Id::FACE)
			{
				// MENU/ACTION/FACE：KeyConfig/icon/<actionId>
				nl::node n = key_icons[std::to_string(mapping.action)];
				if (!n)
					n = key_icons[mapping.action];
				if (n)
					classic_qs_icons[i] = Texture(n);
			}
#endif
		}
	}

	Point<int16_t> UIStatusBar::classic_qs_cell_at(size_t slot, int16_t cols) const
	{
		const int16_t c = cols > 0 ? cols : 1;
		const int16_t col = static_cast<int16_t>(slot % static_cast<size_t>(c));
		const int16_t row = static_cast<int16_t>(slot / static_cast<size_t>(c));

		return position + quickslot_pos + Point<int16_t>(
			static_cast<int16_t>(QS_FIRST_X + col * QS_PITCH_X),
			static_cast<int16_t>(QS_FIRST_Y + row * QS_PITCH_Y)
		);
	}

	int16_t UIStatusBar::classic_qs_slot_at(Point<int16_t> cursor_position) const
	{
		if (!classic_mode || !quickslot_active || !classic_quickslot)
			return -1;

		const int16_t cols = classic_qs_cols;
		const size_t count = std::min(classic_qs_count, CLASSIC_QS_SLOTS);

		for (size_t i = 0; i < count; i++)
		{
			const Point<int16_t> tl = classic_qs_cell_at(i, cols);
			Rectangle<int16_t> cell(
				tl,
				tl + Point<int16_t>(QS_SLOT, QS_SLOT)
			);

			if (cell.contains(cursor_position))
				return static_cast<int16_t>(i);
		}

		return -1;
	}

	// 2026-11-19 modern(192) 快捷格：单击使用（keydown → keyup 由 caller 配对）。
	void UIStatusBar::activate_modern_qs_slot(size_t slot)
	{
		if (slot >= MODERN_QS_SLOTS)
			return;

		const Keyboard& keyboard = UI::get().get_keyboard();
		Keyboard::Mapping mapping = keyboard.get_maple_mapping(MODERN_QS_KEYS[slot]);

		if (mapping.type == KeyType::Id::NONE)
			return;

		Sound(Sound::Name::BUTTONCLICK).play();
		Stage::get().send_key(mapping.type, mapping.action, true);
	}

	void UIStatusBar::release_modern_qs_slot(size_t slot)
	{
		if (slot >= MODERN_QS_SLOTS)
			return;

		const Keyboard& keyboard = UI::get().get_keyboard();
		Keyboard::Mapping mapping = keyboard.get_maple_mapping(MODERN_QS_KEYS[slot]);

		if (mapping.type == KeyType::Id::NONE)
			return;

		Stage::get().send_key(mapping.type, mapping.action, false);
	}

	void UIStatusBar::activate_classic_qs_slot(size_t slot)
	{
		if (slot >= CLASSIC_QS_SLOTS)
			return;

		const Keyboard& keyboard = UI::get().get_keyboard();
		uint8_t maple_key = (classic_qs_count == 8 && slot < 8)
			? static_cast<uint8_t>(keyboard.get_quickslot_key(static_cast<uint8_t>(slot)))
			: CLASSIC_QS_KEYS[slot];
		Keyboard::Mapping mapping = keyboard.get_maple_mapping(maple_key);

		if (mapping.type == KeyType::Id::NONE)
			return;

		Sound(Sound::Name::BUTTONCLICK).play();
		Stage::get().send_key(mapping.type, mapping.action, true);
	}

	void UIStatusBar::show_qs_tooltip(int16_t slot)
	{
		// 2026-11-23：原生快捷格 hover 提示（对应 083 CUIStatusBar::ProcessToolTip/
		// SetShortcutTooltip）。此前本端快捷栏 send_cursor 只返回 CANCLICK，技能/
		// 消耗物品悬停无任何提示。按当前模式解析 slot 对应的键位映射，再用与
		// UIKeyConfig 相同的 KEYCONFIG parent 挂 tooltip。
		if (slot < 0)
			return;

		const Keyboard& keyboard = UI::get().get_keyboard();
		uint8_t maple_key;

		if (classic_mode)
		{
			// 8 槽布局触发键可被 CHANGE_QUICKSLOT 改；26/32 宽格用固定表。
			maple_key = (classic_qs_count == 8 && static_cast<size_t>(slot) < 8)
				? static_cast<uint8_t>(keyboard.get_quickslot_key(static_cast<uint8_t>(slot)))
				: CLASSIC_QS_KEYS[static_cast<size_t>(slot) % CLASSIC_QS_SLOTS];
		}
		else
		{
			maple_key = MODERN_QS_KEYS[static_cast<size_t>(slot) % MODERN_QS_SLOTS];
		}

		Keyboard::Mapping mapping = keyboard.get_maple_mapping(maple_key);

		if (mapping.type == KeyType::Id::ITEM)
		{
			UI::get().show_item(Tooltip::Parent::KEYCONFIG, mapping.action);
		}
		else if (mapping.type == KeyType::Id::SKILL)
		{
			const SkillBook& book = Stage::get().get_player().get_skills();
			int32_t level = book.get_level(mapping.action);
			int32_t masterlevel = book.get_masterlevel(mapping.action);
			int64_t expiration = book.get_expiration(mapping.action);
			UI::get().show_skill(Tooltip::Parent::KEYCONFIG, mapping.action,
				level, masterlevel, expiration);
		}
		// MACRO/其它类型原生无 tooltip，保持不挂。
	}

	// 原生「拖出快捷格即清除绑定」（MinMS UIStatusBar.cpp:915-935 unbind_key）：
	// 拖到非快捷区域松手时解除源格绑定，本地立即生效 + 135 包持久化 + 重绘。
	void UIStatusBar::QsIconType::drop_on_stage() const
	{
		if (source_key == 0)
			return;
		Keyboard& kb = UI::get().get_keyboard();
		kb.remove(source_key);
		std::vector<std::tuple<KeyConfig::Key, KeyType::Id, int32_t>> cleared;
		cleared.emplace_back(static_cast<KeyConfig::Key>(source_key), KeyType::Id::NONE, KeyAction::Id::LENGTH);
		ChangeKeyMapPacket(cleared).dispatch();
		// 重绘由 UIStateGame 松手后的 redraw_quickslots 收口，此处不重复。
	}

	void UIStatusBar::setup_qs_drag_icon(size_t slot, Point<int16_t> cursor_position)
	{
		const Keyboard& keyboard = UI::get().get_keyboard();
		uint8_t maple_key = classic_mode
			? ((classic_qs_count == 8 && slot < 8)
				? static_cast<uint8_t>(keyboard.get_quickslot_key(static_cast<uint8_t>(slot)))
				: CLASSIC_QS_KEYS[slot])
			: MODERN_QS_KEYS[std::min(slot, MODERN_QS_SLOTS - 1)];
		Keyboard::Mapping mapping = keyboard.get_maple_mapping(maple_key);

		qs_icon_type = std::make_unique<QsIconType>();
		qs_icon_type->bind_type = mapping.type;
		qs_icon_type->bind_action = mapping.action;
		qs_icon_type->source_key = maple_key;

		Texture tex;
		int16_t count = -1;
		if (mapping.type == KeyType::Id::ITEM)
		{
			count = Stage::get().get_player().get_inventory().get_total_item_count(mapping.action);
			tex = ItemData::get(mapping.action).get_icon(mapping.action / 10000 == 238);
		}
		else if (mapping.type == KeyType::Id::SKILL)
		{
			tex = SkillData::get(mapping.action).get_icon(SkillData::Icon::NORMAL);
		}
		else if (mapping.type == KeyType::Id::MACRO)
		{
#ifdef USE_NX
			nl::node mi = nl::nx::UI["UIWindow.img"]["SkillMacro"]["Macroicon"][mapping.action];
			if (mi["icon"])
				tex = Texture(mi["icon"]);
#endif
		}
		else if (mapping.type == KeyType::Id::MENU || mapping.type == KeyType::Id::ACTION
			|| mapping.type == KeyType::Id::FACE)
		{
			// 原生「自带按键图标」(功能键)：与 refresh_modern_qs_icons /
			// refresh_classic_qs_icons 的绘制端同源（同为 MENU/ACTION/FACE，
			// 不含 CASH——KeyConfig/icon 未给 CASH 图标，避免抓起空贴图），
			// 取 KeyConfig/icon/<action>。
			// 功能键此前无此分支 → 抓起空贴图，表现为「拖不动」；补齐后拖拽预览
			// 与技能/道具一致。图标 origin (0,32) 一并抹平，避免预览上浮一格。
#ifdef USE_NX
			nl::node key_icons = UiAssetResolve::status_bar_root()["KeyConfig"]["icon"];
			if (!key_icons)
				key_icons = nl::nx::UI["UIWindow.img"]["KeyConfig"]["icon"];
			if (!key_icons)
				key_icons = nl::nx::UI["UIWindow2.img"]["KeyConfig"]["icon"];
			if (key_icons)
			{
				nl::node n = key_icons[std::to_string(mapping.action)];
				if (!n)
					n = key_icons[mapping.action];
				if (n)
				{
					tex = Texture(n);
					tex.shift(tex.get_origin());
				}
			}
#endif
		}

		// 2026-11-22 修「抓起的消耗品图标浮在鼠标上方」：
		// 道具 get_icon(false) 贴图 origin.y≈32（底边锚点）。快捷栏原位图标是裸
		// Texture，draw 时用 cell_pos+origin.y 补偿；但 drag icon 是 Icon 对象，
		// QsIconType.get_type()==KEY → Icon 构造不会像 ITEM 型那样自动 shift(0,32)，
		// 于是 dragdraw=cursor-offset-origin 少减了 32 → 图标整体上浮一格（32px）。
		// 仅对道具补齐（技能/宏贴图 origin 口径不同，勿动），与背包道具 drag 一致。
		if (mapping.type == KeyType::Id::ITEM)
			tex.shift(Point<int16_t>(0, 32));

		qs_drag_icon = Icon(std::move(qs_icon_type), tex, count);
		// 拖拽偏移 = 光标相对起点格左上角；drag_icon 后图标跟手，不会跳格。
		const Point<int16_t> origin_cell = classic_mode
			? classic_qs_cell_at(slot, classic_qs_cols)
			: modern_qs_cell_at(std::min(slot, MODERN_QS_SLOTS - 1));
		qs_drag_icon.start_drag(cursor_position - origin_cell);
		UI::get().drag_icon(&qs_drag_icon);
	}

	void UIStatusBar::release_classic_qs_slot(size_t slot)
	{
		if (slot >= CLASSIC_QS_SLOTS)
			return;

		const Keyboard& keyboard = UI::get().get_keyboard();
		uint8_t maple_key = (classic_qs_count == 8 && slot < 8)
			? static_cast<uint8_t>(keyboard.get_quickslot_key(static_cast<uint8_t>(slot)))
			: CLASSIC_QS_KEYS[slot];
		Keyboard::Mapping mapping = keyboard.get_maple_mapping(maple_key);

		if (mapping.type == KeyType::Id::NONE)
			return;

		// 补发 keyup：Stage 每帧读 is_key_down(ATTACK) 驱动连击（Stage.cpp:324），
		// 若快捷栏单击只 keydown 不 keyup，keysdown[ATTACK] 永久为 true → 无限攻击。
		// 松手（或移出槽）时发 false，使按住=连击、松开=停止，符合原生键盘语义。
		Stage::get().send_key(mapping.type, mapping.action, false);
	}

	// turn13：宏/技能图标拖到底部快捷格 = 直接绑定该格键位（原生 KeyConfig 语义）。
	// 基类 send_icon 返回 true 会把图标无声吞掉 —— 状态栏永远 active，get_front
	// 反向扫描先命中它，拖到快捷格的图标从此再也到不了 UIKeyConfig。
	bool UIStatusBar::send_icon(const Icon& icon, Point<int16_t> cursor_position)
	{
		// 2026-11-19 用户报「快捷键上道具/按键/技能无法拖拽」根因：
		// 原守卫 `!classic_mode` 直接 return false，modern(192) 模式下快捷格
		// 永远不接受拖放。现代分支改用 modern_qs_cell_at + MODERN_QS_KEYS。
		if (!classic_mode && !quickslot_active)
			return false;

		int16_t slot = classic_mode
			? classic_qs_slot_at(cursor_position)
			: modern_qs_slot_at(cursor_position);

		if (slot < 0)
			return false;

		// 组装等价 Keyboard::Mapping：技能图标走 (SKILL, sid)；
		// 整行宏图标走 (MACRO, idx)；道具图标走 (ITEM, itemid)。
		// RCA：原实现只有 SKILL/MACRO 两支，ITEM 落到 else → return false，
		// 于是背包里的药水拖到右下角快捷格时被状态栏无声拒绝（用户 bug 1）。
		// 判定改用「实质 id getter」而非 get_type()：快捷栏自身拖出的 QsIconType
		// 恒为 KEY 型，但同样携带 item/skill/macro id，须能被这里识别以完成换位。
		KeyType::Id btype = KeyType::Id::NONE;
		int32_t baction = 0;

		if (icon.get_skill_id() > 0)
		{
			btype = KeyType::Id::SKILL;
			baction = icon.get_skill_id();
		}
		else if (icon.get_macro_index() >= 0)
		{
			btype = KeyType::Id::MACRO;
			baction = icon.get_macro_index();
		}
		else if (icon.get_item_id() > 0)
		{
			btype = KeyType::Id::ITEM;
			baction = icon.get_item_id();
		}
		else
		{
			// 原生「自带功能键」(MENU/ACTION/FACE/CASH) 无 skill/item/macro id，
			// 靠 QsIconType::get_binding 透出的 (KeyType,KeyAction) 识别落点。
			// 这条通道只对快捷栏自身拖出的 QsIconType 生效；装备/背包等其它
			// Icon::Type 用默认 get_binding=false，穿戴/卸下路径不受影响。
			uint8_t btype_raw = 0;
			if (!icon.get_binding(btype_raw, baction)
				|| KeyType::typebyid(btype_raw) == KeyType::Id::NONE)
				return false;
			btype = static_cast<KeyType::Id>(btype_raw);
		}

		const Keyboard& keyboard = UI::get().get_keyboard();
		uint8_t maple_key = classic_mode
			? ((classic_qs_count == 8 && slot < 8)
				? static_cast<uint8_t>(keyboard.get_quickslot_key(static_cast<uint8_t>(slot)))
				: CLASSIC_QS_KEYS[slot])
			: MODERN_QS_KEYS[static_cast<size_t>(slot)];

		// 同一物品/技能拖到多个快捷格时不应出现多份：原位置若有相同绑定则先清空
		//（语义＝移动，与 UIKeyConfig::stage_mapping 的去重一致）。否则旧格仍显示同一图标。
		Keyboard& kb = UI::get().get_keyboard();
		// 原生「移动」语义：去重扫描必须按当前模式的键位表走。
		// 旧实现 modern 也扫 CLASSIC_QS_KEYS/classic_qs_count → 功能键/技能/道具
		// 从 modern 格拖走时旧格清不掉（旧格仍显示同一图标）。
		const size_t scan_count = classic_mode
			? std::min(classic_qs_count, CLASSIC_QS_SLOTS)
			: std::min(modern_qs_count, MODERN_QS_SLOTS);
		std::vector<std::tuple<KeyConfig::Key, KeyType::Id, int32_t>> clears;
		for (size_t i = 0; i < scan_count; ++i)
		{
			if (i == slot)
				continue;
			uint8_t other_key;
			if (classic_mode)
				other_key = (classic_qs_count == 8 && i < 8)
					? static_cast<uint8_t>(keyboard.get_quickslot_key(static_cast<uint8_t>(i)))
					: CLASSIC_QS_KEYS[i];
			else
				other_key = MODERN_QS_KEYS[i];
			if (other_key == maple_key)
				continue;
			Keyboard::Mapping om = keyboard.get_maple_mapping(other_key);
			if (om.type == btype && om.action == baction)
				clears.emplace_back(static_cast<KeyConfig::Key>(other_key), KeyType::Id::NONE, KeyAction::Id::LENGTH);
		}

		// 本地立即生效 + 上行 CHANGE_KEYMAP(135) 持久化 + 快捷栏图标刷新。
		for (const auto& c : clears)
			kb.remove(static_cast<uint8_t>(std::get<0>(c)));
		kb.assign(maple_key, btype, baction);
		std::vector<std::tuple<KeyConfig::Key, KeyType::Id, int32_t>> updated = clears;
		updated.emplace_back(static_cast<KeyConfig::Key>(maple_key), btype, baction);
		ChangeKeyMapPacket(updated).dispatch();
		redraw_quickslots();
		Sound(Sound::Name::BUTTONCLICK).play();

		// 2026-11-22：落点结算后必须清拖拽瞬态。旧实现只在这里 return true，
		// 起拖时置位的 qs_dragging/qs_drag_slot 从不复位 → 以后每次点快捷格
		// 都被 :2964/:2969 的 `if (qs_dragging) return CANCLICK` 吞掉，
		// 单击使用彻底失效（药水拖上去也用不了的根因之一）。
		qs_dragging = false;
		qs_drag_slot = -1;
		qs_pressed = false;

		return true;
	}

	Cursor::State UIStatusBar::send_cursor(bool clicked, Point<int16_t> cursor_position)
	{
		// 2026-11-20 【拖拽根因修复】`clicked` 语义 = 「左键当前是否按住」，不是「刚按下」。
		// 旧代码把每次 clicked==true 都当成新按下 → 拖动经过每个格都把 qs_drag_slot
		// 重设成当前格、并 return CLICKING，起拖分支（qs_dragging）永远进不去。
		// 真机 trace 铁证：at=(821)slot=0 drag_slot=0 → at=(854)slot=1 **drag_slot=1**
		// （起点被覆盖）→ dragging 恒 0 → 拖放从未生效。
		// 修法：用 qs_pressed 判「按下边沿」，按住期间只做拖动检测，松开时结算。
		if (quickslot_active)
		{
			const int16_t slot = classic_mode
				? classic_qs_slot_at(cursor_position)
				: modern_qs_slot_at(cursor_position);

			// ---- ① 起拖检测放在最前：不依赖 clicked 真假 ----
			// 真机 trace 铁证：鼠标仍按住时 clicked 会由 1 变 0（框架只在 down 帧传 1），
			// 若把 clicked==false 当"松开"就会在移动帧清状态 → 起拖永不触发。
			// 故：只要已记录起点且位移超阈值，无论 clicked 真假都起拖。
			if (qs_drag_slot >= 0 && !qs_dragging)
			{
				const Point<int16_t> delta = cursor_position - qs_press_pos;
				const int16_t adx = delta.x() < 0 ? static_cast<int16_t>(-delta.x()) : delta.x();
				const int16_t ady = delta.y() < 0 ? static_cast<int16_t>(-delta.y()) : delta.y();
				if (adx > QS_DRAG_THRESHOLD || ady > QS_DRAG_THRESHOLD)
				{
					setup_qs_drag_icon(static_cast<size_t>(qs_drag_slot), cursor_position);
					qs_dragging = true;
				}
			}

			if (clicked)
			{
				// ---- ② down/按住帧：**同格内绝不刷新起点** ----
				// 若每帧都重置 qs_press_pos，delta 永远等于「相邻两帧间距」(~5px)，
				// 永远超不过阈值 → 起拖永不触发（真机 trace 已证实）。
				if (!qs_dragging)
				{
					// 2026-11-20 回归修复：只有落在快捷格内的按下才进入拖/用判定并吞掉
					// 本次点击。旧代码在此处**无条件** return CLICKING，导致点在
					// 商城/帮助/目录/界面等主功能按钮（slot<0）时永远到不了
					// UIElement::send_cursor，于是「上面一排图标 + 下面几个大按钮
					// 点击全部失效」。恢复 HEAD 口径：非快捷格交回基类派发。
					if (slot >= 0)
					{
						if (slot != qs_drag_slot)
						{
							qs_drag_slot = static_cast<int16_t>(slot);
							qs_press_pos = cursor_position;
						}
						return Cursor::State::CLICKING;
					}
					// 非快捷格（主功能按钮 / 空白区）：交回基类派发，按钮正常响应。
					return UIElement::send_cursor(clicked, cursor_position);
				}
				return Cursor::State::CANCLICK; // 拖拽中
			}

			// ---- ③ up/移动帧 ----
			// 已起拖：等 UIStateGame::drop_icon → 目标格 send_icon 结算，这里只保持状态。
			if (qs_dragging)
				return Cursor::State::CANCLICK;

			// 未起拖且点击落在原格 → 单击使用；否则取消。
			if (qs_drag_slot >= 0)
			{
				if (slot == qs_drag_slot)
				{
					if (classic_mode)
					{
						activate_classic_qs_slot(static_cast<size_t>(qs_drag_slot));
						release_classic_qs_slot(static_cast<size_t>(qs_drag_slot));
					}
					else
					{
						activate_modern_qs_slot(static_cast<size_t>(qs_drag_slot));
						release_modern_qs_slot(static_cast<size_t>(qs_drag_slot));
					}
				}
				qs_drag_slot = -1;
				qs_pressed = false;
			}

			if (slot >= 0)
			{
				// 2026-11-23：快捷格 hover 弹 tooltip（原生 CUIStatusBar::ProcessToolTip）。
				// 只在「未拖拽 + 鼠标停留在该格」时挂；每帧 UIStateGame 会先 clear，
				// 故这里持续重挂即可，移开后（slot<0）本帧不挂即自然消失。
				show_qs_tooltip(slot);
				return Cursor::State::CANCLICK;
			}
		}
		else if (qs_drag_slot >= 0 || qs_pressed)
		{
			qs_drag_slot = -1;
			qs_dragging = false;
			qs_pressed = false;
		}

		return UIElement::send_cursor(clicked, cursor_position);
	}

	void UIStatusBar::draw_classic(float alpha) const
	{
		// 2026-11-21 用户要求：StatusBar.img/base/backgrnd 与底部信息栏整组一起居中。
		// 坐标基准：position.x 就是「内容组左缘」（= HudLayout::hud_center_x(VWIDTH,
		// content_w) = (VWIDTH-content_w)/2，见 init_classic/update_screen :308/:2366），
		// 血量/名牌/QS/数值全部以 position 为原点绘制，整组左缘=position.x、宽=content_w
		// 恰好居中于视口。因此背景板必须覆盖同一矩形：左缘=position.x，宽=content_w。
		// 历史 bug：曾把 classic_content_cx（同是左缘）误当视口中心再减 bg_w/2，
		// 1920 档算出 -306，把背景推到屏幕左外、右侧露空。
		if (classic_backgrnd)
		{
			const int16_t bg_w = classic_content_w > 0 ? classic_content_w : VWIDTH;
			const int16_t bg_x = position.x();	// 与信息栏整组同源左缘 → 一起居中
			classic_backgrnd.draw(DrawArgument(
				Point<int16_t>(bg_x, position.y()), Point<int16_t>(bg_w, 0)));
		}

		if (classic_backgrnd2)
			classic_backgrnd2.draw(position);

		// 量规三段（gauge/bar 与 gauge/graduation 同为 340x31 三联图集，HP|MP|EXP 同行 1:1）。
		// 图集自身给出的绘制模型（逐行比对参考截图，见 evidence/w1w3-hud/coords.md §1.4）：
		//   bar        = 上带 y2..10 烘焙「HP/MP/EXP」标签 + 下带 y15..28 彩色填充
		//   gray 1x16  = 未填充段的底色 (204,204,204)
		//   graduation = 白边框 + 框内半透明黑渐变（给填充上阴影）→ 必须画在最上层
		// 旧实现按百分比裁 bar 整段，于是 0% 的 EXP 连标签一起消失，且填充盖住了 graduation 的阴影。
		// 水平 Range(first, second) 会同时把屏幕矩形右移 first，故三段均传 hpmp_pos 原点。
		{
			constexpr int16_t atlas_w = 340;
			constexpr int16_t fill_top = 15;	// bar 彩色带首行（实测红块 y15..28）
			constexpr int16_t fill_h = 14;

			struct Gauge_seg
			{
				int16_t x;		// 分段在图集内的起点
				int16_t w;		// 分段宽度
				int16_t fill_x;	// 彩色填充相对分段起点的偏移
				int16_t fill_w;	// 彩色填充宽度
			};

			// 填充带实测：红 x2..106 / 蓝 x110..214 / 绿 x223..337。
			const Gauge_seg segs[3] = {
				{ SEG_HP, SEG_W_HP, 2, 105 },
				{ SEG_MP, SEG_W_MP, 1, 105 },
				{ SEG_EXP, SEG_W_EXP, 1, 115 }
			};
			const float pct[3] = { hpbar.get_percentage(), mpbar.get_percentage(), expbar.get_percentage() };

			for (int i = 0; i < 3; ++i)
			{
				const Gauge_seg& s = segs[i];
				Range<int16_t> crop(s.x, static_cast<int16_t>(atlas_w - (s.x + s.w)));
				DrawArgument at(position + hpmp_pos);

				if (classic_gauge_bar)
					classic_gauge_bar.draw(at, {}, crop);

				int16_t filled = static_cast<int16_t>(pct[i] * s.fill_w + 0.5f);

				filled = std::clamp<int16_t>(filled, 0, s.fill_w);

				if (classic_gauge_gray && filled < s.fill_w)
				{
					classic_gauge_gray.draw(
						DrawArgument(
							position + hpmp_pos + Point<int16_t>(static_cast<int16_t>(s.x + s.fill_x + filled), fill_top),
							Point<int16_t>(static_cast<int16_t>(s.fill_w - filled), fill_h)
						)
					);
				}

				if (classic_graduation)
					classic_graduation.draw(at, {}, crop);
			}
		}

		// 低血/低蓝时按 WZ hpFlash/mpFlash 周期播放（5帧×delay，循环由 update 驱动）。
		{
			const uint8_t hp_warn_draw = Setting<SysOptHpWarn>::get().load();
			const uint8_t mp_warn_draw = Setting<SysOptMpWarn>::get().load();
			const float hp_pct_draw = gethppercent();
			const float mp_pct_draw = getmppercent();
			const bool hp_low_draw = hp_warn_draw > 0
				&& hp_pct_draw > 0.0f
				&& hp_pct_draw <= static_cast<float>(hp_warn_draw) / 100.0f;
			const bool mp_low_draw = mp_warn_draw > 0
				&& mp_pct_draw > 0.0f
				&& mp_pct_draw <= static_cast<float>(mp_warn_draw) / 100.0f;

			// classic_*_flash：-1=无素材；>=0=有素材。低量期间每帧绘制（update 驱动循环）。
			if (hp_low_draw && classic_hp_flash >= 0)
				classic_hp_flash_sprite.draw(position, alpha);

			if (mp_low_draw && classic_mp_flash >= 0)
				classic_mp_flash_sprite.draw(position, alpha);
		}

		int32_t level = stats.get_stat(MapleStat::Id::LEVEL);
		int32_t hp = stats.get_stat(MapleStat::Id::HP);
		int32_t mp = stats.get_stat(MapleStat::Id::MP);
		int32_t maxhp = stats.get_total(EquipStat::Id::HP);
		int32_t maxmp = stats.get_total(EquipStat::Id::MP);
		int64_t exp = stats.get_exp();

		// 等级：Basic.img/LevelNo 橙盒数字（11x13，推进 11+1），以参考实测单字中心
		// x = lvbadge_pos.x() + 2 居中对齐（基准 +5→+2，使 3000 级 4 位数字整体左移、
		// 右侧留出安全间距）；LevelNo 缺失时回退 ShowLevel 白字 + 右括号。
		const std::string lvstr = std::to_string(level);
		const int16_t lvw = static_cast<int16_t>(static_cast<int16_t>(lvstr.length()) * 12 - 1);
		const Point<int16_t> lv_at = position
			+ Point<int16_t>(static_cast<int16_t>(lvbadge_pos.x() + 2 - lvw / 2), lvbadge_pos.y());

		if (classic_levelno.getw('0') > 0)
		{
			classic_levelno.draw(lvstr, DrawArgument(lv_at));
		}
		else if (classic_showlevel.getw('0') > 0)
		{
			const int16_t slw = static_cast<int16_t>(static_cast<int16_t>(lvstr.length()) * 6 - 1);
			Point<int16_t> sl_at = position + Point<int16_t>(
				static_cast<int16_t>(lvbadge_pos.x() + 2 - slw / 2), lvbadge_pos.y() + 1);

			classic_showlevel.draw(lvstr, DrawArgument(sl_at));

			if (classic_showlevel_bracket)
				classic_showlevel_bracket.draw(
					DrawArgument(sl_at + Point<int16_t>(static_cast<int16_t>(slw + 1), 0)));
		}

		// 芯片内两行：职业名（上）+ 角色名（下）。*_pos.y() 是参考实测的墨迹顶，
		// 绘制点按各自当前文字反解 —— 中文名含「新/等级」这类 bt=10 的字，
		// 纯拉丁名只有 bt=9，同一档常量必然错 1px，只有按串精算才对（R27-B1b）。
		joblabel.change_text(stats.get_jobname());
		joblabel.draw(position + Point<int16_t>(joblabel_pos.x(), joblabel.y_for_ink_top(joblabel_pos.y())));
		namelabel.draw(position + Point<int16_t>(namelabel_pos.x(), namelabel.y_for_ink_top(namelabel_pos.y())));

		// 量规数值改画 StatusBar.img/number 位图字库（W1/RC6）：
		// 5x7 白数字 + 2x9 黄绿括号 + 7x7 slash/percent，不再走 Text(A11B, YELLOW)。
		// 参考截图逐字对拍：HP/MP = "[hp/maxhp]"，EXP = "[xx.xx%]"（两位小数，带括号）。
		// 标签 HP/MP/EXP 由 gauge/graduation 图集烘焙，不叠加 Text（否则重影）。
		draw_hud_number(hpmpset, classic_exp_dot,
			"[" + std::to_string(hp) + "/" + std::to_string(maxhp) + "]",
			DrawArgument(position + hpset_pos));

		draw_hud_number(hpmpset, classic_exp_dot,
			"[" + std::to_string(mp) + "/" + std::to_string(maxmp) + "]",
			DrawArgument(position + mpset_pos));

		// EXP 段：仅显示「当前经验值」（位图字库原始完整数字，与 HP/MP 同款，不缩写），
		// 不显示升级所需；升级所需仅用于下方百分比计算。百分比 "[xx.xx%]" 仍叠画在
		// 黄绿经验条填充区中央（见下方 draw_hud_number）。
		int64_t need = ExpTable::required_exp(level);
		double ratio = (need > 0) ? (static_cast<double>(exp) / static_cast<double>(need)) : 0.0;
		float pct = (ratio >= 1.0) ? 100.0f : static_cast<float>(ratio * 100.0);

		// EXP 数字行：位图字库原始完整数字（不缩写，与 HP/MP 同款），与血量/蓝量数字行
		// 同高（y=statset_pos.y()=41；仅当前经验值，不显示升级所需）。
		draw_hud_number(hpmpset, classic_exp_dot, std::to_string(exp), DrawArgument(position + statset_pos));

		char pbuf[16];
		std::snprintf(pbuf, sizeof(pbuf), "[%.2f%%]", pct);
		std::string pctstr(pbuf);

		int16_t pw = 0;
		for (char c : pctstr)
			pw += hud_advance(hpmpset, c);

		constexpr int16_t EXP_FILL_X = 1;
		constexpr int16_t EXP_FILL_W = 114;
		constexpr int16_t EXP_FILL_Y = 53;
		constexpr int16_t EXP_FILL_H = 14;
		constexpr int16_t GLYPH_H = 7;
		// 百分比与经验框右缘对齐（留 2px 边距）
		Point<int16_t> pct_at(
			static_cast<int16_t>(GAUGE_X + SEG_EXP + EXP_FILL_X + EXP_FILL_W - pw - 2),
			static_cast<int16_t>(EXP_FILL_Y + (EXP_FILL_H - GLYPH_H) / 2));
		draw_hud_number(hpmpset, classic_exp_pct_dot, pctstr, DrawArgument(position + pct_at));

		if (quickslot_active && classic_quickslot)
		{
			classic_quickslot.draw(position + quickslot_pos);

			// 槽格几何来自图集本身（见文件顶部 QS_* 推导），三种面板通用：
			// 151 = 9+4*28+3*7+9 / 466 = 9+13*28+12*7+9 / 571 = 9+16*28+15*7+9。
			const int16_t cols = classic_qs_cols;
			const size_t count = std::min(classic_qs_count, CLASSIC_QS_SLOTS);

			for (size_t i = 0; i < count; i++)
			{
				const Point<int16_t> cell_pos = classic_qs_cell_at(i, cols);

				if (classic_qs_icons[i])
				{
					// 图标锚点按每张贴图自身 origin 自适应：Texture::draw → get_rectangle
					// 会减去 origin，要让绘制顶落在格顶，绘制点必须 = cell_top + origin.y。
					// KeyConfig/icon（MENU/ACTION/FACE）origin.y=32（底边锚点，orange-wz 实测）；
					// SkillData 构造里已 icon.shift(get_origin()) 把 origin 清零、ItemData 图标同理。
					// 旧实现统一 +QS_ICON_SHIFT_Y(32) 只适用于前者，把技能/道具图标整体下移
					// 32px 画到格外面板之外 —— 真机表现为「绑定了枪连击的 Z 格只剩键帽数字、
					// 看不到技能图标」。按自身 origin 自适应后，26/32 宽格与 8 槽窄格一并修正。
					const int16_t icon_dy = classic_qs_icons[i].get_origin().y();
					classic_qs_icons[i].draw(cell_pos + Point<int16_t>(0, icon_dy));

					// 快捷格技能冷却遮罩（四源深核 D2，2026-11-24）：
					// 旧实现整格二元黑（只判 remaining>0）；MinMS UIStatusBar.cpp:440-459
					// 按 remain/total 精确比例从顶部裁切黑罩。total 来自 s9 234 包下发的
					// cooltime 秒（PlayerHandlers.cpp AddCooldownHandler），Player 同存总时长。
					// 不解析 WZ cooltime，避免与服务端权威值双源冲突。
					{
						const Keyboard& kb = UI::get().get_keyboard();
						const uint8_t mk = (classic_qs_count == 8 && i < 8)
							? static_cast<uint8_t>(kb.get_quickslot_key(static_cast<uint8_t>(i)))
							: CLASSIC_QS_KEYS[i];
						const Keyboard::Mapping cm = kb.get_maple_mapping(mk);
						if (cm.type == KeyType::Id::SKILL)
						{
							const Player& player = Stage::get().get_player();
							const int32_t remain = player.get_cooldown_remaining(cm.action);
							const int32_t total = player.get_cooldown_total(cm.action);
							if (remain > 0 && total > 0)
							{
								// remain/total 越大，剩余越多 → 遮罩越高；从格顶向下裁切。
								const float frac = std::min(1.0f,
									static_cast<float>(remain) / static_cast<float>(total));
								const int16_t h = static_cast<int16_t>(30 * frac);
								if (h > 0)
								{
									ColorBox(30, h, Color::Name::BLACK, 0.6f).draw(
										DrawArgument(cell_pos + Point<int16_t>(0, 30 - h)));
								}
							}
						}
					}
				}

				// 键帽层：原生切图 quickslotConfig/key/<mapleKey> 画在图标之上（占格顶带）。
				// 切图 7~28x11、origin=(0,0)，与旧 StatusBar.img/key 灰贴纸同尺寸族；
				// 用户只反馈键帽「样式」不符、未反馈位置偏移，故保持旧贴纸的 cell_pos
				// 零偏移同位替换，不擅自挪动落点。8 槽跟 quickslots，宽格用固定表。
				// 键帽优先走原生切图：26 格固定表与 8 槽动态键位统一按 maple key 取图，
				// RIGHT_* 归一到左键节点（RIGHT_SHIFT=86→42 / RIGHT_CONTROL=85→29 / RIGHT_ALT=87→56）。
				uint8_t mk = (classic_qs_count == 8 && i < 8)
					? static_cast<uint8_t>(UI::get().get_keyboard().get_quickslot_key(static_cast<uint8_t>(i)))
					: CLASSIC_QS_KEYS[i];
				int16_t cap_id = static_cast<int16_t>(mk);
				switch (mk)
				{
				case KeyConfig::Key::RIGHT_CONTROL: cap_id = KeyConfig::Key::LEFT_CONTROL; break;
				case KeyConfig::Key::RIGHT_SHIFT: cap_id = KeyConfig::Key::LEFT_SHIFT; break;
				case KeyConfig::Key::RIGHT_ALT: cap_id = KeyConfig::Key::LEFT_ALT; break;
				default: break;
				}

				int8_t cap = -1;
				std::string keyname;
				if (classic_qs_count == 8 && i < 8)
				{
					switch (mk)
					{
						case KeyConfig::Key::LEFT_SHIFT: case KeyConfig::Key::RIGHT_SHIFT: cap = 0; break;
						case KeyConfig::Key::INSERT: cap = 1; break;
						case KeyConfig::Key::HOME: cap = 2; break;
						case KeyConfig::Key::PAGE_UP: cap = 3; break;
						case KeyConfig::Key::LEFT_CONTROL: case KeyConfig::Key::RIGHT_CONTROL: cap = 4; break;
						case KeyConfig::Key::DELETE: cap = 5; break;
						case KeyConfig::Key::END: cap = 6; break;
						case KeyConfig::Key::PAGE_DOWN: keyname = "Pdn"; break;
						case KeyConfig::Key::NUM1: keyname = "1"; break;
						case KeyConfig::Key::NUM2: keyname = "2"; break;
						case KeyConfig::Key::NUM3: keyname = "3"; break;
						case KeyConfig::Key::NUM4: keyname = "4"; break;
						case KeyConfig::Key::NUM5: keyname = "5"; break;
						case KeyConfig::Key::Q: keyname = "Q"; break;
						case KeyConfig::Key::W: keyname = "W"; break;
						case KeyConfig::Key::E: keyname = "E"; break;
						case KeyConfig::Key::R: keyname = "R"; break;
						case KeyConfig::Key::T: keyname = "T"; break;
						case KeyConfig::Key::A: keyname = "A"; break;
						case KeyConfig::Key::S: keyname = "S"; break;
						case KeyConfig::Key::D: keyname = "D"; break;
						case KeyConfig::Key::F: keyname = "F"; break;
						case KeyConfig::Key::Z: keyname = "Z"; break;
						case KeyConfig::Key::X: keyname = "X"; break;
						case KeyConfig::Key::C: keyname = "C"; break;
						case KeyConfig::Key::V: keyname = "V"; break;
						default: break;
					}
				}
				else
				{
					cap = CLASSIC_QS_KEYCAP[i];
					keyname = CLASSIC_QS_KEYNAME[i];
				}

				// 原生切图优先：淡蓝斜体白框（正确端图2 同款）。
				if (cap_id >= 0 && cap_id < static_cast<int16_t>(classic_qs_cap.size())
					&& classic_qs_cap[cap_id])
				{
					classic_qs_cap[cap_id].draw(cell_pos);
				}
				else if (cap >= 0 && classic_key_label[cap])
				{
					// 缺切图回退：StatusBar.img/key/0..6 灰贴纸。
					classic_key_label[cap].draw(cell_pos);
				}
				else if (!keyname.empty())
				{
					// 最后回退：描边文字（保持旧行为，避免缺素材包时空白）。
					classic_qs_keytext.change_text(keyname);
					classic_qs_keytext.draw(cell_pos + Point<int16_t>(QS_CAP_X, QS_CAP_Y));
				}

				// 绑定中文名：Text(A11M, maxwidth=28) —— 复用 T12b 的 CJK 字宽度量做
				// 断行，不在本批重写换行；落在格下半带。（道具不再走这里，改走下方数量。）
				if (!classic_qs_names[i].empty())
				{
					classic_qs_label.change_text(classic_qs_names[i]);
					classic_qs_label.draw(DrawArgument(cell_pos + Point<int16_t>(0, QS_LABEL_Y)));
				}

				// 道具数量：原生快捷格在格右下叠数量（与背包 Icon::draw 的 Basic.img/ItemNo 同源）。
				// 用 ItemNo 字模而非中文 Text：数字位宽固定、不会因 maxwidth=28 断行而错位（用户 bug 2）。
				if (!classic_qs_counts[i].empty())
				{
					const std::string& qty = classic_qs_counts[i];
					if (classic_qs_itemno_ok)
					{
						int16_t qw = 0;
						for (char ch : qty)
							qw += classic_qs_itemno.getw(static_cast<int8_t>(ch));
						classic_qs_itemno.draw(qty, DrawArgument(
							cell_pos + Point<int16_t>(static_cast<int16_t>(QS_SLOT - qw - 1),
								static_cast<int16_t>(QS_SLOT - 12))));
					}
					else
					{
						// 缺 ItemNo 字模回退：仍用 Text 画数字，落点同为格右下。
						classic_qs_label.change_text(qty);
						classic_qs_label.draw(DrawArgument(
							cell_pos + Point<int16_t>(QS_SLOT - 16, QS_LABEL_Y)));
					}
				}
			}
		}

		// 按钮簇第 3 颗「双人物+星」= StatusBar.img/base/iconMemo (14x11)，纯 bitmap
		// 无 normal/pressed/... 四态子节点，不能走 MapleButton，只能按 Texture 直画。
		// 参考全幅图 1x 模板匹配 SAD=0.0 @x602 y82；BtClaim x576 → 相对 +26，
		// 比按钮行 (y78) 低 4px ⇒ 落 (group_x+26, 12)。本端旧代码从未绘制该节点。
		if (classic_icon_memo)
		{
			const int16_t memo_group_x = static_cast<int16_t>(
				quickslot_pos.x() - HudLayout::qs_group_gap(VWIDTH));

			classic_icon_memo.draw(position + Point<int16_t>(
				static_cast<int16_t>(memo_group_x + 26), 12));
		}

		UIElement::draw_buttons(alpha);

		// D9 · Angel 083 SetButtonBlink @0x8dfe21：4 钮之上叠 SP_1986..1989 真 KEY_ANI
		// 两帧动画（常态<->发光，delay 500ms=1Hz），仅在 blink_elapsed_ms[i]>=0 时显示。
		// 外部程序误判资源并自定义脉冲叠层，已按 .nx 实测改回真帧。
		static constexpr uint16_t blink_ids[4] = {
			Buttons::BT_CLASSIC_EQUIP, Buttons::BT_CLASSIC_INV,
			Buttons::BT_CLASSIC_STAT, Buttons::BT_CLASSIC_SKILL
		};
		for (int i = 0; i < 4; ++i)
		{
			if (blink_elapsed_ms[i] < 0)
				continue;

			Button* b = button_at(blink_ids[i]);
			if (!b)
				continue;

			Rectangle<int16_t> r = b->bounds(position);
			blink_ani[i].draw(DrawArgument(r.get_left_top()), alpha);
		}
	}

	void UIStatusBar::update_classic()
	{
		UIElement::update();

		// D9 · Angel 083 CUIStatusBar::Update @0x8de964：
		//   do { v8=*v7; if (v8 + 90000 < timeGetTime() && v8 != -1) SetButtonBlink(idx, 0); } while(idx<4);
		// 逻辑帧累计毫秒等价 90000ms 超时清除；同时推进真 KEY_ANI 两帧动画。
		for (int i = 0; i < 4; ++i)
		{
			if (blink_elapsed_ms[i] < 0)
				continue;

			blink_elapsed_ms[i] += Constants::TIMESTEP;
			blink_ani[i].update();

			if (blink_elapsed_ms[i] >= BLINK_TIMEOUT_MS)
				blink_elapsed_ms[i] = -1;
		}

		// 083 CWvsContext::OnStatChanged @0xa1fce9：低等级(<=0xA=10)升级 -> STAT 钮启动 blink
		//（紧邻 SP_5547_LEVELUP 音效）。UI 层自感知等级上升，不动 s9 协议 handler。
		int16_t blink_level = stats.get_stat(MapleStat::Id::LEVEL);
		if (blink_last_level > 0 && blink_level > blink_last_level
			&& blink_level <= BLINK_LEVEL_LIMIT)
			start_button_blink(Buttons::BT_CLASSIC_STAT);
		blink_last_level = blink_level;

		expbar.update(getexppercent());

		float hp = gethppercent();
		float mp = getmppercent();

		// B4-5「HP/MP 警报」消费端（R28-B4b 接线）：低于阈值百分比时周期性重放
		// gauge/hpFlash·mpFlash 覆盖层，0 = 不报警（原生档 0 就是不闪）。
		// 量纲：ui.sysopt.hp_warn/mp_warn 存 0..100 百分比；默认 52 ↔ 原生注册表
		// soHPFlash/soMPFlash = 10（0..20 档 × 5% = 50%），是证据允许的最贴近校准。
		// 不再「数值一变就闪」—— 那是旧占位行为，与「警报」设置项语义无关，
		// 且满血受伤也闪（用户裁决 R28-B4b 决策点：低血才闪）。
		// 素材缺失（-1）时完全不触发，与旧口径一致；classic_*_flash 自身倒计时
		// 驱动重放（5 帧闪 + 1 帧间隔），**不新增类数据成员** —— 公共头改布局会踩
		// 增量构建旧 obj 坑（memory:msbuild-header-layout-stale-objs）。
		// 注：覆盖层素材只有 classic 底版有，现代皮肤（is_classic_hud()==false）
		// 无 hpFlash/mpFlash 节点，本设置对其无观感 —— 已记入 evidence §4 预留。
		const uint8_t hp_warn = Setting<SysOptHpWarn>::get().load();
		const uint8_t mp_warn = Setting<SysOptMpWarn>::get().load();

		const bool hp_low = hp_warn > 0 && hp > 0.0f && hp <= static_cast<float>(hp_warn) / 100.0f;
		const bool mp_low = mp_warn > 0 && mp > 0.0f && mp <= static_cast<float>(mp_warn) / 100.0f;

		// 对齐 MinMS UIStatusBar：低血/低蓝期间持续 update WZ 闪光动画（5 帧周期循环），
		// 阈值内一直重播；不再用「下降穿越」只闪一次后永远静音。
		const bool hp_flash_active = hp_low;
		const bool mp_flash_active = mp_low;

		if (classic_hp_flash >= 0)
		{
			if (hp_flash_active)
			{
				classic_hp_flash_sprite.update();
				classic_hp_flash = 1;
			}
			else
			{
				classic_hp_flash = 0;
			}
		}

		if (classic_mp_flash >= 0)
		{
			if (mp_flash_active)
			{
				classic_mp_flash_sprite.update();
				classic_mp_flash = 1;
			}
			else
			{
				classic_mp_flash = 0;
			}
		}

		classic_last_hp = hp;
		classic_last_mp = mp;

		hpbar.update(hp);
		mpbar.update(mp);
		namelabel.change_text(stats.get_name());
		refresh_classic_qs_icons();
	}
}