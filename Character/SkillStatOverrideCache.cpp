//////////////////////////////////////////////////////////////////////////////////
#include "SkillStatOverrideCache.h"

namespace ms
{
	SkillStatOverrideCache& SkillStatOverrideCache::get()
	{
		static SkillStatOverrideCache instance;
		return instance;
	}

	void SkillStatOverrideCache::clear()
	{
		skills.clear();
	}

	void SkillStatOverrideCache::touch(int32_t skill_id)
	{
		if (skill_id > 0)
			skills.insert(skill_id);
	}

	const std::unordered_set<int32_t>& SkillStatOverrideCache::touched() const
	{
		return skills;
	}
}
