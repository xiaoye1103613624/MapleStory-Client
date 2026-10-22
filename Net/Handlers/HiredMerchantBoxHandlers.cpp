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
#include "HiredMerchantBoxHandlers.h"

namespace ms
{
	// spawnHiredMerchantBox: int ownerId, int itemId, short x, short y, short 0, string owner, byte, byte[] {1,4}
	void HiredMerchantSpawnHandler::handle(InPacket& recv) const
	{
		recv.skip_int();   // ownerId
		recv.skip_int();   // itemId
		recv.skip_short(); // x
		recv.skip_short(); // y
		recv.skip_short(); // 0
		recv.skip_string(); // owner
		recv.skip_byte();  // itemId % 100
		recv.skip(2);      // {1, 4}
	}

	// removeHiredMerchantBox: int id
	void HiredMerchantDestroyHandler::handle(InPacket& recv) const
	{
		recv.skip_int(); // id
	}

	// updateHiredMerchantBox: int ownerId, byte 5, int objectId, string description, byte itemId%100, byte[] roomInfo
	void HiredMerchantUpdateHandler::handle(InPacket& recv) const
	{
		recv.skip_int();   // ownerId
		recv.skip_byte();  // 5
		recv.skip_int();   // objectId
		recv.skip_string(); // description
		recv.skip_byte();  // itemId % 100
		recv.skip(2);      // roomInfo {visitor capacity}
	}
}
