//////////////////////////////////////////////////////////////////////////////////
// BeautyState.cpp — 见 .h。
//////////////////////////////////////////////////////////////////////////////////
#include "BeautyState.h"

namespace ms
{
	namespace BeautyState
	{
		namespace
		{
			int32_t g_unlocked = 0;
			std::array<Slot, 6> g_hair, g_face, g_skin;
			bool g_data = false;
		}

		void apply(int32_t unlocked,
			const std::array<int32_t, 6>& hair, const std::array<int32_t, 6>& face,
			const std::array<int32_t, 6>& skin,
			const std::array<bool, 6>& hairSaved, const std::array<bool, 6>& faceSaved,
			const std::array<bool, 6>& skinSaved)
		{
			g_unlocked = unlocked;
			for (int i = 0; i < 6; ++i)
			{
				g_hair[i] = { hair[i], hairSaved[i] };
				g_face[i] = { face[i], faceSaved[i] };
				g_skin[i] = { skin[i], skinSaved[i] };
			}
			g_data = true;
		}

		int32_t unlocked() { return g_unlocked; }
		const Slot& slot(int32_t tab, int32_t index)
		{
			if (tab == 0) return g_hair[index];
			if (tab == 1) return g_face[index];
			return g_skin[index];
		}
		void mark_opened() { /* OPEN 应答仅表示窗口可显示，无数据落地 */ }
		bool has_data() { return g_data; }
	}
}
