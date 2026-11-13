//////////////////////////////////////////////////////////////////////////////////
//	服务端 SET_ITEM_FINAL_DAMAGE(0x175) 战斗面板缓存（仅显示，不改伤害公式）
//	字段序对齐 BeiDou PacketCreator.setItemFinalDamageBonus / ijl15 SetItemBridge
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>

namespace ms
{
	struct CombatPanelStats
	{
		bool synced = false;

		int32_t final_damage_percent = 0;
		int32_t damage_skin_id = 0;
		int32_t dam_r = 0;
		int32_t boss_dam_r = 0;
		int32_t normal_dam_r = 0;
		int32_t ignore_pdr = 0;
		int32_t ignore_mdr = 0;
		int32_t crit_rate = 0;
		int32_t crit_dam = 0;
		int32_t pad_r = 0;
		int32_t mad_r = 0;
		int32_t item_drop_prop = 0;
		int32_t meso_drop_prop = 0;
		int32_t damage_reduce = 0;
		int32_t asr_r = 0;
		int32_t buff_time_r = 0;
		int32_t stance_prop = 0;

		static CombatPanelStats& get();
		void clear();
		/// 刷新已打开的属性窗数值（若有）
		void notify_ui() const;
	};
}
