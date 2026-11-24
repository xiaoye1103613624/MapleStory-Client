//////////////////////////////////////////////////////////////////////////////////
#include "SkillBonusCache.h"

namespace ms
{
	SkillBonusCache& SkillBonusCache::get()
	{
		static SkillBonusCache instance;
		return instance;
	}

	void SkillBonusCache::clear()
	{
		levels.clear();
	}

	void SkillBonusCache::set(int32_t skill_id, int32_t add_level)
	{
		if (skill_id <= 0)
			return;

		if (add_level == 0)
			levels.erase(skill_id);
		else
			levels[skill_id] = add_level;
	}

	void SkillBonusCache::replace_all(const std::unordered_map<int32_t, int32_t>& next)
	{
		levels = next;
	}

	int32_t SkillBonusCache::get_add_level(int32_t skill_id) const
	{
		auto it = levels.find(skill_id);
		return it == levels.end() ? 0 : it->second;
	}
}
