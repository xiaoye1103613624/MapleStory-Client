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
#include "Obj.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	Obj::Obj(nl::node src)
	{
		nl::node obj_src = nl::nx::Map["Obj"][src["oS"] + ".img"][src["l0"]][src["l1"]][src["l2"]];
		// Login Title logo: keep a0/a1 fade from Map Obj, swap bitmap to UI MSTitle.
		bool login_logo = src["oS"].get_string() == "login"
			&& src["l0"].get_string() == "Title"
			&& src["l1"].get_string() == "logo";
		nl::node mstitle = login_logo ? nl::nx::UI["Login.img"]["Title"]["MSTitle"] : nl::node();

		if (login_logo && mstitle)
			animation = Animation(obj_src, mstitle);
		else
			animation = Animation(obj_src);

		pos = Point<int16_t>(src["x"], src["y"]);
		flip = src["f"].get_bool();
		z = src["z"];
	}

	void Obj::update()
	{
		animation.update();
	}

	void Obj::draw(Point<int16_t> viewpos, float inter) const
	{
		animation.draw(DrawArgument(pos + viewpos, flip), inter);
	}

	uint8_t Obj::getz() const
	{
		return z;
	}
}