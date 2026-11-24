//////////////////////////////////////////////////////////////////////////////////
// 窗口内商城（方案 C）C→S 动作包。
//
// 协议逐字节对照 S10 server/cashshop/CashShopWindowPackets.java 与参考包
// cashshopwnd.cpp:73-76（kAction_*）。opcode CASHSHOP_WINDOW_ACTION = 0x3730，
// body 首字节为 action，其后字段顺序与服务端读取顺序严格一致：
//   0 REQUEST_CATALOG :（无 body）
//   1 BUY            : int itemId
//   2 BUY_CART       : byte n + n×int itemId（n ≤ 20，客户端侧限幅）
//   3 REQUEST_CATEGORY: byte tab + byte cat
//   4 CLICK_CATEGORY  : int categoryId（动态分类树点击）
//   5 EXPAND_SLOTS    : int cashType + byte invType
//
// 注意：只在构造期写字节，dispatch() 由调用方决定；这些类本身与 UI 线程无关。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <cstdint>
#include <vector>

namespace ms
{
	// body action 字面量，与服务端常量一一对应（不要改动数值，改则须双端同步）。
	enum CashShopWindowAction : int8_t
	{
		CSW_ACTION_REQUEST_CATALOG = 0,
		CSW_ACTION_BUY = 1,
		CSW_ACTION_BUY_CART = 2,
		CSW_ACTION_REQUEST_CATEGORY = 3,
		CSW_ACTION_CLICK_CATEGORY = 4,
		CSW_ACTION_EXPAND_SLOTS = 5,
	};

	// 0 REQUEST_CATALOG：开场拉一次分类索引/余额（服务端回 RESP_INDEX + RESP_CASH）。
	class CashShopWindowRequestCatalogPacket : public OutPacket
	{
	public:
		CashShopWindowRequestCatalogPacket() : OutPacket(OutPacket::Opcode::CASHSHOP_WINDOW_ACTION)
		{
			write_byte(CSW_ACTION_REQUEST_CATALOG);
		}
	};

	// 1 BUY：单件购买，服务端回 RESP_BUY(code, itemId)。
	class CashShopWindowBuyPacket : public OutPacket
	{
	public:
		explicit CashShopWindowBuyPacket(int32_t itemId) : OutPacket(OutPacket::Opcode::CASHSHOP_WINDOW_ACTION)
		{
			write_byte(CSW_ACTION_BUY);
			write_int(itemId);
		}
	};

	// 2 BUY_CART：购物车全有或全无；n 上限 20（与 UI 购物车槽数一致），
	// 防止畸形输入把整包撑大。服务端回 RESP_BUY_CART(code, itemId, delivered, spent)。
	class CashShopWindowBuyCartPacket : public OutPacket
	{
	public:
		explicit CashShopWindowBuyCartPacket(const std::vector<int32_t>& itemIds)
			: OutPacket(OutPacket::Opcode::CASHSHOP_WINDOW_ACTION)
		{
			write_byte(CSW_ACTION_BUY_CART);
			const int8_t n = static_cast<int8_t>(itemIds.size() > 20u ? 20u : itemIds.size());
			write_byte(n);
			for (int8_t i = 0; i < n; ++i)
				write_int(itemIds[static_cast<size_t>(i)]);
		}
	};

	// 3 REQUEST_CATEGORY：按 tab/cat 拉一个分类的商品（分包 RESP_CATALOG）。
	class CashShopWindowRequestCategoryPacket : public OutPacket
	{
	public:
		CashShopWindowRequestCategoryPacket(int8_t tab, int8_t cat)
			: OutPacket(OutPacket::Opcode::CASHSHOP_WINDOW_ACTION)
		{
			write_byte(CSW_ACTION_REQUEST_CATEGORY);
			write_byte(tab);
			write_byte(cat);
		}
	};

	// 4 CLICK_CATEGORY：动态分类树（RESP_TAXONOMY）里点一个叶节点。
	class CashShopWindowClickCategoryPacket : public OutPacket
	{
	public:
		explicit CashShopWindowClickCategoryPacket(int32_t categoryId)
			: OutPacket(OutPacket::Opcode::CASHSHOP_WINDOW_ACTION)
		{
			write_byte(CSW_ACTION_CLICK_CATEGORY);
			write_int(categoryId);
		}
	};

	// 5 EXPAND_SLOTS：扩容请求；cashType 为币种，invType 为背包类型。
	class CashShopWindowExpandSlotsPacket : public OutPacket
	{
	public:
		CashShopWindowExpandSlotsPacket(int32_t cashType, int8_t invType)
			: OutPacket(OutPacket::Opcode::CASHSHOP_WINDOW_ACTION)
		{
			write_byte(CSW_ACTION_EXPAND_SLOTS);
			write_int(cashType);
			write_byte(invType);
		}
	};
}
