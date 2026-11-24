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
#include "Drop.h"

#include "DropAuras.h"

namespace ms
{
	Drop::Drop(int32_t id, int32_t own, Point<int16_t> start, Point<int16_t> dst, int8_t type, int8_t mode, bool pldrp, int8_t grd) : MapObject(id)
	{
		owner = own;
		set_position(start.x(), start.y() - 4);
		dest = dst;
		pickuptype = type;
		playerdrop = pldrp;
		grade = grd;

		// ⑥ 光环：按档位取素材。素材缺失时三组都保持 invalid，
		// draw_aura_* 直接早退，掉落物照常显示（优雅降级）。
		DropAuras& auras = DropAuras::get();
		auras.init();

		if (auras.has_grade(grade))
		{
			aura_front = auras.front(grade);
			aura_front_ok = true;
			aura_front.reset();
		}

		if (auras.has_back(grade))
		{
			aura_back = auras.back(grade);
			aura_back_ok = true;
			aura_back.reset();
		}

		// 光柱共用一套素材：仅潜能品级 grade>=1 才挂，普通掉落不播。
		if (grade >= 1 && auras.has_pillar())
		{
			aura_pillar = auras.pillar();
			aura_pillar_ok = true;
			aura_pillar.reset();
		}

		angle.set(0.0f);
		opacity.set(1.0f);
		moved = 0.0f;
		looter = nullptr;

		switch (mode)
		{
		case 0:
		case 1:
			state = Drop::State::DROPPED;
			basey = static_cast<double>(dest.y() - 4);
			phobj.vspeed = -5.0f;
			phobj.hspeed = static_cast<double>(dest.x() - start.x()) / 48;
			break;
		case 2:
			state = Drop::State::FLOATING;
			basey = phobj.crnt_y();
			phobj.type = PhysicsObject::Type::FIXATED;
			break;
		case 3:
			state = Drop::State::PICKEDUP;
			phobj.vspeed = -5.0f;
			break;
		default:
			// s9 的 DROP_ITEM mod 实际只发 {1,2,3}（MapleMap.spawnDrop/spawnMesoDrop 恒 1、
			// MapItem:253 恒 2、disappearing*Drop 恒 3），所以下列分支在当前协议下不可达；
			// 但 State state; 在 Drop.h 里没有初值，缺 default 时未知 mode 会让 state 保持
			// 未初始化，update() 立刻拿它做比较（UB）。兜底成 DROPPED 等价于「最保守的落地态」，
			// 合法 mode 的取值与旧行为逐字一致。
			state = Drop::State::DROPPED;
			basey = static_cast<double>(dest.y() - 4);
			phobj.vspeed = -5.0f;
			phobj.hspeed = static_cast<double>(dest.x() - start.x()) / 48;
			break;
		}
	}

	int8_t Drop::update(const Physics& physics)
	{
		physics.move_object(phobj);

		// ⑥ 光环推进（与掉落物状态机解耦：光环只是视觉层，不参与物理/拾取）。
		update_aura();

		if (state == Drop::State::DROPPED)
		{
			if (phobj.onground)
			{
				phobj.hspeed = 0.0;
				phobj.type = PhysicsObject::Type::FIXATED;
				state = Drop::State::FLOATING;
				angle.set(0.0f);
				set_position(dest.x(), dest.y() - 4);
			}
			else
			{
				static const float SPINSTEP = 0.2f;
				angle += SPINSTEP;
			}
		}

		if (state == Drop::State::FLOATING)
		{
			phobj.y = basey + 5.0f + (cos(moved) - 1.0f) * 2.5f;
			moved = (moved < 360.0f) ? moved + 0.025f : 0.0f;
		}

		if (state == Drop::State::PICKEDUP)
		{
			static const uint16_t PICKUPTIME = 48;
			static const float OPCSTEP = 1.0f / PICKUPTIME;

			if (looter)
			{
				double hdelta = looter->x - phobj.x;
				phobj.hspeed = looter->hspeed / 2.0 + (hdelta - 16.0) / PICKUPTIME;
			}

			opacity -= OPCSTEP;

			if (opacity.last() <= OPCSTEP)
			{
				opacity.set(1.0f);

				MapObject::deactivate();
				return -1;
			}
		}

		return phobj.fhlayer;
	}

