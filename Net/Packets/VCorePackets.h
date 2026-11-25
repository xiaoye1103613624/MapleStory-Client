////////////////////////////////////////////////////////////////////////////////
//  VCORE_ACTION (C→S 0x3770 = 14192) — V 矩阵/核心请求包
//
//  【字段布局按服务端 VCoreHandler.java 真实读取顺序对齐，lead 2026-11-25 修正】
//   action=0 装备：int action + int vcoreOid + int usedOid + int 保留(填0) + int index
//   action=1 卸下：int action + int vcoreOid + int 保留(填0)
//   action=4 强化：int action + int targetOid + int count + count×int materialOid
//   action=5 分解：int action + int vcoreOid
//
//  所有 id 用 int32 写出（4B），与服务端 readInt 对齐；保留位固定写 0。
//  回包 0x3771/0x3772 布局见 Net/Handlers/VCoreHandlers.h。不改服务端。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <cstdint>
#include <vector>

namespace ms
{
    class VCoreActionPacket : public OutPacket
    {
    public:
        // action=0 装备：vcoreOid 装到 index 槽；usedOid 为被替换/占用的核心 oid
        // （0 = 空槽装备；当前列表 0x3771 无该字段，UI 单击装备只能填 0）。
        static VCoreActionPacket equip(int32_t vcore_id, int32_t used_oid, int32_t index)
        {
            VCoreActionPacket p(0);
            p.write_int(vcore_id);
            p.write_int(used_oid);
            p.write_int(0);       // 保留位：服务端 readInt 后丢弃
            p.write_int(index);
            return p;
        }

        // action=1 卸下：写 int vcoreOid + int 保留(0)。
        static VCoreActionPacket unequip(int32_t vcore_id)
        {
            VCoreActionPacket p(1);
            p.write_int(vcore_id);
            p.write_int(0);       // 保留位
            return p;
        }

        // action=4 强化：targetOid + count + count×materialOid。
        static VCoreActionPacket enhance(int32_t target_id, const std::vector<int32_t>& materials)
        {
            VCoreActionPacket p(4);
            p.write_int(target_id);
            p.write_int(static_cast<int32_t>(materials.size()));
            for (int32_t oid : materials)
                p.write_int(oid);
            return p;
        }

        // action=5 分解：仅 int vcoreOid（与原约定一致）。
        static VCoreActionPacket decompose(int32_t vcore_id)
        {
            VCoreActionPacket p(5);
            p.write_int(vcore_id);
            return p;
        }

    private:
        explicit VCoreActionPacket(int32_t action)
            : OutPacket(OutPacket::Opcode::VCORE_ACTION)
        {
            write_int(action);
        }
    };
}
