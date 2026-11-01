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
#include "MapleButton.h"

#include "../../Graphics/Texture.h"
#include "../../Util/UiAssetResolve.h"

namespace ms
{
	Point<int16_t> maple_btn_nopos()
	{
		return Point<int16_t>(-32768, -32768);
	}

	bool try_maple_btn(std::map<uint16_t, std::unique_ptr<Button>>& buttons, uint16_t id, nl::node src,
		Point<int16_t> position, const std::string& site)
	{
		if (!UiAssetResolve::usable(src))
			return false;

		if (position.x() == -32768 && position.y() == -32768)
		{
			UiAssetResolve::diag_once("maplebtn no-position " + site + " id=" + std::to_string(id),
				"node=" + src.name() + " -> skipped instead of stacking at (0,0)");
			return false;
		}

		Point<int16_t> origin;

		if (UiAssetResolve::usable(src["normal"]["0"]))
			origin = Texture(src["normal"]["0"]).get_origin();

		buttons[id] = std::make_unique<MapleButton>(
			src,
			Point<int16_t>(static_cast<int16_t>(position.x() + origin.x()), static_cast<int16_t>(position.y() + origin.y()))
		);
		return true;
	}

	MapleButton::MapleButton(nl::node src, Point<int16_t> pos)
	{
		nl::node normal = src["normal"];

		if (normal.size() > 1)
			animations[Button::State::NORMAL] = normal;
		else
			textures[Button::State::NORMAL] = normal["0"];

		textures[Button::State::PRESSED] = src["pressed"]["0"];
		textures[Button::State::MOUSEOVER] = src["mouseOver"]["0"];
		textures[Button::State::DISABLED] = src["disabled"]["0"];

		active = true;
		position = pos;
		state = Button::State::NORMAL;
	}

	MapleButton::MapleButton(nl::node src, int16_t x, int16_t y) : MapleButton(src, Point<int16_t>(x, y)) {}
	MapleButton::MapleButton(nl::node src) : MapleButton(src, Point<int16_t>()) {}

	void MapleButton::draw(Point<int16_t> parentpos) const
	{
		if (active)
		{
			textures[state].draw(position + parentpos);
			animations[state].draw(position + parentpos, 1.0f);
		}
	}

	void MapleButton::update()
	{
		if (active)
			animations[state].update(6);
	}

	Rectangle<int16_t> MapleButton::bounds(Point<int16_t> parentpos) const
	{
		Point<int16_t> lt;
		Point<int16_t> rb;

		if (textures[state].is_valid())
		{
			lt = parentpos + position - textures[state].get_origin();
			rb = lt + textures[state].get_dimensions();
		}
		else
		{
			lt = parentpos + position - animations[state].get_origin();
			rb = lt + animations[state].get_dimensions();
		}

		return Rectangle<int16_t>(lt, rb);
	}

	int16_t MapleButton::width() const
	{
		return textures[state].width();
	}

	Point<int16_t> MapleButton::origin() const
	{
		return textures[state].get_origin();
	}
}