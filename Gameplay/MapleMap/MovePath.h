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

#include <vector>

#include "../Movement.h"
#include "../Physics/PhysicsObject.h"

namespace ms
{
	// 服务端移动路径回放器（等价 Angel v83 `CMovePath`）。
	//
	// Angel `CMob::OnMove` 在末尾调用 `CMovePath::OnMovePacket`，把整段移动路径
	// 装入并按 `duration` 逐段推进；非控制怪的位置完全由该路径决定，不做本地
	// 物理自由推进。本类复刻该语义：`push()` 装入路径，`apply()` 每帧推进并按
	// 段 `newstate`/`fh` 同步姿态与平台，路径结束时返回 false 供上层回调
	// （控制怪据此触发下一次 GenerateMovePath）。
	class MovePath
	{
	public:
		MovePath() = default;

		// 清空并停用。
		void reset();

		// 装入一条服务端下发的移动路径。start 为包内起点（用于对齐首段 lastx/lasty，
		// 避免插值从旧位置跳变）。
		void push(Point<int16_t> start, std::vector<Movement>&& segments);

		bool is_active() const
		{
			return active;
		}

		// 每帧推进。返回 true = 路径仍在进行；false = 本帧已结束（无路径或刚走完最后一段）。
		bool apply(PhysicsObject& phobj);

		// 当前段的姿态（newstate），供 Mob 切换动画。
		uint8_t get_stance() const;

		// 当前段的 foothold id，供平台约束。
		uint16_t get_fh() const;

	private:
		void enter_segment(size_t i);

		std::vector<Movement> segments;
		size_t index = 0;
		int16_t remaining = 0;	// 当前段剩余毫秒
		int16_t total = 0;		// 当前段总毫秒（插值分母，防 0）
		bool active = false;
	};
}