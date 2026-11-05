//////////////////////////////////////////////////////////////////////////////////
//	UIMapSearchPanel
//////////////////////////////////////////////////////////////////////////////////
#include "UIMapSearchPanel.h"

#include "../../Audio/Audio.h"
#include "../../Graphics/Geometry.h"
#include "../../MapleStory.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"

#include <utility>

namespace ms
{
	namespace
	{
		const char* TAB_KEYS[static_cast<size_t>(UIMapSearchPanel::Tab::LENGTH)] = {
			"all", "map", "mob", "drop", "npc"
		};

		const char* BTN_STATE_NAMES[static_cast<size_t>(UIMapSearchPanel::BtnVisual::LENGTH)] = {
			"normal", "mouseOver", "pressed", "disabled"
		};

		// layout.json — detail card / results list / cost (panel-local coords).
		const int16_t DETAIL_X = 18;
		const int16_t DETAIL_Y = 112;
		const int16_t DETAIL_W = 223;
		const int16_t DETAIL_H = 116;
		const int16_t DETAIL_ICON = 48;

		const int16_t LIST_HDR_X = 18;
		const int16_t LIST_HDR_Y = 112;

		const int16_t LIST_X = 18;
		const int16_t LIST_Y = 130;
		const int16_t LIST_W = 223;
		const int16_t LIST_H = 343;
		const int16_t RROW_H = 28; // matches list_row_selected.png (216x28)
		const int16_t RVISIBLE = LIST_H / RROW_H;

		const int16_t LIST_SCROLL_X = 246;
		const int16_t LIST_SCROLL_Y = 130;
		const int16_t LIST_SCROLL_W = 9;
		const int16_t LIST_SCROLL_H = 343;

		const int16_t COST_X = 18;
		const int16_t COST_Y = 480;
	}

	Rectangle<int16_t> UIMapSearchPanel::PngButton::bounds(Point<int16_t> panel_pos) const
	{
		Point<int16_t> tl = panel_pos + offset;
		return Rectangle<int16_t>(tl, tl + size);
	}

	void UIMapSearchPanel::PngButton::draw(Point<int16_t> panel_pos) const
	{
		BtnVisual v = enabled ? visual : BtnVisual::DISABLED;
		const LocalPngTexture& tex = textures[static_cast<size_t>(v)];

		if (tex.is_valid())
			tex.draw(panel_pos + offset);
	}

	Cursor::State UIMapSearchPanel::PngButton::send_cursor(bool clicked, Point<int16_t> cursorpos, Point<int16_t> panel_pos)
	{
		if (!enabled || !bounds(panel_pos).contains(cursorpos))
		{
			if (visual != BtnVisual::PRESSED)
				visual = BtnVisual::NORMAL;

			return Cursor::State::IDLE;
		}

		if (clicked)
		{
			visual = BtnVisual::PRESSED;
			return Cursor::State::CLICKING;
		}

		visual = BtnVisual::MOUSEOVER;
		return Cursor::State::CANCLICK;
	}

