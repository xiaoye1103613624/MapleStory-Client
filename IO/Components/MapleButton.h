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

#include "Button.h"

#include <map>
#include <memory>
#include <string>

namespace ms
{
	// A standard MapleStory button with 4 states and a texture for each state
	class MapleButton : public Button
	{
	public:
		MapleButton(nl::node src, Point<int16_t> position);
		MapleButton(nl::node src, int16_t x, int16_t y);
		MapleButton(nl::node src);

		void draw(Point<int16_t> position) const;
		void update();
		Rectangle<int16_t> bounds(Point<int16_t> parentpos) const;
		int16_t width() const;
		Point<int16_t> origin() const;
		Cursor::State send_cursor(bool, Point<int16_t>) { return Cursor::State::IDLE; }

	private:
		Texture textures[Button::State::NUM_STATES];
		Animation animations[Button::State::NUM_STATES];
	};

	// Sentinel meaning "the caller never said where this button goes".
	// Historically every such button fell back to (0,0) and stacked on top of the
	// window title, which reads as "button missing / button in the wrong corner".
	// Skipping + logging makes the missing layout data visible instead.
	Point<int16_t> maple_btn_nopos();

	/// Returns true when the button was created. Missing asset or missing
	/// position => not created, plus one diagnostic line per distinct site.
	///
	/// `position` is the sprite's **top-left**, not its anchor: MapleButton treats
	/// its own position as the anchor (draw = pos - origin), so the normal-state
	/// origin is added back here. Layout tables can then be read straight off
	/// screenshots and asset extents.
	bool try_maple_btn(std::map<uint16_t, std::unique_ptr<Button>>& buttons, uint16_t id, nl::node src,
		Point<int16_t> position = maple_btn_nopos(), const std::string& site = std::string());
}