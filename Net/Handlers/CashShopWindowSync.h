//////////////////////////////////////////////////////////////////////////////////
// 窗口内商城（方案 C）S→C 同步数据的跨线程桥。
//
// 双线程约束（参考包 cashshopwnd.cpp 明确要求）：收包线程只解析字节 + 入队，
// 绝不直接操作 UI（UI::emplace / UIElement 状态只能在主线程碰）。主线程在
// UI::update() 每帧调 CashShopWindowSync::pump() 消费队列并转发给 UIWindowCashShop。
//
// 数据形状与 S10 CashShopWindowPackets.java 的写出字段一一对应；buf 用 mutex
// 保护，pump 在锁内 swap 到本地后解锁再处理，避免持锁调用 UI。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

namespace ms
{
	struct CashShopWindowCatalogRow
	{
		int32_t itemId = 0;
		int32_t price = 0;
		int16_t count = 0;
		int8_t tab = 0;
		int8_t cat = 0;
		std::string name;
	};

	struct CashShopWindowIndexRow
	{
		int8_t tab = 0;
		int8_t cat = 0;
		int32_t count = 0;
	};

	struct CashShopWindowTaxonomyRow
	{
		int32_t id = 0;
		int32_t parentId = 0;
		int8_t clickType = 0;
		std::string clickParam;
		int8_t legacyTab = 0;
		int8_t legacyCat = 0;
		int32_t itemCount = 0;
		std::string name;
	};

	// 一条待主线程消费的事件。kind 取值与 S→C resp 字节一致（6 另加 TAXONOMY=6）：
	// 0 OPEN / 1 CATALOG / 2 BUY / 3 CASH / 4 BUY_CART / 5 INDEX / 6 TAXONOMY。
	struct CashShopWindowEvent
	{
		int8_t kind = -1;
		// OPEN / CASH：三币种（NX_CREDIT, MAPLE_POINT, NX_PREPAID）
		int32_t cash[3] = { 0, 0, 0 };
		// CATALOG
		int8_t clear = 0;
		int8_t tab = 0;
		int8_t cat = 0;
		int16_t rowCount = 0;
		std::vector<CashShopWindowCatalogRow> rows;
		// INDEX
		std::vector<CashShopWindowIndexRow> index;
		// TAXONOMY
		std::vector<CashShopWindowTaxonomyRow> taxonomy;
		// BUY / BUY_CART
		int8_t code = 0;
		int32_t itemId = 0;
		int8_t delivered = 0;
		int32_t spent = 0;
	};

	class CashShopWindowSync
	{
	public:
		// 收包线程调用：把一条已解析事件入队。只碰 mutex 与队列。
		static void enqueue(CashShopWindowEvent event);

		// 主线程调用（UI::update 每帧）：swap 出所有待处理事件并转发给窗口。
		// 窗口未打开时 OPEN 事件负责 emplace；其余事件在窗口不存在时丢弃并记 DEBUG。
		static void pump();

		// 窗口关闭时清空残留事件，避免下次打开时收到上一个会话的陈旧商品。
		static void clear();

	private:
		static std::mutex mtx;
		static std::queue<CashShopWindowEvent> queue;
	};
}
