//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — Summon map object (真对象化：Skill.img summon 状态机)
//
//	取证（_tools/summon_probe 2026-11-03，直接解析 Data/Skill/*.img）：
//	v83 玩家召唤兽的动画不在 Mob.nx，而是随技能打包在
//	Skill/<ns>.img/skill/<id>/summon 下，子节点即状态机帧集：
//	  stand(8帧循环, origin 35,96)  hit(4帧受击)  die(1帧死亡)  summoned(12帧入场闪光)
//	修前读 summon["0"]（该节点不存在）⇒ 落到 summoned["0"]（7×7 占位小帧），
//	这就是召唤兽看不见 / 显示成小碎块的根因。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "FieldMarker.h"

#include <map>
#include <string>

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

		void draw(double viewx, double viewy, float alpha) const override;
		int8_t update(const Physics& physics) override;

		// 受击 / 死亡：由 DAMAGE_SUMMON、REMOVE_SPECIAL_MAPOBJECT(animated) 驱动。
		void apply_damage(int32_t damage) override;
		void play_die();
		// 入场闪光（spawnSummon animated=true，s9 Monster.java 接管时补发）。
		void play_spawn();

		void set_stance(int8_t s);
		void set_flip(bool flip);

	private:
		// 状态机：die 播完自清；hit/summoned 播完回 stand。
		void set_state(const std::string& state);
		const Animation* state_anim() const;

		std::map<std::string, Animation> animations;
		std::string state = "stand";
		bool dying = false;

		int32_t owner_cid = 0;
		int32_t skill_id = 0;
		int8_t skill_level = 0;
		bool flip = false;
	};
}
