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
#include "UINotice.h"

#include "../UI.h"

#include "../../Constants.h"
#include "../../Gameplay/Stage.h"
#include "../../IO/Components/MapleButton.h"
#include "../../Net/Packets/GameplayPackets.h"
#include "../../Net/Packets/MiscOutPackets.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"
#include "../../Util/ResolutionTiers.h"
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
		//
		// round96（P3 · corewindow/D10）083 对照：本端这套「由贴图件推导」与 083 的
		// 「外框硬编码」差 12x3，来源已坐实——
		//   083 `CWorldMapDlg::CreateWorldMapDlg @0x9eb366:139`
		//        `CDialog::CreateDlg(this, a2: 67, a3: 0, a4: 666, a5: 524, a6: 10, a7: 1, a8: 0)`
		//        → 外层 666x524；
		//   083 `CWorldMapDlg::OnCreate @0x9eb75b`
		//        `sub_9ECC4A(a1: 12, a2: 34, a3: 642, a4: 472)` → 内容区 (12,34) 642x472；
		//   本端 Backgrnd 640x470 + top 33 / bot 18 / side 7 → 654x521，内容 (7,33)。
		// 083 的 666 = 12 + 642 + 12、524 = 34 + 472 + 18，即两侧各留 12px 内边距；
		// 而 Border/3..4 实测仅 7px 宽，照搬硬编码会让贴图与内容之间留 5px 空隙（视觉破坏）。
		// 故本轮**不改几何**（`DECISIONS.md:101` 的 s9 布局优先口径不变），只把差值来源钉在这里；
		// 真要改须连同九切片重排一起做，真机对齐项见 PLT §55-3。
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

		// 两侧把手箭头：复用原生 Basic.img/BtHide 的 normal 帧（13x12 黄绿底白左箭头）。
		// BtHide 是按钮容器（normal/mouseOver/pressed 子帧），必须取到 bitmap 帧，
		// 直接 Texture(容器) 会 is_valid()==false 导致箭头不渲染。
		collapse_arrow = nl::nx::UI["Basic.img"]["BtHide"]["normal"]["0"];

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
		// 地图名检索支持中文：开启系统 IME（focus_textfield 据此启动 SDL 文本输入）。
		search_text.set_accept_unicode(true);

		mob_panel.set_select_callback([this](int32_t mob_id)
		{
			on_mob_selected(mob_id);
		});
		mob_panel.set_content_height(bg_dimensions.y());
		map_search_panel.set_content_height(bg_dimensions.y());
		map_search_panel.set_select_callback([this](int32_t id)
		{
			select_map_spot(id);
		});
		// todo 8：双击结果行 = 跳到该图所属分区并选中它。
		map_search_panel.set_doubleclick_handler([this](int32_t id)
		{
			jump_to_map_region(id);
		});

		if (kUseMapInfoPanel)
		{
			info_panel.set_select_callback([this](int32_t mob_id)
			{
				on_mob_selected(mob_id);
			});
			info_panel.set_content_height(bg_dimensions.y());
		}

		// 旧 WorldMapSearch 轨在本 v83 包缺素材（backgrnd 为空）→ 默认隐藏，避免右侧漂浮无框搜索文字；
		// 搜索改由新 UIMapSearchPanel 接管。素材齐备时仍按原生启用旧轨。
		set_search(search_background.is_valid());

		dragarea = Point<int16_t>(bg_dimensions_x, 20);

		// 多分辨率：PosMAP(313,64) 是 1280×720 基准下的居中读数；其它档位按新视口居中。
		// 1280×720 保持原生绝对位置，不覆盖用户拖拽后保存的 Setting<PosMAP>。
		if (Constants::Constants::get().get_viewwidth() != ResolutionTiers::LOGIN_W
			|| Constants::Constants::get().get_viewheight() != ResolutionTiers::LOGIN_H)
			center_on_screen();
	}

	void UIWorldMap::update_screen(int16_t, int16_t)
	{
		// 分辨率实时切换：UIState 只对 active 元素广播，故这里只重锚不缩放（1x 原生像素）。
		center_on_screen();
		ensure_panels_on_screen();
	}

	void UIWorldMap::draw(float alpha) const
	{
		if (mob_panel.is_visible())
			mob_panel.draw(alpha);

		if (kUseMapInfoPanel && left_expand > 0.001f && info_panel.is_visible())
			info_panel.draw(alpha);

		if (right_expand > 0.001f && map_search_panel.is_visible())
			map_search_panel.draw(alpha);

		UIElement::draw_sprites(alpha);

		if (search)
		{
			search_background.draw(position + background_dimensions);
			search_notice.draw(position + background_dimensions);
			search_text.draw(Point<int16_t>(1, -5));
		}

		// 底图本身带 origin (320, 235)，加回 origin 才能把可见左上角落在 base_position。
		base_img.draw(position + base_position + base_img.get_origin());

		// 两侧收起/展开箭头：已移入地图框内，必须在底图之后绘制，否则被底图覆盖；
		// 悬停有 tip（见 send_cursor），点击切换收起/展开。
		draw_tab(get_left_tab_rect(), left_state, true);
		draw_tab(get_right_tab_rect(), right_state, false);

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

		// 玩家层（0x178）：选中地图当前在线玩家。逐行画在底图右上区。
		// 服务端 WorldMapPlayersHandler.MAX_PLAYERS=40；可视区按行高截断。
		// 注：原生 BeiDou 大地图无「在线玩家列表」UI（IDA 仅见 MapLink/linkMap
		// 加载），本层为 s9 自定义协议挂接，布局取右上可读区而非原生节点。
		if (players_mapid == selected_map_id && !map_players.empty())
		{
			int16_t line_y = 24;
			Point<int16_t> anchor = position + base_anchor + Point<int16_t>(-138, -150);

			{
				Text hdr(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK,
					Locale::format("worldmap.players.online", std::to_string(map_players.size())));
				hdr.draw(anchor + Point<int16_t>(0, line_y));
				line_y += 16;
			}

			for (const auto& p : map_players)
			{
				// lv/名字/频道；短行文本按半透明底衬保证可读（底图是彩色地图）
				std::string line = std::to_string(static_cast<int>(p.level)) + " " + p.name
					+ " c" + std::to_string(static_cast<int>(p.channel));

				ColorBox row(150, 12, Color::Name::WHITE, 0.55f);
				row.draw(anchor + Point<int16_t>(-2, line_y - 1));

				Text t(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, line);
				t.draw(anchor + Point<int16_t>(0, line_y));

				line_y += 13;
				if (line_y > 220)
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
		advance_side(left_expand, left_state);
		advance_side(right_expand, right_state);
		layout_panels();
		mob_panel.update();
		map_search_panel.update();
		info_panel.update();

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
		else
		{
			// 打开大地图：默认两侧收起，纯看地图更干净（低分辨率也不会被挤爆）。
			left_state = SideState::COLLAPSED;
			right_state = SideState::COLLAPSED;
			left_expand = 0.0f;
			right_expand = 0.0f;
			left_pinned = false;
			right_pinned = false;
			layout_panels();
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
				request_teleport(selected_map_id);
				LOG(LOG_DEBUG, "UIWorldMap: SoftKey teleport selected=" << selected_map_id);
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& q) {
					if (q.empty()) return;
					try {
						int32_t mapid = std::stoi(q);
						if (mapid > 0) {
							select_map_spot(mapid);
							request_teleport(mapid);
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
				request_teleport(selected_map_id);
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
		else if (keycode == 'X' || keycode == 'x')
		{
			toggle_left();
		}
		else if (keycode == 'P' || keycode == 'p')
		{
			set_search(!search);
			toggle_right();
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

		if (kUseMapInfoPanel && info_panel.is_visible() && info_panel.get_bounds().contains(cursor_position))
			return true;

		if (map_search_panel.is_visible() && map_search_panel.get_bounds().contains(cursor_position))
			return true;

		// 两侧收起/展开把手（已移入地图框内）也必须并入命中区，
		// 否则 get_front 派发不到本元素 → 点击把手变成「点击窗外」（地图失活/穿透）。
		if (get_left_tab_rect().contains(cursor_position)
			|| get_right_tab_rect().contains(cursor_position))
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
				toggle_right();
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

		if (kUseMapInfoPanel)
			info_panel.remove_cursor();
	}

	Cursor::State UIWorldMap::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		// 两侧把手：悬停显示 tip，点击切换收起/展开。把手已移入地图框内，
		// 优先判定以拦截边缘的点击。
		const Rectangle<int16_t> ltab = get_left_tab_rect();
		const Rectangle<int16_t> rtab = get_right_tab_rect();

		if (ltab.contains(cursorpos) || rtab.contains(cursorpos))
		{
			if (ltab.contains(cursorpos))
				UI::get().show_text(Tooltip::Parent::WORLDMAP,
					left_state == SideState::COLLAPSED ? Locale::text("uiworldmap.557") : Locale::text("uiworldmap.557"));
			else
				UI::get().show_text(Tooltip::Parent::WORLDMAP,
					right_state == SideState::COLLAPSED ? Locale::text("uiworldmap.560") : Locale::text("uiworldmap.560"));

			if (clicked)
			{
				if (ltab.contains(cursorpos))
					toggle_left();
				else
					toggle_right();
			}

			return clicked ? Cursor::State::CLICKING : Cursor::State::CANCLICK;
		}

		if (mob_panel.is_visible())
		{
			Cursor::State mob_state = mob_panel.send_cursor(clicked, cursorpos);

			if (mob_state != Cursor::State::IDLE || mob_panel.get_bounds().contains(cursorpos))
				return mob_state == Cursor::State::IDLE ? Cursor::State::IDLE : mob_state;
		}

		if (kUseMapInfoPanel && info_panel.is_visible())
		{
			Cursor::State info_state = info_panel.send_cursor(clicked, cursorpos);

			if (info_state != Cursor::State::IDLE || info_panel.get_bounds().contains(cursorpos))
				return info_state == Cursor::State::IDLE ? Cursor::State::IDLE : info_state;
		}

		if (map_search_panel.is_visible())
		{
			Cursor::State search_state = map_search_panel.send_cursor(clicked, cursorpos);

			if (search_state != Cursor::State::IDLE || map_search_panel.get_bounds().contains(cursorpos))
				return search_state == Cursor::State::IDLE ? Cursor::State::IDLE : search_state;
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

	Point<int16_t> UIWorldMap::get_map_info_panel_pos() const
	{
		// 左吸附：面板右缘紧贴地图左缘（把手已入框内，不再留 TAB_W 间隙），收起时整体左移缓动。
		const int16_t slide = static_cast<int16_t>(UIMapInfoPanel::WIDTH * (1.0f - left_expand));
		return position - Point<int16_t>(UIMapInfoPanel::WIDTH + slide, 0);
	}

	Point<int16_t> UIWorldMap::get_map_search_panel_pos() const
	{
		// 右吸附：搜索面板左缘紧贴地图右缘（+搜索栏宽度，无把手间隙），收起时整体右移缓动。
		const int16_t slide = static_cast<int16_t>(UIMapSearchPanel::WIDTH * (1.0f - right_expand));
		return position + Point<int16_t>(bg_dimensions.x() + search_rail_width() + slide, 0);
	}

	void UIWorldMap::sync_side_panels()
	{
		if (mob_panel.is_visible() || map_search_panel.is_visible()
			|| (kUseMapInfoPanel && info_panel.is_visible()))
			ensure_panels_on_screen();

		mob_panel.set_content_height(bg_dimensions.y());
		map_search_panel.set_content_height(bg_dimensions.y());

		if (kUseMapInfoPanel)
		{
			info_panel.set_content_height(bg_dimensions.y());
			info_panel.set_position(get_map_info_panel_pos());
		}

		mob_panel.set_position(get_mob_panel_pos());
		map_search_panel.set_position(get_map_search_panel_pos());
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

		if (kUseMapInfoPanel)
			info_panel.clear();

		map_search_panel.set_teleport_enabled(false);
		map_search_panel.set_teleport_handler(nullptr);
	}

	void UIWorldMap::expand_left()
	{
		if (left_state == SideState::EXPANDED || left_state == SideState::EXPANDING)
			return;

		left_state = SideState::EXPANDING;
		left_pinned = false;

		// 用户直接点把手展开时，若已选中地图就补上信息面板内容，保证有可见反馈。
		if (kUseMapInfoPanel && selected_map_id != 0)
			info_panel.set_map(selected_map_id);

		layout_panels();
	}

	void UIWorldMap::collapse_left()
	{
		if (left_state == SideState::COLLAPSED || left_state == SideState::COLLAPSING)
			return;

		left_state = SideState::COLLAPSING;
		left_pinned = true;
		layout_panels();
	}

	void UIWorldMap::toggle_left()
	{
		if (left_state == SideState::COLLAPSED || left_state == SideState::COLLAPSING)
			expand_left();
		else
			collapse_left();
	}

	void UIWorldMap::expand_right()
	{
		if (right_state == SideState::EXPANDED || right_state == SideState::EXPANDING)
			return;

		right_state = SideState::EXPANDING;
		right_pinned = false;
		layout_panels();
	}

	void UIWorldMap::collapse_right()
	{
		if (right_state == SideState::COLLAPSED || right_state == SideState::COLLAPSING)
			return;

		right_state = SideState::COLLAPSING;
		right_pinned = true;
		layout_panels();
	}

	void UIWorldMap::toggle_right()
	{
		if (right_state == SideState::COLLAPSED || right_state == SideState::COLLAPSING)
			expand_right();
		else
			collapse_right();
	}

	void UIWorldMap::advance_side(float& value, SideState& state)
	{
		if (state == SideState::EXPANDING)
		{
			value += Constants::TIMESTEP / SIDE_EXPAND_MS;

			if (value >= 1.0f)
			{
				value = 1.0f;
				state = SideState::EXPANDED;
			}
		}
		else if (state == SideState::COLLAPSING)
		{
			value -= Constants::TIMESTEP / SIDE_EXPAND_MS;

			if (value <= 0.0f)
			{
				value = 0.0f;
				state = SideState::COLLAPSED;
			}
		}
	}

	void UIWorldMap::layout_panels()
	{
		if (kUseMapInfoPanel)
			info_panel.set_position(get_map_info_panel_pos());

		map_search_panel.set_position(get_map_search_panel_pos());
	}

	Rectangle<int16_t> UIWorldMap::get_left_tab_rect() const
	{
		const int16_t ty = (bg_dimensions.y() - TAB_H) / 2;
		// 把手移入世界地图框内（左侧内缘），不再外凸在窗体外。
		return Rectangle<int16_t>(
			position.x(), position.x() + TAB_W,
			position.y() + ty, position.y() + ty + TAB_H);
	}

	Rectangle<int16_t> UIWorldMap::get_right_tab_rect() const
	{
		const int16_t ty = (bg_dimensions.y() - TAB_H) / 2;
		return Rectangle<int16_t>(
			position.x() + bg_dimensions.x() - TAB_W, position.x() + bg_dimensions.x(),
			position.y() + ty, position.y() + ty + TAB_H);
	}

	void UIWorldMap::draw_tab(const Rectangle<int16_t>& rect, SideState state, bool left) const
	{
		if (!collapse_arrow.is_valid())
			return;

		const Point<int16_t> tl = rect.get_left_top();
		const Point<int16_t> dim = collapse_arrow.get_dimensions();

		// 箭头放大 2x（原 13x12 → 26x24），居中绘制在把手命中区内。
		// 把手已移入世界地图框内，不再绘制外凸棕色背景，仅保留箭头作展开/收起指示。
		const float scale = 2.0f;
		const int16_t aw = static_cast<int16_t>(dim.x() * scale);
		const int16_t ah = static_cast<int16_t>(dim.y() * scale);

		// 收起时箭头指向面板所在侧（外），展开时指向内（表示收起动作）。
		const bool expanded_like = (state == SideState::EXPANDING || state == SideState::EXPANDED);
		const bool flip = left ? expanded_like : !expanded_like;

		const Point<int16_t> ap = tl + Point<int16_t>(
			(TAB_W - aw) / 2 + (flip ? aw : 0),
			(TAB_H - ah) / 2);
		collapse_arrow.draw(DrawArgument(ap, flip ? -scale : scale, scale));
	}

	void UIWorldMap::ensure_panels_on_screen()
	{
		const int16_t margin = 4;
		const int16_t view_w = Constants::Constants::get().get_viewwidth();

		// 仅当面板实际展开（缓动进度 > 0）才参与贴边布局，默认收起时不挤动地图。
		const bool left_visible = kUseMapInfoPanel
			? (info_panel.is_visible() && left_expand > 0.001f)
			: mob_panel.is_visible();
		const int16_t left_width = kUseMapInfoPanel ? UIMapInfoPanel::WIDTH : UIMapMobPanel::WIDTH;

		if (left_visible)
		{
			const int16_t need_left = left_width + margin;

			if (position.x() < need_left)
				position = Point<int16_t>(need_left, position.y());
		}

		{
			int16_t right_extra = 0;

			if (map_search_panel.is_visible() && right_expand > 0.001f)
				right_extra = static_cast<int16_t>(right_extra + UIMapSearchPanel::WIDTH);

			if (right_extra > 0)
			{
				const int16_t right_edge = static_cast<int16_t>(
					position.x() + bg_dimensions.x() + search_rail_width() + right_extra + margin);

				if (right_edge > view_w)
				{
					int16_t shift = static_cast<int16_t>(right_edge - view_w);
					position = Point<int16_t>(position.x() - shift, position.y());

					if (left_visible)
					{
						const int16_t need_left = left_width + margin;

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

		if (kUseMapInfoPanel)
		{
			// 信息面板接管左侧：旧 mob_panel 仅保留代码，不显示（D2 并存）。
			mob_panel.clear();
			info_panel.set_map(map_id);
			// 选中地图点 = 触发左侧信息面板展开（除非用户手动收起过）。
			if (!left_pinned)
				expand_left();
		}
		else
		{
			mob_panel.set_map(map_id);
		}

		// 选中地图点：左侧信息面板接管（含掉落），右侧仅搜索。
		ensure_panels_on_screen();
		mob_panel.set_position(get_mob_panel_pos());

		if (kUseMapInfoPanel)
			info_panel.set_position(get_map_info_panel_pos());

		map_search_panel.set_position(get_map_search_panel_pos());
		map_search_panel.set_teleport_enabled(map_id != 0);
		map_search_panel.set_teleport_handler([this]() {
			request_teleport(selected_map_id);
		});
		map_search_panel.set_selected_map(map_id);

		// 0x178 玩家层：选中即请求（服务端 CACHE_TTL_MS=750，短时重复命中缓存）。
		// SoftKey「L」保留为手动刷新/无选中时输入 mapid 的调试通道。
		if (map_id > 0)
		{
			WorldMapPlayersPacket(map_id).dispatch();
			map_players.clear();
			players_mapid = 0;	// 应答到达前视为无数据，避免显示上一图的列表
			LOG(LOG_DEBUG, "[UIWorldMap] WORLD_MAP_PLAYERS request map=" << map_id);
		}

		LOG(LOG_INFO, "[UIWorldMap] selected_map_id=" << selected_map_id);
	}

	void UIWorldMap::doubleclick(Point<int16_t> cursorpos)
	{
		// 目前只有右侧搜索结果行接了双击（todo 8：跳分区）。面板自身判命中区，
		// 未命中就什么都不做 —— 与 send_cursor 的列表分支共用同一套几何。
		if (map_search_panel.is_visible())
			map_search_panel.doubleclick(cursorpos);
	}

	// mapid → 所属分区：原生分区名就是 "WorldMap0" + mapid/10000000，与本文件
	// update() 里算玩家当前图 parent 的口径完全一致，因此不需要另建反向索引表。
	void UIWorldMap::jump_to_map_region(int32_t map_id)
	{
		if (map_id <= 0)
			return;

		const std::string region = "WorldMap0" + std::to_string(map_id / 10000000);

		// 分区不存在时不跳：update_world 对缺失节点会回落到 WorldMap.img 默认图，
		// 静默换图会让人以为「跳错了地方」，所以这里显式拒绝并留日志。
		if (!nl::nx::Map["WorldMap"][region + ".img"])
		{
			LOG(LOG_WARN, "[UIWorldMap] jump_to_map_region: no such region " << region << " (map=" << map_id << ")");
			return;
		}

		Sound(Sound::Name::SELECTMAP).play();

		clear_side_panels();
		update_world(region);
		select_map_spot(map_id);

		LOG(LOG_INFO, "[UIWorldMap] jumped to region " << region << " map=" << map_id);
	}

	void UIWorldMap::request_teleport(int32_t map_id)
	{
		if (map_id == 0)
			return;

		constexpr int64_t MAPWARP_COST = 100000;
		if (Stage::get().get_player().get_inventory().get_meso() < MAPWARP_COST)
		{
			// 事件驱动现取（不进绑定表）：这条文案每次点击都可能带不同数值，
			// 属于「有第二个写入点且由事件驱动」那一类，按绑定判据不许上表。
			UI::get().emplace<UIOk>(
				Locale::format("mapwarp.insufficient", std::to_string(MAPWARP_COST)),
				[](bool) {});
			return;
		}

		GeneralChatPacket("!mapwarp " + std::to_string(map_id), true).dispatch();
		LOG(LOG_DEBUG, "UIWorldMap: request_teleport !mapwarp map=" << map_id);
	}

	void UIWorldMap::set_map_players(int32_t map_id, std::vector<WorldMapPlayer>&& players)
	{
		// 乱序陈旧应答丢弃：只接受当前选中图的回包。
		if (map_id != selected_map_id)
			return;

		map_players = std::move(players);
		players_mapid = map_id;
	}

	void UIWorldMap::on_mob_selected(int32_t mob_id)
	{
		if (kUseMapInfoPanel)
		{
			info_panel.set_mob(mob_id);
			// 选中怪物：展开左侧信息 + 右侧掉落（除非用户手动收起过）。
			if (!left_pinned)
				expand_left();
			if (!right_pinned)
				expand_right();
		}

		ensure_panels_on_screen();
		map_search_panel.set_position(get_map_search_panel_pos());

		if (kUseMapInfoPanel)
			info_panel.set_position(get_map_info_panel_pos());
	}
}
