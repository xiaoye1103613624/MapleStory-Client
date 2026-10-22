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
#include "TradeHandlers.h"

#include "../../Character/EquipStat.h"
#include "../../IO/UI.h"
#include "../../IO/UITypes/UITrade.h"
#include "../../Net/Packets/TradePackets.h"
#include "../../Template/EnumMap.h"

namespace ms
{
	void TradeHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.read_byte();

		auto uitrade = UI::get().get_element<UITrade>();
		if (!uitrade)
			uitrade = UI::get().emplace<UITrade>();

		switch (mode)
		{
			case 5: // getTradeStart（含 charLook，整体跳过）
			{
				while (recv.available())
					recv.skip_byte();
				uitrade->start_trade();
				break;
			}
			case 4: // getTradePartnerAdd（含 charLook，整体跳过）
			{
				recv.read_byte(); // 1
				while (recv.available())
					recv.skip_byte();
				break;
			}
			case 2: // tradeInvite（收到交易请求，自动接受）
			{
				recv.read_byte(); // 3
				std::string name = recv.read_string();
				recv.skip(4);
				uitrade->partner_joined(name);
				TradeVisitPacket().dispatch();
				break;
			}
			case 0x10: // getTradeMesoSet
			{
				uint8_t number = recv.read_byte();
				int32_t meso = recv.read_int();
				uitrade->set_meso(number, meso);
				break;
			}
			case 0x0F: // getTradeItemAdd
			{
				uint8_t number = recv.read_byte();
				uint8_t itemPosition = recv.read_byte();
				int32_t itemid;
				int16_t quantity;
				parse_trade_item(recv, itemid, quantity);
				uitrade->add_item(number, itemPosition, itemid, quantity);
				break;
			}
			case 0x11: // getTradeConfirmation（锁定）
			{
				uitrade->set_locked();
				break;
			}
			case 0x0A: // getTradeResult
			{
				recv.read_byte(); // number
				uint8_t op = recv.read_byte();
				uitrade->result(0, op);
				break;
			}
			case 6: // getTradeChat
			{
				recv.read_byte(); // 8
				recv.read_bool();
				std::string msg = recv.read_string();
				uitrade->add_chat(msg);
				break;
			}
			default:
				break;
		}
	}

	void TradeHandler::parse_trade_item(InPacket& recv, int32_t& itemid, int16_t& quantity) const
	{
		uint8_t type = recv.read_byte();
		int32_t iid = recv.read_int();
		itemid = iid;

		if (type == 1) // EQUIP
		{
			recv.read_bool();
			recv.read_long();
			recv.read_byte();
			recv.read_byte();
			EnumMap<EquipStat::Id, uint16_t> stats;
			for (auto& s : stats)
				s.second = recv.read_short();
			recv.read_string();
			recv.read_short();
			recv.skip(8);
			quantity = 1;
		}
		else if (iid >= 5000000 && iid <= 5000102)
		{
			recv.read_bool();
			recv.read_long();
			recv.read_padded_string(13);
			recv.read_byte();
			recv.read_short();
			recv.read_byte();
			recv.skip(18);
			quantity = 1;
		}
		else
		{
			recv.read_bool();
			recv.read_long();
			int16_t qty = recv.read_short();
			recv.read_string();
			recv.read_short();
			if ((iid / 10000 == 233) || (iid / 10000 == 207))
				recv.skip(8);
			quantity = qty;
		}
	}
}
