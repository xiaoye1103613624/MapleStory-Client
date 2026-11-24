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

#include "../Components/Slider.h"

#include "../../Graphics/SpecialText.h"
#include "../../Gameplay/Stage.h"

#include <string>
#include <tuple>
#include <vector>

namespace ms
{
	class UIMiniMap : public UIDragElement<PosMINIMAP>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MINIMAP;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIMiniMap(const CharStats& stats);

		void draw(float alpha) const override;
		void update() override;
		// SavedPosClamped：保留 PosMINIMAP，切档时夹回视口。
		void update_screen(int16_t new_width, int16_t new_height) override;

		void remove_cursor() override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> pos) override;
		void send_scroll(double yoffset) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		// 自定义 QoL（非原生 opcode 407）：小地图玩家标记双击开 CharInfo、右击开 ContextMenu。
		void doubleclick(Point<int16_t> cursorpos) override;
		void rightclick(Point<int16_t> cursorpos) override;

		UIElement::Type get_type() const override;

		Point<int16_t> get_abs_position() const { return position; }
		Point<int16_t> get_abs_dimension() const { return dimension; }

		/// 换图成功后由 SetFieldHandlers 显式调用：按当前 npcs 容器重建"非玩家"列表。
		/// 原实现只靠 update() 的 mapid 差分，存在换图时不刷新的时序漏洞（详见 .cpp 实现注释）。
		void refresh_npclist();

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		static constexpr int16_t CENTER_START_X = 64;
		static constexpr int16_t BTN_MIN_Y = 4;
		static constexpr int16_t ML_MR_Y = 17;
		static constexpr int16_t MAX_ADJ = 43;
		/// MaxMap 顶带 72px 里，白色标题条占 y5..18，浅蓝信息带占 y20..65
		/// （MaxMap/nw 逐行取色）；38x38 的地图标记垂直居中于信息带 -> 20+(46-38)/2。
		static constexpr int16_t MARK_X = 8;
		static constexpr int16_t MARK_Y = 24;
		static constexpr int16_t M_START = 36;
		static constexpr int16_t LISTNPC_ITEM_HEIGHT = 17;
		static constexpr int16_t LISTNPC_ITEM_WIDTH = 140;
		static constexpr int16_t LISTNPC_TEXT_WIDTH = 114;
		/// A11 单行墨迹高（含 CJK，实测 10~11，取 11 居中）；只用于定行内墨迹顶，
		/// 真正绘制 y 由 Text::y_for_ink_top 按当前字形反解（对齐 UIDamageRank.h:101）。
		static constexpr int16_t LISTNPC_INK_HEIGHT = 11;
		static constexpr Point<int16_t> WINDOW_UL_POS = Point<int16_t>(0, 0);

		void update_buttons();
		void toggle_buttons();
		void update_text();
		void update_canvas();
		void draw_movable_markers(Point<int16_t> init_pos, float alpha) const;
		void update_static_markers();
		void set_npclist_active(bool active);
		void update_dimensions();
		void update_npclist();
		void draw_npclist(Point<int16_t> minimap_dims, float alpha) const;
		void select_npclist(int16_t choice);

		/// 小地图 NPC 标记命中：与 draw_movable_markers 同公式，须在视口内；未命中返回 0。
		int32_t hit_npc_marker(Point<int16_t> cursor_relative) const;
		/// 小地图玩家标记命中（他角 + 自己）；热区按 marker 尺寸对齐绘制偏移。
		bool hit_player_marker(Point<int16_t> cursor_relative, int32_t& cid, std::string& name, bool& is_self) const;

		enum Buttons
		{
			BT_MIN,
			BT_MAX,
			BT_SMALL,
			BT_BIG,
			BT_MAP,
			BT_NPC,
			BT_THEME		// ThemePack MapleButton toggle (left of BT_MIN)
		};

		enum Type
		{
			MIN,
			NORMAL,
			MAX
		};

		/// Constants
		int32_t mapid;
		int8_t type;
		int8_t user_type;
		bool simpleMode;
		bool big_map;
		bool has_map;
		int16_t scale;
		nl::node Map;
		nl::node MiniMap;
		nl::node marker;
		Texture map_sprite;
		Animation player_marker;
		int16_t combined_text_width;
		int16_t middle_right_x;
		int16_t bt_min_width;
		int16_t bt_max_width;
		int16_t bt_map_width;
		int16_t bt_theme_width;
		int16_t bt_theme_x;
		std::vector<Sprite> min_sprites;
		std::vector<Sprite> normal_sprites;
		std::vector<Sprite> max_sprites;
		// first=传送门名, second=相对窗体的绘制坐标(命中判定用), third=画布坐标(视口裁剪用)
		std::vector<std::tuple<std::string, Point<int16_t>, Point<int16_t>>> static_marker_info;
		int16_t map_draw_origin_x, map_draw_origin_y;
		Point<int16_t> center_offset;
		/// 192 CUIMiniMap::CalculateScr：内容区视口（非全图），以玩家为中心滚动。
		/// map_scr = 画布左上角裁剪起点；map_view = 可见宽高（硬上限 MAX_PANE_*）。
		int16_t map_scr_x = 0;
		int16_t map_scr_y = 0;
		int16_t map_view_w = 0;
		int16_t map_view_h = 0;
		Point<int16_t> min_dimensions;
		Point<int16_t> normal_dimensions;
		Point<int16_t> max_dimensions;

		/// MapleStoryT SetLayer 小模式硬上限：canvas≥280 → pane=210，≥150 → pane=112。
		/// ToggleMap 另用 280×150 / screen-50 / screen-150 强制关闭 BigShow。
		static constexpr int16_t MAX_PANE_W = 210;
		static constexpr int16_t MAX_PANE_H = 112;
		static constexpr int16_t BIG_TOGGLE_W = 280;
		static constexpr int16_t BIG_TOGGLE_H = 150;

		void refresh_map_viewport();
		/// ToggleMap 同款：过小或超屏则禁止 BigShow（全图 overview）。
		void apply_big_map_policy(int16_t canvas_w, int16_t canvas_h);
		Point<int16_t> map_marker_offset() const;
		/// 192 CUIMiniMap::IsPtInViewRange：画布坐标点是否落在当前可见 pane 内。
		/// 未显示区域的传送门/NPC/角色标记必须裁掉，否则会溢出小地图窗体。
		/// 入参为「画布坐标」(world+center_offset)/scale，与 map_scr_* 同坐标系。
		bool canvas_pos_in_view(Point<int16_t> canvas_pos) const;
		Text combined_text;
		// 原生 MAX 顶带（72px，底色 (153,187,204) 浅蓝）的地图名是**白字黑描边**，
		// 参考帧 evidence/refcrops/minimap.png 实测字身 min(rgb)>240、外圈为暗边。
		OutlinedText region_text;
		OutlinedText town_text;

		bool listNpc_enabled;
		nl::node listNpc;
		nl::node MapHelper;
		std::vector<Sprite> listNpc_sprites;
		// NPC 列表条目存值快照（oid + 坐标），不持 MapObject* 裸指针：
		// MapNpcs 在 remove/clear 时会 delete Npc 对象，签名机制只按 oid 集合
		// 刷新，同 oid 对象替换时裸指针会悬垂 → get_oid()/get_position() 读垃圾，
		// 发无效 oid 被服务端静默拒绝（表现为 NPC 对话框打不开）。
		struct NpcListEntry
		{
			int32_t oid = 0;
			Point<int16_t> position;
		};
		std::vector<NpcListEntry> listNpc_list;
		std::vector<Text> listNpc_names;
		std::vector<std::string> listNpc_full_names;

		Point<int16_t> listNpc_dimensions;

		Slider listNpc_slider;
		int16_t listNpc_offset;
		int16_t selected;
		Animation selected_marker;

		/// 兜底刷新用的 npc 容器签名：size 变化或任一条目 oid 变化即判定列表失效。
		/// mapid 差分在 fadeout→load 窗口期失效（clear 时 mapid 未变、spawn 进来时 mapid 已定），
		/// 故以此签名补一道每帧检测（与 toggle_buttons 的 has_npcs 同源）。
		size_t listNpc_sig_size = 0;
		std::vector<int32_t> listNpc_sig_oids;
		bool listNpc_sig_valid = false;

		/// 计算当前 npcs 容器的 {size, 有序 oid} 签名，与缓存比对；不同则刷新缓存返回 true。
		bool npclist_signature_changed();
		/// 仅把当前容器签名写入缓存（update 走 mapid 重建路径后同步基准）。
		void refresh_npclist_signature();

		/// NPC 交谈改「按下记录意图 / 松手确认发包」：
		/// - pending 记录按下时命中的 oid（列表行或场景 NPC 绿点）；
		/// - 松手时只有仍命中同一目标才发包，避免按下拖到别处误触发；
		/// - down 落在列表/绿点命中区时从 dragarea 摘除，杜绝被窗口拖拽吞点击
		///   （对齐 UIDamageRank.cpp:285-299 HitTestHeaderDragArea 的写法）。
		int32_t pending_talk_oid = 0;

		const CharStats& stats;
	};
}