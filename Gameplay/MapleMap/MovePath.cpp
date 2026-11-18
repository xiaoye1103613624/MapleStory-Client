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
#include "MovePath.h"

#include "../../Constants.h"

namespace ms
{
	void MovePath::reset()
	{
		segments.clear();
		index = 0;
		remaining = 0;
		total = 0;
		active = false;
	}

	void MovePath::push(Point<int16_t> start, std::vector<Movement>&& in)
	{
		segments = std::move(in);
		index = 0;
		remaining = 0;
		total = 0;
		active = !segments.empty();

		// 首段 lastx/lasty 用包内起点对齐，避免从上一个残留位置插值跳变。
		if (!segments.empty())
		{
			segments.front().lastx = start.x();
			segments.front().lasty = start.y();
		}

		if (active)
			enter_segment(0);
	}

	void MovePath::enter_segment(size_t i)
	{
		const Movement& seg = segments[i];

		remaining = seg.duration > 0 ? seg.duration : 1;
		total = remaining;
	}

	bool MovePath::apply(PhysicsObject& phobj)
	{
		if (!active || segments.empty())
			return false;

		if (index >= segments.size())
		{
			active = false;
			return false;
		}

		const Movement& seg = segments[index];

		// 按段进度线性插值 last→pos（与服务端路径一致，不做本地物理）。
		if (total > 0)
		{
			float t = 1.0f - static_cast<float>(remaining) / static_cast<float>(total);

			if (t < 0.0f)
				t = 0.0f;
			if (t > 1.0f)
				t = 1.0f;

			phobj.set_x(static_cast<int16_t>(seg.lastx + (seg.xpos - seg.lastx) * t));
			phobj.set_y(static_cast<int16_t>(seg.lasty + (seg.ypos - seg.lasty) * t));
		}
		else
		{
			phobj.set_x(seg.xpos);
			phobj.set_y(seg.ypos);
		}

		phobj.fhid = seg.fh;

		remaining -= Constants::TIMESTEP;

		if (remaining <= 0)
		{
			// 段结束：精确落到段终点，切下一段。
			phobj.set_x(seg.xpos);
			phobj.set_y(seg.ypos);

			index++;

			if (index >= segments.size())
			{
				active = false;
				return false;
			}

			enter_segment(index);
		}

		return true;
	}

	uint8_t MovePath::get_stance() const
	{
		if (segments.empty())
			return 0;

		size_t i = index < segments.size() ? index : segments.size() - 1;
		return segments[i].newstate;
	}

	uint16_t MovePath::get_fh() const
	{
		if (segments.empty())
			return 0;

		size_t i = index < segments.size() ? index : segments.size() - 1;
		return segments[i].fh;
	}
}