////////////////////////////////////////////////////////////////////////////////
//  V 矩阵/核心 2 个 S→C handler（0x3771 / 0x3772）
//  字段读取顺序严格按协议约定（与服务端 impl-vcore agent 对齐）：
//   VCORE_LIST   = int count + ×{int vcoreid, byte slot, byte level,
//                   int skill1, int skill2, int skill3, int index}
//   VCORE_RESULT = int action + int code + int vcoreid
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../PacketHandler.h"

namespace ms
{
    // 0x3771：int count + ×{int vcoreid, byte slot, byte level,
    //         int skill1, int skill2, int skill3, int index}
    class VCoreListHandler : public PacketHandler
    {
    public:
        void handle(InPacket& recv) const override;
    };

    // 0x3772：int action + int code + int vcoreid
    class VCoreResultHandler : public PacketHandler
    {
    public:
        void handle(InPacket& recv) const override;
    };
}
