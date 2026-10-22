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
#include "BuddyHandlers.h"

#include "../../IO/UI.h"
#include "../../IO/UITypes/UIBuddyList.h"
#include "../../Net/Packets/BuddyPackets.h"

namespace ms
{
	void BuddyHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.read_byte();

		auto uibuddy = UI::get().get_element<UIBuddyList>();
		if (!uibuddy)
			uibuddy = UI::get().emplace<UIBuddyList>();

		switch (mode)
		{
			case 7: // updateBuddylist
			{
				int16_t count = recv.read_byte();
				BuddyListData data;
				data.capacity = uibuddy->get_capacity();
				for (int16_t i = 0; i < count; i++)
				{
					Buddy b;
					b.cid = recv.read_int();
					b.name = recv.read_padded_string(13);
					recv.read_byte(); // opposite status
					b.channel = recv.read_int();
					b.group = recv.read_padded_string(13);
					recv.read_int();   // mapid
					b.online = (b.channel != -2 && b.channel != -1);
					data.buddies.push_back(b);
				}
				for (int16_t i = 0; i < count; i++)
					recv.read_int(); // 尾部 mapid 块
				uibuddy->update_buddies(data);
				break;
			}
			case 9: // requestBuddylistAdd（收到好友请求）
			{
				int32_t chrIdFrom = recv.read_int();
				std::string nameFrom = recv.read_string();
				recv.read_int(); // chrId
				recv.read_padded_string(11);
				recv.read_byte();
				recv.read_byte();
				recv.read_byte();
				recv.read_int();
				recv.read_padded_string(13); // group
				recv.read_byte();
				recv.read_int(); // chrId
				// 自动接受（v1 最小可用）；拒绝逻辑留待后续
				BuddyAcceptPacket(chrIdFrom).dispatch();
				break;
			}
			case 0x14: // updateBuddyChannel
			{
				int32_t characterid = recv.read_int();
				recv.read_byte();
				int32_t channel = recv.read_int();
				uibuddy->update_buddy_channel(characterid, channel);
				break;
			}
			case 0x15: // updateBuddyCapacity
			{
				int32_t capacity = recv.read_byte();
				uibuddy->set_capacity(capacity);
				break;
			}
			default: // buddylistMessage 等状态字节
				break;
		}
	}
}
