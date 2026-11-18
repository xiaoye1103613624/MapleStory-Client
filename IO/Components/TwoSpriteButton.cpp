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
#include "TwoSpriteButton.h"

namespace ms
{
	TwoSpriteButton::TwoSpriteButton(nl::node nsrc, nl::node ssrc, Point<int16_t> np, Point<int16_t> sp) : textures(ssrc, nsrc), npos(np), spos(sp)
	{
		state = Button::State::NORMAL;
		active = true;
	}

	TwoSpriteButton::TwoSpriteButton(nl::node nsrc, nl::node ssrc, Point<int16_t> pos) : TwoSpriteButton(nsrc, ssrc, pos, pos) {}
	TwoSpriteButton::TwoSpriteButton(nl::node nsrc, nl::node ssrc) : TwoSpriteButton(nsrc, ssrc, Point<int16_t>()) {}
	TwoSpriteButton::TwoSpriteButton() : textures({}, {}) {}

	void TwoSpriteButton::set_position(Point<int16_t> pos)
	{
		// 同步基类字段（其它代码可能读 Button::position）与本类绘制/命中实际使用的
		// npos/spos。Slider::setvertical 调它把上下箭头搬到新轨道端点，之前只搬了基类
		// 字段，draw 读 npos/spos 纹丝不动 → 箭头钉死在旧位置。
		Button::set_position(pos);
		npos = pos;
		spos = pos;
	}

	void TwoSpriteButton::draw(Point<int16_t> parentpos) const
	{
		if (active)
		{
			bool selected = state == Button::State::MOUSEOVER || state == Button::State::PRESSED;

			if (selected)
				textures[selected].draw(spos + parentpos);
			else
				textures[selected].draw(npos + parentpos);
		}
	}

	Rectangle<int16_t> TwoSpriteButton::bounds(Point<int16_t> parentpos) const
	{
		bool selected = state == Button::State::MOUSEOVER || state == Button::State::PRESSED;
		Point<int16_t> absp;
		Point<int16_t> dim;

		if (selected)
		{
			absp = parentpos + spos - textures[selected].get_origin();
			dim = textures[selected].get_dimensions();
		}
		else
		{
			absp = parentpos + npos - textures[selected].get_origin();
			dim = textures[selected].get_dimensions();
		}

		return Rectangle<int16_t>(absp, absp + dim);
	}

	int16_t TwoSpriteButton::width() const
	{
		bool selected = state == Button::State::MOUSEOVER || state == Button::State::PRESSED;

		return textures[selected].width();
	}

	Point<int16_t> TwoSpriteButton::origin() const
	{
		bool selected = state == Button::State::MOUSEOVER || state == Button::State::PRESSED;

		return textures[selected].get_origin();
	}
}