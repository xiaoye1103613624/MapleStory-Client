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

#include "../../Graphics/SpecialText.h"
#include "../../Graphics/Texture.h"
#include "../../Template/BoolPair.h"

#include <cstdint>
#include <string>

namespace ms
{
	// W12: name tag backed by the deployed pack's UI/NameTag.img.
	//
	// NameTag.img/<id> is a three-slice frame:
	//   w   left cap   (origin.x == width: authored to hang left of its draw point)
	//   e   body       (horizontally repeatable)
	//   c   right cap
	//   clr ARGB label color (int, sometimes the string form such as '-4096')
	//
	// Same layout under NameTag.img/medal/<id> and NameTag.img/pet/<id>.
	class NameTag
	{
	public:
		enum class Group : uint8_t
		{
			NONE,
			MEDAL,
			PET
		};

		// ids actually present in NameTag.img (measured against the deployed UI.nx)
		static constexpr int32_t MIN_ID = 3;
		static constexpr int32_t MAX_ID = 254;

		// Resolves NameTag.img/[medal|pet/]<id>; null node when the id is out of
		// range or missing, so callers can degrade to a frameless label.
		static nl::node frame_node(int32_t id, Group group);

		// id based entry (NPC / mob / pet / medal labels).
		NameTag(int32_t id, Group group, Text::Font font, std::string name);

		// Frame-node entry: src already points at a frame group. Accepts both the
		// v83 {w,e,c,clr} layout and the older <state>/<0|1|2> triple used by
		// Login.img/CharSelect/nameTag.
		NameTag(nl::node src, Text::Font font, std::string name);

		void draw(Point<int16_t> position) const;
		void set_selected(bool selected);

		bool has_frame() const;
		// label width + left cap + right cap
		int16_t frame_width() const;
		int16_t frame_height() const;

	private:
		struct Frame
		{
			Texture left;
			Texture mid;
			Texture right;

			bool valid() const
			{
				return static_cast<bool>(left && mid && right);
			}

			int16_t height() const
			{
				// the three slices of one id are not necessarily the same height
				int16_t tallest = left.height();

				if (mid.height() > tallest)
				{
					tallest = mid.height();
				}

				if (right.height() > tallest)
				{
					tallest = right.height();
				}

				return tallest;
			}
		};

		NameTag(Text::Font font, std::string name);

		// {w,e,c} + optional clr of one frame group
		bool load_v83(nl::node src, bool selected);
		// <0|1|2> triple of one state
		bool load_triple(nl::node src, bool selected);
		void draw_label(Point<int16_t> position) const;

		OutlinedText name;
		BoolPair<Frame> frames;
		Color label;
		bool tinted;
		bool selected;
	};
}
