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
#include "NameTag.h"

#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdlib>

namespace ms
{
	namespace
	{
		// clr is an ARGB int, but the deployed pack keeps at least one id in string
		// form (NameTag.img/178 -> '-4096') and nl::node::get_integer() throws on a
		// non-numeric string, so the type is queried before the value is read.
		Color read_argb(nl::node clr, bool& valid)
		{
			valid = false;

			uint32_t argb = 0;

			switch (clr.data_type())
			{
				case nl::node::type::integer:
				case nl::node::type::real:
				{
					argb = static_cast<uint32_t>(clr.get_integer() & 0xFFFFFFFF);
					valid = true;
					break;
				}
				case nl::node::type::string:
				{
					const std::string text = clr.get_string();
					char* end = nullptr;

					errno = 0;

					if (const long long value = std::strtoll(text.c_str(), &end, 10);
						errno == 0 && end != text.c_str())
					{
						argb = static_cast<uint32_t>(value & 0xFFFFFFFF);
						valid = true;
					}

					break;
				}
				default:
					break;
			}

			// ARGB, not the RGBA order Color(uint32_t) expects.
			return valid
				? Color(
					static_cast<uint8_t>((argb >> 16) & 0xFF),
					static_cast<uint8_t>((argb >> 8) & 0xFF),
					static_cast<uint8_t>(argb & 0xFF),
					static_cast<uint8_t>((argb >> 24) & 0xFF))
				: Color(1.0f, 1.0f, 1.0f, 1.0f);
		}

		// Places a slice so that its box starts at (x, y) no matter which origin the
		// asset carries: NameTag.img hangs w at origin.x == width (it is authored to
		// dangle left of its draw point), e and c at origin.x == 0.
		void draw_slice(const Texture& slice, int16_t x, int16_t y, int16_t stretch_width)
		{
			if (!slice)
				return;

			const Point<int16_t> origin = slice.get_origin();

			slice.draw(DrawArgument(
				Point<int16_t>(x + origin.x(), y + origin.y()),
				Point<int16_t>(stretch_width, 0)));
		}

		// The body is seamless (its column alpha is constant), so it is repeated at
		// close to natural width instead of stretching one copy across a long name.
		void draw_body(const Texture& body, int16_t x, int16_t y, int16_t width)
		{
			const int16_t body_width = body.width();

			if (body_width <= 0 || width <= 0)
				return;

			const int16_t tiles = std::max<int16_t>(1,
				static_cast<int16_t>(std::lround(static_cast<double>(width) / body_width)));
			const int16_t tile = width / tiles;

			for (int16_t i = 0, drawn = 0; i < tiles; i++)
			{
				const int16_t this_tile = i + 1 == tiles
					? static_cast<int16_t>(width - drawn)
					: tile;

				draw_slice(body, static_cast<int16_t>(x + drawn), y, this_tile);

				drawn += this_tile;
			}
		}
	}

	nl::node NameTag::frame_node(int32_t id, Group group)
	{
		nl::node root = nl::nx::UI["NameTag.img"];

		switch (group)
		{
			case Group::MEDAL:
				root = root["medal"];
				break;
			case Group::PET:
				root = root["pet"];
				break;
			default:
				// medal/ and pet/ carry their own id space (medal/0 and medal/1 exist),
				// so the 3..254 window only guards the top-level group.
				if (id < MIN_ID || id > MAX_ID)
				{
					UiAssetResolve::diag_once("nametag id " + std::to_string(id),
						"id outside NameTag.img range [3,254], label only");
					return nl::node();
				}
				break;
		}

		nl::node frame = root[std::to_string(id)];

		if (!frame)
		{
			UiAssetResolve::diag_once("nametag node " + std::to_string(static_cast<uint32_t>(group)) + "/" + std::to_string(id),
				"NameTag.img has no such frame, label only");
		}

		return frame;
	}

	NameTag::NameTag(Text::Font font, std::string n)
		: name(OutlinedText(font, Text::Alignment::CENTER, Color::Name::EAGLE, Color::Name::JAMBALAYA))
		, label(1.0f, 1.0f, 1.0f, 1.0f)
		, tinted(false)
		, selected(false)
	{
		name.change_text(n);
	}

	NameTag::NameTag(int32_t id, Group group, Text::Font font, std::string n)
		: NameTag(font, n)
	{
		if (load_v83(frame_node(id, group), false))
			frames[true] = frames[false];
	}

