//////////////////////////////////////////////////////////////////////////////////
//	Minimal taming-mob (mount) look for MONSTER_RIDING preview
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Graphics/Animation.h"

namespace ms
{
	// 最小坐骑预览：有 MONSTER_RIDING + 装备 TAMEDMOB 时在角色脚下画 stand/walk。
	// 非完整 CActionMan / MergeTamingMobSprite；缺帧则空画。
	class TamingMobLook
	{
	public:
		TamingMobLook() = default;

		void set_itemid(int32_t itemid);
		void clear();
		bool is_valid() const { return itemid != 0 && has_anim; }

		void draw(Point<int16_t> absp, bool flip, float alpha) const;
		void update();
		void set_moving(bool moving);

		int32_t get_itemid() const { return itemid; }

	private:
		int32_t itemid = 0;
		bool has_anim = false;
		bool has_stand = false;
		bool has_walk = false;
		bool use_walk = false;
		Animation stand;
		Animation walk;
	};
}
