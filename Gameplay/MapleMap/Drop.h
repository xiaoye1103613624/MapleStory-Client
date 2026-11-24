//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "MapObject.h"

#include "../../Graphics/Animation.h"
#include "../../Template/Rectangle.h"

namespace ms
{
	class Drop : public MapObject
	{
	public:
		virtual int8_t update(const Physics& physics) override;

		void expire(int8_t, const PhysicsObject*);

		Rectangle<int16_t> bounds() const;
	// 只有非拾取中且仍 active 的掉落物才能成为拾取目标。PICKEDUP 态要淡出
	// 48 帧才 deactivate，期间 bounds 仍在玩家脚下；不过滤会让 find_loot_at
	// 一直选中正在消失的旧卡、对已移除 oid 反复发包，挡住真正的新掉落。
	bool is_pickable() const { return state != State::PICKEDUP && is_active(); }

	protected:
		// grade：⑥ 掉落品质（0=不上光环，1..4=Rare..Legendary）。
		// 素材与档位语义见 DropAuras.h；这里只负责按档位驱动三组 Animation。
		Drop(int32_t oid, int32_t owner, Point<int16_t> start,
			Point<int16_t> dest, int8_t type, int8_t mode, bool playerdrop,
			int8_t grade = 0);

		// 光环绘制：back 在图标下层、front 在上层，光柱播一次后自隐。
		// 由子类在自己的图标绘制前后调用（ItemDrop/MesoDrop）。
		void draw_aura_back(Point<int16_t> absp, float alpha) const;
		void draw_aura_front(Point<int16_t> absp, float alpha) const;
		// 光环推进（在 update 里调一次）；返回是否仍需要绘制光环。
		void update_aura();

		// ⑥ B 方案：把特效动画的几何中心对齐装备锚点 absp。
		// DrawArgument(pos) 让图片 origin 落在 pos；要让图片中心落在 absp，
		// 需 pos = absp + origin - dimensions/2（飞行中/落地后统一以装备为中心）。
		static Point<int16_t> centered_pos(Point<int16_t> absp, const Animation& anim);

		Linear<float> opacity;
		Linear<float> angle;

		int8_t grade = 0;

	private:
		enum State
		{
			DROPPED,
			FLOATING,
			PICKEDUP
		};

		int32_t owner;
		int8_t pickuptype;
		bool playerdrop;

		const PhysicsObject* looter;
		State state;

		Point<int16_t> dest;
		double basey;
		double moved;

		// ⑥ 每掉落物各自持有光环状态（不能用全局定时器：参考实现
		// dropitemaura.cpp 用 SetTimer 全局推进，本端 Animation 自带
		// delay=90ms 推进，只要每个 drop 各持一份即可，无需定时器）。
		mutable Animation aura_front;
		mutable Animation aura_back;
		mutable Animation aura_pillar;
		bool aura_front_ok = false;
		bool aura_back_ok = false;
		bool aura_pillar_ok = false;
		bool pillar_done = false;
	};
}