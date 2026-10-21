//////////////////////////////////////////////////////////////////////////////////
//	Boot splash shown while patch / resource / session init runs.
//	Pre-WZ: procedural dark screen + spinner. Post-WZ: Logo.img/Nexon loop.
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Graphics/Animation.h"
#include "../Graphics/Text.h"
#include "../Template/Point.h"

#include <chrono>
#include <cstdint>
#include <string>

namespace ms
{
	class BootSplash
	{
	public:
		static BootSplash& get();

		// Draw one splash frame. Safe before and after ResourceService::init().
		void pump(const char* message);

		// Load Logo.img/Nexon once UI root is bound. No-op if already loaded / missing.
		void try_load_nexon();

		// Drop logo textures before UI atlas pressure (optional).
		void clear();

	private:
		BootSplash();

		void draw_procedural(const char* message);
		void draw_nexon(const char* message);
		void advance_clock();

		Animation nexon;
		Point<int16_t> nexon_pos;
		bool nexon_ready;
		bool nexon_attempted;

		std::chrono::steady_clock::time_point last_tick;
		bool clock_started;
		uint16_t pending_ms;
		float spin_phase;
		std::string last_message;
	};
}
