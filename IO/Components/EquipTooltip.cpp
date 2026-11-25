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
#include "EquipTooltip.h"

#include "../../Configuration.h"
#include "../../Data/EquipData.h"
#include "../../Data/WeaponData.h"
#include "../../Gameplay/Stage.h"
#include "../../Graphics/EquipStarPixels.h"
#include "../../Graphics/EquipGradePixels.h"
#include "../../Util/Locale.h"
#include "../../Util/UiAssetResolve.h"

#include <algorithm>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// 正文行水平锚点（FRAME_WIDTH = 264 为类私有常量，这里按 132 记一半）。
	namespace
	{
		// 附加属性行的冒号中轴。
		constexpr int16_t BODY_CENTER_X = 132;
		// 元信息行（道具ID/装备分类/攻击速度/…）的正文左缘。
		constexpr int16_t BODY_LEFT_X = 9;
	}

	// 停靠链 [悬停][套装?][成长?][对比]
	int16_t EquipTooltip::set_panel_offset() const
	{
		return PREVIEW_OFFSET;
	}

	int16_t EquipTooltip::growth_panel_offset() const
	{
		if (settip.valid())
			return set_panel_offset() + SetTooltip::FRAME_WIDTH + 12 + 6;

		return PREVIEW_OFFSET;
	}

	int16_t EquipTooltip::set_aware_preview_offset() const
	{
		int16_t off = PREVIEW_OFFSET;

		if (settip.valid())
			off = set_panel_offset() + SetTooltip::FRAME_WIDTH + 12 + 6;

		if (growthtip.valid())
			off = growth_panel_offset() + GrowthTooltip::FRAME_WIDTH + 12 + 6;

		return off;
	}

	// 正文行「标签：值」的拆分与摆放（2026-11-11 用户实拍反馈）。
	//
	// 原生 tip 的两类正文行：
	//   元信息行  道具ID: 1060002 / 装备分类: 下衣 / 攻击速度: 普通 / 可升级次数: 7
	//             → 标签贴正文左缘，冒号紧跟标签；值紧接冒号。
	//   附加属性行 力量: 3 / 敏捷: 0 ...
	//             → 冒号固定在正文中线，标签右对齐到冒号、值自冒号起左对齐，
	//               多行叠起来冒号成一条竖直的中轴线。
	//
	// 旧实现把整串（含冒号）用 Text::Alignment::CENTER 画在中线上，于是
	// 「装备分类: 下衣」的冒号随标签长度左右漂移，用户看到的就是「冒号不齐」。
	namespace
	{
		// 全角「：」的 UTF-8 三字节序列。
		constexpr const char* FULLWIDTH_COLON = "\xEF\xBC\x9A";

		// 2026-11-20 用户裁决 q2：长标签（≥6 汉字，如「金锤子已提高的强化次数」「所有者」）
		// 若仍把冒号压中轴，标签会左伸到框外、值被顶出右侧边框 → 这类行改左对齐。
		// 短标签（力量/攻击力/道具ID…）维持冒号中轴。locale 为 UTF-8（每汉字 3 字节），
		// 统计首个字节落在 0xE0..0xEF 的 UTF-8 三字节序列数即汉字数。
		size_t utf8_han_count(const std::string& text)
		{
			size_t n = 0;

			for (size_t i = 0; i + 2 < text.size(); ++i)
			{
				const uint8_t c = static_cast<uint8_t>(text[i]);

				if (c >= 0xE0 && c <= 0xEF && (static_cast<uint8_t>(text[i + 1]) & 0xC0) == 0x80
					&& (static_cast<uint8_t>(text[i + 2]) & 0xC0) == 0x80)
				{
					++n;
					i += 2;
				}
			}

			return n;
		}

		constexpr size_t LONG_LABEL_HAN = 6;  // ≥6 汉字视为长标签，冒号不居中

		// 拆出整串里最后一个冒号：中文 locale 用全角「：」，英文 locale 用半角 ':'。
		// 只认最后一个，避免本地化文本里出现别的冒号（例如「上限」拼接）。
		size_t last_colon(const std::string& text)
		{
			size_t pos = text.rfind(FULLWIDTH_COLON);

			if (pos != std::string::npos)
				return pos;

			return text.rfind(':');
		}

		size_t colon_width(const std::string& text, size_t pos)
		{
			return (pos != std::string::npos && text.compare(pos, 3, FULLWIDTH_COLON) == 0) ? 3 : 1;
		}
	}

	void EquipTooltip::BodyLine::build(const std::string& full, Color::Name valuecolor, bool center_colon)
	{
		label = Text();
		colon = Text();
		value = Text();

		const size_t pos = last_colon(full);

		center_whole = false;
		// q2：长标签冒号不居中，防溢出 264px 框宽。判定基于冒号左侧标签的汉字数。
		centered = center_colon;

		if (pos != std::string::npos && utf8_han_count(full.substr(0, pos)) >= LONG_LABEL_HAN)
			centered = false;

		// 正文行（含 inc 属性与元信息）统一纯白，与顶部职业/需求位图字模(255,255,255)同色。
		// 旧实现标签用 TOOLTIPBODY 奶油(255,238,204)，实测最亮仅(250,233,200)，
		// 比职业行纯白暗一档、红框属性区观感偏灰（用户 2026-11-17 反馈）。
		// 数值本就是 WHITE，但走 Text::A11M 抗锯齿后最亮(233,233,233)；
		// 标签一并改纯白后整行观感与职业行对齐。
		const Color::Name namecolor = Color::Name::WHITE;

		if (pos == std::string::npos)
		{
			// 无冒号：居中行整串按中线摆（draw 用 CENTER 宽估）；非居中仍左对齐。
			if (center_colon)
				value = Text(Text::Font::A11M, Text::Alignment::CENTER, valuecolor, full);
			else
				value = Text(Text::Font::A11M, Text::Alignment::LEFT, valuecolor, full);
			return;
		}

		const size_t cw = colon_width(full, pos);

		label = Text(Text::Font::A11M, Text::Alignment::LEFT, namecolor, full.substr(0, pos));
		colon = Text(Text::Font::A11M, Text::Alignment::LEFT, namecolor, full.substr(pos, cw));
		value = Text(Text::Font::A11M, Text::Alignment::LEFT, valuecolor, full.substr(pos + cw));
	}

	// 2026-11-21 用户实拍「装备只有 ID 没有名称」根因：String/Eqp.img 无该 id 的
	// name 节点 → ItemData::get_name() 空 → 旧代码回退裸数字 ID（如 1152060）。
	// 原生对无名装备显示「部位/分类」的可读名，这里用本端已有的 equip.type.* 文案：
	// 「护肩(装备分类：护肩)」这类重复不可取，直接用本地化分类名做标题。
	std::string equip_fallback_name(const EquipData& equipdata, int32_t item_id)
	{
		const std::string rawtype = equipdata.get_type();
		if (!rawtype.empty())
		{
			const std::string localized = Locale::enum_text("equip.type.", rawtype);
			// enum_text 缺键会回显 "equip.type.XXX"；只有含本地化结果才用。
			if (!localized.empty() && localized.find("equip.type.") == std::string::npos)
				return localized;
		}
		return "装备" + std::to_string(item_id);
	}

	void EquipTooltip::BodyLine::build_centered(const std::string& full, Color::Name valuecolor)
	{
		label = Text();
		colon = Text();
		value = Text(Text::Font::A11M, Text::Alignment::CENTER, valuecolor, full);
		centered = false;
		center_whole = true;
	}

	int16_t EquipTooltip::BodyLine::draw(Point<int16_t> pos) const
	{
		if (empty())
			return 0;

		// 2026-11-20 trade/owner 整串居中：以 tip 中线为轴，与无冒号居中行同口径。
		if (center_whole && !value.empty())
		{
			value.draw(pos + Point<int16_t>(BODY_CENTER_X, 0));
			return static_cast<int16_t>(BODY_CENTER_X + value.width() / 2);
		}

		const int16_t lw = label.width();
		const int16_t cw = colon.width();
		const int16_t vw = value.width();

		// 无冒号的居中行：整串以 tip 中线为轴（CENTER 文本原点即中心）。
		if (centered && label.empty() && colon.empty() && !value.empty())
		{
			value.draw(pos + Point<int16_t>(BODY_CENTER_X, 0));
			return static_cast<int16_t>(BODY_CENTER_X + vw / 2);
		}

		int16_t lx = 0;

		if (centered && !label.empty())
			lx = static_cast<int16_t>(BODY_CENTER_X - cw / 2 - lw);
		else
			lx = BODY_LEFT_X;

		int16_t cx = static_cast<int16_t>(lx + lw);

		if (!label.empty())
			label.draw(pos + Point<int16_t>(lx, 0));

		if (!colon.empty())
			colon.draw(pos + Point<int16_t>(cx, 0));

		const int16_t vx = static_cast<int16_t>(cx + cw);

		if (!value.empty())
			value.draw(pos + Point<int16_t>(vx, 0));

		return static_cast<int16_t>(vx + vw);
	}
	// 2026-11-15 按 079 参照帧 + Angel 上游重排：头部区块整体上移，
	// 图标底板紧贴名称分隔线、需求块与框顶齐、成长/职业/正文间距一致。
	// 依据：079 逐像素（缩放 ≈1.288）、Angel base.draw(pos+(12,10))。
	namespace
	{
		constexpr int16_t REQ_LABEL_X = 97;      // 需求块标签位图 x（相对需求块起点）
		constexpr int16_t REQ_VALUE_DX = 50;     // 数字紧跟标签，47px 位图 + 3px 间隙
		// 5x7 字模相对 47x11 标签下沉 2px，视觉与标签中线齐（2026-11-11 用户实拍）。
		constexpr int16_t REQ_VALUE_DY = 2;
		constexpr int16_t GROWTH_LEV_Y = 84;     // 第 7 行「道具等级」（紧跟 6 行需求，行距 14）
		constexpr int16_t GROWTH_EXP_Y = 98;     // 第 8 行「道具经验」
		// 需求块推进：6 行 / 8 行分别 87 / 115（行距 14；旧 12px 会叠字）。
		constexpr int16_t REQ_SHIFT_LEGACY = 41;
		constexpr int16_t REQ_SHIFT_BASE = 87;
		constexpr int16_t REQ_SHIFT_GROWTH = 115;
		// 框体基准高度：内容整体较原布局上移 73px，同步收紧 73px，背板底边距不变。
		constexpr int16_t HEIGHT_BASE = 540 + 15 - 73;
		// 079 原生 1:1 (m00303)：名称墨团 y95..110、图标框顶 y138 ⇒ 名称底到图标 28px。
		// A14B 名称墨高约 15px（绘制原点 y7 ⇒ 墨底 y22），图标/需求块起点取 52 ⇒ 30px 间距。
		// 旧 30 把 req 压到名称脚边（m00304 ③）；52 同时给名称区两行隐藏词条
		//（潜能 y24 / 交易 y38）留出不重叠空间。
		constexpr int16_t HEADER_SHIFT = 52;  // m00304 ③：名称→req/图标 30px 间距

		// 名称区上下词条统一间距体系（2026-11-18 用户要求：名称上下词条间距一致）。
		// 名称绘制点、上方星顶、下方首行词条、词条行距全部由这一组常量推导，
		// 改一处即全联动，避免「星 -6 / 名称 7 / 词条 24 / 行距 14」散落硬编码导致不齐。
		constexpr int16_t NAME_Y = 7;                 // 名称文字绘制 y（相对 pos）
		constexpr int16_t NAME_INK_H = 15;            // A14B 名称墨高
		constexpr int16_t STAR_TOP = -6;              // 星排顶（背板顶沿），星 13px 高，底=STAR_TOP+13=7 正好贴名称顶
		constexpr int16_t SUBLINE_STEP = 18;          // 名称下方各词条（潜能等级/交易/所有者）统一行距
		// 2026-11-21 用户实拍「名称/不可交易/所有者贴在一起、间距不一样」根因：
		// 旧 SUBLINE_FIRST = 名称墨底+2 = 24，名称(A14B,墨底 y22)到首行只有 2px，
		// 而词条相互是 18px draw-y（A11M 墨高≈11 ⇒ 视觉空隙≈7px）。
		// 让首行也留同样的视觉空隙：名称墨底(NAME_Y+NAME_INK_H=22) + (SUBLINE_STEP-11=7) = 29。
		// 这样 名称→trade、trade→owner、owner→需求 三段空隙一致。
		constexpr int16_t SUBLINE_FIRST = NAME_Y + NAME_INK_H + (SUBLINE_STEP - 11); // = 29

		// 分隔线以下正文区间距体系（2026-11-20 用户 q1：同类词条上下间距一致）。
		// 元信息/属性/可升级/金锤/潜能/火花/宝珠/星之力所有行统一 LINE_STEP；
		// 潜能块、强化块与主属性块之间统一 BLOCK_GAP；描述前统一 DESC_GAP。
		// 主窗与对比窗共用同一组常量，消除原 15 / 6+15 / 13 的散落魔数。
		constexpr int16_t LINE_STEP = 15;   // 正文相邻行统一行距（原 stat_y=15、潜能/强化行距=15）
		constexpr int16_t BLOCK_GAP = 6;    // 潜能块/强化块与主属性块之间的留白（原 shift_y(6)）
		constexpr int16_t DESC_GAP = 13;    // 描述行前的留白（原 shift_y(13)）
	}

	// 道具图标底板（2026-11-15 按 079 参照帧还原）：UIWindow.img/IconBase/0 是
	// 32x32 的白底 + 黑描边 + 蓝角方框，与道具图标同为 2x 绘制、两者同心同尺寸。
	// orange-wz 实读该位图：白(255,255,255) 780px / 黑 112px / 蓝角(187,204,221)。
	// 079 帧里它是半透明浅蓝（非实心白），故以 ICON_BOX_ALPHA 控制不透明度。
	namespace
	{
		constexpr int16_t ICON_BOX_SIZE = 32;	// 原生 32x32
		constexpr float ICON_BOX_SCALE = 2.0f;	// 与 itemicon 同 2x
		constexpr float ICON_BOX_ALPHA = 0.65f;	// 079 半透明浅蓝白
		constexpr int16_t ICON_BOX_X = 16;		// 相对分隔线（与原 iconbase 同位）
		constexpr int16_t ICON_BOX_Y = 0;				// 与需求首行同起点（079）
	}

	EquipTooltip::EquipTooltip()
	{
		nl::node Equip = nl::nx::UI["UIWindow.img"]["ToolTip"]["Equip"];
		nl::node EquipCan = Equip["Can"];
		nl::node EquipCannot = Equip["Cannot"];

		// 原生职业行下方分隔线为 1px 纯白（参考帧 y237 全宽 255,255,255），旧 α0.4 偏灰。
		separator = ColorLine(FRAME_WIDTH, Color::Name::WHITE, 1.0f, false);

		// 095 布局分组标题：潜在属性(绿) / 附加潜能(紫)，参考图「● 潜在属性 / ● 附加潜能」。
		pot_title = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::GREEN,
			Locale::text("equip.potential.title"));
		bonus_title = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::VIOLET,
			Locale::text("equip.bonus.title"));
		pot_title_preview = pot_title;
		bonus_title_preview = bonus_title;

		// 2026-11-15 按 079 参照帧还原：图标背后就是 UIWindow.img/IconBase/0
		//（32x32 白底 + 黑描边 + 蓝角），与道具图标同为 2x 绘制。
		// 旧实现用 32x32 ColorBox 直接 draw（未乘 2x），底板只有图标一半大，
		// 图标溢出底板 —— 这正是 079 参照与当前截图「背景大小不一致」的根因。
		nl::node iconbase_node = nl::nx::UI["UIWindow.img"]["IconBase"]["0"];

		if (iconbase_node)
		iconbase = Texture(iconbase_node);


		// 成长两行资源：启用态标签/字模/百分号，禁用态字模备用。
		nl::node growth_on = Equip["GrowthEnabled"];
		nl::node growth_off = Equip["GrowthDisabled"];

		growth_lev_label = Texture(growth_on["itemLEV"]);
		growth_exp_label = Texture(growth_on["itemEXP"]);
		growth_percent = Texture(growth_on["percent"]);
		fame_none = Texture(EquipCan["none"]);
		growthset[true] = Charset(growth_on, Charset::Alignment::LEFT);
		growthset[false] = Charset(growth_off, Charset::Alignment::LEFT);

		show_growth = false;
		show_growth_preview = false;
		req_shift = REQ_SHIFT_BASE;
		req_shift_preview = REQ_SHIFT_BASE;

		requirements.push_back(MapleStat::Id::LEVEL);
		requirements.push_back(MapleStat::Id::STR);
		requirements.push_back(MapleStat::Id::DEX);
		requirements.push_back(MapleStat::Id::INT);
		requirements.push_back(MapleStat::Id::LUK);
		// 原生需求块含「要求人气」行，位图 reqPOP 已在本函数映射。
		requirements.push_back(MapleStat::Id::FAME);

		reqstattextures[MapleStat::Id::LEVEL][false] = EquipCannot["reqLEV"];
		reqstattextures[MapleStat::Id::LEVEL][true] = EquipCan["reqLEV"];
		reqstattextures[MapleStat::Id::FAME][false] = EquipCannot["reqPOP"];
		reqstattextures[MapleStat::Id::FAME][true] = EquipCan["reqPOP"];
		reqstattextures[MapleStat::Id::STR][false] = EquipCannot["reqSTR"];
		reqstattextures[MapleStat::Id::STR][true] = EquipCan["reqSTR"];
		reqstattextures[MapleStat::Id::DEX][false] = EquipCannot["reqDEX"];
		reqstattextures[MapleStat::Id::DEX][true] = EquipCan["reqDEX"];
		reqstattextures[MapleStat::Id::INT][false] = EquipCannot["reqINT"];
		reqstattextures[MapleStat::Id::INT][true] = EquipCan["reqINT"];
		reqstattextures[MapleStat::Id::LUK][false] = EquipCannot["reqLUK"];
		reqstattextures[MapleStat::Id::LUK][true] = EquipCan["reqLUK"];

		// 2026-11-08 原生复校：需求块是单列 8 行、行距 12px（req_6x.png 逐像素取样），
		// 旧两列 15px 布局与原生不符且缺「要求人气/道具等级/道具经验」三行。
		// 47x11 标签位图 + 数字：行距 12px 才不会像 9px 那样叠成糊块。
		// 需求标签位图高约 11~13px；旧步进 12 在中文抗锯齿下会上下叠字。
		constexpr int16_t REQ_ROW = 14;
		reqstatpositions[MapleStat::Id::LEVEL] = Point<int16_t>(REQ_LABEL_X, 0);
		reqstatpositions[MapleStat::Id::STR] = Point<int16_t>(REQ_LABEL_X, REQ_ROW);
		reqstatpositions[MapleStat::Id::DEX] = Point<int16_t>(REQ_LABEL_X, REQ_ROW * 2);
		reqstatpositions[MapleStat::Id::INT] = Point<int16_t>(REQ_LABEL_X, REQ_ROW * 3);
		reqstatpositions[MapleStat::Id::LUK] = Point<int16_t>(REQ_LABEL_X, REQ_ROW * 4);
		reqstatpositions[MapleStat::Id::FAME] = Point<int16_t>(REQ_LABEL_X, REQ_ROW * 5);

		reqset[false] = Charset(EquipCannot, Charset::Alignment::LEFT);
		reqset[true] = Charset(EquipCan, Charset::Alignment::LEFT);

		// 包内无 YellowNumber，可装备态改用 Can 数字字模（绿）
		lvset[false] = Charset(EquipCannot, Charset::Alignment::LEFT);
		lvset[true] = Charset(EquipCan, Charset::Alignment::LEFT);

		// 原生职业名双态位图：Can/* 纯白 (255,255,255)、Cannot/* 红 (204,17,34)
		//（orange-wz 实读 live UIWindow.img 逐像素坐实）。6 张恒画，逐职业取态。
		static const char* const jobnames[6] = {"beginner", "warrior", "magician", "bowman", "thief", "pirate"};

		for (uint8_t i = 0; i < 6; ++i)
		{
			jobs[false][i] = EquipCannot[jobnames[i]];
			jobs[true][i] = EquipCan[jobnames[i]];
		}

		// 095 星之力图标：黄/灰两态 BGRA 源自 095 原生黄星 canvas（EquipStarPixels.h），
		// 上传共享 atlas 后按星数在名称上方绘制；不改任何 WZ/IMG 资源。
		star_yellow.set(equipstar::ID_YELLOW, equipstar::W, equipstar::H,
			std::vector<uint8_t>(equipstar::YELLOW_BGRA, equipstar::YELLOW_BGRA + equipstar::W * equipstar::H * 4));
		star_gray.set(equipstar::ID_GRAY, equipstar::W, equipstar::H,
			std::vector<uint8_t>(equipstar::GRAY_BGRA, equipstar::GRAY_BGRA + equipstar::W * equipstar::H * 4));

		// 品级角标 6 档：192 Grade/0..5 的 BGRA 常量直传 atlas（EquipGradePixels.h）。