	UIMapSearchPanel::UIMapSearchPanel() :
		visible(true),
		panel_height(HEIGHT),
		active_tab(Tab::ALL),
		result_offset(0),
		selected_map_id(0),
		index_loaded(false),
		search_text_dim(Point<int16_t>(SEARCH_W - 8, 12)),
		search_offset(Point<int16_t>(SEARCH_X + 4, SEARCH_Y + 3))
	{
		size_t atlas = ATLAS_BASE;

		// v3 管线底板（原生 Border 拼框 + 标题烤字，D5）。候选顺序：客户端运行时 →
		// v3 管线产物 → v1 资源（带详情卡残留，仅兜底）。
		chrome.load(atlas++, {
			"UI/MapSearch/search_panel_backgrnd.png",
			"docs/features/map-search-ui/ui/search_panel_backgrnd/v3_pipeline/chrome/search_panel_backgrnd.png",
			"docs/features/map-search-ui/resources/chrome/search_panel_backgrnd.png",
		});

		if (!chrome.is_valid())
			LOG(LOG_WARN, "[UIMapSearchPanel] chrome PNG missing");

		// Inventory-style: Tab/disabled/{key} + Tab/enabled/{key} → LocalTwoSpriteButton
		for (uint8_t i = 0; i < static_cast<uint8_t>(Tab::LENGTH); i++)
		{
			LocalPngTexture dis, en;
			std::string key = TAB_KEYS[i];
			load_png(dis, atlas++, "Tab/disabled/" + key + ".png");
			load_png(en, atlas++, "Tab/enabled/" + key + ".png");

			Point<int16_t> tab_pos(TAB_X[i], TAB_Y);
			tab_buttons[i] = std::make_unique<LocalTwoSpriteButton>(std::move(dis), std::move(en), tab_pos);
		}

		if (tab_buttons[static_cast<size_t>(Tab::ALL)])
			tab_buttons[static_cast<size_t>(Tab::ALL)]->set_state(Button::State::PRESSED);

		bt_back.offset = Point<int16_t>(BACK_X, BACK_Y);
		bt_back.size = Point<int16_t>(BACK_W, BACK_H);

		for (uint8_t s = 0; s < static_cast<uint8_t>(BtnVisual::LENGTH); s++)
			load_png(bt_back.textures[s], atlas++, std::string("BtBack_") + BTN_STATE_NAMES[s] + ".png");

		bt_teleport.offset = Point<int16_t>(TELE_X, TELE_Y);
		bt_teleport.size = Point<int16_t>(TELE_W, TELE_H);

		for (uint8_t s = 0; s < static_cast<uint8_t>(BtnVisual::LENGTH); s++)
			load_png(bt_teleport.textures[s], atlas++, std::string("BtTeleport_") + BTN_STATE_NAMES[s] + ".png");

		bt_teleport.enabled = false;
		bt_teleport.visual = BtnVisual::DISABLED;
		teleport_handler = nullptr;

		Rectangle<int16_t> field(search_offset, search_offset + search_text_dim);
		search_text = Textfield(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, field, 24);
		search_text.set_state(Textfield::State::NORMAL);

		load_png(list_row_sel, atlas++, "list_row_selected.png");

		result_slider = Slider(
			Slider::Type::DEFAULT_SILVER,
			Range<int16_t>(LIST_SCROLL_Y, LIST_SCROLL_Y + LIST_SCROLL_H),
			LIST_SCROLL_X,
			RVISIBLE,
			1,
			[this](bool upwards)
			{
				int16_t next = result_offset + (upwards ? -1 : 1);
				int16_t maxoff = static_cast<int16_t>(filtered.size()) - RVISIBLE;

				if (maxoff < 0)
					maxoff = 0;

				if (next < 0)
					next = 0;
				else if (next > maxoff)
					next = maxoff;

				result_offset = next;
			}
		);
		result_slider.setenabled(false);

		// 详情卡已移除（todo 5）：detail_frame/detail_name/detail_street 不再加载与持有。
		// 吸附面板不是 UIElement，绑定走自带的 LocaleBindings（与 UIMapMobPanel 等同源）。
		// 三条都在 ctor 写死、之后只 draw => 满足绑定判据。cost_label 的「0」是本包还没接
		// 传送费用的假初值（全文只有这一处写入，永远不会被数据改写），所以连值一起进表；
		// 日后接真费用时，把这一行换成事件驱动的 Locale::format 现取即可。
		list_header = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK);
		locale_binds.bind(list_header, "mapsearch.list_header");
		no_data = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::GRAY);
		locale_binds.bind(no_data, "mapsearch.no_data");
		cost_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK);
		locale_binds.bind(cost_label, "mapsearch.cost", "100000");

		// 面板标题：D5 已把「地图搜索」烤进 v3 底板顶脊（与信息面板同管线），
		// 程序不再重复绘制；title_text 保留绑定仅作兜底数据（不 draw）。
		title_text = Text(Text::Font::A13B, Text::Alignment::CENTER, Color::Name::BLACK, "地图搜索");
		locale_binds.bind(title_text, "mapsearch.title");

		// 搜索框接收中文：启用系统 IME，UTF-8 直接进字图缓存（思源宋体含 CJK）。
		search_text.set_accept_unicode(true);

		apply_filter();

		LOG(LOG_INFO, "[UIMapSearchPanel] ready chrome=" << (chrome.is_valid() ? 1 : 0)
			<< " tabs=LocalTwoSpriteButton w=" << WIDTH);
	}

	bool UIMapSearchPanel::load_png(LocalPngTexture& tex, size_t atlas_id, const std::string& filename)
	{
		return tex.load(atlas_id, {
			"UI/MapSearch/" + filename,
			"docs/features/map-search-ui/resources/chrome/" + filename,
			"docs/features/map-search-ui/resources/tabs/enabled/" + filename,
			"docs/features/map-search-ui/resources/tabs/disabled/" + filename,
			"docs/features/map-search-ui/resources/tabs/" + filename,
			"docs/features/map-search-ui/resources/buttons/" + filename,
		});
	}

	void UIMapSearchPanel::set_position(Point<int16_t> pos)
	{
		position = pos;
	}

	void UIMapSearchPanel::layout_for_height(int16_t worldmap_height)
	{
		panel_height = worldmap_height > 200 ? worldmap_height : HEIGHT;

		int16_t tele_y = static_cast<int16_t>(panel_height - (HEIGHT - TELE_Y));

		if (tele_y < 200)
			tele_y = TELE_Y;

		bt_teleport.offset = Point<int16_t>(TELE_X, tele_y);
	}

	void UIMapSearchPanel::set_content_height(int16_t worldmap_height)
	{
		layout_for_height(worldmap_height);
	}

	bool UIMapSearchPanel::is_visible() const
	{
		return visible;
	}

	Point<int16_t> UIMapSearchPanel::get_dimensions() const
	{
		return Point<int16_t>(WIDTH, panel_height);
	}

	Rectangle<int16_t> UIMapSearchPanel::get_bounds() const
	{
		return Rectangle<int16_t>(position, position + get_dimensions());
	}

	void UIMapSearchPanel::set_tab(Tab tab)
	{
		if (tab == active_tab)
			return;

		uint8_t old_i = static_cast<uint8_t>(active_tab);
		uint8_t new_i = static_cast<uint8_t>(tab);

		if (tab_buttons[old_i])
			tab_buttons[old_i]->set_state(Button::State::NORMAL);

		if (tab_buttons[new_i])
			tab_buttons[new_i]->set_state(Button::State::PRESSED);

		active_tab = tab;
		apply_filter();
		LOG(LOG_DEBUG, "[UIMapSearchPanel] tab=" << static_cast<int>(tab));
	}

	void UIMapSearchPanel::draw(float) const
	{
		if (!visible)
			return;

		if (chrome.is_valid())
			chrome.draw(position, WIDTH, panel_height);

		for (uint8_t i = 0; i < static_cast<uint8_t>(Tab::LENGTH); i++)
		{
			if (tab_buttons[i])
				tab_buttons[i]->draw(position);
		}

		bt_back.draw(position);
		bt_teleport.draw(position);
		// D5：标题烤进底板，程序侧不画（防双字）。绑定仍在，供日后切回无字底板复用。
		(void)title_text;
		search_text.draw(Point<int16_t>(0, 0));

		// ---- Results list ----
		list_header.draw(position + Point<int16_t>(LIST_HDR_X, LIST_HDR_Y));

		const bool is_map = (active_tab == Tab::ALL || active_tab == Tab::MAP);

		if (is_map)
		{
			for (int16_t r = 0; r < RVISIBLE; r++)
			{
				size_t idx = static_cast<size_t>(result_offset) + static_cast<size_t>(r);

				if (idx >= filtered.size())
					break;

				const auto& e = index_store[filtered[idx]];
				Point<int16_t> rowpos = position + Point<int16_t>(LIST_X, LIST_Y + r * RROW_H);

				// 列表行底纹：奇偶行交替浅灰，区分条目（风格对齐技能窗列表）。
				ColorBox rowbg(LIST_W, RROW_H, Color::Name::GRAY, (r % 2 == 0) ? 0.06f : 0.12f);
				rowbg.draw(rowpos);

				if (static_cast<int32_t>(e.id) == selected_map_id && list_row_sel.is_valid())
					list_row_sel.draw(rowpos, LIST_W, RROW_H);

				Text row(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, e.name);
				row.draw(rowpos + Point<int16_t>(4, 3));
			}

			if (result_slider.isenabled())
				result_slider.draw(position);
		}
		else
		{
			// 局部 Text 每帧重建 => 只能就地现取，绑定表管不到（对象一帧就没了）。
			Text nodata2(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::GRAY,
				Locale::text("mapsearch.tab_unloaded"));
			nodata2.draw(position + Point<int16_t>(LIST_X, LIST_Y + 4));
		}

		// ---- Cost ----
		cost_label.draw(position + Point<int16_t>(COST_X, COST_Y));
	}

	void UIMapSearchPanel::update()
	{
		// 刷新点放在 visible 判断之前：隐藏期间语言换了也要把绑定表跟上，否则面板重新
		// 打开的那一帧会先闪一次旧语言文案。代次没变只有一次整数比较，不构成热点。
		if (locale_binds.refresh())
			LOG(LOG_INFO, "Locale refresh gen=" << Locale::get().generation() << " rebound "
				<< locale_binds.count() << " labels (" << locale_binds.hostname() << ")");

		if (!visible)
			return;

		Point<int16_t> field_pos = position + search_offset;
		search_text.update(field_pos, search_text_dim);

		std::string cur = search_text.get_text();

		if (cur != last_query)
		{
			last_query = cur;
			apply_filter();
		}
	}

	void UIMapSearchPanel::remove_cursor()
	{
		for (uint8_t i = 0; i < static_cast<uint8_t>(Tab::LENGTH); i++)
		{
			if (!tab_buttons[i])
				continue;

			if (tab_buttons[i]->get_state() == Button::State::MOUSEOVER)
				tab_buttons[i]->set_state(Button::State::NORMAL);
		}

		if (bt_back.visual != BtnVisual::DISABLED)
			bt_back.visual = BtnVisual::NORMAL;

		if (bt_teleport.enabled && bt_teleport.visual != BtnVisual::DISABLED)
			bt_teleport.visual = BtnVisual::NORMAL;
	}

	Cursor::State UIMapSearchPanel::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (!visible || !get_bounds().contains(cursorpos))
		{
			remove_cursor();
			return Cursor::State::IDLE;
		}

		if (Cursor::State st = search_text.send_cursor(cursorpos, clicked))
			return st;

		// Inventory-style tab cursor: NORMAL↔MOUSEOVER, click → PRESSED via set_tab
		Cursor::State tab_ret = Cursor::State::IDLE;

		for (uint8_t i = 0; i < static_cast<uint8_t>(Tab::LENGTH); i++)
		{
			auto* btn = tab_buttons[i].get();

			if (!btn || !btn->is_active())
				continue;

			if (btn->bounds(position).contains(cursorpos))
			{
				if (btn->get_state() == Button::State::NORMAL)
				{
					Sound(Sound::Name::BUTTONOVER).play();
					btn->set_state(Button::State::MOUSEOVER);
					tab_ret = Cursor::State::CANCLICK;
				}
				else if (btn->get_state() == Button::State::MOUSEOVER)
				{
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();
						set_tab(static_cast<Tab>(i));
						tab_ret = Cursor::State::CLICKING;
					}
					else
					{
						tab_ret = Cursor::State::CANCLICK;
					}
				}
				else if (btn->get_state() == Button::State::PRESSED)
				{
					tab_ret = Cursor::State::CANCLICK;
				}
			}
			else if (btn->get_state() == Button::State::MOUSEOVER)
			{
				btn->set_state(Button::State::NORMAL);
			}
		}

		if (tab_ret != Cursor::State::IDLE)
			return tab_ret;

		Cursor::State back_st = bt_back.send_cursor(clicked, cursorpos, position);

		if (back_st != Cursor::State::IDLE)
		{
			if (clicked && bt_back.visual == BtnVisual::PRESSED)
			{
				Sound(Sound::Name::BUTTONCLICK).play();
				if (back_handler)
					back_handler();
				LOG(LOG_DEBUG, "[UIMapSearchPanel] BtBack");
			}

			return back_st;
		}

		Cursor::State tele_st = bt_teleport.send_cursor(clicked, cursorpos, position);

		if (tele_st != Cursor::State::IDLE)
		{
			if (clicked && bt_teleport.enabled && bt_teleport.visual == BtnVisual::PRESSED)
			{
				Sound(Sound::Name::BUTTONCLICK).play();
				if (teleport_handler)
					teleport_handler();
				LOG(LOG_DEBUG, "[UIMapSearchPanel] BtTeleport");
			}
			return tele_st;
		}

		// ---- Results slider + list ----
		if (result_slider.isenabled())
			if (Cursor::State s = result_slider.send_cursor(cursorpos - position, clicked))
				return s;

		const bool is_map = (active_tab == Tab::ALL || active_tab == Tab::MAP);

		if (is_map && !filtered.empty())
		{
			Rectangle<int16_t> larea(
				position + Point<int16_t>(LIST_X, LIST_Y),
				position + Point<int16_t>(LIST_X + LIST_W, LIST_Y + LIST_H));

			if (larea.contains(cursorpos))
			{
				int16_t ly = cursorpos.y() - larea.top();
				int16_t row = ly / RROW_H;
				size_t idx = static_cast<size_t>(result_offset) + static_cast<size_t>(row);

				if (idx < filtered.size())
				{
					if (clicked)
						select_result(idx);

					return Cursor::State::CANCLICK;
				}
			}
		}

		return Cursor::State::IDLE;
	}

	void UIMapSearchPanel::set_teleport_enabled(bool enable)
	{
		bt_teleport.enabled = enable;
		bt_teleport.visual = enable ? BtnVisual::NORMAL : BtnVisual::DISABLED;
	}

	void UIMapSearchPanel::set_teleport_handler(std::function<void()> handler)
	{
		teleport_handler = std::move(handler);
	}

	void UIMapSearchPanel::set_back_handler(std::function<void()> handler)
	{
		back_handler = std::move(handler);
	}

	void UIMapSearchPanel::set_select_callback(std::function<void(int32_t)> callback)
	{
		on_select = std::move(callback);
	}

	void UIMapSearchPanel::set_selected_map(int32_t id)
	{
		selected_map_id = id;

		for (size_t i = 0; i < filtered.size(); i++)
		{
			if (index_store[filtered[i]].id == id)
			{
				result_offset = static_cast<int16_t>(i);
				break;
			}
		}
	}

	void UIMapSearchPanel::rebuild_index()
	{
		if (index_loaded)
			return;

		index_store = NxHelper::Map::get_map_search_index();
		index_loaded = true;
	}

	void UIMapSearchPanel::apply_filter()
	{
		rebuild_index();
		filtered.clear();

		const bool is_map = (active_tab == Tab::ALL || active_tab == Tab::MAP);

		if (!is_map)
		{
			// Mob / Drop / NPC indexes are not yet populated client-side (see feature README).
			result_slider.setenabled(false);
			result_offset = 0;
			return;
		}

		std::string q = string_format::tolower(last_query);

		for (size_t i = 0; i < index_store.size(); i++)
		{
			if (q.empty())
			{
				filtered.push_back(i);
			}
			else
			{
				std::string nm = string_format::tolower(index_store[i].name);
				std::string st = string_format::tolower(index_store[i].street);

				if (nm.find(q) != std::string::npos || st.find(q) != std::string::npos)
					filtered.push_back(i);
			}
		}

		int16_t maxoff = static_cast<int16_t>(filtered.size()) - RVISIBLE;

		if (maxoff < 0)
			maxoff = 0;

		if (result_offset > maxoff)
			result_offset = maxoff;

		result_slider.setrows(RVISIBLE, filtered.size() > static_cast<size_t>(RVISIBLE)
			? static_cast<int16_t>(filtered.size()) : RVISIBLE);
		result_slider.setenabled(filtered.size() > static_cast<size_t>(RVISIBLE));
	}

	void UIMapSearchPanel::select_result(size_t pos)
	{
		if (pos >= filtered.size())
			return;

		int32_t id = index_store[filtered[pos]].id;
		selected_map_id = id;

		LOG(LOG_INFO, "[UIMapSearchPanel] select map_id=" << id);

		if (on_select)
			on_select(id);
	}

	void UIMapSearchPanel::set_doubleclick_handler(std::function<void(int32_t)> handler)
	{
		on_doubleclick = std::move(handler);
	}

	// 双击结果行：命中区与 send_cursor 的列表分支同一套几何（LIST_X/Y/W/H + RROW_H），
	// 于是「选中」与「跳分区」落在同一行上，不会出现两套坐标各说各话。
	void UIMapSearchPanel::doubleclick(Point<int16_t> cursorpos)
	{
		if (!visible || (active_tab != Tab::ALL && active_tab != Tab::MAP) || filtered.empty())
			return;

		Rectangle<int16_t> larea(
			position + Point<int16_t>(LIST_X, LIST_Y),
			position + Point<int16_t>(LIST_X + LIST_W, LIST_Y + LIST_H));

		if (!larea.contains(cursorpos))
			return;

		int16_t ly = cursorpos.y() - larea.top();
		int16_t row = ly / RROW_H;
		size_t idx = static_cast<size_t>(result_offset) + static_cast<size_t>(row);

		if (idx >= filtered.size())
			return;

		int32_t id = index_store[filtered[idx]].id;

		LOG(LOG_INFO, "[UIMapSearchPanel] doubleclick map_id=" << id);

		if (on_doubleclick)
			on_doubleclick(id);
	}
}
