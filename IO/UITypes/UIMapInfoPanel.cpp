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
#include "../../Util/MobElemAttr.h"
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
	// mob_row_bg 左上灰底实测：内边距 (2,4)，图标在灰底内居中。
	static constexpr int16_t ICON_PLATE_X = 2;
	static constexpr int16_t ICON_PLATE_Y = 4;
	static constexpr int16_t TEXT_X = ICON_BOX + 6; // 灰底右侧 2px 缝
	// 相对原 6/24/42/58 整组上移一行（≈12px），贴齐图标顶。
	static constexpr int16_t TEXT_NAME_Y = 0;
	static constexpr int16_t TEXT_TYPE_Y = 16;
	static constexpr int16_t TEXT_STAT_Y = 32;
	static constexpr int16_t TEXT_ATTR_Y = 48;
	static constexpr int16_t UPPER_SCROLL_X = 210;
	static constexpr int16_t UPPER_SCROLL_Y = 44;
	static constexpr int16_t UPPER_SCROLL_H = 233;
	static constexpr int16_t SLOT_ICON_MAX = 28; // 30 槽内留边

	// 底板实测蓝灰条 y=285..303（高 19）；Tab 18px 贴条顶光学居中。
	static constexpr int16_t TAB_Y = 286;
	static constexpr int16_t TAB_H = 18;
	static constexpr int16_t TAB_W[4] = { 50, 50, 50, 53 };
	static constexpr int16_t TAB_X[4] = { 18, 68, 118, 168 };

	static constexpr int16_t LOWER_X = 18;
	static constexpr int16_t LOWER_Y = 309;
	static constexpr int16_t LOWER_W = 203;
	static constexpr int16_t LOWER_H = 174;
	// 2026-11-22 修正：底板 PNG 已移除自带的 5×5 烤格，运行时按 4×4 画唯一一套格子。
	static constexpr int16_t GRID_COLS = 4;
	static constexpr int16_t GRID_VISIBLE = 4;
	static constexpr int16_t SLOT = 38;
	static constexpr int16_t SLOT_GAP = 4;
	static constexpr int16_t GRID_X = 22;    // 4*38+3*4=164，well 内水平居中
	static constexpr int16_t GRID_Y = 313;
	// 右侧滑块：用户 2026-11-22 23:39 红竖框实测 x≈195..211（面板坐标），
	// 旧 x=215 落框外右侧；滑块位图自身 origin 右偏≈8px，锚点取 199 使可见中心≈203 落框内。
	static constexpr int16_t LOWER_SCROLL_X = 199;
	static constexpr int16_t LOWER_SCROLL_Y = 310;
	static constexpr int16_t LOWER_SCROLL_H = 170;

	// 底部 mesos 白框：probe 实测纯白区 x=19..176 / y=490..505（y=483 是上描边）。
	// 金币墨迹须落白框内（用户红横框对应此白框），垂直居中。
	static constexpr int16_t MESO_X = 19;
	static constexpr int16_t MESO_FIELD_W = 157; // 白框宽到 mesos 标签前
	static constexpr int16_t MESO_Y = 490;       // 白框真正上沿（非描边）
	static constexpr int16_t MESO_H = 16;

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
		// 每张 LocalPngTexture 独立 atlas id（对齐 UIMapSearchPanel）。
		size_t atlas = ATLAS_BASE;

		// D5：标题「地图信息」烤进底板顶脊。客户端运行时优先，仓库 chrome 兜底。
		chrome.load(atlas++, {
			"UI/MapInfo/info_panel_backgrnd.png",
			"docs/features/map-info-panel/ui/info_panel_backgrnd/v2/info_panel_backgrnd.png",
			"docs/features/map-info-panel/resources/chrome/info_panel_backgrnd.png",
		});

		if (!chrome.is_valid())
			LOG(LOG_WARN, "[UIMapInfoPanel] chrome PNG missing — panel will be empty frame");

		row_selected.load(atlas++, {
			"UI/MapInfo/list_row_selected.png",
			"docs/features/map-info-panel/ui/list_row_selected/v2/list_row_selected.png",
			"docs/features/map-info-panel/resources/chrome/list_row_selected.png",
		});
		slot.load(atlas++, {
			"UI/MapInfo/slot.png",
			"docs/features/map-info-panel/ui/slot/v2/slot.png",
			"docs/features/map-info-panel/resources/chrome/slot.png",
		});

		// v2 原生风：怪物行交替底纹（含图标灰底）。缺图则回退到无底纹（旧观感）。
		row_bg_a.load(atlas++, {
			"UI/MapInfo/mob_row_bg_a.png",
			"docs/features/map-info-panel/ui/mob_row_bg/v2/mob_row_bg_a.png",
		});
		row_bg_b.load(atlas++, {
			"UI/MapInfo/mob_row_bg_b.png",
			"docs/features/map-info-panel/ui/mob_row_bg/v2/mob_row_bg_b.png",
		});

		for (size_t i = 0; i < static_cast<size_t>(Tab::LENGTH); i++)
		{
			LocalPngTexture nsrc;
			LocalPngTexture ssrc;
			std::string key(TAB_NAMES[i]);

			nsrc.load(atlas++, {
				"UI/MapInfo/Tab/disabled/" + key + ".png",
				"docs/features/map-info-panel/ui/tab/v2/" + key + "_disabled.png",
			});
			ssrc.load(atlas++, {
				"UI/MapInfo/Tab/enabled/" + key + ".png",
				"docs/features/map-info-panel/ui/tab/v2/" + key + "_enabled.png",
			});

			tab_buttons[i] = std::make_unique<LocalTwoSpriteButton>(
				std::move(nsrc), std::move(ssrc), Point<int16_t>(TAB_X[i], TAB_Y));

			// 激活态用 PRESSED（与 UIMapSearchPanel 一致）；未激活 NORMAL。
			tab_buttons[i]->set_state(i == 0 ? Button::State::PRESSED : Button::State::NORMAL);
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
		meso_amount = Text(Text::Font::A11B, Text::Alignment::RIGHT, Color::Name::BLACK, "-");

		// D5：标题烤进底板，程序侧不画（防双字）。绑定仍在，供日后切回无字底板复用。
		title_text = Text(Text::Font::A13B, Text::Alignment::CENTER, Color::Name::BLACK, Locale::text("uimapinfopanel.223"));
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
		// 底板按设计稿 240×537 绘制，禁止随世界地图框高拉伸——拉伸后
		// GRID/TAB/MESO 常量仍按 537 坐标系，格子会溢出下框（真机已踩）。
		(void)worldmap_height;
		panel_height = HEIGHT;
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
			chrome.draw(position); // 固定 240×537，不拉伸

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
					icon_back.draw(row_pos + Point<int16_t>(ICON_PLATE_X, ICON_PLATE_Y));

				if (static_cast<int16_t>(idx) == selected_index && row_selected.is_valid())
					row_selected.draw(row_pos);

				const MobRow& row = mob_rows[idx];

				if (row.icon.is_valid())
				{
					// 图标在 40×40 灰底内居中（origin 已归零）。
					const int16_t iw = static_cast<int16_t>(row.icon.width() * row.icon_scale);
					const int16_t ih = static_cast<int16_t>(row.icon.height() * row.icon_scale);
					Point<int16_t> icon_pos = row_pos + Point<int16_t>(
						ICON_PLATE_X + (ICON_BOX - iw) / 2,
						ICON_PLATE_Y + (ICON_BOX - ih) / 2);
					row.icon.draw(DrawArgument(icon_pos, row.icon_scale, row.icon_scale));
				}

				row.name_label.draw(row_pos + Point<int16_t>(TEXT_X, TEXT_NAME_Y));
				row.type_label.draw(row_pos + Point<int16_t>(TEXT_X, TEXT_TYPE_Y));
				row.stat_label.draw(row_pos + Point<int16_t>(TEXT_X, TEXT_STAT_Y));
				row.attr_label.draw(row_pos + Point<int16_t>(TEXT_X, TEXT_ATTR_Y));
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

						// 底板烤格已移除（用户 2026-11-22 要求），运行时画唯一一套 4×4 格子。
						if (slot.is_valid())
							slot.draw(cell_pos, SLOT, SLOT);

						const DropCell& cell = drop_cells[idx];

						// 格内分区：上方图标区居中，下方爆率条（运行时画，底板已无烤格）。
						const int16_t bar_h = 13;
						const int16_t icon_area_h = SLOT - bar_h; // 图标区 = 格高 - 爆率条
						if (cell.icon.is_valid())
						{
							float scale = 1.0f;
							int16_t max_dim = cell.icon.width() > cell.icon.height()
								? cell.icon.width() : cell.icon.height();
							if (max_dim > icon_area_h && max_dim > 0)
								scale = static_cast<float>(icon_area_h) / static_cast<float>(max_dim);

							const int16_t iw = static_cast<int16_t>(cell.icon.width() * scale);
							const int16_t ih = static_cast<int16_t>(cell.icon.height() * scale);
							// 水平居中整格、垂直居中图标区 → 每个图标落在其格子中心。
							Point<int16_t> off(
								static_cast<int16_t>((SLOT - iw) / 2),
								static_cast<int16_t>((icon_area_h - ih) / 2));
							cell.icon.draw(DrawArgument(cell_pos + off, scale, scale));
						}

						if (static_cast<size_t>(idx) == static_cast<size_t>(hover_drop_idx))
							drop_hover.draw(cell_pos);

						// 数量字：右上（避开底部爆率条）；ItemNo 字库无 "." 和 "%"，
						// 故数量只用纯数字，爆率改用 Text（支持小数点/百分号）。
						int16_t qty = (cell.min_qty == cell.max_qty) ? cell.min_qty : cell.max_qty;
						static const Charset countset(nl::nx::UI["Basic.img"]["ItemNo"], Charset::Alignment::RIGHT);

						if (qty > 1)
							countset.draw(std::to_string(qty), DrawArgument(cell_pos + Point<int16_t>(SLOT - 2, 0)));

						if (cell.chance > 0.0)
						{
							// 用户 2026-11-22 18:04 反馈"看不清爆率"：根因是 Text::draw 的
							// 传入点为基线，字形顶在基线上方约 9px —— 旧代码把基线放在
							// bar_pos.y+0，导致字大半画在深色条上方、压在图标/蓝格线上。
							// 修复：深色条加深到 0.78、基线放进条内靠下（字形完整落在条中），
							// 字号 A12B 更亮，LEFT+显式居中避免 CENTER 与手算叠加偏左。
							// bar_h 与上方图标区共用同一个 13px 分区高度。
							std::ostringstream oss;
							oss << std::fixed
								<< std::setprecision(cell.chance < 0.01 ? 1 : 0)
								<< (cell.chance * 100.0) << "%";
							const std::string rate_text = oss.str();
							const Point<int16_t> bar_pos = cell_pos + Point<int16_t>(0, SLOT - 13);
							ColorBox rate_bg(SLOT, 13, Color::Name::BLACK, 0.78f);
							rate_bg.draw(bar_pos);
							Text rate(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::YELLOW, rate_text);
							const int16_t rx = cell_pos.x() + static_cast<int16_t>((SLOT - rate.width()) / 2);
							// 墨迹顶对齐到黑条内 (bar_pos.y+1)，确保整行字落在灰/黑背景中。
							const int16_t ink_top = bar_pos.y() + 1;
							rate.draw(Point<int16_t>(rx, rate.y_for_ink_top(ink_top)));
						}
					}
				}
			}

			if (drop_slider.isenabled())
				drop_slider.draw(position);
		}

		// 底栏 mesos 前白框：金币区间画在框内垂直居中（用户 2026-11-22 要求）。
		std::string meso_str = meso_drop_range.empty() ? "-" : meso_drop_range;
		meso_amount.change_text(meso_str);
		const int16_t meso_ink_top = MESO_Y + (MESO_H - meso_amount.height()) / 2;
		meso_amount.draw(Point<int16_t>(
			position.x() + MESO_X + MESO_FIELD_W - 2,
			position.y() + meso_amount.y_for_ink_top(meso_ink_top)));
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

	void UIMapInfoPanel::send_scroll(double yoffset)
	{
		if (!visible)
			return;

		Point<int16_t> cursor = UI::get().get_cursor_position();
		Point<int16_t> local = cursor - position;

		// 上区怪物列表：按列表区判定；下区掉落网格：按网格区判定。
		Rectangle<int16_t> ma = mob_list_area();
		Rectangle<int16_t> ga = drop_grid_area();

		if (ma.contains(local) && mob_slider.isenabled())
			mob_slider.send_scroll(yoffset);
		else if (ga.contains(local) && drop_slider.isenabled())
			drop_slider.send_scroll(yoffset);
	}

	Cursor::State UIMapInfoPanel::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (!visible || !get_bounds().contains(cursorpos))
			return Cursor::State::IDLE;

		// Tabs — hover + click；激活态固定 PRESSED（enabled 精灵）。
		for (size_t i = 0; i < static_cast<size_t>(Tab::LENGTH); i++)
		{
			auto* btn = tab_buttons[i].get();
			const bool is_active = (i == static_cast<size_t>(active_tab));
			Rectangle<int16_t> b = btn->bounds(position);

			if (b.contains(cursorpos))
			{
				if (!is_active && btn->get_state() == Button::State::NORMAL)
				{
					Sound(Sound::Name::BUTTONOVER).play();
					btn->set_state(Button::State::MOUSEOVER);
				}

				if (clicked && !is_active)
				{
					Sound(Sound::Name::BUTTONCLICK).play();
					set_tab(static_cast<Tab>(i));
					return Cursor::State::CLICKING;
				}

				return Cursor::State::CANCLICK;
			}
			else if (!is_active && btn->get_state() == Button::State::MOUSEOVER)
			{
				btn->set_state(Button::State::NORMAL);
			}
			else if (is_active && btn->get_state() != Button::State::PRESSED)
			{
				btn->set_state(Button::State::PRESSED);
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
			// drop_grid_area() 返回面板局部坐标；命中判定必须加 position 转全局。
			// 旧版直接拿局部矩形比全局 cursorpos → 命中区错位到屏幕左上，
			// 只有前几个格子"碰巧"落在错误区里才弹 tooltip（用户实测时有时无）。
			Rectangle<int16_t> g = drop_grid_area();
			Rectangle<int16_t> g_abs(
				position + Point<int16_t>(g.left(), g.top()),
				position + Point<int16_t>(g.right(), g.bottom()));

			if (g_abs.contains(cursorpos))
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

			// 属性/抗性接 192 elemAttr 数据（I/L/F=属性攻击，S/H/D/P=抗性）。
			std::string attack_attrs;
			std::string resist_attrs;
			std::string decoded = data::elem_attr_display(mob.elem_attr);
			if (decoded.empty())
			{
				attack_attrs = "-";
				resist_attrs = "-";
			}
			else
			{
				// decoded 形如 "冰:弱 火:抵抗 毒:免疫"；按关键字归类到两栏。
				std::stringstream ss(decoded);
				std::string token;
				while (ss >> token)
				{
					bool is_attack = token.find("冰:") == 0 || token.find("雷:") == 0 || token.find("火:") == 0;
					std::string& bucket = is_attack ? attack_attrs : resist_attrs;
					if (!bucket.empty())
						bucket += " ";
					bucket += token;
				}
				if (attack_attrs.empty()) attack_attrs = "-";
				if (resist_attrs.empty()) resist_attrs = "-";
			}
			row.attr_label = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::GRAY,
				Locale::format("mapinfo.attr", attack_attrs) + "  " + Locale::format("mapinfo.resist", resist_attrs));

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
		meso_drop_range.clear();

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
				std::string lo = std::to_string(d.min_qty);
				std::string hi = std::to_string(d.max_qty);
				string_format::split_number(lo);
				string_format::split_number(hi);
				meso_drop_range = (d.min_qty == d.max_qty) ? lo : (lo + "~" + hi);
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
				cell.icon = item.get_icon(d.item_id / 10000 == 238);

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
			tab_buttons[i]->set_state(i == static_cast<size_t>(tab) ? Button::State::PRESSED : Button::State::NORMAL);

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
