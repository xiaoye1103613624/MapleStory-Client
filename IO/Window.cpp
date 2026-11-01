////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////
#include "Window.h"

#include "UI.h"

#include "../Character/CharStats.h"
#include "../Character/Player.h"
#include "../Data/JobData.h"
#include "../Gameplay/Stage.h"

#include "../Configuration.h"
#include "../MapleStory.h"
#include "../Timer.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <imm.h>
#pragma comment(lib, "imm32.lib")
#endif

#include <SDL3/SDL.h>

// stb_image 的单例实现：仅本翻译单元定义，供下方 init() 加载窗口图标 Icon.png 使用。
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <string>

#include <fstream>
#include <vector>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>

namespace ms
{
	// The game's key handling is expressed with GLFW's key code values (see
	// IO/KeyCodes.h): printable keys are their ASCII/Unicode code point and
	// special keys start at 256. SDL reports USB HID based scancodes instead,
	// so every incoming key is translated back into the matching code here.
	// Translating at this single point keeps the key tables and every bit of
	// game logic untouched.
	int32_t glfw_key_from_scancode(SDL_Scancode scancode)
	{
		// Letters: GLFW uses the upper-case ASCII code.
		if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z)
			return GLFW_KEY_A + static_cast<int32_t>(scancode - SDL_SCANCODE_A);

