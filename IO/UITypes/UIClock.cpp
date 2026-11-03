//////////////////////////////////////////////////////////////////////////////////
//	Field Clock / Timer HUD
//////////////////////////////////////////////////////////////////////////////////
#include "UIClock.h"

#include "UISoftKey.h"

#include "../UI.h"
#include "../../Constants.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/Misc.h"

#include <string>
#include "../../Util/Locale.h"

namespace ms
{
	UIClock::UIClock()
		: label(Text::Font::A18M, Text::Alignment::CENTER, Color::Name::YELLOW)
	{
		int16_t w = Constants::Constants::get().get_viewwidth();
		int16_t h = Constants::Constants::get().get_viewheight();
		update_screen(w, h);
	}

	void UIClock::set_clock(int8_t type, const std::string& text, int32_t secs)
	{
		mode = type;
		seconds = secs;
		visible = true;
		if (mode == 2 && seconds > 0)
		{
			remain_ms = seconds * 1000;
			label.change_text(Locale::format("clock.seconds_left", std::to_string(seconds)));
		}
		else
		{
			remain_ms = 0;
			label.change_text(text.empty() ? Locale::text("clock.title") : text);
		}
		LOG(LOG_DEBUG, "UIClock: mode=" << (int)mode << " secs=" << seconds
			<< " text=" << label.get_text());
	}

	void UIClock::stop()
	{
		visible = false;
		mode = 0;
		remain_ms = 0;
		seconds = 0;
		label.change_text("");
		LOG(LOG_DEBUG, "UIClock: stop");
	}

	void UIClock::draw(float) const
	{
		if (!visible)
			return;
		label.draw(position);
	}

	void UIClock::update()
	{
		if (!visible || mode != 2 || remain_ms <= 0)
			return;
		remain_ms -= Constants::TIMESTEP;
		int32_t left = remain_ms > 0 ? (remain_ms + 999) / 1000 : 0;
		if (left != seconds)
		{
			seconds = left;
			if (seconds > 0)
				label.change_text(Locale::format("clock.seconds_left", std::to_string(seconds)));
			else
			{
				label.change_text(Locale::text("clock.zero"));
				visible = false;
			}
		}
	}

	void UIClock::update_screen(int16_t new_width, int16_t new_height)
	{
		position = Point<int16_t>(
			static_cast<int16_t>(new_width / 2),
			static_cast<int16_t>(40)
		);
		dimension = Point<int16_t>(160, 24);
	}

	void UIClock::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) { stop(); return; }
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				try {
					int32_t n = std::stoi(tip);
					set_clock(2, "", n);
				} catch (...) {
					NpcTalkMorePacket(tip).dispatch();
					set_clock(1, tip, 0);
				}
			}, []() {}, "Clock|sec|tip");
		}
		else if (keycode == 'C' || keycode == 'c' || keycode == 'X' || keycode == 'x')
		{
			stop();
		}
		else if (keycode == 'R' || keycode == 'r')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				set_clock(1, tip, 0);
			}, []() {}, "Clock text");
		}
	}

	UIElement::Type UIClock::get_type() const
	{
		return TYPE;
	}
}
