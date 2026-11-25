////////////////////////////////////////////////////////////////////////////////
//  FAMILIAR_ACTION (C→S 0x3760 = 14176) — 精灵收藏请求包
//
//  与服务端 impl-familiar agent 约定（lead 已确认）：
//   mode=0  进入：仅 int mode，无附加字段（服务端回 0x3761 列表）
//   mode=1  召唤切换：int mode + int familiarId
//   mode=2  升级：int mode + int familiarId
//
//  注意：familiarId 用 int32 写出（4B），与服务端 readInt 对齐；
//  参考 exp 4B/8B 错位教训，绝不写成 long。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <cstdint>

namespace ms
{
    class FamiliarActionPacket : public OutPacket
    {
    public:
        // mode=0 进入精灵收藏：服务端回 FAMILIAR_LIST(0x3761)
        static FamiliarActionPacket enter()
        {
            return FamiliarActionPacket(0);
        }

        // mode=1 召唤/取消召唤切换：带 int familiarId
        static FamiliarActionPacket summon(int32_t familiar_id)
        {
            FamiliarActionPacket p(1);
            p.write_int(familiar_id);
            return p;
        }

        // mode=2 升级精灵：带 int familiarId
        static FamiliarActionPacket level_up(int32_t familiar_id)
        {
            FamiliarActionPacket p(2);
            p.write_int(familiar_id);
            return p;
        }

    private:
        explicit FamiliarActionPacket(int32_t mode)
            : OutPacket(OutPacket::Opcode::FAMILIAR_ACTION)
        {
            write_int(mode);
        }
    };
}
