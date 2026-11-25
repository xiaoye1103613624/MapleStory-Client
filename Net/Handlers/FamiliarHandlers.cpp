////////////////////////////////////////////////////////////////////////////////
//  精灵收藏 S→C handler 实现
//
//  【字段对齐铁律】服务端写什么顺序，客户端就按什么顺序读；每个字段读之前
//  先 recv.available() 校验剩余字节，不足整条直接 return，避免 exp 4B/8B
//  混读错位。read_string() 已含 2 字节长度头（见 InPacket::read_string）。
////////////////////////////////////////////////////////////////////////////////
#include "FamiliarHandlers.h"

#include "../../IO/UITypes/UIFamiliarState.h"
#include "../../Util/Misc.h"

namespace ms
{
    namespace
    {
        // name 之后的尾部固定字段：grade(1)+level(1)+exp(4)+skill(4)+
        // opt1(4)+opt2(4)+opt3(4)+summon(1) = 23B
        constexpr size_t FAMILIAR_TAIL_FIXED = 23;

        // 读一条 0x3761 记录，字段严格按约定顺序。
        // 与 read_string() 等价（先 2B 长度头再内容），但额外做长度/尾部边界校验，
        // 防止服务端异常包导致 read_int/read_byte 越界抛 PacketError。
        bool read_entry(InPacket& recv, FamiliarEntry& out)
        {
            // 至少 familiarId(4) + 长度头(2) + 尾部固定(23)
            if (recv.available() < 4 + 2 + FAMILIAR_TAIL_FIXED)
                return false;

            out.familiarId = recv.read_int();
            const size_t name_len = static_cast<size_t>(recv.read_short());
            // name 内容 + 尾部固定字段都必须仍在包内。
            if (recv.available() < name_len + FAMILIAR_TAIL_FIXED)
                return false;
            out.name = recv.read_padded_string(static_cast<uint16_t>(name_len));

            out.grade = static_cast<uint8_t>(recv.read_byte());
            out.level = static_cast<uint8_t>(recv.read_byte());
            out.exp = recv.read_int();       // 4B，按 exp 4B/8B 教训不读 long
            out.skill = recv.read_int();
            out.opt1 = recv.read_int();
            out.opt2 = recv.read_int();
            out.opt3 = recv.read_int();
            out.summon = recv.read_byte() != 0;
            return true;
        }
    }

    void FamiliarListHandler::handle(InPacket& recv) const
    {
        if (recv.available() < 4)
            return;

        const int32_t count = recv.read_int();
        if (count < 0 || count > 1000)
        {
            LOG(LOG_WARN, "[FAM] list count invalid=" << count);
            return;
        }

        std::vector<FamiliarEntry> rows;
        rows.reserve(static_cast<size_t>(count));
        for (int32_t i = 0; i < count; ++i)
        {
            FamiliarEntry e;
            if (!read_entry(recv, e))
            {
                LOG(LOG_WARN, "[FAM] list truncated at " << i << "/" << count);
                return;
            }
            rows.push_back(std::move(e));
        }

        UIFamiliarState::get().set_list(std::move(rows));
        LOG(LOG_DEBUG, "[DBG-FAM] list count=" << count);
    }

    void FamiliarResultHandler::handle(InPacket& recv) const
    {
        if (recv.available() < 12)
            return;

        const int32_t mode = recv.read_int();
        const int32_t code = recv.read_int();
        const int32_t familiar_id = recv.read_int();

        UIFamiliarState::get().set_result(mode, code, familiar_id);
        LOG(LOG_DEBUG, "[DBG-FAM] result mode=" << mode << " code=" << code
            << " familiarId=" << familiar_id);
    }
}
