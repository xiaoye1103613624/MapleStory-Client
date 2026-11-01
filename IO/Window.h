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
#pragma once

#include "../Error.h"

#include "../Template/Singleton.h"
#include "KeyCodes.h"

#ifndef USE_OPENGL_ES
#define GLEW_STATIC
#include <glew.h>
#else
#include <GLES3/gl3.h>
#endif
#include <SDL3/SDL.h>

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>

namespace ms
{
	class Window : public Singleton<Window>
	{
	public:
		Window();
		~Window();

		Error init();
		Error initwindow();

		bool not_closed() const;
		void update();
		void begin() const;
		void end() const;
		void fadeout(float step, std::function<void()> fadeprocedure);
		void check_events();
		/// Pump SDL events without resize/fullscreen side effects.
		/// Used during heavy atlas uploads so the window stays responsive.
		void poll_events();

		void setclipboard(const std::string& text) const;
		std::string getclipboard() const;

		void toggle_fullscreen();

		/// 累计的"用户主动输入"事件数（按下任意键 / 鼠标左中右键按下 / 触屏按下）。
		/// 启动动画等 pre-UI 阶段用它判断"用户想跳过"，不依赖 UI 是否已初始化。
		uint32_t get_input_events() const { return input_events; }

		/// Desktop ASCII textfields: disable system IME so physical keys reach
		/// KEY_DOWN (Chinese IME would otherwise swallow letters / commit UTF-8).
		/// Android: SDL_StartTextInput for the soft keyboard.
		void begin_ascii_text_input();
		void end_ascii_text_input();
		bool text_input_active() const;

		/// SDL window pointer (for BASS HWND / focus queries). Valid after init().
		SDL_Window* get_handle() const { return glwnd; }

		/// Align BGM/SFX mute state with the real SDL_WINDOW_INPUT_FOCUS flag.
		/// SDL can emit FOCUS_LOST at startup and skip FOCUS_GAINED, leaving volume at 0.
		void sync_audio_focus();

		/// Store the 破功值 (damage cap) synced by the server.
		/// 来源：服务端 SendOpcode.LIMIT_BREAK_SYNC(0x3730)，负载为 int64；
		/// 负数表示从未同步过，此时标题里省略该段（与 ijl15 插件行为一致）。
		void set_limit_break(int64_t value);

	private:
		void updateopc();

		/// 用当前角色信息拼出窗口标题：
		/// "<基础标题>【角色id-角色名】【职业id-职业中文名】【破功值：x】【当前时间：…】【在线时长：…】"
		/// 登录界面（还没有角色）时只保留基础标题与当前时间。
		std::string build_title();
		/// 每帧调用，内部按 500ms 节流，只在标题变化时真正调用 SDL。
		void update_title();

		SDL_Window* glwnd;
		SDL_GLContext glcontext;
		bool fullscreen;
		bool close_requested;
		uint32_t input_events = 0;
		float opacity;
		float opcstep;
		std::function<void()> fadeprocedure;
		int16_t width;
		int16_t height;

#ifdef _WIN32
		/// Previous IME context while ASCII text entry disables ImmAssociateContext.
		void* ime_context = nullptr;
		bool ime_suppressed = false;
#endif

		// ---- 窗口标题相关状态 ----
		// 破功值：由 0x3730 同步，< 0 表示尚未同步。
		int64_t limit_break = -1;
		// 已经写到窗口上的标题，用来避免重复调用 SDL_SetWindowTitle。
		std::string applied_title;
		// 正在计时的角色 id；0 表示当前没有角色（登录界面）。
		int32_t title_cid = 0;
		// 在线时长是否在计时（角色上线后开始，换角色/回登录界面时停止）。
		bool online_timing = false;
		std::chrono::steady_clock::time_point online_since;
		// 上次重建标题的时刻，默认构造的 time_point 表示"还没刷新过"。
		std::chrono::steady_clock::time_point last_title_tick;
	};
}