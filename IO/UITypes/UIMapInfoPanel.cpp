////////////////////////////////////////////////////////////////////////////////
//	UIMapInfoPanel — left-docked map info panel for UIWorldMap
////////////////////////////////////////////////////////////////////////////////
#include "UIMapInfoPanel.h"

#include "../UI.h"

#include "../../Character/Inventory/Inventory.h"
#include "../../Gameplay/Stage.h"
#include "../../MapleStory.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <iomanip>
#include <sstream>

namespace ms
{
	namespace
	{
		Texture first_frame(nl::node anim)
		{
			if (!anim)
				return {};

			if (nl::node zero = anim["0"])
				return Texture(zero);

			auto it = anim.begin();

			if (it != anim.end())
				return Texture(*it);

			return {};
		}
	}

	// Layout constants (mirror docs/features/map-info-panel/resources/layout.json).
	static constexpr int16_t UPPER_X = 18;
	static constexpr int16_t UPPER_Y = 42;
	static constexpr int16_t UPPER_W = 189; // list content width
	static constexpr int16_t UPPER_H = 237;
	static constexpr int16_t MOB_ROW_H = 78;
	static constexpr int16_t MOB_VISIBLE = 3;
	static constexpr int16_t ICON_BOX = 40;
	static constexpr int16_t UPPER_SCROLL_X = 210;
	static constexpr int16_t UPPER_SCROLL_Y = 44;
	static constexpr int16_t UPPER_SCROLL_H = 233;

	static constexpr int16_t TAB_Y = 285;
	static constexpr int16_t TAB_H = 18;
	static constexpr int16_t TAB_W[4] = { 50, 50, 50, 53 };
	static constexpr int16_t TAB_X[4] = { 18, 68, 118, 168 };

	static constexpr int16_t LOWER_X = 18;
	static constexpr int16_t LOWER_Y = 309;
	static constexpr int16_t LOWER_W = 203;
	static constexpr int16_t LOWER_H = 174;
	static constexpr int16_t GRID_COLS = 5;
	static constexpr int16_t GRID_VISIBLE = 5;
	static constexpr int16_t SLOT = 32;
	static constexpr int16_t SLOT_GAP = 2;
	static constexpr int16_t GRID_X = 30;
	static constexpr int16_t GRID_Y = 312;
	static constexpr int16_t LOWER_SCROLL_X = 210;
	static constexpr int16_t LOWER_SCROLL_Y = 311;
	static constexpr int16_t LOWER_SCROLL_H = 170;

	static constexpr int16_t MESO_X = 18;
	static constexpr int16_t MESO_Y = 489;
	static constexpr int16_t MESO_W = 159;
	static constexpr int16_t MESO_H = 16;
	static constexpr int16_t MESO_LABEL_X = 181;

	static constexpr char TAB_NAMES[4][9] = { "all", "equip", "consume", "etc" };

