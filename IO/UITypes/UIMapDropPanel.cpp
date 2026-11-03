//////////////////////////////////////////////////////////////////////////////////
//	UIMapDropPanel
//////////////////////////////////////////////////////////////////////////////////
#include "UIMapDropPanel.h"

#include "../UI.h"

#include "../../Data/ItemData.h"
#include "../../Util/Misc.h"

#include "../../MapleStory.h"
#include "../../Util/Locale.h"

#include <iomanip>
#include <sstream>

namespace ms
{
	UIMapDropPanel::UIMapDropPanel() :
		visible(false),
		has_selection(false),
		mob_id(0),
		list_offset(0),
		hover_index(-1),
		panel_height(537)
	{
		chrome.load(ATLAS_ID, {
			"UI/MapMobDrops/drop_panel_backgrnd.png",
			"docs/features/map-mob-drops/resources/chrome/drop_panel_backgrnd.png",
		});

		if (!chrome.is_valid())
			LOG(LOG_WARN, "[UIMapDropPanel] chrome PNG missing — side panel will be empty frame");

		hover_bg = ColorBox(WIDTH - PAD_X * 2 - 14, ROW_HEIGHT - 2, Color::Name::BLACK, 0.06f);
		// 两块面板的空态标签都只在 ctor 写、之后只 draw（已全文核过无第二个写入点）=> 满足绑定
		// 判据，于是切语言时已经开着的面板也会跟着换文案，不必重开。
		empty_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK);
		locale_binds.bind(empty_label, "mappanel.select_monster");
		empty_drops_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK);
		locale_binds.bind(empty_drops_label, "mappanel.no_drop_data");

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

	void UIMapDropPanel::set_mob(int32_t id)
	{
		mob_id = id;
		has_selection = id != 0;
		visible = true;
		list_offset = 0;
		hover_index = -1;
		rebuild_rows();

		if (has_selection)
		{
			LOG(LOG_INFO, "[UIMapDropPanel] mob_id=" << mob_id << " drops=" << rows.size()
				<< " valid=" << (MobDropData::get(mob_id) ? 1 : 0));
		}
	}

	void UIMapDropPanel::clear()
	{
		visible = false;
		has_selection = false;
		mob_id = 0;
		list_offset = 0;
		hover_index = -1;
		rows.clear();
		slider.setenabled(false);
		UI::get().clear_tooltip(Tooltip::Parent::WORLDMAP);
	}

	void UIMapDropPanel::set_position(Point<int16_t> pos)
	{
		position = pos;
	}

	void UIMapDropPanel::set_content_height(int16_t worldmap_height)
	{
		int16_t min_h = PAD_TOP + PAD_BOTTOM + ROW_HEIGHT * 3;
		panel_height = worldmap_height > min_h ? worldmap_height : min_h;
		hover_bg.setwidth(WIDTH - PAD_X * 2 - 14);
		slider.setvertical(Range<int16_t>(PAD_TOP, panel_height - PAD_BOTTOM));
	}

	bool UIMapDropPanel::is_visible() const
	{
		return visible;
	}

	bool UIMapDropPanel::has_mob() const
	{
		return has_selection;
	}

	int32_t UIMapDropPanel::get_mob_id() const
	{
		return mob_id;
	}

	Point<int16_t> UIMapDropPanel::get_dimensions() const
	{
		return Point<int16_t>(WIDTH, panel_height);
	}

	Rectangle<int16_t> UIMapDropPanel::get_bounds() const
	{
		return Rectangle<int16_t>(position, position + get_dimensions());
	}

	void UIMapDropPanel::draw(float) const
	{
		if (!visible)
			return;

		if (chrome.is_valid())
			chrome.draw(position, WIDTH, panel_height);

		Rectangle<int16_t> area = list_area();

		if (!has_selection)
		{
			empty_label.draw(position + Point<int16_t>(area.left(), area.top() + 8));
			return;
		}

		if (rows.empty())
		{
			empty_drops_label.draw(position + Point<int16_t>(area.left(), area.top() + 8));
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

			if (static_cast<int16_t>(idx) == hover_index)
				hover_bg.draw(row_pos);

			const Row& row = rows[idx];

			if (row.icon.is_valid())
			{
				Point<int16_t> origin = row.icon.get_origin();
				row.icon.draw(row_pos + Point<int16_t>(2 + origin.x(), 30));
			}

			row.id_label.draw(row_pos + Point<int16_t>(34, 2));
			row.name_label.draw(row_pos + Point<int16_t>(34, 16));
			row.meta_label.draw(row_pos + Point<int16_t>(120, 2));
		}

		if (slider.isenabled())
			slider.draw(position);
	}

	void UIMapDropPanel::update()
	{
		// 吸附面板不是 UIElement，拿不到 UIState 派发点上的那一次 refresh_localized()，
		// 于是在自己的 update 里重取：同一张 LocaleBindings，绑定判据与日志口径都不变，
		// 只是刷新点从「一处派发」挪成「宿主自取」—— 这正是「reload 广播、窗自取」的形态。
		if (locale_binds.refresh())
			LOG(LOG_INFO, "Locale refresh gen=" << Locale::get().generation() << " rebound "
				<< locale_binds.count() << " labels (" << locale_binds.hostname() << ")");
	}

	void UIMapDropPanel::remove_cursor()
	{
		hover_index = -1;
		slider.remove_cursor();
	}

	Cursor::State UIMapDropPanel::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (!visible || !get_bounds().contains(cursorpos))
		{
			hover_index = -1;
			UI::get().clear_tooltip(Tooltip::Parent::WORLDMAP);
			return Cursor::State::IDLE;
		}

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

		if (!abs_area.contains(cursorpos) || rows.empty())
		{
			hover_index = -1;
			UI::get().clear_tooltip(Tooltip::Parent::WORLDMAP);
			return Cursor::State::IDLE;
		}

		int16_t local_y = cursorpos.y() - abs_area.top();
		int16_t row = local_y / ROW_HEIGHT;
		int16_t index = list_offset + row;

		if (index < 0 || static_cast<size_t>(index) >= rows.size())
		{
			hover_index = -1;
			UI::get().clear_tooltip(Tooltip::Parent::WORLDMAP);
			return Cursor::State::IDLE;
		}

		hover_index = index;
		UI::get().show_item(Tooltip::Parent::WORLDMAP, rows[static_cast<size_t>(index)].item_id);

		return Cursor::State::CANCLICK;
	}

	void UIMapDropPanel::rebuild_rows()
	{
		rows.clear();

		if (!has_selection)
		{
			slider.setenabled(false);
			return;
		}

		std::vector<MobDropEntry> drops = MobDropData::get_drops(mob_id);

		for (const MobDropEntry& drop : drops)
		{
			Row row;
			row.item_id = drop.item_id;

			const ItemData& item = ItemData::get(drop.item_id);

			if (item.is_valid())
			{
				row.icon = item.get_icon(false);
				row.name_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, item.get_name());
			}
			else
			{
				row.name_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK,
					Locale::text("mapdrop.unknown_item"));
			}

			row.id_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::GRAY, std::to_string(drop.item_id));

			std::ostringstream meta;
			meta << std::fixed << std::setprecision(2) << (drop.chance * 100.0) << "%";
			meta << " x";

			if (drop.min_qty == drop.max_qty)
				meta << drop.min_qty;
			else
				meta << drop.min_qty << "~" << drop.max_qty;

			row.meta_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BROWN, meta.str());
			rows.push_back(std::move(row));
		}

		int16_t rowmax = static_cast<int16_t>(rows.size());
		int16_t visible_rows = (list_area().bottom() - list_area().top()) / ROW_HEIGHT;

		if (visible_rows < 1)
			visible_rows = VISIBLE_ROWS;

		slider.setrows(visible_rows, rowmax > visible_rows ? rowmax : visible_rows);
		slider.setenabled(rowmax > visible_rows);
	}

	Rectangle<int16_t> UIMapDropPanel::list_area() const
	{
		return Rectangle<int16_t>(
			Point<int16_t>(PAD_X, PAD_TOP),
			Point<int16_t>(WIDTH - PAD_X - 14, panel_height - PAD_BOTTOM)
		);
	}
}
