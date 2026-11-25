////////////////////////////////////////////////////////////////////////////////
//  FORCE_ACTION (C→S 0x3780 = 14208) — 神秘之力 / 奥术之力请求包
//
//  与服务端 impl-force agent 约定（lead 下发，按此实现）：
//   mode=0  查询：仅 int mode，无其它载荷（服务端回 0x3781 FORCE_INFO）
//   mode=1  升级神秘之力：仅 int mode
//   mode=2  升级奥术之力：仅 int mode
//
//  所有模式都只写一个 int32 mode，无附加字段（严格对齐服务端 readInt）。
//  与 S→C 0x3781（FORCE_INFO）分属 In/Out 两张表，不冲突。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <cstdint>

namespace ms
{
    class ForceActionPacket : public OutPacket
    {
    public:
        // mode=0 查询：服务端回 FORCE_INFO(0x3781)
        static ForceActionPacket query()
        {
            return ForceActionPacket(0);
        }

        // mode=1 升级神秘之力
        static ForceActionPacket upgradeArcane()
        {
            return ForceActionPacket(1);
        }

        // mode=2 升级奥术之力
        static ForceActionPacket upgradeAuthentic()
        {
            return ForceActionPacket(2);
        }

    private:
        explicit ForceActionPacket(int32_t mode)
            : OutPacket(OutPacket::Opcode::FORCE_ACTION)
        {
            write_int(mode);
        }
    };
}
