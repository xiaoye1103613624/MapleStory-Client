//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//////////////////////////////////////////////////////////////////////////////////
#include "UIHelp.h"

#include "../UI.h"

#include "../Components/MapleButton.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIHelp::UIHelp()
	{
		// 复用 Login Notice 羊皮纸底板，避免缺 Help 专用窗体时卡死。
		nl::node notice = nl::nx::UI["Login.img"]["Notice"];
		nl::node backgrnd = notice["backgrnd"]["0"];
		nl::node bt_yes = notice["BtYes"];

		Texture bg_tex(backgrnd);
		dimension = bg_tex.get_dimensions();
		sprites.emplace_back(backgrnd);

		title = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::BLACK, "Help");
		body = Text(
			Text::Font::A11M,
			Text::Alignment::LEFT,
			Color::Name::BLACK,
			"Arrow keys : move\\n"
			"Alt : jump\\n"
			"Ctrl : attack\\n"
			"I : inventory   S : stats\\n"
			"K : skills     Q : quest\\n"
			"M : world map  Esc : menu",
			static_cast<uint16_t>(dimension.x() - 48),
			true
		);

		Texture yes_tex(bt_yes["normal"]["0"]);
		const int16_t btn_w = yes_tex.get_dimensions().x();
		const int16_t btn_h = yes_tex.get_dimensions().y();
		const int16_t btn_x = static_cast<int16_t>((dimension.x() - btn_w) / 2);
		const int16_t btn_y = static_cast<int16_t>(dimension.y() - btn_h - 24);
		buttons[Buttons::BT_OK] = std::make_unique<MapleButton>(bt_yes, Point<int16_t>(btn_x, btn_y));

		center_on_screen();
	}

	void UIHelp::draw(float inter) const
	{
		UIElement::draw(inter);

		const int16_t cx = static_cast<int16_t>(position.x() + dimension.x() / 2);
		title.draw(Point<int16_t>(cx, static_cast<int16_t>(position.y() + 28)));
		body.draw(Point<int16_t>(static_cast<int16_t>(position.x() + 28), static_cast<int16_t>(position.y() + 52)));
	}

	void UIHelp::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && (escape || keycode == KeyAction::Id::RETURN))
			deactivate();
	}

	void UIHelp::update_screen(int16_t, int16_t)
	{
		center_on_screen();
	}

	UIElement::Type UIHelp::get_type() const
	{
		return TYPE;
	}

	Button::State UIHelp::button_pressed(uint16_t)
	{
		deactivate();
		return Button::State::NORMAL;
	}
}
