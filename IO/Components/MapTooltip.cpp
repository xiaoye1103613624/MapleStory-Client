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
#include "MapTooltip.h"

#include <algorithm>

#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// TODO: Get number of available quests
	MapTooltip::MapTooltip() : parent(Tooltip::Parent::NONE), title(""), description(""), fillwidth(0), fillheight(0)
	{
		// Mob/Npc 图标在包内 UIWindow.img/ToolTip 下不存在（仅 Equip/cash/mesos），取到空纹理即不绘。
		nl::node WorldMap = UiAssetResolve::window("ToolTip")["WorldMap"];

		Mob = WorldMap["Mob"];
		Npc = WorldMap["Npc"];

		// TODO: Get list of where party members are
		//Party = WorldMap["Party"];
	}

	void MapTooltip::draw(Point<int16_t> position) const
	{
		if (title_label.empty())
			return;

		int16_t max_width = Constants::Constants::get().get_viewwidth();
		int16_t max_height = Constants::Constants::get().get_viewheight();

		// 底板宽按 Parent 区分：minimap = 对称 padding（fillwidth + 2*MINIMAP_PAD_X），
		// worldmap = 既有 fillwidth + 24（draw_worldmap 的底板余量）。回拉余量 20 = tip_offset.x。
		int16_t box_w = (parent == Tooltip::Parent::MINIMAP)
			? fillwidth + MINIMAP_PAD_X * 2
			: fillwidth + 24;
		int16_t cur_width = position.x() + box_w + 20;
		int16_t cur_height = position.y() + fillheight + 24;
		int16_t adj_x = cur_width - max_width;
		int16_t adj_y = cur_height - max_height;

		if (adj_x > 0)
			position.shift_x(adj_x * -1);

		if (adj_y > 0)
			position.shift_y(adj_y * -1);

		// Shift everything so the cursor is in the top left corner
		position.shift(Point<int16_t>(20, -3));

		switch (parent)
		{
			case Tooltip::Parent::WORLDMAP:
				draw_worldmap(position);
				break;
			case Tooltip::Parent::MINIMAP:
				draw_minimap(position);
				break;
			default:
				break;
		}
	}

	void MapTooltip::set_title(Tooltip::Parent p, std::string t, bool bolded)
	{
		if (parent == p && title == t)
			return;

		parent = p;
		title = t;

		if (title.empty())
			return;

		switch (parent)
		{
			case Tooltip::Parent::WORLDMAP:
				set_worldmap_title(bolded);
				break;
			case Tooltip::Parent::MINIMAP:
				set_minimap_title(bolded);
				break;
			default:
				break;
		}
	}

	void MapTooltip::set_desc(std::string d)
	{
		if (description == d)
			return;

		description = d;

		if (description.empty())
			return;

		switch (parent)
		{
			case Tooltip::Parent::WORLDMAP:
				set_worldmap_desc();
				break;
			case Tooltip::Parent::MINIMAP:
				set_minimap_desc();
				break;
			default:
				break;
		}
	}

	void MapTooltip::set_mapid(int32_t mapid, bool portal)
	{
		std::unordered_map<int64_t, std::pair<std::string, std::string>> life = NxHelper::Map::get_life_on_map(mapid);

		switch (parent)
		{
			case Tooltip::Parent::WORLDMAP:
				set_worldmap_mapid(life, portal);
				break;
			case Tooltip::Parent::MINIMAP:
				set_minimap_mapid(life, portal);
				break;
			default:
				break;
		}
	}

	void MapTooltip::reset()
	{
		parent = Tooltip::Parent::NONE;

		title = "";
		title_label.change_text("");

		description = "";
		desc_label.change_text("");

		for (uint8_t i = 0; i < MAX_LIFE; i++)
		{
			mob_labels[i].change_text("");
			npc_labels[i].change_text("");
		}

		fillwidth = 0;
		fillheight = 0;
	}

	void MapTooltip::draw_worldmap(Point<int16_t> position) const
	{
		int16_t width = fillwidth - 8;
		int16_t halfwidth = width / 2;

		// 框体中心对齐标题中心 (fillwidth/2 + 10)；Q1：程序描边，见 TextTooltip.h TooltipBackdrop
		// 2026-11-16 m01309：用户反馈覆盖不全——四边各再加 2px 余量，含分隔线与生活标签。
	backdrop.draw(position + Point<int16_t>(0, -2), fillwidth + 24, fillheight + 28);

		position.shift(Point<int16_t>(9, 4));

		title_label.draw(position + Point<int16_t>(halfwidth + 5, 0));

		if (!desc_label.empty())
		{
			position.shift_y(title_label.height() + 5);

			desc_label.draw(position + Point<int16_t>(-1, 0));
			position.shift_y(desc_label.height());
		}
		else
		{
			position.shift_y(title_label.height() - 2);
		}

		for (uint8_t i = 0; i < MAX_LIFE; i++)
		{
			Text mob = mob_labels[i];

			if (!mob.empty())
			{
				if (i == 0) {
					separator.draw(position + SEPARATOR_ADJ);
					position.shift_y(10);

					Mob.draw(position + LIFE_ICON_ADJ);
				}

				mob.draw(position + LIFE_LABEL_ADJ);
				position.shift_y(mob.height() + 1);
			}
		}

		if (!mob_labels->empty() && !npc_labels->empty())
			position.shift_y(-2);

		for (uint8_t i = 0; i < MAX_LIFE; i++)
		{
			Text npc = npc_labels[i];

			if (!npc.empty())
			{
				if (i == 0) {
					separator.draw(position + SEPARATOR_ADJ);
					position.shift_y(10);

					Npc.draw(position + LIFE_ICON_ADJ);
				}

				npc.draw(position + LIFE_LABEL_ADJ);
				position.shift_y(npc.height() + 1);
			}
		}
	}

	void MapTooltip::draw_minimap(Point<int16_t> position) const
	{
		// MinMS 重写版布局（E:\project\MinMS\src\client\IO\Components\MapTooltip.cpp:60-135）：
		// 对称 padding——所有行文字都已限宽，box_w = fillwidth + 2*PAD_X 恰好包住内容，
		// 标题/描述/生命行统一以 PAD_X + fillwidth/2 为中心，消除旧版
		// 「宽度裁剪 -12/-8 + halfwidth+5/+7 偏移」导致的左右不等宽与文字溢出。
		// 包内无 UIToolTip.img/Item/Frame2 九宫格（orange-wz 实测），底板沿用程序描边 backdrop；
		// 包内亦无 WorldMap/Mob/Npc 图标，生命行不再为其预留 16px 缩进（原版另一处不对称源）。
		constexpr int16_t PAD_X = MINIMAP_PAD_X;
		constexpr int16_t PAD_Y = MINIMAP_PAD_Y;

		const int16_t box_w = fillwidth + PAD_X * 2;
		const int16_t box_h = fillheight + PAD_Y * 2;

		// MinMS：小地图标记就在光标旁，tooltip 向左下拉贴近标记、不盖光标。
		position.shift(Point<int16_t>(-55, 12));

		backdrop.draw(position + Point<int16_t>(0, -4), box_w, box_h);

		Point<int16_t> cursor = position + Point<int16_t>(PAD_X + fillwidth / 2, PAD_Y - 4);

		title_label.draw(cursor);
		cursor.shift_y(title_label.height() + 2);

		if (!desc_label.empty())
		{
			separator.draw(position + Point<int16_t>(PAD_X, cursor.y() - position.y()));
			cursor.shift_y(6);

			desc_label.draw(cursor);
			cursor.shift_y(desc_label.height() + 2);
		}

		bool mob_drawn = false;
		for (uint8_t i = 0; i < MAX_LIFE; i++)
		{
			Text mob = mob_labels[i];

			if (mob.empty())
				continue;

			if (!mob_drawn)
			{
				separator.draw(position + Point<int16_t>(PAD_X, cursor.y() - position.y()));
				cursor.shift_y(6);
				mob_drawn = true;
			}

			mob.draw(cursor);
			cursor.shift_y(mob.height() + 1);
		}

		bool npc_drawn = false;
		for (uint8_t i = 0; i < MAX_LIFE; i++)
		{
			Text npc = npc_labels[i];

			if (npc.empty())
				continue;

			if (!npc_drawn)
			{
				separator.draw(position + Point<int16_t>(PAD_X, cursor.y() - position.y()));
				cursor.shift_y(6);
				npc_drawn = true;
			}

			npc.draw(cursor);
			cursor.shift_y(npc.height() + 1);
		}
	}

	void MapTooltip::rebuild_minimap_layout()
	{
		// draw_minimap 行累计的镜像：fillwidth = 最宽行、fillheight = 全部行高与行距之和，
		// 供对称底板 box_w/box_h 精确包住内容（替代旧 set_minimap_* 里各自为政的散装累计）。
		fillwidth = title_label.width();
		fillheight = title_label.height() + 2;

		if (!desc_label.empty())
		{
			fillwidth = std::max(fillwidth, desc_label.width());
			fillheight += 8 + desc_label.height();
		}

		bool mob_any = false;
		bool npc_any = false;
		int16_t life_width = 0;

		for (uint8_t i = 0; i < MAX_LIFE; i++)
		{
			if (!mob_labels[i].empty())
			{
				mob_any = true;
				life_width = std::max(life_width, mob_labels[i].width());
				fillheight += mob_labels[i].height() + 1;
			}
		}

		if (mob_any)
			fillheight += 6;

		for (uint8_t i = 0; i < MAX_LIFE; i++)
		{
			if (!npc_labels[i].empty())
			{
				npc_any = true;
				life_width = std::max(life_width, npc_labels[i].width());
				fillheight += npc_labels[i].height() + 1;
			}
		}

		if (npc_any)
			fillheight += 6;

		fillwidth = std::max(fillwidth, life_width);

		separator = ColorLine(fillwidth, Color::Name::WHITE, 0.40f, false);
	}

	void MapTooltip::set_worldmap_title(bool bolded)
	{
		fillwidth = 206;
		title_label = Text(bolded ? Text::Font::A12B : Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, title);

		int16_t width = title_label.width();
		int16_t height = title_label.height();

		if (width > fillwidth)
			fillwidth = width;

		separator = ColorLine(fillwidth, Color::Name::WHITE, 0.40f, false);

		if (height > fillheight)
			fillheight = height;
	}

	void MapTooltip::set_minimap_title(bool bolded)
	{
		// 原生通用 tip 规范：名称用 A12B（加粗），与 set_worldmap_title /
		// EquipTooltip 的名称口径一致；desc 仍 A12M。旧实现忽略 bolded 恒用 A12M。
		title_label = Text(bolded ? Text::Font::A12B : Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, title);

		rebuild_minimap_layout();
	}

	void MapTooltip::set_worldmap_desc()
	{
		desc_label = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, description, fillwidth);

		int16_t width = desc_label.width();
		int16_t height = desc_label.height();

		if (width > fillwidth)
			fillwidth = width;

		separator = ColorLine(fillwidth, Color::Name::WHITE, 0.40f, false);

		if (height > fillheight)
			fillheight = height;
	}

	void MapTooltip::set_minimap_desc()
	{
		// MinMS：minimap desc 限宽 100 由 Text 自动换行，防长描述横向溢出（MinMS :168-169）。
		desc_label = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, description, 100);

		rebuild_minimap_layout();
	}

	void MapTooltip::set_worldmap_mapid(std::unordered_map<int64_t, std::pair<std::string, std::string>> life, bool)
	{
		int32_t m = 0;
		int32_t n = 0;
		bool desc_empty = desc_label.empty();

		if (!desc_empty)
			fillheight += title_label.height() + 7;

		for (auto& l : life)
		{
			auto& life_object = l.second;

			if (life_object.first == "m" && m < MAX_LIFE)
			{
				mob_labels[m] = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::CHARTREUSE, life_object.second);
				fillheight += mob_labels->height() + 1;

				m++;
			}
			else if (life_object.first == "n" && n < MAX_LIFE)
			{
				npc_labels[n] = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::MALIBU, life_object.second);

				if (m > 0 && n == 0)
					fillheight += 8;

				fillheight += npc_labels->height() + 1;

				n++;
			}
		}

		fillheight -= 3;

		if (desc_empty && m == 0 && n == 0)
			fillheight -= 8;
	}

	void MapTooltip::set_minimap_mapid(std::unordered_map<int64_t, std::pair<std::string, std::string>> life, bool portal)
	{
		// portal 仅区分调用方语义（世界地图 / 传送门悬停），布局统一由 rebuild 处理。
		(void)portal;

		int32_t m = 0;
		int32_t n = 0;

		for (auto& l : life)
		{
			auto& life_object = l.second;

			if (life_object.first == "m" && m < MAX_LIFE)
			{
				// MinMS：生命行 CENTER 对齐 + 限宽 220 换行（MinMS :199）；行宽不再
				// 预留包内不存在的 Mob/Npc 图标（旧 +width+8 是溢出帮凶之一）。
				mob_labels[m] = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::CHARTREUSE, life_object.second, 220);

				m++;
			}
			else if (life_object.first == "n" && n < MAX_LIFE)
			{
				npc_labels[n] = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::MALIBU, life_object.second, 220);

				n++;
			}
		}

		rebuild_minimap_layout();
	}
}