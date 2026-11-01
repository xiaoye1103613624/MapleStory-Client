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
#include "UIMiniMap.h"

#include "UISoftKey.h"
#include "UIWorldMap.h"

#include "../UI.h"

#include "../Components/MapleButton.h"

#include "../../Gameplay/MapleMap/Npc.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/GameplayPackets.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Constants.h"

#include <cmath>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIMiniMap::UIMiniMap(const CharStats& stats) : UIDragElement<PosMINIMAP>(Point<int16_t>(128, 20)), stats(stats)
	{
		big_map = true;
		has_map = false;
		listNpc_enabled = false;
		listNpc_dimensions = Point<int16_t>(150, 170);
		listNpc_offset = 0;
		selected = -1;

		type = Setting<MiniMapType>::get().load();
		user_type = type;
		simpleMode = Setting<MiniMapSimpleMode>::get().load();

		nl::node UIWindow2 = nl::nx::UI["UIWindow2.img"];

		if (!UIWindow2)
			UIWindow2 = nl::nx::UI["UIWindow.img"];

		std::string node = simpleMode ? "MiniMapSimpleMode" : "MiniMap";
		MiniMap = UiAssetResolve::window(node.c_str());

		if (!MiniMap && simpleMode)
			MiniMap = UiAssetResolve::window("MiniMap");

		listNpc = UiAssetResolve::window("MiniMap")["ListNpc"];
		MapHelper = nl::nx::Map["MapHelper.img"];

		if (!UiAssetResolve::usable(MiniMap))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		if (MiniMap)
		{
			// 本包 UIWindow.img/MiniMap 只有 BtMap（36x12，origin 0）；BtMin/BtMax/
			// BtSmall/BtBig/BtNpc 是越代件，包内不存在，请求它们只会得到透明空壳。
			nl::node btmap = MiniMap["BtMap"];

			if (UiAssetResolve::usable(btmap))
				buttons[Buttons::BT_MAP] = std::make_unique<MapleButton>(btmap, Point<int16_t>(0, 0));
			else
				UiAssetResolve::diag_once("minimap btmap", "UIWindow.img/MiniMap/BtMap missing");
		}

		region_text = OutlinedText(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK);
		town_text = OutlinedText(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK);
		combined_text = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK);

		if (Setting<MiniMapDefaultHelpers>::get().load() && UIWindow2["MiniMapSimpleMode"]["DefaultHelper"])
			marker = UIWindow2["MiniMapSimpleMode"]["DefaultHelper"];
		else
			marker = MapHelper["minimap"];

		player_marker = Animation(marker["user"]);
		selected_marker = Animation(MiniMap["iconNpc"]);
	}

	void UIMiniMap::draw(float inter) const
	{
		if (type == Type::MIN)
		{
			for (Sprite sprite : min_sprites)
				sprite.draw(position, inter);

			combined_text.draw(position + Point<int16_t>(7, -3));
		}
		else if (type == Type::NORMAL)
		{
			for (Sprite sprite : normal_sprites)
				sprite.draw(position, inter);

			// 29px 顶带里「小地图」是 MinMap/nw 烤死的艺术字（60x29 装饰帽），只占到 local y≈20；
			// 参考帧 native-refs-r14/d15f70a2 显示它下面还有一行街道名「彩虹岛」，
			// 墨迹在 local x14..47 / y25..32，画布从 y28 起 —— 两行不重叠。
			// 用的是描边字（白字黑廓），压到浅色底带上依然可读。
			town_text.draw(position + Point<int16_t>(8, 18));

			if (has_map)
			{
				Animation portal_marker = Animation(marker["portal"]);

				for (auto& sprite : static_marker_info)
					portal_marker.draw(position + sprite.second, inter);

				draw_movable_markers(position, inter);

				if (listNpc_enabled)
					draw_npclist(normal_dimensions, inter);
			}
		}
		else
		{
			for (Sprite sprite : max_sprites)
				sprite.draw(position, inter);

			// 72px 顶带（MaxMap/nw 逐行实测）：y0..4 黑框、**y5..18 白色标题条**、
			// y19 分隔、**y20..65 浅蓝信息带 (153,187,204)**、y66..69 深蓝、y70..71 亮边。
			// 「小地图」三个烤死在 nw 里，占 x6..50 / y5..18。
			// 所以地图标记必须落在浅蓝带内：38x38 居中 -> y = 20 + (46-38)/2 = 24；
			// 旧值 (8,8) 会把标记压在白色标题条和「小地图」艺术字上。
			// 两行文字同理：A12B 汉字墨迹 = draw_y+6 起，参考帧墨迹在 y28..38 / 45..54
			// -> draw_y = 22 / 39（旧值 28 / 44 整体低 6px，第二行墨迹会顶到 y66 深蓝边）。
			region_text.draw(position + Point<int16_t>(46, 22));
			town_text.draw(position + Point<int16_t>(46, 39));

			if (has_map)
			{
				Animation portal_marker(marker["portal"]);

				for (auto& sprite : static_marker_info)
					portal_marker.draw(position + sprite.second + Point<int16_t>(0, MAX_ADJ), inter);

				draw_movable_markers(position + Point<int16_t>(0, MAX_ADJ), inter);

				if (listNpc_enabled)
					draw_npclist(max_dimensions, inter);
			}
		}

		UIElement::draw(inter);
	}

	void UIMiniMap::update()
	{
		int32_t mid = Stage::get().get_mapid();

		if (mid != mapid)
		{
			mapid = mid;
			Map = NxHelper::Map::get_map_node_name(mapid);

			nl::node town = Map["info"]["town"];
			nl::node miniMap = Map["miniMap"];

			// Official tutorial maps ship miniMap canvases up to ~6000px wide.
			// Uploading those into the 8192 atlas on the main thread freezes the
			// client (black screen + Not Responding) right after enter-game.
			// Guard both the width/height metadata AND the canvas bitmap itself —
			// some packs report small metadata while canvas pixels are huge.
			constexpr int32_t kMaxCanvasEdge = 2048;
			int32_t canvas_w = miniMap ? static_cast<int32_t>(miniMap["width"]) : 0;
			int32_t canvas_h = miniMap ? static_cast<int32_t>(miniMap["height"]) : 0;
			int32_t bmp_w = 0;
			int32_t bmp_h = 0;

			if (miniMap)
			{
				nl::node canvas_node = miniMap["canvas"];

				if (canvas_node && canvas_node.data_type() == nl::node::type::bitmap)
				{
					nl::bitmap canvas_bmp = canvas_node;
					bmp_w = static_cast<int32_t>(canvas_bmp.width());
					bmp_h = static_cast<int32_t>(canvas_bmp.height());
				}
			}

			const bool meta_bad = !miniMap
				|| canvas_w <= 0 || canvas_h <= 0
				|| canvas_w > kMaxCanvasEdge || canvas_h > kMaxCanvasEdge;
			const bool bmp_bad = bmp_w <= 0 || bmp_h <= 0
				|| bmp_w > kMaxCanvasEdge || bmp_h > kMaxCanvasEdge;
			const bool canvas_too_large = meta_bad || bmp_bad;

			if (!miniMap || canvas_too_large)
			{
				has_map = false;
				type = Type::MIN;

				if (canvas_too_large && miniMap)
				{
					LOG(LOG_WARN, "UIMiniMap: skip oversized canvas meta="
						<< canvas_w << "x" << canvas_h
						<< " bmp=" << bmp_w << "x" << bmp_h
						<< " map=" << mapid);
				}
			}
			else
			{
				has_map = true;

				if (town && town.get_bool())
					type = Type::MAX;
				else
					type = user_type;
			}

			if (miniMap)
			{
				int mag = static_cast<int>(miniMap["mag"]);

				if (mag < 0)
					mag = 0;
				else if (mag > 8)
					mag = 8;

				scale = static_cast<int16_t>(std::pow(2, mag));
				center_offset = Point<int16_t>(miniMap["centerX"], miniMap["centerY"]);
			}
			else
			{
				scale = 16;
				center_offset = {};
			}

			update_text();
			update_buttons();
			update_canvas();
			update_static_markers();
			toggle_buttons();
			update_npclist();
		}

		if (type == Type::MIN)
		{
			for (Sprite sprite : min_sprites)
				sprite.update();
		}
		else if (type == Type::NORMAL)
		{
			for (Sprite sprite : normal_sprites)
				sprite.update();
		}
		else
		{
			for (Sprite sprite : max_sprites)
				sprite.update();
		}

		if (listNpc_enabled)
			for (Sprite sprite : listNpc_sprites)
				sprite.update();

		if (selected >= 0)
			selected_marker.update();

		UIElement::update();
	}

	void UIMiniMap::remove_cursor()
	{
		UIDragElement::remove_cursor();

		listNpc_slider.remove_cursor();

		UI::get().clear_tooltip(Tooltip::Parent::MINIMAP);
	}

	Cursor::State UIMiniMap::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State dstate = UIDragElement::send_cursor(clicked, cursorpos);

		if (dragged)
			return dstate;

		Point<int16_t> cursor_relative = cursorpos - position;

		if (listNpc_slider.isenabled())
			if (Cursor::State new_state = listNpc_slider.send_cursor(cursor_relative, clicked))
				return new_state;

		if (listNpc_enabled)
		{
			Point<int16_t> relative_point = cursor_relative - Point<int16_t>(10 + (type == Type::MAX ? max_dimensions : normal_dimensions).x(), 23);
			Rectangle<int16_t> list_bounds = Rectangle<int16_t>(0, LISTNPC_ITEM_WIDTH, 0, LISTNPC_ITEM_HEIGHT * 8);

			if (list_bounds.contains(relative_point))
			{
				int16_t list_index = listNpc_offset + relative_point.y() / LISTNPC_ITEM_HEIGHT;
				bool in_list = list_index < listNpc_names.size();

				if (clicked)
					select_npclist(in_list ? list_index : -1);
				else if (in_list)
					UI::get().show_text(Tooltip::Parent::MINIMAP, listNpc_full_names[list_index]);

				return Cursor::State::IDLE;
			}
		}

		bool found = false;
		auto npcs = Stage::get().get_npcs().get_npcs();

		for (auto npc = npcs->begin(); npc != npcs->end(); npc++)
		{
			Point<int16_t> npc_pos = (npc->second->get_position() + center_offset) / scale + Point<int16_t>(map_draw_origin_x, map_draw_origin_y);
			Rectangle<int16_t> marker_spot = Rectangle<int16_t>(npc_pos - Point<int16_t>(4, 8), npc_pos);

			if (type == Type::MAX)
				marker_spot.shift(Point<int16_t>(0, MAX_ADJ));

			if (marker_spot.contains(cursor_relative))
			{
				found = true;

				auto n = static_cast<Npc*>(npc->second.get());
				std::string name = n->get_name();
				std::string func = n->get_func();

				UI::get().show_map(Tooltip::Parent::MINIMAP, name, func, {}, false, false);
				break;
			}
		}

		if (!found)
		{
			for (auto& sprite : static_marker_info)
			{
				Rectangle<int16_t> marker_spot = Rectangle<int16_t>(sprite.second, sprite.second + 8);

				if (type == Type::MAX)
					marker_spot.shift(Point<int16_t>(0, MAX_ADJ));

				if (marker_spot.contains(cursor_relative))
				{
					nl::node portal_tm = Map["portal"][sprite.first]["tm"];
					std::string portal_cat = NxHelper::Map::get_map_category(portal_tm);
					nl::node portal_name = nl::nx::String["Map.img"][portal_cat][portal_tm]["mapName"];

					if (portal_name)
					{
						found = true;

						UI::get().show_map(Tooltip::Parent::MINIMAP, portal_name, "", portal_tm, false, true);
						break;
					}
				}
			}
		}

		return Cursor::State::IDLE;
	}

	void UIMiniMap::send_scroll(double yoffset)
	{
		if (listNpc_enabled && listNpc_slider.isenabled())
			listNpc_slider.send_scroll(yoffset);
	}

	void UIMiniMap::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'))
		{
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				int32_t mid = 0;
				if (tip.empty()) {
					if (auto wm = UI::get().get_element<UIWorldMap>())
						mid = wm->get_selected_map_id();
				} else {
					try { mid = std::stoi(tip); } catch (...) { return; }
				}
				if (mid > 0) {
					ChangeMapPacket(false, mid, "", false).dispatch();
					LOG(LOG_DEBUG, "UIMiniMap: SoftKey ChangeMap id=" << mid);
				}
			}, []() {}, "Map id|empty=WMap sel");
			return;
		}
		if (pressed && (keycode == 'W' || keycode == 'w'))
		{
			auto wm = UI::get().get_element<UIWorldMap>();
			if (!wm) wm = UI::get().emplace<UIWorldMap>();
			else if (!wm->is_active()) wm->makeactive();
			return;
		}
		if (has_map)
		{
			if (type < Type::MAX)
				type++;
			else
				type = Type::MIN;

			user_type = type;

			toggle_buttons();
		}
	}

	Button::State UIMiniMap::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BT_MIN:
			{
				type -= 1;
				toggle_buttons();

				return type == Type::MIN ? Button::State::DISABLED : Button::State::NORMAL;
			}
			case BT_MAX:
			{
				type += 1;
				toggle_buttons();

				return type == Type::MAX ? Button::State::DISABLED : Button::State::NORMAL;
			}
			case BT_SMALL:
			case BT_BIG:
			{
				big_map = !big_map;
				// TODO: Toggle scrolling map
				toggle_buttons();
				break;
			}
			case BT_MAP:
			{
				// 这颗钮的贴图就是「大地图」三个字（UIWindow.img/MiniMap/BtMap/normal/0，
				// 36x12 橙底白字，见 evidence/w11-map/minimap-band.md §4），本包 MiniMap
				// 节点下也只有 BtMap 一颗钮（无 BtMin/BtMax/BtSmall/BtBig/BtNpc）。
				// 所以它的语义是「打开大地图」，不是缩放小地图；小地图的
				// MIN/NORMAL/MAX 由 Map/info/town 自动决定（城镇 -> MAX）。
				// 缩放切换仍保留在 send_key 的调试分支里，便于验证三种态。
				if (auto wm = UI::get().get_element<UIWorldMap>())
				{
					if (!wm->is_active())
						wm->makeactive();
				}
				else
					UI::get().emplace<UIWorldMap>();

				break;
			}
			case BT_NPC:
			{
				set_npclist_active(!listNpc_enabled);
				break;
			}
		}

		return Button::State::NORMAL;
	}

	UIElement::Type UIMiniMap::get_type() const
	{
		return TYPE;
	}

	void UIMiniMap::update_buttons()
	{
		// Buttons may be missing when UIWindow2/UIWindow MiniMap nodes are incomplete.
		// 修复：只读访问必须用 find，禁止 map::operator[]（会插入空 unique_ptr 条目，
		// 导致 UIElement::remove_cursor 遍历时对空指针解引用崩溃 0xC0000005）。
		auto btn_width = [this](Buttons id) -> int16_t
		{
			auto it = buttons.find(id);
			return (it != buttons.end() && it->second) ? it->second->width() + 1 : 0;
		};

		bt_min_width = btn_width(Buttons::BT_MIN);
		bt_max_width = btn_width(Buttons::BT_MAX);
		bt_map_width = btn_width(Buttons::BT_MAP);

		combined_text_width = combined_text.width();
	}

	void UIMiniMap::toggle_buttons()
	{
		auto btn = [this](Buttons id) -> Button*
		{
			auto it = buttons.find(id);
			return (it != buttons.end()) ? it->second.get() : nullptr;
		};

		int16_t bt_min_x;

		if (type == Type::MIN)
		{
			if (Button* b = btn(Buttons::BT_MAP)) b->set_active(true);
			if (Button* b = btn(Buttons::BT_MAX)) b->set_active(true);
			if (Button* b = btn(Buttons::BT_MIN)) b->set_active(true);
			if (Button* b = btn(Buttons::BT_NPC)) b->set_active(false);
			if (Button* b = btn(Buttons::BT_SMALL)) b->set_active(false);
			if (Button* b = btn(Buttons::BT_BIG)) b->set_active(false);

			if (Button* b = btn(Buttons::BT_MIN))
				b->set_state(Button::State::DISABLED);

			if (Button* b = btn(Buttons::BT_MAX))
				b->set_state(has_map ? Button::State::NORMAL : Button::State::DISABLED);

			bt_min_x = combined_text_width + 11;

			if (Button* b = btn(Buttons::BT_MIN))
				b->set_position(Point<int16_t>(bt_min_x, BTN_MIN_Y));

			bt_min_x += bt_min_width;

			if (Button* b = btn(Buttons::BT_MAX))
				b->set_position(Point<int16_t>(bt_min_x, BTN_MIN_Y));

			bt_min_x += bt_max_width;

			if (Button* b = btn(Buttons::BT_MAP))
				b->set_position(Point<int16_t>(bt_min_x, BTN_MIN_Y));

			min_dimensions = Point<int16_t>(bt_min_x + bt_map_width + 7, 20);

			update_dimensions();

			dragarea = dimension;

			set_npclist_active(false);
		}
		else
		{
			bool has_npcs = Stage::get().get_npcs().get_npcs()->size() > 0;

			if (Button* b = btn(Buttons::BT_MAP)) b->set_active(true);
			if (Button* b = btn(Buttons::BT_MAX)) b->set_active(true);
			if (Button* b = btn(Buttons::BT_MIN)) b->set_active(true);
			if (Button* b = btn(Buttons::BT_NPC)) b->set_active(has_npcs);

			if (big_map)
			{
				if (Button* b = btn(Buttons::BT_BIG)) b->set_active(false);
				if (Button* b = btn(Buttons::BT_SMALL)) b->set_active(true);
			}
			else
			{
				if (Button* b = btn(Buttons::BT_BIG)) b->set_active(true);
				if (Button* b = btn(Buttons::BT_SMALL)) b->set_active(false);
			}

			if (Button* b = btn(Buttons::BT_MIN))
				b->set_state(Button::State::NORMAL);

			auto btn_at = [this](Buttons id) -> Button*
			{
				auto it = buttons.find(id);
				return (it != buttons.end()) ? it->second.get() : nullptr;
			};

			int16_t small_w = btn_at(Buttons::BT_SMALL) ? btn_at(Buttons::BT_SMALL)->width() + 1 : 0;
			int16_t npc_w = (has_npcs && btn_at(Buttons::BT_NPC)) ? btn_at(Buttons::BT_NPC)->width() : 0;

			bt_min_x = middle_right_x - (bt_min_width + small_w + bt_max_width + bt_map_width + npc_w);

			if (Button* b = btn(Buttons::BT_MIN))
				b->set_position(Point<int16_t>(bt_min_x, BTN_MIN_Y));

			bt_min_x += bt_max_width;

			if (Button* b = btn(Buttons::BT_MAX))
				b->set_position(Point<int16_t>(bt_min_x, BTN_MIN_Y));

			bt_min_x += bt_max_width;

			if (Button* b = btn(Buttons::BT_SMALL))
				b->set_position(Point<int16_t>(bt_min_x, BTN_MIN_Y));
			if (Button* b = btn(Buttons::BT_BIG))
				b->set_position(Point<int16_t>(bt_min_x, BTN_MIN_Y));

			bt_min_x += bt_max_width;

			if (Button* b = btn(Buttons::BT_MAP))
				b->set_position(Point<int16_t>(bt_min_x, BTN_MIN_Y));

			bt_min_x += bt_map_width;

			if (Button* b = btn(Buttons::BT_NPC))
				b->set_position(Point<int16_t>(bt_min_x, BTN_MIN_Y));

			if (Button* b = btn(Buttons::BT_MAX))
				b->set_state(type == Type::MAX ? Button::State::DISABLED : Button::State::NORMAL);

			set_npclist_active(listNpc_enabled && has_npcs);

			dragarea = Point<int16_t>(dimension.x(), 20);
		}
	}

	void UIMiniMap::update_text()
	{
		NxHelper::Map::MapInfo map_info = NxHelper::Map::get_map_info_by_id(mapid);
		combined_text.change_text(map_info.full_name);
		region_text.change_text(map_info.name);
		town_text.change_text(map_info.street_name);
	}

	void UIMiniMap::update_canvas()
	{
		min_sprites.clear();
		normal_sprites.clear();
		max_sprites.clear();

		if (!MiniMap)
		{
			min_dimensions = Point<int16_t>(128, 23);
			normal_dimensions = min_dimensions;
			max_dimensions = min_dimensions;
			dimension = min_dimensions;
			return;
		}

		// simpleMode 走的是本包不存在的 MiniMapSimpleMode/Window 结构；本包只有
		// MinMap/MaxMap 九切片，故无论 simpleMode 与否都用 MinMap/MaxMap。
		if (simpleMode)
			UiAssetResolve::diag_once("minimap simplemode", "MiniMap/Window absent, using MinMap/MaxMap");

		nl::node Min = MiniMap["Min"];
		nl::node Normal = MiniMap["MinMap"];
		nl::node Max = MiniMap["MaxMap"];

		// origin 补偿：让某件的可见框左上角落在 (x,y)，与 NameTag::draw_slice 同法
		// （本包 MinMap/MaxMap/Min 各件 origin 实测均为 (0,0)，补偿为恒等）。
		auto slice = [](nl::node piece, int16_t x, int16_t y, int16_t sx, int16_t sy) -> Sprite
		{
			const Point<int16_t> org = Texture(piece).get_origin();
			return Sprite(piece, DrawArgument(Point<int16_t>(x + org.x(), y + org.y()), Point<int16_t>(sx, sy)));
		};

		// 展开框九切片拼法（左上角件宽60仅装饰压在顶带；真实边框：左/右=6，顶=29，底=14）。
		auto build_frame = [&](std::vector<Sprite>& out, nl::node f,
			int16_t W, int16_t H, int16_t top, int16_t bot, int16_t tlcap)
		{
			const int16_t side = 6;
			out.emplace_back(slice(f["nw"], 0, 0, 0, 0));
			out.emplace_back(slice(f["n"], tlcap, 0, W - tlcap - side, 0));
			out.emplace_back(slice(f["ne"], W - side, 0, 0, 0));
			out.emplace_back(slice(f["w"], 0, top, 0, H - top - bot));
			out.emplace_back(slice(f["c"], side, top, W - side * 2, H - top - bot));
			out.emplace_back(slice(f["e"], W - side, top, 0, H - top - bot));
			out.emplace_back(slice(f["sw"], 0, H - bot, 0, 0));
			out.emplace_back(slice(f["s"], side, H - bot, W - side * 2, 0));
			out.emplace_back(slice(f["se"], W - side, H - bot, 0, 0));
		};

		Point<int16_t> map_dimensions(0, 0);

		// Only decode/upload the miniMap canvas when we actually draw it.
		// Giant tutorial canvases (e.g. 5944x1311) must never hit the atlas here.
		// has_map already incorporates bitmap-edge checks before Texture().
		if (has_map)
		{
			map_sprite = Texture(Map["miniMap"]["canvas"]);
			map_dimensions = map_sprite.get_dimensions();
		}
		else
		{
			map_sprite = Texture();
		}

		int16_t mark_text_width = 48 + std::max(region_text.width(), town_text.width()) + 10;

		// 内容区尺寸：宽度取地图与文字二者较大，高度至少容纳地图。
		int16_t body_w = std::max(std::max<int16_t>(106, mark_text_width - 12), map_dimensions.x());
		int16_t body_h = std::max<int16_t>(40, map_dimensions.y());

		const int16_t n_top = 29, n_bot = 14, n_cap = 60;
		const int16_t m_top = 72, m_bot = 15;
		const int16_t side = 6;

		int16_t W = body_w + side * 2;
		int16_t H_normal = body_h + n_top + n_bot;
		int16_t H_max = body_h + m_top + m_bot;

		// 地图画布居中于内容区；MAX 由 draw 端叠加 MAX_ADJ 使画布落到 72 顶带下。
		map_draw_origin_x = static_cast<int16_t>(side + std::max<int16_t>(0, (body_w - map_dimensions.x()) / 2));
		map_draw_origin_y = n_top;
		middle_right_x = W;

		// Min 三切片收缩条（左8 / 中平铺 / 右4，高20）。
		int16_t W_min = combined_text_width + bt_map_width + 18;
		int16_t body_w_min = W_min - 8 - 4;

		min_sprites.emplace_back(slice(Min["w"], 0, 0, 0, 0));
		min_sprites.emplace_back(slice(Min["c"], 8, 0, body_w_min, 0));
		min_sprites.emplace_back(slice(Min["e"], 8 + body_w_min, 0, 0, 0));
		min_dimensions = Point<int16_t>(W_min, 20);

		// Normal 展开框（MinMap 九切片）+ 地图画布。
		build_frame(normal_sprites, Normal, W, H_normal, n_top, n_bot, n_cap);

		if (has_map)
			normal_sprites.emplace_back(Sprite(Map["miniMap"]["canvas"], DrawArgument(Point<int16_t>(map_draw_origin_x, map_draw_origin_y))));

		normal_dimensions = Point<int16_t>(W, H_normal);

		// Max 城镇框（MaxMap 九切片）+ 地图画布 + 地图标记。
		build_frame(max_sprites, Max, W, H_max, m_top, m_bot, n_cap);

		if (has_map)
			max_sprites.emplace_back(Sprite(Map["miniMap"]["canvas"], DrawArgument(Point<int16_t>(map_draw_origin_x, map_draw_origin_y + MAX_ADJ))));

		// 地图标记（MapHelper.img/mark/*，全量 38x38）落在 72 顶带的浅蓝信息带内：
		// 带 = y20..65（MaxMap/nw 逐行实测），38 高垂直居中 -> y=24；左内缩 8。
		max_sprites.emplace_back(Sprite(MapHelper["mark"][Map["info"]["mapMark"]], DrawArgument(Point<int16_t>(MARK_X, MARK_Y))));

		max_dimensions = Point<int16_t>(W, H_max);
	}

	void UIMiniMap::draw_movable_markers(Point<int16_t> init_pos, float alpha) const
	{
		if (!has_map)
			return;

		Animation marker_sprite;
		Point<int16_t> sprite_offset;

		/// NPCs
		MapObjects* npcs = Stage::get().get_npcs().get_npcs();
		marker_sprite = Animation(marker["npc"]);
		sprite_offset = marker_sprite.get_dimensions() / Point<int16_t>(2, 0);

		for (auto npc = npcs->begin(); npc != npcs->end(); ++npc)
		{
			Point<int16_t> npc_pos = npc->second.get()->get_position();
			marker_sprite.draw((npc_pos + center_offset) / scale - sprite_offset + Point<int16_t>(map_draw_origin_x, map_draw_origin_y) + init_pos, alpha);
		}

		/// Other characters
		MapObjects* chars = Stage::get().get_chars().get_chars();
		marker_sprite = Animation(marker["another"]);
		sprite_offset = marker_sprite.get_dimensions() / Point<int16_t>(2, 0);

		for (auto chr = chars->begin(); chr != chars->end(); ++chr)
		{
			Point<int16_t> chr_pos = chr->second.get()->get_position();
			marker_sprite.draw((chr_pos + center_offset) / scale - sprite_offset + Point<int16_t>(map_draw_origin_x, map_draw_origin_y) + init_pos, alpha);
		}

		/// Player
		Point<int16_t> player_pos = Stage::get().get_player().get_position();
		sprite_offset = player_marker.get_dimensions() / Point<int16_t>(2, 0);
		player_marker.draw((player_pos + center_offset) / scale - sprite_offset + Point<int16_t>(map_draw_origin_x, map_draw_origin_y) + init_pos, alpha);
	}

	void UIMiniMap::update_static_markers()
	{
		static_marker_info.clear();

		if (!has_map)
			return;

		Animation marker_sprite;

		/// Portals
		nl::node portals = Map["portal"];
		marker_sprite = Animation(marker["portal"]);
		Point<int16_t> marker_offset = marker_sprite.get_dimensions() / Point<int16_t>(2, 0);

		for (nl::node portal = portals.begin(); portal != portals.end(); ++portal)
		{
			int portal_type = portal["pt"];

			if (portal_type == 2)
			{
				Point<int16_t> marker_pos = (Point<int16_t>(portal["x"], portal["y"]) + center_offset) / scale - marker_offset + Point<int16_t>(map_draw_origin_x, map_draw_origin_y);
				static_marker_info.emplace_back(portal.name(), marker_pos);
			}
		}
	}

	void UIMiniMap::set_npclist_active(bool active)
	{
		listNpc_enabled = active;

		if (!active)
			select_npclist(-1);

		update_dimensions();
	}

	void UIMiniMap::update_dimensions()
	{
		if (type == Type::MIN)
		{
			dimension = min_dimensions;
		}
		else
		{
			Point<int16_t> base_dims = type == Type::MAX ? max_dimensions : normal_dimensions;
			dimension = base_dims;

			if (listNpc_enabled)
			{
				dimension += listNpc_dimensions;
				dimension.set_y(std::max(base_dims.y(), listNpc_dimensions.y()));
			}
		}
	}

	void UIMiniMap::update_npclist()
	{
		listNpc_sprites.clear();
		listNpc_names.clear();
		listNpc_full_names.clear();
		listNpc_list.clear();
		selected = -1;
		listNpc_offset = 0;

		if (simpleMode)
			return;

		auto npcs = Stage::get().get_npcs().get_npcs();

		for (auto npc = npcs->begin(); npc != npcs->end(); ++npc)
		{
			listNpc_list.emplace_back(npc->second.get());

			auto n = static_cast<Npc*>(npc->second.get());
			std::string name = n->get_name();
			std::string func = n->get_func();

			if (func != "")
				name += " (" + func + ")";

			Text name_text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, name);

			listNpc_names.emplace_back(name_text);
			listNpc_full_names.emplace_back(name);
		}

		for (size_t i = 0; i < listNpc_names.size(); i++)
			string_format::format_with_ellipsis(listNpc_names[i], LISTNPC_TEXT_WIDTH - (listNpc_names.size() > 8 ? 0 : 20));

		const Point<int16_t> listNpc_pos = Point<int16_t>(type == Type::MAX ? max_dimensions.x() : normal_dimensions.x(), 0);
		int16_t c_stretch = 20;
		int16_t m_stretch = 102;

		if (listNpc_names.size() > 8)
		{
			listNpc_slider = Slider(
				Slider::DEFAULT_SILVER, Range<int16_t>(23, 11 + LISTNPC_ITEM_HEIGHT * 8), listNpc_pos.x() + LISTNPC_ITEM_WIDTH + 1, 8, listNpc_names.size(),
				[&](bool upwards)
				{
					int16_t shift = upwards ? -1 : 1;
					bool above = listNpc_offset + shift >= 0;
					bool below = listNpc_offset + 8 + shift <= listNpc_names.size();

					if (above && below)
						listNpc_offset += shift;
				}
			);

			c_stretch += 12;
		}
		else
		{
			listNpc_slider.setenabled(false);
			m_stretch = LISTNPC_ITEM_HEIGHT * listNpc_names.size() - 34;
			c_stretch -= 17;
		}

		listNpc_sprites.emplace_back(listNpc["c"], DrawArgument(listNpc_pos + Point<int16_t>(CENTER_START_X, M_START), Point<int16_t>(c_stretch, m_stretch)));
		listNpc_sprites.emplace_back(listNpc["w"], DrawArgument(listNpc_pos + Point<int16_t>(0, M_START), Point<int16_t>(0, m_stretch)));
		listNpc_sprites.emplace_back(listNpc["e"], DrawArgument(listNpc_pos + Point<int16_t>(CENTER_START_X + c_stretch, M_START), Point<int16_t>(0, m_stretch)));
		listNpc_sprites.emplace_back(listNpc["n"], DrawArgument(listNpc_pos + Point<int16_t>(CENTER_START_X, 0), Point<int16_t>(c_stretch, 0)));
		listNpc_sprites.emplace_back(listNpc["s"], DrawArgument(listNpc_pos + Point<int16_t>(CENTER_START_X, M_START + m_stretch), Point<int16_t>(c_stretch, 0)));
		listNpc_sprites.emplace_back(listNpc["nw"], DrawArgument(listNpc_pos + Point<int16_t>(0, 0)));
		listNpc_sprites.emplace_back(listNpc["ne"], DrawArgument(listNpc_pos + Point<int16_t>(CENTER_START_X + c_stretch, 0)));
		listNpc_sprites.emplace_back(listNpc["sw"], DrawArgument(listNpc_pos + Point<int16_t>(0, M_START + m_stretch)));
		listNpc_sprites.emplace_back(listNpc["se"], DrawArgument(listNpc_pos + Point<int16_t>(CENTER_START_X + c_stretch, M_START + m_stretch)));

		listNpc_dimensions = Point<int16_t>(CENTER_START_X * 2 + c_stretch, M_START + m_stretch + 30);

		update_dimensions();
	}

	void UIMiniMap::draw_npclist(Point<int16_t> minimap_dims, float alpha) const
	{
		Animation npc_marker = Animation(marker["npc"]);

		for (Sprite sprite : listNpc_sprites)
			sprite.draw(position, alpha);

		Point<int16_t> listNpc_pos = position + Point<int16_t>(minimap_dims.x() + 10, 23);

		for (int8_t i = 0; i + listNpc_offset < listNpc_list.size() && i < 8; i++)
		{
			if (selected - listNpc_offset == i)
			{
				ColorBox highlight = ColorBox(LISTNPC_ITEM_WIDTH - (listNpc_slider.isenabled() ? 0 : 30), LISTNPC_ITEM_HEIGHT, Color::Name::YELLOW, 1.0f);
				highlight.draw(listNpc_pos);
			}

			npc_marker.draw(DrawArgument(listNpc_pos + Point<int16_t>(0, 2), false, npc_marker.get_dimensions() / 2), alpha);
			listNpc_names[listNpc_offset + i].draw(DrawArgument(listNpc_pos + Point<int16_t>(14, -2)));

			listNpc_pos.shift_y(LISTNPC_ITEM_HEIGHT);
		}

		if (listNpc_slider.isenabled())
			listNpc_slider.draw(position);

		if (selected >= 0)
		{
			Point<int16_t> npc_pos =
				(listNpc_list[selected]->get_position() + center_offset) / scale +
				Point<int16_t>(map_draw_origin_x, map_draw_origin_y - npc_marker.get_dimensions().y() + (type == Type::MAX ? MAX_ADJ : 0));

			selected_marker.draw(position + npc_pos, 0.5f);
		}
	}

	void UIMiniMap::select_npclist(int16_t choice)
	{
		if (selected == choice)
			return;

		if (selected >= 0 && selected < listNpc_names.size())
			listNpc_names[selected].change_color(Color::Name::WHITE);

		if (choice > listNpc_names.size() || choice < 0)
		{
			selected = -1;
		}
		else
		{
			selected = choice != selected ? choice : -1;

			if (selected >= 0)
				listNpc_names[selected].change_color(Color::Name::BLACK);
		}
	}
}