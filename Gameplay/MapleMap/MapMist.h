//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — Mist map object (FieldMarker specialization)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "FieldMarker.h"

namespace ms
{
	class MapMist : public FieldMarker
	{
	public:
		MapMist(int32_t oid, int32_t owner_cid, int32_t skill_id, Point<int16_t> pos,
			int32_t lifetime_ms = 60000);

		int32_t get_owner() const;
		int32_t get_skill() const;

	private:
		int32_t owner_cid = 0;
		int32_t skill_id = 0;
	};
}
