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
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	// 现金商城购买包：Opcode CASHSHOP_OPERATION(325)，与 s9 CashOperationHandler action 0x03 对齐。
	// 服务端读取顺序：byte action(0x03) -> byte(占位 0) -> int useNX -> int sn。
	class CashShopBuyPacket : public OutPacket
	{
	public:
		CashShopBuyPacket(int32_t useNX, int32_t sn) : OutPacket(OutPacket::Opcode::CASHSHOP_OPERATION)
		{
			write_byte(0x03);
			write_byte(0);
			write_int(useNX);
			write_int(sn);
		}
	};
}
