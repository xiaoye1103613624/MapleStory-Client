////////////////////////////////////////////////////////////////////////////////
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
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.	//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	class OutPacket;

	// 制造出站包：Opcode MAKER_SKILL(0x71)，对接 s9 MakerProcessor(type=1 创建)
	class MakerCreatePacket : public OutPacket
	{
	public:
		MakerCreatePacket(int32_t toCreate) : OutPacket(OutPacket::Opcode::MAKER_SKILL)
		{
			write_int(1);        // type = create
			write_int(toCreate); // toCreate = 结果道具 itemId（s9 MakerProcessor.getItemCreateEntry(toCreate)）

			// 装备类配方服务端还会读 stimulant + reagents；此处占位(无 stimulant / 无 reagent)
			if ((toCreate / 1000000) == 1)
			{
				write_byte(0);
				write_int(0);
			}
		}
	};
}
