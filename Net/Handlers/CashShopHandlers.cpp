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
#include "../../Util/Locale.h"
#include "../../Util/Resource/ResourceService.h"

#include <nlnx/nx.hpp>

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

		// Angel NoticeFailReason 子集 → locale；未知码走 generic。
		std::string cs_fail_text(uint8_t code)
		{
			switch (code)
			{
				case 0x01: return Locale::text("cashshop.fail.timeout");
				case 0x02: return Locale::text("cashshop.fail.cash");
				case 0x04: return Locale::text("cashshop.fail.age");
				case 0x05: return Locale::text("cashshop.fail.limit");
				case 0x06: return Locale::text("cashshop.fail.same_account");
				case 0x07: return Locale::text("cashshop.fail.name");
				case 0x08: return Locale::text("cashshop.fail.gender");
				case 0x09: return Locale::text("cashshop.fail.inventory");
				case 0x0A: return Locale::text("cashshop.fail.coupon_expired");
				case 0x0B: return Locale::text("cashshop.fail.coupon_used");
				case 0x0C: return Locale::text("cashshop.fail.coupon_pcbang");
				case 0x0D: return Locale::text("cashshop.fail.stock");
				default: return Locale::format("cashshop.fail.generic", std::to_string(code));
			}
		}

		// s9 PacketCreator.writeModifiedCashItem:7447-7485 按 CommodityFlag
		// (org/gms/constants/game/CommodityFlag.java) 的 sort 序只写"非空字段"，
		// FLAG = 这些字段的 flag 位之和。此处按同一张表反解（位序取自 Java 源码，
		// 写序取自 sort 值），替换掉旧注释里凭猜测的"11 字段 / 44B 每件"读法：
		//   sort 0 SN(int) 1 FLAG(int) 2 ITEM_ID(bit0,int) 3 COUNT(bit1,short)
		//        4 PRIORITY(bit4,byte) 5 PRICE(bit2,int) 6 BONUS(bit3,byte)
		//        7 PERIOD(bit5,short) 8 MAPLE_POINT(bit6,int) 9 MESO(bit7,int)
		//        10 FOR_PREMIUM_USER(bit8,byte) 11 COMMODITY_GENDER(bit9,byte)
		//        12 ON_SALE(bit10,byte) 13 CLASS(bit11,byte) 14 LIMIT(bit12,byte)
		//        15 PB_CASH(bit13,short) 16 PB_POINT(bit14,short) 17 PB_GIFT(bit15,short)
		//        18 PACKAGE_SN(bit16, byte n + n*int)
		CashCommodity decode_modified_cash_item(InPacket& recv)
		{
			CashCommodity c;
			c.sn = recv.read_int();
			const uint32_t flag = static_cast<uint32_t>(recv.read_int());

			if (flag & (1u << 0)) c.itemid = recv.read_int();			// ITEM_ID
			if (flag & (1u << 1)) c.count = recv.read_short();			// COUNT
			if (flag & (1u << 4)) c.priority = recv.read_byte();			// PRIORITY
			if (flag & (1u << 2)) c.price = recv.read_int();			// PRICE
			if (flag & (1u << 3)) recv.read_byte();				// BONUS
			if (flag & (1u << 5)) c.period = recv.read_short();			// PERIOD
			if (flag & (1u << 6)) recv.read_int();				// MAPLE_POINT
			if (flag & (1u << 7)) recv.read_int();				// MESO
			if (flag & (1u << 8)) recv.read_byte();				// FOR_PREMIUM_USER
			if (flag & (1u << 9)) c.gender = recv.read_byte();			// COMMODITY_GENDER
			if (flag & (1u << 10)) { c.onsale = recv.read_byte() == 1; c.has_onsale = true; }	// ON_SALE
			if (flag & (1u << 11)) recv.read_byte();				// CLASS
			if (flag & (1u << 12)) recv.read_byte();				// LIMIT
			if (flag & (1u << 13)) recv.read_short();				// PB_CASH
			if (flag & (1u << 14)) recv.read_short();				// PB_POINT
			if (flag & (1u << 15)) recv.read_short();				// PB_GIFT
			if (flag & (1u << 16))
			{
				// PACKAGE_SN：0 = 非礼包；>0 后面跟 n 个子件 SN（礼包详情后续再做）
				int8_t n = recv.read_byte();
				for (int8_t i = 0; i < n; i++)
					recv.read_int();
			}

			return c;
		}

		// 在售目录基表：客户端本地 Etc/Commodity.img（实测 8947 行，OnSale==1
		// 共 2010 行）。字段与 s9 CashShop.java:139-179 loadAllCashItems 同源。
		// 排序沿用 s9 CashShopService.java:92 的 Priority 降序 → ItemId 升序。
		void load_local_commodities(std::vector<CashCommodity>& out)
		{
			nl::node catalog = res::ResourceService::get().root(res::RootId::Etc)["Commodity.img"];

			if (!catalog)
			{
				LOG(LOG_WARN, "SetCashShop: Etc/Commodity.img unavailable, catalog stays empty");
				return;
			}

			for (nl::node row : catalog)
			{
				const int32_t sn = row["SN"].get_integer();
				const int32_t itemid = row["ItemId"].get_integer();
				// OnSale 缺省 0，与 s9 CashShop.java:145 getIntConvert 默认值一致。
				if (sn <= 0 || itemid <= 0 || row["OnSale"].get_integer() != 1)
					continue;

				CashCommodity c;
				c.sn = sn;
				c.itemid = itemid;
				c.price = row["Price"].get_integer();
				c.period = row["Period"].get_integer();
				c.priority = row["Priority"].get_integer();
				c.gender = static_cast<int16_t>(row["Gender"].get_integer());
				c.count = static_cast<int16_t>(row["Count"].get_integer());
				if (c.count <= 0)
					c.count = 1;
				out.push_back(c);
			}

			std::sort(out.begin(), out.end(), [](const CashCommodity& a, const CashCommodity& b)
			{
				if (a.priority != b.priority)
					return a.priority > b.priority;
				return a.itemid < b.itemid;
			});
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
					e.cash_id = recv.length() >= 8 ? recv.read_long() : 0;
					if (recv.length() >= 8) { recv.skip(4); recv.skip(4); }
					e.itemid = recv.length() >= 4 ? recv.read_int() : 0;
					e.sn = recv.length() >= 4 ? recv.read_int() : 0;
					e.qty = recv.length() >= 2 ? recv.read_short() : 0;
					if (recv.length() >= 13) recv.skip(13);
					if (recv.length() >= 8) recv.skip(8);
					if (recv.length() >= 8) recv.skip(8);
					UIStateCashShop::cash_inventory.push_back(e);
				}
				UIStateCashShop::cash_storage_slots = recv.length() >= 2 ? recv.read_short() : 0;
				UIStateCashShop::cash_char_slots = recv.length() >= 2 ? recv.read_short() : 0;
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
					g.cash_id = recv.length() >= 8 ? recv.read_long() : 0;
					g.itemid = recv.length() >= 4 ? recv.read_int() : 0;
					if (recv.length() >= 13)
					{
						char from[14] = {};
						for (int k = 0; k < 13 && recv.available(); ++k)
							from[k] = static_cast<char>(recv.read_byte());
						g.from = from;
					}
					if (recv.length() >= 73) recv.skip(73);
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
						recv.length() >= 4 ? recv.read_int() : 0;
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
				e.cash_id = recv.length() >= 8 ? recv.read_long() : 0;
				if (recv.length() >= 8) { recv.skip(4); recv.skip(4); }
				e.itemid = recv.length() >= 4 ? recv.read_int() : 0;
				e.sn = recv.length() >= 4 ? recv.read_int() : 0;
				e.qty = recv.length() >= 2 ? recv.read_short() : 0;
				if (recv.length() >= 13) recv.skip(13);
				if (recv.length() >= 8) recv.skip(8);
				if (recv.length() >= 8) recv.skip(8);
				UIStateCashShop::cash_inventory.push_back(e);
				LOG(LOG_DEBUG, "CashShopOp: bought item=" << e.itemid << " sn=" << e.sn);
				while (recv.available())
					recv.skip_byte();
				// s9 buy 成功回执：确认本次 pending 购买，刷新"购物车"栏 UI。
				if (auto ui = UI::get().get_element<UICashShop>())
					ui->confirm_purchase_success();
				cs_tip(Locale::format("cashshop.bought", std::to_string(e.itemid)));
				break;
			}
			case 0x5C: // showCashShopMessage（失败/提示码）
			{
				uint8_t msg = static_cast<uint8_t>(recv.read_byte());
				LOG(LOG_DEBUG, "CashShopOp: message=0x" << std::hex << (int)msg);
				while (recv.available())
					recv.skip_byte();
				// s9 buy 失败回执：清 pending，购物车保留（MinMS 同口径）。
				if (auto ui = UI::get().get_element<UICashShop>())
					ui->clear_pending_buy();
				cs_tip(cs_fail_text(msg));
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
				cs_tip(Locale::format_all("cashshop.gift_ok", { to, std::to_string(itemid) }));
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
				cs_tip(Locale::format("cashshop.inv_slots", std::to_string(slots)));
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
				cs_tip(Locale::format("cashshop.storage_slots", std::to_string(slots)));
				break;
			}
			case 0x64: // bought char slots（s9 showBoughtCharacterSlot）
			{
				int16_t slots = recv.read_short();
				UIStateCashShop::cash_char_slots = slots;
				LOG(LOG_DEBUG, "CashShopOp: charSlots=" << slots);
				while (recv.available())
					recv.skip_byte();
				cs_tip(Locale::format("cashshop.char_slots", std::to_string(slots)));
				break;
			}
			case 0x68: // takeFromCashInventory — short pos + cashItemInfo
			{
				int16_t pos = recv.read_short();
				CashInvEntry e;
				e.cash_id = recv.length() >= 8 ? recv.read_long() : 0;
				if (recv.length() >= 8) { recv.skip(4); recv.skip(4); }
				e.itemid = recv.length() >= 4 ? recv.read_int() : 0;
				e.sn = recv.length() >= 4 ? recv.read_int() : 0;
				e.qty = recv.length() >= 2 ? recv.read_short() : 0;
				if (recv.length() >= 13) recv.skip(13);
				if (recv.length() >= 8) recv.skip(8);
				if (recv.length() >= 8) recv.skip(8);
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
				e.cash_id = recv.length() >= 8 ? recv.read_long() : 0;
				if (recv.length() >= 8) { recv.skip(4); recv.skip(4); }
				e.itemid = recv.length() >= 4 ? recv.read_int() : 0;
				e.sn = recv.length() >= 4 ? recv.read_int() : 0;
				e.qty = recv.length() >= 2 ? recv.read_short() : 0;
				if (recv.length() >= 13) recv.skip(13);
				if (recv.length() >= 8) recv.skip(8);
				if (recv.length() >= 8) recv.skip(8);
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
				cs_tip(Locale::format("cashshop.refund", std::to_string(pts)));
				break;
			}
			case 0x87: // showBoughtCashRing：cashItemInfo + string + itemId + short qty
			{
				CashInvEntry e;
				e.cash_id = recv.length() >= 8 ? recv.read_long() : 0;
				if (recv.length() >= 8) { recv.skip(4); recv.skip(4); }
				e.itemid = recv.length() >= 4 ? recv.read_int() : 0;
				e.sn = recv.length() >= 4 ? recv.read_int() : 0;
				e.qty = recv.length() >= 2 ? recv.read_short() : 0;
				if (recv.length() >= 13) recv.skip(13);
				if (recv.length() >= 8) recv.skip(8);
				if (recv.length() >= 8) recv.skip(8);
				UIStateCashShop::cash_inventory.push_back(e);
				std::string recipient = recv.available() ? recv.read_string() : "";
				int32_t itemid = recv.length() >= 4 ? recv.read_int() : e.itemid;
				int16_t qty = recv.length() >= 2 ? recv.read_short() : e.qty;
				LOG(LOG_DEBUG, "CashShopOp: ring to=" << recipient
					<< " item=" << itemid << " x" << qty);
				while (recv.available())
					recv.skip_byte();
				cs_tip(Locale::format("cashshop.ring", recipient));
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
					e.cash_id = recv.length() >= 8 ? recv.read_long() : 0;
					if (recv.length() >= 8) { recv.skip(4); recv.skip(4); }
					e.itemid = recv.length() >= 4 ? recv.read_int() : 0;
					e.sn = recv.length() >= 4 ? recv.read_int() : 0;
					e.qty = recv.length() >= 2 ? recv.read_short() : 0;
					if (recv.length() >= 13) recv.skip(13);
					if (recv.length() >= 8) recv.skip(8);
					if (recv.length() >= 8) recv.skip(8);
					UIStateCashShop::cash_inventory.push_back(e);
				}
				if (recv.length() >= 2)
					recv.read_short();
				LOG(LOG_DEBUG, "CashShopOp: package items=" << (int)n);
				while (recv.available())
					recv.skip_byte();
				// s9 礼包购买成功回执：确认 pending。
				if (auto ui = UI::get().get_element<UICashShop>())
					ui->confirm_purchase_success();
				cs_tip(Locale::format("cashshop.package", std::to_string(n)));
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
				// 任务道具购买成功回执：确认 pending。
				if (auto ui = UI::get().get_element<UICashShop>())
					ui->confirm_purchase_success();
				cs_tip(Locale::format("cashshop.quest_item", std::to_string(itemid)));
				break;
			}
			case 0x9E: // showNameChangeSuccess
			case 0xA0: // showWorldTransferSuccess
			{
				CashInvEntry e;
				e.cash_id = recv.length() >= 8 ? recv.read_long() : 0;
				if (recv.length() >= 8) { recv.skip(4); recv.skip(4); }
				e.itemid = recv.length() >= 4 ? recv.read_int() : 0;
				e.sn = recv.length() >= 4 ? recv.read_int() : 0;
				e.qty = recv.length() >= 2 ? recv.read_short() : 0;
				if (recv.length() >= 13) recv.skip(13);
				if (recv.length() >= 8) recv.skip(8);
				if (recv.length() >= 8) recv.skip(8);
				UIStateCashShop::cash_inventory.push_back(e);
				LOG(LOG_DEBUG, "CashShopOp: name/world ok mode=0x" << std::hex << (int)(uint8_t)mode
					<< std::dec << " item=" << e.itemid);
				while (recv.available())
					recv.skip_byte();
				cs_tip(mode == static_cast<int8_t>(0x9E)
					? Locale::text("cashshop.name_change_ok")
					: Locale::text("cashshop.world_transfer_ok"));
				break;
			}
			default:
			{
				LOG(LOG_DEBUG, "CashShopOp: unhandled mode=0x" << std::hex << (int)(uint8_t)mode
					<< std::dec << " rem=" << recv.available());
				while (recv.available())
					recv.skip_byte();
				cs_tip(Locale::format("cashshop.mode_unhandled",
					std::to_string(static_cast<int>(static_cast<uint8_t>(mode)))));
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
		cs_tip(in_use
			? Locale::format("cashshop.name_in_use", name)
			: Locale::format("cashshop.name_available", name));
	}

	void CashShopNameChangePossibleHandler::handle(InPacket& recv) const
	{
		recv.read_int();
		int8_t error = recv.read_byte();
		recv.read_int();
		LOG(LOG_DEBUG, "CashShopNamePossible: error=" << (int)error);
		while (recv.available())
			recv.skip_byte();
		cs_tip(error ? Locale::format("cashshop.name_change_err", std::to_string(error)) : Locale::text("cashshop.name_change_ok"));
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
		cs_tip(error ? Locale::format("cashshop.world_transfer_err", std::to_string(error)) : Locale::text("cashshop.world_transfer_ok"));
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
			e.cash_id = recv.length() >= 8 ? recv.read_long() : 0;
			if (recv.length() >= 8) { recv.skip(4); recv.skip(4); }
			e.itemid = recv.length() >= 4 ? recv.read_int() : 0;
			e.sn = recv.length() >= 4 ? recv.read_int() : 0;
			e.qty = recv.length() >= 2 ? recv.read_short() : 0;
			if (recv.length() >= 13) recv.skip(13);
			if (recv.length() >= 8) recv.skip(8);
			if (recv.length() >= 8) recv.skip(8);
			if (e.itemid != 0)
				UIStateCashShop::cash_inventory.push_back(e);
			int32_t reward_id = recv.length() >= 4 ? recv.read_int() : e.itemid;
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
				cs_tip(Locale::format_all(jackpot ? "cashshop.gacha_jackpot" : "cashshop.gacha_result",
					{ std::to_string(reward_id), std::to_string(qty) }));
			return;
		}
		LOG(LOG_DEBUG, "CashItemGacha: sub=0x" << std::hex << (int)(uint8_t)sub);
		while (recv.available())
			recv.skip_byte();
		cs_tip(Locale::format("cashshop.fail.generic", std::to_string(static_cast<int>(static_cast<uint8_t>(sub)))));
	}

	void SetCashShopHandler::handle(InPacket& recv) const
	{
		// s9 PacketCreator.openCashShop(c, false) :7399-7445。字段序：
		//   charinfo / byte(1) / str(account) / int(0) / short(modified_count)
		//   + modified_count × CommodityFlag / skip(121)
		//   / 热卖网格 writeInt(cat) writeInt(gender) writeInt(sn)
		//   / int(0) short(0) byte(0) int(75)
		CashShopParser::parseCharacterInfo(recv);

		recv.skip_byte();	// Not MTS
		recv.skip_string();	// account_name
		recv.skip_int();	// 服务端固定 writeInt(0)

		// modified_cash_item 覆盖表条数；运营未改价时为 0（正常语义）。
		int16_t modified_count = recv.read_short();

		std::vector<CashCommodity> overrides;
		if (modified_count < 0)
			modified_count = 0;
		overrides.reserve(static_cast<size_t>(modified_count));
		// 覆盖表是变长位图编码；任何解码越界只丢弃覆盖项，
		// 目录仍由本地 Commodity.img 支撑，不能因它让整包解析炸掉。
		try
		{
			for (int16_t i = 0; i < modified_count && recv.available(); i++)
				overrides.push_back(decode_modified_cash_item(recv));
		}
		catch (const PacketError& err)
		{
			LOG(LOG_WARN, "SetCashShop: modified overrides truncated at "
				<< overrides.size() << "/" << modified_count << ": " << err.what());
		}
		if (static_cast<int16_t>(overrides.size()) != modified_count)
			modified_count = static_cast<int16_t>(overrides.size());

		recv.skip(121);

		// 热卖网格：8 类 × 2 性别 × 该榜前 5；只跳读，UI 热卖位后续再接。
		// 实测抓包：每榜 5 条 → 8×2×5 = 80 组 × 12B（@724 起、@1684 收尾 11B）。
		// 不硬编码 80：以 (cat,gender,sn) 值域守卫，越界即视为读尽，并把已试探
		// 的 12B 退回，保证随后 int/short/byte/int 收尾不错位。
		std::vector<int32_t>& best_sellers = UIStateCashShop::cash_best_sellers;
		best_sellers.clear();
		while (recv.length() >= 12 + 11)	// 一条网格 12B + 尾部 11B
		{
			const size_t mark = recv.position();
			const int32_t cat = recv.read_int();
			const int32_t gender = recv.read_int();
			const int32_t sn = recv.read_int();
			if (cat < 1 || cat > 8 || (gender != 0 && gender != 1) || sn <= 0)
			{
				recv.seek(mark);
				break;
			}
			best_sellers.push_back(sn);
		}

		// 收尾 11B（s9 :7439-7442）：int(0) + short(0) + byte(0) + int(75)。
		const int32_t tail_zero = recv.read_int();
		const int16_t tail_marker = recv.read_short();
		recv.skip_byte();
		const int32_t tail_cash = recv.read_int();

		// 目录 = 本地 Commodity.img 基表 + 服务端覆盖（对齐 s9
		// CashShopService.java:59-73 的"以数据库为准更新可能更新的字段"）。
		std::vector<CashCommodity>& catalog = UIStateCashShop::cashshop_commodities;
		catalog.clear();
		load_local_commodities(catalog);

		for (const CashCommodity& ov : overrides)
		{
			auto it = std::find_if(catalog.begin(), catalog.end(),
				[&ov](const CashCommodity& c) { return c.sn == ov.sn; });
			if (it != catalog.end())
			{
				if (ov.itemid > 0) it->itemid = ov.itemid;
				if (ov.price > 0) it->price = ov.price;
				if (ov.period > 0) it->period = ov.period;
				if (ov.count > 0) it->count = ov.count;
				if (ov.gender != 2) it->gender = ov.gender;
				if (ov.has_onsale) it->onsale = ov.onsale;
				continue;
			}
			// 服务端只发改价行（WZ 侧无此行时）：直接收录为在售条目。
			if (ov.itemid > 0 && ov.onsale)
				catalog.push_back(ov);
		}

		// 只裁下架项。性别不做裁剪：s9 CashShopService.java:76-81 的目录过滤
		// 条件只有 onSale / itemId，gender 仅作为数据字段下发；实测 Commodity.img
		// 上架行里 Gender 取值有 0/1/2/-1 四种，客户端自行按性别过滤会少显示商品。
		catalog.erase(std::remove_if(catalog.begin(), catalog.end(),
			[](const CashCommodity& c) { return !c.onsale; }), catalog.end());

		while (recv.available())
			recv.skip_byte();

		// Release 下 LOG 是 no-op：显式标注这些量仅供诊断。
		(void)tail_zero; (void)tail_marker; (void)tail_cash; (void)best_sellers;

		LOG(LOG_INFO, "SetCashShop: modified=" << modified_count
			<< " best=" << best_sellers.size()
			<< " tail=(" << tail_zero << "," << tail_marker << "," << tail_cash << ")"
			<< " catalog=" << catalog.size());

		transition();

		UI::get().change_state(UI::State::CASHSHOP);
	}

	void SetCashShopHandler::transition() const
	{
		// Angel CCashShop 画布 = CashShop.img/Base/backgrnd 800×600（非 1024×768）。
		// 旧强制 1024 会让 800 宽控件相对视口漂浮，并露出底图外的双 EXIT 错觉。
		Constants::Constants::get().set_viewwidth(800);
		Constants::Constants::get().set_viewheight(600);

		// 商城进出是纯状态切换（不装地图），黑场只服务观感；与换图同批提速
		// 0.025→0.04（25 拍 ×8ms = 200ms/侧），Wave P 丝滑批。
		float fadestep = 0.04f;

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