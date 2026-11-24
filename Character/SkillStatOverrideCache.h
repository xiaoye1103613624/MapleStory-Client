//////////////////////////////////////////////////////////////////////////////////
// 技改方案覆盖包（S→C SendOpcode.SKILL_STAT_OVERRIDE = 0x3745）登记缓存。
// 只记录「被覆盖过的技能 id」，真正的字段叠加值存在 SkillData 的 overlay_stats 中。
// 空包（清覆盖）时由 handler 遍历本表对相关 SkillData 调 clear_overlay()，
// 未登记的技能天然回退 Skill.nx（客户端兜底）。
//
// 字段下标与服务端 org.gms.service.skill.SkillFieldMeta.FIELDS 顺序严格一致：
//   0 damage  1 attackCount  2 mobCount  3 mpCon  4 hpCon
//   5 time(客户端无字段,忽略) 6 cooltime(无) 7 x(无) 8 y(无)
//   9 matk  10 watk(无)  11 fixdamage  12 bulletCount  13 bulletConsume
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <unordered_set>

namespace ms
{
	class SkillStatOverrideCache
	{
	public:
		static SkillStatOverrideCache& get();

		void clear();
		void touch(int32_t skill_id);
		const std::unordered_set<int32_t>& touched() const;

	private:
		std::unordered_set<int32_t> skills;
	};
}
