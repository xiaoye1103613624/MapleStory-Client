//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — Summon map object
//////////////////////////////////////////////////////////////////////////////////
#include "MapSummon.h"

#include <string>

namespace ms
{
	MapSummon::MapSummon(int32_t oid, int32_t owner, int32_t skill, Point<int16_t> pos,
		int8_t level, int32_t lifetime_ms)
		: FieldMarker(oid, Kind::SUMMON, pos,
			"S" + std::to_string(skill) + (level > 0 ? (" Lv" + std::to_string(level)) : ""),
			skill, lifetime_ms),
		  owner_cid(owner),
		  skill_id(skill),
		  skill_level(level)
	{
	}

	int32_t MapSummon::get_owner() const
	{
		return owner_cid;
	}

	int32_t MapSummon::get_skill() const
	{
		return skill_id;
	}

	int8_t MapSummon::get_skill_level() const
	{
		return skill_level;
	}

	void MapSummon::set_stance(int8_t s)
	{
		stance = s;
		set_label("S" + std::to_string(skill_id)
			+ (skill_level > 0 ? (" Lv" + std::to_string(skill_level)) : "")
			+ " st" + std::to_string(stance));
	}
}
