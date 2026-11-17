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
#include "../../Graphics/Color.h"
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
		// 道具图标 + 原生 IconBase/0 底板（同为 2x，底板先画、图标叠上去）。
		// pos = 分隔线原点，与原生 tip 的 (16,80)/(2,2) 偏移一致。
		void draw_item_icon(Point<int16_t> pos, const Texture& icon) const;
		// 079/Angel：6 张职业名恒画，逐职业 Can(白)/Cannot(红)。
		void draw_job_line(Point<int16_t> pos, const std::vector<uint8_t>& allowedjobs) const;

		// 内容右界 ≈261（reqset 177+54+数字位宽），框宽取 264，兼作对比窗横向间距。
		static constexpr int16_t FRAME_WIDTH = 264;

		// 用户 m00832「两个 tooltip 交叉」：主窗背板 draw(pos+(-6,-6), FRAME_WIDTH+12)
		// 右缘 = X+FRAME_WIDTH+6；旧 draw_preview 只 shift_x(FRAME_WIDTH=264) ⇒
		// 对比窗背板左缘 = X+258，与主窗重叠 12px，两片不透明背板连成一整块。
		// 取 FRAME_WIDTH+12+6=282 ⇒ 两窗之间留 6px 场景缝隙，1px 亮边各自完整。
		static constexpr int16_t PREVIEW_OFFSET = FRAME_WIDTH + 12 + 6;

		// 2026-11-17 布局 [悬停][套装][对比]：套装面板插在主窗与对比窗之间时，
		// 对比窗需整体右移「套装面板宽 + 缝隙」。set 无效时原样返回 PREVIEW_OFFSET，
		// 与 m00832 既有「两窗不重叠」修复完全一致（非套装物品零影响）。
		int16_t set_aware_preview_offset() const;

	// 079 原生 1:1 (m00303/m00751) 职业带成员间空隙 ≈11px 恒定。
	// m00754 ⑦ 翻案：R4 的 JOB_STEP 六等分让「墨迹左沿」等距，但六张
	// 位图墨宽 23/23/35/34/23/23 不等 ⇒ 空隙 19/19/7/8/19 忽大忽小。
	// draw_job_line 改为按墨宽累计（ink_x += ink_w[i] + JOB_GAP），
	// 空隙由实现里的 JOB_GAP=11 钉死；这里只保留起始边距 JOB_SIDE。
	static constexpr int16_t JOB_SIDE = 4;

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

		// 2026-11-15 按 079 参照帧还原：IconBase/0（32x32 白底 + 黑描边 + 蓝角）
		// 与道具图标同为 2x 绘制。旧 ColorBox 是 32px 未缩放，只有图标一半大。
		Texture iconbase;
		// m00754 ③：ToolTip/Equip/{cash,mesos} 徽标属误加（Angel 上游 draw
		// 无此物），且 30x5 的 mesos 位图压在 REQ_LABEL_X=97 的「要求等级」上；
		// 字段 / ctor 加载 / 绘制调用已整段移除，不留死代码。
		// 用户 m00832：名称前圆点、正文行前「加号」一律不显示，
		// Texture 字段 / ctor 加载 / 绘制调用同步移除，不留死代码。
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
		// 交易限制行：079 原生在名称区下方叠高展示（m00304 ④），
		// 不属于分隔线以下属性块；对照 Angel 上游 ItemTooltip 名称下 quality 行。
		BodyLine trade_line;
		BodyLine trade_line_preview;
		bool has_trade = false;
		bool has_trade_preview = false;
		// 所有者行（095/服务端 owner 字段）：镜像交易行，画在名称区下方独立词条行。
		// 数据读 Equip::get_owner()，owner 为空不显示；与交易行同为橙色彩蛋。
		BodyLine owner_line;
		BodyLine owner_line_preview;
		bool has_owner = false;
		bool has_owner_preview = false;
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

		// 对比预览窗侧的 095 强化三件套（镜像主窗，数据源为已装备物品 eequip）。
		// 主窗 spark/soul/star 画悬停物品，预览侧画已穿戴物品，两者不可混用。
		bool has_spark_line_preview = false;
		bool has_soul_line_preview = false;
		bool has_star_line_preview = false;
		Text spark_line_preview;
		Text soul_line_preview;
		Text star_line_preview;

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