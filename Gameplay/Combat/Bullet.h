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

#include "../Physics/PhysicsObject.h"

#include "../../Graphics/Animation.h"

namespace ms
{
	// Represents a projectile on a map
	class Bullet
	{
	public:
		// rotate_period_ms = WZ ball/rotatePeriod（每转一圈毫秒数，0=不旋转）。
		Bullet(Animation animation, Point<int16_t> origin, bool toleft,
			uint16_t rotate_period_ms = 0);

		void draw(double viewx, double viewy, float alpha) const;
		bool settarget(Point<int16_t> target);
		bool update(Point<int16_t> target);

	private:
		Animation animation;
		MovingObject moveobj;
		bool flip;
		// 投射物持续旋转（MinMS Bullet.cpp:124-133）：每 rotate_period 转一圈。
		uint16_t rotate_period = 0;
		uint16_t rotate_elapsed = 0;
		float angle = 0.0f;
	};
}