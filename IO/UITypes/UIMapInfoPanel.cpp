////////////////////////////////////////////////////////////////////////////////
//	UIMapInfoPanel — left-docked map info panel for UIWorldMap
////////////////////////////////////////////////////////////////////////////////
#include "UIMapInfoPanel.h"

#include "../UI.h"
#include "../Components/Charset.h"

#include "../../Audio/Audio.h"
#include "../../Character/Inventory/Inventory.h"
#include "../../Gameplay/Stage.h"
#include "../../MapleStory.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"
#include "../../Net/Packets/MiscOutPackets.h"

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

		// Mirror UIMapMobPanel: shift origin to (0,0) so the icon draws from top-left.
		void normalize_icon_origin(Texture& icon)
		{
			if (!icon.is_valid())
				return;

			Point<int16_t> origin = icon.get_origin();

			if (origin.x() != 0 || origin.y() != 0)
				icon.shift(origin);
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
		// D5：标题「地图信息」烤进底板顶脊（原生管线产物）。客户端运行时优先，
		// 仓库 chrome 兜底；标题由 PNG 携带，程序侧不重复绘制。
		chrome.load(ATLAS_ID, {
			"UI/MapInfo/info_panel_backgrnd.png",
			"docs/features/map-info-panel/resources/chrome/info_panel_backgrnd.png",
		});

		if (!chrome.is_valid())
			LOG(LOG_WARN, "[UIMapInfoPanel] chrome PNG missing — panel will be empty frame");

		row_selected.load(ATLAS_ID, {
			"UI/MapInfo/list_row_selected.png",
			"docs/features/map-info-panel/ui/list_row_selected/v2/list_row_selected.png",
			"docs/features/map-info-panel/resources/chrome/list_row_selected.png",
		});
		slot.load(ATLAS_ID, {
			"UI/MapInfo/slot.png",
			"docs/features/map-info-panel/ui/slot/v2/slot.png",
			"docs/features/map-info-panel/resources/chrome/slot.png",
		});

		// v2 原生风：怪物行交替底纹（含图标灰底）。缺图则回退到无底纹（旧观感）。
		row_bg_a.load(ATLAS_ID, {
			"UI/MapInfo/mob_row_bg_a.png",
			"docs/features/map-info-panel/ui/mob_row_bg/v2/mob_row_bg_a.png",
		});
		row_bg_b.load(ATLAS_ID, {
			"UI/MapInfo/mob_row_bg_b.png",
			"docs/features/map-info-panel/ui/mob_row_bg/v2/mob_row_bg_b.png",
		});

		for (size_t i = 0; i < static_cast<size_t>(Tab::LENGTH); i++)
		{
			LocalPngTexture nsrc;
			LocalPngTexture ssrc;
			std::string key(TAB_NAMES[i]);

			nsrc.load(ATLAS_ID, {
				"UI/MapInfo/Tab/disabled/" + key + ".png",
				"docs/features/map-info-panel/ui/tab/v2/" + key + "_disabled.png",
			});
			ssrc.load(ATLAS_ID, {
				"UI/MapInfo/Tab/enabled/" + key + ".png",
				"docs/features/map-info-panel/ui/tab/v2/" + key + "_enabled.png",
			});

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

		drop_hover = ColorBox(SLOT, SLOT, Color::Name::BLACK, 0.12f);
		hover_drop_idx = -1;
		// v2 行底纹缺图时的兜底：图标位灰底（40x40，与 mob_row_bg 烤的灰底同位同色）。
		icon_back = ColorBox(ICON_BOX, ICON_BOX, Color::Name::GRAY, 0.25f);

		// 三条标签都只在 ctor 写 => 进绑定表。下一行 meso_amount 的 "0" 不建键：那是数据初值，
		// draw 前会被 meso_str 改写，按绑定判据（有无第二个写入点）不许上表。
		empty_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK);
		locale_binds.bind(empty_label, "mappanel.no_monsters");
		empty_drops_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK);
		locale_binds.bind(empty_drops_label, "mappanel.no_drop_data");
		meso_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK);
		locale_binds.bind(meso_label, "mappanel.mesos");
		meso_amount = Text(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::BLACK, "0");

		// D5：标题烤进底板，程序侧不画（防双字）。绑定仍在，供日后切回无字底板复用。
		title_text = Text(Text::Font::A13B, Text::Alignment::CENTER, Color::Name::BLACK, "地图信息");
		locale_binds.bind(title_text, "mapinfo.title");
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
		meso_drop_range.clear();

		// Always ask the server for this mob's drops. The server reads from an
		// in-memory, concurrency-safe cache (no per-request DB hit) and carries a
		// bumped version on reload, so re-requesting keeps the panel fresh after a
		// server-side !reloaddrops without any client-side version tracking.
		if (id != 0)
		{
			WorldMapDropRequestPacket(id, map_id, true).dispatch();
			last_requested_mob = id;
		}

		drop_built_mob = -1; // rebuild once the server responds
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
		last_requested_mob = 0;
		drop_built_mob = -1;
		meso_drop_range.clear();
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

		// D5：标题烤进底板，程序侧不画（防双字）。
		(void)title_text;

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

				// v2 原生风：奇偶行交替底纹（图标灰底已烤在行图里）。
				const LocalPngTexture& stripe = (i % 2 == 0) ? row_bg_a : row_bg_b;

				if (stripe.is_valid())
					stripe.draw(row_pos);
				else
					icon_back.draw(row_pos + Point<int16_t>(2, 4));

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
							// Origin normalized to (0,0); center the icon in the 32x32 slot.
							Point<int16_t> off(
								static_cast<int16_t>((SLOT - cell.icon.width()) / 2),
								static_cast<int16_t>((SLOT - cell.icon.height()) / 2));
							cell.icon.draw(cell_pos + off);
						}

						if (static_cast<size_t>(idx) == static_cast<size_t>(hover_drop_idx))
							drop_hover.draw(cell_pos);

						// ItemNo 数量字：背包同款位图字库（Basic.img/ItemNo，8x11），
						// 右下角，仅数量 >1 时显示（min==max 取 min，否则取上限）。
						// 百分比让位到右上角，避免与数量字叠在同一角。
						int16_t qty = (cell.min_qty == cell.max_qty) ? cell.min_qty : cell.max_qty;

						if (qty > 1)
						{
							static const Charset countset(nl::nx::UI["Basic.img"]["ItemNo"], Charset::Alignment::RIGHT);
							countset.draw(std::to_string(qty), DrawArgument(cell_pos + Point<int16_t>(SLOT - 1, SLOT - 12)));
						}

						if (cell.chance > 0.0)
						{
							std::ostringstream oss;
							oss << std::fixed
								<< std::setprecision(cell.chance < 0.01 ? 1 : 0)
								<< (cell.chance * 100.0) << "%";
							Text rate(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::BROWN, oss.str());
							rate.draw(cell_pos + Point<int16_t>(SLOT - 2, 1));
						}
					}
				}
			}

			if (drop_slider.isenabled())
				drop_slider.draw(position);
		}

		// Meso bar (bottom). Prefer the per-mob meso drop range from the drop table.
		std::string meso_str;
		if (!meso_drop_range.empty())
		{
			meso_str = meso_drop_range;
		}
		else
		{
			int64_t meso = Stage::get().get_player().get_inventory().get_meso();
			meso_str = std::to_string(meso);
			string_format::split_number(meso_str);
		}
		meso_amount.change_text(meso_str);
		meso_amount.draw(position + Point<int16_t>(MESO_X + MESO_W, MESO_Y));
		meso_label.draw(position + Point<int16_t>(MESO_LABEL_X, MESO_Y));
	}

	void UIMapInfoPanel::update()
	{
		// 吸附面板不是 UIElement，拿不到 UIState 派发点上的那一次 refresh_localized()，
		// 于是在自己的 update 里重取：同一张 LocaleBindings，绑定判据与日志口径都不变，
		// 只是刷新点从「一处派发」挪成「宿主自取」—— 这正是「reload 广播、窗自取」的形态。
		if (locale_binds.refresh())
			LOG(LOG_INFO, "Locale refresh gen=" << Locale::get().generation() << " rebound "
				<< locale_binds.count() << " labels (" << locale_binds.hostname() << ")");

		// Lazily rebuild the drop grid once the server's drop data has arrived.
		if (selected_mob_id != 0 && MobDropData::is_loaded(selected_mob_id) && drop_built_mob != selected_mob_id)
		{
			rebuild_drops();
			drop_built_mob = selected_mob_id;
		}
	}

	void UIMapInfoPanel::remove_cursor()
	{
		mob_slider.remove_cursor();
		drop_slider.remove_cursor();
		hover_drop_idx = -1;
		UI::get().clear_tooltip(Tooltip::Parent::WORLDMAP);
	}

	Cursor::State UIMapInfoPanel::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (!visible || !get_bounds().contains(cursorpos))
			return Cursor::State::IDLE;

		// Tabs — hover + click, mirror UIMapSearchPanel.
		for (size_t i = 0; i < static_cast<size_t>(Tab::LENGTH); i++)
		{
			auto* btn = tab_buttons[i].get();
			Rectangle<int16_t> b = btn->bounds(position);

			if (b.contains(cursorpos))
			{
				if (btn->get_state() == Button::State::NORMAL)
				{
					Sound(Sound::Name::BUTTONOVER).play();
					btn->set_state(Button::State::MOUSEOVER);
					return Cursor::State::CANCLICK;
				}
				else if (btn->get_state() == Button::State::MOUSEOVER)
				{
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();
						set_tab(static_cast<Tab>(i));
						return Cursor::State::CLICKING;
					}

					return Cursor::State::CANCLICK;
				}

				return Cursor::State::CANCLICK;
			}
			else if (btn->get_state() == Button::State::MOUSEOVER)
			{
				// Revert hover: keep highlighted only if this tab is the active one.
				btn->set_state(i == static_cast<size_t>(active_tab)
					? Button::State::MOUSEOVER : Button::State::NORMAL);
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

		// Lower drop grid hover -> item tooltip + cell highlight.
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
						hover_drop_idx = static_cast<int16_t>(idx);
						UI::get().show_item(Tooltip::Parent::WORLDMAP, drop_cells[idx].item_id);
						return Cursor::State::CANCLICK;
					}
				}

				hover_drop_idx = -1;
				UI::get().clear_tooltip(Tooltip::Parent::WORLDMAP);
			}
			else
			{
				hover_drop_idx = -1;
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

			// 行对象每重建一次就换一批（mob_rows.clear() + push_back），绑定表存的是裸指针，
			// 上表等于把悬垂指针留在表里 => 这类文案只能在赋值点现取。
			row.type_label = Text(Text::Font::A11M, Text::Alignment::LEFT,
				mob.boss ? Color::Name::RED : Color::Name::BLACK,
				mob.boss ? Locale::text("mapinfo.type_boss") : Locale::text("mapinfo.type_normal"));

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

			// 「属性/抗性」的值本包还没接数据，先填占位符 "-" —— 所以模板留 `{}`、占位符当值
			// 喂进去，日后接真数据只改这里的实参，不用再动键位。
			row.attr_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::GRAY,
				Locale::format("mapinfo.attr", "-") + "  " + Locale::format("mapinfo.resist", "-"));

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
			// Meso drop: drive the meso bar instead of the item grid.
			if (d.item_id == 0)
			{
				meso_drop_range = (d.min_qty == d.max_qty)
					? std::to_string(d.min_qty)
					: (std::to_string(d.min_qty) + "~" + std::to_string(d.max_qty));
				continue;
			}

			if (!classify_drop(active_tab, d.item_id))
				continue;

			DropCell cell;
			cell.item_id = d.item_id;
			cell.chance = d.chance;
			cell.min_qty = d.min_qty;
			cell.max_qty = d.max_qty;

			const ItemData& item = ItemData::get(d.item_id);

			if (item.is_valid())
				cell.icon = item.get_icon(false);

			normalize_icon_origin(cell.icon);

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
