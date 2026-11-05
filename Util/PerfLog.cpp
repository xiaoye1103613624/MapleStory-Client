////////////////////////////////////////////////////////////////////////////////
//	PerfLog 实现 —— 见 PerfLog.h 头注释。
////////////////////////////////////////////////////////////////////////////////
#include "PerfLog.h"

#include "../Configuration.h"

#include <chrono>
#include <cstdio>
#include <fstream>
#include <mutex>

namespace ms
{
	namespace perf
	{
		namespace
		{
			// 进程起点：所有相对毫秒以此为零点，单调、不受系统时钟调整影响。
			const auto t0 = std::chrono::steady_clock::now();

			std::mutex logmtx;
			std::ofstream logfile;
			int lines = 0;

			// 开关缓存：配置每帧可热重载（reload_if_changed），这里按 2s 粒度
			// 重取，避免每次 mark 走 Setting::load 的字符串路径。
			struct flag_state
			{
				bool value = false;
				std::chrono::steady_clock::time_point next_check{};
			};

			bool enabled_cached()
			{
				static flag_state st;
				auto now = std::chrono::steady_clock::now();

				if (now >= st.next_check)
				{
					st.value = Setting<PerfLogEnabled>::get().load();
					st.next_check = now + std::chrono::seconds(2);
				}

				return st.value;
			}
		}

		void mark(const char* tag, const std::string& msg)
		{
			if (!enabled_cached())
				return;

			std::lock_guard<std::mutex> lock(logmtx);

			if (lines >= 5000)
				return;

			if (!logfile.is_open())
				logfile.open("boot_perf.log", std::ios::app);

			if (!logfile.is_open())
				return;

			const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - t0).count();

			char head[64];
			std::snprintf(head, sizeof(head), "[PERF] t=%lldms ", (long long)ms);

			logfile << head << tag << ' ' << msg << '\n';
			logfile.flush();
			++lines;
		}
	}
}
