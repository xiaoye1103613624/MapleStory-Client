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

#include "Texture.h"

#include "../Template/Interpolated.h"
#include "../Template/Rectangle.h"

#include <vector>

namespace ms
{
	// A single frame within an animation.
	class Frame
	{
	public:
		Frame(nl::node src);
		// Timing / opacity from meta; bitmap from bitmap_src; origin kept from meta.
		Frame(nl::node meta, nl::node bitmap_src);
		Frame();

		void draw(const DrawArgument& args) const;

		uint8_t start_opacity() const;
		uint16_t start_scale() const;
		uint16_t get_delay() const;
		Point<int16_t> get_origin() const;
		Point<int16_t> get_dimensions() const;
		Point<int16_t> get_head() const;
		Rectangle<int16_t> get_bounds() const;
		float opcstep(uint16_t timestep) const;
		float scalestep(uint16_t timestep) const;
		// 单帧节点里 a0 是静态 alpha；没有 a1 作为配对端点时，把
		// read_frame_timing 生成的 {a0, 255-a0} 互补 ramp 压平成 {a0, a0}，
		// 避免静态背景每 ~100ms 呼吸一次造成 ~9-10Hz 频闪（如 shineWood back/16）。
		void freeze_opacity_if_static();

	private:
		Texture texture;
		uint16_t delay;
		std::pair<uint8_t, uint8_t> opacities;
		std::pair<int16_t, int16_t> scales;
		Rectangle<int16_t> bounds;
		Point<int16_t> head;
		bool has_explicit_a0;
		bool has_explicit_a1;
	};

	// Class which consists of multiple textures to make an Animation.
	class Animation
	{
	public:
		Animation(nl::node source);
		// Same frame timing as source, but every frame draws texture_override.
		Animation(nl::node source, nl::node texture_override);
		Animation();

		bool update();
		bool update(uint16_t timestep);
		void reset();

		void draw(const DrawArgument& arguments, float alpha) const;

		uint16_t get_delay(int16_t frame) const;
		uint16_t getdelayuntil(int16_t frame) const;
		Point<int16_t> get_origin() const;
		Point<int16_t> get_dimensions() const;
		Point<int16_t> get_head() const;
		Rectangle<int16_t> get_bounds() const;

	private:
		const Frame& get_frame() const;

		std::vector<Frame> frames;
		bool animated;
		bool zigzag;

		Nominal<int16_t> frame;
		Linear<float> opacity;
		Linear<float> xyscale;

		uint16_t delay;
		int16_t framestep;
		float opcstep;
	};
}