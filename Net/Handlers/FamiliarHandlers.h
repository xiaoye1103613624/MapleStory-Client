////////////////////////////////////////////////////////////////////////////////
//  精灵收藏 2 个 S→C handler（0x3761 / 0x3762）
//  字段读取顺序严格按 lead 下发且与服务端 impl-familiar agent 约定：
//   FAMILIAR_LIST   = int count + ×{int familiarId, string name, byte grade,
//                     byte level, int exp, int skill, int opt1, int opt2, int opt3, byte summon}
//   FAMILIAR_RESULT = int mode + int code + int familiarId
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../PacketHandler.h"

namespace ms
{
    // 0x3761：int count + ×{int familiarId, string name, byte grade, byte level,
    //         int exp, int skill, int opt1, int opt2, int opt3, byte summon}
    class FamiliarListHandler : public PacketHandler
    {
    public:
        void handle(InPacket& recv) const override;
    };

    // 0x3762：int mode + int code + int familiarId
    class FamiliarResultHandler : public PacketHandler
    {
    public:
        void handle(InPacket& recv) const override;
    };
}
