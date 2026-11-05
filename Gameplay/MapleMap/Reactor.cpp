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
#include "Reactor.h"

#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	Reactor::Reactor(int32_t o, int32_t r, int8_t s, Point<int16_t> p) : MapObject(o, p), rid(r), state(s)
	{
		std::string strid = string_format::extend_id(rid, 7);
		src = nl::nx::Reactor[strid + ".img"];

		// 被击打音效：路径经 _tools/summon_probe 解析 Sound/Reactor.img 取证（2026-11-03）：
		// {rid}/{state}/Hit —— audio 在**状态层**下（0..3 各一条，4 为空），
		// 与 IDA sub_989ADD 的 "Sound/Reactor.img/%s/%s"(3073)+"Hit"(5543) 吻合。
		// 因此命中哪个状态播该状态的 Hit（见 play_state_sound）。
		// 修前：TODO 从不播；波2-B 初版按 {rid}/Hit 加载，层级错误 ⇒ 空节点静音。

		animation_ended = true;
		dead = false;
		hittable = false;

		nl::node st = src[state] ? src[state] : src[0];
		for (auto sub : st)
			if (sub.name() == "event")
				if (sub["0"]["type"].get_integer() == 0)
					hittable = true;

		reload_idle();
	}

	void Reactor::reload_idle()
	{
		// Idle / 悬浮：原生读 state 节点下的帧循环；部分版本另挂 "default" 子节点
		// （IDA string id 960，sub_71165A @0x712213 push 3C0h）。
		// 本包 079/s9 Reactor.wz 无 name="default" 键（全量 XML 零命中），
		// 暗黑龙王裂隙 2201004 等靠 state/0 下 0..N canvas（delay=120）循环漂浮。
		// 修前：normal=src[0] 且 idle 分支从不 update ⇒ 523+ 多帧 idle 冻在第 0 帧。
		nl::node st = src[state];
		if (!st)
			st = src[0];

		nl::node idle = st["default"];
		if (!idle)
			idle = st;
		if (!idle)
			idle = src["default"];

		if (idle)
			normal = Animation(idle);
	}

	void Reactor::draw(double viewx, double viewy, float alpha) const
	{
		Point<int16_t> absp = phobj.get_absolute(viewx, viewy, alpha);
		Point<int16_t> shift = Point<int16_t>(0, normal.get_origin().y());

		if (animation_ended)
		{
			normal.draw(absp - shift, alpha);
		}
		else
		{
			animations.at(state - 1).draw(DrawArgument(absp - shift), 1.0);
		}
	}

	int8_t Reactor::update(const Physics& physics)
	{
		physics.move_object(phobj);

		if (!animation_ended)
		{
			animation_ended = animations.at(state - 1).update();
			// hit 转场播完后切到新状态 idle（含多帧悬浮），否则会一直画旧 normal。
			if (animation_ended)
				reload_idle();
		}
		else
		{
			// idle 循环：Animation::update 末帧返回 ended=true 并 rewind 到 0，忽略返回值。
			normal.update();
		}

		if (animation_ended && dead)
			deactivate();

		return phobj.fhlayer;
	}

	void Reactor::set_state(int8_t newstate)
	{
		const int8_t old = this->state;
		// 转场帧必须按 draw/update 的取帧下标（state-1）存储，而不是旧状态值：
		// s9 顺序触发（new = old+1，Reactor.java:292 setState）时两者重合、行为不变；
		// 乱序跳态（timeout 换状态 Reactor.java:347-349）时修前 animations.at(state-1)
		// 查不到旧键直接抛 std::out_of_range（崩溃点 :62/:71）。
		this->state = newstate;

		if (hittable)
		{
			nl::node hit = src[old]["hit"];
			if (hit)
			{
				animations[newstate - 1] = hit;
				animation_ended = false;
				play_state_sound(old);
			}
			else
			{
				// 缺 hit 帧：立刻换新状态 idle，不再 at() 崩。
				animation_ended = true;
				reload_idle();
			}
		}
		else
		{
			// 非打击型（道具/触碰等）：无 hit 转场，直接切 idle。
			animation_ended = true;
			reload_idle();
		}
	}

	void Reactor::destroy(int8_t newstate, Point<int16_t> position)
	{
		// s9 destroyReactor（PacketCreator.java:4233：oid+state+pos）传破坏后状态与落点；
		// 修前两个参数全弃用、写死 ++，服务端乱序下发时与真实状态脱节。
		if (position.x() || position.y())
			set_position(position);

		const int8_t old = this->state;
		this->state = (newstate > old) ? newstate : static_cast<int8_t>(old + 1);

		nl::node hit = src[old]["hit"];
		if (hit)
		{
			animations[this->state - 1] = hit;
			play_state_sound(old);
			animation_ended = false;
		}
		else
		{
			animation_ended = true; // 无 hit 帧：update 下一拍直接 deactivate
			reload_idle();
		}
		dead = true;
	}

	void Reactor::play_state_sound(int8_t st)
	{
#ifdef USE_NX
		// Sound/Reactor.img/{rid}/{state}/Hit —— 见 ctor 顶部取证注释。
		nl::node snd = nl::nx::Sound["Reactor.img"][std::to_string(rid)]
			[std::to_string(static_cast<int>(st))]["Hit"];

		if (snd)
			Sound(snd).play();
#else
		(void)st;
#endif
	}

	bool Reactor::is_hittable() const
	{
		return hittable;
	}

	bool Reactor::is_in_range(const Rectangle<int16_t>& range) const
	{
		if (!active)
			return false;

		Rectangle<int16_t> bounds(Point<int16_t>(-30, -normal.get_dimensions().y()), Point<int16_t>(normal.get_dimensions().x() - 10, 0)); //normal.get_bounds(); //animations.at(stance).get_bounds();
		bounds.shift(get_position());

		return range.overlaps(bounds);
	}
}