		// Digits: the scancode order is 1..9 followed by 0.
		if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_9)
			return GLFW_KEY_1 + static_cast<int32_t>(scancode - SDL_SCANCODE_1);

		switch (scancode)
		{
			case SDL_SCANCODE_0:
				return GLFW_KEY_0;
			case SDL_SCANCODE_RETURN:
				return GLFW_KEY_ENTER;
			case SDL_SCANCODE_ESCAPE:
				return GLFW_KEY_ESCAPE;
			case SDL_SCANCODE_BACKSPACE:
				return GLFW_KEY_BACKSPACE;
			case SDL_SCANCODE_TAB:
				return GLFW_KEY_TAB;
			case SDL_SCANCODE_SPACE:
				return GLFW_KEY_SPACE;
			case SDL_SCANCODE_CAPSLOCK:
				return GLFW_KEY_CAPS_LOCK;
			case SDL_SCANCODE_F1:
				return GLFW_KEY_F1;
			case SDL_SCANCODE_F2:
				return GLFW_KEY_F2;
			case SDL_SCANCODE_F3:
				return GLFW_KEY_F3;
			case SDL_SCANCODE_F4:
				return GLFW_KEY_F4;
			case SDL_SCANCODE_F5:
				return GLFW_KEY_F5;
			case SDL_SCANCODE_F6:
				return GLFW_KEY_F6;
			case SDL_SCANCODE_F7:
				return GLFW_KEY_F7;
			case SDL_SCANCODE_F8:
				return GLFW_KEY_F8;
			case SDL_SCANCODE_F9:
				return GLFW_KEY_F9;
			case SDL_SCANCODE_F10:
				return GLFW_KEY_F10;
			case SDL_SCANCODE_F11:
				return GLFW_KEY_F11;
			case SDL_SCANCODE_F12:
				return GLFW_KEY_F12;
			case SDL_SCANCODE_PRINTSCREEN:
				return GLFW_KEY_PRINT_SCREEN;
			case SDL_SCANCODE_SCROLLLOCK:
				return GLFW_KEY_SCROLL_LOCK;
			case SDL_SCANCODE_PAUSE:
				return GLFW_KEY_PAUSE;
			case SDL_SCANCODE_INSERT:
				return GLFW_KEY_INSERT;
			case SDL_SCANCODE_HOME:
				return GLFW_KEY_HOME;
			case SDL_SCANCODE_PAGEUP:
				return GLFW_KEY_PAGE_UP;
			case SDL_SCANCODE_DELETE:
				return GLFW_KEY_DELETE;
			case SDL_SCANCODE_END:
				return GLFW_KEY_END;
			case SDL_SCANCODE_PAGEDOWN:
				return GLFW_KEY_PAGE_DOWN;
			case SDL_SCANCODE_RIGHT:
				return GLFW_KEY_RIGHT;
			case SDL_SCANCODE_LEFT:
				return GLFW_KEY_LEFT;
			case SDL_SCANCODE_DOWN:
				return GLFW_KEY_DOWN;
			case SDL_SCANCODE_UP:
				return GLFW_KEY_UP;
			case SDL_SCANCODE_NUMLOCKCLEAR:
				return GLFW_KEY_NUM_LOCK;
			case SDL_SCANCODE_KP_DIVIDE:
				return GLFW_KEY_KP_DIVIDE;
			case SDL_SCANCODE_KP_MULTIPLY:
				return GLFW_KEY_KP_MULTIPLY;
			case SDL_SCANCODE_KP_MINUS:
				return GLFW_KEY_KP_SUBTRACT;
			case SDL_SCANCODE_KP_PLUS:
				return GLFW_KEY_KP_ADD;
			case SDL_SCANCODE_KP_ENTER:
				return GLFW_KEY_KP_ENTER;
			case SDL_SCANCODE_KP_1:
				return GLFW_KEY_KP_1;
			case SDL_SCANCODE_KP_2:
				return GLFW_KEY_KP_2;
			case SDL_SCANCODE_KP_3:
				return GLFW_KEY_KP_3;
			case SDL_SCANCODE_KP_4:
				return GLFW_KEY_KP_4;
			case SDL_SCANCODE_KP_5:
				return GLFW_KEY_KP_5;
			case SDL_SCANCODE_KP_6:
				return GLFW_KEY_KP_6;
			case SDL_SCANCODE_KP_7:
				return GLFW_KEY_KP_7;
			case SDL_SCANCODE_KP_8:
				return GLFW_KEY_KP_8;
			case SDL_SCANCODE_KP_9:
				return GLFW_KEY_KP_9;
			case SDL_SCANCODE_KP_0:
				return GLFW_KEY_KP_0;
			case SDL_SCANCODE_KP_PERIOD:
				return GLFW_KEY_KP_DECIMAL;
			case SDL_SCANCODE_KP_EQUALS:
				return GLFW_KEY_KP_EQUAL;
			case SDL_SCANCODE_LCTRL:
				return GLFW_KEY_LEFT_CONTROL;
			case SDL_SCANCODE_LSHIFT:
				return GLFW_KEY_LEFT_SHIFT;
			case SDL_SCANCODE_LALT:
				return GLFW_KEY_LEFT_ALT;
			case SDL_SCANCODE_LGUI:
				return GLFW_KEY_LEFT_SUPER;
			case SDL_SCANCODE_RCTRL:
				return GLFW_KEY_RIGHT_CONTROL;
			case SDL_SCANCODE_RSHIFT:
				return GLFW_KEY_RIGHT_SHIFT;
			case SDL_SCANCODE_RALT:
				return GLFW_KEY_RIGHT_ALT;
			case SDL_SCANCODE_RGUI:
				return GLFW_KEY_RIGHT_SUPER;
			default:
				break;
		}

		// Any other key that produces a printable character: SDL's keycode is
		// the same code point that GLFW uses.
		SDL_Keycode keycode = SDL_GetKeyFromScancode(scancode, SDL_KMOD_NONE, false);

		if (keycode >= 32 && keycode <= 126)
			return static_cast<int32_t>(keycode);

		return 0;
	}

	void send_cursor_position(float xpos, float ypos)
	{
		Point<int16_t> cursor_position = Point<int16_t>(
			static_cast<int16_t>(xpos),
			static_cast<int16_t>(ypos)
			);

		Point<int16_t> screen = Point<int16_t>(
			Constants::Constants::get().get_viewwidth(),
			Constants::Constants::get().get_viewheight()
			);

		if (cursor_position.x() > 0 && cursor_position.y() > 0)
			if (cursor_position.x() < screen.x() && cursor_position.y() < screen.y())
				UI::get().send_cursor(cursor_position);
	}

	std::chrono::time_point<std::chrono::steady_clock> start = ContinuousTimer::get().start();

	void send_mouse_button(const SDL_MouseButtonEvent& event)
	{
		switch (event.button)
		{
			case SDL_BUTTON_LEFT:
			{
				if (event.down)
				{
					UI::get().send_cursor(true);
				}
				else
				{
					auto diff_ms = ContinuousTimer::get().stop(start) / 1000;

					start = ContinuousTimer::get().start();

					// ~system double-click window (~500ms); old 200ms was too tight.
					if (diff_ms > 10 && diff_ms < 500)
						UI::get().doubleclick();

					UI::get().send_cursor(false);
				}

				break;
			}
			case SDL_BUTTON_RIGHT:
			{
				if (event.down)
					UI::get().rightclick();

				break;
			}
		}
	}

	namespace
	{
		// 当前本地时间，格式 yyyy-MM-dd HH:mm:ss（标题里的【当前时间】段）。
		std::string format_local_time()
		{
			std::time_t rawtime = std::time(nullptr);
			std::tm local{};

#ifdef _WIN32
			localtime_s(&local, &rawtime);
#else
			if (std::tm* ptr = std::localtime(&rawtime))
				local = *ptr;
#endif

			char buffer[32] = {};
			std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local);

			return std::string(buffer);
		}

		// 在线时长，格式 dd HH:mm:ss（标题里的【在线时长】段）。
		std::string format_duration(int64_t seconds)
		{
			int64_t days = seconds / 86400;
			int64_t hours = (seconds / 3600) % 24;
			int64_t minutes = (seconds / 60) % 60;
			int64_t secs = seconds % 60;

			char buffer[32];
			std::snprintf(buffer, sizeof(buffer), "%02lld %02lld:%02lld:%02lld", days, hours, minutes, secs);

			return std::string(buffer);
		}
	}

	Window::Window()
	{
		glwnd = nullptr;
		glcontext = nullptr;
		close_requested = false;
		opacity = 1.0f;
		opcstep = 0.0f;
		width = Constants::Constants::get().get_viewwidth();
		height = Constants::Constants::get().get_viewheight();
	}

	Window::~Window()
	{
		if (glcontext)
			SDL_GL_DestroyContext(glcontext);

		if (glwnd)
			SDL_DestroyWindow(glwnd);

		SDL_Quit();
	}

	Error Window::init()
	{
		fullscreen = Setting<Fullscreen>::get().load();

		if (!SDL_Init(SDL_INIT_VIDEO))
			return Error::Code::GLFW;

		// 记录桌面尺寸：check_events() / toggle_fullscreen() 用它判断是否"超出可窗口化
		// 的尺寸"。此前 MAXWIDTH/MAXHEIGHT 从未赋值（恒为 0），任何改分辨率的分支都会被
		// 判成超出上限而强制全屏；把默认分辨率改成 1280x720 后必须先补上，
		// 否则进入商城（固定 1024x768）等场景会被强制切全屏。
		const SDL_DisplayMode* desktop = SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay());

		if (desktop && desktop->w > 0 && desktop->h > 0)
		{
			Configuration::get().set_max_width(static_cast<int16_t>(desktop->w));
			Configuration::get().set_max_height(static_cast<int16_t>(desktop->h));
		}

		// One window with one OpenGL context for the whole lifetime: GLEW needs a
		// current context before any GL call, and keeping the same context alive
		// means resolution and fullscreen changes never invalidate GL resources.
