////////////////////////////////////////////////////////////////////////////////
//  拍卖行 5 个 S→C handler（0x3750..0x3754）
//  字段读取顺序严格对齐 s9 AuctionHandler.java:317-376（write 顺序即 read 顺序）。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../PacketHandler.h"

namespace ms
{
    // 0x3750：int mode + int code + int invId
    class AuctionMessageHandler : public PacketHandler
    {
    public:
        void handle(InPacket& recv) const override;
    };

    // 0x3751：int subMode + int count + ×{long invId, int status, int itemId,
    //         int qty, long meso, long expire, string owner}
    class AuctionListHandler : public PacketHandler
    {
    public:
        void handle(InPacket& recv) const override;
    };

    // 0x3752：int count + ×{同上简表}（无 subMode）
    class AuctionSellListHandler : public PacketHandler
    {
    public:
        void handle(InPacket& recv) const override;
    };

    // 0x3753：int count + ×{long invId, int status, int itemId, int qty, long bid}
    class AuctionCompleteListHandler : public PacketHandler
    {
    public:
        void handle(InPacket& recv) const override;
    };

    // 0x3754：int code + long price + int invId
    class AuctionBuyResultHandler : public PacketHandler
    {
    public:
        void handle(InPacket& recv) const override;
    };
}
