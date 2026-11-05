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

#include "../../Graphics/Texture.h"
#include "../../Graphics/LocalPngTexture.h"
#include "../../Template/Rectangle.h"

#include "../Components/AreaButton.h"
#include "../Components/Textfield.h"

#include <vector>

#include "UIMapInfoPanel.h"
#include "UIMapMobPanel.h"
#include "UIMapSearchPanel.h"

namespace ms
{
	// s9 WORLD_MAP_PLAYERS(0x178) 回包单元：同一地图在线玩家（名字/等级/频道）。
	struct WorldMapPlayer
	{
		std::string name;
		int16_t level = 0;
		int8_t channel = 0;
	};

	class UIWorldMap : public UIDragElement<PosMAP>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::WORLDMAP;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIWorldMap();

		void draw(float inter) const override;
		void update() override;
		// 多分辨率：视口变化时按新屏幕居中重锚（保留原生 1x 尺寸与相对偏移）。
		void update_screen(int16_t new_width, int16_t new_height) override;

		void toggle_active() override;

		void remove_cursor() override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursor_pos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		// 双击右侧搜索结果行 → 跳到该图所属分区并选中点位（todo 8）。
		void doubleclick(Point<int16_t> cursorpos) override;

		UIElement::Type get_type() const override;

		// Include left/right docked panels so clicks outside the map chrome still hit this UI.
		bool is_in_range(Point<int16_t> cursor_position) const override;

		int32_t get_selected_map_id() const { return selected_map_id; }

		// WORLD_MAP_PLAYERS(0x178) 回包入口：只有 mapid 与当前选中一致才缓存，
		// 防止连续切换选中后旧应答覆盖新应答（服务端有 3s 缓存，乱序可达）。
		void set_map_players(int32_t map_id, std::vector<WorldMapPlayer>&& players);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		static constexpr uint8_t MAPSPOT_TYPE_MAX = 4u;

		// 两侧吸附面板的收起/展开状态机与缓动进度（复用 UIWorldSelect::panel_expand 范式）。
		enum class SideState
		{
			COLLAPSED,
			EXPANDING,
			EXPANDED,
			COLLAPSING
		};

		// 两侧把手（箭头）命中区：现已移入世界地图框内（不再外凸），
		// 宽度取 2x 箭头尺寸，使放大后的箭头完整可见且可点。
		static constexpr int16_t TAB_W = 26;
		static constexpr int16_t TAB_H = 92;

		// 侧栏展开/收起全程时长（ms）。
		// 安置说明（2026-11-04）：在途的 advance_side 用
		// `Constants::TIMESTEP / SIDE_EXPAND_MS` 累加进度，本常量必须为 float —
		// 整型除法会恒为 0，侧栏永远展不开。取值对齐同族先例
		// UIWorldSelect.cpp:149 `CH_EXPAND_MS = 220.0f`。
		static constexpr float SIDE_EXPAND_MS = 220.0f;

		void expand_left();
		void collapse_left();
		void toggle_left();
		void expand_right();
		void collapse_right();
		void toggle_right();
		void advance_side(float& value, SideState& state);
		void layout_panels();
		Rectangle<int16_t> get_left_tab_rect() const;
		Rectangle<int16_t> get_right_tab_rect() const;
		void draw_tab(const Rectangle<int16_t>& rect, SideState state, bool left) const;

		void set_search(bool enable);
		void update_world(std::string parent_map);
		void sync_side_panels();
		void clear_side_panels();
		void select_map_spot(int32_t map_id);
		// 付费传送：扣 10 万金币（服务端 !mapwarp 校验+扣费），统一四个入口（按钮 / S·F / T·回车 / 搜索框）。
		void request_teleport(int32_t map_id);
		// mapid → 所属 WorldMap 分区（原生分区名即 "WorldMap0" + mapid/10000000，
		// 与 update() 里玩家当前图算 parent 的口径完全一致，不需要自建反向索引）。
		void jump_to_map_region(int32_t map_id);
		void on_mob_selected(int32_t mob_id);
		// Keep left/right docked panels inside the viewport (default PosMAP is near the left edge).
		void ensure_panels_on_screen();

		Point<int16_t> get_search_pos();
		Point<int16_t> get_mob_panel_pos() const;
		Point<int16_t> get_map_info_panel_pos() const;
		Point<int16_t> get_map_search_panel_pos() const;
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

		// 两侧面板收起/展开状态（默认收起：打开大地图即纯看地图，低分辨率不被挤爆）。
		SideState left_state = SideState::COLLAPSED;
		SideState right_state = SideState::COLLAPSED;
		float left_expand = 0.0f;
		float right_expand = 0.0f;
		bool left_pinned = false;   // 用户手动收起 → 不再自动展开
		bool right_pinned = false;
		Texture collapse_arrow;     // UI/Basic.img/BtHide（左箭头），右面板翻转复用

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

		// 选中地图的在线玩家（0x178 回包缓存）。players_mapid = 缓存归属图，
		// 与 selected_map_id 不一致时不显示（陈旧应答丢弃）。
		std::vector<WorldMapPlayer> map_players;
		int32_t players_mapid = 0;

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
		// 左侧信息面板（地图信息：怪物列表 + 掉落网格 + 金币条 + 四档标签）。
		// 启用时替代 UIMapMobPanel 的占位（D2 并存：mob_panel 代码保留，仅不显示）。
		static constexpr bool kUseMapInfoPanel = true;
		UIMapInfoPanel info_panel;
	};
}
