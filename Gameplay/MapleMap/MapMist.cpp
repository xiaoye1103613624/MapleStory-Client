//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — Mist map object
//////////////////////////////////////////////////////////////////////////////////
#include "MapMist.h"

#include <string>

namespace ms
{
	MapMist::MapMist(int32_t oid, int32_t owner, int32_t skill, Point<int16_t> pos,
		int32_t lifetime_ms)
		: FieldMarker(oid, Kind::MIST, pos, "Mist " + std::to_string(skill), skill, lifetime_ms),
		  owner_cid(owner),
		  skill_id(skill)
	{
	}

	int32_t MapMist::get_owner() const
	{
		return owner_cid;
	}

	int32_t MapMist::get_skill() const
	{
		return skill_id;
	}
}
