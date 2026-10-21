//////////////////////////////////////////////////////////////////////////////////
//	UIMapMobPanel
//////////////////////////////////////////////////////////////////////////////////
#include "UIMapMobPanel.h"

#include "../../Util/Misc.h"

#include "../../MapleStory.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

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

		void normalize_icon_origin(Texture& icon)
		{
			if (!icon.is_valid())
				return;

			Point<int16_t> origin = icon.get_origin();

			if (origin.x() != 0 || origin.y() != 0)
				icon.shift(origin);
		}
	}

	UIMapMobPanel::UIMapMobPanel() :
		visible(false),
		map_id(0),
		selected_index(-1),
		list_offset(0),
		panel_height(537)
	{
		chrome.load(ATLAS_ID, {
			"UI/MapMobDrops/mob_panel_backgrnd.png",
			"docs/features/map-mob-drops/resources/chrome/mob_panel_backgrnd.png",
		});

		if (!chrome.is_valid())
			LOG(LOG_WARN, "[UIMapMobPanel] chrome PNG missing — side panel will be empty frame");

		select_bg = ColorBox(WIDTH - PAD_X * 2 - 14, ROW_HEIGHT - 2, Color::Name::BLACK, 0.08f);
		empty_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "No monsters on this map");

		slider = Slider(
			Slider::Type::DEFAULT_SILVER,
			Range<int16_t>(PAD_TOP, panel_height - PAD_BOTTOM),
			WIDTH - 12,
			VISIBLE_ROWS,
			1,
			[this](bool upwards)
			{
				int16_t next = list_offset + (upwards ? -1 : 1);
				int16_t max_off = static_cast<int16_t>(rows.size()) - VISIBLE_ROWS;

				if (max_off < 0)
					max_off = 0;

				if (next < 0)
					next = 0;
				else if (next > max_off)
					next = max_off;

				list_offset = next;
			}
		);
		slider.setenabled(false);
	}

	void UIMapMobPanel::set_select_callback(SelectCallback callback)
	{
		on_select = std::move(callback);
	}

	void UIMapMobPanel::set_map(int32_t id)
	{
		map_id = id;
		visible = true;
		selected_index = -1;
		list_offset = 0;
		rebuild_rows();

		LOG(LOG_INFO, "[UIMapMobPanel] map_id=" << map_id << " mobs=" << rows.size());
	}

	void UIMapMobPanel::clear()
	{
		visible = false;
		map_id = 0;
		selected_index = -1;
		list_offset = 0;
		rows.clear();
		slider.setenabled(false);
	}

	void UIMapMobPanel::set_position(Point<int16_t> pos)
	{
		position = pos;
	}

	void UIMapMobPanel::set_content_height(int16_t worldmap_height)
	{
		int16_t min_h = PAD_TOP + PAD_BOTTOM + ROW_HEIGHT * 3;
		panel_height = worldmap_height > min_h ? worldmap_height : min_h;
		select_bg.setwidth(WIDTH - PAD_X * 2 - 14);
		slider.setvertical(Range<int16_t>(PAD_TOP, panel_height - PAD_BOTTOM));
	}

	bool UIMapMobPanel::is_visible() const
	{
		return visible;
	}

	int32_t UIMapMobPanel::get_map_id() const
	{
		return map_id;
	}

	int32_t UIMapMobPanel::get_selected_mob_id() const
	{
		if (selected_index < 0 || static_cast<size_t>(selected_index) >= rows.size())
			return 0;

		return rows[static_cast<size_t>(selected_index)].id;
	}

	Point<int16_t> UIMapMobPanel::get_dimensions() const
	{
		return Point<int16_t>(WIDTH, panel_height);
	}

	Rectangle<int16_t> UIMapMobPanel::get_bounds() const
	{
		return Rectangle<int16_t>(position, position + get_dimensions());
	}

	void UIMapMobPanel::draw(float) const
	{
		if (!visible)
			return;

		if (chrome.is_valid())
			chrome.draw(position, WIDTH, panel_height);

		Rectangle<int16_t> area = list_area();

		if (rows.empty())
		{
			empty_label.draw(position + Point<int16_t>(area.left(), area.top() + 8));
			return;
		}

		int16_t visible_h = (area.bottom() - area.top()) / ROW_HEIGHT;

		if (visible_h < 1)
			visible_h = 1;

		for (int16_t i = 0; i < visible_h; i++)
		{
			size_t idx = static_cast<size_t>(list_offset + i);

			if (idx >= rows.size())
				break;

			Point<int16_t> row_pos = position + Point<int16_t>(area.left(), area.top() + i * ROW_HEIGHT);

			if (static_cast<int16_t>(idx) == selected_index)
				select_bg.draw(row_pos);

			const Row& row = rows[idx];

			if (row.icon.is_valid())
				row.icon.draw(DrawArgument(row_pos + Point<int16_t>(2, 4), row.icon_scale, row.icon_scale));

			row.name_label.draw(row_pos + Point<int16_t>(ICON_BOX + 4, 4));
			row.hp_label.draw(row_pos + Point<int16_t>(ICON_BOX + 4, 24));
		}

		if (slider.isenabled())
			slider.draw(position);
	}

	void UIMapMobPanel::update()
	{
	}

	void UIMapMobPanel::remove_cursor()
	{
		slider.remove_cursor();
	}

	Cursor::State UIMapMobPanel::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (!visible || !get_bounds().contains(cursorpos))
			return Cursor::State::IDLE;

		if (slider.isenabled())
		{
			if (Cursor::State state = slider.send_cursor(cursorpos - position, clicked))
				return state;
		}

		Rectangle<int16_t> area = list_area();
		Rectangle<int16_t> abs_area(
			position + Point<int16_t>(area.left(), area.top()),
			position + Point<int16_t>(area.right(), area.bottom())
		);

		if (!abs_area.contains(cursorpos))
			return Cursor::State::IDLE;

		int16_t local_y = cursorpos.y() - abs_area.top();
		int16_t row = local_y / ROW_HEIGHT;
		int16_t index = list_offset + row;

		if (index < 0 || static_cast<size_t>(index) >= rows.size())
			return Cursor::State::IDLE;

		if (clicked)
			select_row(index);

		return Cursor::State::CANCLICK;
	}

	void UIMapMobPanel::rebuild_rows()
	{
		rows.clear();

		std::vector<NxHelper::Map::MapMobInfo> mobs = NxHelper::Map::get_mobs_on_map(map_id);

		for (const auto& mob : mobs)
		{
			Row row;
			row.id = mob.id;
			row.icon = load_mob_stand_icon(mob.id);
			normalize_icon_origin(row.icon);

			if (row.icon.is_valid())
			{
				int16_t max_dim = row.icon.width() > row.icon.height() ? row.icon.width() : row.icon.height();

				if (max_dim > ICON_BOX && max_dim > 0)
					row.icon_scale = static_cast<float>(ICON_BOX) / static_cast<float>(max_dim);
			}

			std::ostringstream name_ss;
			name_ss << "Lv." << mob.level << " " << mob.name;
			row.name_label = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::EMPEROR, name_ss.str());

			std::string hp_text = "HP ";

			if (mob.max_hp > 0)
			{
				std::string num = std::to_string(mob.max_hp);
				string_format::split_number(num);
				hp_text += num;
			}
			else
			{
				hp_text += "-";
			}

			row.hp_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, hp_text);
			rows.push_back(std::move(row));
		}

		int16_t rowmax = static_cast<int16_t>(rows.size());
		int16_t visible_rows = (list_area().bottom() - list_area().top()) / ROW_HEIGHT;

		if (visible_rows < 1)
			visible_rows = VISIBLE_ROWS;

		slider.setrows(visible_rows, rowmax > visible_rows ? rowmax : visible_rows);
		slider.setenabled(rowmax > visible_rows);
		list_offset = 0;
	}

	void UIMapMobPanel::select_row(int16_t index)
	{
		if (index < 0 || static_cast<size_t>(index) >= rows.size())
			return;

		selected_index = index;
		int32_t mob = rows[static_cast<size_t>(index)].id;

		LOG(LOG_INFO, "[UIMapMobPanel] selected mob_id=" << mob);

		if (on_select)
			on_select(mob);
	}

	Rectangle<int16_t> UIMapMobPanel::list_area() const
	{
		return Rectangle<int16_t>(
			Point<int16_t>(PAD_X, PAD_TOP),
			Point<int16_t>(WIDTH - PAD_X - 14, panel_height - PAD_BOTTOM)
		);
	}

	Texture UIMapMobPanel::load_mob_stand_icon(int32_t mob_id)
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
}
