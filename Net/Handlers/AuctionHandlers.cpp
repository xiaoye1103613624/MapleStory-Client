////////////////////////////////////////////////////////////////////////////////
//  拍卖行 S→C handler 实现
//
//  【字段对齐铁律】服务端写什么顺序，客户端就按什么顺序读；每个字段读之前
//  先 recv.available() 校验剩余字节，不足整条直接 return，避免 exp 4B/8B
//  混读错位。read_string() 已含 2 字节长度头（见服务端 readAscii 注释）。
////////////////////////////////////////////////////////////////////////////////
#include "AuctionHandlers.h"

#include "../../IO/UITypes/UIAuctionState.h"
#include "../../Util/Misc.h"

namespace ms
{
    namespace
    {
        // 单条目固定开销：invId(8) + status(4) + itemId(4) + qty(4) + meso(8) + expire(8) = 36B
        constexpr size_t AUCTION_ENTRY_FIXED = 36;
        constexpr size_t AUCTION_COMPLETE_FIXED = 28; // invId(8)+status(4)+itemId(4)+qty(4)+bid(8)

        // 读一条简表：long invId / int status / int itemId / int qty / long meso / long expire / string owner
        bool read_entry(InPacket& recv, AuctionEntry& out)
        {
            if (recv.available() < AUCTION_ENTRY_FIXED + 2)
                return false;

            out.invId = static_cast<int64_t>(recv.read_long());
            out.status = recv.read_int();
            out.itemId = recv.read_int();
            out.qty = recv.read_int();
            out.meso = static_cast<int64_t>(recv.read_long());
            out.expire = static_cast<int64_t>(recv.read_long());
            out.owner = recv.read_string();
            return true;
        }
    }

    void AuctionMessageHandler::handle(InPacket& recv) const
    {
        if (recv.available() < 12)
            return;

        const int32_t mode = recv.read_int();
        const int32_t code = recv.read_int();
        const int64_t inv_id = static_cast<int64_t>(recv.read_int()); // 服务端 writeInt(invId)
        UIAuctionState::get().set_message(mode, code, inv_id);

        LOG(LOG_DEBUG, "[DBG-AUC] message mode=" << mode << " code=" << code << " invId=" << inv_id);
    }

    void AuctionListHandler::handle(InPacket& recv) const
    {
        if (recv.available() < 8)
            return;

        const int32_t submode = recv.read_int();
        const int32_t count = recv.read_int();
        if (count < 0 || count > 1000)
        {
            LOG(LOG_WARN, "[AUC] list count invalid=" << count << " subMode=" << submode);
            return;
        }

        std::vector<AuctionEntry> rows;
        rows.reserve(static_cast<size_t>(count));
        for (int32_t i = 0; i < count; ++i)
        {
            AuctionEntry e;
            if (!read_entry(recv, e))
            {
                LOG(LOG_WARN, "[AUC] list truncated at " << i << "/" << count);
                return;
            }
            rows.push_back(std::move(e));
        }

        auto& state = UIAuctionState::get();
        state.last_list_submode = submode;
        // 服务端 40=在售首页，41=行情搜索；两个视图分开存，互不覆盖。
        if (submode == 41)
        {
            state.quotes_list = std::move(rows);
            ++state.quotes_seq;
        }
        else
        {
            state.home_list = std::move(rows);
            ++state.home_seq;
        }
        LOG(LOG_DEBUG, "[DBG-AUC] list subMode=" << submode << " count=" << count);
    }

    void AuctionSellListHandler::handle(InPacket& recv) const
    {
        if (recv.available() < 4)
            return;

        const int32_t count = recv.read_int();
        if (count < 0 || count > 1000)
        {
            LOG(LOG_WARN, "[AUC] sell list count invalid=" << count);
            return;
        }

        std::vector<AuctionEntry> rows;
        rows.reserve(static_cast<size_t>(count));
        for (int32_t i = 0; i < count; ++i)
        {
            AuctionEntry e;
            if (!read_entry(recv, e))
            {
                LOG(LOG_WARN, "[AUC] sell list truncated at " << i << "/" << count);
                return;
            }
            rows.push_back(std::move(e));
        }

        auto& state = UIAuctionState::get();
        state.home_list = std::move(rows); // 0x3752 简表按在售首页处理
        ++state.home_seq;
        LOG(LOG_DEBUG, "[DBG-AUC] sell list count=" << count);
    }

    void AuctionCompleteListHandler::handle(InPacket& recv) const
    {
        if (recv.available() < 4)
            return;

        const int32_t count = recv.read_int();
        if (count < 0 || count > 1000)
        {
            LOG(LOG_WARN, "[AUC] complete count invalid=" << count);
            return;
        }

        std::vector<AuctionCompleteEntry> rows;
        rows.reserve(static_cast<size_t>(count));
        for (int32_t i = 0; i < count; ++i)
        {
            if (recv.available() < AUCTION_COMPLETE_FIXED)
            {
                LOG(LOG_WARN, "[AUC] complete truncated at " << i << "/" << count);
                return;
            }
            AuctionCompleteEntry e;
            e.invId = static_cast<int64_t>(recv.read_long());
            e.status = recv.read_int();
            e.itemId = recv.read_int();
            e.qty = recv.read_int();
            e.bid = static_cast<int64_t>(recv.read_long());
            rows.push_back(std::move(e));
        }

        auto& state = UIAuctionState::get();
        state.complete_list = std::move(rows);
        ++state.complete_seq;
        LOG(LOG_DEBUG, "[DBG-AUC] complete count=" << count);
    }

    void AuctionBuyResultHandler::handle(InPacket& recv) const
    {
        if (recv.available() < 16)
            return;

        const int32_t code = recv.read_int();
        const int64_t price = static_cast<int64_t>(recv.read_long());
        const int64_t inv_id = static_cast<int64_t>(recv.read_int()); // 服务端 writeInt(invId)

        auto& state = UIAuctionState::get();
        state.buy_code = code;
        state.buy_price = price;
        state.buy_inv_id = inv_id;
        ++state.buy_seq;
        LOG(LOG_DEBUG, "[DBG-AUC] buyResult code=" << code << " price=" << price << " invId=" << inv_id);
    }
}
