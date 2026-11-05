//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — Evan Dragon map object
//////////////////////////////////////////////////////////////////////////////////
#include "MapDragon.h"

#include "../../Constants.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	MapDragon::MapDragon(int32_t oid, int32_t owner, Point<int16_t> pos, int8_t stance, int16_t job)
		// 原生龙无头顶名牌；空 label 复用基类清除路径。
		: FieldMarker(oid, Kind::DRAGON, pos, std::string()),
		owner_cid(owner)
	{
#ifdef USE_NX
		// 等级映射：Evan 子职 1001..1005（s9 hasSPTable：EVAN1..EVAN5），
		// 但部署资源只有 8840000 一只（8840001..14 与参照端同缺）⇒ 全档取同帧集。
		// job 存而少用：仅作来源记录（日志/调试），不产生表现差异。
		(void)job;

		nl::node src = nl::nx::Mob["8840000.img"];

		// 探针坐实段名：stand/move/hit(单帧段名为 hit，非 hit1)/die1。
		// 帧集加载走 MapSummon 同法：只收容器段（type::none 且有名），
		// info 段跳过。
		if (src)
		{
			for (auto c : src)
			{
				if (c.data_type() != nl::node::type::none)
					continue;

				std::string name = c.name();

				if (name == "info" || name == "summon")
					continue;

				animations[name] = Animation(c);
			}
		}

		if (animations.count("stand") == 0)
			return;	// 无 stand ⇒ 基类占位文本兜底（不空画）。

		state = "stand";
		// 初始朝向（蓝军自检补漏）：spawn 包 stance 字节奇偶 = 朝向，
		// 与 Mob::set_stance（Mob.cpp:189）同法；修前落位后首帧恒用默认朝向。
		flip = (stance % 2) == 0;
#endif
	}

	void MapDragon::apply_move(Point<int16_t> pos, uint8_t state_byte)
	{
		set_position(pos);

		// 朝向：偶数=朝左（flip），与 Mob::set_stance（Mob.cpp:189）同法。
		flip = (state_byte % 2) == 0;

		// 有位移 ⇒ 播 move 段（若有），400ms 余晖后回 stand（原生龙不驻留移动帧）。
		if (animations.count("move") > 0)
		{
			auto iter = animations.find("move");
			iter->second.reset();
			state = "move";
			move_ttl = 400;
		}
	}

	void MapDragon::draw(double viewx, double viewy, float alpha) const
	{
		if (!active)
			return;

		auto iter = animations.find(state);

		if (iter == animations.end())
		{
			// 帧集缺失 ⇒ 基类占位兜底（同 MapSummon::draw 口径）。
			FieldMarker::draw(viewx, viewy, alpha);
			return;
		}

		Point<int16_t> absp = phobj.get_absolute(viewx, viewy, alpha);
		iter->second.draw(DrawArgument(absp, flip), alpha);
	}

	int8_t MapDragon::update(const Physics& physics)
	{
		int8_t layer = FieldMarker::update(physics);

		if (layer == -1)
			return layer;

		auto iter = animations.find(state);

		if (iter == animations.end())
			return layer;

		iter->second.update();

		if (state == "move")
		{
			if (move_ttl > 0)
				move_ttl -= Constants::TIMESTEP;
			else
			{
				state = "stand";
				auto sit = animations.find("stand");
				if (sit != animations.end())
					sit->second.reset();
			}
		}

		return layer;
	}
}
