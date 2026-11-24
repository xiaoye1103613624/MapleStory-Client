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

#include "../../Template/BoolPair.h"

namespace ms
{
	class TwoSpriteButton : public Button
	{
	public:
		TwoSpriteButton(nl::node nsrc, nl::node ssrc, Point<int16_t> npos, Point<int16_t> spos);
		TwoSpriteButton(nl::node nsrc, nl::node ssrc, Point<int16_t> position);
		TwoSpriteButton(nl::node nsrc, nl::node ssrc);
		TwoSpriteButton();

		// Slider::setvertical 改轨道两端时用本方法挪上下箭头。基类 Button::set_position
		// 只改 Button::position，而本类 draw()/bounds() 读的是 npos/spos —— 必须覆盖同步。
		void set_position(Point<int16_t> position) override;
		void draw(Point<int16_t> position) const;
		void update() {}
		Rectangle<int16_t> bounds(Point<int16_t> position) const;
		int16_t width() const;
		Point<int16_t> origin() const;
		Cursor::State send_cursor(bool, Point<int16_t>) { return Cursor::State::IDLE; }

	private:
		BoolPair<Texture> textures;
		Point<int16_t> npos;
		Point<int16_t> spos;
	};
}