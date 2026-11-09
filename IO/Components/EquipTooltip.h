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
		// 内容右界 ≈261（reqset 177+54+数字位宽），框宽取 264，兼作对比窗横向间距。
		static constexpr int16_t FRAME_WIDTH = 264;

		// 2026-11-09 R28 预留项②：装备 Tooltip 正文行前的橙色菱形项目符（ToolTip/Equip/Dot/0）。
		static constexpr float DOT_SCALE = 2.0f;   // 与 itemicon/iconbase 同 2x 渲染
		static constexpr int16_t DOT_GAP = 4;      // 项目符右缘到文本左缘间距
		static constexpr int16_t DOT_W = 5;        // 位图原生尺寸 5x5
		static constexpr int16_t DOT_H = 5;

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
		// IconBase/0 = 32x32 灰底板，2x 垫在道具图标下，与 SkillTooltip 同源。
		Texture iconbase;
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
		Text itemid;
		Text itemid_preview;

		Text name;
		Text name_preview;
		Text desc;
		Text desc_preview;
		Text potflag;
		Text potflag_preview;
		Text category;
		Text category_preview;
		Text wepspeed;
		Text wepspeed_preview;
		Text slots;
		Text slots_preview;
		Text hammers;
		Text hammers_preview;
		Text atkinc;
		Text atkinc_preview;
		EnumMap<EquipStat::Id, Text> statlabels;
		EnumMap<EquipStat::Id, Text> statlabels_preview;
		// 对比侧每条属性的差值文本（悬停值 - 已装备值）：正数绿色、负数红色。
		EnumMap<EquipStat::Id, Text> statdeltas_preview;

		TooltipBackdrop backdrop;
		ColorLine separator;

		Equip::Potential prank;
		Equip::Potential prank_preview;

		// 095 潜能/魔方：3 条主潜能 + 2 条 bonus 潜能行（值为 PotentialOption.img 行 id）。
		bool haspotlines[5] = {};
		Text potlines[5];
		bool haspotlines_preview[5] = {};
		Text potlines_preview[5];

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