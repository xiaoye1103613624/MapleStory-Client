//////////////////////////////////////////////////////////////////////////////////
//	UIMapSearchPanel — right-docked map-search shell outside WorldMapSearch
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Components/LocalTwoSpriteButton.h"
#include "../Components/Textfield.h"
#include "../Cursor.h"

#include "../../Graphics/LocalPngTexture.h"
#include "../../Template/Rectangle.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace ms
{
	// Adsorb outside the original WorldMapSearch rail. Owned by UIWorldMap.
	// Tabs follow inventory: Tab/enabled|disabled + LocalTwoSpriteButton (PRESSED=active).
	class UIMapSearchPanel
	{
	public:
		static constexpr int16_t WIDTH = 260;
		static constexpr int16_t HEIGHT = 537;
		static constexpr size_t ATLAS_BASE = 0x4D535248ull; // 'MSRH'

		enum class Tab : uint8_t
		{
			ALL,
			MAP,
			MOB,
			DROP,
			NPC,
			LENGTH
		};

		enum class BtnVisual : uint8_t
		{
			NORMAL,
			MOUSEOVER,
			PRESSED,
			DISABLED,
			LENGTH
		};

		UIMapSearchPanel();

		void set_position(Point<int16_t> pos);
		void set_content_height(int16_t worldmap_height);

		bool is_visible() const;
		Point<int16_t> get_dimensions() const;
		Rectangle<int16_t> get_bounds() const;

		void draw(float alpha) const;
		void update();
		void remove_cursor();
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos);

		void set_teleport_enabled(bool enable);
		void set_teleport_handler(std::function<void()> handler);
		void set_back_handler(std::function<void()> handler);

	private:
		struct PngButton
		{
			LocalPngTexture textures[static_cast<size_t>(BtnVisual::LENGTH)];
			Point<int16_t> offset;
			Point<int16_t> size;
			BtnVisual visual = BtnVisual::NORMAL;
			bool enabled = true;

			Rectangle<int16_t> bounds(Point<int16_t> panel_pos) const;
			void draw(Point<int16_t> panel_pos) const;
			Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos, Point<int16_t> panel_pos);
		};

		bool load_png(LocalPngTexture& tex, size_t atlas_id, const std::string& filename);
		void set_tab(Tab tab);
		void layout_for_height(int16_t worldmap_height);

		bool visible;
		int16_t panel_height;
		Point<int16_t> position;
		Tab active_tab;

		LocalPngTexture chrome;
		std::unique_ptr<LocalTwoSpriteButton> tab_buttons[static_cast<size_t>(Tab::LENGTH)];

		PngButton bt_back;
		PngButton bt_teleport;
		std::function<void()> teleport_handler;
		std::function<void()> back_handler;

		Textfield search_text;
		Point<int16_t> search_text_dim;
		Point<int16_t> search_offset;

		// layout.json — Tab shelf under search field
		static constexpr int16_t TAB_COUNT = static_cast<int16_t>(Tab::LENGTH);
		static constexpr int16_t TAB_X[TAB_COUNT] = { 18, 62, 106, 150, 194 };
		static constexpr int16_t TAB_W[TAB_COUNT] = { 44, 44, 44, 44, 47 };
		static constexpr int16_t TAB_Y = 65;

		static constexpr int16_t SEARCH_X = 18;
		static constexpr int16_t SEARCH_Y = 42;
		static constexpr int16_t SEARCH_W = 223;
		static constexpr int16_t SEARCH_H = 18;

		static constexpr int16_t BACK_X = 18;
		static constexpr int16_t BACK_Y = 90;
		static constexpr int16_t BACK_W = 44;
		static constexpr int16_t BACK_H = 18;

		static constexpr int16_t TELE_X = 177;
		static constexpr int16_t TELE_Y = 484;
		static constexpr int16_t TELE_W = 64;
		static constexpr int16_t TELE_H = 22;
	};
}
