////////////////////////////////////////////////////////////////////////////////
//	公会 Boss PQ HUD
////////////////////////////////////////////////////////////////////////////////
#include "UIGuildBoss.h"

#include "../../Constants.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"

namespace ms
{
	UIGuildBoss::UIGuildBoss()
		: title(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::YELLOW, Color::Name::BLACK),
		line_healer(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE, Color::Name::BLACK),
		line_pulley(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::LIGHTGREY, Color::Name::BLACK)
	{
		dimension = Point<int16_t>(280, 64);
		int16_t w = Constants::Constants::get().get_viewwidth();
		update_screen(w, Constants::Constants::get().get_viewheight());
		LOG(LOG_INFO, "[GuildBoss] HUD ctor");
	}

	void UIGuildBoss::update_screen(int16_t new_width, int16_t)
	{
		// 顶栏居中，避开 UIBossHP 条带（约 y=0..40）
		position = Point<int16_t>(
			static_cast<int16_t>((new_width - dimension.x()) / 2), 48);
	}

	void UIGuildBoss::bump_visible()
	{
		visible = true;
		hide_ms = 12000;
		if (!active)
			makeactive();
	}

	void UIGuildBoss::set_healer_y(int16_t y)
	{
		healer_y = y;
		has_healer = true;
		bump_visible();
		LOG(LOG_INFO, "[GuildBoss] healer y=" << y);
	}

	void UIGuildBoss::set_pulley_state(int8_t state)
	{
		pulley_state = state;
		has_pulley = true;
		bump_visible();
		LOG(LOG_INFO, "[GuildBoss] pulley state=" << (int)state);
	}

	void UIGuildBoss::draw(float) const
	{
		if (!visible || hide_ms <= 0)
			return;

		title.change_text(Locale::text("guildboss.title"));
		title.draw(position + Point<int16_t>(dimension.x() / 2, 4));

		if (has_healer)
		{
			line_healer.change_text(Locale::format("guildboss.healer_y",
				std::to_string(healer_y)));
			line_healer.draw(position + Point<int16_t>(dimension.x() / 2, 24));
		}
		if (has_pulley)
		{
			line_pulley.change_text(Locale::format("guildboss.pulley_state",
				std::to_string(pulley_state)));
			line_pulley.draw(position + Point<int16_t>(dimension.x() / 2,
				has_healer ? 42 : 24));
		}
	}

	void UIGuildBoss::update()
	{
		if (!visible)
			return;
		hide_ms -= Constants::TIMESTEP;
		if (hide_ms <= 0)
		{
			visible = false;
			deactivate();
			LOG(LOG_DEBUG, "[GuildBoss] HUD auto-hide");
		}
	}

	void UIGuildBoss::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape)
		{
			visible = false;
			hide_ms = 0;
			deactivate();
		}
		(void)keycode;
	}

	UIElement::Type UIGuildBoss::get_type() const
	{
		return TYPE;
	}
}
