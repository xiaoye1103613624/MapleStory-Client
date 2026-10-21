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

#include <algorithm>
#include <cstdlib>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	static Point<int16_t> sanitize_origin(Point<int16_t> origin, Point<int16_t> dimensions)
	{
		// BeiDou .nxcache / 个别 img 向量会给出远超画布的 origin（如 1156,8531），
		// 导致整页 UI 飘走、像「没背景 / 双框 / 卷轴错位」。超出合理范围则归零。
		const int32_t lim_x = std::max<int32_t>(64, std::abs(dimensions.x()) * 2 + 64);
		const int32_t lim_y = std::max<int32_t>(64, std::abs(dimensions.y()) * 2 + 64);

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
}