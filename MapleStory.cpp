//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#include "Gameplay/Stage.h"
#include "IO/LoginBackground.h"
#include "IO/UI.h"
#include "IO/Window.h"
#include "Net/Session.h"
#include "Util/HardwareInfo.h"
#include "Util/ScreenResolution.h"
#include "Configuration.h"

#include "Util/DataFiles.h"

#include <fstream>

namespace ms
{
	// TEMP DEBUG [DBG-START]: Release GUI builds compile LOG() to nothing and
	// have no console. Stage markers are written to a file so the post-data
	// startup path (window/sound/game init) can be diagnosed when the client
	// exits silently. Remove once the startup is verified.
	static void dbg_stage(const char* stage)
	{
		std::ofstream f("_start_debug.log", std::ios::app);
		f << stage << std::endl;
	}

	Error init()
	{
		dbg_stage("[STAGE] begin");

		// 确保配置先加载，宽高会同步到 Constants，Window 创建时才能取到正确分辨率。
		Configuration::get();

		if (Error error = Session::get().init()) {
			dbg_stage("[STAGE] Session::init FAILED");
			return error;
		}
		dbg_stage("[STAGE] Session::init OK");

		// The data backend (nx / wz / img) is selected at runtime from the
		// DataFormat setting, so every format goes through this single entry.
		if (Error error = DataFiles::init()) {
			dbg_stage("[STAGE] DataFiles::init FAILED");
			return error;
		}
		dbg_stage("[STAGE] DataFiles::init OK");

		if (Error error = Window::get().init()) {
			dbg_stage("[STAGE] Window::init FAILED");
			return error;
		}
		dbg_stage("[STAGE] Window::init OK");

		if (Error error = Sound::init()) {
			dbg_stage("[STAGE] Sound::init FAILED");
			return error;
		}
		dbg_stage("[STAGE] Sound::init OK");

		if (Error error = Music::init()) {
			dbg_stage("[STAGE] Music::init FAILED");
			return error;
		}
		dbg_stage("[STAGE] Music::init OK");

		Char::init();
		DamageNumber::init();
		MapPortals::init();
		Stage::get().init();
		UI::get().init();
		LoginBackground::get().init();
		dbg_stage("[STAGE] game init OK");

		return Error::NONE;
	}

	void update()
	{
		Window::get().check_events();
		Window::get().update();
		Stage::get().update();
		UI::get().update();
		Session::get().read();
	}

	void draw(float alpha)
	{
		Window::get().begin();
		Stage::get().draw(alpha);
		UI::get().draw(alpha);
		Window::get().end();
	}

	bool running()
	{
		return Session::get().is_connected()
			&& UI::get().not_quitted()
			&& Window::get().not_closed();
	}

	void loop()
	{
		Timer::get().start();

		int64_t timestep = Constants::TIMESTEP * 1000;
		int64_t accumulator = timestep;

		int64_t period = 0;
		int32_t samples = 0;

		bool show_fps = Configuration::get().get_show_fps();

		while (running())
		{
			int64_t elapsed = Timer::get().stop();

			// Update game with constant timestep as many times as possible.
			for (accumulator += elapsed; accumulator >= timestep; accumulator -= timestep)
				update();

			// Draw the game. Interpolate to account for remaining time.
			float alpha = static_cast<float>(accumulator) / timestep;
			draw(alpha);

			if (show_fps)
			{
				if (samples < 100)
				{
					period += elapsed;
					samples++;
				}
				else if (period)
				{
					int64_t fps = (samples * 1000000) / period;

					LOG(LOG_INFO, "FPS: " << fps);

					period = 0;
					samples = 0;
				}
			}
		}

		Sound::close();
	}

	void start()
	{
		dbg_stage("[START] entering");
		try
		{
			// Initialize and check for errors
			if (Error error = init())
			{
				const char* message = error.get_message();
				const char* args = error.get_args();
				bool can_retry = error.can_retry();

				dbg_stage(("[START] init error: " + std::string(message) + (args ? args : "")).c_str());

				if (args && args[0])
					LOG(LOG_ERROR, message << args);
				else
					LOG(LOG_ERROR, message);

				if (can_retry)
					LOG(LOG_INFO, "Enter 'retry' to try again.");

				std::string command;
				std::cin >> command;

				if (can_retry && command == "retry")
					start();
			}
			else
			{
				dbg_stage("[START] entering loop");
				loop();
			}
		}
		catch (const std::exception& ex)
		{
			dbg_stage(("[START] UNCAUGHT exception: " + std::string(ex.what())).c_str());
		}
	}
}

#ifdef _DEBUG
int main()
#else
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
#endif
{
	ms::HardwareInfo();
	ms::ScreenResolution();
	ms::start();

	return 0;
}