	UIMapInfoPanel::UIMapInfoPanel() :
		visible(false),
		map_id(0),
		selected_mob_id(0),
		selected_index(-1),
		mob_offset(0),
		drop_offset(0),
		panel_height(HEIGHT),
		active_tab(Tab::ALL)
	{
		chrome.load(ATLAS_ID, {
			"UI/MapInfo/info_panel_backgrnd.png",
			"docs/features/map-info-panel/resources/chrome/info_panel_backgrnd.png",
		});

		if (!chrome.is_valid())
			LOG(LOG_WARN, "[UIMapInfoPanel] chrome PNG missing — panel will be empty frame");

		row_selected.load(ATLAS_ID, {
			"UI/MapInfo/list_row_selected.png",
			"docs/features/map-info-panel/resources/chrome/list_row_selected.png",
		});
		slot.load(ATLAS_ID, {
			"UI/MapInfo/slot.png",
			"docs/features/map-info-panel/resources/chrome/slot.png",
		});

		for (size_t i = 0; i < static_cast<size_t>(Tab::LENGTH); i++)
		{
			LocalPngTexture nsrc;
			LocalPngTexture ssrc;

			nsrc.load(ATLAS_ID, { "UI/MapInfo/Tab/disabled/" + std::string(TAB_NAMES[i]) + ".png" });
			ssrc.load(ATLAS_ID, { "UI/MapInfo/Tab/enabled/" + std::string(TAB_NAMES[i]) + ".png" });

			tab_buttons[i] = std::make_unique<LocalTwoSpriteButton>(
				nsrc, ssrc, Point<int16_t>(TAB_X[i], TAB_Y));

			// ALL tab active by default (enabled sprite); others disabled.
			tab_buttons[i]->set_state(i == 0 ? Button::State::MOUSEOVER : Button::State::NORMAL);
		}

		mob_slider = Slider(
			Slider::Type::DEFAULT_SILVER,
			Range<int16_t>(UPPER_SCROLL_Y, UPPER_SCROLL_Y + UPPER_SCROLL_H),
			UPPER_SCROLL_X,
			MOB_VISIBLE,
			1,
			[this](bool upwards)
			{
				int16_t next = mob_offset + (upwards ? -1 : 1);
				int16_t max_off = static_cast<int16_t>(mob_rows.size()) - MOB_VISIBLE;

				if (max_off < 0)
					max_off = 0;

				if (next < 0)
					next = 0;
				else if (next > max_off)
					next = max_off;

				mob_offset = next;
			}
		);
		mob_slider.setenabled(false);

		drop_slider = Slider(
			Slider::Type::DEFAULT_SILVER,
			Range<int16_t>(LOWER_SCROLL_Y, LOWER_SCROLL_Y + LOWER_SCROLL_H),
			LOWER_SCROLL_X,
			GRID_VISIBLE,
			1,
			[this](bool upwards)
			{
				int16_t next = drop_offset + (upwards ? -1 : 1);
				int16_t max_off = (static_cast<int16_t>(drop_cells.size()) + GRID_COLS - 1) / GRID_COLS - GRID_VISIBLE;

				if (max_off < 0)
					max_off = 0;

				if (next < 0)
					next = 0;
				else if (next > max_off)
					next = max_off;

				drop_offset = next;
			}
		);
		drop_slider.setenabled(false);

		empty_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "No monsters on this map");
		empty_drops_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "No drop data");
		meso_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "mesos");
		meso_amount = Text(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::BLACK, "0");
	}

	void UIMapInfoPanel::set_select_callback(SelectCallback callback)
	{
		on_select = std::move(callback);
	}

	void UIMapInfoPanel::set_map(int32_t id)
	{
		map_id = id;
		visible = true;
		selected_mob_id = 0;
		selected_index = -1;
		mob_offset = 0;
		drop_offset = 0;

		rebuild_mobs();
		drop_cells.clear();
		drop_slider.setenabled(false);

		LOG(LOG_INFO, "[UIMapInfoPanel] map_id=" << map_id << " mobs=" << mob_rows.size());
	}

	void UIMapInfoPanel::set_mob(int32_t id)
	{
		selected_mob_id = id;
		rebuild_drops();
	}

	void UIMapInfoPanel::clear()
	{
		visible = false;
		map_id = 0;
		selected_mob_id = 0;
		selected_index = -1;
		mob_offset = 0;
		drop_offset = 0;
		mob_rows.clear();
		drop_cells.clear();
		mob_slider.setenabled(false);
		drop_slider.setenabled(false);
		UI::get().clear_tooltip(Tooltip::Parent::WORLDMAP);
	}

	void UIMapInfoPanel::set_position(Point<int16_t> pos)
	{
		position = pos;
	}

	void UIMapInfoPanel::set_content_height(int16_t worldmap_height)
	{
		panel_height = worldmap_height > 0 ? worldmap_height : HEIGHT;
		mob_slider.setvertical(Range<int16_t>(UPPER_SCROLL_Y, UPPER_SCROLL_Y + UPPER_SCROLL_H));
		drop_slider.setvertical(Range<int16_t>(LOWER_SCROLL_Y, LOWER_SCROLL_Y + LOWER_SCROLL_H));
	}

	bool UIMapInfoPanel::is_visible() const
	{
		return visible;
	}

	int32_t UIMapInfoPanel::get_map_id() const
	{
		return map_id;
	}

	int32_t UIMapInfoPanel::get_selected_mob_id() const
	{
		return selected_mob_id;
	}

	Point<int16_t> UIMapInfoPanel::get_dimensions() const
	{
		return Point<int16_t>(WIDTH, panel_height);
	}

	Rectangle<int16_t> UIMapInfoPanel::get_bounds() const
	{
		return Rectangle<int16_t>(position, position + get_dimensions());
	}

	void UIMapInfoPanel::draw(float) const
	{
		if (!visible)
			return;

		if (chrome.is_valid())
			chrome.draw(position, WIDTH, panel_height);

		// Upper mob list well.
		Rectangle<int16_t> area = mob_list_area();

		if (mob_rows.empty())
		{
			empty_label.draw(position + Point<int16_t>(area.left(), area.top() + 8));
		}
		else
		{
			for (int16_t i = 0; i < MOB_VISIBLE; i++)
			{
				size_t idx = static_cast<size_t>(mob_offset + i);

				if (idx >= mob_rows.size())
					break;

				Point<int16_t> row_pos = position + Point<int16_t>(area.left(), area.top() + i * MOB_ROW_H);

				if (static_cast<int16_t>(idx) == selected_index && row_selected.is_valid())
					row_selected.draw(row_pos);

				const MobRow& row = mob_rows[idx];

				if (row.icon.is_valid())
					row.icon.draw(DrawArgument(row_pos + Point<int16_t>(2, 4), row.icon_scale, row.icon_scale));

				row.name_label.draw(row_pos + Point<int16_t>(ICON_BOX + 4, 4));
				row.type_label.draw(row_pos + Point<int16_t>(ICON_BOX + 4, 22));
				row.stat_label.draw(row_pos + Point<int16_t>(ICON_BOX + 4, 40));
				row.attr_label.draw(row_pos + Point<int16_t>(ICON_BOX + 4, 58));
			}
		}

		if (mob_slider.isenabled())
			mob_slider.draw(position);

		// Tab row.
		for (size_t i = 0; i < static_cast<size_t>(Tab::LENGTH); i++)
			tab_buttons[i]->draw(position);

		// Lower drop grid well.
		if (selected_mob_id != 0)
		{
			int16_t cellmax = static_cast<int16_t>(drop_cells.size());

			if (cellmax == 0)
			{
				empty_drops_label.draw(position + Point<int16_t>(GRID_X, GRID_Y + 8));
			}
			else
			{
				for (int16_t r = 0; r < GRID_VISIBLE; r++)
				{
					int16_t row = drop_offset + r;

					if (row * GRID_COLS >= cellmax)
						break;

					for (int16_t c = 0; c < GRID_COLS; c++)
					{
						size_t idx = static_cast<size_t>(row) * GRID_COLS + static_cast<size_t>(c);

						if (idx >= static_cast<size_t>(cellmax))
							break;

						Point<int16_t> cell_pos = position + Point<int16_t>(
							GRID_X + c * (SLOT + SLOT_GAP),
							GRID_Y + r * (SLOT + SLOT_GAP));

						if (slot.is_valid())
							slot.draw(cell_pos);

						const DropCell& cell = drop_cells[idx];

						if (cell.icon.is_valid())
						{
							Point<int16_t> origin = cell.icon.get_origin();
							cell.icon.draw(cell_pos + Point<int16_t>(1 + origin.x(), 1 + origin.y()));
						}

						if (cell.chance > 0.0)
						{
							std::ostringstream oss;
							oss << std::fixed
								<< std::setprecision(cell.chance < 0.01 ? 1 : 0)
								<< (cell.chance * 100.0) << "%";
							Text rate(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::BROWN, oss.str());
							rate.draw(cell_pos + Point<int16_t>(SLOT - 2, SLOT - 14));
						}
					}
				}
			}

			if (drop_slider.isenabled())
				drop_slider.draw(position);
		}

		// Meso bar (bottom).
		int64_t meso = Stage::get().get_player().get_inventory().get_meso();
		std::string meso_str = std::to_string(meso);
		string_format::split_number(meso_str);
		meso_amount.change_text(meso_str);
		meso_amount.draw(position + Point<int16_t>(MESO_X + 4, MESO_Y));
		meso_label.draw(position + Point<int16_t>(MESO_LABEL_X, MESO_Y));
	}

	void UIMapInfoPanel::update()
	{
	}

	void UIMapInfoPanel::remove_cursor()
	{
		mob_slider.remove_cursor();
		drop_slider.remove_cursor();
		UI::get().clear_tooltip(Tooltip::Parent::WORLDMAP);
	}

	Cursor::State UIMapInfoPanel::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (!visible || !get_bounds().contains(cursorpos))
			return Cursor::State::IDLE;

		// Tabs.
		for (size_t i = 0; i < static_cast<size_t>(Tab::LENGTH); i++)
		{
			Rectangle<int16_t> b = tab_buttons[i]->bounds(position);

			if (b.contains(cursorpos))
			{
				if (clicked)
					set_tab(static_cast<Tab>(i));

				return Cursor::State::CANCLICK;
			}
		}

		// Sliders.
		if (mob_slider.isenabled())
			if (Cursor::State s = mob_slider.send_cursor(cursorpos - position, clicked))
				return s;

		if (drop_slider.isenabled())
			if (Cursor::State s = drop_slider.send_cursor(cursorpos - position, clicked))
				return s;

		// Upper mob list rows.
		Rectangle<int16_t> area = mob_list_area();
		Rectangle<int16_t> abs_area(
			position + Point<int16_t>(area.left(), area.top()),
			position + Point<int16_t>(area.right(), area.bottom()));

		if (abs_area.contains(cursorpos) && !mob_rows.empty())
		{
			int16_t local_y = cursorpos.y() - abs_area.top();
			int16_t row = local_y / MOB_ROW_H;
			int16_t index = mob_offset + row;

			if (index >= 0 && index < static_cast<int16_t>(mob_rows.size()))
			{
				if (clicked)
					select_mob_row(index);

				return Cursor::State::CANCLICK;
			}
		}

		// Lower drop grid hover -> item tooltip.
		if (selected_mob_id != 0 && !drop_cells.empty())
		{
			Rectangle<int16_t> g = drop_grid_area();

			if (g.contains(cursorpos))
			{
				int16_t lx = cursorpos.x() - (position.x() + GRID_X);
				int16_t ly = cursorpos.y() - (position.y() + GRID_Y);
				int16_t c = lx / (SLOT + SLOT_GAP);
				int16_t r = ly / (SLOT + SLOT_GAP);

				if (c >= 0 && c < GRID_COLS && r >= 0 && r < GRID_VISIBLE)
				{
					size_t idx = static_cast<size_t>(drop_offset + r) * GRID_COLS + static_cast<size_t>(c);

					if (idx < drop_cells.size())
					{
						UI::get().show_item(Tooltip::Parent::WORLDMAP, drop_cells[idx].item_id);
						return Cursor::State::CANCLICK;
					}
				}

				UI::get().clear_tooltip(Tooltip::Parent::WORLDMAP);
			}
		}

		return Cursor::State::IDLE;
	}

	void UIMapInfoPanel::rebuild_mobs()
	{
		mob_rows.clear();

		std::vector<NxHelper::Map::MapMobInfo> mobs = NxHelper::Map::get_mobs_on_map(map_id);

		for (const auto& mob : mobs)
		{
			MobRow row;
			row.id = mob.id;
			row.icon = load_mob_stand_icon(mob.id);

			if (row.icon.is_valid())
			{
				int16_t max_dim = row.icon.width() > row.icon.height() ? row.icon.width() : row.icon.height();

				if (max_dim > ICON_BOX && max_dim > 0)
					row.icon_scale = static_cast<float>(ICON_BOX) / static_cast<float>(max_dim);
			}

			std::ostringstream name_ss;
			name_ss << "Lv." << mob.level << " " << mob.name;
			row.name_label = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::EMPEROR, name_ss.str());

			row.type_label = Text(Text::Font::A11M, Text::Alignment::LEFT,
				mob.boss ? Color::Name::RED : Color::Name::BLACK,
				mob.boss ? "BOSS" : "普通");

			std::ostringstream stat;
			stat << "HP ";
			if (mob.max_hp > 0)
			{
				std::string s = std::to_string(mob.max_hp);
				string_format::split_number(s);
				stat << s;
			}
			else
			{
				stat << "-";
			}
			stat << "  EXP ";
			if (mob.exp > 0)
			{
				std::string s = std::to_string(mob.exp);
				string_format::split_number(s);
				stat << s;
			}
			else
			{
				stat << "-";
			}
			row.stat_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, stat.str());

			row.attr_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::GRAY, "属性: -  抗性: -");

			mob_rows.push_back(std::move(row));
		}

		int16_t rowmax = static_cast<int16_t>(mob_rows.size());
		mob_slider.setrows(MOB_VISIBLE, rowmax > MOB_VISIBLE ? rowmax : MOB_VISIBLE);
		mob_slider.setenabled(rowmax > MOB_VISIBLE);
		mob_offset = 0;
	}

	void UIMapInfoPanel::rebuild_drops()
	{
		drop_cells.clear();

		if (selected_mob_id == 0)
		{
			drop_slider.setenabled(false);
			return;
		}

		std::vector<MobDropEntry> drops = MobDropData::get_drops(selected_mob_id);

		for (const auto& d : drops)
		{
			if (!classify_drop(active_tab, d.item_id))
				continue;

			DropCell cell;
			cell.item_id = d.item_id;
			cell.chance = d.chance;

			const ItemData& item = ItemData::get(d.item_id);

			if (item.is_valid())
				cell.icon = item.get_icon(false);

			drop_cells.push_back(std::move(cell));
		}

		int16_t cellmax = static_cast<int16_t>(drop_cells.size());
		int16_t maxrow = (cellmax + GRID_COLS - 1) / GRID_COLS;
		drop_slider.setrows(GRID_VISIBLE, maxrow > GRID_VISIBLE ? maxrow : GRID_VISIBLE);
		drop_slider.setenabled(maxrow > GRID_VISIBLE);
		drop_offset = 0;
	}

	void UIMapInfoPanel::set_tab(Tab tab)
	{
		active_tab = tab;

		for (size_t i = 0; i < static_cast<size_t>(Tab::LENGTH); i++)
			tab_buttons[i]->set_state(i == static_cast<size_t>(tab) ? Button::State::MOUSEOVER : Button::State::NORMAL);

		rebuild_drops();
	}

	void UIMapInfoPanel::select_mob_row(int16_t index)
	{
		if (index < 0 || index >= static_cast<int16_t>(mob_rows.size()))
			return;

		selected_index = index;
		int32_t mob = mob_rows[static_cast<size_t>(index)].id;

		LOG(LOG_INFO, "[UIMapInfoPanel] selected mob_id=" << mob);

		if (on_select)
			on_select(mob);
	}

	Rectangle<int16_t> UIMapInfoPanel::mob_list_area() const
	{
		return Rectangle<int16_t>(
			Point<int16_t>(UPPER_X, UPPER_Y),
			Point<int16_t>(UPPER_X + UPPER_W, UPPER_Y + UPPER_H));
	}

	Rectangle<int16_t> UIMapInfoPanel::drop_grid_area() const
	{
		return Rectangle<int16_t>(
			Point<int16_t>(GRID_X, GRID_Y),
			Point<int16_t>(GRID_X + GRID_COLS * (SLOT + SLOT_GAP), GRID_Y + GRID_VISIBLE * (SLOT + SLOT_GAP)));
	}

	Texture UIMapInfoPanel::load_mob_stand_icon(int32_t mob_id)
	{
		std::string strid = string_format::extend_id(mob_id, 7);
		nl::node src = nl::nx::Mob[strid + ".img"];

		if (!src)
			return {};

		nl::node info = src["info"];
		std::string linkid = info["link"].get_string();
		nl::node link_src = linkid.empty() ? nl::node() : nl::nx::Mob[linkid + ".img"];
		nl::node link = link_src ? link_src : src;

		Texture icon = first_frame(link["stand"]);

		if (!icon.is_valid())
			icon = first_frame(link["fly"]);

		return icon;
	}

	bool UIMapInfoPanel::classify_drop(Tab tab, int32_t item_id)
	{
		switch (tab)
		{
			case Tab::ALL:
				return true;
			case Tab::EQUIP:
				return item_id >= 1000000 && item_id < 2000000;
			case Tab::USE:
				return item_id >= 2000000 && item_id < 3000000;
			case Tab::ETC:
				return item_id >= 3000000; // setup / etc / cash / others
			default:
				return true;
		}
	}
}