#ifdef USE_OPENGL_ES
		// Ask SDL for an OpenGL ES 3.0 context (the shaders are GLSL ES 1.00,
		// which a 3.0 context accepts for backward compatibility).
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
		// Desktop OpenGL: 配套 GLSL 1.20 着色器（attribute / varying / gl_FragColor）
		// 仅在兼容 profile 下可编译；core profile 会拒绝这些旧关键字，
		// 导致 GraphicsGL::init() 报 "Failed to compile vertex shader"。
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		glwnd = SDL_CreateWindow("", width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

		if (!glwnd)
			return Error::Code::GLFW;

		glcontext = SDL_GL_CreateContext(glwnd);

		if (!glcontext)
			return Error::Code::WINDOW;

		if (!SDL_GL_MakeCurrent(glwnd, glcontext))
			return Error::Code::WINDOW;

		if (Error error = GraphicsGL::get().init())
			return error;

		return initwindow();
	}

	Error Window::initwindow()
	{
		if (!glwnd)
			return Error::Code::WINDOW;

		SDL_SetWindowTitle(glwnd, Configuration::get().get_title().c_str());
		SDL_SetWindowSize(glwnd, width, height);
		SDL_SetWindowFullscreen(glwnd, fullscreen);
		SDL_ShowWindow(glwnd);

		if (!SDL_GL_MakeCurrent(glwnd, glcontext))
			return Error::Code::WINDOW;

		bool vsync = Setting<VSync>::get().load();
		SDL_GL_SetSwapInterval(vsync ? 1 : 0);

		glViewport(0, 0, width, height);

		SDL_HideCursor();

		float xpos = 0.0f;
		float ypos = 0.0f;

		SDL_GetMouseState(&xpos, &ypos);
		send_cursor_position(xpos, ypos);

		// Window icon, loaded from the executable's directory.
		const char* basepath = SDL_GetBasePath();

		std::string iconpath = basepath ? std::string(basepath) : std::string();
		SDL_free(const_cast<char*>(basepath));

		iconpath += "Icon.png";

		int32_t icon_width = 0;
		int32_t icon_height = 0;

		auto stbi = stbi_load(iconpath.c_str(), &icon_width, &icon_height, 0, 4);

		if (stbi == NULL)
		{
#ifdef __ANDROID__
			// APK assets are not on the filesystem at the base path, so the
			// window icon is simply skipped on Android (it is not fatal).
			LOG(LOG_WARN, "Icon not found, skipping on Android: " << iconpath);
#else
			return Error(Error::Code::MISSING_ICON, stbi_failure_reason());
#endif
		}
		else
		{
			SDL_Surface* icon = SDL_CreateSurfaceFrom(
				icon_width,
				icon_height,
				SDL_PIXELFORMAT_RGBA32,
				stbi,
				icon_width * 4
				);

			if (icon)
			{
				SDL_SetWindowIcon(glwnd, icon);
				SDL_DestroySurface(icon);
			}

			stbi_image_free(stbi);
		}

		GraphicsGL::get().reinit();

		return Error::Code::NONE;
	}

	bool Window::not_closed() const
	{
		return !close_requested;
	}

	void Window::update()
	{
		updateopc();
		update_title();
	}

	void Window::set_limit_break(int64_t value)
	{
		// 破功值（伤害上限）由服务端 LIMIT_BREAK_SYNC(0x3730) 下发，int64。
		limit_break = value;

		LOG(LOG_NETWORK, "破功值同步: " << limit_break);
	}

	void Window::update_title()
	{
		// 标题里的时间精确到秒，没必要每帧重建：500ms 刷新一次即可，
		// 顺带避免高频调用 SDL_SetWindowTitle。
		auto now = std::chrono::steady_clock::now();

		if (last_title_tick.time_since_epoch().count() != 0
			&& std::chrono::duration_cast<std::chrono::milliseconds>(now - last_title_tick).count() < 500)
			return;

		last_title_tick = now;

		std::string title = build_title();

		if (!glwnd || title == applied_title)
			return;

		SDL_SetWindowTitle(glwnd, title.c_str());
		applied_title = title;
	}

	std::string Window::build_title()
	{
		// 基础标题：Configuration::TITLE（"XiaoYE MS v0.0.1"）。
		std::string title = Configuration::get().get_title();

		const Player& player = Stage::get().get_player();
		const CharStats& stats = player.get_stats();
		int32_t cid = player.get_oid();

		// 在线时长从"角色上线"开始计时：换角色或退回登录界面（无角色）时重置。
		if (cid > 0)
		{
			if (cid != title_cid)
			{
				title_cid = cid;
				online_since = std::chrono::steady_clock::now();
				online_timing = true;
			}
		}
		else
		{
			title_cid = 0;
			online_timing = false;
		}

		// 【角色id-角色名】
		if (cid > 0)
		{
			title += "【";
			title += std::to_string(cid);
			title += "-";
			title += stats.get_name();
			title += "】";
		}

		// 【职业id-职业中文名】：中文名取 String/Skill.img/<职业id>/bookName
		// （与技能册一致，随数据语言），取不到时退回 Job 的英文名。
		uint16_t jobid = stats.get_job().get_id();

		if (jobid > 0)
		{
			std::string jobname = JobData::get(jobid).get_name();

			if (jobname.empty())
				jobname = stats.get_job().get_name();

			title += "【";
			title += std::to_string(jobid);
			title += "-";
			title += jobname;
			title += "】";
		}

		// 【破功值：x】：服务端未同步前（limit_break < 0）不显示该段。
		if (limit_break >= 0)
		{
			title += "【破功值：";
			title += std::to_string(limit_break);
			title += "】";
		}

		// 【当前时间：yyyy-MM-dd HH:mm:ss】
		title += "【当前时间：";
		title += format_local_time();
		title += "】";

		// 【在线时长：dd HH:mm:ss】
		if (online_timing)
		{
			int64_t elapsed = std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::steady_clock::now() - online_since
				).count();

			title += "【在线时长：";
			title += format_duration(elapsed);
			title += "】";
		}

		return title;
	}

	void Window::updateopc()
	{
		if (opcstep != 0.0f)
		{
			opacity += opcstep;

			if (opacity >= 1.0f)
			{
				opacity = 1.0f;
				opcstep = 0.0f;
			}
			else if (opacity <= 0.0f)
			{
				opacity = 0.0f;
				opcstep = -opcstep;

				try
				{
					fadeprocedure();
				}
				catch (...)
				{
					// 装图异常也必须揭开黑罩，否则永久黑屏+BGM。
					opacity = 1.0f;
					opcstep = 0.0f;
					throw;
				}

				opacity = 1.0f;
				opcstep = 0.0f;
			}
		}
	}

	void Window::poll_events()
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_EVENT_QUIT:
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					// Same behaviour as the old close callback: hand the request
					// to the UI (which opens the quit confirmation) and keep
					// running instead of tearing the window down.
					UI::get().send_close();
					break;
				case SDL_EVENT_KEY_DOWN:
				case SDL_EVENT_KEY_UP:
				{
					int32_t keycode = glfw_key_from_scancode(event.key.scancode);

					if (event.type == SDL_EVENT_KEY_DOWN)
						input_events++;

					if (keycode != 0)
						UI::get().send_key(keycode, event.type == SDL_EVENT_KEY_DOWN);

					break;
				}
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
				case SDL_EVENT_MOUSE_BUTTON_UP:
					if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
						input_events++;

					send_mouse_button(event.button);
					break;
				case SDL_EVENT_MOUSE_MOTION:
					send_cursor_position(event.motion.x, event.motion.y);
					break;
				case SDL_EVENT_MOUSE_WHEEL:
					UI::get().send_scroll(event.wheel.y);
					break;
