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

		normal = src[0];
		animation_ended = true;
		dead = false;
		hittable = false;

		for (auto sub : src[0])
			if (sub.name() == "event")
				if (sub["0"]["type"].get_integer() == 0)
					hittable = true;
	}

	void Reactor::draw(double viewx, double viewy, float alpha) const
	{
		Point<int16_t> absp = phobj.get_absolute(viewx, viewy, alpha);
		Point<int16_t> shift = Point<int16_t>(0, normal.get_origin().y());

		if (animation_ended)
		{
			// TODO: Handle 'default' animations (horntail reactor floating)
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
			animation_ended = animations.at(state - 1).update();

		if (animation_ended && dead)
			deactivate();

		return phobj.fhlayer;
	}

	void Reactor::set_state(int8_t state)
	{
		if (hittable)
		{
			animations[this->state] = src[this->state]["hit"];
			animation_ended = false;
			play_state_sound(this->state);
		}

		this->state = state;
	}

	void Reactor::destroy(int8_t, Point<int16_t>)
	{
		animations[this->state] = src[this->state]["hit"];
		play_state_sound(this->state);
		state++;
		dead = true;
		animation_ended = false;
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