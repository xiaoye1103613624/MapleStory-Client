//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — Summon map object (真对象化实现)
//////////////////////////////////////////////////////////////////////////////////
#include "MapSummon.h"

#include "../../Util/Misc.h"

#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	MapSummon::MapSummon(int32_t oid, int32_t owner, int32_t skill, Point<int16_t> pos,
		int8_t level, int32_t lifetime_ms)
		: FieldMarker(oid, Kind::SUMMON, pos,
			// 名牌不再拼 S<id> 技术串：真对象有外观后，名牌保持原生极简
			// （原生召唤兽无头顶名牌；保留空 label 以复用基类清除路径）。
			std::string(),
			skill, lifetime_ms),
		  owner_cid(owner),
		  skill_id(skill),
		  skill_level(level)
	{
#ifdef USE_NX
		// 取证结构：Skill/<ns>.img/skill/<id>/summon{stand,hit,die,summoned}
		// （_tools/summon_probe 2026-11-03）。基类占位实现读 summon["0"] 恒空 ⇒
		// 回退到 summoned["0"] 小碎帧；这里按状态机整体加载。
		std::string strid = skill_id < 10000000
			? string_format::extend_id(skill_id, 7)
			: std::to_string(skill_id);
		nl::node src = nl::nx::Skill[string_format::skill_namespace(skill_id) + ".img"]
			["skill"][strid]["summon"];

		if (!src)
			src = nl::nx::Skill[string_format::skill_namespace(skill_id) + ".img"]
				["skill"][strid]["summoned"];

		for (auto c : src)
		{
			if (c.data_type() != nl::node::type::none)
				continue;

			std::string name = c.name();

			if (name == "info")
				continue;

			animations[name] = Animation(c);
		}
#endif

		// 没有 stand 帧 ⇒ 退化为旧占位（基类 ItemUp 等）。
		if (animations.count("stand") == 0)
			return;

		state = "stand";

		if (animations.count("summoned") > 0)
		{
			// spawnSummon animated ⇒ 入场闪光先播（update 末尾自动回 stand）。
			set_state("summoned");
		}
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

	void MapSummon::set_state(const std::string& next)
	{
		auto iter = animations.find(next);

		if (iter == animations.end())
			return;

		state = next;
		iter->second.reset();
	}

	const Animation* MapSummon::state_anim() const
	{
		auto iter = animations.find(state);

		if (iter == animations.end())
			return nullptr;

		return &iter->second;
	}

	void MapSummon::apply_damage(int32_t damage)
	{
		FieldMarker::apply_damage(damage);

		if (damage <= 0 || dying)
			return;

		// 受击帧：原生召唤兽受击会播 hit 段（取证：hit 4 帧存在）。
		if (animations.count("hit") > 0)
			set_state("hit");
	}

	void MapSummon::play_die()
	{
		if (dying)
			return;

		dying = true;

		if (animations.count("die") > 0)
		{
			set_state("die");
			return;
		}

		deactivate();
	}

	void MapSummon::play_spawn()
	{
		if (animations.count("summoned") > 0 && !dying)
			set_state("summoned");
	}

	void MapSummon::set_stance(int8_t s)
	{
		// SUMMON_SKILL 的 newStance（s9 Character.java:4217/4232，beholder 系召唤物）：
		// 固定 5（驻留）与随机 6..8（浮动系）。原生据此切换上下浮动帧；但本数据
		// 召唤兽帧集只有 stand/hit/die/summoned（探针实测，无游走/浮动帧），
		// 位置移动由 MOVE_SUMMON 包驱动 ⇒ 无帧可播，保持当前状态，不借用 hit
		// 冒充动作（会误表现为受击）。旧实现把 st 写进调试名牌，真对象化后移除。
		(void)s;
	}

	void MapSummon::set_flip(bool f)
	{
		flip = f;
	}

	void MapSummon::draw(double viewx, double viewy, float alpha) const
	{
		if (!active)
			return;

		const Animation* cur = state_anim();

		if (!cur)
		{
			// 帧集缺失（例如 3111002 傀儡只有 stand）⇒ 基类占位兜底。
			FieldMarker::draw(viewx, viewy, alpha);
			return;
		}

		Point<int16_t> absp = phobj.get_absolute(viewx, viewy, alpha);
		cur->draw(DrawArgument(absp, flip), alpha);
	}

	int8_t MapSummon::update(const Physics& physics)
	{
		int8_t layer = FieldMarker::update(physics);

		if (layer == -1)
			return layer;

		Animation* cur = const_cast<Animation*>(state_anim());

		if (!cur)
			return layer;

		if (cur->update())
		{
			// 一次性段播完的回退规则（取证帧集只有 stand/hit/die/summoned）：
			//   die      ⇒ 消失（REMOVE_SPECIAL_MAPOBJECT 通常也会补发，双保险）
			//   summoned ⇒ 入场闪光结束回 stand
			//   hit      ⇒ 回 stand
			//   stand    ⇒ 循环，不动
			if (state == "die")
			{
				deactivate();
				return -1;
			}

			if (state != "stand")
				set_state("stand");
		}

		return layer;
	}
}
