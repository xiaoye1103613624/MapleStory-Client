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

#include "DrawArgument.h"

#include "../Template/Point.h"

#ifdef USE_NX
#include <nlnx/bitmap.hpp>
#endif

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ms
{
	// Represents a single image loaded from a of game data
	class Texture
	{
	public:
		Texture() {}
		Texture(nl::node source);
		~Texture() {}

		void draw(const DrawArgument& args) const;
		void draw(const DrawArgument& args, const Range<int16_t>& vertical) const;
		void draw(const DrawArgument& args, const Range<int16_t>& vertical, const Range<int16_t>& horizontal) const;
		void shift(Point<int16_t> amount);

		bool is_valid() const;
		// 允许在布尔上下文（if (tex) / && / !）中判空，语义等同 is_valid()。
		explicit operator bool() const { return is_valid(); }
		int16_t width() const;
		int16_t height() const;
		Point<int16_t> get_origin() const;
		Point<int16_t> get_dimensions() const;

		// 读出原始 BGRA 像素。唯一能用原始像素的通道（运行期调色/逐格采样），
		// 因此 Texture 之外不需要再碰 nl::bitmap。
		// 为什么是拷贝而不是返回指针：nl::bitmap::data() 复用一块全局静态缓冲，
		// 任何一次调用都会让先前拿到的指针失效，跨帧持有必脏。
		bool copy_pixels(std::vector<uint8_t>& bgra) const;
		// 源位图身份，用于给派生像素（如调色裁片）做稳定的缓存键。
		size_t get_bitmap_id() const;

	private:
		nl::bitmap bitmap;
		Point<int16_t> origin;
		Point<int16_t> dimensions;
	};
}