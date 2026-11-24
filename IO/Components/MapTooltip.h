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

#include "TextTooltip.h"
#include "Tooltip.h"

#include <vector>

#include "../../Graphics/Geometry.h"

namespace ms
{
	class MapTooltip : public Tooltip
	{
	public:
		MapTooltip();

		void draw(Point<int16_t> position) const override;

		void set_title(Tooltip::Parent parent, std::string title, bool bolded);
		void set_desc(std::string description);
		void set_mapid(int32_t mapid, bool portal);
		void set_players(const std::vector<MapTooltipPlayer>& players);

		void reset();

	private:
		static constexpr uint8_t MAX_LIFE = 10u;
		// MinMS 重写版的对称 padding 口径（MinMS MapTooltip.cpp:68-69）：
		// 底板宽 = fillwidth + PAD_X*2、高 = 内容高 + PAD_Y*2，左右严格等宽。
		static constexpr int16_t MINIMAP_PAD_X = 8;
		static constexpr int16_t MINIMAP_PAD_Y = 5;
		static constexpr Point<int16_t> SEPARATOR_ADJ = Point<int16_t>(1, 10);
		static constexpr Point<int16_t> LIFE_LABEL_ADJ = Point<int16_t>(16, 3);
		static constexpr Point<int16_t> LIFE_ICON_ADJ = Point<int16_t>(1, 9);

		void draw_worldmap(Point<int16_t> position) const;
		void draw_minimap(Point<int16_t> position) const;
		void rebuild_minimap_layout();

		void set_worldmap_title(bool bolded);
		void set_minimap_title(bool bolded);
		void set_worldmap_desc();
		void set_minimap_desc();
		void set_worldmap_mapid(std::unordered_map<int64_t, std::pair<std::string, std::string>> life, bool portal);
		void set_minimap_mapid(std::unordered_map<int64_t, std::pair<std::string, std::string>> life, bool portal);

		TooltipBackdrop backdrop;

		Texture Mob;
		Texture Npc;
		// 世界地图 tip 每行生命按自身 id 取的真实图标（MonsterBook.img/icon 在本端不存在）。
		// mob_icon_stand[0..] 对应 mob_labels[0..]，npc 同理；空则不画。
		Texture mob_icon_stand[MAX_LIFE];
		Texture npc_icon_stand[MAX_LIFE];
		//Texture Party;

		Tooltip::Parent parent;

		std::string title;
		std::string description;

		Text title_label;
		Text desc_label;
		Text mob_labels[MAX_LIFE];
		Text npc_labels[MAX_LIFE];
		Text mob_count_header;   // "Monsters (N):" 数量统计头（世界地图 tip）
		Text npc_count_header;   // "NPCs (N):"
		Text player_header;
		Text player_labels[MAX_LIFE];

		int16_t fillwidth;
		int16_t fillheight;

		ColorLine separator;
	};
}