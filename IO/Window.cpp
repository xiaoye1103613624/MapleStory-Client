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

#include "../Configuration.h"
#include "../Timer.h"

// stb_image 的单例实现：仅本翻译单元定义，供下方 init() 加载窗口图标 Icon.png 使用。
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <string>

#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>

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

					if (diff_ms > 10 && diff_ms < 200)
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

		// One window with one OpenGL context for the whole lifetime: GLEW needs a
		// current context before any GL call, and keeping the same context alive
		// means resolution and fullscreen changes never invalidate GL resources.
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
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

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
			return Error(Error::Code::MISSING_ICON, stbi_failure_reason());

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

				fadeprocedure();
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

					if (keycode != 0)
						UI::get().send_key(keycode, event.type == SDL_EVENT_KEY_DOWN);

					break;
				}
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
				case SDL_EVENT_MOUSE_BUTTON_UP:
					send_mouse_button(event.button);
					break;
				case SDL_EVENT_MOUSE_MOTION:
					send_cursor_position(event.motion.x, event.motion.y);
					break;
				case SDL_EVENT_MOUSE_WHEEL:
					UI::get().send_scroll(event.wheel.y);
					break;
				case SDL_EVENT_WINDOW_FOCUS_GAINED:
					UI::get().send_focus(1);
					break;
				case SDL_EVENT_WINDOW_FOCUS_LOST:
					UI::get().send_focus(0);
					break;
				default:
					break;
			}
		}
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
