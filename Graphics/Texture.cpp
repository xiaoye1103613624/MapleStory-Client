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
#include "Texture.h"

#include "GraphicsGL.h"
#include "TintContext.h"

#include <algorithm>
#include <cstdlib>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	static Point<int16_t> sanitize_origin(Point<int16_t> origin, Point<int16_t> dimensions)
	{
		// 仅过滤明显损坏的向量（nxcache/脏 img 曾见 1156,8531）。
		// 不可按「画布尺寸×2」截断：192 WorldMapSearch/notice origin=(-28,-221)、
		// StatusBar3 Menu ox≈-174 等是合法的父节点相对落点；旧阈值会把提示文案
		// 打回输入框旁、菜单钮叠死。下限 1024 仍能拦掉千级脏数据。
		const int32_t lim_x = std::max<int32_t>(1024, std::abs(dimensions.x()) * 8 + 256);
		const int32_t lim_y = std::max<int32_t>(1024, std::abs(dimensions.y()) * 8 + 256);

		if (std::abs(static_cast<int32_t>(origin.x())) > lim_x
			|| std::abs(static_cast<int32_t>(origin.y())) > lim_y)
		{
			return Point<int16_t>(0, 0);
		}

		return origin;
	}

	Texture::Texture(nl::node src)
	{
		if (src.data_type() == nl::node::type::bitmap)
		{
			if (src.root() == nl::nx::Map001)
			{
				const std::string& _outlink = src["_outlink"];

				if (!_outlink.empty())
				{
					size_t first = _outlink.find_first_of('/');

					if (first != std::string::npos)
					{
						const std::string& first_part = _outlink.substr(0, first);

						if (first_part == "Map")
						{
							const std::string& path = _outlink.substr(first + 1);
							nl::node foundOutlink = nl::nx::Map.resolve(path);

							if (foundOutlink)
								src = foundOutlink;
						}
					}
				}
			}

			bitmap = src;
			dimensions = Point<int16_t>(bitmap.width(), bitmap.height());
			origin = sanitize_origin(src["origin"], dimensions);
			GraphicsGL::get().addbitmap(bitmap);
		}
	}

	void Texture::draw(const DrawArgument& args) const
	{
		draw(args, Range<int16_t>(0, 0));
	}

	void Texture::draw(const DrawArgument& args, const Range<int16_t>& vertical) const
	{
		draw(args, vertical, Range<int16_t>(0, 0));
	}

	void Texture::draw(const DrawArgument& args, const Range<int16_t>& vertical, const Range<int16_t>& horizontal) const
	{
		if (!is_valid())
			return;

		// ColorPrism: draw-time per-character tint. Shared Clothing can't carry a baked
		// colour, so ask the active draw context for this frame's HSL and redirect to a
		// cached recoloured raw-atlas id. No tint pushed => original path, zero change.
		TintCache::HSL hsl;
		if (TintContext::get().resolve_hsl(bitmap.id(), hsl))
		{
			const size_t raw = TintCache::get().tinted(*this, hsl);
			if (raw)
			{
				GraphicsGL::get().draw_raw_ex(
					raw,
					args.get_rectangle(origin, dimensions),
					vertical,
					horizontal,
					args.get_color(),
					args.get_angle()
				);
				return;
			}
		}

		GraphicsGL::get().draw(
			bitmap,
			args.get_rectangle(origin, dimensions),
			vertical,
			horizontal,
			args.get_color(),
			args.get_angle()
		);
	}

	void Texture::shift(Point<int16_t> amount)
	{
		origin -= amount;
	}

	bool Texture::is_valid() const
	{
		return bitmap.id() > 0;
	}

	int16_t Texture::width() const
	{
		return dimensions.x();
	}

	int16_t Texture::height() const
	{
		return dimensions.y();
	}

	Point<int16_t> Texture::get_origin() const
	{
		return origin;
	}

	Point<int16_t> Texture::get_dimensions() const
	{
		return dimensions;
	}

	bool Texture::copy_pixels(std::vector<uint8_t>& bgra) const
	{
		bgra.clear();

		if (!is_valid())
			return false;

		const void* src = bitmap.data();

		if (!src)
			return false;

		// bitmap::length() is 4 * width * height; the atlas upload path uses the
		// same buffer as GL_BGRA, so this is the format contract of the render layer.
		const size_t bytes = static_cast<size_t>(bitmap.length());

		if (bytes == 0)
			return false;

		const uint8_t* first = static_cast<const uint8_t*>(src);

		bgra.assign(first, first + bytes);
		return true;
	}

	size_t Texture::get_bitmap_id() const
	{
		return bitmap.id();
	}
}