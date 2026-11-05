//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — Mist map object（Skill tile 矩形平铺）
//////////////////////////////////////////////////////////////////////////////////
#include "MapMist.h"

#include "../../Util/Misc.h"
#include "../../Util/Randomizer.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	MapMist::MapMist(int32_t oid, int32_t owner, int32_t skill, int32_t type,
		int8_t level, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
		int32_t lifetime_ms)
		// skill_id=0：禁止基类 try_load_anim 误吃 skill/effect 单点闪；名牌留空对齐原生无头顶字。
		: FieldMarker(oid, Kind::MIST, Point<int16_t>(
			static_cast<int16_t>(std::clamp(x1, -32768, 32767)),
			static_cast<int16_t>(std::clamp(y1, -32768, 32767))),
			std::string(), 0, lifetime_ms),
		  owner_cid(owner),
		  skill_id(skill),
		  mist_type(type),
		  skill_level(level),
		  box_x1(x1),
		  box_y1(y1),
		  box_x2(x2),
		  box_y2(y2)
	{
		if (box_x2 < box_x1)
			std::swap(box_x1, box_x2);
		if (box_y2 < box_y1)
			std::swap(box_y1, box_y2);

		load_tile_variants();
		if (has_tiles)
			build_instances();

		LOG(LOG_DEBUG, "MapMist: oid=" << oid << " skill=" << skill_id
			<< " type=" << mist_type << " lv=" << static_cast<int>(skill_level)
			<< " box=(" << box_x1 << "," << box_y1 << ")-(" << box_x2 << "," << box_y2
			<< ") tiles=" << variants.size() << " instances=" << instances.size());
	}

	int32_t MapMist::get_owner() const
	{
		return owner_cid;
	}

	int32_t MapMist::get_skill() const
	{
		return skill_id;
	}

	int8_t MapMist::get_skill_level() const
	{
		return skill_level;
	}

	int32_t MapMist::get_mist_type() const
	{
		return mist_type;
	}

	bool MapMist::use_smoke_layout() const
	{
		// s9 type=2 烟幕；IDA `sub_432A1E` 专接 4221006（及同族烟幕）单点 tile/0。
		return mist_type == 2 || skill_id == 4221006;
	}

	void MapMist::load_tile_variants()
	{
#ifdef USE_NX
		nl::node tile;

		if (mist_type == 0)
		{
			// Mob mist：Skill/MobSkill.img/%03d/level/%d/tile（IDA 串 @0xAFE1F4）
			std::string sid = string_format::extend_id(skill_id, 3);
			int8_t lv = skill_level > 0 ? skill_level : 1;
			tile = nl::nx::Skill["MobSkill.img"][sid]["level"][std::to_string(lv)]["tile"];
			if (!tile && lv != 1)
				tile = nl::nx::Skill["MobSkill.img"][sid]["level"]["1"]["tile"];
		}
		else
		{
			// 玩家雾：Skill/<ns>.img/skill/<id>/tile（2111003 / 4221006 等）
			std::string strid = skill_id < 10000000
				? string_format::extend_id(skill_id, 7)
				: std::to_string(skill_id);
			tile = nl::nx::Skill[string_format::skill_namespace(skill_id) + ".img"]
				["skill"][strid]["tile"];
		}

		if (!tile)
		{
			LOG(LOG_DEBUG, "MapMist: no tile node skill=" << skill_id
				<< " type=" << mist_type << " lv=" << static_cast<int>(skill_level));
			return;
		}

		std::vector<int> ids;
		for (auto c : tile)
		{
			if (c.data_type() != nl::node::type::none)
				continue;
			char* end = nullptr;
			long id = std::strtol(c.name().c_str(), &end, 10);
			if (end && *end == '\0' && id >= 0)
				ids.push_back(static_cast<int>(id));
		}
		std::sort(ids.begin(), ids.end());

		for (int id : ids)
		{
			nl::node v = tile[std::to_string(id)];
			if (!v)
				continue;
			Animation anim(v);
			if (anim.get_dimensions().x() <= 0 && anim.get_dimensions().y() <= 0)
				continue;
			variants.push_back(anim);
		}

		has_tiles = !variants.empty();
#else
		(void)skill_id;
		(void)mist_type;
		(void)skill_level;
#endif
	}

	void MapMist::build_instances()
	{
		instances.clear();
		if (!has_tiles)
			return;

		if (use_smoke_layout())
			build_smoke_single();
		else
			build_scatter();
	}

	void MapMist::build_smoke_single()
	{
		// IDA sub_432A1E：中点 x、底边 y，只放 tile/0。
		int32_t mid_x = (box_x1 + box_x2) / 2;
		instances.push_back({
			0,
			Point<int16_t>(
				static_cast<int16_t>(std::clamp(mid_x, -32768, 32767)),
				static_cast<int16_t>(std::clamp(box_y2, -32768, 32767)))
		});
	}

	void MapMist::build_scatter()
	{
		// IDA sub_432776：水平扫描；纵向上按距中心距离收缩（椭圆带）；
		// 每格随机选 tile 变体；x 步进 35+rand(20)；y 步进 spriteH*0.8。
		if (box_x2 <= box_x1 || box_y2 <= box_y1)
			return;

		const double width = static_cast<double>(box_x2 - box_x1);
		const double height = static_cast<double>(box_y2 - box_y1);
		const int32_t mid_x = (box_x1 + box_x2) / 2;

		Randomizer rng;
		int32_t x = box_x1;
		const size_t nvar = variants.size();
		constexpr int kMaxInstances = 256;

		while (x < box_x2 && static_cast<int>(instances.size()) < kMaxInstances)
		{
			const double taper = height * (std::abs(static_cast<double>(x - mid_x)) / width * 0.5);
			double y = static_cast<double>(box_y1) + taper;
			const double y_limit = static_cast<double>(box_y2) - taper;

			while (y < y_limit && static_cast<int>(instances.size()) < kMaxInstances)
			{
				size_t vi = nvar > 1 ? static_cast<size_t>(rng.next_int(static_cast<int>(nvar))) : 0;
				instances.push_back({
					vi,
					Point<int16_t>(
						static_cast<int16_t>(std::clamp(x, -32768, 32767)),
						static_cast<int16_t>(std::clamp(static_cast<int32_t>(std::lround(y)), -32768, 32767)))
				});

				Point<int16_t> dim = variants[vi].get_dimensions();
				int16_t h = dim.y() > 0 ? dim.y() : 40;
				y += static_cast<double>(h) * 0.8;
			}

			x += rng.next_int(20) + 35;
		}
	}

	void MapMist::draw(double viewx, double viewy, float alpha) const
	{
		if (!active)
			return;

		if (!has_tiles || instances.empty())
		{
			// 无 tile 素材时保留基类占位（调试可见），避免静默消失。
			FieldMarker::draw(viewx, viewy, alpha);
			return;
		}

		for (const Instance& inst : instances)
		{
			if (inst.variant >= variants.size())
				continue;

			Point<int16_t> absp(
				static_cast<int16_t>(std::lround(static_cast<double>(inst.pos.x()) + viewx)),
				static_cast<int16_t>(std::lround(static_cast<double>(inst.pos.y()) + viewy)));
			variants[inst.variant].draw(absp, alpha);
		}
	}

	int8_t MapMist::update(const Physics& physics)
	{
		int8_t layer = FieldMarker::update(physics);

		if (layer == -1)
			return layer;

		for (Animation& anim : variants)
			anim.update();

		return layer;
	}
}
