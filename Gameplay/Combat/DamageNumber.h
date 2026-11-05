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

#include "../../Template/BoolPair.h"

#include "../../IO/Components/Charset.h"

#include <cstdint>
#include <string>

namespace ms
{
	class DamageNumber
	{
	public:
		static const size_t NUM_TYPES = 3;

		enum Type
		{
			NORMAL,
			CRITICAL,
			TOPLAYER
		};

		DamageNumber(Type type, int64_t damage, int16_t starty, int16_t x = 0);
		DamageNumber();

		void draw(double viewx, double viewy, float alpha) const;
		void set_x(int16_t headx);
		bool update();

		static int16_t rowheight(bool critical);
		static void init();

	private:
		int16_t getadvance(char c, bool first) const;

		static constexpr uint16_t FADE_TIME = 500;

		Type type;
		bool miss;
		bool multiple;
		int8_t firstnum;
		std::string restnum;
		int16_t shift;
		// 超长伤害数字（最多 16 位，2^53）的等比缩放系数，保证不溢出屏幕。
		// 运行时字符集只有 0-9 + Miss，无单位符号，故用缩放而不是单位缩写。
		float numscale;
		MovingObject moveobj;
		Linear<float> opacity;

		static BoolPair<Charset> charsets[NUM_TYPES];
	};
}