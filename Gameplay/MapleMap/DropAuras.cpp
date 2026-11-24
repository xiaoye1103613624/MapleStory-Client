//////////////////////////////////////////////////////////////////////////////////
//	DropAuras — 装备掉落品质光环素材（2026-11-12 装备槽位扩展 ⑥）
//////////////////////////////////////////////////////////////////////////////////
#include "DropAuras.h"

#include "../../MapleStory.h"

#include "../../Util/Misc.h"

#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// 参考实现 dropitemaura.cpp:24 的档位名表（g_gradeNames[4]），逐字对齐素材节点名。
		constexpr const char* GRADE_NAMES[4] = { "Rare", "Epic", "Unique", "Legendary" };

		// 独立 .img 的根名（素材入库位置见 DropAuras.h 头注释）。
		constexpr const char* AURA_IMG = "DropAuraEff.img";
	}

	DropAuras::DropAuras() {}

	DropAuras& DropAuras::get()
	{
		static DropAuras auras;
		return auras;
	}

	void DropAuras::init()
	{
		if (loaded)
			return;

		loaded = true;

#ifdef USE_NX
		nl::node root = nl::nx::Effect[AURA_IMG];

		if (!root)
		{
			// 素材缺失 = 优雅降级：所有 has_* 返回 false，掉落物照常显示，只是没有光环。
			LOG(LOG_DEBUG, "DropAuras: Effect/" << AURA_IMG << " missing -> no drop aura");
			return;
		}

		for (int8_t g = 0; g < GRADE_MAX; ++g)
		{
			nl::node grade = root["dropItemAura"][GRADE_NAMES[g]];

			if (!grade)
			{
				LOG(LOG_DEBUG, "DropAuras: grade slot missing: " << GRADE_NAMES[g]);
				continue;
			}

			nl::node f = grade["front"];
			nl::node b = grade["back"];

			if (f)
			{
				fronts[g + 1] = Animation(f);
				front_ok[g + 1] = true;
			}
			if (b)
			{
				backs[g + 1] = Animation(b);
				back_ok[g + 1] = true;
			}
		}

		nl::node e = root["dropItemEffect"];

		if (e)
		{
			fall = Animation(e);
			fall_ok = true;
		}

		LOG(LOG_DEBUG, "DropAuras: pillar " << (fall_ok ? "ok" : "missing"));
#endif
	}

	bool DropAuras::has_grade(int8_t grade) const
	{
		if (grade < GRADE_RARE || grade > GRADE_MAX)
			return false;

		return front_ok[grade];
	}

	bool DropAuras::has_back(int8_t grade) const
	{
		if (grade < GRADE_RARE || grade > GRADE_MAX)
			return false;

		return back_ok[grade];
	}

	bool DropAuras::has_pillar() const
	{
		return fall_ok;
	}

	const Animation& DropAuras::front(int8_t grade) const
	{
		static const Animation invalid;

		if (grade < GRADE_RARE || grade > GRADE_MAX)
			return invalid;

		return fronts[grade];
	}

	const Animation& DropAuras::back(int8_t grade) const
	{
		static const Animation invalid;

		if (grade < GRADE_RARE || grade > GRADE_MAX)
			return invalid;

		return backs[grade];
	}

	const Animation& DropAuras::pillar() const
	{
		return fall;
	}
}
