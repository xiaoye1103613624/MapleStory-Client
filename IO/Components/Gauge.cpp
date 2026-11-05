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
#include "Gauge.h"

#include <algorithm>

namespace ms
{
	Gauge::Gauge(Type type, Texture front, int16_t maximum, float percentage) : Gauge(type, front, {}, maximum, percentage) {}
	Gauge::Gauge(Type type, Texture front, Texture middle, int16_t maximum, float percentage) : Gauge(type, front, {}, {}, maximum, percentage) {}
	Gauge::Gauge(Type type, Texture front, Texture middle, Texture end, int16_t maximum, float percentage)
		: type(type), barfront(front), barmid(middle), barend(end), maximum(maximum), percentage(percentage), target(percentage), step(0), clip_left(0) {}

	void Gauge::draw(const DrawArgument& args) const
	{
		int16_t length = static_cast<int16_t>(percentage * maximum);

		if (length > 0)
		{
			if (type == Type::DEFAULT)
			{
				barfront.draw(args + DrawArgument(Point<int16_t>(0, 0), Point<int16_t>(length, 0)));
				barmid.draw(args);
				barend.draw(args + Point<int16_t>(length + 8, 20));
			}
			else if (type == Type::CASHSHOP)
			{
				Point<int16_t> pos_adj = Point<int16_t>(45, 1);

				barfront.draw(args - pos_adj);
				barmid.draw(args + DrawArgument(Point<int16_t>(0, 0), Point<int16_t>(length, 0)));
				barend.draw(args - pos_adj + Point<int16_t>(length + barfront.width(), 0));
			}
			else if (type == Type::WORLDSELECT || type == Type::CLIP)
			{
				// horizontal Range(left_omit, right_omit) 会同时平移屏幕矩形；
				// CLIP 带 clip_left 时先左移，使可见段仍锚在 args 原点。
				const int16_t left_omit = clip_left;
				const int16_t right_omit = static_cast<int16_t>(
					std::max(0, barfront.width() - left_omit - length));
				DrawArgument draw_args = (left_omit != 0)
					? args + Point<int16_t>(static_cast<int16_t>(-left_omit), 0)
					: args;
				barfront.draw(draw_args, {}, Range<int16_t>(left_omit, right_omit));
			}
		}
		else if (type != Type::WORLDSELECT)
		{
			// WORLDSELECT 0% 不画填充：负载槽已烘焙在频道单元位图
			// channel/N/normal (6..80, 18..26) 内，参照帧空负载时槽为空
			// （选中频道页.png 无彩虹条）；旧实现画满宽彩虹条，观感不符。
		}
	}

	void Gauge::update(float t)
	{
		if (target != t)
		{
			target = t;
			step = (target - percentage) / 24;
		}

		if (percentage != target)
		{
			percentage += step;

			if (step < 0.0f)
			{
				if (target - percentage >= step)
					percentage = target;
			}
			else if (step > 0.0f)
			{
				if (target - percentage <= step)
					percentage = target;
			}
		}
	}
}