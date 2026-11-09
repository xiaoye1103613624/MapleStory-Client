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

	// 严格按 s9 PacketCreator.addItemInfo(p, item, /*zeroPosition=*/true) 解析。
	// 三处旧版错位：PET 块是 35 字节（不是 37）；EQUIP 尾部还有 5 个 short 潜能行
	// （095 追加 10 字节）；普通道具的 quantity 要作为结果返回。
	static void parse_iteminfo(InPacket& recv, int32_t& itemid_out, int16_t& quantity_out)
	{
		quantity_out = 1;
		uint8_t itemType = recv.read_byte();
		int32_t itemid = recv.read_int();
		itemid_out = itemid;

		bool isCash = recv.read_bool();
		if (isCash)
			recv.skip(8); // cash id / pet id / ring id (long)
		recv.skip(8);     // expiration (long)

		if (itemid >= 5000000 && itemid <= 5000102) // PET
		{
			// 13 name + 1 level + 2 tameness + 1 fullness + 8 expiration
			// + 2 attribute + 2 skill + 4 remainLife + 2 attribute = 35
			recv.skip(35);
			return;
		}
		if (itemType == 1) // EQUIP
		{
			recv.skip(2);   // upgradeSlots + level
			recv.skip(30);  // 15 shorts (str..jump)
			recv.skip_string(); // owner
			recv.skip_short();  // flag
			if (isCash)
				recv.skip(10); // 10 x 0x40
			else
				recv.skip(18); // byte + byte + int expNibble + int vicious + long
			recv.skip(8);   // getTime(-2)
			recv.skip(4);   // -1
			recv.skip(10);  // 5 x short 潜能（potential1..3 / bonus1..2）
		}
		else // 普通道具
		{
			quantity_out = recv.read_short();
			recv.skip_string(); // owner
			recv.skip_short();  // flag
			if ((itemid / 10000 == 207) || (itemid / 10000 == 233))
				recv.skip(8); // rechargeable: int 2 + 4 字节标记
		}
	}

	// 供雇佣商人商品展示：只需 itemId，数量忽略。
	static void skip_iteminfo(InPacket& recv, int32_t& itemid_out)
	{
		int16_t quantity = 1;
		parse_iteminfo(recv, itemid_out, quantity);
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
					// s9 PacketCreator.getTradeStart：
					//   ROOM(5), 3, 2, number,
					//   [number==1 ? 0 + charLook(partner) + partnerName],
					//   number, charLook(self), selfName, 0xFF
					// number=0 为发起人（此时伙伴尚未进入），1 为被邀请人。
					uint8_t number = 0;
					std::string self;
					std::string partner;
					try
					{
						if (recv.available())
							recv.read_byte(); // 2 (slotMax)
						if (recv.available())
							number = static_cast<uint8_t>(recv.read_byte());
						if (number == 1)
						{
							recv.read_byte(); // 0
							skip_charlook(recv);
							partner = recv.read_string();
						}
						recv.read_byte(); // number 重复
						skip_charlook(recv);
						self = recv.read_string();
					}
					catch (...)
					{
					}
					while (recv.available())
						recv.skip_byte();
					if (ensure_trade())
						uitrade->start_trade(number, self, partner);
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
				// 游戏设置「交易申请」拒绝 → 发 DECLINE(3) 拒绝邀请，不弹 YesNo。
				// 不能用 EXIT(0xA)：那是"退出已建立的交易"，会走 cancelTrade，语义不同。
				if (!UIGameOpt::accepts(UIGameOpt::TRADE_APPLY))
				{
					TradeDeclinePacket().dispatch();
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
						TradeDeclinePacket().dispatch();
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
			case 0x11: // getTradeConfirmation：对方按下确认并已锁定
			{
				// s9 Trade.lockTrade() 只给 partner 发这个包，所以收到它表示"对方已确认"。
				while (recv.available())
					recv.skip_byte();
				if (ensure_trade())
					uitrade->set_partner_locked();
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
			case 0x0A: // getTradeResult：EXIT(0xA) + number + operation
			{
				uint8_t number = static_cast<uint8_t>(recv.read_byte());
				uint8_t op = static_cast<uint8_t>(recv.read_byte());
				while (recv.available())
					recv.skip_byte();
				if (ensure_trade())
					uitrade->result(number, op);

				// s9 Trade.TradeResult：1 无响应 / 2 对方取消 / 7 成功 / 8 失败 /
				// 9 唯一道具上限 / 12 不同地图 / 13 文件损坏。旧代码把 6 当成功是错的。
				std::string tip;
				Color::Name color = Color::Name::WHITE;
				switch (op)
				{
					case 1: tip = Locale::text("trade.result.no_response"); break;
					case 2: tip = Locale::text("trade.result.partner_cancel"); break;
					case 7: tip = Locale::text("trade.result.success"); color = Color::Name::YELLOW; break;
					case 8: tip = Locale::text("trade.result.unsuccessful"); color = Color::Name::RED; break;
					case 9: tip = Locale::text("trade.result.unique_limit"); color = Color::Name::RED; break;
					case 12: tip = Locale::text("trade.result.another_map"); color = Color::Name::RED; break;
					case 13: tip = Locale::text("trade.result.damaged"); color = Color::Name::RED; break;
					default: tip = Locale::format("trade.result.unknown", std::to_string(op)); break;
				}
				ensure_sm(color, tip);
				break;
			}
			case 6: // getTradeChat：CHAT(6) + CHAT_THING(8) + byte owner + "name : msg"
			{
				recv.read_byte(); // 8
				recv.read_byte(); // owner: 0 = 自己 / 1 = 对方（s9 写的是 byte 不是 bool）
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
		// 与雇佣商人共用 addItemInfo(zeroPosition=true) 的解析，避免两处布局漂移：
		// 旧实现漏跳 cashId、装备潜能尾部 10 字节，且宠物块按 37 字节跳（实为 35）。
		parse_iteminfo(recv, itemid, quantity);
	}
}
