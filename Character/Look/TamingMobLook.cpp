//////////////////////////////////////////////////////////////////////////////////
//	Minimal taming-mob (mount) look for MONSTER_RIDING preview
//////////////////////////////////////////////////////////////////////////////////
#include "TamingMobLook.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <string>

namespace ms
{
	void TamingMobLook::set_itemid(int32_t iid)
	{
		if (iid == itemid && has_anim)
			return;

		clear();
		itemid = iid;
		if (iid <= 0)
			return;

		// 190xxxx 坐骑：优先 TamingMob/<id>.img；缺帧则空画（显式预留完整 CActionMan）。
		std::string strid = std::to_string(iid);
		nl::node src = nl::nx::TamingMob[strid + ".img"];
		if (!src)
			src = nl::nx::TamingMob[strid];
		if (!src)
			return;

		nl::node stand_n = src["stand"];
		if (!stand_n)
			stand_n = src["stand0"];
		nl::node walk_n = src["walk"];
		if (!walk_n)
			walk_n = src["walk1"];
		if (!walk_n)
			walk_n = src["move"];

		if (stand_n)
		{
			stand = Animation(stand_n);
			has_stand = true;
		}
		if (walk_n)
		{
			walk = Animation(walk_n);
			has_walk = true;
		}

		has_anim = has_stand || has_walk;
		if (!has_anim)
			itemid = 0;
	}

	void TamingMobLook::clear()
	{
		itemid = 0;
		has_anim = false;
		has_stand = false;
		has_walk = false;
		use_walk = false;
		stand = Animation();
		walk = Animation();
	}

	void TamingMobLook::draw(Point<int16_t> absp, bool f, float alpha) const
	{
		if (!has_anim)
			return;

		const bool walking = use_walk && has_walk;
		const Animation& anim = walking ? walk : (has_stand ? stand : walk);
		// 坐骑锚在脚下略下移，避免完全盖住角色中心。
		anim.draw(DrawArgument(absp + Point<int16_t>(0, 4), f), alpha);
	}

	void TamingMobLook::update()
	{
		if (!has_anim)
			return;
		if (use_walk && has_walk)
			walk.update();
		else if (has_stand)
			stand.update();
		else if (has_walk)
			walk.update();
	}

	void TamingMobLook::set_moving(bool moving)
	{
		use_walk = moving && has_walk;
	}
}
