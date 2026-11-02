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

#include "../UIDragElement.h"

#include "../Components/AreaButton.h"
#include "../Components/Textfield.h"

#include "UIMapDropPanel.h"
#include "UIMapInfoPanel.h"
#include "UIMapMobPanel.h"
#include "UIMapSearchPanel.h"

namespace ms
{
	class UIWorldMap : public UIDragElement<PosMAP>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::WORLDMAP;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIWorldMap();

		void draw(float inter) const override;
		void update() override;

		void toggle_active() override;

		void remove_cursor() override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursor_pos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		// Include left/right docked panels so clicks outside the map chrome still hit this UI.
		bool is_in_range(Point<int16_t> cursor_position) const override;

		int32_t get_selected_map_id() const { return selected_map_id; }

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		static constexpr uint8_t MAPSPOT_TYPE_MAX = 4u;

		void set_search(bool enable);
		void update_world(std::string parent_map);
		void sync_side_panels();
		void clear_side_panels();
		void select_map_spot(int32_t map_id);
		void on_mob_selected(int32_t mob_id);
		// Keep left/right docked panels inside the viewport (default PosMAP is near the left edge).
		void ensure_panels_on_screen();

		Point<int16_t> get_search_pos();
		Point<int16_t> get_mob_panel_pos() const;
		Point<int16_t> get_map_info_panel_pos() const;
		Point<int16_t> get_map_search_panel_pos() const;
		Point<int16_t> get_drop_panel_pos() const;
		int16_t search_rail_width() const;

		enum Buttons
		{
			BT_CLOSE,
			BT_SEARCH,
			BT_AUTOFLY,
			BT_NAVIREG,
			BT_ALLSEARCH,
			BT_SEARCH_CLOSE,
			BT_LINK0,
			BT_LINK1,
			BT_LINK2,
			BT_LINK3,
			BT_LINK4,
			BT_LINK5,
			BT_LINK6,
			BT_LINK7,
			BT_LINK8,
			BT_LINK9
		};

		struct MapSpot
		{
			std::string description;
			Texture path;
			std::string title;
			uint8_t type;
			Texture marker;
			bool bolded;
			std::vector<int32_t> map_ids;
		};

		bool search;
		bool show_path_img;

		int32_t mapid;
		int32_t selected_map_id;

		std::string parent_map;
		std::string user_map;

		Texture search_background;
		Texture search_notice;
		Texture base_img;
		Texture path_img;

		Animation cur_pos;
		Animation npc_pos[MAPSPOT_TYPE_MAX];

		Textfield search_text;

		std::map<uint16_t, Texture> link_images;
		std::map<uint16_t, std::string> link_maps;

		std::vector<std::pair<Point<int16_t>, MapSpot>> map_spots;

		Point<int16_t> bg_dimensions;
		Point<int16_t> bg_search_dimensions;
		Point<int16_t> background_dimensions;
		Point<int16_t> base_position;
		// WorldMap/*.img 的 spot / path / linkImg 全部以 BaseImg 的 origin（底图中心）
		// 为原点，而不是画布左上角；base_anchor = base_position + base_img.origin。
		// 详见 evidence/w11-map/worldmap-anchor.md。
		Point<int16_t> base_anchor;
		Point<int16_t> search_text_dim;

		UIMapMobPanel mob_panel;
		UIMapSearchPanel map_search_panel;
		UIMapDropPanel drop_panel;
		// 左侧信息面板（地图信息：怪物列表 + 掉落网格 + 金币条 + 四档标签）。
		// 启用时替代 UIMapMobPanel 的占位（D2 并存：mob_panel 代码保留，仅不显示）。
		static constexpr bool kUseMapInfoPanel = true;
		UIMapInfoPanel info_panel;
	};
}
