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
#include "../../IO/UITypes/UIMerchant.h"
#include "../../Net/Packets/TradePackets.h"
#include "../../Net/Packets/PlayerInteractionPackets.h"
#include "../../Template/EnumMap.h"

namespace ms
{
	// 跳过 s9 addCharLook(p, visitor, false) 块（gender/skin/face/bool/hair + 装备 + 现金武器 + 宠物）。
	static void skip_charlook(InPacket& recv)
	{
		recv.skip_byte(); // gender
		recv.skip_byte(); // skin
		recv.skip_int();  // face
		recv.skip_byte(); // !mega (bool)
		recv.skip_int();  // hair
		while (recv.available() && recv.read_byte() != -1)
			recv.skip_int(); // myEquip: byte pos + int id, 以 0xFF 结束
		while (recv.available() && recv.read_byte() != -1)
			recv.skip_int(); // maskedEquip: byte pos + int id, 以 0xFF 结束
		recv.skip_int(); // cash weapon (-111)
		recv.skip_int();
		recv.skip_int();
		recv.skip_int(); // 3 pets
	}

	// 跳过 s9 addItemInfo(item, true)（zeroPosition）并取出 itemId，供雇佣商人商品展示。
	static void skip_iteminfo(InPacket& recv, int32_t& itemid_out)
	{
		uint8_t itemType = recv.read_byte();
		int32_t itemid = recv.read_int();
		itemid_out = itemid;

		bool isCash = recv.read_bool();
		if (isCash)
			recv.skip(8); // cash id (long)
		recv.skip(8);     // expiration time (long)

		if (itemType == 1) // EQUIP
		{
			recv.skip(2);  // upgradeSlots + level
			recv.skip(30); // 15 shorts
			recv.skip_string(); // owner
			recv.skip_short();  // flag
			if (isCash)
				recv.skip(10);
			else
				recv.skip(18); // byte + byte + int + int + long
			recv.skip(8);  // getTime(-2)
			recv.skip(4);  // -1
		}
		else if (itemid >= 5000000 && itemid <= 5000102) // PET
		{
			recv.skip(37); // 宠物块（固定串13 + 等级/亲密度/饱食 + 到期 + 属性等）
		}
		else // 普通道具
		{
			recv.skip_short();  // quantity
			recv.skip_string(); // owner
			recv.skip_short();  // flag
			if ((itemid / 10000 == 233) || (itemid / 10000 == 207))
				recv.skip(8); // rechargeable 附加
		}
	}

	void TradeHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.read_byte();

		auto uitrade = UI::get().get_element<UITrade>();
		if (!uitrade)
			uitrade = UI::get().emplace<UITrade>();

		switch (mode)
		{
			case 5: // getTradeStart(交易) / getHiredMerchant(雇佣商人) 共用 action 5，靠后续字节区分
			{
				uint8_t sub = recv.read_byte();
				if (sub == 0x05) // 雇佣商人开张：0x05, 0x04, ...
				{
					recv.read_byte();  // 0x04
					recv.read_short(); // visitorSlot + 1
					recv.read_int();   // itemId
					recv.read_string(); // "Hired Merchant"

					// 访客（最多 3，遇 0xFF 结束）
					while (recv.available())
					{
						int8_t v = recv.read_byte();
						if (v == -1)
							break;
						skip_charlook(recv);
						recv.read_string();
					}

					recv.skip_short();      // msgList 长度（访客为 0）
					std::string owner = recv.read_string();
					recv.read_string();     // description
					recv.skip_byte();       // 0x10 (slots)
					recv.skip_int();        // meso
					uint8_t items_size = recv.read_byte();

					std::vector<UIMerchant::MerchItem> items;
					if (items_size == 0)
					{
						recv.read_byte(); // 末尾补齐 byte 0
					}
					else
					{
						for (uint8_t i = 0; i < items_size && recv.available(); i++)
						{
							int16_t bundles = recv.read_short();
							int16_t qty = recv.read_short();
							int32_t price = recv.read_int();
							int32_t itemid = 0;
							skip_iteminfo(recv, itemid);
							items.push_back({ itemid, qty, price, bundles });
						}
					}

					auto uimerchant = UI::get().get_element<UIMerchant>();
					if (!uimerchant)
						uimerchant = UI::get().emplace<UIMerchant>();
					uimerchant->set_owner(owner);
					uimerchant->set_items(items);
				}
				else // 交易开始（sub == 3）
				{
					while (recv.available())
						recv.skip_byte();
					uitrade->start_trade();
				}
				break;
			}
			case 0x19: // updateHiredMerchant（刷新商品/金币，无 charLook、无 owner 分支，格式干净）
			{
				recv.skip_int(); // meso
				uint8_t items_size = recv.read_byte();
				std::vector<UIMerchant::MerchItem> items;
				for (uint8_t i = 0; i < items_size && recv.available(); i++)
				{
					int16_t bundles = recv.read_short();
					int16_t qty = recv.read_short();
					int32_t price = recv.read_int();
					int32_t itemid = 0;
					skip_iteminfo(recv, itemid);
					items.push_back({ itemid, qty, price, bundles });
				}
				auto uimerchant = UI::get().get_element<UIMerchant>();
				if (!uimerchant)
					uimerchant = UI::get().emplace<UIMerchant>();
				uimerchant->set_items(items);
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
