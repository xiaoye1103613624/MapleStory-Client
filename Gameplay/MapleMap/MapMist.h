//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — Mist map object（Skill tile 矩形平铺）
//
//	证据：
//	- IDA SPAWN_MIST `sub_431A63` → 平铺 `sub_432776` / 烟幕单点 `sub_432A1E`
//	- 资源：`Skill/<ns>.img/skill/<id>/tile` 或 `Skill/MobSkill.img/%03d/level/%d/tile`
//	- 包字段：s9 spawnMist（oid/type/owner/skill/level/delay/box4×int）
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "FieldMarker.h"

#include "../../Graphics/Animation.h"

#include <cstdint>
#include <vector>

namespace ms
{
	class MapMist : public FieldMarker
	{
	public:
		MapMist(int32_t oid, int32_t owner_cid, int32_t skill_id, int32_t mist_type,
			int8_t skill_level, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
			int32_t lifetime_ms = 0);

		int32_t get_owner() const;
		int32_t get_skill() const;
		int8_t get_skill_level() const;
		int32_t get_mist_type() const;

		void draw(double viewx, double viewy, float alpha) const override;
		int8_t update(const Physics& physics) override;

	private:
		struct Instance
		{
			size_t variant = 0;
			Point<int16_t> pos;
		};

		void load_tile_variants();
		void build_instances();
		void build_scatter();
		void build_smoke_single();
		bool use_smoke_layout() const;

		int32_t owner_cid = 0;
		int32_t skill_id = 0;
		int32_t mist_type = 0;
		int8_t skill_level = 0;
		int32_t box_x1 = 0;
		int32_t box_y1 = 0;
		int32_t box_x2 = 0;
		int32_t box_y2 = 0;

		std::vector<Animation> variants;
		std::vector<Instance> instances;
		bool has_tiles = false;
	};
}