#define SET_GRADE_TEX(idx, NM) \
		grade_tex[idx].set(equipgrade::ID_##NM, equipgrade::W_##NM, equipgrade::H_##NM, \
			std::vector<uint8_t>(equipgrade::NM##_BGRA, equipgrade::NM##_BGRA + equipgrade::W_##NM * equipgrade::H_##NM * 4))
		SET_GRADE_TEX(0, NONE);
		SET_GRADE_TEX(1, HIDDEN);
		SET_GRADE_TEX(2, RARE);
		SET_GRADE_TEX(3, EPIC);
		SET_GRADE_TEX(4, UNIQUE);
		SET_GRADE_TEX(5, LEGENDARY);
#undef SET_GRADE_TEX

		// ===== 192 UIToolTip.img 图形化增强资源（构造期一次性加载）=====
		// 契约 docs/features/ui004-enhancement-ui-port-2026-11/CONTRACT.md：
		// 解析入口 UiAssetResolve::tooltip() → nl::nx::UI["UIToolTip.img"]。
		// 结构：Item/Equip/Star/{Star0,Star1}/Star(canvas)、AddOption/0..5、
		//       Summary/{incline,decline,icon}。
		// 保守原则：每个节点单独判空，缺哪个就留哪个空 Texture；下方所有绘制点
		// 都走 if (tex) 守卫，缺资源时现有文本 / 内嵌 BGRA 像素星照常显示。
		{
			nl::node tt = UiAssetResolve::tooltip();
			int star_loaded = 0;
			int addopt_loaded = 0;
			int summary_loaded = 0;

			if (tt)
			{
				nl::node equip192 = tt["Item"]["Equip"];

				// 星之力两态 canvas（11×10，各自 origin 由 Texture 读出）。
				nl::node star0 = equip192["Star"]["Star0"]["Star"];
				nl::node star1 = equip192["Star"]["Star1"]["Star"];

				if (star0) { star0Tex = Texture(star0); starTex[0] = star0Tex; ++star_loaded; }
				if (star1) { star1Tex = Texture(star1); starTex[1] = star1Tex; ++star_loaded; }

				// 潜能品级小图标（0..5），用字符串名索引避免 operator[](uint) 的歧义。
				nl::node addopt = equip192["AddOption"];

				for (int i = 0; i < 6; ++i)
				{
					nl::node grade = addopt[std::to_string(i)];

					if (grade)
					{
						addOptionTex[i] = Texture(grade);
						++addopt_loaded;
					}
				}

				// 卷轴差值箭头（incline=正 / decline=负）+ 百分比数字图标。
				nl::node summary = equip192["Summary"];
				nl::node inc = summary["incline"];
				nl::node dec = summary["decline"];
				nl::node ico = summary["icon"];

				if (inc) { summaryInclineTex = Texture(inc); ++summary_loaded; }
				if (dec) { summaryDeclineTex = Texture(dec); ++summary_loaded; }
				if (ico) { summaryIconTex = Texture(ico); ++summary_loaded; }
			}

			// [DBG-TT] 构造期一次性诊断：192 资源是否命中、各加载了几张。
			// 仅 _DEBUG 下输出（Release 的 LOG 宏为空实现），不污染上线日志。
			LOG(LOG_DEBUG, "[DBG-TT] EquipTooltip ctor: tooltip_root="
				<< static_cast<bool>(tt) << " star=" << star_loaded
				<< " addOption=" << addopt_loaded
				<< " summary=" << summary_loaded
				<< " (0=全部缺失,走原有回退)");
		}

		invpos = 0;
		invpos_preview = 0;
		show_compare = Configuration::get().get_equip_compare();
		has_trade = false;
		has_trade_preview = false;
		has_owner = false;
		has_owner_preview = false;
	}

	namespace
	{
		// ItemTooltip 同源：unique/untradable → 交易限制文案（i18n）。
		std::string equip_trade_label(const ItemData& itemdata)
		{
			const bool u = itemdata.is_unique();
			const bool t = itemdata.is_untradable();
			if (u && t)
				return Locale::text("equip.trade.both");
			if (u)
				return Locale::text("equip.trade.unique");
			if (t)
				return Locale::text("equip.trade.untradable");
			return {};
		}
	}

	void EquipTooltip::set_equip_itemid(int32_t item_id)
	{
		const EquipData& ed = EquipData::get(item_id);

		if (!ed)
		{
			itemid_cache = 0;
			itemid_equip.reset();
			set_equip(Tooltip::Parent::NONE, 0);
			return;
		}

		// item 变了要打穿 set_equip 的 invpos 早退（同为 -1）。
		if (itemid_cache != item_id)
			invpos = 0;

		EnumMap<EquipStat::Id, uint16_t> stats;

		for (auto es : EquipStat::values)
		{
			const int16_t v = ed.get_defstat(es);
			stats[es] = static_cast<uint16_t>(std::max<int16_t>(0, v));
		}

		itemid_equip = std::make_unique<Equip>(
			item_id, 0, "", 0, ed.get_slots(), 0, stats,
			0, 0, 0, 0, 0, 0, 0, 0);
		itemid_cache = item_id;
		set_equip(Tooltip::Parent::ITEMINVENTORY, -1);
	}

	void EquipTooltip::set_equip(Tooltip::Parent parent, int16_t ivp)
	{
		// 早退只跳过「同一槽位重复悬停」的重建，但 ⑤ 套装件数依赖**已穿戴集合**，
		// 换装备后同一槽位的 itemid 会变而 invp 不变。因此把「已穿戴快照哈希」
		// 纳入判定：集合变了就必须重算，否则套装面板会停在旧件数上。
		const std::vector<int32_t> equipped_snapshot = Stage::get().get_player().get_inventory().equipped_item_ids();

		size_t snapshot_hash = 1469598103934665603ull;

		for (int32_t id : equipped_snapshot)
		{
			snapshot_hash ^= static_cast<size_t>(id);
			snapshot_hash *= 1099511628211ull;
		}

		if (invpos == ivp && snapshot_hash == equip_snapshot_hash)
			return;

		equip_snapshot_hash = snapshot_hash;

		// 背包/已装备槽位路径清掉 itemid 合成源；清除(0)同样收口。
		if (ivp >= 0)
		{
			itemid_cache = 0;
			itemid_equip.reset();
		}

		invpos = ivp;
		invpos_preview = 0;

		// 每次重新悬停都重置成长行状态，避免沿用上一件装备的数据。
		show_growth = false;
		show_growth_preview = false;
		has_trade = false;
		has_trade_preview = false;
		has_owner = false;
		has_owner_preview = false;
		req_shift = REQ_SHIFT_BASE;
		req_shift_preview = REQ_SHIFT_BASE;

		// 192 Summary 箭头：每次重新悬停清空符号缓存，避免沿用上一件装备的箭头方向。
		std::fill(std::begin(scroll_sign), std::end(scroll_sign), int8_t(0));
		std::fill(std::begin(scroll_sign_preview), std::end(scroll_sign_preview), int8_t(0));

		const Player& player = Stage::get().get_player();

		InventoryType::Id invtype;

		switch (parent)
		{
		case Tooltip::Parent::ITEMINVENTORY:
		case Tooltip::Parent::SHOP:
			invtype = InventoryType::Id::EQUIP;
			break;
		case Tooltip::Parent::EQUIPINVENTORY:
			invtype = InventoryType::Id::EQUIPPED;
			break;
		default:
			invtype = InventoryType::Id::NONE;
		}

		const Inventory& inventory = player.get_inventory();
		Optional<const Equip> oequip;

		if (ivp < 0 && itemid_equip)
			oequip = Optional<const Equip>(*itemid_equip);
		else
			oequip = inventory.get_equip(invtype, invpos);

		const CharStats& stats = player.get_stats();

		// itemid 模式与背包装备同口径：可与本机同部位已装备做对比。
		if (show_compare && (invtype == InventoryType::Id::EQUIP || ivp < 0) && oequip)
		{
			const int32_t item_id = oequip.get()->get_item_id();
			const EquipData& equipdata = EquipData::get(item_id);
			EquipSlot::Id eqslot = equipdata.get_eqslot();

			if (inventory.has_equipped(eqslot))
			{
				auto eequip = inventory.get_equip(InventoryType::Id::EQUIPPED, eqslot);

				if (eequip)
				{
					const Equip& equip = *eequip;

					int32_t item_id = equip.get_item_id();

					const EquipData& equipdata = EquipData::get(item_id);
					const ItemData& itemdata = equipdata.get_itemdata();

					const uint8_t preview_itemlevel = equip.get_itemlevel();
					const int16_t preview_itemexp = equip.get_itemexp();

					// 用户抉择：成长两行仅当 itemlevel/itemexp 任一非零时显示。
					show_growth_preview = (preview_itemlevel > 0) || (preview_itemexp > 0);
					req_shift_preview = show_growth_preview ? REQ_SHIFT_GROWTH : REQ_SHIFT_BASE;
					growth_level_str_preview = std::to_string(preview_itemlevel);
					growth_exp_str_preview = std::to_string(preview_itemexp);
					height_preview = HEIGHT_BASE + (req_shift_preview - REQ_SHIFT_LEGACY);

					itemicon_preview = itemdata.get_icon(false);

					for (auto& ms : requirements)
					{
						canequip_preview[ms] = stats.get_stat(ms) >= equipdata.get_reqstat(ms);
						// 需求值按原值输出：0 就是 "0"。旧的 3 位左补零（及
						// reqstr.size() < 3 判空）是 v095 遗风，用户 2026-11-11
						// 明确要求「属性为0就显示0 不要显示000」，且
						// 3 - reqstr.size() 在 size>3 时会无符号下溢。
						reqstatstrings_preview[ms] = std::to_string(equipdata.get_reqstat(ms));
					}

					okjobs_preview.clear();

					// TODO: Remove from UIItemInventory and move into Job?
					switch (equipdata.get_reqstat(MapleStat::Id::JOB))
					{
					case 0:
						okjobs_preview.push_back(0);
						okjobs_preview.push_back(1);
						okjobs_preview.push_back(2);
						okjobs_preview.push_back(3);
						okjobs_preview.push_back(4);
						okjobs_preview.push_back(5);
						canequip_preview[MapleStat::Id::JOB] = true;
						break;
					case 1:
						okjobs_preview.push_back(1);
						canequip_preview[MapleStat::Id::JOB] = (stats.get_stat(MapleStat::Id::JOB) / 100 == 1) || (stats.get_stat(MapleStat::Id::JOB) / 100 >= 20);
						break;
					case 2:
						okjobs_preview.push_back(2);
						canequip_preview[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 2;
						break;
					case 3: // Magician, Warrior
						okjobs_preview.push_back(1);
						okjobs_preview.push_back(2);
						canequip_preview[MapleStat::Id::JOB] = (stats.get_stat(MapleStat::Id::JOB) / 100 == 1) || (stats.get_stat(MapleStat::Id::JOB) / 100 >= 20) || stats.get_stat(MapleStat::Id::JOB) / 100 == 2;
						break;
					case 4:
						okjobs_preview.push_back(3);
						canequip_preview[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 3;
						break;
					case 8:
						okjobs_preview.push_back(4);
						canequip_preview[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 4;
						break;
					case 16:
						okjobs_preview.push_back(5);
						canequip_preview[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 5;
						break;
					default:
						canequip_preview[MapleStat::Id::JOB] = false;
						break;
					}
					// End of TODO

					prank_preview = equip.get_potrank();

					switch (prank_preview)
					{
					case Equip::Potential::POT_HIDDEN:
						potflag_preview = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::RED);
						potflag_preview.change_text(Locale::text("equip.pot.hidden"));
						break;
					case Equip::Potential::POT_RARE:
						potflag_preview = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
						potflag_preview.change_text(Locale::text("equip.pot.rare"));
						break;
					case Equip::Potential::POT_EPIC:
						potflag_preview = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
						potflag_preview.change_text(Locale::text("equip.pot.epic"));
						break;
					case Equip::Potential::POT_UNIQUE:
						potflag_preview = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
						potflag_preview.change_text(Locale::text("equip.pot.unique"));
						break;
					case Equip::Potential::POT_LEGENDARY:
						potflag_preview = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
						potflag_preview.change_text(Locale::text("equip.pot.legendary"));
						break;
					default:
						height_preview -= 16;
						break;
					}

					Color::Name namecolor;

					switch (equip.get_quality())
					{
					case EquipQuality::Id::GREY:
						namecolor = Color::Name::LIGHTGREY;
						break;
					case EquipQuality::Id::ORANGE:
						namecolor = Color::Name::ORANGE;
						break;
					case EquipQuality::Id::BLUE:
						namecolor = Color::Name::MEDIUMBLUE;
						break;
					case EquipQuality::Id::VIOLET:
						namecolor = Color::Name::VIOLET;
						break;
					case EquipQuality::Id::GOLD:
						namecolor = Color::Name::YELLOW;
						break;
					default:
						namecolor = Color::Name::WHITE;
						break;
					}

					std::string namestr = itemdata.get_name();

					// 2026-11-21：无名装备用本地化分类名兜底，不再露裸 ID。
					if (namestr.empty())
						namestr = equip_fallback_name(equipdata, item_id);

					const int8_t reqGender = itemdata.get_gender();

					switch (reqGender)
					{
					case 0: // Male
						namestr += " " + Locale::text("equip.gender.male");
						break;
					case 1: // Female
						namestr += " " + Locale::text("equip.gender.female");
						break;
					case 2: // Unisex
					default:
						break;
					}

					if (equip.get_level() > 0)
					{
						namestr.append(" (+");
						namestr.append(std::to_string(equip.get_level()));
						namestr.append(")");
					}

					// 079 原生 1:1 名称墨高约 16px、粗白字；A12B 实测仅 11px（m00304 ①）。
					name_preview = Text(Text::Font::A14B, Text::Alignment::CENTER, namecolor, namestr, 400);

					// 095 品级副标题（对比窗侧）：镜像主窗，数据源为已装备 equip。
					has_rank_line_preview = false;

					if (equip.get_potrank() != Equip::Potential::POT_NONE)
					{
						const char* rank_key = nullptr;

						switch (equip.get_potrank())
						{
						case Equip::Potential::POT_HIDDEN:    rank_key = "equip.rank.hidden"; break;
						case Equip::Potential::POT_RARE:      rank_key = "equip.rank.rare"; break;
						case Equip::Potential::POT_EPIC:      rank_key = "equip.rank.epic"; break;
						case Equip::Potential::POT_UNIQUE:    rank_key = "equip.rank.unique"; break;
						case Equip::Potential::POT_LEGENDARY: rank_key = "equip.rank.legendary"; break;
						default: break;
						}

						if (rank_key)
						{
							rank_line_preview = Text(Text::Font::A11M, Text::Alignment::CENTER,
								namecolor, Locale::text(rank_key));
							has_rank_line_preview = true;
						}
					}
					// 2026-11-11 用户实拍：右上「攻击力增加」整行移除（历史误接的 equip.label_atkinc）。

					std::string desctext = itemdata.get_desc();
					hasdesc_preview = desctext.size() > 0;

					if (hasdesc_preview)
					{
						// 分隔线以下描述：水平居中（非图标旁前置块）。
						desc_preview = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::TOOLTIPBODY, desctext, 250);
						height_preview += desc_preview.height() + 10;
					}

					// 分隔线以下：冒号中轴居中（center_colon=true）；全部纯白。
					itemid_preview.build(Locale::format("equip.label_itemid", std::to_string(item_id)), Color::Name::WHITE, true);
					category_preview.build(Locale::format("equip.label_type", Locale::enum_text("equip.type.", equipdata.get_type())), Color::Name::WHITE, true);


					{
					const std::string tlab = equip_trade_label(itemdata);
					has_trade_preview = !tlab.empty();
					// 079 原生（m00304 ④）：交易词条在名称区下方独立叠高行，
					// 不再计入分隔线以下属性块高度。
					if (has_trade_preview)
						trade_line_preview.build_centered(tlab, Color::Name::ORANGE);
					}

					// 所有者行（对比窗侧）：数据源为已装备物品 equip（:352），
					// owner 非空才显示，与主窗口径对称。
					{
						const std::string& owner = equip.get_owner();
						has_owner_preview = !owner.empty();
						if (has_owner_preview)
						{
							owner_line_preview.build_centered(Locale::format("equip.owner", owner), Color::Name::ORANGE);
							height_preview += 14;
						}
					}

					is_weapon_preview = equipdata.is_weapon();

					if (is_weapon_preview)
					{
						const WeaponData& weapon = WeaponData::get(item_id);
						wepspeed_preview.build(Locale::format("equip.label_wepspeed", Locale::enum_text("equip.speed.", weapon.getspeedstring())), Color::Name::WHITE, true);
					}
					else
					{
						height_preview -= 18;
					}

					hasslots_preview = (equip.get_slots() > 0) || (equip.get_level() > 0);

					if (hasslots_preview)
					{
						slots_preview.build(Locale::format("equip.label_slots", std::to_string(equip.get_slots())), Color::Name::WHITE, true);

						// 原生只显示次数；旧逻辑在 vicious>1 时拼接「上限」造成「2 上限」。
						hammers_preview.build(
							Locale::format("equip.label_hammers", std::to_string(equip.get_vicious())),
							Color::Name::WHITE, true);
					}
					else
					{
						height_preview -= 36;
					}

					// 197 武器级突破上限（对比窗侧）：镜像主窗 :1276-1290。
					has_breakthrough_line_preview = (equip.get_total_limit_break() > 0);
					if (has_breakthrough_line_preview)
					{
						const int64_t lb_total = equip.get_total_limit_break();
						const int32_t used = equip.get_add_limit_num() > 0 ? 1 : 0;
						breakthrough_line_preview = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW,
							Locale::format_all("equip.breakthrough", { std::to_string(lb_total), std::to_string(used) }),
							FRAME_WIDTH - 20);
						height_preview += LINE_STEP;
					}

					statlabels_preview.clear();
					statdeltas_preview.clear();

					const EquipData& hover_data = EquipData::get(oequip.get()->get_item_id());
					bool hover_blank = true;
					bool hover_has_def = false;
					bool equip_blank = true;
					bool equip_has_def = false;

					for (EquipStat::Id es = EquipStat::Id::STR; es <= EquipStat::Id::JUMP; es = static_cast<EquipStat::Id>(es + 1))
					{
						if (oequip.get()->get_stat(es) > 0)
							hover_blank = false;
						if (hover_data.get_defstat(es) > 0)
							hover_has_def = true;
						if (equip.get_stat(es) > 0)
							equip_blank = false;
						if (equipdata.get_defstat(es) > 0)
							equip_has_def = true;
					}

					const bool hover_use_def = hover_blank && hover_has_def;
					const bool equip_use_def = equip_blank && equip_has_def;

					// 095 潜能行（对比窗侧）：镜像主窗 set_equip 的 :852-888 口径，
					// 数据源为已装备物品 equip（:352），文案走 locale equip.potline.*，
					// 缺键回退带编号占位（Locale::text_or，Util/Locale.h:55-57）。
					{
						int16_t pot_height_preview = 0;

						for (size_t i = 0; i < 6; ++i)
						{
							haspotlines_preview[i] = false;

							int16_t pot_id = (i < 3) ? equip.get_potential(i) : equip.get_bonus_potential(i - 3);

							if (pot_id <= 0)
								continue;

							const std::string key = "equip.potline." + std::to_string(pot_id);
							std::string content = Locale::text_or(key, "潜能 " + std::to_string(pot_id));

							if (i >= 3)
								content = "(B) " + content;

							potlines_preview[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
								content, FRAME_WIDTH - 20);
							haspotlines_preview[i] = true;
							pot_height_preview += LINE_STEP;
						}

						if (pot_height_preview > 0)
							// 095 分组标题行补偿（与主窗对称）。
							if (haspotlines_preview[0] || haspotlines_preview[1] || haspotlines_preview[2])
								pot_height_preview += LINE_STEP;
							if (haspotlines_preview[3] || haspotlines_preview[4] || haspotlines_preview[5])
								pot_height_preview += LINE_STEP;
							height_preview += pot_height_preview + BLOCK_GAP;
					}

					// 095 强化三件套（对比窗侧）：镜像主窗 :890-941，火花/宝珠 WHITE、
					// 星之力 YELLOW；星之力是星数不是词条 id，按「星之力 N ★×」显示。
					{
						int16_t extra_height_preview = 0;

						has_spark_line_preview = false;
						has_soul_line_preview = false;
						has_star_line_preview = false;

						if (int16_t sid = equip.get_spark(); sid > 0)
						{
							const std::string content = Locale::text_or(
								"equip.spark." + std::to_string(sid),
								"火花 " + std::to_string(sid));
							spark_line_preview = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
								content, FRAME_WIDTH - 20);
							has_spark_line_preview = true;
							extra_height_preview += LINE_STEP;
						}

						if (int16_t slid = equip.get_soul(); slid > 0)
						{
							const std::string content = Locale::text_or(
								"equip.soul." + std::to_string(slid),
								"宝珠 " + std::to_string(slid));
							soul_line_preview = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
								content, FRAME_WIDTH - 20);
							has_soul_line_preview = true;
							extra_height_preview += LINE_STEP;
						}

						starforce_preview = equip.get_starforce();
						if (uint8_t sf = starforce_preview; sf > 0)
						{
							std::string stars;
							for (uint8_t i = 0; i < sf && i < 25; ++i)
								stars += "*";
							star_line_preview = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW,
								"星之力 " + std::to_string(sf) + " " + stars, FRAME_WIDTH - 20);
							has_star_line_preview = true;
							extra_height_preview += LINE_STEP;
						}

						if (extra_height_preview > 0)
							height_preview += extra_height_preview + BLOCK_GAP;
					}

					for (EquipStat::Id es = EquipStat::Id::STR; es <= EquipStat::Id::JUMP; es = static_cast<EquipStat::Id>(es + 1))
					{
						const uint16_t equipped_value = equip_use_def
							? equipdata.get_defstat(es) : equip.get_stat(es);
						const uint16_t hovered_value = hover_use_def
							? hover_data.get_defstat(es) : oequip.get()->get_stat(es);

						// 对比窗只列「已装备自身有的属性」；悬停独有属性不在此补 0 行。
						if (equipped_value == 0)
						{
							height_preview -= 18;
							continue;
						}

						// 已装备物品自身的卷轴加成（白）；对比差值另用绿/红接在右侧
						int16_t scroll = static_cast<int16_t>(equipped_value) - equipdata.get_defstat(es);
						std::string statstr = std::to_string(equipped_value);

						if (scroll != 0)
						{
							statstr.append(" (");
							statstr.append((scroll < 0) ? "-" : "+");
							statstr.append(std::to_string(abs(scroll)) + ")");
						}

						statlabels_preview[es].build(Locale::format(Locale::enum_key("equip.stat.", EquipStat::names[es]), statstr), Color::Name::WHITE, true);

						int32_t diff = static_cast<int32_t>(hovered_value) - static_cast<int32_t>(equipped_value);

						if (diff != 0)
						{
							// 192 Summary 箭头方向缓存：供 draw_preview 在差值文本旁叠加 incline/decline。
							scroll_sign_preview[es] = diff > 0 ? 1 : -1;

							std::string diffstr = " (";

							if (diff > 0)
								diffstr += "+";

							diffstr += std::to_string(diff) + ")";

							statdeltas_preview[es] = Text(
								Text::Font::A11M,
								Text::Alignment::LEFT,
								diff > 0 ? Color::Name::LIGHTGREEN : Color::Name::TORCHRED,
								diffstr
							);
						}
					}

					invpos_preview = 1;
				}
			}
		}

		if (!oequip)
			return;

		const Equip& equip = *oequip;

		int32_t item_id = equip.get_item_id();

		// companion：套装 + 成长（成长无数据时 valid=false，自动跳过）。
		// 2026-11-21：把悬停装备的强化等级传给套装面板做成员名 Lv: 前缀。
		settip.set_item(item_id, equipped_snapshot, equip.get_level());
		growthtip.set_item(item_id,
			static_cast<int32_t>(equip.get_itemlevel()),
			static_cast<int32_t>(equip.get_level()));

		const EquipData& equipdata = EquipData::get(item_id);
		const ItemData& itemdata = equipdata.get_itemdata();

		const uint8_t itemlevel = equip.get_itemlevel();
		const int16_t itemexp = equip.get_itemexp();

		show_growth = (itemlevel > 0) || (itemexp > 0);
		req_shift = show_growth ? REQ_SHIFT_GROWTH : REQ_SHIFT_BASE;
		growth_level_str = std::to_string(itemlevel);
		growth_exp_str = std::to_string(itemexp);
		// height 与 draw 的需求块推进量共用同一增量，避免内容越出背板。
		height = HEIGHT_BASE + (req_shift - REQ_SHIFT_LEGACY);

		itemicon = itemdata.get_icon(false);

		for (auto& ms : requirements)
		{
			canequip[ms] = stats.get_stat(ms) >= equipdata.get_reqstat(ms);
			// 同 preview 分支：原值输出，不补零（用户 2026-11-11 第 5 条）。
			reqstatstrings[ms] = std::to_string(equipdata.get_reqstat(ms));
		}

		okjobs.clear();

		switch (equipdata.get_reqstat(MapleStat::Id::JOB))
		{
		case 0: // Common
			okjobs.push_back(0);
			okjobs.push_back(1);
			okjobs.push_back(2);
			okjobs.push_back(3);
			okjobs.push_back(4);
			okjobs.push_back(5);
			canequip[MapleStat::Id::JOB] = true;
			break;
		case 1: // Warrior
			okjobs.push_back(1);
			canequip[MapleStat::Id::JOB] = (stats.get_stat(MapleStat::Id::JOB) / 100 == 1) || (stats.get_stat(MapleStat::Id::JOB) / 100 >= 20);
			break;
		case 2: // Magician
			okjobs.push_back(2);
			canequip[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 2;
			break;
		case 3: // Magician, Warrior
			okjobs.push_back(1);
			okjobs.push_back(2);
			canequip[MapleStat::Id::JOB] = (stats.get_stat(MapleStat::Id::JOB) / 100 == 1) || (stats.get_stat(MapleStat::Id::JOB) / 100 >= 20) || stats.get_stat(MapleStat::Id::JOB) / 100 == 2;
			break;
		case 4: // Bowman
			okjobs.push_back(3);
			canequip[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 3;
			break;
		case 8: // Thief
			okjobs.push_back(4);
			canequip[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 4;
			break;
		case 16: // Pirate
			okjobs.push_back(5);
			canequip[MapleStat::Id::JOB] = stats.get_stat(MapleStat::Id::JOB) / 100 == 5;
			break;
		default:
			canequip[MapleStat::Id::JOB] = false;
			break;
		}

		prank = equip.get_potrank();

		switch (prank)
		{
		case Equip::Potential::POT_HIDDEN:
			potflag = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::RED);
			potflag.change_text(Locale::text("equip.pot.hidden"));
			break;
		case Equip::Potential::POT_RARE:
			potflag = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
			potflag.change_text(Locale::text("equip.pot.rare"));
			break;
		case Equip::Potential::POT_EPIC:
			potflag = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
			potflag.change_text(Locale::text("equip.pot.epic"));
			break;
		case Equip::Potential::POT_UNIQUE:
			potflag = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
			potflag.change_text(Locale::text("equip.pot.unique"));
			break;
		case Equip::Potential::POT_LEGENDARY:
			potflag = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE);
			potflag.change_text(Locale::text("equip.pot.legendary"));
			break;
		default:
			height -= 16;
			break;
		}

		Color::Name namecolor;

		switch (equip.get_quality())
		{
		case EquipQuality::Id::GREY:
			namecolor = Color::Name::LIGHTGREY;
			break;
		case EquipQuality::Id::ORANGE:
			namecolor = Color::Name::ORANGE;
			break;
		case EquipQuality::Id::BLUE:
			namecolor = Color::Name::MEDIUMBLUE;
			break;
		case EquipQuality::Id::VIOLET:
			namecolor = Color::Name::VIOLET;
			break;
		case EquipQuality::Id::GOLD:
			namecolor = Color::Name::YELLOW;
			break;
		default:
			namecolor = Color::Name::WHITE;
			break;
		}

		std::string namestr = itemdata.get_name();

		// 2026-11-21：无名装备用本地化分类名兜底，不再露裸 ID。
		if (namestr.empty())
			namestr = equip_fallback_name(equipdata, item_id);

		const int8_t reqGender = itemdata.get_gender();

		switch (reqGender)
		{
		case 0: // Male
			namestr += " " + Locale::text("equip.gender.male");
			break;
		case 1: // Female
			namestr += " " + Locale::text("equip.gender.female");
			break;
		case 2: // Unisex
		default:
			break;
		}

		if (equip.get_level() > 0)
		{
			namestr.append(" (+");
			namestr.append(std::to_string(equip.get_level()));
			namestr.append(")");
		}

		// 079 原生 1:1 名称墨高约 16px、粗白字；A12B 实测仅 11px（m00304 ①）。
		// A14B 墨高约 15px，是「加大加粗」后的 079 口径。
		name = Text(Text::Font::A14B, Text::Alignment::CENTER, namecolor, namestr, 400);

		// 095 品级副标题（参考图「SS级（传说极品）」）：由潜能等级 get_potrank() 映射，
		// 画在名称下方、交易词条上方；POT_NONE 不显示。颜色跟随名称品质色（namecolor）。
		has_rank_line = false;

		if (equip.get_potrank() != Equip::Potential::POT_NONE)
		{
			const char* rank_key = nullptr;

			switch (equip.get_potrank())
			{
			case Equip::Potential::POT_HIDDEN:    rank_key = "equip.rank.hidden"; break;
			case Equip::Potential::POT_RARE:      rank_key = "equip.rank.rare"; break;
			case Equip::Potential::POT_EPIC:      rank_key = "equip.rank.epic"; break;
			case Equip::Potential::POT_UNIQUE:    rank_key = "equip.rank.unique"; break;
			case Equip::Potential::POT_LEGENDARY: rank_key = "equip.rank.legendary"; break;
			default: break;
			}

			if (rank_key)
			{
				rank_line = Text(Text::Font::A11M, Text::Alignment::CENTER,
					namecolor, Locale::text(rank_key));
				has_rank_line = true;
			}
		}

		// 2026-11-11 用户实拍：右上「攻击力增加」整行移除（历史误接的 equip.label_atkinc）。

		std::string desctext = itemdata.get_desc();
		hasdesc = desctext.size() > 0;

		if (hasdesc)
		{
			desc = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::TOOLTIPBODY, desctext, 250);
			height += desc.height() + 10;
		}

		// 用户裁决：分隔线以下全部冒号中轴居中；inc 行全白，元信息纯白。
		itemid.build(Locale::format("equip.label_itemid", std::to_string(item_id)), Color::Name::WHITE, true);
		category.build(Locale::format("equip.label_type", Locale::enum_text("equip.type.", equipdata.get_type())), Color::Name::WHITE, true);


		{
		const std::string tlab = equip_trade_label(itemdata);
		has_trade = !tlab.empty();
		// 079 原生（m00304 ④）：交易词条在名称区下方独立叠高行，
		// 不再计入分隔线以下属性块高度。
		if (has_trade)
			// 2026-11-20：与 owner 行同口径整串居中，消除两行上下/水平不齐。
			trade_line.build_centered(tlab, Color::Name::ORANGE);
		}

		// 所有者行（095/服务端 owner 字段）：owner 非空才显示，空 owner 不留空行。
		// 与交易行同色（ORANGE），画在名称区下方词条区；多一行补偿高度防止背板截断。
		{
		const std::string& owner = equip.get_owner();
		has_owner = !owner.empty();
		if (has_owner)
		{
			// 2026-11-20：整串居中（含「所有者：」），与 trade 行视觉中心一致。
			owner_line.build_centered(Locale::format("equip.owner", owner), Color::Name::ORANGE);
			height += 14;
		}
		}

		is_weapon = equipdata.is_weapon();

		if (is_weapon)
		{
			const WeaponData& weapon = WeaponData::get(item_id);
			wepspeed.build(Locale::format("equip.label_wepspeed", Locale::enum_text("equip.speed.", weapon.getspeedstring())), Color::Name::WHITE, true);
		}
		else
		{
			height -= 18;
		}

		hasslots = (equip.get_slots() > 0) || (equip.get_level() > 0);

		if (hasslots)
		{
			slots.build(Locale::format("equip.label_slots", std::to_string(equip.get_slots())), Color::Name::WHITE, true);

			// 原生只显示次数；禁止再拼 locale「上限」。
			hammers.build(
				Locale::format("equip.label_hammers", std::to_string(equip.get_vicious())),
				Color::Name::WHITE, true);
		}
		else
		{
			height -= 36;
		}

		// 197 武器级突破上限（方案 C，2026-11-25）：突破卷累计或突破石加成 > 0 才出行，
		// 与 197 tooltip「突破上限武器 / 突破上限 +N / 可突破次数」语义对齐。无突破值不显示，避免误导。
		has_breakthrough_line = (equip.get_total_limit_break() > 0);
		if (has_breakthrough_line)
		{
			const int64_t lb_total = equip.get_total_limit_break();
			const int32_t used = equip.get_add_limit_num() > 0 ? 1 : 0;
			breakthrough_line = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW,
				Locale::format_all("equip.breakthrough", { std::to_string(lb_total), std::to_string(used) }),
				FRAME_WIDTH - 20);
			height += LINE_STEP;
		}

		statlabels.clear();
		statdeltas.clear();

		// 包内属性全 0 但 WZ 有基础值（服务端未填 / 脏数据）时，回退显示 WZ 默认。
		bool hover_stat_blank = true;
		bool hover_has_def = false;

		for (EquipStat::Id es = EquipStat::Id::STR; es <= EquipStat::Id::JUMP; es = static_cast<EquipStat::Id>(es + 1))
		{
			if (equip.get_stat(es) > 0)
				hover_stat_blank = false;
			if (equipdata.get_defstat(es) > 0)
				hover_has_def = true;
		}

		const bool hover_use_def = hover_stat_blank && hover_has_def;

		// 对比开启时取同部位已装备，供悬停 tip 右侧绿/红差值。
		Optional<const Equip> compare_equipped;
		const EquipData* compare_data = nullptr;
		bool compare_use_def = false;

		if (show_compare && invpos_preview == 1)
		{
			EquipSlot::Id eqslot = equipdata.get_eqslot();

			if (inventory.has_equipped(eqslot))
			{
				compare_equipped = inventory.get_equip(InventoryType::Id::EQUIPPED, eqslot);

				if (compare_equipped)
				{
					compare_data = &EquipData::get(compare_equipped.get()->get_item_id());
					bool c_blank = true;
					bool c_has = false;

					for (EquipStat::Id es = EquipStat::Id::STR; es <= EquipStat::Id::JUMP; es = static_cast<EquipStat::Id>(es + 1))
					{
						if (compare_equipped.get()->get_stat(es) > 0)
							c_blank = false;
						if (compare_data->get_defstat(es) > 0)
							c_has = true;
					}

					compare_use_def = c_blank && c_has;
				}
			}
		}

		for (EquipStat::Id es = EquipStat::Id::STR; es <= EquipStat::Id::JUMP; es = static_cast<EquipStat::Id>(es + 1))
		{
			const uint16_t shown = hover_use_def ? equipdata.get_defstat(es) : equip.get_stat(es);

			if (shown > 0)
			{
				int16_t scroll = static_cast<int16_t>(shown) - equipdata.get_defstat(es);
				std::string statstr = std::to_string(shown);

				if (scroll != 0)
				{
					statstr.append(" (");
					statstr.append((scroll < 0) ? "-" : "+");
					statstr.append(std::to_string(abs(scroll)) + ")");
				}

				statlabels[es].build(Locale::format(Locale::enum_key("equip.stat.", EquipStat::names[es]), statstr), Color::Name::WHITE, true);

				if (compare_equipped && compare_data)
				{
					const uint16_t equipped_value = compare_use_def
						? compare_data->get_defstat(es) : compare_equipped.get()->get_stat(es);
					const int32_t diff = static_cast<int32_t>(shown) - static_cast<int32_t>(equipped_value);

					if (diff != 0)
					{
						// 192 Summary 箭头方向缓存：供 draw 在差值文本旁叠加 incline/decline。
						scroll_sign[es] = diff > 0 ? 1 : -1;

						std::string diffstr = " (";

						if (diff > 0)
							diffstr += "+";

						diffstr += std::to_string(diff) + ")";

						statdeltas[es] = Text(
							Text::Font::A11M,
							Text::Alignment::LEFT,
							diff > 0 ? Color::Name::LIGHTGREEN : Color::Name::TORCHRED,
							diffstr);
					}
				}
			}
			else
			{
				height -= 18;
			}
		}

		// ⑦ 095 潜能行（2026-11-12 Round 8）：把 EquipTooltip.h 里存在已久但
		// 从未被赋值的 potlines[5] 真正接上。
		// 数据：Equip::get_potential(0..2) = 3 条主潜能，get_bonus_potential(0..1)
		// = 2 条附加潜能；值语义 = 潜能条目 id（0 = 空）。
		// 文案：live 包内**没有** Potential.img / PotentialOption.img（实测 Etc/
		// 目录无该节点），因此走本地词条表 locale 的 equip.potline.* ，
		// 键 = 潜能 id，缺键时显示「潜能 N」而不是留空——宁可显示编号也不静默。
		{
			int16_t pot_height = 0;

			for (size_t i = 0; i < 6; ++i)
			{
				haspotlines[i] = false;

				int16_t pot_id = (i < 3) ? equip.get_potential(i) : equip.get_bonus_potential(i - 3);

				if (pot_id <= 0)
					continue;

				// 交付口径：主潜能与附加潜能用不同前缀区分（附加潜能行加「(B)」）。
				// Locale::text_or = has ? tr(key) : fallback（Util/Locale.h:55-57），
				// 正是「缺键显示编号、不留空」的语义。
				const std::string key = "equip.potline." + std::to_string(pot_id);
				std::string content = Locale::text_or(key, "潜能 " + std::to_string(pot_id));

				if (i >= 3)
					content = "(B) " + content;

				potlines[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
					content, FRAME_WIDTH - 20);
				haspotlines[i] = true;
				pot_height += LINE_STEP;
			}

			if (pot_height > 0)
			{
				// 095 分组标题行补偿：潜在/附加两组各有一条圆点标题。
				if (haspotlines[0] || haspotlines[1] || haspotlines[2])
					pot_height += LINE_STEP;
				if (haspotlines[3] || haspotlines[4] || haspotlines[5])
					pot_height += LINE_STEP;
				height += pot_height + BLOCK_GAP;
			}
		}

		// ⑦ 095 强化三件套（Round 15）：火花 / 宝珠 / 星之力。
		// 与潜能同一套口径：值 0 = 无 → 不出行；文案走 locale 词条表
		// （live 包内无 AdditionalOption.img / SoulOption.img，实测 Etc/ 无此节点），
		// 缺键用 Locale::text_or 回退成带编号的占位，绝不静默留空。
		{
			int16_t extra_height = 0;

			has_spark_line = false;
			has_soul_line = false;
			has_star_line = false;

			if (int16_t sid = equip.get_spark(); sid > 0)
			{
				const std::string content = Locale::text_or(
					"equip.spark." + std::to_string(sid),
					"火花 " + std::to_string(sid));

				spark_line = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
					content, FRAME_WIDTH - 20);
				has_spark_line = true;
				extra_height += LINE_STEP;
			}

			if (int16_t slid = equip.get_soul(); slid > 0)
			{
				const std::string content = Locale::text_or(
					"equip.soul." + std::to_string(slid),
					"宝珠 " + std::to_string(slid));

				soul_line = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
					content, FRAME_WIDTH - 20);
				has_soul_line = true;
				extra_height += LINE_STEP;
			}

			starforce = equip.get_starforce();
			if (uint8_t sf = starforce; sf > 0)
			{
				// 星之力是「星数」不是词条 id，直接按原生「★ × N」显示。
				std::string stars;

				for (uint8_t i = 0; i < sf && i < 25; ++i)
					stars += "*";

				star_line = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW,
					"星之力 " + std::to_string(sf) + " " + stars, FRAME_WIDTH - 20);
				has_star_line = true;
				extra_height += LINE_STEP;
			}

			if (extra_height > 0)
				height += extra_height + BLOCK_GAP;
		}
	}

	void EquipTooltip::draw_item_icon(Point<int16_t> pos, const Texture& icon) const
	{
		// 底板锚点：名称之后 (16,0)，32x32 再 2x，与需求首行同起点（079/Angel）。
		const Point<int16_t> boxpos = pos + Point<int16_t>(ICON_BOX_X, ICON_BOX_Y);

		// 079 参照帧：底板是半透明浅蓝白（IconBase/0 @ α0.65 叠在 (79,79,119)
		// 背板上 ≈ (193,193,207)，对照取样 (196,196,211)），不是纯白实心块。
		if (iconbase.is_valid())
			iconbase.draw(DrawArgument(boxpos, ICON_BOX_SCALE, ICON_BOX_SCALE, ICON_BOX_ALPHA));

		if (icon.is_valid())
		{
			// Texture::draw 的 pos 对应图标 origin 点：渲染矩形中心 =
			// pos + scale*(w/2 - origin)。让图标中心落在底板中心 (32,32)，须反解
			// pos = boxpos + 32 + origin*scale - (w,h)*scale/2。旧实现把左上角
			// 钉死在 (2,2)，小图标整体偏左上（2026-11-15 用户实拍打回）。
			const Point<int16_t> ivorg = icon.get_origin();
			const int16_t half = static_cast<int16_t>(ICON_BOX_SIZE * ICON_BOX_SCALE / 2);
			const int16_t hw = static_cast<int16_t>(icon.width() * ICON_BOX_SCALE / 2);
			const int16_t hh = static_cast<int16_t>(icon.height() * ICON_BOX_SCALE / 2);
			const Point<int16_t> iconpos(
				boxpos.x() + half + static_cast<int16_t>(ivorg.x() * ICON_BOX_SCALE) - hw,
				boxpos.y() + half + static_cast<int16_t>(ivorg.y() * ICON_BOX_SCALE) - hh
			);

			icon.draw(DrawArgument(iconpos, ICON_BOX_SCALE, ICON_BOX_SCALE));
		}
	}

	void EquipTooltip::draw_grade_badge(Point<int16_t> pos, Equip::Potential rank) const
	{
		// 095/192 品级角标：按潜能等级画在道具图标框右上角。
		// 索引与 Equip::Potential 枚举顺序一致（NONE=0..LEGENDARY=5）。
		const int idx = static_cast<int>(rank);

		if (idx < 0 || idx >= 6 || !grade_tex[idx].is_valid())
			return;

		// 图标框 64x64（32*2x），角标锚在框右上：小角标贴角内，大角标略外扩。
		const Point<int16_t> boxpos = pos + Point<int16_t>(ICON_BOX_X, ICON_BOX_Y);
		const int16_t badge_x = boxpos.x() + ICON_BOX_SIZE * ICON_BOX_SCALE - grade_tex[idx].width() + 2;
		const int16_t badge_y = boxpos.y() - 2;

		grade_tex[idx].draw(Point<int16_t>(badge_x, badge_y));
	}

	void EquipTooltip::draw_job_line(Point<int16_t> pos, const std::vector<uint8_t>& allowedjobs) const
	{
		// 079 原生 / Angel：职业行恒为 6 张职业名位图并排，不因 reqJOB 裁剪。
		// 该职业在允许集合里 -> Can（白）；否则 -> Cannot（红，实测 204,17,34）。
		// m00754 ⑦ 连打三回的真因：用户要的是「成员之间空隙一致」，不是
		// 「墨迹左沿等距」。orange-wz 实读 live Can/Cannot 六位图墨盒完全一致：
		// 透明左留白 {0,9,1,2,0,3}、墨宽 {23,23,35,34,23,23}。R4 让墨沿按
		// JOB_STEP=42 等距 ⇒ 空隙 {19,19,7,8,19,19} 忽大忽小，实拍 m00749
		// {20,20,7,14,19} 与推算吻合。现按「上一张墨迹右沿 + 固定 JOB_GAP
		// = 下一张墨迹左沿」累计，空隙恒 11px（079 原生逐像素反算 gap≈11），
		// drawx 再减去该位图 leftpad 把墨沿钉到累计点。
		static constexpr int16_t JOB_INK_PAD[6] = { 0, 9, 1, 2, 0, 3 };
		static constexpr int16_t JOB_INK_W[6] = { 23, 23, 35, 34, 23, 23 };
		static constexpr int16_t JOB_GAP = 11;
		// m00877 ①：空隙恒 11 之外，左右边距也必须相等 —— 墨迹总宽
		// 216px 在 FRAME_WIDTH 内两侧均分起始点，而非贴 JOB_SIDE=4 起画
		// （否则左窄右宽，用户一眼看出不对称）。
		constexpr int16_t total_ink = 23 + 23 + 35 + 34 + 23 + 23 + 5 * JOB_GAP;
		int16_t ink_x = static_cast<int16_t>((FRAME_WIDTH - total_ink) / 2);

		for (uint8_t i = 0; i < 6; ++i)
		{
			bool allowed = false;

			for (uint8_t j : allowedjobs)
			{
				if (j == i)
				{
					allowed = true;
					break;
				}
			}

			const int16_t drawx = static_cast<int16_t>(ink_x - JOB_INK_PAD[i]);
			const Texture& tex = jobs[allowed].at(i);
			tex.draw(pos + Point<int16_t>(drawx, 0));

			ink_x = static_cast<int16_t>(ink_x + JOB_INK_W[i] + JOB_GAP);
		}
	}

	int16_t EquipTooltip::draw_stars(Point<int16_t> pos, uint8_t sf_value) const
	{
		// 095 星之力：名称上方一整排星。满格 STAR_SLOTS=10，前 sf 颗黄（激活），
		// 其余灰（未激活占位）；sf=0 不画。两态均为 13x13 原生星缩放绘制。
		constexpr uint8_t STAR_SLOTS = 10;
		constexpr int16_t STAR_STEP = 15;   // 星间距（含 2px 间隙）
		constexpr int16_t STAR_W = 13;
		constexpr int16_t STAR_H = 13;

		if (sf_value == 0)
			return 0;

		uint8_t sf = sf_value > STAR_SLOTS ? STAR_SLOTS : sf_value;

		// ===== 192 星形 canvas 优先（UiAssetResolve::tooltip()）=====
		// Item/Equip/Star/Star0/Star(亮) 与 Star1/Star(暗) 是带 origin 的 canvas。
		// 用它替代内嵌 BGRA 常量。两态任一缺失即整段回退到下方 095 像素星逻辑，
		// 绝不因新资源缺失导致星区空白或崩溃。
		const bool has192 = starTex[0] && starTex[1];

		if (has192)
		{
			// 画布宽高/origin 直接取自 canvas（192 星 11×10，非 095 的 13×13）。
			const Point<int16_t> dim = starTex[0].get_dimensions();
			const int16_t tex_w = dim.x();
			const int16_t tex_h = dim.y();
			const int16_t step = static_cast<int16_t>(tex_w + 2);   // 与 095 一致的 2px 间隙
			const int16_t total_w = static_cast<int16_t>((STAR_SLOTS - 1) * step + tex_w);
			const int16_t start_x = static_cast<int16_t>((FRAME_WIDTH - total_w) / 2);

			for (uint8_t i = 0; i < STAR_SLOTS; ++i)
			{
				const Texture& tex = starTex[i < sf ? 0 : 1];   // 前 sf 颗亮，其余暗
				const Point<int16_t> sp = pos + Point<int16_t>(start_x + i * step, 0);

				// canvas 有自身 origin：draw 的 pos 就是 origin 落点，按原点对齐即可
				// （与 Texture::draw 的 origin 语义一致，见 Graphics/Texture.cpp）。
				if (tex)
					tex.draw(sp);
			}

			return tex_h;
		}

		// ===== 095 回退：内嵌 BGRA 像素星（EquipStarPixels.h）=====
		if (!star_yellow.is_valid() || !star_gray.is_valid())
			return 0;

		int16_t total_w = static_cast<int16_t>((STAR_SLOTS - 1) * STAR_STEP + STAR_W);
		int16_t start_x = static_cast<int16_t>((FRAME_WIDTH - total_w) / 2);

		for (uint8_t i = 0; i < STAR_SLOTS; ++i)
		{
			Point<int16_t> sp = pos + Point<int16_t>(start_x + i * STAR_STEP, 0);
			if (i < sf)
				star_yellow.draw(sp, STAR_W, STAR_H);
			else
				star_gray.draw(sp, STAR_W, STAR_H);
		}

		return STAR_H;
	}

	void EquipTooltip::draw_pot_grade_icon(Point<int16_t> linepos, int16_t text_w,
		Equip::Potential rank) const
	{
		// 192 潜能品级小图标（Item/Equip/AddOption/0..5）叠加在潜能文本行末尾。
		// rank 与索引一一对应（NONE=0..LEGENDARY=5，见 Equip::Potential）。
		// NONE 不画；纹理缺失静默跳过，文本回退逻辑完全不变。
		const int idx = static_cast<int>(rank);

		if (idx <= 0 || idx >= 6)
			return;

		const Texture& tex = addOptionTex[idx];

		if (!tex)
			return;

		// 行首原点 + 文本墨宽 + 4px 间隙；y 用纹理自身 origin 对齐文本行基线区。
		const Point<int16_t> ipos(linepos.x() + text_w + 4, linepos.y());
		tex.draw(ipos);
	}

	void EquipTooltip::draw_summary_arrow(Point<int16_t> textpos, int16_t text_w,
		int8_t sign) const
	{
		// 192 Summary 差值箭头叠加在文本 "( +N )/( -N )" 之后：
		// sign>0 → incline（上箭头/增），sign<0 → decline（下箭头/减）；0 不画。
		// 纹理缺失静默跳过，文本本身照常显示（保守增强）。
		const Texture* tex = nullptr;

		if (sign > 0)
			tex = &summaryInclineTex;
		else if (sign < 0)
			tex = &summaryDeclineTex;

		if (!tex || !(*tex))
			return;

		const Point<int16_t> apos(textpos.x() + text_w + 2, textpos.y());
		tex->draw(apos);
	}

	void EquipTooltip::draw(Point<int16_t> pos) const
	{
		if (invpos == 0)
			return;

		// 2026-11-20 用户实拍「几个 tooltip 顶部不在一条水平线」根因：
		// 主窗/套装/成长/对比四片各自从「被 shift_y 污染后的 pos」推顶，且 height
		// 是 set_equip 的另一套启发式累加 → 顶沿随道具内容漂移。
		// 统一以「夹取后的入口 pos」为唯一锚点 base_pos：所有 companion 面板顶 =
		// base_pos.y()，主窗背板顶 = base_pos.y()-6，四窗顶沿严格同一水平线。
		// （base_pos 在下方夹取完成后赋值。）

		int16_t max_width = Constants::Constants::get().get_viewwidth();
		int16_t max_height = Constants::Constants::get().get_viewheight();
		// 主窗背板右缘 = X + FRAME_WIDTH + 6（夹取再多留 6px 余量）。
		int16_t cur_width = pos.x() + FRAME_WIDTH + 12;
		int16_t cur_height = pos.y() + 36;

		// 布局 [悬停][套装?][成长?][对比]：右界取最右一窗。
		if (settip.valid())
			cur_width = pos.x() + set_panel_offset() + SetTooltip::FRAME_WIDTH + 12;

		if (growthtip.valid())
			cur_width = pos.x() + growth_panel_offset() + GrowthTooltip::FRAME_WIDTH + 12;

		if (invpos_preview == 1)
			cur_width = pos.x() + set_aware_preview_offset() + FRAME_WIDTH + 12;

		if (invpos_preview == 1)
			cur_height += (height > height_preview) ? height : height_preview;
		else
			cur_height += height;

		if (settip.valid() && settip.height() > cur_height - (pos.y() + 36))
			cur_height = pos.y() + 36 + settip.height();

		if (growthtip.valid() && growthtip.height() > cur_height - (pos.y() + 36))
			cur_height = pos.y() + 36 + growthtip.height();

		int16_t adj_x = cur_width - max_width;
		int16_t adj_y = cur_height - max_height;

		if (adj_x > 0)
			pos.shift_x(adj_x * -1);

		if (adj_y > 0)
			pos.shift_y(adj_y * -1);

		// 夹取会平移整组窗口；锚点必须取夹取后的 pos，否则 companion 面板
		// 会在靠右/靠下时与主窗脱节（旧 pos.y()-height 同样没跟夹取同步）。
		const Point<int16_t> base_pos = pos;

		// 对比窗用同一锚点起画（内部不再单独夹取），保证四窗顶沿一致。
		// 右向偏移仍在 draw_preview 内部按 set_aware_preview_offset 处理。
		draw_preview(pos);

		// 2026-11-20 用户实拍「底部空白有时长有时短」根因：
		// 背板高度用 set_equip 的启发式 height（混 -18/-36/+14/desc.height()+10），
		// 与实际绘制的 shift_y 总和不相等 → 内容越短空白越大。
		// 改为「绘制前先算真实内容底」：先按 draw 的同一条 shift 链把 pos 推到底，
		// 得到 content_bottom，再回头画背板（y 仍从 base_pos 起，高度由内容底决定）。
		// 由于下方绘制链会原地修改 pos，这里先用一个副本推算出高度。
		Point<int16_t> measure = pos;
		{
			int16_t sub_y = SUBLINE_FIRST;
			if (prank != Equip::Potential::POT_NONE) sub_y += SUBLINE_STEP;
			if (has_rank_line) sub_y += SUBLINE_STEP;
			if (has_trade) sub_y += SUBLINE_STEP;
			if (has_owner) sub_y += SUBLINE_STEP;
			measure.shift_y((sub_y > HEADER_SHIFT) ? sub_y : HEADER_SHIFT);
			measure.shift_y(req_shift);
			measure.shift_y(37); // 职业行/分隔线
			measure.shift_y(LINE_STEP * 2); // itemid/category 两行
			if (is_weapon) measure.shift_y(LINE_STEP);
			for (EquipStat::Id es = EquipStat::Id::STR; es <= EquipStat::Id::JUMP;
				es = static_cast<EquipStat::Id>(es + 1))
				if (!statlabels[es].empty()) measure.shift_y(LINE_STEP);
			if (hasslots) { measure.shift_y(LINE_STEP); measure.shift_y(LINE_STEP); }
			bool anypot = false;
			for (bool h : haspotlines) anypot = anypot || h;
			if (anypot)
			{
				measure.shift_y(BLOCK_GAP);
				if (haspotlines[0] || haspotlines[1] || haspotlines[2])
					measure.shift_y(LINE_STEP);
				if (haspotlines[3] || haspotlines[4] || haspotlines[5])
					measure.shift_y(LINE_STEP);
				for (bool h : haspotlines) if (h) measure.shift_y(LINE_STEP);
			}
			if (has_spark_line || has_soul_line || has_star_line)
			{
				measure.shift_y(BLOCK_GAP);
				if (has_spark_line) measure.shift_y(LINE_STEP);
				if (has_soul_line) measure.shift_y(LINE_STEP);
				if (has_star_line) measure.shift_y(LINE_STEP);
			}
			if (hasdesc)
			{
				measure.shift_y(DESC_GAP);
				// desc 是最后一行，draw 后不再 shift，必须显式加上它的墨高，
				// 否则背板会截断底部描述（这正是「底部空白时长时短」的另一半根因）。
				measure.shift_y(desc.height() + 3);
			}
		}
		// 内容从 base_pos.y() 起到 measure.y() 止；背板顶沿 -6、底沿 +6，
		// 故背板高度 = 内容跨度 + 12，底边恒为内容底 +6，与词条多少无关。
		const int16_t content_extent = static_cast<int16_t>(measure.y() - base_pos.y());
		backdrop.draw_equip(base_pos + Point<int16_t>(-6, -6),
			FRAME_WIDTH + 12, content_extent + 12);

		// 095 星之力：名称上方一整排星（黄=激活/灰=未激活），画在背板顶沿。
		draw_stars(pos + Point<int16_t>(0, STAR_TOP), starforce);

		name.draw(pos + Point<int16_t>(BODY_CENTER_X, NAME_Y));

		// 名称区隐藏词条：潜能等级、交易限制（唯一道具 / 不可交易）逐行叠高，
		// 都画在名称下方、图标上方，不侵占分隔线以下属性区（m00304 ②④）。
		int16_t sub_y = SUBLINE_FIRST;

		if (prank != Equip::Potential::POT_NONE)
		{
			potflag.draw(pos + Point<int16_t>(BODY_CENTER_X, sub_y));
			sub_y += SUBLINE_STEP;
		}

		// 095 品级副标题（参考图「SS级（传说极品）」）：名称区第二行。
		if (has_rank_line)
		{
			rank_line.draw(pos + Point<int16_t>(BODY_CENTER_X, sub_y));
			sub_y += SUBLINE_STEP;
		}

		if (has_trade)
		{
			trade_line.draw(pos + Point<int16_t>(0, sub_y));
			sub_y += SUBLINE_STEP;
		}

		// 所有者行：与交易行同区叠高；owner 为空时 has_owner=false 不占行。
		if (has_owner)
		{
			owner_line.draw(pos + Point<int16_t>(0, sub_y));
			sub_y += SUBLINE_STEP;
		}

		// 词条相互间距与末行到需求块间距统一为 SUBLINE_STEP（18px），同类词条保持等距：
		// 2 词条时 sub_y=60>52 下压，owner(y42, 墨底≈54)到需求首行(y60)留白 6px，
		// 消除旧 14px 步长下 owner(y38)墨底紧贴需求(y52)的叠字；
		// 无词条/单词条 sub_y≤42 仍走 HEADER_SHIFT，视觉零变化。
		pos.shift_y((sub_y > HEADER_SHIFT) ? sub_y : HEADER_SHIFT);

		draw_item_icon(pos, itemicon);
		draw_grade_badge(pos, prank);

		// 需求块：单列 12px，6 条固定需求 + 可选成长 2 行（图标右侧前置区，Can/Cannot 色保留）。
		for (MapleStat::Id ms : requirements)
		{
			Point<int16_t> reqpos = reqstatpositions[ms];
			bool reqok = canequip[ms];
			reqstattextures[ms][reqok].draw(pos + reqpos);

			Point<int16_t> valuepos = pos + reqpos + Point<int16_t>(REQ_VALUE_DX, REQ_VALUE_DY);

			if (ms == MapleStat::Id::FAME)
			{
				// 原生「要求人气」值恒为短横（Can/none 5x1），不画 000。
				fame_none.draw(valuepos);
			}
			else if (ms == MapleStat::Id::LEVEL)
			{
				lvset[reqok].draw(reqstatstrings[ms], 6, valuepos);
			}
			else
			{
				reqset[reqok].draw(reqstatstrings[ms], 6, valuepos);
			}
		}

		if (show_growth)
		{
			Point<int16_t> levpos = pos + Point<int16_t>(REQ_LABEL_X, GROWTH_LEV_Y);
			Point<int16_t> exppos = pos + Point<int16_t>(REQ_LABEL_X, GROWTH_EXP_Y);

			growth_lev_label.draw(levpos);
			growth_exp_label.draw(exppos);

			growthset[true].draw(growth_level_str, 6, levpos + Point<int16_t>(REQ_VALUE_DX, REQ_VALUE_DY));

			int16_t expw = growthset[true].draw(growth_exp_str, 6, exppos + Point<int16_t>(REQ_VALUE_DX, REQ_VALUE_DY));
			growth_percent.draw(exppos + Point<int16_t>(REQ_VALUE_DX + expw + 2, REQ_VALUE_DY));
		}

		pos.shift_y(req_shift);

		Point<int16_t> job_position(pos + Point<int16_t>(0, 14));

		// 职业行恒画 6 张，逐职业 Can(白)/Cannot(红)；draw_job_line 内按
		// 「墨迹右沿 + JOB_GAP」累计排布，成员间空隙恒 11px（m00754 ⑦）。
		draw_job_line(job_position, okjobs);

		// 079 原生 1:1：职业墨团 y219-229 到分隔线 y237 仅 8px；
		// 旧 47 留白 20px（m00304 ⑤「上下间距过大」），收到 35。
		separator.draw(pos + Point<int16_t>(0, 35));

		pos.shift_y(37);

		// 分隔线以下（用户 2026-11-12）：整区水平居中；inc 行冒号中轴+全白；
		// 元信息冒号中轴+纯白；交易限制橙字居中。前置需求/职业块在上方不改。
		// q1（2026-11-20）：行距统一走 LINE_STEP，与对比窗共用同一常量。
		const int16_t stat_y = LINE_STEP;

		itemid.draw(pos);
		pos.shift_y(stat_y);

		category.draw(pos);
		pos.shift_y(stat_y);

		// 079 原生（m00304 ④）：交易词条已移到名称区下方，此处不再占属性行。

		if (is_weapon)
		{
			wepspeed.draw(pos);
			pos.shift_y(stat_y);
		}

		for (EquipStat::Id es = EquipStat::Id::STR; es <= EquipStat::Id::JUMP; es = static_cast<EquipStat::Id>(es + 1))
		{
			const BodyLine& line = statlabels[es];

			if (line.empty())
				continue;

			int16_t right = line.draw(pos);
			const Text& diff = statdeltas[es];

			if (!diff.empty())
				// 2026-11-21 用户实拍「对比差值画到 tooltip 外面」根因：
				// BodyLine::draw 返回的 right 是相对 tip 的 x（0..FRAME_WIDTH），
				// 旧代码把它当绝对屏幕坐标画 → 差值跑到屏幕左侧（红框处）。
				// 必须加 pos.x() 才是屏幕坐标。
				diff.draw(Point<int16_t>(pos.x() + right, pos.y()));

			// 192 Summary 箭头增强：在差值文本 "( +N )" 尾部叠加 incline/decline。
			// 纹理缺失或 sign=0 时 draw_summary_arrow 静默跳过；文本保留不受影响。
			if (scroll_sign[es] != 0)
				draw_summary_arrow(
					Point<int16_t>(pos.x() + right, pos.y()), diff.width(), scroll_sign[es]);

			pos.shift_y(stat_y);
		}

		if (hasslots)
		{
			slots.draw(pos);
			pos.shift_y(stat_y);
			hammers.draw(pos);
			pos.shift_y(stat_y);
		}

		// 197 武器突破上限行：紧跟金锤子之后，与 height 里的 LINE_STEP 补偿一一对应。
		if (has_breakthrough_line)
		{
			breakthrough_line.draw(DrawArgument(pos));
			pos.shift_y(stat_y);
		}

		// ⑦ 095 潜能行：画在「可升级次数/金锤子」之后、「描述」之前，
		// 顺序与原生 tip 的「附加属性块」位置一致。
		{
			bool any = false;

			for (bool h : haspotlines)
				any = any || h;

			if (any)
			{
				pos.shift_y(BLOCK_GAP);

				// 095 布局：潜在属性(主 i=0..2)与附加潜能(i=3..4)各带圆点标题行。
				bool has_main = haspotlines[0] || haspotlines[1] || haspotlines[2];
				bool has_bonus = haspotlines[3] || haspotlines[4] || haspotlines[5];

				if (has_main)
				{
					pot_title.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
					pos.shift_y(LINE_STEP);

					for (size_t i = 0; i < 3; ++i)
					{
						if (!haspotlines[i])
							continue;

						potlines[i].draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
						// 192 潜能品级小图标增强：接在文本行尾，纹理缺失/无等级时静默跳过。
						draw_pot_grade_icon(
							pos + Point<int16_t>(BODY_LEFT_X, 0), potlines[i].width(), prank);
						pos.shift_y(LINE_STEP);
					}
				}

				if (has_bonus)
				{
					bonus_title.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
					pos.shift_y(LINE_STEP);

					for (size_t i = 3; i < 6; ++i)
					{
						if (!haspotlines[i])
							continue;

						potlines[i].draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
						// 192 潜能品级小图标增强：接在文本行尾，纹理缺失/无等级时静默跳过。
						draw_pot_grade_icon(
							pos + Point<int16_t>(BODY_LEFT_X, 0), potlines[i].width(), prank);
						pos.shift_y(LINE_STEP);
					}
				}
			}
		}

		// ⑦ 095 强化三件套（Round 15）：紧接潜能行之后，缺项跳过。
		if (has_spark_line || has_soul_line || has_star_line)
		{
			pos.shift_y(BLOCK_GAP);

			if (has_spark_line)
			{
				spark_line.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
				pos.shift_y(LINE_STEP);
			}

			if (has_soul_line)
			{
				soul_line.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
				pos.shift_y(LINE_STEP);
			}

			if (has_star_line)
			{
				star_line.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
				pos.shift_y(LINE_STEP);
			}
		}

		if (hasdesc)
		{
			pos.shift_y(DESC_GAP);
			separator.draw(pos);
			desc.draw(pos + Point<int16_t>(BODY_CENTER_X, 3));
		}

		// companion：[悬停][套装][成长]；对比窗由 draw_preview 按 set_aware_preview_offset 排布。
		// 2026-11-20：顶沿改用统一锚点 base_pos.y()，与主窗背板顶 (base_pos.y-6)
		// 严格对齐；旧的 pos.y()-height 依赖两套独立累加，必然逐道具漂移。
		const int16_t tip_top = base_pos.y();

		if (settip.valid())
		{
			Point<int16_t> setpos(base_pos.x() + set_panel_offset(), tip_top);

			if (setpos.x() + SetTooltip::FRAME_WIDTH > max_width)
				setpos = Point<int16_t>(base_pos.x() - SetTooltip::FRAME_WIDTH - 12, tip_top);

			settip.draw(setpos);
		}

		if (growthtip.valid())
		{
			Point<int16_t> gpos(base_pos.x() + growth_panel_offset(), tip_top);

			if (gpos.x() + GrowthTooltip::FRAME_WIDTH > max_width)
			{
				const int16_t left = settip.valid()
					? (base_pos.x() - SetTooltip::FRAME_WIDTH - GrowthTooltip::FRAME_WIDTH - 24)
					: (base_pos.x() - GrowthTooltip::FRAME_WIDTH - 12);
				gpos = Point<int16_t>(left, tip_top);
			}

			growthtip.draw(gpos);
		}
	}

	void EquipTooltip::draw_preview(Point<int16_t> pos) const
	{
		if (invpos_preview == 0)
			return;

		// 2026-11-20：垂直夹取上移到 draw() 统一处理（含主窗/套装/成长/对比的
		// 最大内容底），这里不再单独按 height_preview 夹 y —— 两窗各夹各的正是
		// 「几个 tooltip 顶部不在一条水平线」的成因之一。
		int16_t max_width = Constants::Constants::get().get_viewwidth();
		int16_t preview_off = set_aware_preview_offset();
		int16_t cur_width = pos.x() + preview_off + FRAME_WIDTH + 12;

		int16_t adj_x = cur_width - max_width;

		if (adj_x > 0)
			pos.shift_x(adj_x * -1);

		// 2026-11-17 [悬停][套装][对比]：对比窗从套装窗右侧起排。
		pos.shift_x(preview_off);

		// 2026-11-20：与主窗同口径的真实内容高度测算，消除底部空白时长时短。
		const Point<int16_t> preview_base = pos;
		Point<int16_t> pmeasure = pos;
		{
			int16_t sub_y = SUBLINE_FIRST;
			if (prank_preview != Equip::Potential::POT_NONE) sub_y += SUBLINE_STEP;
			if (has_rank_line_preview) sub_y += SUBLINE_STEP;
			if (has_trade_preview) sub_y += SUBLINE_STEP;
			if (has_owner_preview) sub_y += SUBLINE_STEP;
			pmeasure.shift_y((sub_y > HEADER_SHIFT) ? sub_y : HEADER_SHIFT);
			pmeasure.shift_y(req_shift_preview);
			pmeasure.shift_y(37);
			pmeasure.shift_y(LINE_STEP * 2);
			if (is_weapon_preview) pmeasure.shift_y(LINE_STEP);
			for (EquipStat::Id es = EquipStat::Id::STR; es <= EquipStat::Id::JUMP;
				es = static_cast<EquipStat::Id>(es + 1))
				if (!statlabels_preview[es].empty()) pmeasure.shift_y(LINE_STEP);
			if (hasslots_preview) { pmeasure.shift_y(LINE_STEP); pmeasure.shift_y(LINE_STEP); }
			bool anypot = false;
			for (bool h : haspotlines_preview) anypot = anypot || h;
			if (anypot)
			{
				pmeasure.shift_y(BLOCK_GAP);
				if (haspotlines_preview[0] || haspotlines_preview[1] || haspotlines_preview[2])
					pmeasure.shift_y(LINE_STEP);
				if (haspotlines_preview[3] || haspotlines_preview[4] || haspotlines_preview[5])
					pmeasure.shift_y(LINE_STEP);
				for (bool h : haspotlines_preview) if (h) pmeasure.shift_y(LINE_STEP);
			}
			if (has_spark_line_preview || has_soul_line_preview || has_star_line_preview)
			{
				pmeasure.shift_y(BLOCK_GAP);
				if (has_spark_line_preview) pmeasure.shift_y(LINE_STEP);
				if (has_soul_line_preview) pmeasure.shift_y(LINE_STEP);
				if (has_star_line_preview) pmeasure.shift_y(LINE_STEP);
			}
			if (hasdesc_preview)
			{
				// 预览侧 desc 实际是 shift_y(-4) 后 separator + 文本 draw 在 +8。
				pmeasure.shift_y(-4);
				pmeasure.shift_y(desc_preview.height() + 8);
			}
		}
		const int16_t preview_extent = static_cast<int16_t>(pmeasure.y() - preview_base.y());

		// 装备 tooltip 预览层同带织纹背板（背板顶沿 -6，高度 = 内容跨度 +12）。
		backdrop.draw_equip(preview_base + Point<int16_t>(-6, -6),
			FRAME_WIDTH + 12, preview_extent + 12);

		// 095 星之力（对比窗侧）：已装备物品星数，画在背板顶沿。
		draw_stars(pos + Point<int16_t>(0, STAR_TOP), starforce_preview);

		name_preview.draw(pos + Point<int16_t>(BODY_CENTER_X, NAME_Y));

		// 名称区隐藏词条：潜能等级、交易限制逐行叠高（m00304 ②④）。
		int16_t sub_y = SUBLINE_FIRST;

		if (prank_preview != Equip::Potential::POT_NONE)
		{
			potflag_preview.draw(pos + Point<int16_t>(BODY_CENTER_X, sub_y));
			sub_y += SUBLINE_STEP;
		}

		// 095 品级副标题（对比窗侧）：镜像主窗，名称区第二行。
		if (has_rank_line_preview)
		{
			rank_line_preview.draw(pos + Point<int16_t>(BODY_CENTER_X, sub_y));
			sub_y += SUBLINE_STEP;
		}

		if (has_trade_preview)
		{
			trade_line_preview.draw(pos + Point<int16_t>(0, sub_y));
			sub_y += SUBLINE_STEP;
		}

		// 所有者行（对比窗侧）：与交易行同区叠高，owner 为空不占行。
		if (has_owner_preview)
		{
			owner_line_preview.draw(pos + Point<int16_t>(0, sub_y));
			sub_y += SUBLINE_STEP;
		}

		// 与主窗同一 SUBLINE_STEP：末行到需求块、词条相互间距完全一致。
		pos.shift_y((sub_y > HEADER_SHIFT) ? sub_y : HEADER_SHIFT);

		// 2026-11-11：攻击力增加行（equip.label_atkinc）属历史误接，原生 tip 无此行，
		// 已整段移除。图标与主窗体共用 draw_item_icon（IconBase 位图底板 2x + 中心对齐）。
		draw_item_icon(pos, itemicon_preview);
		draw_grade_badge(pos, prank_preview);

		for (MapleStat::Id ms : requirements)
		{
			Point<int16_t> reqpos = reqstatpositions[ms];
			bool reqok = canequip_preview[ms];
			reqstattextures[ms][reqok].draw(pos + reqpos);

			Point<int16_t> valuepos = pos + reqpos + Point<int16_t>(REQ_VALUE_DX, REQ_VALUE_DY);

			if (ms == MapleStat::Id::FAME)
			{
				fame_none.draw(valuepos);
			}
			else if (ms == MapleStat::Id::LEVEL)
			{
				lvset[reqok].draw(reqstatstrings_preview[ms], 6, valuepos);
			}
			else
			{
				reqset[reqok].draw(reqstatstrings_preview[ms], 6, valuepos);
			}
		}

		if (show_growth_preview)
		{
			Point<int16_t> levpos = pos + Point<int16_t>(REQ_LABEL_X, GROWTH_LEV_Y);
			Point<int16_t> exppos = pos + Point<int16_t>(REQ_LABEL_X, GROWTH_EXP_Y);

			growth_lev_label.draw(levpos);
			growth_exp_label.draw(exppos);

			growthset[true].draw(growth_level_str_preview, 6, levpos + Point<int16_t>(REQ_VALUE_DX, REQ_VALUE_DY));

			int16_t expw = growthset[true].draw(growth_exp_str_preview, 6, exppos + Point<int16_t>(REQ_VALUE_DX, REQ_VALUE_DY));
			growth_percent.draw(exppos + Point<int16_t>(REQ_VALUE_DX + expw + 2, REQ_VALUE_DY));
		}

		pos.shift_y(req_shift_preview);

		Point<int16_t> job_position(pos + Point<int16_t>(0, 14));

		// 与主窗体同一处：恒画 6 张、逐职业白/红双态、成员间空隙恒 11px。
		draw_job_line(job_position, okjobs_preview);

		// 079 原生 1:1：职业墨团 y219-229 到分隔线 y237 仅 8px；
		// 旧 47 留白 20px（m00304 ⑤「上下间距过大」），收到 35。
		separator.draw(pos + Point<int16_t>(0, 35));

		pos.shift_y(37);

		const int16_t stat_y = LINE_STEP;

		itemid_preview.draw(pos);
		pos.shift_y(stat_y);

		category_preview.draw(pos);
		pos.shift_y(stat_y);

		// 079 原生（m00304 ④）：交易词条已移到名称区下方，此处不再占属性行。

		if (is_weapon_preview)
		{
			wepspeed_preview.draw(pos);
			pos.shift_y(stat_y);
		}

		for (EquipStat::Id es = EquipStat::Id::STR; es <= EquipStat::Id::JUMP; es = static_cast<EquipStat::Id>(es + 1))
		{
			const BodyLine& line = statlabels_preview[es];

			if (line.empty())
				continue;

			int16_t right = line.draw(pos);

			// 差值紧跟居中后的属性文本右缘（2026-11-21：right 是相对坐标，须加 pos.x()）。
			const Text& diff = statdeltas_preview[es];

			if (!diff.empty())
				diff.draw(Point<int16_t>(pos.x() + right, pos.y()));

			// 192 Summary 箭头增强（对比窗侧）：镜像主窗，在差值文本尾部叠加箭头。
			if (scroll_sign_preview[es] != 0)
				draw_summary_arrow(
					Point<int16_t>(pos.x() + right, pos.y()), diff.width(), scroll_sign_preview[es]);

			pos.shift_y(stat_y);
		}

		if (hasslots_preview)
		{
			slots_preview.draw(pos);
			pos.shift_y(stat_y);
			hammers_preview.draw(pos);
			pos.shift_y(stat_y);
		}

		// 197 武器突破上限行（对比窗侧）：紧跟金锤子之后，与 height_preview 补偿对应。
		if (has_breakthrough_line_preview)
		{
			breakthrough_line_preview.draw(DrawArgument(pos));
			pos.shift_y(stat_y);
		}

		// 095 潜能行（对比窗侧）：镜像主窗 :1163-1184 的顺序与配色，
		// 画在 slots/hammers 之后、描述之前，数据源为已装备物品。
		{
			bool any_preview = false;
			for (bool h : haspotlines_preview)
				any_preview = any_preview || h;

			if (any_preview)
			{
				pos.shift_y(BLOCK_GAP);

				// 095 分组标题（对比窗侧）：潜在/附加各一条圆点标题。
				bool has_main = haspotlines_preview[0] || haspotlines_preview[1] || haspotlines_preview[2];
				bool has_bonus = haspotlines_preview[3] || haspotlines_preview[4] || haspotlines_preview[5];

				if (has_main)
				{
					pot_title_preview.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
					pos.shift_y(LINE_STEP);

					for (size_t i = 0; i < 3; ++i)
					{
						if (!haspotlines_preview[i])
							continue;
						potlines_preview[i].draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
						// 192 潜能品级小图标增强（对比窗侧）：接在文本行尾。
						draw_pot_grade_icon(
							pos + Point<int16_t>(BODY_LEFT_X, 0), potlines_preview[i].width(), prank_preview);
						pos.shift_y(LINE_STEP);
					}
				}

				if (has_bonus)
				{
					bonus_title_preview.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
					pos.shift_y(LINE_STEP);

					for (size_t i = 3; i < 6; ++i)
					{
						if (!haspotlines_preview[i])
							continue;
						potlines_preview[i].draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
						// 192 潜能品级小图标增强（对比窗侧）：接在文本行尾。
						draw_pot_grade_icon(
							pos + Point<int16_t>(BODY_LEFT_X, 0), potlines_preview[i].width(), prank_preview);
						pos.shift_y(LINE_STEP);
					}
				}
			}
		}

		// 095 强化三件套（对比窗侧）：镜像主窗，缺项跳过不留空行。
		if (has_spark_line_preview || has_soul_line_preview || has_star_line_preview)
		{
			pos.shift_y(BLOCK_GAP);

			if (has_spark_line_preview)
			{
				spark_line_preview.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
				pos.shift_y(LINE_STEP);
			}

			if (has_soul_line_preview)
			{
				soul_line_preview.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
				pos.shift_y(LINE_STEP);
			}

			if (has_star_line_preview)
			{
				star_line_preview.draw(pos + Point<int16_t>(BODY_LEFT_X, 0));
				pos.shift_y(LINE_STEP);
			}
		}

		if (hasdesc_preview)
		{
			pos.shift_y(-4);
			separator.draw(pos);
			desc_preview.draw(pos + Point<int16_t>(BODY_CENTER_X, 8));
		}
	}
}
