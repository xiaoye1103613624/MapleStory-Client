//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — Summon map object (FieldMarker specialization)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "FieldMarker.h"

namespace ms
{
	class MapSummon : public FieldMarker
	{
	public:
		MapSummon(int32_t oid, int32_t owner_cid, int32_t skill_id, Point<int16_t> pos,
			int8_t skill_level = 0, int32_t lifetime_ms = 0);

		int32_t get_owner() const;
		int32_t get_skill() const;
		int8_t get_skill_level() const;
		void set_stance(int8_t stance);

	private:
		int32_t owner_cid = 0;
		int32_t skill_id = 0;
		int8_t skill_level = 0;
		int8_t stance = 0;
	};
}
