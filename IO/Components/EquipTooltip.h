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

#include "Charset.h"
#include "SetTooltip.h"
#include "TextTooltip.h"
#include "Tooltip.h"

#include "../../Character/MapleStat.h"
#include "../../Graphics/Geometry.h"
#include "../../Graphics/Text.h"
#include "../../Template/BoolPair.h"

#include "../../Character/Inventory/Equip.h"

namespace ms
{
	class EquipTooltip : public Tooltip
	{
	public:
		EquipTooltip();

		void set_equip(Tooltip::Parent parent, int16_t invpos);
		void draw(Point<int16_t> position) const override;
		void draw_preview(Point<int16_t> position) const;

	private:
		// 道具图标 + 灰底板（底板先画、图标叠上去，两者同屏幕锚点）。
		// pos = 分隔线原点，与原生 tip 的 (16,80)/(2,2) 偏移一致。
		void draw_item_icon(Point<int16_t> pos, const Texture& icon) const;
		// 图标旁 cash/mesos 徽标（前置块，不参与分隔线以下居中）。
		void draw_cash_badge(Point<int16_t> pos, bool cash) const;

		// 内容右界 ≈261（reqset 177+54+数字位宽），框宽取 264，兼作对比窗横向间距。
		static constexpr int16_t FRAME_WIDTH = 264;

		// 2026-11-09 R28 预留项②：装备 Tooltip 正文行前的橙色菱形项目符（ToolTip/Equip/Dot/0）。
		static constexpr float DOT_SCALE = 2.0f;   // 与 itemicon/iconbase 同 2x 渲染
		static constexpr int16_t DOT_GAP = 4;      // 项目符右缘到文本左缘间距
		static constexpr int16_t DOT_W = 5;        // 位图原生尺寸 5x5
		static constexpr int16_t DOT_H = 5;

		// 职业名横排的固定步长（2026-11-11 用户实拍：按位图宽度 +2 排开时，
		// 「新手」40px 与「战士/魔法师」38px 交替，肉眼看出间距不一致）。
		static constexpr int16_t JOB_STEP = 41;

		// 正文行「标签：数值」的一半。原生把冒号固定在正文中线：标签右对齐到中线、
		// 数值从中线起左对齐，多行叠起来冒号成一条竖线（2026-11-11 用户实拍要求）。
		//
		// 两个排版档位：
		//   centered = true  —— 附加属性行（力量/敏捷/智力/运气/攻击力…），冒号压在中线上；
		//   centered = false —— 元信息行（道具ID/装备分类/攻击速度/可升级次数/金锤子…），
		//                       标签自左缘排开、冒号紧随其后（原生参考帧就是这么排的，
		//                       这几个标签最长 13 个汉字，居中会顶出 264px 框宽）。
		struct BodyLine
		{
			Text label;		// 冒号左侧的标签（不含冒号）
			Text colon;		// 分隔符（中文全角「：」/ 英文半角「:」）
			Text value;		// 冒号右侧的数值，默认白色
			bool centered = false;

			bool empty() const
			{
				return label.empty() && value.empty();
			}

			// full = 本地化整串（「装备分类：下衣」/「STR: 3」）。
			void build(const std::string& full, Color::Name valuecolor, bool center_colon);

			// pos = 本行的绘制原点（左缘 x=0 口径）。返回值右端的 x 偏移，
			// 供对比窗把差值接在数值后面。
			int16_t draw(Point<int16_t> pos) const;
		};

		int16_t invpos;
		int16_t invpos_preview;
		int16_t height;
		int16_t height_preview;
		bool hasdesc;
		bool hasdesc_preview;
		bool hasslots;
		bool hasslots_preview;
		bool is_weapon;
		bool is_weapon_preview;
		// 是否构建/绘制「同部位已装备物品」对比（ui.equip_compare）。
		bool show_compare;
		EnumMap<MapleStat::Id, std::string> reqstatstrings;
		EnumMap<MapleStat::Id, std::string> reqstatstrings_preview;
		Texture itemicon;
		Texture itemicon_preview;

