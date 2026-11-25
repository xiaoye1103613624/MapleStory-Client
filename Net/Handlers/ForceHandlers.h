////////////////////////////////////////////////////////////////////////////////
//  神秘之力 / 奥术之力 S→C handler（0x3781）
//  字段读取顺序严格按 lead 下发且与服务端 impl-force agent 约定：
//   FORCE_INFO = int arcaneForce + int authenticForce + int arcaneLevel
//                + int authenticLevel  （4×int = 16B，无其它字段）
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../PacketHandler.h"

namespace ms
{
    // 0x3781：int arcaneForce + int authenticForce + int arcaneLevel + int authenticLevel
    class ForceInfoHandler : public PacketHandler
    {
    public:
        void handle(InPacket& recv) const override;
    };
}
