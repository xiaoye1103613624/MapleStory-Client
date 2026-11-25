////////////////////////////////////////////////////////////////////////////////
//  V 矩阵/核心 S→C handler 实现
//
//  【字段对齐铁律】服务端写什么顺序，客户端就按什么顺序读；每条记录读取前
//  先 recv.available() 校验剩余字节，不足整条直接 return，避免 4B/1B 混读
//  错位（参考 exp 4B/8B 教训）。每条 0x3771 记录固定 22B：
//   vcoreid(4) + slot(1) + level(1) + skill1(4) + skill2(4) + skill3(4) + index(4)。
////////////////////////////////////////////////////////////////////////////////
#include "VCoreHandlers.h"

#include "../../IO/UITypes/VCoreState.h"
#include "../../Util/Misc.h"

namespace ms
{
    namespace
    {
        constexpr size_t VCORE_ENTRY_FIXED = 22;   // 4 + 1 + 1 + 4*4

        // 读一条 0x3771 记录，字段严格按约定顺序；越界返回 false 由调用方整包丢弃。
        bool read_entry(InPacket& recv, VCoreEntry& out)
        {
            if (recv.available() < VCORE_ENTRY_FIXED)
                return false;

            out.vcoreid = recv.read_int();
            out.slot = static_cast<uint8_t>(recv.read_byte());
            out.level = static_cast<uint8_t>(recv.read_byte());
            out.skill1 = recv.read_int();
            out.skill2 = recv.read_int();
            out.skill3 = recv.read_int();
            out.index = recv.read_int();
            return true;
        }
    }

    void VCoreListHandler::handle(InPacket& recv) const
    {
        if (recv.available() < 4)
            return;

        const int32_t count = recv.read_int();
        if (count < 0 || count > 1000)
        {
            LOG(LOG_WARN, "[VCORE] list count invalid=" << count);
            return;
        }

        std::vector<VCoreEntry> rows;
        rows.reserve(static_cast<size_t>(count));
        for (int32_t i = 0; i < count; ++i)
        {
            VCoreEntry e;
            if (!read_entry(recv, e))
            {
                LOG(LOG_WARN, "[VCORE] list truncated at " << i << "/" << count);
                return;
            }
            rows.push_back(std::move(e));
        }

        VCoreState::get().set_list(std::move(rows));
        LOG(LOG_DEBUG, "[DBG-VCORE] list count=" << count);
    }

    void VCoreResultHandler::handle(InPacket& recv) const
    {
        if (recv.available() < 12)
            return;

        const int32_t action = recv.read_int();
        const int32_t code = recv.read_int();
        const int32_t vcoreid = recv.read_int();

        VCoreState::get().set_result(action, code, vcoreid);
        LOG(LOG_DEBUG, "[DBG-VCORE] result action=" << action
            << " code=" << code << " vcoreid=" << vcoreid);
    }
}
