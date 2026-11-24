//////////////////////////////////////////////////////////////////////////////////
// BeautyState — 美容院造型槽客户端状态（服务端 0x174 DATA 下发）。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <array>
#include <cstdint>

namespace ms
{
	namespace BeautyState
	{
		struct Slot
		{
			int32_t id = 0;      // hair/face/skin id（按 tab）
			bool saved = false;  // 该槽已保存
		};

		void apply(int32_t unlocked,
			const std::array<int32_t, 6>& hair, const std::array<int32_t, 6>& face,
			const std::array<int32_t, 6>& skin,
			const std::array<bool, 6>& hairSaved, const std::array<bool, 6>& faceSaved,
			const std::array<bool, 6>& skinSaved);

		int32_t unlocked();
		/** tab: 0 Hair, 1 Face, 2 Skin */
		const Slot& slot(int32_t tab, int32_t index);
		void mark_opened();
		bool has_data();
	}
}
