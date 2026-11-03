//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — collection for field placeholders
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "FieldMarker.h"
#include "MapObjects.h"

#include <cstdint>
#include <string>

namespace ms
{
	class MapFieldMarkers
	{
	public:
		void draw(Layer::Id layer, double viewx, double viewy, float alpha) const;
		void update(const Physics& physics);

		void spawn(int32_t oid, FieldMarker::Kind kind, Point<int16_t> pos, const std::string& label, int32_t skill_id = 0, int32_t lifetime_ms = 0);
		void spawn_summon(int32_t oid, int32_t owner_cid, int32_t skill_id, Point<int16_t> pos,
			int8_t skill_level = 0, int32_t lifetime_ms = 0);
		void spawn_mist(int32_t oid, int32_t owner_cid, int32_t skill_id, Point<int16_t> pos,
			int32_t lifetime_ms = 60000);
		void set_summon_stance(int32_t oid, int8_t stance);
		// 真对象化（波3）：召唤兽入场/死亡动画需要拿到 MapSummon 实例，
		// 只经 FieldMarker 基类接口够不到 play_die/play_spawn。
		void summon_die(int32_t oid);
		void summon_spawn(int32_t oid);
		void move(int32_t oid, Point<int16_t> pos);
		void set_label(int32_t oid, const std::string& label);
		bool contains(int32_t oid) const;
		void apply_damage(int32_t oid, int32_t damage);
		void remove(int32_t oid);
		void clear();

	private:
		MapObjects markers;
	};
}
