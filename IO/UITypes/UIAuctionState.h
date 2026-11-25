////////////////////////////////////////////////////////////////////////////////
//  UIAuctionState — 拍卖行 S→C 数据的单例落地（供 handler 写入、UI 读取）
//
//  字段严格对照服务端 AuctionHandler.java 的 write 顺序（317-376 行）：
//   · AUCTION_LIST      0x3751：int subMode + int count + ×{long invId, int status,
//                                 int itemId, int qty, long meso, long expire, string owner}
//   · AUCTION_SELL_LIST 0x3752：int count + ×{同上简表}
//   · AUCTION_COMPLETE  0x3753：int count + ×{long invId, int status, int itemId,
//                                 int qty, long bid}
//   · AUCTION_MESSAGE   0x3750：int mode + int code + int invId
//   · AUCTION_BUY_RESULT 0x3754：int code + long price + int invId
//
//  收包线程只写这些 POD/vector，UI 从主线程读取；粒度与本项目其它桥不同
//  （CashShopWindowSync 用 mutex+queue），这里直接覆盖快照：拍卖列表不是高速流，
//  最坏读到上一帧，均摊延迟低于 1 帧，故不引入额外队列。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
    struct AuctionEntry
    {
        int64_t invId = 0;
        int32_t status = 0;
        int32_t itemId = 0;
        int32_t qty = 0;
        int64_t meso = 0;
        int64_t expire = 0;
        std::string owner;
    };

    struct AuctionCompleteEntry
    {
        int64_t invId = 0;
        int32_t status = 0;
        int32_t itemId = 0;
        int32_t qty = 0;
        int64_t bid = 0;
    };

    class UIAuctionState
    {
    public:
        // 单个进程内唯一实例；函数内静态跨 translation unit 只生成一份（C++11 线程安全）。
        static UIAuctionState& get()
        {
            static UIAuctionState instance;
            return instance;
        }

        // --- 0x3750 通用反馈 ---
        void set_message(int32_t mode, int32_t code, int64_t inv_id)
        {
            last_mode = mode;
            last_code = code;
            last_inv_id = inv_id;
            ++message_seq;
        }

        int32_t last_mode = 0;
        int32_t last_code = 0;
        int64_t last_inv_id = 0;
        uint32_t message_seq = 0;

        // --- 0x3751 / 0x3752 在售 / 行情列表 ---
        // subMode 与服务端 MODE_SEARCH_HOME(40)/MODE_SEARCH_QUOTES(41) 对齐。
        std::vector<AuctionEntry> home_list;
        std::vector<AuctionEntry> quotes_list;
        int32_t last_list_submode = 0;
        uint32_t home_seq = 0;
        uint32_t quotes_seq = 0;

        // --- 0x3753 完成（下架/购买/领取）列表 ---
        std::vector<AuctionCompleteEntry> complete_list;
        uint32_t complete_seq = 0;

        // --- 0x3754 购买结果 ---
        int32_t buy_code = 0;
        int64_t buy_price = 0;
        int64_t buy_inv_id = 0;
        uint32_t buy_seq = 0;
    };
}