		// 2026-11-08 原生 tooltip 复校新增（evidence/r28-tooltip-native/）：
		// IconBase/0 = 32x32 位图。但部署包里的 IconBase/0 是「白底黑框 + 蓝角」，
		// 直接 2x 垫在图标下就变成用户截图里那块「白框掉在图标下方」的错图。
		// 2026-11-11 改为程序自绘灰底板（ColorBox），不依赖该位图。
		ColorBox iconbase_box;
		// IMG ToolTip/Equip/{cash,mesos}：现金/金币徽标，画在图标区（前置块，不居中整块）。
		Texture cash_badge;
		Texture mesos_badge;
		bool show_cash = false;
		bool show_cash_preview = false;
		// 2026-11-09 R28 预留项②：ToolTip/Equip/Dot/0 = 5x5 橙色菱形项目符，逐行画在正文左缘前。
		Texture dot;
		// GrowthEnabled/GrowthDisabled 提供成长两行的标签、数字字模与 percent 位图；
		// fame_none 为 Can/none（5x1 短横），用于原生「要求人气」行。
		BoolPair<Charset> growthset;
		Texture growth_lev_label;
		Texture growth_exp_label;
		Texture growth_percent;
		Texture fame_none;
		// 用户抉择：仅当 itemlevel/itemexp 任一非零时才显示成长两行。
		bool show_growth;
		bool show_growth_preview;
		std::string growth_level_str;
		std::string growth_exp_str;
		std::string growth_level_str_preview;
		std::string growth_exp_str_preview;
		// 需求块整体推进量（6 固定行 / 8 行含成长两行），draw 与 height 共用以免脱节。
		int16_t req_shift;
		int16_t req_shift_preview;
		// 装备分类上方新增的道具 ID 行。
		BodyLine itemid;
		BodyLine itemid_preview;

		Text name;
		Text name_preview;
		Text desc;
		Text desc_preview;
		Text potflag;
		Text potflag_preview;
		BodyLine category;
		BodyLine category_preview;
		BodyLine wepspeed;
		BodyLine wepspeed_preview;
		BodyLine slots;
		BodyLine slots_preview;
		BodyLine hammers;
		BodyLine hammers_preview;
		// 交易限制行（分隔线以下居中区；对照 ItemTooltip untradable/unique）。
		BodyLine trade_line;
		BodyLine trade_line_preview;
		bool has_trade = false;
		bool has_trade_preview = false;
		// 2026-11-11 用户实拍：右上「攻击力增加」是历史误接的 equip.label_atkinc
		// （`攻击力增加` 只是「攻击力」这个属性名 + 「增加」二字的本地化词条，
		// 原生 tip 里没有这一行），已整段移除，不再保留绘制字段。
		EnumMap<EquipStat::Id, BodyLine> statlabels;
		EnumMap<EquipStat::Id, BodyLine> statlabels_preview;
		// 对比侧每条属性的差值文本（悬停值 - 已装备值）：正数绿色、负数红色。
		EnumMap<EquipStat::Id, Text> statdeltas_preview;

		TooltipBackdrop backdrop;
		ColorLine separator;

		// ⑤ 套装 companion 面板（2026-11-12）：
		// 参考 E:\MXD\扩展改动\套装系统\README.md §UI 的三段布局
		//   [悬停装备 tip] → [套装 tip] → [已穿比较 tip]
		// 装备 tip 本身不改，套装面板作为独立第三层在 draw 末尾叠画。
		SetTooltip settip;
		// 已穿戴集合的快照哈希：set_equip 的早退判定要连它一起比，
		// 否则「同一槽位换了一件装备」不会触发套装件数重算。
		size_t equip_snapshot_hash = 0;

		Equip::Potential prank;
		Equip::Potential prank_preview;

		// 095 潜能/魔方：3 条主潜能 + 2 条 bonus 潜能行（值为 PotentialOption.img 行 id）。
		bool haspotlines[5] = {};
		Text potlines[5];
		bool haspotlines_preview[5] = {};
		Text potlines_preview[5];

		// ⑦ 095 强化三件套（Round 15）：火花 / 宝珠 / 星之力，各一行。
		// 与潜能行同处「可升级次数」之后的附加属性块，缺项自动跳过不留空行。
		bool has_spark_line = false;
		bool has_soul_line = false;
		bool has_star_line = false;
		Text spark_line;
		Text soul_line;
		Text star_line;

		std::vector<MapleStat::Id> requirements;
		EnumMap<MapleStat::Id, BoolPair<Texture>> reqstattextures;
		EnumMap<MapleStat::Id, bool> canequip;
		EnumMap<MapleStat::Id, bool> canequip_preview;
		EnumMap<MapleStat::Id, Point<int16_t>> reqstatpositions;
		BoolPair<Charset> reqset;
		BoolPair<Charset> lvset;

		BoolPair<std::map<uint8_t, Texture>> jobs;
		std::vector<uint8_t> okjobs;
		std::vector<uint8_t> okjobs_preview;
	};
}