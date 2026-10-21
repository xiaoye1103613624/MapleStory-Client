//////////////////////////////////////////////////////////////////////////////////
//	Boot splash: procedural spinner until WZ/IMG ready, then Logo.img/Nexon.
//////////////////////////////////////////////////////////////////////////////////
#include "BootSplash.h"

#include "../Configuration.h"
#include "../Constants.h"
#include "../IO/Window.h"
#include "../Graphics/GraphicsGL.h"
#include "../Util/Misc.h"

#include <algorithm>
#include <cmath>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	BootSplash& BootSplash::get()
	{
		static BootSplash instance;
		return instance;
	}

	BootSplash::BootSplash()
		: nexon_ready(false)
		, nexon_attempted(false)
		, clock_started(false)
		, pending_ms(0)
		, spin_phase(0.0f)
	{
	}

	void BootSplash::clear()
	{
		nexon = Animation();
		nexon_ready = false;
		nexon_attempted = false;
	}

	void BootSplash::advance_clock()
	{
		const auto now = std::chrono::steady_clock::now();

		if (!clock_started)
		{
			clock_started = true;
			last_tick = now;
			pending_ms = Constants::TIMESTEP;
			return;
		}

		const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tick).count();
		last_tick = now;

		// Cap so a stalled hitch does not skip the whole Nexon reel.
		pending_ms = static_cast<uint16_t>(std::clamp<int64_t>(elapsed, 1, 80));
	}

	void BootSplash::try_load_nexon()
	{
		if (nexon_attempted)
			return;

		nexon_attempted = true;

#ifdef USE_NX
		nl::node Logo = nl::nx::UI["Logo.img"];
		nl::node Nexon = Logo["Nexon"];

		if (Nexon.data_type() == nl::node::type::none)
		{
			LOG(LOG_WARN, "[BOOT] Logo.img/Nexon missing; keep procedural splash");
			return;
		}

		nexon = Animation(Nexon);

		Point<int16_t> dim = nexon.get_dimensions();
		Point<int16_t> origin = nexon.get_origin();

		if (dim.x() <= 0 || dim.y() <= 0)
		{
			LOG(LOG_WARN, "[BOOT] Logo.img/Nexon has empty frames; keep procedural splash");
			nexon = Animation();
			return;
		}

		Point<int16_t> screen_center(
			Constants::Constants::get().get_viewwidth() / 2,
			Constants::Constants::get().get_viewheight() / 2
		);

		nexon_pos = screen_center - Point<int16_t>(dim.x() / 2, dim.y() / 2) + origin;
		nexon_ready = true;

		LOG(LOG_INFO, "[BOOT] Logo.img/Nexon loaded dim=" << dim.x() << "x" << dim.y());
#else
		LOG(LOG_WARN, "[BOOT] USE_NX off; keep procedural splash");
#endif
	}

	void BootSplash::draw_procedural(const char* message)
	{
		GraphicsGL& gl = GraphicsGL::get();
		const int16_t vw = Constants::Constants::get().get_viewwidth();
		const int16_t vh = Constants::Constants::get().get_viewheight();
		const int16_t cx = vw / 2;
		const int16_t cy = vh / 2;

		gl.drawscreenfill(0.05f, 0.05f, 0.07f, 1.0f);

		spin_phase += pending_ms * 0.012f;
		if (spin_phase > 6.2831853f)
			spin_phase -= 6.2831853f;

		constexpr int kArms = 12;
		constexpr float kRadius = 28.0f;

		for (int i = 0; i < kArms; ++i)
		{
			const float t = spin_phase + (6.2831853f * i) / kArms;
			const float fade = 0.25f + 0.75f * (static_cast<float>(i) / kArms);
			const int16_t x0 = static_cast<int16_t>(cx + std::cos(t) * (kRadius - 6.0f));
			const int16_t y0 = static_cast<int16_t>(cy + std::sin(t) * (kRadius - 6.0f) - 24);
			const int16_t x1 = static_cast<int16_t>(cx + std::cos(t) * (kRadius + 8.0f));
			const int16_t y1 = static_cast<int16_t>(cy + std::sin(t) * (kRadius + 8.0f) - 24);

			const int16_t left = std::min(x0, x1);
			const int16_t top = std::min(y0, y1);
			const int16_t w = static_cast<int16_t>(std::max(3, std::abs(x1 - x0)));
			const int16_t h = static_cast<int16_t>(std::max(3, std::abs(y1 - y0)));

			gl.drawrectangle(left, top, w, h, 0.55f, 0.75f, 1.0f, fade);
		}

		// Thin progress shimmer under the spinner (indeterminate).
		const int16_t bar_w = 180;
		const int16_t bar_h = 4;
		const int16_t bar_x = cx - bar_w / 2;
		const int16_t bar_y = cy + 20;
		gl.drawrectangle(bar_x, bar_y, bar_w, bar_h, 0.18f, 0.20f, 0.24f, 1.0f);

		const float pulse = 0.5f + 0.5f * std::sin(spin_phase * 2.0f);
		const int16_t fill_w = static_cast<int16_t>(40 + pulse * (bar_w - 40));
		const int16_t fill_x = bar_x + static_cast<int16_t>((bar_w - fill_w) * (0.5f + 0.5f * std::sin(spin_phase)));
		gl.drawrectangle(fill_x, bar_y, fill_w, bar_h, 0.45f, 0.70f, 1.0f, 0.9f);

		if (message && message[0])
		{
			Text label(Text::Font::A18M, Text::Alignment::CENTER, Color::Name::WHITE, message);
			label.draw(DrawArgument(cx, static_cast<int16_t>(cy + 48)));
		}
	}

	void BootSplash::draw_nexon(const char* message)
	{
		GraphicsGL& gl = GraphicsGL::get();
		const int16_t vw = Constants::Constants::get().get_viewwidth();
		const int16_t vh = Constants::Constants::get().get_viewheight();

		// Letterbox behind non-fullscreen frames (720x480 on 1280x720 etc.).
		gl.drawscreenfill(0.0f, 0.0f, 0.0f, 1.0f);

		nexon.update(pending_ms);

		Point<int16_t> dim = nexon.get_dimensions();
		Point<int16_t> origin = nexon.get_origin();
		Point<int16_t> screen_center(vw / 2, vh / 2);
		nexon_pos = screen_center - Point<int16_t>(dim.x() / 2, dim.y() / 2) + origin;

		nexon.draw(nexon_pos, 1.0f);

		if (message && message[0])
		{
			Text label(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, message);
			label.draw(DrawArgument(vw / 2, static_cast<int16_t>(vh - 36)));
		}
	}

	void BootSplash::pump(const char* message)
	{
		if (message)
			last_message = message;

		advance_clock();

		Window::get().check_events();
		Window::get().begin();

		if (nexon_ready)
			draw_nexon(last_message.c_str());
		else
			draw_procedural(last_message.c_str());

		Window::get().end();
	}
}
