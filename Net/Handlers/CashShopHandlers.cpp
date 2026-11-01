//////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////
#include "CashShopHandlers.h"

#include "Helpers/CashShopParser.h"

#include "../../Character/Inventory/InventoryType.h"
#include "../../Configuration.h"
#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"
#include "../../IO/UIStateCashShop.h"
#include "../../IO/UITypes/UICashGachapon.h"
#include "../../IO/UITypes/UICashShop.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIItemInventory.h"
#include "../../IO/UITypes/UIMiscShells.h"
#include "../../IO/UITypes/UIStorage.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../IO/Window.h"
#include "../../Graphics/Color.h"
#include "../../Util/Misc.h"

#include <algorithm>
#include <string>

namespace ms
{
	namespace {
		void cs_tip(const std::string& tip)
		{
			// 商城 / CashGacha UI 已开：刷 meta 或不 tip；否则 chat 优先，再 SM
			if (auto ui = UI::get().get_element<UICashShop>())
			{
				if (ui->is_active())
				{
					ui->refresh_cs_meta();
					return;
				}
			}
			if (auto gacha = UI::get().get_element<UICashGachapon>())
			{
				if (gacha->is_active())
					return;
			}
			if (auto chatbar = UI::get().get_element<UIChatBar>())
			{
				chatbar->show_message(tip.c_str(), UIChatBar::MessageType::YELLOW);
				return;
			}
		}
	}

	void QueryCashResultHandler::handle(InPacket& recv) const
	{
		// s9 PacketCreator.showCash：三币种 int
		int32_t nx_credit = recv.read_int();
		int32_t maple_point = recv.read_int();
		int32_t nx_prepaid = recv.read_int();

		UIStateCashShop::cash_nx_credit = nx_credit;
		UIStateCashShop::cash_maple_point = maple_point;
		UIStateCashShop::cash_nx_prepaid = nx_prepaid;
		UIStateCashShop::cash_balances_ready = true;

		// 同步本地点券缓存，供背包 Point 标签等非 CashShop 态读取
		if (maple_point >= 0)
			Setting<MaplePointsLocal>::get().save(static_cast<uint64_t>(maple_point));

		LOG(LOG_DEBUG, "QueryCashResult: NX=" << nx_credit
			<< " Maple=" << maple_point
			<< " Prepaid=" << nx_prepaid);
		while (recv.available())
			recv.skip_byte();
		if (auto ui = UI::get().get_element<UICashShop>())
			ui->refresh_cs_meta();
		// 余额 UI 已刷，进商城不 tip
	}

	void CashShopOperationHandler::skip_cash_item_info(InPacket& recv)
	{
		// s9 addCashItemInformation（非 gift）：long + 2×int + itemId + sn + short qty
		// + 13B giftFrom + long expire + long 0
		recv.skip(8);   // cashId / pet / ring
		recv.skip(4);   // accountId
		recv.skip(4);   // 0
		recv.skip(4);   // itemId
		recv.skip(4);   // sn
		recv.skip(2);   // quantity
		recv.skip(13);  // giftFrom padded
		recv.skip(8);   // expiration
		recv.skip(8);   // 0
	}

	void CashShopOperationHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.read_byte();

