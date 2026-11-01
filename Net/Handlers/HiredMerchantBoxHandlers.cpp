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

#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"
#include "../../IO/UITypes/UIMiscShells.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Util/Misc.h"
#include "../../Graphics/Color.h"

#include <string>

namespace ms
{
	namespace {
	}

	// spawnHiredMerchantBox: int ownerId, int itemId, short x, short y, short 0, string owner, byte, byte[] {1,4}
	void HiredMerchantSpawnHandler::handle(InPacket& recv) const
	{
		int32_t ownerId = recv.read_int();
		int32_t itemId = recv.read_int();
		int16_t x = recv.read_short();
		int16_t y = recv.read_short();
		recv.skip_short(); // 0
		std::string owner = recv.read_string();
		recv.skip_byte();  // itemId % 100
		recv.skip(2);      // {1, 4}
		while (recv.available())
			recv.skip_byte();
		const int32_t oid = ownerId ^ 0x30000000;
		const std::string label = owner.empty() ? ("Shop #" + std::to_string(itemId)) : owner;
		auto& markers = Stage::get().get_field_markers();
		if (markers.contains(oid))
			markers.set_label(oid, label);
		else
			markers.spawn(oid, FieldMarker::Kind::KITE, Point<int16_t>(x, y), label);
		// 场标已显，不 tip
		LOG(LOG_DEBUG, "HiredMerchantSpawn owner=" << owner << " item=" << itemId
			<< " (" << x << "," << y << ")");
	}

	// removeHiredMerchantBox: int id
	void HiredMerchantDestroyHandler::handle(InPacket& recv) const
	{
		int32_t id = recv.read_int();
		while (recv.available())
			recv.skip_byte();
		Stage::get().get_field_markers().remove(id ^ 0x30000000);
		LOG(LOG_DEBUG, "HiredMerchantDestroy id=" << id);
	}

	// updateHiredMerchantBox: int ownerId, byte 5, int objectId, string description, byte itemId%100, byte[] roomInfo
	void HiredMerchantUpdateHandler::handle(InPacket& recv) const
	{
		int32_t ownerId = recv.read_int();
		recv.skip_byte();  // 5
		int32_t objectId = recv.read_int();
		std::string description = recv.read_string();
		recv.skip_byte();  // itemId % 100
		recv.skip(2);      // roomInfo {visitor capacity} — 无坐标
		while (recv.available())
			recv.skip_byte();
		int32_t oid = ownerId ^ 0x30000000;
		std::string label = description.empty()
			? ("Shop#" + std::to_string(objectId)) : description;
		// 仅刷新已有场标标签；无坐标时不在玩家脚下伪造 spawn
		if (Stage::get().get_field_markers().contains(oid))
			Stage::get().get_field_markers().set_label(oid, label);
		LOG(LOG_DEBUG, "HiredMerchantUpdate oid=" << objectId << " owner=" << ownerId
			<< " desc=" << description
			<< " has=" << Stage::get().get_field_markers().contains(oid));
	}
}
