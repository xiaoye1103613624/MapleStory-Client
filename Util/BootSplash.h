//////////////////////////////////////////////////////////////////////////////////
//	Boot splash shown while patch / resource / session init runs.
//	加载阶段：程序化 spinner（此时 WZ/IMG 还没装）；资源就绪后：
//	原生启动动画 = 经典客户端的 Logo.img/Nexon 片头（黑底居中逐帧）。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Graphics/Text.h"
#include "../Template/Point.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>

namespace ms
{
	class BootSplash
	{
	public:
		static BootSplash& get();

		// Draw one loading frame. Safe before and after ResourceService::init().
		void pump(const char* message);

		// 播放原生启动动画（Logo.img/Nexon 片头）。
		//
		// 返回 true 表示真的播过（调用方无需处理返回值，仅为便于日志/测试）。
		// 以下情况直接返回 false：boot.logo=false、资源缺失、同一进程已播过。
		// 播放期间逐帧泵事件，按任意键或点击鼠标立即结束（经典客户端行为）。
		bool play_logo();

		// Load Logo.img/Nexon once UI root is bound. No-op if already loaded / missing.
		bool try_load_nexon();

		// Drop logo textures before UI atlas pressure (optional).
		void clear();

	private:
		BootSplash();

		void draw_procedural(const char* message);
		// 画第 index 帧 Logo（黑底 + 居中），message 非空时在底部加状态字。
		void draw_logo(std::size_t index, const char* message);
		void advance_clock();

		// Logo.img/Nexon 的帧数（0 = 不可用）。
		std::size_t logo_frames;
		bool logo_attempted;
		bool logo_played;

		std::chrono::steady_clock::time_point last_tick;
		bool clock_started;
		uint16_t pending_ms;
		float spin_phase;
		std::string last_message;
	};
}