		switch (mode)
		{
			case 0x4B: // showCashInventory：short count + items + storageSlots + charSlots
			{
				int16_t count = recv.read_short();
				if (count < 0) count = 0;
				if (count > 200) count = 200;
				UIStateCashShop::cash_inventory.clear();
				UIStateCashShop::cash_inventory.reserve(static_cast<size_t>(count));
				for (int16_t i = 0; i < count; i++)
				{
					CashInvEntry e;
					e.cash_id = recv.available() >= 8 ? recv.read_long() : 0;
					if (recv.available() >= 8) { recv.skip(4); recv.skip(4); }
					e.itemid = recv.available() >= 4 ? recv.read_int() : 0;
					e.sn = recv.available() >= 4 ? recv.read_int() : 0;
					e.qty = recv.available() >= 2 ? recv.read_short() : 0;
					if (recv.available() >= 13) recv.skip(13);
					if (recv.available() >= 8) recv.skip(8);
					if (recv.available() >= 8) recv.skip(8);
					UIStateCashShop::cash_inventory.push_back(e);
				}
				UIStateCashShop::cash_storage_slots = recv.available() >= 2 ? recv.read_short() : 0;
				UIStateCashShop::cash_char_slots = recv.available() >= 2 ? recv.read_short() : 0;
				LOG(LOG_DEBUG, "CashShopOp: inventory count=" << count
					<< " storage=" << UIStateCashShop::cash_storage_slots
					<< " chars=" << UIStateCashShop::cash_char_slots);
				if (auto ui = UI::get().get_element<UICashShop>())
					ui->refresh_cs_meta();
				while (recv.available())
					recv.skip_byte();
				break;
			}
			case 0x4D: // showGifts：short count + gift cashItemInfo
			{
				// gift：long + itemId + 13B from + 73B msg（无 account/sn/qty/expire）
				int16_t count = recv.read_short();
				if (count < 0) count = 0;
				if (count > 100) count = 100;
				UIStateCashShop::cash_gifts.clear();
				UIStateCashShop::cash_gifts.reserve(static_cast<size_t>(count));
				for (int16_t i = 0; i < count; i++)
				{
					CashGiftEntry g;
					g.cash_id = recv.available() >= 8 ? recv.read_long() : 0;
					g.itemid = recv.available() >= 4 ? recv.read_int() : 0;
					if (recv.available() >= 13)
					{
						char from[14] = {};
						for (int k = 0; k < 13 && recv.available(); ++k)
							from[k] = static_cast<char>(recv.read_byte());
						g.from = from;
					}
					if (recv.available() >= 73) recv.skip(73);
					UIStateCashShop::cash_gifts.push_back(std::move(g));
				}
				LOG(LOG_DEBUG, "CashShopOp: gifts count=" << count);
				if (auto ui = UI::get().get_element<UICashShop>())
					ui->refresh_cs_meta();
				while (recv.available())
					recv.skip_byte();
				break;
			}
			case 0x4F: // showWishList
			case 0x55: // updateWishList
			{
				for (int i = 0; i < 10; i++)
					UIStateCashShop::cash_wishlist[static_cast<size_t>(i)] =
						recv.available() >= 4 ? recv.read_int() : 0;
				LOG(LOG_DEBUG, "CashShopOp: wishlist mode=0x" << std::hex << (int)(uint8_t)mode);
				if (auto ui = UI::get().get_element<UICashShop>())
					ui->refresh_cs_meta();
				while (recv.available())
					recv.skip_byte();
				break;
			}
			case 0x57: // showBoughtCashItem — 写入 cash_inventory 缓存
			{
				CashInvEntry e;
				e.cash_id = recv.available() >= 8 ? recv.read_long() : 0;
				if (recv.available() >= 8) { recv.skip(4); recv.skip(4); }
				e.itemid = recv.available() >= 4 ? recv.read_int() : 0;
				e.sn = recv.available() >= 4 ? recv.read_int() : 0;
				e.qty = recv.available() >= 2 ? recv.read_short() : 0;
				if (recv.available() >= 13) recv.skip(13);
				if (recv.available() >= 8) recv.skip(8);
				if (recv.available() >= 8) recv.skip(8);
				UIStateCashShop::cash_inventory.push_back(e);
				LOG(LOG_DEBUG, "CashShopOp: bought item=" << e.itemid << " sn=" << e.sn);
				while (recv.available())
					recv.skip_byte();
				cs_tip("Bought #" + std::to_string(e.itemid));
				break;
			}
			case 0x5C: // showCashShopMessage
			{
				int8_t msg = recv.read_byte();
				LOG(LOG_DEBUG, "CashShopOp: message=0x" << std::hex << (int)(uint8_t)msg);
				while (recv.available())
					recv.skip_byte();
				// 已开 CS 时 cs_tip 会 refresh；未开才 SM
				cs_tip("CS msg 0x" + std::to_string(static_cast<int>(static_cast<uint8_t>(msg))));
				break;
			}
			case 0x5E: // showGiftSucceed：string to + itemId + short count + int price
			{
				std::string to = recv.read_string();
				int32_t itemid = recv.read_int();
				int16_t count = recv.read_short();
				int32_t price = recv.read_int();
				LOG(LOG_DEBUG, "CashShopOp: giftOk to=" << to
					<< " item=" << itemid << " x" << count << " price=" << price);
				while (recv.available())
					recv.skip_byte();
				// 送礼成功：记录到 gifts（from=本地，to 在 tip），并刷 CS meta
				{
					CashGiftEntry g;
					g.itemid = itemid;
					g.from = to; // 接收方名暂存 from 槽，便于 UI 列表展示
					UIStateCashShop::cash_gifts.push_back(g);
				}
				(void)count; (void)price;
				cs_tip("Gift → " + to + " #" + std::to_string(itemid));
				break;
			}
			case 0x60: // bought inv slots：byte type + short slots
			{
				int8_t type = recv.read_byte();
				int16_t slots = recv.read_short();
				LOG(LOG_DEBUG, "CashShopOp: invSlots type=" << (int)type << " slots=" << slots);
				while (recv.available())
					recv.skip_byte();
				InventoryType::Id inv = InventoryType::by_value(type);
				Stage::get().get_player().get_inventory().set_slotmax(inv,
					static_cast<uint8_t>(slots > 0 ? (slots > 255 ? 255 : slots) : 0));
				if (auto invui = UI::get().get_element<UIItemInventory>())
				{
					if (invui->is_active())
					{
						invui->makeactive();
						break; // 背包已开：格子自显，不 tip
					}
				}
				cs_tip("Inv slots → " + std::to_string(slots));
				break;
			}
			case 0x62: // bought storage slots
			{
				int16_t slots = recv.read_short();
				UIStateCashShop::cash_storage_slots = slots;
				LOG(LOG_DEBUG, "CashShopOp: storageSlots=" << slots);
				while (recv.available())
					recv.skip_byte();
				if (auto st = UI::get().get_element<UIStorage>())
				{
					if (st->is_active())
						break; // 仓库已开：格数自显，不 tip
				}
				cs_tip("Storage slots → " + std::to_string(slots));
				break;
			}
			case 0x64: // bought char slots（s9 showBoughtCharacterSlot）
			{
				int16_t slots = recv.read_short();
				UIStateCashShop::cash_char_slots = slots;
				LOG(LOG_DEBUG, "CashShopOp: charSlots=" << slots);
				while (recv.available())
					recv.skip_byte();
				cs_tip("Char slots → " + std::to_string(slots));
				break;
			}
			case 0x68: // takeFromCashInventory — short pos + cashItemInfo
			{
				int16_t pos = recv.read_short();
				CashInvEntry e;
				e.cash_id = recv.available() >= 8 ? recv.read_long() : 0;
				if (recv.available() >= 8) { recv.skip(4); recv.skip(4); }
				e.itemid = recv.available() >= 4 ? recv.read_int() : 0;
				e.sn = recv.available() >= 4 ? recv.read_int() : 0;
				e.qty = recv.available() >= 2 ? recv.read_short() : 0;
				if (recv.available() >= 13) recv.skip(13);
				if (recv.available() >= 8) recv.skip(8);
				if (recv.available() >= 8) recv.skip(8);
				auto& inv = UIStateCashShop::cash_inventory;
				if (pos >= 0 && static_cast<size_t>(pos) < inv.size())
					inv.erase(inv.begin() + pos);
				else if (e.cash_id != 0)
				{
					inv.erase(std::remove_if(inv.begin(), inv.end(),
						[&](const CashInvEntry& x) { return x.cash_id == e.cash_id; }),
						inv.end());
				}
				LOG(LOG_DEBUG, "CashShopOp: takeFromCash pos=" << pos << " id=" << e.itemid);
				while (recv.available())
					recv.skip_byte();
				if (auto ui = UI::get().get_element<UICashShop>())
					ui->refresh_cs_meta();
				break;
			}
			case 0x6A: // putIntoCashInventory — cashItemInfo
			{
				CashInvEntry e;
				e.cash_id = recv.available() >= 8 ? recv.read_long() : 0;
				if (recv.available() >= 8) { recv.skip(4); recv.skip(4); }
				e.itemid = recv.available() >= 4 ? recv.read_int() : 0;
				e.sn = recv.available() >= 4 ? recv.read_int() : 0;
				e.qty = recv.available() >= 2 ? recv.read_short() : 0;
				if (recv.available() >= 13) recv.skip(13);
				if (recv.available() >= 8) recv.skip(8);
				if (recv.available() >= 8) recv.skip(8);
				UIStateCashShop::cash_inventory.push_back(e);
				LOG(LOG_DEBUG, "CashShopOp: putIntoCash id=" << e.itemid);
				while (recv.available())
					recv.skip_byte();
				if (auto ui = UI::get().get_element<UICashShop>())
					ui->refresh_cs_meta();
				break;
			}
			case 0x6C: // deleteCashItem — long cashId
			{
				int64_t cash_id = recv.read_long();
				auto& inv = UIStateCashShop::cash_inventory;
				inv.erase(std::remove_if(inv.begin(), inv.end(),
					[&](const CashInvEntry& x) { return x.cash_id == cash_id; }),
					inv.end());
				LOG(LOG_DEBUG, "CashShopOp: deleteCash id=" << cash_id);
				while (recv.available())
					recv.skip_byte();
				if (auto ui = UI::get().get_element<UICashShop>())
					ui->refresh_cs_meta();
				break;
			}
			case 0x85: // refundCashItem — long cashId + int maplePoints
			{
				int64_t cash_id = recv.read_long();
				int32_t pts = recv.read_int();
				auto& inv = UIStateCashShop::cash_inventory;
				inv.erase(std::remove_if(inv.begin(), inv.end(),
					[&](const CashInvEntry& x) { return x.cash_id == cash_id; }),
					inv.end());
				if (pts > 0)
				{
					UIStateCashShop::cash_maple_point += pts;
					if (UIStateCashShop::cash_maple_point < 0)
						UIStateCashShop::cash_maple_point = 0;
					Setting<MaplePointsLocal>::get().save(
						static_cast<uint64_t>(UIStateCashShop::cash_maple_point));
				}
				LOG(LOG_DEBUG, "CashShopOp: refund id=" << cash_id << " maple=" << pts
					<< " bal=" << UIStateCashShop::cash_maple_point);
				while (recv.available())
					recv.skip_byte();
				cs_tip("CS refund +" + std::to_string(pts));
				break;
			}
			case 0x87: // showBoughtCashRing：cashItemInfo + string + itemId + short qty
			{
				CashInvEntry e;
				e.cash_id = recv.available() >= 8 ? recv.read_long() : 0;
				if (recv.available() >= 8) { recv.skip(4); recv.skip(4); }
				e.itemid = recv.available() >= 4 ? recv.read_int() : 0;
				e.sn = recv.available() >= 4 ? recv.read_int() : 0;
				e.qty = recv.available() >= 2 ? recv.read_short() : 0;
				if (recv.available() >= 13) recv.skip(13);
				if (recv.available() >= 8) recv.skip(8);
				if (recv.available() >= 8) recv.skip(8);
				UIStateCashShop::cash_inventory.push_back(e);
				std::string recipient = recv.available() ? recv.read_string() : "";
				int32_t itemid = recv.available() >= 4 ? recv.read_int() : e.itemid;
				int16_t qty = recv.available() >= 2 ? recv.read_short() : e.qty;
				LOG(LOG_DEBUG, "CashShopOp: ring to=" << recipient
					<< " item=" << itemid << " x" << qty);
				while (recv.available())
					recv.skip_byte();
				cs_tip("Ring → " + recipient);
				break;
			}
			case 0x89: // showBoughtCashPackage：byte size + N×cashItemInfo + short 0
			{
				int8_t n = recv.read_byte();
				if (n < 0) n = 0;
				if (n > 50) n = 50;
				for (int8_t i = 0; i < n; i++)
				{
					CashInvEntry e;
					e.cash_id = recv.available() >= 8 ? recv.read_long() : 0;
					if (recv.available() >= 8) { recv.skip(4); recv.skip(4); }
					e.itemid = recv.available() >= 4 ? recv.read_int() : 0;
					e.sn = recv.available() >= 4 ? recv.read_int() : 0;
					e.qty = recv.available() >= 2 ? recv.read_short() : 0;
					if (recv.available() >= 13) recv.skip(13);
					if (recv.available() >= 8) recv.skip(8);
					if (recv.available() >= 8) recv.skip(8);
					UIStateCashShop::cash_inventory.push_back(e);
				}
				if (recv.available() >= 2)
					recv.read_short();
				LOG(LOG_DEBUG, "CashShopOp: package items=" << (int)n);
				while (recv.available())
					recv.skip_byte();
				cs_tip("CS package x" + std::to_string(n));
				break;
			}
			case 0x8D: // showBoughtQuestItem
			{
				recv.read_int();   // 1
				recv.read_short(); // 1
				recv.read_byte();  // 0x0B
				recv.read_byte();  // 0
				int32_t itemid = recv.read_int();
				CashInvEntry e;
				e.itemid = itemid;
				e.qty = 1;
				UIStateCashShop::cash_inventory.push_back(e);
				LOG(LOG_DEBUG, "CashShopOp: questItem=" << itemid);
				while (recv.available())
					recv.skip_byte();
				cs_tip("Quest item #" + std::to_string(itemid));
				break;
			}
			case 0x9E: // showNameChangeSuccess
			case 0xA0: // showWorldTransferSuccess
			{
				CashInvEntry e;
				e.cash_id = recv.available() >= 8 ? recv.read_long() : 0;
				if (recv.available() >= 8) { recv.skip(4); recv.skip(4); }
				e.itemid = recv.available() >= 4 ? recv.read_int() : 0;
				e.sn = recv.available() >= 4 ? recv.read_int() : 0;
				e.qty = recv.available() >= 2 ? recv.read_short() : 0;
				if (recv.available() >= 13) recv.skip(13);
				if (recv.available() >= 8) recv.skip(8);
				if (recv.available() >= 8) recv.skip(8);
				UIStateCashShop::cash_inventory.push_back(e);
				LOG(LOG_DEBUG, "CashShopOp: name/world ok mode=0x" << std::hex << (int)(uint8_t)mode
					<< std::dec << " item=" << e.itemid);
				while (recv.available())
					recv.skip_byte();
				cs_tip(mode == static_cast<int8_t>(0x9E) ? "Name change ok" : "World transfer ok");
				break;
			}
			default:
			{
				LOG(LOG_DEBUG, "CashShopOp: unhandled mode=0x" << std::hex << (int)(uint8_t)mode
					<< std::dec << " rem=" << recv.available());
				while (recv.available())
					recv.skip_byte();
				// 未知 CS 模式：已开刷 meta；否则 tip
				cs_tip("CS mode 0x" + std::to_string(static_cast<int>(static_cast<uint8_t>(mode))));
				break;
			}
		}
	}

	void CashShopCheckNameChangeHandler::handle(InPacket& recv) const
	{
		std::string name = recv.read_string();
		bool in_use = recv.read_bool();
		LOG(LOG_DEBUG, "CashShopCheckName: name=" << name << " inUse=" << in_use);
		while (recv.available())
			recv.skip_byte();
		cs_tip(in_use ? (name + " in use") : (name + " available"));
	}

	void CashShopNameChangePossibleHandler::handle(InPacket& recv) const
	{
		recv.read_int();
		int8_t error = recv.read_byte();
		recv.read_int();
		LOG(LOG_DEBUG, "CashShopNamePossible: error=" << (int)error);
		while (recv.available())
			recv.skip_byte();
		cs_tip(error ? ("Name change err " + std::to_string(error)) : "Name change possible");
	}

	void CashShopTransferWorldPossibleHandler::handle(InPacket& recv) const
	{
		recv.read_int();
		int8_t error = recv.read_byte();
		recv.read_int();
		bool list = recv.read_bool();
		if (list && error == 0)
		{
			int32_t n = recv.read_int();
			if (n < 0) n = 0;
			if (n > 32) n = 32;
			for (int32_t i = 0; i < n; i++)
				recv.read_string();
		}
		LOG(LOG_DEBUG, "CashShopTransferWorld: error=" << (int)error << " list=" << list);
		while (recv.available())
			recv.skip_byte();
		cs_tip(error ? ("World transfer err " + std::to_string(error)) : "World transfer ok");
	}

	void CashItemGachaponResultHandler::handle(InPacket& recv) const
	{
		// s9 onCashItemGachaponOpenFailed(0xE4) / onCashGachaponOpenSuccess(0xE5)
		int8_t sub = recv.read_byte();

		if (sub == static_cast<int8_t>(0xE4))
		{
			LOG(LOG_DEBUG, "CashItemGacha: open failed");
			while (recv.available())
				recv.skip_byte();
			auto ui = UI::get().get_element<UICashGachapon>();
			if (!ui)
				ui = UI::get().emplace<UICashGachapon>();
			else if (!ui->is_active())
				ui->makeactive();
			if (ui)
				ui->set_failed();
			// Gacha UI 已显失败，不 tip
			return;
		}
		if (sub == static_cast<int8_t>(0xE5))
		{
			auto ui = UI::get().get_element<UICashGachapon>();
			if (!ui)
				ui = UI::get().emplace<UICashGachapon>();
			else if (!ui->is_active())
				ui->makeactive();
			int64_t box_id = recv.read_long();
			int32_t remaining = recv.read_int();
			// 解析奖品进 cash_inventory（对齐 showBoughtCashItem）
			CashInvEntry e;
			e.cash_id = recv.available() >= 8 ? recv.read_long() : 0;
			if (recv.available() >= 8) { recv.skip(4); recv.skip(4); }
			e.itemid = recv.available() >= 4 ? recv.read_int() : 0;
			e.sn = recv.available() >= 4 ? recv.read_int() : 0;
			e.qty = recv.available() >= 2 ? recv.read_short() : 0;
			if (recv.available() >= 13) recv.skip(13);
			if (recv.available() >= 8) recv.skip(8);
			if (recv.available() >= 8) recv.skip(8);
			if (e.itemid != 0)
				UIStateCashShop::cash_inventory.push_back(e);
			int32_t reward_id = recv.available() >= 4 ? recv.read_int() : e.itemid;
			int8_t qty = recv.available() ? recv.read_byte() : static_cast<int8_t>(e.qty);
			bool jackpot = recv.available() ? recv.read_bool() : false;
			LOG(LOG_DEBUG, "CashItemGacha: box=" << box_id
				<< " left=" << remaining
				<< " reward=" << reward_id
				<< " x" << (int)qty
				<< " jackpot=" << jackpot
				<< " cashId=" << e.cash_id);
			if (auto cs = UI::get().get_element<UICashShop>())
				cs->refresh_cs_meta();
			while (recv.available())
				recv.skip_byte();
			if (ui)
				ui->set_result(reward_id, qty, remaining, jackpot);
			else
				cs_tip((jackpot ? "JACKPOT #" : "Gacha #") + std::to_string(reward_id)
					+ " x" + std::to_string(qty));
			return;
		}
		LOG(LOG_DEBUG, "CashItemGacha: sub=0x" << std::hex << (int)(uint8_t)sub);
		while (recv.available())
			recv.skip_byte();
		cs_tip("Cash Gacha 0x" + std::to_string(static_cast<int>(static_cast<uint8_t>(sub))));
	}

	void SetCashShopHandler::handle(InPacket& recv) const
	{
		CashShopParser::parseCharacterInfo(recv);

		recv.skip_byte();	// Not MTS
		recv.skip_string();	// account_name
		recv.skip_int();

		int16_t specialcashitem_size = recv.read_short();

		for (size_t i = 0; i < specialcashitem_size; i++)
		{
			recv.skip_int();	// sn
			recv.skip_int();	// mod
			recv.skip_byte();	// info
		}

		recv.skip(121);

		for (size_t cat = 1; cat <= 8; cat++)
		{
			for (size_t gender = 0; gender < 2; gender++)
			{
				for (size_t in = 0; in < 5; in++)
				{
					recv.skip_int(); // category
					recv.skip_int(); // gender
					recv.skip_int(); // commoditysn
				}
			}
		}

		// 主商品列表（getCSPacket: writeShort(count) + 11 字段 commodity）
		int16_t commodity_count = recv.read_short();
		UIStateCashShop::cashshop_commodities.clear();
		if (commodity_count > 0 && commodity_count < 4000)
		{
			for (int16_t i = 0; i < commodity_count; i++)
			{
				CashCommodity c;
				int32_t itemid = recv.read_int();
				int32_t price = recv.read_int();
				int32_t sn = recv.read_int();
				recv.read_int();   // period
				recv.read_short(); // gender
				recv.read_bool();  // onSale
				recv.read_int();   // price again
				recv.read_int();   // period again
				recv.read_int();   // serial2
				recv.read_int();   // stock
				recv.read_int();   // 0
				c.sn = sn;
				c.itemid = itemid;
				c.price = price;
				c.count = 1;
				UIStateCashShop::cashshop_commodities.push_back(c);
			}
		}
		else
		{
			recv.skip(commodity_count * 44); // 容错：count 异常时尽量跳过
		}

		while (recv.available())
			recv.skip_byte();

		transition();

		UI::get().change_state(UI::State::CASHSHOP);
	}

	void SetCashShopHandler::transition() const
	{
		Constants::Constants::get().set_viewwidth(1024);
		Constants::Constants::get().set_viewheight(768);

		float fadestep = 0.025f;

		Window::get().fadeout(
			fadestep,
			[]()
			{
				GraphicsGL::get().clear();

				Stage::get().load(-1, 0);

				UI::get().enable();
				Timer::get().start();
				GraphicsGL::get().unlock();
			}
		);

		GraphicsGL::get().lock();
		Stage::get().clear();
		Timer::get().start();
	}
}