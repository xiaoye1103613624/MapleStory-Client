////////////////////////////////////////////////////////////////////////////////
//  AUCTION_ACTION (C→S 0x3751 = 14161) — 拍卖行请求包
//
//  对齐 s9 AuctionHandler.java:73-105 的 mode 分发与逐字段读取：
//   mode=0  进入：仅 int mode，无额外
//   mode=10 上架：int bargain + int itemId + int quantity + long meso +
//                 int hours + byte inv + short slot + short unk + int cash
//   mode=12 下架：int mode + int invId（服务端 readInt）
//   mode=20 购买：int mode + int invId + long price
//   mode=30/31 领取：int mode + int invId
//   mode=40/41 搜索：int mode + byte subMode + int state + string str1 + string str2
//                   （服务端 readByte + readInt + readAscii×2；简单处理）
//
//  注意：invId 服务端用 writeInt 写出（AuctionHandler.message 第 321 行），
//  客户端按 int32 发送，不要写成 long —— 这是 exp 4B/8B 教训的另一面。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <cstdint>
#include <string>

namespace ms
{
    class AuctionActionPacket : public OutPacket
    {
    public:
        // mode=0 进入拍卖（服务端回 0x3750/0x3751）
        static AuctionActionPacket enter()
        {
            return AuctionActionPacket(0);
        }

        // mode=10 上架：bargain(可讲价) / itemId / quantity / meso(单价)
        // hours(挂机时长) / inv(背包类型 byte) / slot(格子 short) / cash(0)
        static AuctionActionPacket list(int32_t bargain, int32_t item_id, int32_t quantity,
                                       int64_t meso, int32_t hours, uint8_t inv,
                                       int16_t slot, int32_t cash = 0)
        {
            AuctionActionPacket p(10);
            p.write_int(bargain);
            p.write_int(item_id);
            p.write_int(quantity);
            p.write_long(meso);
            p.write_int(hours);
            p.write_byte(static_cast<int8_t>(inv));
            p.write_short(slot);
            p.write_short(0); // 197 保留字段 unk
            p.write_int(cash);
            return p;
        }

        // mode=12 下架：invId
        static AuctionActionPacket take_down(int32_t inv_id)
        {
            AuctionActionPacket p(12);
            p.write_int(inv_id);
            return p;
        }

        // mode=20 购买：int invId + long price（与服务端 handleBuy 读序一致）
        static AuctionActionPacket buy(int32_t inv_id, int64_t price)
        {
            AuctionActionPacket p(20);
            p.write_int(inv_id);
            p.write_long(price);
            return p;
        }

        // mode=30 领取报酬 / 31 领取物品
        static AuctionActionPacket claim(int32_t inv_id, bool item = false)
        {
            AuctionActionPacket p(item ? 31 : 30);
            p.write_int(inv_id);
            return p;
        }

        // mode=40 首页搜索 / 41 行情搜索
        // 服务端读 byte unk2 + int state2 + string str1 + string str2（readAscii）
        static AuctionActionPacket search(int32_t sub_mode, int32_t state,
                                          const std::string& str1, const std::string& str2)
        {
            AuctionActionPacket p(sub_mode == 41 ? 41 : 40);
            p.write_byte(0); // unk2
            p.write_int(state);
            p.write_string(str1);
            p.write_string(str2);
            return p;
        }

    private:
        explicit AuctionActionPacket(int32_t mode)
            : OutPacket(OutPacket::Opcode::AUCTION_ACTION)
        {
            write_int(mode);
        }
    };
}