#ifdef __ANDROID__
				case SDL_EVENT_FINGER_DOWN:
				{
					input_events++;

					// Finger coordinates are normalised [0,1]; map to window pixels.
					float fx = event.tfinger.x * static_cast<float>(width);
					float fy = event.tfinger.y * static_cast<float>(height);
					send_cursor_position(fx, fy);
					UI::get().send_cursor(true);
					break;
				}
				case SDL_EVENT_FINGER_MOTION:
				{
					float fx = event.tfinger.x * static_cast<float>(width);
					float fy = event.tfinger.y * static_cast<float>(height);
					send_cursor_position(fx, fy);
					break;
				}
				case SDL_EVENT_FINGER_UP:
				{
					float fx = event.tfinger.x * static_cast<float>(width);
					float fy = event.tfinger.y * static_cast<float>(height);
					send_cursor_position(fx, fy);
					UI::get().send_cursor(false);
					break;
				}
#endif
				case SDL_EVENT_TEXT_INPUT:
				{
					// Desktop + Android: composed text from IME / soft keyboard.
					// Requires start_text_input() while a Textfield is focused.
					if (event.text.text && event.text.text[0])
						UI::get().send_text(event.text.text);
					break;
				}
				case SDL_EVENT_WINDOW_FOCUS_GAINED:
				case SDL_EVENT_WINDOW_FOCUS_LOST:
					// Defer to sync_audio_focus(): event order at startup is unreliable
					// (LOST without a later GAINED leaves BGM permanently muted).
					break;
				default:
					break;
			}
		}

		sync_audio_focus();
	}

	void Window::sync_audio_focus()
	{
		if (!glwnd)
			return;

		const bool focused = (SDL_GetWindowFlags(glwnd) & SDL_WINDOW_INPUT_FOCUS) != 0;
		static int last_focused = -1;

		if (static_cast<int>(focused) == last_focused)
			return;

		last_focused = focused ? 1 : 0;
		UI::get().send_focus(last_focused);
	}

	void Window::begin_ascii_text_input()
	{
		if (!glwnd)
			return;

#ifdef __ANDROID__
		// Soft keyboard / composed text on mobile.
		if (!SDL_TextInputActive(glwnd))
			SDL_StartTextInput(glwnd);
#elif defined(_WIN32)
		// Account/password are ASCII-only. Leaving the system IME attached makes
		// Chinese input mode swallow KEY_DOWN letters and commit UTF-8 that our
		// FreeType atlas cannot draw (looks like mojibake / "invisible" text).
		if (!ime_suppressed)
		{
			SDL_PropertiesID props = SDL_GetWindowProperties(glwnd);
			HWND hwnd = static_cast<HWND>(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));

			if (hwnd)
			{
				ime_context = ImmAssociateContext(hwnd, nullptr);
				ime_suppressed = true;
			}
		}

		// Do not SDL_StartTextInput on desktop: that re-enables IME and blocks
		// the KEY_DOWN → Keyboard ASCII path HeavenClient relies on.
		if (SDL_TextInputActive(glwnd))
			SDL_StopTextInput(glwnd);
