//////////////////////////////////////////////////////////////////////////////
//	Boot phase timing helper — logs [BOOT] stage=Nms
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../MapleStory.h"

#include <chrono>

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
			}

		private:
			const char* stage_;
			std::chrono::steady_clock::time_point start_;
		};
	}
}
