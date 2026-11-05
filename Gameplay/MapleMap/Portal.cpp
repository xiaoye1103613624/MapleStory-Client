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
#include "Portal.h"

namespace ms
{
	Portal::Portal(const Animation* a, HideAnims hide, Type t, std::string nm, bool intramap, Point<int16_t> p, int32_t tid, std::string tnm)
		: regular(a), hide_src(hide), type(t), name(nm), position(p),
		  warpinfo(tid, intramap, tnm, nm,
			t == SCRIPTED || t == SCRIPTED_INVISIBLE || t == SCRIPTED_TOUCH || t == SCRIPTED_HIDDEN)
	{
		touched = false;
		phase = HidePhase::NONE;
	}

	Portal::Portal() : Portal(nullptr, {}, SPAWN, "", false, Point<int16_t>(), 0, "") {}

	void Portal::set_phase(HidePhase next, const Animation* src)
	{
		phase = next;
		if (src)
		{
			owned = *src;
			owned.reset();
		}
	}

	void Portal::update(Point<int16_t> playerpos)
	{
		bool now = bounds().contains(playerpos);

		if (is_hide_family(type))
		{
			// 上升沿：进入 → Start；下降沿：离开 → Exit。
			if (now && !touched)
				set_phase(HidePhase::START, hide_src.start ? hide_src.start : hide_src.cont);
			else if (!now && touched)
				set_phase(HidePhase::EXIT, hide_src.exit ? hide_src.exit : hide_src.cont);

			touched = now;

			if (phase == HidePhase::NONE)
				return;

			bool ended = owned.update();
			if (!ended)
				return;

			if (phase == HidePhase::START)
				set_phase(HidePhase::CONTINUE, hide_src.cont ? hide_src.cont : hide_src.start);
			else if (phase == HidePhase::EXIT)
				phase = HidePhase::NONE;
			// CONTINUE：循环，update 结束时 Animation 自复位；再 reset 一次保险
			else if (phase == HidePhase::CONTINUE)
				owned.reset();
			return;
		}

		// TOUCH：仅跟踪 touched（显式门动画由 MapPortals 共享推进）
		touched = now;
	}

	void Portal::draw(Point<int16_t> viewpos, float inter) const
	{
		if (is_hide_family(type))
		{
			if (phase == HidePhase::NONE)
				return;
			owned.draw(position + viewpos, inter);
			return;
		}

		if (!regular)
			return;

		// 旧语义：HIDDEN 未触碰不画——已迁到 hide 家族；TOUCH 始终画。
		regular->draw(position + viewpos, inter);
	}

	std::string Portal::get_name() const
	{
		return name;
	}

	Portal::Type Portal::get_type() const
	{
		return type;
	}

	Point<int16_t> Portal::get_position() const
	{
		return position;
	}

	Rectangle<int16_t> Portal::bounds() const
	{
		auto lt = position + Point<int16_t>(-25, -100);
		auto rb = position + Point<int16_t>(25, 25);

		return Rectangle<int16_t>(lt, rb);
	}

	Portal::WarpInfo Portal::getwarpinfo() const
	{
		return warpinfo;
	}
}
