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
#include "../../IO/UITypes/UINotice.h"
#include "../../IO/UITypes/UIMerchant.h"
#include "../../IO/UITypes/UIMinigame.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIMiscShells.h"
#include "../../IO/UITypes/UIOptionMenu.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Graphics/Color.h"
#include "../../Net/Packets/TradePackets.h"
#include "../../Net/Packets/PlayerInteractionPackets.h"
#include "../../Template/EnumMap.h"
#include "../../Util/Misc.h"
#include "../../Util/Locale.h"

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

		auto uitrade = UI::get().get_element<UITrade>();
		auto ensure_trade = [&]() -> Optional<UITrade> {
			if (!uitrade)
				uitrade = UI::get().emplace<UITrade>();
			else if (!uitrade->is_active())
				uitrade->makeactive();
			return uitrade;
		};
		auto ensure_merchant = []() -> Optional<UIMerchant> {
			auto ui = UI::get().get_element<UIMerchant>();
			if (!ui) ui = UI::get().emplace<UIMerchant>();
			else if (!ui->is_active()) ui->makeactive();
			return ui;
		};

		switch (mode)
		{
			case 5: // ROOM：交易 / 个人店 / 雇佣商人 / 小游戏 / MiniRoomError 共用，靠后续字节区分
			{
				uint8_t sub = recv.read_byte();
				// s9 getMiniRoomError：ROOM + byte0 + status（15=仅自由市场等）
				if (sub == 0)
				{
					uint8_t status = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
					while (recv.available())
						recv.skip_byte();
					std::string tip;
					switch (status)
					{
						case 12: tip = Locale::text("miniroom.err_channel"); break;
						case 13: tip = Locale::text("miniroom.err_establish"); break;
						case 15: tip = Locale::text("miniroom.err_free_market"); break;
						case 17: tip = Locale::text("miniroom.err_enter"); break;
						case 18: tip = Locale::text("miniroom.err_maintenance"); break;
						default:
							tip = Locale::format("miniroom.err_generic", std::to_string(status));
							break;
					}
					LOG(LOG_DEBUG, "TradeHandler: MiniRoomError status=" << static_cast<int>(status));
					ensure_sm(Color::Name::RED, tip);
					if (auto ps = UI::get().get_element<UIPersonalShop>())
						ps->set_tip(tip);
					if (auto es = UI::get().get_element<UIEntrustedShop>())
						es->set_tip(tip);
					break;
				}
				if (sub == 1) // getMiniGame：type=1
				{
					// s9 visitMiniGame 大致：slot + charLook + name + gameType(+wins…)
					std::string tip = "Minigame room";
					uint8_t game_type = 1;
					try {
						if (recv.available())
							recv.read_byte(); // slot / first
						if (recv.available() >= 10)
							skip_charlook(recv);
						if (recv.available() >= 2) {
							std::string owner = recv.read_string();
							if (!owner.empty() && owner.size() < 32)
								tip = owner;
						}
						if (recv.available()) {
							game_type = static_cast<uint8_t>(recv.read_byte());
							if (game_type == 1) tip += " · Omok";
							else if (game_type == 2) tip += " · MatchCard";
						}
						// wins/ties/losses/points（各 int）若有则跳过
						for (int i = 0; i < 4 && recv.available() >= 4; i++)
							recv.read_int();
					} catch (...) {
						// 解析失败则吞剩余
					}
					while (recv.available())
						recv.skip_byte();
					LOG(LOG_DEBUG, "TradeHandler: ROOM minigame tip=" << tip
						<< " gtype=" << static_cast<int>(game_type));
					auto uimg = UI::get().get_element<UIMinigame>();
					if (!uimg)
						uimg = UI::get().emplace<UIMinigame>();
					else if (!uimg->is_active())
						uimg->makeactive();
					if (uimg)
						uimg->set_tip(tip);
					auto table = UI::get().get_element<UIMinigameTable>();
					if (!table)
						table = UI::get().emplace<UIMinigameTable>();
					else if (!table->is_active())
						table->makeactive();
					if (table) {
						table->set_game_kind(game_type == 2 ? 2 : 1);
						table->set_tip(tip);
					}
					break;
				}
				// s9 getPlayerShop：ROOM + 4 + 4 + (owner?0:1) + sold?/0 + look + name + visitors + FF + desc + slots + items
				if (sub == 4)
				{
					std::string owner;
					std::string desc;
					std::vector<UIMerchant::MerchItem> items;
					try {
						if (recv.available())
							recv.read_byte(); // 第二字节 4（slotMax 类）
						uint8_t number = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
						if (number == 0) {
							uint8_t sold = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
							for (uint8_t i = 0; i < sold && recv.available(); ++i) {
								recv.read_int();    // itemId
								recv.read_short();  // qty
								recv.read_int();    // mesos
								recv.read_string(); // buyer
							}
						} else if (recv.available()) {
							recv.read_byte(); // visitor path 补 0
						}
						if (recv.available() >= 10)
							skip_charlook(recv);
						if (recv.available() >= 2)
							owner = recv.read_string();
						while (recv.available()) {
							int8_t v = static_cast<int8_t>(recv.read_byte());
							if (v == -1)
								break;
							skip_charlook(recv);
							recv.read_string();
						}
						if (recv.available() >= 2)
							desc = recv.read_string();
						if (recv.available())
							recv.read_byte(); // 0x10 slots
						uint8_t items_size = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
						for (uint8_t i = 0; i < items_size && recv.available(); ++i) {
							int16_t bundles = recv.read_short();
							int16_t qty = recv.read_short();
							int32_t price = recv.read_int();
							int32_t itemid = 0;
							skip_iteminfo(recv, itemid);
							items.push_back({ itemid, qty, price, bundles });
						}
					} catch (...) {
					}
					while (recv.available())
						recv.skip_byte();
					LOG(LOG_DEBUG, "TradeHandler: ROOM PlayerShop ownerLen=" << owner.size()
						<< " items=" << items.size());
					std::string tip = owner.empty() ? Locale::text("personalshop.open")
						: Locale::format("merchant.owner_shop", owner);
					if (!desc.empty())
						tip += " · " + desc;
					// 勿 emplace PersonalShop：ctor 会再弹 SoftKey；CREATE 路径通常已开着
					if (auto ps = UI::get().get_element<UIPersonalShop>()) {
						if (!ps->is_active())
							ps->makeactive();
						ps->set_tip(tip);
					} else {
						ensure_sm(Color::Name::YELLOW, tip);
					}
					if (auto uimerchant = ensure_merchant()) {
						uimerchant->set_owner(owner.empty() ? "PlayerShop" : owner);
						uimerchant->set_items(items);
					}
					break;
				}
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

					if (auto uimerchant = ensure_merchant())
					{
						uimerchant->set_owner(owner);
						uimerchant->set_items(items);
					}
					while (recv.available())
						recv.skip_byte();
					// Merchant UI 已显，不 tip
					break;
				}
				if (sub == 3) // 交易开始（getTradeStart）
				{
					while (recv.available())
						recv.skip_byte();
					if (ensure_trade())
						uitrade->start_trade();
					// Trade UI 已开，不 SM
					break;
				}
				// 未知 ROOM 子类型：勿误开交易窗
				LOG(LOG_DEBUG, "TradeHandler: ROOM unknown sub=" << static_cast<int>(sub)
					<< " rem=" << recv.available());
				while (recv.available())
					recv.skip_byte();
				ensure_sm(Color::Name::YELLOW,
					Locale::format("miniroom.err_generic", std::to_string(sub)));
				break;
			}
			case 0x19: // updateHiredMerchant
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
				if (auto uimerchant = ensure_merchant())
					uimerchant->set_items(items);
				while (recv.available())
					recv.skip_byte();
				break;
			}
			case 4: // getTradePartnerAdd（slot + charLook + name）
			{
				if (recv.available())
					recv.read_byte(); // slot
				std::string partner;
				try {
					if (recv.available() >= 10)
						skip_charlook(recv);
					if (recv.available() >= 2)
						partner = recv.read_string();
				} catch (...) {
					partner.clear();
				}
				while (recv.available())
					recv.skip_byte();
				if (!partner.empty())
				{
					if (ensure_trade())
						uitrade->partner_joined(partner);
					// 交易窗已显搭档，不 tip
				}
				break;
			}
			case 2: // tradeInvite
			{
				recv.read_byte(); // 3
				std::string name = recv.read_string();
				recv.skip(4);
				while (recv.available())
					recv.skip_byte();
				// 游戏设置「交易申请」拒绝 → 静默 Exit，不弹 YesNo
				if (!UIGameOpt::accepts(UIGameOpt::TRADE_APPLY))
				{
					TradeExitPacket().dispatch();
					LOG(LOG_DEBUG, "TradeHandler: GameOpt refuse TRADE_APPLY from=" << name);
					break;
				}
				std::string msg = name + " wants to trade. Accept?";
				// YesNo 已显，不重复 SM
				UI::get().emplace<UIYesNo>(msg, [name](bool yes)
				{
					if (yes)
					{
						auto ui = UI::get().get_element<UITrade>();
						if (!ui) ui = UI::get().emplace<UITrade>();
						if (ui) ui->partner_joined(name);
						TradeVisitPacket().dispatch();
						LOG(LOG_DEBUG, "TradeHandler: accept trade from=" << name);
					}
					else
					{
						TradeExitPacket().dispatch();
						LOG(LOG_DEBUG, "TradeHandler: deny trade from=" << name);
					}
				});
				break;
			}
			case 0x10: // getTradeMesoSet
			{
				uint8_t number = recv.read_byte();
				int32_t meso = recv.read_int();
				while (recv.available())
					recv.skip_byte();
				if (ensure_trade())
					uitrade->set_meso(number, meso);
				(void)meso;
				break;
			}
			case 0x0F: // getTradeItemAdd
			{
				uint8_t number = recv.read_byte();
				uint8_t itemPosition = recv.read_byte();
				int32_t itemid;
				int16_t quantity;
				parse_trade_item(recv, itemid, quantity);
				while (recv.available())
					recv.skip_byte();
				if (ensure_trade())
					uitrade->add_item(number, itemPosition, itemid, quantity);
				(void)itemid; (void)quantity;
				break;
			}
			case 0x11: // getTradeConfirmation（锁定）
			{
				while (recv.available())
					recv.skip_byte();
				if (ensure_trade())
					uitrade->set_locked();
				break;
			}
			case 0x3A: // minigame READY
			{
				while (recv.available())
					recv.skip_byte();
				auto uimg = UI::get().get_element<UIMinigame>();
				if (!uimg) uimg = UI::get().emplace<UIMinigame>();
				else if (!uimg->is_active()) uimg->makeactive();
				if (uimg) uimg->set_tip("Opponent ready");
				break;
			}
			case 0x3B: // UN_READY
			{
				while (recv.available())
					recv.skip_byte();
				auto uimg = UI::get().get_element<UIMinigame>();
				if (!uimg) uimg = UI::get().emplace<UIMinigame>();
				else if (!uimg->is_active()) uimg->makeactive();
				if (uimg) uimg->set_tip("Unready");
				break;
			}
			case 0x3D: // START
			{
				while (recv.available())
					recv.skip_byte();
				auto uimg = UI::get().get_element<UIMinigame>();
				if (!uimg) uimg = UI::get().emplace<UIMinigame>();
				else if (!uimg->is_active()) uimg->makeactive();
				if (uimg) uimg->set_tip("Game start");
				auto table = UI::get().get_element<UIMinigameTable>();
				if (!table)
					table = UI::get().emplace<UIMinigameTable>();
				else if (!table->is_active())
					table->makeactive();
				if (table) {
					table->clear_board();
					table->set_tip("Playing");
				}
				break;
			}
			case 0x3E: // Omok place broadcast
			{
				uint8_t x = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
				uint8_t y = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
				uint8_t piece = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
				while (recv.available())
					recv.skip_byte();
				auto table = UI::get().get_element<UIMinigameTable>();
				if (!table) table = UI::get().emplace<UIMinigameTable>();
				else if (!table->is_active()) table->makeactive();
				if (table) {
					table->set_game_kind(1);
					table->note_place(x, y, piece);
				}
				auto uimg = UI::get().get_element<UIMinigame>();
				if (!uimg) uimg = UI::get().emplace<UIMinigame>();
				else if (!uimg->is_active()) uimg->makeactive();
				if (uimg) uimg->set_tip("Omok (" + std::to_string(x) + "," + std::to_string(y) + ")");
				break;
			}
			case 0x3F: // MatchCard select broadcast
			{
				uint8_t turn = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
				uint8_t slot = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
				while (recv.available())
					recv.skip_byte();
				std::string tip = "Card turn=" + std::to_string(turn) + " #" + std::to_string(slot);
				auto table = UI::get().get_element<UIMinigameTable>();
				if (!table) table = UI::get().emplace<UIMinigameTable>();
				else if (!table->is_active()) table->makeactive();
				if (table) {
					table->set_game_kind(2);
					table->set_tip(tip);
					break;
				}
				auto uimg = UI::get().get_element<UIMinigame>();
				if (!uimg) uimg = UI::get().emplace<UIMinigame>();
				else if (!uimg->is_active()) uimg->makeactive();
				if (uimg)
				{
					uimg->set_tip(tip);
					break;
				}
				ensure_sm(Color::Name::WHITE, tip);
				break;
			}
			case 0x0A: // getTradeResult
			{
				recv.read_byte(); // number
				uint8_t op = recv.read_byte();
				while (recv.available())
					recv.skip_byte();
				if (ensure_trade())
					uitrade->result(0, op);
				else
					ensure_sm(Color::Name::WHITE,
						op == 6 ? "Trade success" : ("Trade result " + std::to_string(op)));
				break;
			}
			case 6: // getTradeChat
			{
				recv.read_byte(); // 8
				recv.read_bool();
				std::string msg = recv.read_string();
				while (recv.available())
					recv.skip_byte();
				if (ensure_trade())
					uitrade->add_chat(msg);
				break;
			}
			default:
			{
				LOG(LOG_DEBUG, "Room mode=" << static_cast<int>(mode)
					<< " rem=" << recv.available());
				while (recv.available())
					recv.skip_byte();
				std::string tip = "Room mode " + std::to_string(mode);
				if (uitrade && uitrade->is_active())
					break; // 交易窗已开：模式自显，不 tip
				if (auto uimg = UI::get().get_element<UIMinigame>())
				{
					if (uimg->is_active())
					{
						uimg->set_tip(tip);
						break;
					}
				}
				ensure_sm(Color::Name::YELLOW, tip);
				break;
			}
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
