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
#include "Cursor.h"

#include "../Constants.h"
#include "../Util/Misc.h"
#include "../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	Cursor::Cursor()
	{
		state = Cursor::State::IDLE;
		hide_counter = 0;

		for (int i = 0; i < State::LENGTH; i++)
			state_loaded[i] = false;
	}

	void Cursor::init()
	{
		nl::node src = nl::nx::UI["Basic.img"]["Cursor"];

		// v83 / BeiDou 通常只有 Cursor/0..13。LEAF=18、聊天栏 67+ 在经典包里
		// 常缺失或指向其它 Basic 子树；误绑会把光标画成数字字形。
		static const State kClassicStates[] = {
			IDLE, CANCLICK, GAME, HOUSE, CANCLICK2, CANGRAB, GIFT,
			VSCROLL, HSCROLL, VSCROLLIDLE, HSCROLLIDLE, GRABBING,
			CLICKING, RCLICK
		};
		static const State kModernExtra[] = {
			LEAF, CHATBARVDRAG, CHATBARHDRAG, CHATBARBLTRDRAG,
			CHATBARMOVE, CHATBARBRTLDRAG
		};

		for (int i = 0; i < State::LENGTH; i++)
			state_loaded[i] = false;

		if (!src)
		{
			LOG(LOG_WARN, "[Cursor] Basic.img/Cursor missing");
			return;
		}

		auto try_bind = [&](State s)
		{
			const int id = static_cast<int>(s);
			nl::node node = src[std::to_string(id)];

			if (!node)
				return;

			// 至少要有一张可绘 bitmap，避免空 imgdir / 字库节点被当成光标。
			bool has_bitmap = node.data_type() == nl::node::type::bitmap;
			if (!has_bitmap)
			{
				for (auto sub : node)
				{
					if (sub.data_type() == nl::node::type::bitmap)
					{
						has_bitmap = true;
						break;
					}
				}
			}
			if (!has_bitmap)
				return;

			animations[s] = node;
			state_loaded[s] = true;
		};

		for (State s : kClassicStates)
			try_bind(s);

		if (!UiAssetResolve::is_classic_hud())
		{
			for (State s : kModernExtra)
				try_bind(s);
		}

		if (!state_loaded[IDLE])
			LOG(LOG_WARN, "[Cursor] IDLE (0) frame missing — cursor may be invisible");
	}

	void Cursor::draw(float alpha) const
	{
		constexpr int64_t HIDE_AFTER = HIDE_TIME / Constants::TIMESTEP;

		if (hide_counter < HIDE_AFTER && state_loaded[state])
			animations[state].draw(position, alpha);
	}

	void Cursor::update()
	{
		if (state_loaded[state])
			animations[state].update();

		switch (state)
		{
		case Cursor::State::CANCLICK:
		case Cursor::State::CANCLICK2:
		case Cursor::State::CANGRAB:
		case Cursor::State::CLICKING:
		case Cursor::State::GRABBING:
			hide_counter = 0;
			break;
		default:
			hide_counter++;
			break;
		}
	}

	void Cursor::set_state(State s)
	{
		State next = s;

		if (next != State::IDLE && (next < 0 || next >= State::LENGTH || !state_loaded[next]))
			next = State::IDLE;

		if (state != next)
		{
			state = next;

			if (state_loaded[state])
				animations[state].reset();

			hide_counter = 0;
		}
	}

	void Cursor::set_position(Point<int16_t> cursor_position)
	{
		position = cursor_position;
		hide_counter = 0;
	}

	Cursor::State Cursor::get_state() const
	{
		return state;
	}

	Point<int16_t> Cursor::get_position() const
	{
		return position;
	}
}
