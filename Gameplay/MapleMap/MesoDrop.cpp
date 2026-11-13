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
#include "MesoDrop.h"

namespace ms
{
	MesoDrop::MesoDrop(int32_t oid, int32_t owner, Point<int16_t> start, Point<int16_t> dest, int8_t type, int8_t mode, bool pd, const Animation& icn, int8_t grade) : Drop(oid, owner, start, dest, type, mode, pd, grade), icon(icn) {}

	void MesoDrop::draw(double viewx, double viewy, float alpha) const
	{
		if (!active)
			return;

		Point<int16_t> absp = phobj.get_absolute(viewx, viewy, alpha);

		// 金币的服务端 grade 恒为 0（getDropItemGrade 里 meso 直接 return 0），
		// 因此这里的两层绘制实际是空操作；保留调用是为了与 ItemDrop 同构，
		// 将来若要给金币加「大额闪光」不必再改结构。
		draw_aura_back(absp, alpha);
		icon.draw({ angle.get(alpha), absp, opacity.get(alpha) }, alpha);
		draw_aura_front(absp, alpha);
	}
}