	NameTag::NameTag(nl::node src, Text::Font font, std::string n)
		: NameTag(font, n)
	{
		if (!src)
		{
			UiAssetResolve::diag_once("nametag null node", "NameTag(nl::node): no source node, label only");
			return;
		}

		const nl::node unselected = src["0"];

		if (unselected["0"] && unselected["1"] && unselected["2"])
		{
			// pre-v83 <state>/<0|1|2> triple, still what Login.img/CharSelect/nameTag ships
			load_triple(unselected, false);
			load_triple(src["1"], true);

			if (!frames[true].valid())
				frames[true] = frames[false];
		}
		else if (load_v83(src, false))
		{
			frames[true] = frames[false];
		}
	}

	bool NameTag::load_v83(nl::node src, bool sel)
	{
		Frame& frame = frames[sel];

		frame.left = Texture(src["w"]);
		frame.mid = Texture(src["e"]);
		frame.right = Texture(src["c"]);

		if (!frame.valid())
		{
			frame = Frame();

			UiAssetResolve::diag_once("nametag slices " + src.name(),
				"NameTag.img/" + src.name() + " has no usable {w,e,c}, label only");
			return false;
		}

		bool valid = false;
		const Color argb = read_argb(src["clr"], valid);

		if (valid)
		{
			label = argb;
			tinted = true;

			// Text only accepts the fixed Color::Name table: keep the glyphs WHITE and
			// multiply the frame's exact ARGB in through the DrawArgument.
			name.inner.change_color(Color::Name::WHITE);
			name.l.change_color(Color::Name::WHITE);
			name.r.change_color(Color::Name::WHITE);
			name.t.change_color(Color::Name::WHITE);
			name.b.change_color(Color::Name::WHITE);
		}

		return true;
	}

	bool NameTag::load_triple(nl::node src, bool sel)
	{
		Frame& frame = frames[sel];

		frame.left = Texture(src["0"]);
		frame.mid = Texture(src["1"]);
		frame.right = Texture(src["2"]);

		if (!frame.valid())
			frame = Frame();

		return frame.valid();
	}

	void NameTag::draw(Point<int16_t> position) const
	{
		const Frame& frame = frames[selected];
		const int16_t text_width = name.width();

		if (!frame.valid())
		{
			draw_label(position);
			return;
		}

		// horizontal: the label is CENTER aligned, so its first glyph sits at
		// position.x - width/2 (GraphicsGL::LayoutBuilder::add_line).
		const int16_t text_left = static_cast<int16_t>(position.x() - text_width / 2);

		// vertical: the band GraphicsGL uses for Text::Background::NAMETAG
		// (top = y + 6, height = layout height - 2), with each slice centered on it -
		// the three slices of one id are not necessarily the same height.
		const int16_t band_top = static_cast<int16_t>(position.y() + 6);
		const int16_t band_height = std::max<int16_t>(static_cast<int16_t>(name.height() - 2), 1);

		const auto centered = [band_top, band_height](const Texture& slice)
		{
			return static_cast<int16_t>(band_top + (band_height - slice.height()) / 2);
		};

		draw_slice(frame.left, static_cast<int16_t>(text_left - frame.left.width()), centered(frame.left), 0);
		draw_body(frame.mid, text_left, centered(frame.mid), text_width);
		draw_slice(frame.right, static_cast<int16_t>(text_left + text_width), centered(frame.right), 0);

		draw_label(position);
	}

	void NameTag::draw_label(Point<int16_t> position) const
	{
		if (!tinted)
		{
			name.draw(position);
			return;
		}

		const Color outline(label.r() * 0.25f, label.g() * 0.25f, label.b() * 0.25f, label.a());

		name.l.draw(DrawArgument(position + Point<int16_t>(-1, 0), outline));
		name.r.draw(DrawArgument(position + Point<int16_t>(1, 0), outline));
		name.t.draw(DrawArgument(position + Point<int16_t>(0, -1), outline));
		name.b.draw(DrawArgument(position + Point<int16_t>(0, 1), outline));
		name.inner.draw(DrawArgument(position, label));
	}

	void NameTag::set_selected(bool s)
	{
		selected = s;

		// a frame that ships its own ARGB owns the label color
		if (tinted)
			return;

		name.change_color(s ? Color::Name::WHITE : Color::Name::EAGLE);
	}

	bool NameTag::has_frame() const
	{
		return frames[selected].valid();
	}

	int16_t NameTag::frame_width() const
	{
		const Frame& frame = frames[selected];

		if (!frame.valid())
			return name.width();

		return static_cast<int16_t>(frame.left.width() + name.width() + frame.right.width());
	}

	int16_t NameTag::frame_height() const
	{
		const Frame& frame = frames[selected];

		return frame.valid() ? frame.height() : name.height();
	}
}
