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
#include "UIWorldMap.h"

#include "UISoftKey.h"
#include "UIMiniMap.h"

#include "../UI.h"

#include "../../Constants.h"
#include "../../Gameplay/Stage.h"
#include "../../IO/Components/MapleButton.h"
#include "../../Net/Packets/GameplayPackets.h"
#include "../../Net/Packets/MiscOutPackets.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#include "../../MapleStory.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIWorldMap::UIWorldMap() :
		UIDragElement<PosMAP>(),
		search_text_dim(Point<int16_t>(82, 14)),
		selected_map_id(0)
	{
		nl::node close = UiAssetResolve::close_btn();

		nl::node WorldMap = UiAssetResolve::window("WorldMap");
		nl::node WorldMapSearch = WorldMap["WorldMapSearch"];
		nl::node Border = WorldMap["Border"];

		if (!UiAssetResolve::usable(WorldMap) || !UiAssetResolve::usable(Border))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			UiAssetResolve::diag_once("worldmap border", "UIWindow.img/WorldMap/Border missing");
			return;
		}

		nl::node backgrnd = WorldMapSearch["backgrnd"];
		nl::node MapHelper = nl::nx::Map["MapHelper.img"]["worldMap"];

		cur_pos = MapHelper["curPos"];

		for (size_t i = 0; i < MAPSPOT_TYPE_MAX; i++)
			npc_pos[i] = MapHelper["npcPos" + std::to_string(i)];

		// 大地图窗框：Border{0..7} 拼九切片（件→边框映射与算式见 evidence/w11-map/coords.md）。
		// 底图固定 640x470（WorldMap.img BaseImg），顶/底/侧带高宽从各 Border 件 WxH 推出。
		const int16_t content_w = 640;
		const int16_t content_h = 470;
		const int16_t top_h = Texture(Border["0"]).height();   // 33
		const int16_t bot_h = Texture(Border["5"]).height();   // 18
		const int16_t side_w = Texture(Border["3"]).width();   // 7
		const int16_t tl_w = Texture(Border["0"]).width();     // 60
		const int16_t frame_w = content_w + side_w * 2;         // 654
		const int16_t frame_h = content_h + top_h + bot_h;      // 521
		const Point<int16_t> content_topleft(side_w, top_h);    // 底图左上角

		// topleft = 该件可见框左上角（屏幕相对窗口），按各件 origin 补偿。
		auto place = [&](const std::string& key, Point<int16_t> topleft, Point<int16_t> stretch)
		{
			nl::node piece = Border[key];

			if (!piece)
			{
				UiAssetResolve::diag_once("worldmap border " + key, "missing Border/" + key);
				return;
			}

			sprites.emplace_back(piece, DrawArgument(topleft + Texture(piece).get_origin(), stretch));
		};

		place("0", Point<int16_t>(0, 0), Point<int16_t>(0, 0));                               // 左上角
		place("1", Point<int16_t>(tl_w, 0), Point<int16_t>(frame_w - tl_w - side_w, 0));       // 上边
		place("2", Point<int16_t>(frame_w - side_w, 0), Point<int16_t>(0, 0));                 // 右上角
		place("3", Point<int16_t>(0, top_h), Point<int16_t>(0, content_h));                    // 左边
		place("4", Point<int16_t>(frame_w - side_w, top_h), Point<int16_t>(0, content_h));     // 右边
		place("5", Point<int16_t>(0, frame_h - bot_h), Point<int16_t>(0, 0));                  // 左下角
		place("6", Point<int16_t>(side_w, frame_h - bot_h), Point<int16_t>(content_w, 0));     // 下边
		place("7", Point<int16_t>(frame_w - side_w, frame_h - bot_h), Point<int16_t>(0, 0));   // 右下角

		search_background = backgrnd;
		search_notice = WorldMapSearch["notice"];

		bg_dimensions = Point<int16_t>(frame_w, frame_h);
		bg_search_dimensions = search_background.get_dimensions();

		int16_t bg_dimensions_x = bg_dimensions.x();
		int16_t bg_search_dimensions_x = bg_search_dimensions.x();

		background_dimensions = Point<int16_t>(bg_dimensions_x, 0);

		base_position = content_topleft;
		// base_img 还没加载时先退化成画布左上角，update_world() 会补上底图 origin。
		base_anchor = content_topleft;

		Point<int16_t> close_dimensions = Point<int16_t>(bg_dimensions_x - 22, 4);

		if (close)
			buttons[Buttons::BT_CLOSE] = std::make_unique<MapleButton>(close, close_dimensions);
		if (WorldMap["BtSearch"])
			buttons[Buttons::BT_SEARCH] = std::make_unique<MapleButton>(WorldMap["BtSearch"]);
		if (WorldMap["BtAutoFly_1"])
			buttons[Buttons::BT_AUTOFLY] = std::make_unique<MapleButton>(WorldMap["BtAutoFly_1"]);
		if (WorldMap["BtNaviRegister"])
			buttons[Buttons::BT_NAVIREG] = std::make_unique<MapleButton>(WorldMap["BtNaviRegister"]);
		if (close)
			buttons[Buttons::BT_SEARCH_CLOSE] = std::make_unique<MapleButton>(close, close_dimensions + Point<int16_t>(bg_search_dimensions_x, 0));
		if (WorldMapSearch["BtAllsearch"])
			buttons[Buttons::BT_ALLSEARCH] = std::make_unique<MapleButton>(WorldMapSearch["BtAllsearch"], background_dimensions);

		int16_t search_limit = 8;

		search_text = Textfield(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Rectangle<int16_t>(get_search_pos(), get_search_pos() + search_text_dim), search_limit);

		mob_panel.set_select_callback([this](int32_t mob_id)
		{
			on_mob_selected(mob_id);
		});
		mob_panel.set_content_height(bg_dimensions.y());
		map_search_panel.set_content_height(bg_dimensions.y());
		drop_panel.set_content_height(bg_dimensions.y());

		set_search(true);

		dragarea = Point<int16_t>(bg_dimensions_x, 20);
	}

	void UIWorldMap::draw(float alpha) const
	{
		if (mob_panel.is_visible())
			mob_panel.draw(alpha);

		if (map_search_panel.is_visible())
			map_search_panel.draw(alpha);

		if (drop_panel.is_visible())
			drop_panel.draw(alpha);

		UIElement::draw_sprites(alpha);

		if (search)
		{
			search_background.draw(position + background_dimensions);
			search_notice.draw(position + background_dimensions);
			search_text.draw(Point<int16_t>(1, -5));
		}

		// 底图本身带 origin (320, 235)，加回 origin 才能把可见左上角落在 base_position。
		base_img.draw(position + base_position + base_img.get_origin());

		if (link_images.size() > 0)
		{
			for (auto& iter : buttons)
			{
				if (const auto button = iter.second.get())
				{
					if (iter.first >= Buttons::BT_LINK0 && button->get_state() == Button::State::MOUSEOVER)
					{
						if (link_images.find(iter.first) != link_images.end())
						{
							link_images.at(iter.first).draw(position + base_anchor);
							break;
						}
					}
				}
			}
		}

		if (show_path_img)
			path_img.draw(position + base_anchor);

		for (auto spot : map_spots)
		{
			// spot 存的是「点位锚点」：marker 贴图以自己的 origin 落在这个点上。
			Point<int16_t> spot_pos = spot.first + position + base_anchor;

			if (selected_map_id != 0)
			{
				for (int32_t id : spot.second.map_ids)
				{
					if (id == selected_map_id)
					{
						Point<int16_t> marker_dim = spot.second.marker.get_dimensions();
						ColorBox highlight(marker_dim.x() + 8, marker_dim.y() + 8, Color::Name::YELLOW, 0.35f);
						// 光晕包住 marker 的可见框（左上角 = 锚点 - marker.origin），四边各外扩 4。
						highlight.draw(spot_pos - spot.second.marker.get_origin() - Point<int16_t>(4, 4));
						break;
					}
				}
			}

			spot.second.marker.draw(spot_pos);
		}

		bool found = false;

		if (!found)
		{
			for (auto spot : map_spots)
			{
				for (auto map_id : spot.second.map_ids)
				{
					if (map_id == mapid)
					{
						found = true;
						npc_pos[spot.second.type].draw(spot.first + position + base_anchor, alpha);
						cur_pos.draw(spot.first + position + base_anchor, alpha);
						break;
					}
				}

				if (found)
					break;
			}
		}

		UIElement::draw_buttons(alpha);
	}

	void UIWorldMap::update()
	{
		int32_t mid = Stage::get().get_mapid();

		if (mid != mapid)
		{
			mapid = mid;

			int32_t prefix = mapid / 10000000;
			std::string parent = "WorldMap0" + std::to_string(prefix);
			user_map = parent;

			update_world(user_map);
		}

		search_text.update(get_search_pos(), search_text_dim);

		for (size_t i = 0; i < MAPSPOT_TYPE_MAX; i++)
			npc_pos[i].update(1);

		cur_pos.update();

		sync_side_panels();
		mob_panel.update();
		map_search_panel.update();
		drop_panel.update();

		UIElement::update();
	}

	void UIWorldMap::toggle_active()
	{
		UIElement::toggle_active();

		if (!active)
		{
			clear_side_panels();
			set_search(true);
			update_world(user_map);
		}
	}

	void UIWorldMap::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape)
		{
			if (search)
			{
				set_search(false);
			}
			else
			{
				if (parent_map == "")
				{
					clear_side_panels();
					toggle_active();

					update_world(user_map);
				}
				else
				{
					Sound(Sound::Name::SELECTMAP).play();

					clear_side_panels();
					update_world(parent_map);
				}
			}
			return;
		}
		if (!pressed) return;
		if (keycode == 'S' || keycode == 's' || keycode == 'F' || keycode == 'f')
		{
			set_search(true);
			if (selected_map_id != 0) {
				ChangeMapPacket(false, selected_map_id, "", false).dispatch();
				LOG(LOG_DEBUG, "UIWorldMap: SoftKey teleport selected=" << selected_map_id);
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& q) {
					if (q.empty()) return;
					try {
						int32_t mapid = std::stoi(q);
						if (mapid > 0) {
							select_map_spot(mapid);
							ChangeMapPacket(false, mapid, "", false).dispatch();
							LOG(LOG_DEBUG, "UIWorldMap: SoftKey teleport map=" << mapid);
							return;
						}
					} catch (...) {}
					search_text.change_text(q);
					LOG(LOG_DEBUG, "UIWorldMap: SoftKey search=" << q);
				}, []() {}, "mapid|search");
			}
		}
		else if (keycode == 'T' || keycode == 't' || keycode == 13)
		{
			if (selected_map_id != 0)
			{
				ChangeMapPacket(false, selected_map_id, "", false).dispatch();
				LOG(LOG_DEBUG, "UIWorldMap: teleport selected=" << selected_map_id);
			}
		}
		else if (keycode == 'M' || keycode == 'm')
		{
			auto mm = UI::get().get_element<UIMiniMap>();
			if (!mm)
				mm = UI::get().emplace<UIMiniMap>(Stage::get().get_player().get_stats());
			else if (!mm->is_active())
				mm->makeactive();
		}
		else if (keycode == 'P' || keycode == 'p')
		{
			set_search(!map_search_panel.is_visible());
		}
		else if (keycode == 'L' || keycode == 'l')
		{
			int32_t mid = selected_map_id;
			if (mid <= 0)
				mid = Stage::get().get_mapid();
			if (mid > 0) {
				WorldMapPlayersPacket(mid).dispatch();
				LOG(LOG_DEBUG, "UIWorldMap: Players map=" << mid);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t id = std::stoi(s);
						if (id > 0) {
							WorldMapPlayersPacket(id).dispatch();
							LOG(LOG_DEBUG, "UIWorldMap: SoftKey Players map=" << id);
						}
					} catch (...) {}
				}, []() {}, "Players mapid");
			}
		}
	}

	UIElement::Type UIWorldMap::get_type() const
	{
		return TYPE;
	}

	bool UIWorldMap::is_in_range(Point<int16_t> cursor_position) const
	{
		if (UIElement::is_in_range(cursor_position))
			return true;

		if (mob_panel.is_visible() && mob_panel.get_bounds().contains(cursor_position))
			return true;

		if (map_search_panel.is_visible() && map_search_panel.get_bounds().contains(cursor_position))
			return true;

		if (drop_panel.is_visible() && drop_panel.get_bounds().contains(cursor_position))
			return true;

		return false;
	}

	Button::State UIWorldMap::button_pressed(uint16_t buttonid)
	{
		if (buttonid >= Buttons::BT_LINK0)
		{
			clear_side_panels();
			update_world(link_maps[buttonid]);

			return Button::State::IDENTITY;
		}

		switch (buttonid)
		{
			case Buttons::BT_CLOSE:
				clear_side_panels();
				deactivate();
				return Button::State::NORMAL;
			case Buttons::BT_SEARCH:
				set_search(!search);
				return Button::State::NORMAL;
			case Buttons::BT_SEARCH_CLOSE:
				set_search(false);
				return Button::State::NORMAL;
			default:
				return Button::State::DISABLED;
		}
	}

	void UIWorldMap::remove_cursor()
	{
		UIDragElement::remove_cursor();

		UI::get().clear_tooltip(Tooltip::Parent::WORLDMAP);

		show_path_img = false;
		mob_panel.remove_cursor();
		map_search_panel.remove_cursor();
		drop_panel.remove_cursor();
	}

	Cursor::State UIWorldMap::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (mob_panel.is_visible())
		{
			Cursor::State mob_state = mob_panel.send_cursor(clicked, cursorpos);

			if (mob_state != Cursor::State::IDLE || mob_panel.get_bounds().contains(cursorpos))
				return mob_state == Cursor::State::IDLE ? Cursor::State::IDLE : mob_state;
		}

		if (map_search_panel.is_visible())
		{
			Cursor::State search_state = map_search_panel.send_cursor(clicked, cursorpos);

			if (search_state != Cursor::State::IDLE || map_search_panel.get_bounds().contains(cursorpos))
				return search_state == Cursor::State::IDLE ? Cursor::State::IDLE : search_state;
		}

		if (drop_panel.is_visible())
		{
			Cursor::State drop_state = drop_panel.send_cursor(clicked, cursorpos);

			if (drop_state != Cursor::State::IDLE || drop_panel.get_bounds().contains(cursorpos))
				return drop_state == Cursor::State::IDLE ? Cursor::State::IDLE : drop_state;
		}

		if (Cursor::State new_state = search_text.send_cursor(cursorpos, clicked))
			return new_state;

		show_path_img = false;

		for (auto path : map_spots)
		{
			// 判定框 = 点位可见框外扩 4（与选中光晕同一块区域），
			// 之前的 `- 10` 是拿错的锚点空间硬凑出来的补偿。
			Point<int16_t> p = path.first + position + base_anchor - path.second.marker.get_origin() - Point<int16_t>(4, 4);
			Point<int16_t> d = p + path.second.marker.get_dimensions() + Point<int16_t>(8, 8);
			Rectangle<int16_t> abs_bounds = Rectangle<int16_t>(p, d);

			if (!abs_bounds.contains(cursorpos))
				continue;

			path_img = path.second.path;
			show_path_img = path_img.is_valid();

			int32_t spot_map_id = path.second.map_ids.empty() ? 0 : path.second.map_ids[0];

			UI::get().show_map(Tooltip::Parent::WORLDMAP, path.second.title, path.second.description, spot_map_id, path.second.bolded, false);

			if (clicked && spot_map_id != 0)
				select_map_spot(spot_map_id);

			return clicked ? Cursor::State::CLICKING : Cursor::State::CANCLICK;
		}

		return UIDragElement::send_cursor(clicked, cursorpos);
	}

	void UIWorldMap::set_search(bool enable)
	{
		search = enable;

		if (auto it = buttons.find(Buttons::BT_SEARCH_CLOSE); it != buttons.end() && it->second)
			it->second->set_active(enable);
		if (auto it = buttons.find(Buttons::BT_ALLSEARCH); it != buttons.end() && it->second)
			it->second->set_active(enable);

		if (enable)
		{
			search_text.set_state(Textfield::State::NORMAL);
			dimension = bg_dimensions + Point<int16_t>(bg_search_dimensions.x(), 0);
		}
		else
		{
			search_text.set_state(Textfield::State::DISABLED);
			dimension = bg_dimensions;
		}

		sync_side_panels();
	}

	void UIWorldMap::update_world(std::string map)
	{
		nl::node WorldMap = nl::nx::Map["WorldMap"][map + ".img"];

		if (!WorldMap)
			WorldMap = nl::nx::Map["WorldMap"]["WorldMap.img"];

		base_img = WorldMap["BaseImg"][0];
		// 所有 640x470 底图的 origin 都是 (320, 235)（012 为 469 高、013/014 的 y=236、
		// 014 的 x=321），即「底图中心」；MapList/spot、MapList/path、MapLink/linkImg
		// 都是相对这个中心点存的偏移，所以画点位/路线/跳转图时要加 base_anchor
		// 而不是 base_position，否则整套点位会偏 (-320, -235)。
		base_anchor = base_position + base_img.get_origin();
		parent_map = WorldMap["info"]["parentMap"].get_string();

		link_images.clear();
		link_maps.clear();

		for (auto& iter : buttons)
			if (const auto button = iter.second.get())
				if (iter.first >= Buttons::BT_LINK0)
					button->set_active(false);

		size_t i = Buttons::BT_LINK0;

		for (nl::node link : WorldMap["MapLink"])
		{
			nl::node l = link["link"];
			Texture link_image = l["linkImg"];

			link_images[i] = link_image;
			link_maps[i] = l["linkMap"].get_string();

			buttons[i] = std::make_unique<AreaButton>(base_anchor - link_image.get_origin(), link_image.get_dimensions());
			buttons[i]->set_active(true);

			i++;
		}

		nl::node mapImage = nl::nx::Map["MapHelper.img"]["worldMap"]["mapImage"];

		map_spots.clear();

		for (nl::node list : WorldMap["MapList"])
		{
			nl::node desc = list["desc"];
			nl::node mapNo = list["mapNo"];
			nl::node path = list["path"];
			nl::node spot = list["spot"];
			nl::node title = list["title"];
			nl::node type = list["type"];
			nl::node marker = mapImage[type];
			
			bool bolded = !desc.get_string().empty();

			std::vector<int32_t> map_ids;

			for (nl::node map_no : mapNo)
				map_ids.push_back(map_no);

			if (!desc && !title)
			{
				NxHelper::Map::MapInfo map_info = NxHelper::Map::get_map_info_by_id(mapNo[0]);
				bolded = !map_info.description.empty();

				map_spots.emplace_back(std::make_pair<Point<int16_t>, MapSpot>(spot, { map_info.description, path, map_info.full_name, type, marker, bolded, map_ids }));
			}
			else
			{
				map_spots.emplace_back(std::make_pair<Point<int16_t>, MapSpot>(spot, { desc, path, title, type, marker, bolded, map_ids }));
			}
		}
	}

	Point<int16_t> UIWorldMap::get_search_pos()
	{
		return position + Point<int16_t>(bg_dimensions.x() + 13, 25);
	}

	int16_t UIWorldMap::search_rail_width() const
	{
		return search ? bg_search_dimensions.x() : 0;
	}

	Point<int16_t> UIWorldMap::get_mob_panel_pos() const
	{
		// Left-adsorb: flush against the world-map chrome.
		return position - Point<int16_t>(UIMapMobPanel::WIDTH, 0);
	}

	Point<int16_t> UIWorldMap::get_map_search_panel_pos() const
	{
		// Outside original WorldMapSearch rail (decision 1B).
		return position + Point<int16_t>(bg_dimensions.x() + search_rail_width(), 0);
	}

	Point<int16_t> UIWorldMap::get_drop_panel_pos() const
	{
		// Right of the new map-search panel (Border + search rail + MapSearch width).
		return position + Point<int16_t>(
			bg_dimensions.x() + search_rail_width() + UIMapSearchPanel::WIDTH, 0);
	}

	void UIWorldMap::sync_side_panels()
	{
		if (mob_panel.is_visible() || map_search_panel.is_visible() || drop_panel.is_visible())
			ensure_panels_on_screen();

		mob_panel.set_content_height(bg_dimensions.y());
		map_search_panel.set_content_height(bg_dimensions.y());
		drop_panel.set_content_height(bg_dimensions.y());
		mob_panel.set_position(get_mob_panel_pos());
		map_search_panel.set_position(get_map_search_panel_pos());
		drop_panel.set_position(get_drop_panel_pos());
		map_search_panel.set_back_handler([this]() {
			clear_side_panels();
			set_search(false);
			LOG(LOG_DEBUG, "UIWorldMap: MapSearch BtBack");
		});
	}

	void UIWorldMap::clear_side_panels()
	{
		selected_map_id = 0;
		mob_panel.clear();
		drop_panel.clear();
		map_search_panel.set_teleport_enabled(false);
		map_search_panel.set_teleport_handler(nullptr);
	}

	void UIWorldMap::ensure_panels_on_screen()
	{
		const int16_t margin = 4;
		const int16_t view_w = Constants::Constants::get().get_viewwidth();

		if (mob_panel.is_visible())
		{
			const int16_t need_left = UIMapMobPanel::WIDTH + margin;

			if (position.x() < need_left)
				position = Point<int16_t>(need_left, position.y());
		}

		{
			int16_t right_extra = 0;

			if (map_search_panel.is_visible())
				right_extra = static_cast<int16_t>(right_extra + UIMapSearchPanel::WIDTH);

			if (drop_panel.is_visible())
				right_extra = static_cast<int16_t>(right_extra + UIMapDropPanel::WIDTH);

			if (right_extra > 0)
			{
				const int16_t right_edge = static_cast<int16_t>(
					position.x() + bg_dimensions.x() + search_rail_width() + right_extra + margin);

				if (right_edge > view_w)
				{
					int16_t shift = static_cast<int16_t>(right_edge - view_w);
					position = Point<int16_t>(position.x() - shift, position.y());

					if (mob_panel.is_visible())
					{
						const int16_t need_left = UIMapMobPanel::WIDTH + margin;

						if (position.x() < need_left)
							position = Point<int16_t>(need_left, position.y());
					}
				}
			}
		}
	}

	void UIWorldMap::select_map_spot(int32_t map_id)
	{
		selected_map_id = map_id;
		mob_panel.set_map(map_id);
		// Drop panel stays visible with empty/select prompt until a mob row is chosen.
		drop_panel.set_mob(0);
		ensure_panels_on_screen();
		mob_panel.set_position(get_mob_panel_pos());
		map_search_panel.set_position(get_map_search_panel_pos());
		drop_panel.set_position(get_drop_panel_pos());
		map_search_panel.set_teleport_enabled(map_id != 0);
		map_search_panel.set_teleport_handler([this]() {
			if (selected_map_id == 0) return;
			ChangeMapPacket(false, selected_map_id, "", false).dispatch();
			LOG(LOG_DEBUG, "UIWorldMap: BtTeleport map=" << selected_map_id);
		});

		LOG(LOG_INFO, "[UIWorldMap] selected_map_id=" << selected_map_id);
	}

	void UIWorldMap::on_mob_selected(int32_t mob_id)
	{
		drop_panel.set_mob(mob_id);
		ensure_panels_on_screen();
		map_search_panel.set_position(get_map_search_panel_pos());
		drop_panel.set_position(get_drop_panel_pos());
	}
}
