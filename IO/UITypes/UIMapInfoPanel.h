////////////////////////////////////////////////////////////////////////////////
//	UIMapInfoPanel — left-docked map info panel for UIWorldMap
//	240x537; upper mob list (3 rows, 78px) + lower 5x5 drop grid + meso bar.
//	Assets: UI/MapInfo/ (info_panel_backgrnd.png, list_row_selected.png,
//	                      slot.png, Tab/enabled|disabled/{all,equip,consume,etc}.png)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Components/LocalTwoSpriteButton.h"
#include "../Components/Slider.h"

#include "../../Data/ItemData.h"
#include "../../Data/MobDropData.h"
#include "../../Graphics/Geometry.h"
#include "../../Graphics/LocalPngTexture.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"
#include "../../Util/Misc.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace ms
{
	// Adsorb to the left of the world map (replaces UIMapMobPanel slot when enabled).
	// Owned by UIWorldMap. Not a UIElement.
	class UIMapInfoPanel
	{
	public:
		static constexpr int16_t WIDTH = 240;
		static constexpr int16_t HEIGHT = 537;
		static constexpr size_t ATLAS_ID = 0x4D50494Eull; // 'MPIN'

		enum class Tab : uint8_t
		{
			ALL,
			EQUIP,
			USE,
			ETC,
			LENGTH
		};

		using SelectCallback = std::function<void(int32_t mob_id)>;

		UIMapInfoPanel();

		void set_select_callback(SelectCallback callback);
		void set_map(int32_t map_id);
		void set_mob(int32_t mob_id);
		void clear();
		void set_position(Point<int16_t> pos);
		void set_content_height(int16_t worldmap_height);

		bool is_visible() const;
		int32_t get_map_id() const;
		int32_t get_selected_mob_id() const;
		Point<int16_t> get_dimensions() const;
		Rectangle<int16_t> get_bounds() const;

		void draw(float alpha) const;
		void update();
		void remove_cursor();
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos);

	private:
		struct MobRow
		{
			int32_t id = 0;
			Texture icon;
			float icon_scale = 1.0f;
			Text name_label;
			Text type_label;
			Text stat_label;
			Text attr_label;
		};

		struct DropCell
		{
			int32_t item_id = 0;
			double chance = 0.0;
			Texture icon;
		};

		bool load_png(LocalPngTexture& tex, size_t atlas_id, const std::string& filename);
		void rebuild_mobs();
		void rebuild_drops();
		void set_tab(Tab tab);
		void select_mob_row(int16_t index);
		Rectangle<int16_t> mob_list_area() const;
		Rectangle<int16_t> drop_grid_area() const;
		static Texture load_mob_stand_icon(int32_t mob_id);
		static bool classify_drop(Tab tab, int32_t item_id);

		bool visible;
		int32_t map_id;
		int32_t selected_mob_id;
		int16_t selected_index;
		int16_t mob_offset;
		int16_t drop_offset; // in rows of GRID_COLS
		int16_t panel_height;
		Point<int16_t> position;
		Tab active_tab;

		LocalPngTexture chrome;
		LocalPngTexture row_selected;
		LocalPngTexture slot;

		Slider mob_slider;
		Slider drop_slider;

		// Index order matches layout.json tab xs/widths: all, equip, consume, etc.
		std::unique_ptr<LocalTwoSpriteButton> tab_buttons[static_cast<size_t>(Tab::LENGTH)];

		std::vector<MobRow> mob_rows;
		std::vector<DropCell> drop_cells;

		Text empty_label;
		Text empty_drops_label;
		Text meso_label;
		mutable Text meso_amount;

		SelectCallback on_select;
	};
}
