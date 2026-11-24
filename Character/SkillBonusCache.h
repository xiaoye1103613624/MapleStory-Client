//////////////////////////////////////////////////////////////////////////////////
//	服务端 SET_SKILL_BONUS(0x17A) 技能窗(+N) 缓存
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <unordered_map>

namespace ms
{
	class SkillBonusCache
	{
	public:
		static SkillBonusCache& get();

		void clear();
		void set(int32_t skill_id, int32_t add_level);
		void replace_all(const std::unordered_map<int32_t, int32_t>& levels);
		int32_t get_add_level(int32_t skill_id) const;

	private:
		std::unordered_map<int32_t, int32_t> levels;
	};
}
