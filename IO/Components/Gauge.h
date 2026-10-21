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

#include "../../Graphics/Texture.h"

namespace ms
{
	class Gauge
	{
	public:
		enum Type : uint8_t
		{
			DEFAULT,
			CASHSHOP,
			WORLDSELECT,
			// 自纹理左侧裁出 percentage*maximum 宽（不拉伸），用于 v83 StatusBar HP/MP。
			CLIP
		};

		Gauge() : type(DEFAULT), maximum(1), percentage(0), target(0), step(0), clip_left(0) {}
		Gauge(Type type, Texture front, int16_t maximum, float percent);
		Gauge(Type type, Texture front, Texture middle, int16_t maximum, float percent);
		Gauge(Type type, Texture front, Texture middle, Texture end, int16_t maximum, float percentage);

		void draw(const DrawArgument& args) const;
		void update(float target);

		/// CLIP 模式下从纹理的 clip_left 起裁出填充（MP 条用中段）。
		void set_clip_left(int16_t left) { clip_left = left; }

	private:
		Texture barfront;
		Texture barmid;
		Texture barend;
		int16_t maximum;

		float percentage;
		float target;
		float step;

		Type type;
		int16_t clip_left;
	};
}