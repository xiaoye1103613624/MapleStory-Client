//////////////////////////////////////////////////////////////////////////////////
//	UIMapDropPanel — right-docked drop list shell for UIWorldMap
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Components/Slider.h"

#include "../../Data/MobDropData.h"
#include "../../Graphics/Geometry.h"
#include "../../Graphics/LocalPngTexture.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <vector>

namespace ms
{
	// Adsorb to the right of the world map (outside search rail). Owned by UIWorldMap.
	class UIMapDropPanel
	{
	public:
		static constexpr int16_t WIDTH = 210;
		static constexpr int16_t ROW_HEIGHT = 36;
		static constexpr int16_t VISIBLE_ROWS = 7;
		static constexpr int16_t PAD_X = 18;
		static constexpr int16_t PAD_TOP = 48;
		static constexpr int16_t PAD_BOTTOM = 18;
		static constexpr size_t ATLAS_ID = 0x44524F50ull; // 'DROP'

		UIMapDropPanel();

		void set_mob(int32_t mob_id);
		void clear();
		void set_position(Point<int16_t> pos);
		void set_content_height(int16_t worldmap_height);

		bool is_visible() const;
		bool has_mob() const;
		int32_t get_mob_id() const;
		Point<int16_t> get_dimensions() const;
		Rectangle<int16_t> get_bounds() const;

		void draw(float alpha) const;
		void update();
		void remove_cursor();
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos);

	private:
		struct Row
		{
			int32_t item_id = 0;
			Texture icon;
			Text id_label;
			Text name_label;
			Text meta_label;
		};

		void rebuild_rows();
		Rectangle<int16_t> list_area() const;

		bool visible;
		bool has_selection;
		int32_t mob_id;
		int16_t list_offset;
		int16_t hover_index;
		int16_t panel_height;
		Point<int16_t> position;

		LocalPngTexture chrome;
		ColorBox hover_bg;
		Text empty_label;
		Text empty_drops_label;
		Slider slider;
		std::vector<Row> rows;
	};
}
