//////////////////////////////////////////////////////////////////////////////
//	Boot phase timing helper — logs [BOOT] stage=Nms
//	2026-11-04（Wave 0.1 收尾）：除 LOG 外同步打 perf::mark —— Release 下
//	LOG 是 no-op（MapleStory.h _DEBUG 门控），boot_perf.log（debug.perf_log）
//	是启动分段耗时唯一落盘通道。开关关闭时 mark 一次 bool 读取即返回。
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../MapleStory.h"
#include "../PerfLog.h"

#include <chrono>
#include <string>

namespace ms
{
	namespace patch
	{
		class BootTimer
		{
		public:
			explicit BootTimer(const char* stage)
				: stage_(stage), start_(std::chrono::steady_clock::now())
			{
			}

			~BootTimer()
			{
				const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::steady_clock::now() - start_).count();
				LOG(LOG_INFO, "[BOOT] " << stage_ << "=" << ms << "ms");
				perf::mark("BOOT", std::string(stage_) + "=" + std::to_string(ms) + "ms");
			}

		private:
			const char* stage_;
			std::chrono::steady_clock::time_point start_;
		};
	}
}