#else
		if (SDL_TextInputActive(glwnd))
			SDL_StopTextInput(glwnd);
#endif
	}

	void Window::end_ascii_text_input()
	{
		if (!glwnd)
			return;

#ifdef __ANDROID__
		if (SDL_TextInputActive(glwnd))
			SDL_StopTextInput(glwnd);
#elif defined(_WIN32)
		if (ime_suppressed)
		{
			SDL_PropertiesID props = SDL_GetWindowProperties(glwnd);
			HWND hwnd = static_cast<HWND>(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));

			if (hwnd)
				ImmAssociateContext(hwnd, static_cast<HIMC>(ime_context));

			ime_context = nullptr;
			ime_suppressed = false;
		}
#endif
	}

	bool Window::text_input_active() const
	{
		return glwnd && SDL_TextInputActive(glwnd);
	}

	void Window::check_events()
	{
		int16_t max_width = Configuration::get().get_max_width();
		int16_t max_height = Configuration::get().get_max_height();
		int16_t new_width = Constants::Constants::get().get_viewwidth();
		int16_t new_height = Constants::Constants::get().get_viewheight();

		if (width != new_width || height != new_height)
		{
			width = new_width;
			height = new_height;

			if (new_width >= max_width || new_height >= max_height)
				fullscreen = true;

			initwindow();
		}

		poll_events();
	}

	void Window::begin() const
	{
		GraphicsGL::get().clearscene();
	}

	void Window::end() const
	{
		GraphicsGL::get().flush(opacity);

		SDL_GL_SwapWindow(glwnd);
	}

	void Window::fadeout(float step, std::function<void()> fadeproc)
	{
		opcstep = -step;
		fadeprocedure = fadeproc;
	}

	void Window::setclipboard(const std::string& text) const
	{
		SDL_SetClipboardText(text.c_str());
	}

	std::string Window::getclipboard() const
	{
		const char* text = SDL_GetClipboardText();

		if (!text)
			return "";

		std::string result = text;
		SDL_free(const_cast<char*>(text));

		return result;
	}

	void Window::toggle_fullscreen()
	{
		int16_t max_width = Configuration::get().get_max_width();
		int16_t max_height = Configuration::get().get_max_height();

		if (width < max_width && height < max_height)
		{
			fullscreen = !fullscreen;
			Setting<Fullscreen>::get().save(fullscreen);

			initwindow();
			poll_events();
		}
	}
}
