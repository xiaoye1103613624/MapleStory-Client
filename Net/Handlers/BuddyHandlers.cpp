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

#include "../../Character/BuddyListModel.h"
#include "../../IO/UI.h"
#include "../../IO/UITypes/UIUserList.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UINotice.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Net/Packets/BuddyPackets.h"
#include "../../Util/Misc.h"

namespace ms
{
	void BuddyHandler::handle(InPacket& recv) const
	{
		auto ensure_sm = [](Color::Name color, const std::string& tip) {
			auto mt = (color == Color::Name::RED) ? UIChatBar::MessageType::RED
				: (color == Color::Name::WHITE) ? UIChatBar::MessageType::WHITE
				: UIChatBar::MessageType::YELLOW;
			if (auto chatbar = UI::get().get_element<UIChatBar>())
			{
				chatbar->show_message(tip.c_str(), mt);
				return;
			}
		};

		uint8_t mode = recv.read_byte();

		// P0 / T1：仿 GuildModel R28-B2 —— 写 BuddyListModel；r3 后 UIUserList
		// 每帧 update() 直读模型，handler 只在收包后触发一次按钮态刷新。
		auto refresh_userlist = []()
		{
			if (auto ul = UI::get().get_element<UIUserList>())
				ul->refresh_from_models();
		};

		switch (mode)
		{
			case 7: // updateBuddylist — 写模型，不强开浮窗
			{
				int16_t count = recv.read_byte();
				BuddyListData data;
				data.capacity = BuddyListModel::get().data().capacity;
				if (data.capacity <= 0)
					data.capacity = 25;
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
				BuddyListModel::get().set(data);
				refresh_userlist();
				while (recv.available())
					recv.skip_byte();
				(void)count;
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
				while (recv.available())
					recv.skip_byte();
				std::string msg = nameFrom + " wants to be your buddy. Accept?";
				UI::get().emplace<UIYesNo>(msg, [chrIdFrom, nameFrom](bool yes)
				{
					if (yes)
					{
						BuddyAcceptPacket(chrIdFrom).dispatch();
						LOG(LOG_DEBUG, "BuddyHandler: accept from=" << nameFrom);
					}
					else
					{
						BuddyDeletePacket(chrIdFrom).dispatch();
						LOG(LOG_DEBUG, "BuddyHandler: deny from=" << nameFrom);
					}
				});
				break;
			}
			case 0x14: // updateBuddyChannel
			{
				int32_t characterid = recv.read_int();
				recv.read_byte();
				int32_t channel = recv.read_int();
				while (recv.available())
					recv.skip_byte();
				BuddyListModel::get().update_channel(characterid, channel);
				refresh_userlist();
				(void)characterid; (void)channel;
				break;
			}
			case 0x15: // updateBuddyCapacity
			{
				int32_t capacity = recv.read_byte();
				while (recv.available())
					recv.skip_byte();
				BuddyListModel::get().set_capacity(capacity);
				refresh_userlist();
				ensure_sm(Color::Name::YELLOW,
					"Buddy slots -> " + std::to_string(capacity));
				(void)capacity;
				break;
			}
			default:
			{
				LOG(LOG_DEBUG, "Buddy mode=" << static_cast<int>(mode)
					<< " rem=" << recv.available());
				while (recv.available())
					recv.skip_byte();
				static const char* fails[] = {
					nullptr, nullptr, nullptr,
					"Buddy deleted",
					nullptr, nullptr, nullptr,
					nullptr, nullptr,
					nullptr,
					"Buddy added",
					"Already buddies",
					"Buddy list full",
					"Cannot find character",
					"Buddy request pending"
				};
				const char* msg = (mode < 15) ? fails[mode] : nullptr;
				std::string tip = msg ? msg : ("Buddy mode " + std::to_string(mode));
				if (msg)
					ensure_sm(Color::Name::YELLOW, tip);
				else if (mode != 0)
					ensure_sm(Color::Name::YELLOW, tip);
				break;
			}
		}
	}
}
