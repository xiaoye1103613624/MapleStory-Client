//////////////////////////////////////////////////////////////////////////////////
//	UIMapMobPanel — left-docked mob list for UIWorldMap
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Components/Slider.h"

#include "../../Graphics/Geometry.h"
#include "../../Graphics/LocalPngTexture.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"
#include "../../Util/Misc.h"

#include <functional>
#include <vector>

namespace ms
{
	// Adsorb to the left of the world map. Not a UIElement — owned by UIWorldMap.
	class UIMapMobPanel
	{
	public:
		static constexpr int16_t WIDTH = 168;
		static constexpr int16_t ROW_HEIGHT = 48;
		static constexpr int16_t VISIBLE_ROWS = 6;
		// Insets matching chrome/mob_panel_backgrnd.png white list well.
		static constexpr int16_t PAD_X = 18;
		static constexpr int16_t PAD_TOP = 48;
		static constexpr int16_t PAD_BOTTOM = 18;
		static constexpr int16_t ICON_BOX = 40;
		static constexpr size_t ATLAS_ID = 0x4D4F4250ull; // 'MOBP'

		using SelectCallback = std::function<void(int32_t mob_id)>;

		UIMapMobPanel();

		void set_select_callback(SelectCallback callback);
		void set_map(int32_t map_id);
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
		struct Row
		{
			int32_t id = 0;
			Texture icon;
			float icon_scale = 1.0f;
			Text name_label;
			Text hp_label;
		};

		void rebuild_rows();
		void select_row(int16_t index);
		Rectangle<int16_t> list_area() const;
		static Texture load_mob_stand_icon(int32_t mob_id);

		bool visible;
		int32_t map_id;
		int16_t selected_index;
		int16_t list_offset;
		int16_t panel_height;
		Point<int16_t> position;

		LocalPngTexture chrome;
		ColorBox select_bg;
		Text empty_label;
		Slider slider;
		std::vector<Row> rows;
		SelectCallback on_select;
	};
}
