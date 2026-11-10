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
#include "UIStatsInfo.h"
#include "UISoftKey.h"
#include "UISkillBook.h"
#include "UIMiscShells.h"

#include "UINotice.h"

#include "../UI.h"

#include "../Components/MapleButton.h"

#include "../../Gameplay/Stage.h"

#include "../../Net/Packets/PlayerPackets.h"
#include "../../Util/PerfLog.h"
#include "../../Util/UiAssetResolve.h"

#include "../../Character/ExpTable.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		nl::node basic_close_btn()
		{
			// 见 UiAssetResolve::close_btn() —— 本包 BtClose 是实心方块，
			// 原生标题栏用的是描边 X（BtClose2）。
			return UiAssetResolve::close_btn();
		}

		Button* find_btn(std::map<uint16_t, std::unique_ptr<Button>>& buttons, uint16_t id)
		{
			auto it = buttons.find(id);
			return (it != buttons.end()) ? it->second.get() : nullptr;
		}
	}

	UIStatsInfo::UIStatsInfo(const CharStats& st) : UIDragElement<PosSTATS>(Point<int16_t>(212, 150)), stats(st)
	{
		nl::node close = basic_close_btn();
		nl::node Stat = UiAssetResolve::window4("Stat");

		if (!UiAssetResolve::usable(Stat))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		nl::node main = Stat["main"];
		bool classic = !UiAssetResolve::usable(main);
		this->classic = classic;

		if (classic)
			main = Stat;

		nl::node detail = Stat["detail"];
		nl::node abilityTitle = detail["abilityTitle"];
		nl::node metierLine = detail["metierLine"];

		if (classic)
		{
			// v83 经典布局：主窗只绘 backgrnd（208x347，标签行烘焙在精灵里）。
			// backgrnd2（218x203）是「详细」属性板——仅详细模式绘于窗口右下侧；
			// basicStat（159x121）是窗内「基本属性点」蓝头信息板——始终绘于窗内
			// (8,195)，盖住 AP/属性区（原生即如此，参考截图 SAD=0.0）。
			// 全部坐标依据见 docs/features/ui-v83-window-restore/evidence/w6-stats/coords.md。
			if (Stat["backgrnd"])
				sprites.emplace_back(Stat["backgrnd"]);

			classic_detail = Texture(Stat["backgrnd2"]);
			classic_help = Texture(Stat["basicStat"]);

			disabled_labels[0] = Texture(Stat["Disabled"]["STR"]);
			disabled_labels[1] = Texture(Stat["Disabled"]["DEX"]);
			disabled_labels[2] = Texture(Stat["Disabled"]["INT"]);
			disabled_labels[3] = Texture(Stat["Disabled"]["LUK"]);
		}
		else
		{
			sprites.emplace_back(main["backgrnd"]);
			sprites.emplace_back(main["backgrnd2"]);
			sprites.emplace_back(main["backgrnd3"]);
		}

		// cover0/cover1 是现代 UI 件，本包 UIWindow.img/Stat 下不存在（nx 实测），
		// 原「Sprite(main["cover0"]) + 新手叠绘」是死代码，W6 起移除。

		if (detail)
		{
			textures_detail.emplace_back(detail["backgrnd"]);
			textures_detail.emplace_back(detail["backgrnd2"]);
			textures_detail.emplace_back(detail["backgrnd3"]);
			textures_detail.emplace_back(detail["backgrnd4"]);

			abilities[Ability::RARE] = abilityTitle["rare"]["0"];
			abilities[Ability::EPIC] = abilityTitle["epic"]["0"];
			abilities[Ability::UNIQUE] = abilityTitle["unique"]["0"];
			abilities[Ability::LEGENDARY] = abilityTitle["legendary"]["0"];
			abilities[Ability::NONE] = abilityTitle["normal"]["0"];

			inner_ability[true] = metierLine["activated"]["0"];
			inner_ability[false] = metierLine["disabled"]["0"];
		}

		try_maple_btn(buttons, Buttons::BT_CLOSE, close, Point<int16_t>(191, 5), "Stat/close");

		nl::node ap_up = main["BtApUp"] ? main["BtApUp"] : Stat["BtApUp"];
		nl::node hp_up = main["BtHpUp"] ? main["BtHpUp"] : ap_up;
		nl::node mp_up = main["BtMpUp"] ? main["BtMpUp"] : ap_up;
		nl::node str_up = main["BtStrUp"] ? main["BtStrUp"] : ap_up;
		nl::node dex_up = main["BtDexUp"] ? main["BtDexUp"] : ap_up;
		nl::node int_up = main["BtIntUp"] ? main["BtIntUp"] : ap_up;
		nl::node luk_up = main["BtLukUp"] ? main["BtLukUp"] : ap_up;

		// BtApUp 12x12：数值框右缘 x=200 留 2px → x=186；行带高 16 → 上下各嵌 2。
		// 行带 y 为 Stat/backgrnd 逐像素实测（coords.md 依据 1）。
		try_maple_btn(buttons, Buttons::BT_HP, hp_up, Point<int16_t>(186, 115), "Stat/BtApUp/HP");
		try_maple_btn(buttons, Buttons::BT_MP, mp_up, Point<int16_t>(186, 133), "Stat/BtApUp/MP");
		try_maple_btn(buttons, Buttons::BT_STR, str_up, Point<int16_t>(186, 245), "Stat/BtApUp/STR");
		try_maple_btn(buttons, Buttons::BT_DEX, dex_up, Point<int16_t>(186, 263), "Stat/BtApUp/DEX");
		try_maple_btn(buttons, Buttons::BT_INT, int_up, Point<int16_t>(186, 281), "Stat/BtApUp/INT");
		try_maple_btn(buttons, Buttons::BT_LUK, luk_up, Point<int16_t>(186, 299), "Stat/BtApUp/LUK");

		// BtAuto 落点依据（083/08v83 口径）：
		//  - Angel 083 `CUIStat::OnCreate` @ 0x8c4c7f 逐字只创建 2006/(124,324) 的 BtDetail，
		//    没有任何 BtAuto 的 CreateCtrl/AddButton 调用点（同函数内 SP_ 节点名仅 JOB /
		//    BTDETAIL / STAT_DISABLED_）。故 BtAuto 坐标不是 083 EA 直接给出的结论。
		//  - 现存落点由 `UI/UIWindow2.img/Stat/main/BtAuto` 的 canvas.origin 推：
		//    origin=(-94,-198) ⇒ 现代窗 topleft=(94,198)；ApUp 同行 y 与经典几乎同值
		//    （现代 STR y=244 / 经典 245），故 y 直取 198。本包 BtAuto 为 100x35
		//    （现代 67x34），按窗宽 208 右缘 2px 钳 x=208-100-2=106（若按 ApUp
		//    Δx=+39 映射得 133 会越界）。
		//  → 该坐标属「节点 origin + 截图实测」推断，漂移风险记在 drop/core 面 backlog；
		//    新手态仍由下面 BEGINNER 分支熄灭；有转职后与 BtApUp 同步 set_active(show_ap)。
		nl::node bt_auto = main["BtAuto"] ? main["BtAuto"] : (Stat["BtAuto"] ? Stat["BtAuto"] : Stat["BtAutoAP"]);
		try_maple_btn(buttons, Buttons::BT_AUTO, bt_auto, Point<int16_t>(106, 198), "Stat/BtAuto");

		// BtDetail 46x19 右下角锚算：x=208-46-9、y=347-19-4（k 截图实测 SAD=0.25）。
		nl::node detail_open = main["BtDetailOpen"] ? main["BtDetailOpen"] : (Stat["BtDetail"] ? Stat["BtDetail"] : main["BtDetail"]);
		try_maple_btn(buttons, Buttons::BT_DETAILOPEN, detail_open, Point<int16_t>(153, 324), "Stat/BtDetail");

		// PLAN §6-Q2 定案：包内无 BtDetailClose/BtApDown；参考截图中详情板右下角
		// 收起件与 Basic.img/BtHide（13x12 黄绿底白色左箭头）像素全等（SAD=0.0），
		// 语义（hide）也最贴近 → 近似复刻接此件。窗口相对 = 面板(208,144)+(197,182)。
		try_maple_btn(buttons, Buttons::BT_DETAILCLOSE, nl::nx::UI["Basic.img"]["BtHide"], Point<int16_t>(405, 326), "Stat/BtHide");

		// BtHyperStatOpen/Close、detail/BtAbility、detail/BtHpUp 均为本包不存在的
		// 现代 UI 节点（nx_inspect 全量核对），原调用点连同旧语义坐标一并删除。

		if (auto* b = find_btn(buttons, Buttons::BT_DETAILCLOSE))
			b->set_active(false);

		// No modern detail panel on v83 — but classic keeps its own 详细板
		// （Stat/backgrnd2 218x203，绘于窗相对 (208,144)，行带实测见 coords.md），
		// 存在即可切换详细模式。
		if (!detail)
		{
			bool classic_detail_ok = classic && classic_detail.is_valid();

			if (auto* b = find_btn(buttons, Buttons::BT_DETAILOPEN))
				b->set_active(classic_detail_ok);
		}

		jobId = stats.get_stat(MapleStat::Id::JOB);

		if (jobId == Job::Level::BEGINNER)
		{
			if (auto* b = find_btn(buttons, Buttons::BT_HP)) b->set_active(false);
			if (auto* b = find_btn(buttons, Buttons::BT_MP)) b->set_active(false);
			if (auto* b = find_btn(buttons, Buttons::BT_STR)) b->set_active(false);
			if (auto* b = find_btn(buttons, Buttons::BT_DEX)) b->set_active(false);
			if (auto* b = find_btn(buttons, Buttons::BT_INT)) b->set_active(false);
			if (auto* b = find_btn(buttons, Buttons::BT_LUK)) b->set_active(false);
			if (auto* b = find_btn(buttons, Buttons::BT_AUTO)) b->set_active(false);
		}

		update_ap();

		// Normal（RC6：v83 参考截图数值为纯黑 (0,0,0)，EMPEROR=87 灰是"颜色不对"主因）
		const Color::Name value_color = classic ? Color::Name::BLACK : Color::Name::EMPEROR;

		for (size_t i = StatLabel::NAME; i <= LUK; i++)
			statlabels[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, value_color);

		statlabels[StatLabel::AP] = Text(Text::Font::A11M, Text::Alignment::RIGHT, value_color);

		for (size_t i = 0; i < StatLabel::NUM_LABELS; i++)
			statinktops[i] = INK_TOP_NONE;

		Point<int16_t> statoffset = Point<int16_t>(73, 29);
		int16_t statoffset_y = 18;

		if (classic)
		{
			// 配准：Stat/backgrnd 的 origin 实测 (0,0)（nx_inspect UI.nx
			// UIWindow.img/Stat），整块对 9f1b69f5 做 SAD ⇒ (0,0) 处 1.13/像素、
			// 次优 (0,1) 3.14 ⇒ **截图坐标 == 窗口坐标**，不需要再加标题行偏移。
			//
			// 落位：下表只记「参考截图上逐行量到的墨迹顶 y」，绘制点在 draw 里交给
			// 该行 Text 自己的度量反解（R27-B1b 唯一真源）。墨迹顶 = 基线 - 本行最高
			// 字形的 bitmap_top，而 SimHei 11px 的 CJK 分 9/10 两档（170 字抽样：
			// 126 字 9、42 字 10）——「新手」的顶是「新」= 10，「50 / 50」的顶是
			// '/' = 9。按「行类别」挑常量必然错 1px，只有按串精算才对。
			// （上面那组 9/10 是 SimHei 的读数，R28-B5 换成思源宋体后数值变了，
			//  但结论不变：正因为逐字不齐，才不能按行类别抄常量。）
			//
			// r27 更正 r15：r15 手抄的 bitmap_top「数字 8、'/' 11、CJK 11」里后两个
			// 是 PIL 口径，FreeType 的 glyph.bitmap_top 实测 9~10 ⇒ 那几行一直画低
			// 2px，正是用户看到的「基础信息数值偏下、不在框内」。
			struct ClassicRow
			{
				StatLabel label;
				int16_t x;
				int16_t ink_top;	// 量到的墨迹顶；INK_TOP_NONE = 该行无墨可解
				int16_t raw_y;		// 仅 INK_TOP_NONE 行使用（r15 遗留绘制点）
			};

			static const ClassicRow classic_ink_rows[] {
				{ StatLabel::NAME,  61,  36, 0 },	// 「Admin」
				{ StatLabel::JOB,   61,  53, 0 },	// 「新手」
				{ StatLabel::LEVEL, 61,  82, 0 },	// 「1」
				{ StatLabel::HP,    61, 118, 0 },	// 「50 / 50」
				{ StatLabel::MP,    61, 136, 0 },	// 「5 / 5」
				{ StatLabel::EXP,   61, 154, 0 },	// 「0 (0%)」
				{ StatLabel::FAME,  61, 172, 0 },	// 「0」
				{ StatLabel::GUILD, 61, 104, 0 },	// 「-」：整行只有 3 个墨点（rel x61..63 y104）
			};
			// 上表 7 行 = `layout.py mark Stat.json 角色能力.png` 的 value_ink_tops /
			// value_col_bbox 两个探针直接吐出来的数（锚点 = Stat/backgrnd 左上）：
			// 墨迹顶 36/53/82/118/136/154/172、值列左缘 x=61。r27 复核时同一套探针
			// 在客户帧上量到 41/59/123/141/159/176 与 x=67 ⇒ r15 那批「40/58/87/…/176、
			// x=67」不是原生值，而是**照着我们自己的渲染结果抄的**（一致偏低 4~6px、
			// 偏右 6px），这正是用户报「数值偏下、不在框内」还没修掉的原因。
			// 同一帧的 chip_grid（粉色标签块行网格，烘焙位图）两端 Δ=0 ⇒ 底板对齐，
			// 偏差全部在文字侧，可以安全地按原生值重定。
			// AP/力量/敏捷/智力/运气在两张参考截图里都被「基本属性点」浮层盖住
			// （浮层占 x8..170 y172..330）⇒ 无墨可解，保持 r15 的绘制点不猜，
			// 只跟着值列左缘把 x 一起改到 61；y 等 B8 关掉浮层后真机抓帧再量。
			static const ClassicRow classic_raw_rows[] {
				{ StatLabel::AP,  84, INK_TOP_NONE, 212 },	// 右对齐到数值框右缘，另算
				{ StatLabel::STR, 61, INK_TOP_NONE, 243 },
				{ StatLabel::DEX, 61, INK_TOP_NONE, 261 },
				{ StatLabel::INT, 61, INK_TOP_NONE, 279 },
				{ StatLabel::LUK, 61, INK_TOP_NONE, 297 },
			};

			for (const ClassicRow& row : classic_ink_rows)
			{
				statoffsets[row.label] = Point<int16_t>(row.x, row.raw_y);
				statinktops[row.label] = row.ink_top;
			}

			for (const ClassicRow& row : classic_raw_rows)
			{
				statoffsets[row.label] = Point<int16_t>(row.x, row.raw_y);
				statinktops[row.label] = INK_TOP_NONE;
			}
		}
		else
		{
			statoffsets[StatLabel::NAME] = statoffset;
			statoffsets[StatLabel::JOB] = statoffset + Point<int16_t>(1, statoffset_y * 1);
			statoffsets[StatLabel::GUILD] = statoffset + Point<int16_t>(1, statoffset_y * 2);
			statoffsets[StatLabel::FAME] = statoffset + Point<int16_t>(1, statoffset_y * 3);
			statoffsets[StatLabel::MIN_DAMAGE] = statoffset + Point<int16_t>(15, statoffset_y * 4);
			statoffsets[StatLabel::MAX_DAMAGE] = statoffset + Point<int16_t>(19, statoffset_y * 5);
			statoffsets[StatLabel::HP] = statoffset + Point<int16_t>(1, statoffset_y * 6);
			statoffsets[StatLabel::MP] = statoffset + Point<int16_t>(1, statoffset_y * 7);
			statoffsets[StatLabel::AP] = statoffset + Point<int16_t>(19, 167);
			statoffsets[StatLabel::STR] = statoffset + Point<int16_t>(1, 196);
			statoffsets[StatLabel::DEX] = statoffset + Point<int16_t>(1, 214);
			statoffsets[StatLabel::INT] = statoffset + Point<int16_t>(1, 232);
			statoffsets[StatLabel::LUK] = statoffset + Point<int16_t>(1, 250);
		}

		// Detailed（classic 详情板数值左对齐于 x=76，参考截图实测；现代面板保持右对齐）
		const Text::Alignment detail_align = classic ? Text::Alignment::LEFT : Text::Alignment::RIGHT;

		statlabels[StatLabel::MIN_DAMAGE_DETAILED] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::MAX_DAMAGE_DETAILED] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::DAMAGE_BONUS] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::BOSS_DAMAGE] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::FINAL_DAMAGE] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::BUFF_DURATION] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::IGNORE_DEFENSE] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::ITEM_DROP_RATE] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::CRITICAL_RATE] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::MESOS_OBTAINED] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::CRITICAL_DAMAGE] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::STATUS_RESISTANCE] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::KNOCKBACK_RESISTANCE] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::DEFENSE] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::SPEED] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::JUMP] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::HONOR] = Text(Text::Font::A11M, Text::Alignment::LEFT, value_color);

		// v83 backgrnd2 烘焙标签「魔法力/魔法防御/命中率/回避率/手技」专用
		statlabels[StatLabel::MAGIC_D] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::MDEF_D] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::ACC_D] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::AVOID_D] = Text(Text::Font::A11M, detail_align, value_color);
		statlabels[StatLabel::HANDS_D] = Text(Text::Font::A11M, detail_align, value_color);

		Point<int16_t> statoffset_detailed = Point<int16_t>(0, 77);
		int16_t statoffset_detailed_lx = 116;
		int16_t statoffset_detailed_rx = 227;

		statoffsets[StatLabel::MIN_DAMAGE_DETAILED] = Point<int16_t>(94, 41);
		statoffsets[StatLabel::MAX_DAMAGE_DETAILED] = Point<int16_t>(105, 59);

		statoffsets[StatLabel::DAMAGE_BONUS] = statoffset_detailed + Point<int16_t>(statoffset_detailed_lx, statoffset_y * 0);
		statoffsets[StatLabel::BOSS_DAMAGE] = statoffset_detailed + Point<int16_t>(statoffset_detailed_rx, statoffset_y * 0);

		statoffsets[StatLabel::FINAL_DAMAGE] = statoffset_detailed + Point<int16_t>(statoffset_detailed_lx, statoffset_y * 1);
		statoffsets[StatLabel::BUFF_DURATION] = statoffset_detailed + Point<int16_t>(statoffset_detailed_rx, statoffset_y * 1);

		statoffsets[StatLabel::IGNORE_DEFENSE] = statoffset_detailed + Point<int16_t>(statoffset_detailed_lx, statoffset_y * 2);
		statoffsets[StatLabel::ITEM_DROP_RATE] = statoffset_detailed + Point<int16_t>(statoffset_detailed_rx, statoffset_y * 2);

		statoffsets[StatLabel::CRITICAL_RATE] = statoffset_detailed + Point<int16_t>(statoffset_detailed_lx, statoffset_y * 3);
		statoffsets[StatLabel::MESOS_OBTAINED] = statoffset_detailed + Point<int16_t>(statoffset_detailed_rx, statoffset_y * 3);

		statoffsets[StatLabel::CRITICAL_DAMAGE] = statoffset_detailed + Point<int16_t>(statoffset_detailed_lx + 3, statoffset_y * 4);

		statoffsets[StatLabel::STATUS_RESISTANCE] = statoffset_detailed + Point<int16_t>(statoffset_detailed_lx, statoffset_y * 5);
		statoffsets[StatLabel::KNOCKBACK_RESISTANCE] = statoffset_detailed + Point<int16_t>(statoffset_detailed_rx, statoffset_y * 5);

		statoffsets[StatLabel::DEFENSE] = statoffset_detailed + Point<int16_t>(statoffset_detailed_lx, statoffset_y * 6);

		statoffsets[StatLabel::SPEED] = statoffset_detailed + Point<int16_t>(statoffset_detailed_lx, statoffset_y * 7);
		statoffsets[StatLabel::JUMP] = statoffset_detailed + Point<int16_t>(statoffset_detailed_lx, statoffset_y * 8);

		statoffsets[StatLabel::HONOR] = statoffset_detailed + Point<int16_t>(73, 263);

		update_all_stats();
		update_stat(MapleStat::Id::JOB);
		update_stat(MapleStat::Id::FAME);

		dimension = Point<int16_t>(212, 318);
		if (classic && Texture(Stat["backgrnd"]).is_valid())
			dimension = Texture(Stat["backgrnd"]).get_dimensions();
		showdetail = false;
	}

	void UIStatsInfo::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		if (classic)
		{
			// v83 八信息行 + AP + 四属性（行带实测见 coords.md；静态贴图不带插值
			// alpha，防闪烁——沿用 2026-10-29 修复结论）。
			static const StatLabel classic_rows[] = {
				StatLabel::NAME, StatLabel::JOB, StatLabel::LEVEL, StatLabel::GUILD,
				StatLabel::HP, StatLabel::MP, StatLabel::EXP, StatLabel::FAME,
				StatLabel::AP, StatLabel::STR, StatLabel::DEX, StatLabel::INT,
				StatLabel::LUK
			};

			for (StatLabel label : classic_rows)
			{
				// 量到墨迹顶的行：绘制点 = 墨迹顶 - 本行文字自己的 (linespace -
				// max bitmap_top)（R27-B1b）。量不到的行：沿用 r15 原始 y。
				const int16_t ink_top = statinktops[label];
				const int16_t y = ink_top == INK_TOP_NONE
					? statoffsets[label].y()
					: statlabels[label].y_for_ink_top(ink_top);

				statlabels[label].draw(position + Point<int16_t>(statoffsets[label].x(), y));
			}

			// 不可加点（AP=0）：灰标签条 Stat/Disabled{STR,DEX,INT,LUK}（48x16）
			// 盖住烘焙绿格（绿格 x7..54，行带顶 y=243/261/279/297）。
			if (!hasap)
			{
				static const Point<int16_t> disabled_pos[4] = {
					Point<int16_t>(7, 243), Point<int16_t>(7, 261),
					Point<int16_t>(7, 279), Point<int16_t>(7, 297)
				};

				for (size_t i = 0; i < 4; i++)
					if (disabled_labels[i].is_valid())
						disabled_labels[i].draw(DrawArgument(position + disabled_pos[i]));
			}

			// 「详细」属性板 backgrnd2（218x203）：贴主窗右缘、底对齐 (208,144)
			// （参考截图 角色能力.png 模板匹配 SAD=46 的最优解，347-203=144）。
			// 9 行数值左对齐 x=76、首行 y=6、pitch 18（截图实测行顶 11/29/47…）。
			if (showdetail && classic_detail.is_valid())
			{
				Point<int16_t> dp = position + Point<int16_t>(208, 144);

				classic_detail.draw(DrawArgument(dp));

				statlabels[StatLabel::MIN_DAMAGE_DETAILED].draw(dp + Point<int16_t>(76, 6));
				statlabels[StatLabel::MAX_DAMAGE_DETAILED].draw(dp + Point<int16_t>(76 + statlabels[StatLabel::MIN_DAMAGE_DETAILED].width(), 6));

				static const StatLabel detail_rows[8] = {
					StatLabel::DEFENSE,
					StatLabel::MAGIC_D,
					StatLabel::MDEF_D,
					StatLabel::ACC_D,
					StatLabel::AVOID_D,
					StatLabel::HANDS_D,
					StatLabel::SPEED,
					StatLabel::JUMP
				};

				for (int16_t i = 0; i < 8; i++)
					statlabels[detail_rows[i]].draw(dp + Point<int16_t>(76, static_cast<int16_t>(24 + i * 18)));
			}

			// 窗内蓝头信息板 basicStat（159x121，origin=(78,0)）：始终绘制于窗内
			// (8,195)（参考截图模板匹配 SAD=0.0）。Texture::draw 会减 origin，
			// 故绘制点 = 左上角 + origin。
			if (classic_help.is_valid())
				classic_help.draw(DrawArgument(position + Point<int16_t>(8, 195) + classic_help.get_origin()));
		}
		else if (showdetail && textures_detail.size() >= 4)
		{
			Point<int16_t> detail_pos(position + Point<int16_t>(212, 0));

			textures_detail[0].draw(detail_pos + Point<int16_t>(0, -1));
			textures_detail[1].draw(detail_pos);
			textures_detail[2].draw(detail_pos);
			textures_detail[3].draw(detail_pos);

			abilities[Ability::NONE].draw(DrawArgument(detail_pos));

			inner_ability[false].draw(detail_pos);
			inner_ability[false].draw(detail_pos + Point<int16_t>(0, 19));
			inner_ability[false].draw(detail_pos + Point<int16_t>(0, 38));
		}

		if (!classic)
		{
			size_t last = showdetail ? StatLabel::HONOR + 1 : StatLabel::NUM_NORMAL;

			for (size_t i = 0; i < last; i++)
			{
				Point<int16_t> labelpos = position + statoffsets[i];

				if (i >= StatLabel::NUM_NORMAL)
					labelpos.shift_x(213);

				if (jobId == Job::Level::BEGINNER)
				{
					if (i < AP || i > NUM_NORMAL)
						statlabels[i].draw(labelpos);
				}
				else
				{
					statlabels[i].draw(labelpos);
				}
			}
		}

		UIElement::draw_buttons(alpha);
	}

	void UIStatsInfo::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'A' || keycode == 'a') {
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				if (tip.empty()) return;
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				MapleStat::Id st = MapleStat::Id::STR;
				bool ok = false;
				if (t == "str" || t == "1") { st = MapleStat::Id::STR; ok = true; }
				else if (t == "dex" || t == "2") { st = MapleStat::Id::DEX; ok = true; }
				else if (t == "int" || t == "3") { st = MapleStat::Id::INT; ok = true; }
				else if (t == "luk" || t == "4") { st = MapleStat::Id::LUK; ok = true; }
				if (ok) {
					SpendApPacket(st).dispatch();
					LOG(LOG_DEBUG, "UIStatsInfo: SoftKey SpendAp " << tip);
				}
			}, []() {}, "AP str|dex|int|luk|1-4");
		}
		else if (keycode == 'P' || keycode == 'p')
		{
			auto ui = UI::get().get_element<UISkillUp>();
			if (!ui) ui = UI::get().emplace<UISkillUp>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'K' || keycode == 'k')
		{
			auto ui = UI::get().get_element<UISkillBook>();
			if (!ui)
				ui = UI::get().emplace<UISkillBook>(
					Stage::get().get_player().get_stats(),
					Stage::get().get_player().get_skills());
			else if (!ui->is_active())
				ui->makeactive();
		}
	}

	bool UIStatsInfo::is_in_range(Point<int16_t> cursorpos) const
	{
		Point<int16_t> pos_adj;

		if (showdetail)
			pos_adj = Point<int16_t>(218, 0);
		else
			pos_adj = Point<int16_t>(0, 0);

		auto bounds = Rectangle<int16_t>(position, position + dimension + pos_adj);
		return bounds.contains(cursorpos);
	}

	UIElement::Type UIStatsInfo::get_type() const
	{
		return TYPE;
	}

	void UIStatsInfo::update_all_stats()
	{
		update_simple(AP, MapleStat::Id::AP);

		if (hasap ^ (stats.get_stat(MapleStat::Id::AP) > 0))
			update_ap();

		statlabels[StatLabel::NAME].change_text(stats.get_name());
		statlabels[StatLabel::LEVEL].change_text(std::to_string(stats.get_stat(MapleStat::Id::LEVEL)));
		update_exp_label();

		statlabels[StatLabel::GUILD].change_text("-");
		statlabels[StatLabel::HP].change_text(std::to_string(stats.get_stat(MapleStat::Id::HP)) + " / " + std::to_string(stats.get_total(EquipStat::Id::HP)));
		statlabels[StatLabel::MP].change_text(std::to_string(stats.get_stat(MapleStat::Id::MP)) + " / " + std::to_string(stats.get_total(EquipStat::Id::MP)));

		update_basevstotal(StatLabel::STR, MapleStat::Id::STR, EquipStat::Id::STR);
		update_basevstotal(StatLabel::DEX, MapleStat::Id::DEX, EquipStat::Id::DEX);
		update_basevstotal(StatLabel::INT, MapleStat::Id::INT, EquipStat::Id::INT);
		update_basevstotal(StatLabel::LUK, MapleStat::Id::LUK, EquipStat::Id::LUK);

		statlabels[StatLabel::MIN_DAMAGE].change_text(std::to_string(stats.get_mindamage()));
		statlabels[StatLabel::MAX_DAMAGE].change_text(" ~ " + std::to_string(stats.get_maxdamage()));

		if (stats.is_damage_buffed())
		{
			statlabels[StatLabel::MIN_DAMAGE].change_color(Color::Name::RED);
			statlabels[StatLabel::MIN_DAMAGE].change_color(Color::Name::RED);
		}
		else
		{
			statlabels[StatLabel::MAX_DAMAGE].change_color(Color::Name::EMPEROR);
			statlabels[StatLabel::MAX_DAMAGE].change_color(Color::Name::EMPEROR);
		}

		statlabels[StatLabel::MIN_DAMAGE_DETAILED].change_text(std::to_string(stats.get_mindamage()));
		statlabels[StatLabel::MAX_DAMAGE_DETAILED].change_text(" ~ " + std::to_string(stats.get_maxdamage()));
		statlabels[StatLabel::DAMAGE_BONUS].change_text("0%");
		statlabels[StatLabel::BOSS_DAMAGE].change_text(std::to_string(static_cast<int32_t>(stats.get_bossdmg() * 100)) + "%");
		statlabels[StatLabel::FINAL_DAMAGE].change_text("0.00%");
		statlabels[StatLabel::BUFF_DURATION].change_text("0%");
		statlabels[StatLabel::IGNORE_DEFENSE].change_text(std::to_string(static_cast<int32_t>(stats.get_ignoredef())) + "%");
		statlabels[StatLabel::ITEM_DROP_RATE].change_text("0%");
		statlabels[StatLabel::CRITICAL_RATE].change_text(std::to_string(static_cast<int32_t>(stats.get_critical() * 100)) + "%");
		statlabels[StatLabel::MESOS_OBTAINED].change_text("0%");
		statlabels[StatLabel::CRITICAL_DAMAGE].change_text("0.00%");
		statlabels[StatLabel::STATUS_RESISTANCE].change_text(std::to_string(static_cast<int32_t>(stats.get_resistance())));
		statlabels[StatLabel::KNOCKBACK_RESISTANCE].change_text("0%");

		update_buffed(StatLabel::DEFENSE, EquipStat::Id::WDEF);

		statlabels[StatLabel::MAGIC_D].change_text(std::to_string(stats.get_total(EquipStat::Id::MAGIC)));
		statlabels[StatLabel::MDEF_D].change_text(std::to_string(stats.get_total(EquipStat::Id::MDEF)));
		statlabels[StatLabel::ACC_D].change_text(std::to_string(stats.get_total(EquipStat::Id::ACC)));
		statlabels[StatLabel::AVOID_D].change_text(std::to_string(stats.get_total(EquipStat::Id::AVOID)));
		statlabels[StatLabel::HANDS_D].change_text(std::to_string(stats.get_total(EquipStat::Id::HANDS)));

		statlabels[StatLabel::SPEED].change_text(std::to_string(stats.get_total(EquipStat::Id::SPEED)) + "%");
		statlabels[StatLabel::JUMP].change_text(std::to_string(stats.get_total(EquipStat::Id::JUMP)) + "%");
		statlabels[StatLabel::HONOR].change_text(std::to_string(stats.get_honor()));
	}

	void UIStatsInfo::update_stat(MapleStat::Id stat)
	{
		switch (stat)
		{
			case MapleStat::Id::JOB:
			{
				jobId = stats.get_stat(MapleStat::Id::JOB);

				statlabels[StatLabel::JOB].change_text(stats.get_jobname());

				bool show_ap = jobId != Job::Level::BEGINNER;
				if (auto* b = find_btn(buttons, Buttons::BT_HP)) b->set_active(show_ap);
				if (auto* b = find_btn(buttons, Buttons::BT_MP)) b->set_active(show_ap);
				if (auto* b = find_btn(buttons, Buttons::BT_STR)) b->set_active(show_ap);
				if (auto* b = find_btn(buttons, Buttons::BT_DEX)) b->set_active(show_ap);
				if (auto* b = find_btn(buttons, Buttons::BT_INT)) b->set_active(show_ap);
				if (auto* b = find_btn(buttons, Buttons::BT_LUK)) b->set_active(show_ap);
				if (auto* b = find_btn(buttons, Buttons::BT_AUTO)) b->set_active(show_ap);

				break;
			}
			case MapleStat::Id::FAME:
			{
				update_simple(StatLabel::FAME, MapleStat::Id::FAME);
				break;
			}
			case MapleStat::Id::LEVEL:
			{
				statlabels[StatLabel::LEVEL].change_text(std::to_string(stats.get_stat(MapleStat::Id::LEVEL)));
				update_exp_label();
				break;
			}
			case MapleStat::Id::EXP:
			{
				update_exp_label();
				break;
			}
			}
	}

	Button::State UIStatsInfo::button_pressed(uint16_t id)
	{
		const Player& player = Stage::get().get_player();

		switch (id)
		{
			case Buttons::BT_CLOSE:
			{
				deactivate();
				break;
			}
			case Buttons::BT_HP:
			{
				send_apup(MapleStat::Id::HP);
				break;
			}
			case Buttons::BT_MP:
			{
				send_apup(MapleStat::Id::MP);
				break;
			}
			case Buttons::BT_STR:
			{
				send_apup(MapleStat::Id::STR);
				break;
			}
			case Buttons::BT_DEX:
			{
				send_apup(MapleStat::Id::DEX);
				break;
			}
			case Buttons::BT_INT:
			{
				send_apup(MapleStat::Id::INT);
				break;
			}
			case Buttons::BT_LUK:
			{
				send_apup(MapleStat::Id::LUK);
				break;
			}
			case Buttons::BT_AUTO:
			{
				uint16_t autostr = 0;
				uint16_t autodex = 0;
				uint16_t autoint = 0;
				uint16_t autoluk = 0;
				uint16_t nowap = stats.get_stat(MapleStat::Id::AP);
				EquipStat::Id id = player.get_stats().get_job().get_primary(player.get_weapontype());

				switch (id)
				{
					case EquipStat::Id::STR:
						autostr = nowap;
						break;
					case EquipStat::Id::DEX:
						autodex = nowap;
						break;
					case EquipStat::Id::INT:
						autoint = nowap;
						break;
					case EquipStat::Id::LUK:
						autoluk = nowap;
						break;
				}

				std::string message =
					"Your AP will be distributed as follows:\\r"
					"\\nSTR : +" + std::to_string(autostr) +
					"\\nDEX : +" + std::to_string(autodex) +
					"\\nINT : +" + std::to_string(autoint) +
					"\\nLUK : +" + std::to_string(autoluk) +
					"\\r\\n";

				std::function<void(bool)> yesnohandler = [&, autostr, autodex, autoint, autoluk](bool yes)
				{
					if (yes)
					{
						// Wave 3：单包 AUTO_DISTRIBUTE_AP(88) 替换旧的 N 次
						// SPEND_AP(87) 逐点循环（HeavenClient 遗法：AP=40 就
						// 40 个网络往返 + 40 次 s2c 回显）。s9 标准分支（DB 种子
						// use_server_auto_assigner=false）body = skip(8) +
						// 2×{int type,int val}，type 走 Stat.getBy5ByteEncoding
						// （STR=64/DEX=128/INT=256/LUK=512，与 MapleStat::codes
						// 不是一套），val 须 0..remainingAp。
						// 语义与旧循环一致：主属性吃满全额。第二对给 0 值占位
						// （服务端严格读满 16B）；gainStatByType 先 gain<=0 早退，
						// type=0(null) 不会被解引用 switch。
						auto to_code = [](uint16_t which) -> AutoAssignApPacket::StatCode
						{
							switch (which)
							{
								case 0: return AutoAssignApPacket::StatCode::STR;
								case 1: return AutoAssignApPacket::StatCode::DEX;
								case 2: return AutoAssignApPacket::StatCode::INT;
								default: return AutoAssignApPacket::StatCode::LUK;
							}
						};

						// 主属性 = 预演算里唯一非零项（上面 switch 只填一个）
						int primary = autostr > 0 ? 0 : (autodex > 0 ? 1 : (autoint > 0 ? 2 : 3));
						const int32_t ap = primary == 0 ? autostr : (primary == 1 ? autodex : (primary == 2 ? autoint : autoluk));

						if (ap <= 0)
							return;	// 无 AP 可分：不发

						// 主属性吃满 AP（与旧逐点循环同语义：旧代码也只点 primary）；
						// 第二对给合法类型码 + 0 值占位（服务端严格读满 16B；
						// gainStatByType 在 gain<=0 早退，switch 不求值 type）。
						// 第二对类型码避开主属性：primary 0=STR 1=DEX 2=INT 3=LUK
						// → 取 (primary+1)%4 的编码，永不与主属性撞。
						const AutoAssignApPacket::StatCode second_code =
							to_code((primary + 1) % 4);

						AutoAssignApPacket(to_code(primary), ap, second_code, 0).dispatch();

						perf::mark("AP", "autoassign primary=" + std::to_string(primary)
							+ " ap=" + std::to_string(ap));
					}
				};

				UI::get().emplace<UIYesNo>(message, yesnohandler, Text::Alignment::LEFT);
				break;
			}
			case Buttons::BT_HYPERSTATOPEN:
			{
				break;
			}
			case Buttons::BT_HYPERSTATCLOSE:
			{
				if (player.get_level() < 140)
					UI::get().emplace<UIOk>("You can use the Hyper Stat at Lv. 140 and above.", [](bool) {});

				break;
			}
			case Buttons::BT_DETAILOPEN:
			{
				// classic 下「详情」钮保持可见（参考截图展开态仍显示），再点即收起
				set_detail(!showdetail);
				break;
			}
			case Buttons::BT_DETAILCLOSE:
			case Buttons::BT_DETAIL_DETAILCLOSE:
			{
				set_detail(false);
				break;
			}
			case Buttons::BT_ABILITY:
			default:
			{
				break;
			}
		}

		return Button::State::NORMAL;
	}

	void UIStatsInfo::send_apup(MapleStat::Id stat) const
	{
		SpendApPacket(stat).dispatch();
		UI::get().disable();
	}

	void UIStatsInfo::set_detail(bool enabled)
	{
		if (classic)
		{
			// classic 详细板用 classic_detail（Stat/backgrnd2），不走 textures_detail
			if (!classic_detail.is_valid())
				return;
		}
		else if (textures_detail.empty())
			return;

		showdetail = enabled;

		// classic：「详情」钮常显（点它=开/关切换，见 button_pressed）；
		// 收起钮 BtHide 只在展开时可点 —— 修复「展开后收不回」（§6-Q2）。
		// 展开时窗口加宽以容纳主窗右侧的 backgrnd2 详情板（否则画在 x>主窗宽 处被裁切、
		// 右侧「攻击力/防御等」详情面板不可见）；收起恢复主窗宽。
		if (classic)
		{
			nl::node statnode = UiAssetResolve::window4("Stat");
			Texture mainbg = statnode["backgrnd"];

			if (mainbg.is_valid())
			{
				const int16_t main_w = mainbg.get_dimensions().x();
				const int16_t main_h = mainbg.get_dimensions().y();
				const int16_t detail_w = classic_detail.is_valid() ? classic_detail.get_dimensions().x() : 0;
				const int16_t gap = 4;

				dimension = enabled
					? Point<int16_t>(main_w + gap + detail_w, main_h)
					: Point<int16_t>(main_w, main_h);
			}
		}

		if (auto* b = find_btn(buttons, Buttons::BT_DETAILOPEN))
			b->set_active(classic || !enabled);
		if (auto* b = find_btn(buttons, Buttons::BT_DETAILCLOSE))
			b->set_active(!classic && enabled);
	}

	void UIStatsInfo::update_ap()
	{
		bool nowap = stats.get_stat(MapleStat::Id::AP) > 0;
		Button::State newstate = nowap ? Button::State::NORMAL : Button::State::DISABLED;

		for (int i = Buttons::BT_HP; i <= Buttons::BT_AUTO; i++)
			if (auto* b = find_btn(buttons, i))
				b->set_state(newstate);

		hasap = nowap;
	}

	// v83 经典窗「经验值」行：当前经验(百分比%)，百分比与 UIStatusBar::getexppercent 同源
	void UIStatsInfo::update_exp_label()
	{
		int16_t level = static_cast<int16_t>(stats.get_stat(MapleStat::Id::LEVEL));
		int64_t exp = stats.get_exp();
		std::string exptext = std::to_string(exp);

		// level 上限已扩到 999：不能再以 ExpTable::LEVELCAP(=256) 截断，也
		// 不能直接索引 values[level]（200+ 已由 1.06 公式覆盖，表项为占位 0）。
		// 统一走 required_exp()，与 UIStatusBar::getexppercent 同源。
		if (level > 0)
		{
			int64_t need = ExpTable::required_exp(level);

			if (need > 0)
			{
				int32_t pct = static_cast<int32_t>(
					static_cast<double>(exp) / static_cast<double>(need) * 100.0
					);
				exptext += " (" + std::to_string(pct) + "%)";
			}
		}

		statlabels[StatLabel::EXP].change_text(exptext);
	}

	void UIStatsInfo::update_simple(StatLabel label, MapleStat::Id stat)
	{
		statlabels[label].change_text(std::to_string(stats.get_stat(stat)));
	}

	void UIStatsInfo::update_basevstotal(StatLabel label, MapleStat::Id bstat, EquipStat::Id tstat)
	{
		int32_t base = stats.get_stat(bstat);
		int32_t total = stats.get_total(tstat);
		int32_t delta = total - base;

		std::string stattext = std::to_string(total);

		if (delta)
		{
			stattext += " (" + std::to_string(base);

			if (delta > 0)
				stattext += "+" + std::to_string(delta);
			else if (delta < 0)
				stattext += "-" + std::to_string(-delta);

			stattext += ")";
		}

		statlabels[label].change_text(stattext);
	}

	void UIStatsInfo::update_buffed(StatLabel label, EquipStat::Id stat)
	{
		int32_t total = stats.get_total(stat);
		int32_t delta = stats.get_buffdelta(stat);

		std::string stattext = std::to_string(total);

		if (delta)
		{
			stattext += " (" + std::to_string(total - delta);

			if (delta > 0)
			{
				stattext += "+" + std::to_string(delta);

				statlabels[label].change_color(Color::Name::RED);
			}
			else if (delta < 0)
			{
				stattext += "-" + std::to_string(-delta);

				statlabels[label].change_color(Color::Name::BLUE);
			}

			stattext += ")";
		}

		statlabels[label].change_text(stattext);
	}
}