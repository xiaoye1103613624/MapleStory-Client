#include "CombatPanelStats.h"

#include "../IO/UI.h"
#include "../IO/UITypes/UIStatsInfo.h"

namespace ms
{
	CombatPanelStats& CombatPanelStats::get()
	{
		static CombatPanelStats instance;
		return instance;
	}

	void CombatPanelStats::clear()
	{
		*this = CombatPanelStats{};
	}

	void CombatPanelStats::notify_ui() const
	{
		if (auto st = UI::get().get_element<UIStatsInfo>())
			st->update_all_stats();
	}
}
