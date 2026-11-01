//////////////////////////////////////////////////////////////////////////////////
//	UIMapSearchPanel
//////////////////////////////////////////////////////////////////////////////////
#include "UIMapSearchPanel.h"

#include "../../Audio/Audio.h"
#include "../../MapleStory.h"
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
		search_text_dim(Point<int16_t>(SEARCH_W - 8, 12)),
		search_offset(Point<int16_t>(SEARCH_X + 4, SEARCH_Y + 3))
	{
		size_t atlas = ATLAS_BASE;

		if (!load_png(chrome, atlas++, "search_panel_backgrnd.png"))
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
		search_text.draw(Point<int16_t>(0, 0));
	}

	void UIMapSearchPanel::update()
	{
		if (!visible)
			return;

		Point<int16_t> field_pos = position + search_offset;
		search_text.update(field_pos, search_text_dim);
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
}