	void Drop::expire(int8_t type, const PhysicsObject* lt)
	{
		// round90 三方对账（Angel 083 只读取证；零行为改动，勿据此改 wire）：
		// 083 CDropPool::OnDropLeaveField @0x506590:83-99 逐字 ——
		//   v2 = Decode1(animation); Decode4(oid);
		//   if (v2 == 2 || v2 == 3 || v2 == 5) Decode4(looter cid);
		//   else if (v2 == 4) { Decode2(delay); *(v4+68) = get_update_time() + delay; *(v79+72) = 4; }
		//   v2 == 0 走 IWzGr2DLayer::Animate / RemoveCanvas / GetCanvas COM 消亡动画。
		// s9 PacketCreator.java:2624-2642 removeItemFromMap 对 animation >= 2 恒写 writeInt(chrId) 4B。
		// => animation=4 是 wire 宽度分叉（083 2B delay vs s9 4B cid），数值同为 4、语义不同；
		//    按 s9 读 int cid 才正确（W 桶；改回 083 读 short 会平移后续字段、断联机）。
		// => animation=0 的 COM 画布特效当前端无 IWzGr2DLayer 栈，48 帧淡出语义等价（F 维 P2，不补）。
		// 证据：tools/_ida_r90/OnDropLeaveField.code.txt +
		//       drop-angel-calibration-2026-11/README.md:11/73/81/119/243-244/266-277。
		// s9 实际发送的 animation 值（removeItemFromMap 调用点全枚举）：0/1/2/4/5
		//（PacketCreator.java:2602 注释 0=expire 1=无动画 2=拾取 4=爆炸；
		//  Character.java:2078 pet 拾取走 5；AbstractDealDamageHandler.java:229 走 4）。
		// 修前 3/4/5 与未知值不进任何 case ⇒ 状态不变、永不消失，掉落物永久残留。
		switch (type)
		{
		case 0:
			state = Drop::State::PICKEDUP;
			break;
		case 2:
		case 4: // 爆炸/吸卷拾取（s9 chrId=0 ⇒ looter 为空，原地淡出）
		case 5: // 宠物拾取：RemoveLootHandler 已把主人 phobj 喂给 looter，同 2 飞取
			angle.set(0.0f);
			state = Drop::State::PICKEDUP;
			looter = lt;
			phobj.vspeed = -4.5f;
			phobj.type = PhysicsObject::Type::NORMAL;
			break;
		case 1:
		default:
			deactivate();
			break;
		}
	}

	// ---------------------------------------------------------------
	// ⑥ 装备掉落品质光环
	// 参考实现 dropitemaura.cpp:133-134 用 front/back 双层 + 120ms 定时器切换到
	// 下一帧；本端 Animation 自带 delay（素材实测 90ms），各自持有即可，
	// 不需要定时器（Animation::update 走固定 TIMESTEP 推进）。
	// 光柱 dropItemEffect 只播一次：Animation::update() 在末帧返回 true 后
	// 会回绕到 0（Audio/Animation.cpp:320-323 的非 zigzag 分支），所以本端
	// 用 pillar_done 记住「已播完」，之后不再绘制也不再 update。
	// ---------------------------------------------------------------

	void Drop::update_aura()
	{
		if (aura_front_ok)
			aura_front.update();

		if (aura_back_ok)
			aura_back.update();

		if (aura_pillar_ok && !pillar_done)
		{
			// 打开始光柱：只在掉落物还是「掉落中/静止」时播，
			// 被捡起（PICKEDUP）后不再播，避免拾取动画上叠光柱。
			if (state == State::PICKEDUP)
				pillar_done = true;
			else if (aura_pillar.update())
				pillar_done = true;
		}
	}

	Point<int16_t> Drop::centered_pos(Point<int16_t> absp, const Animation& anim)
	{
		// DrawArgument(pos) 令图片 origin 落在 pos；几何中心 = pos - origin + dims/2。
		// 要几何中心落在 absp：pos = absp + origin - dims/2。
		Point<int16_t> origin = anim.get_origin();
		Point<int16_t> dims = anim.get_dimensions();
		return absp + Point<int16_t>(origin.x() - dims.x() / 2, origin.y() - dims.y() / 2);
	}

	void Drop::draw_aura_back(Point<int16_t> absp, float alpha) const
	{
		// 层级（自底向上）：光柱 → aura.back → 图标 → aura.front。
		// ⑥ 光效几何中心统一对齐装备锚点 absp（飞行中/落地后一致）。
		// 光柱：几何中心对齐装备（去掉参考实现的 -16 落点上移，B 方案以装备为中心）。
		if (aura_pillar_ok && !pillar_done)
			aura_pillar.draw(DrawArgument(centered_pos(absp, aura_pillar), 1.0f), alpha);

		if (aura_back_ok)
			aura_back.draw(DrawArgument(centered_pos(absp, aura_back), 1.0f), alpha);
	}

	void Drop::draw_aura_front(Point<int16_t> absp, float alpha) const
	{
		if (aura_front_ok)
			aura_front.draw(DrawArgument(centered_pos(absp, aura_front), 1.0f), alpha);
	}

	Rectangle<int16_t> Drop::bounds() const
	{
		auto lt = get_position();
		auto rb = lt + Point<int16_t>(32, 32);

		return Rectangle<int16_t>(lt, rb);
	}
}