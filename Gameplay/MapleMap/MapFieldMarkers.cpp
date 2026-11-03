//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — MapFieldMarkers
//////////////////////////////////////////////////////////////////////////////////
#include "MapFieldMarkers.h"
#include "MapSummon.h"
#include "MapMist.h"

#include <memory>

namespace ms
{
	void MapFieldMarkers::draw(Layer::Id layer, double viewx, double viewy, float alpha) const
	{
		markers.draw(layer, viewx, viewy, alpha);
	}

	void MapFieldMarkers::update(const Physics& physics)
	{
		markers.update(physics);
	}

	void MapFieldMarkers::spawn(int32_t oid, FieldMarker::Kind kind, Point<int16_t> pos, const std::string& label, int32_t skill_id, int32_t lifetime_ms)
	{
		if (auto existing = markers.get(oid))
		{
			// Keep world position on refresh; only update label (merchant desc etc.)
			if (auto* fm = dynamic_cast<FieldMarker*>(existing.get()))
				fm->set_label(label);
			existing->makeactive();
			(void)pos;
			(void)kind;
			(void)skill_id;
			(void)lifetime_ms;
			return;
		}

		markers.add(std::make_unique<FieldMarker>(oid, kind, pos, label, skill_id, lifetime_ms));
	}

	void MapFieldMarkers::spawn_summon(int32_t oid, int32_t owner_cid, int32_t skill_id, Point<int16_t> pos,
		int8_t skill_level, int32_t lifetime_ms)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* sum = dynamic_cast<MapSummon*>(existing.get()))
			{
				sum->set_label("S" + std::to_string(skill_id)
					+ (skill_level > 0 ? (" Lv" + std::to_string(skill_level)) : ""));
				existing->set_position(pos);
			}
			else if (auto* fm = dynamic_cast<FieldMarker*>(existing.get()))
			{
				fm->set_label("S" + std::to_string(skill_id));
			}
			existing->makeactive();
			return;
		}
		markers.add(std::make_unique<MapSummon>(oid, owner_cid, skill_id, pos, skill_level, lifetime_ms));
	}

	void MapFieldMarkers::spawn_mist(int32_t oid, int32_t owner_cid, int32_t skill_id, Point<int16_t> pos,
		int32_t lifetime_ms)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* fm = dynamic_cast<FieldMarker*>(existing.get()))
				fm->set_label("Mist " + std::to_string(skill_id));
			existing->set_position(pos);
			existing->makeactive();
			(void)owner_cid;
			(void)lifetime_ms;
			return;
		}
		markers.add(std::make_unique<MapMist>(oid, owner_cid, skill_id, pos, lifetime_ms));
	}

	void MapFieldMarkers::set_summon_stance(int32_t oid, int8_t stance)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* sum = dynamic_cast<MapSummon*>(existing.get()))
				sum->set_stance(stance);
		}
	}

		void MapFieldMarkers::summon_die(int32_t oid)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* sum = dynamic_cast<MapSummon*>(existing.get()))
				sum->play_die();
			else
				markers.remove(oid);
		}
	}

	void MapFieldMarkers::summon_spawn(int32_t oid)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* sum = dynamic_cast<MapSummon*>(existing.get()))
				sum->play_spawn();
		}
	}

void MapFieldMarkers::move(int32_t oid, Point<int16_t> pos)
	{
		if (auto existing = markers.get(oid))
			existing->set_position(pos);
	}

	void MapFieldMarkers::set_label(int32_t oid, const std::string& label)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* fm = dynamic_cast<FieldMarker*>(existing.get()))
				fm->set_label(label);
		}
	}

	bool MapFieldMarkers::contains(int32_t oid) const
	{
		return markers.contains(oid);
	}

	void MapFieldMarkers::apply_damage(int32_t oid, int32_t damage)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* fm = dynamic_cast<FieldMarker*>(existing.get()))
				fm->apply_damage(damage);
		}
	}

	void MapFieldMarkers::remove(int32_t oid)
	{
		markers.remove(oid);
	}

	void MapFieldMarkers::clear()
	{
		markers.clear();
	}
}
