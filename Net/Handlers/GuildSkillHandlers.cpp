////////////////////////////////////////////////////////////////////////////////
//  公会技能 S→C handler 实现
//
//  【字段对齐铁律】服务端写什么顺序，客户端就按什么顺序读；每个字段读之前
//  先 recv.available() 校验剩余字节，不足整条直接 return，避免 exp 4B/8B
//  混读错位。timestamp 为 8B long；purchaser 前的 2B 长度头由
//  InPacket::read_string() 内部消费（read_string: uint16 len + read_padded_string）。
////////////////////////////////////////////////////////////////////////////////
#include "GuildSkillHandlers.h"

#include "../../IO/UITypes/GuildSkillState.h"
#include "../../Util/Misc.h"

namespace ms
{
    namespace
    {
        // 单条目固定开销：skillId(4) + level(4) + timestamp(8) + 长度头(2) = 18B
        // （不含 purchaser 内容长度）。
        constexpr size_t GUILD_SKILL_ENTRY_FIXED = 18;

        // 读一条 0x3791 记录，字段严格按约定顺序。
        // 先判固定头 18B，再读长度头并校验 purchaser 内容长度仍在包内，
        // 防止服务端异常包导致越界抛 PacketError。
        bool read_entry(InPacket& recv, GuildSkillEntry& out)
        {
            if (recv.available() < GUILD_SKILL_ENTRY_FIXED)
                return false;

            out.skillId = recv.read_int();
            out.level = recv.read_int();
            out.timestamp = recv.read_long();
            const size_t name_len = static_cast<size_t>(recv.read_short()); // string 2B 长度头
            if (recv.available() < name_len)
                return false;
            out.purchaser = recv.read_padded_string(static_cast<uint16_t>(name_len));
            return true;
        }
    }

    void GuildSkillListHandler::handle(InPacket& recv) const
    {
        if (recv.available() < 4)
            return;

        const int32_t count = recv.read_int();
        if (count < 0 || count > 1000)
        {
            LOG(LOG_WARN, "[GSKILL] list count invalid=" << count);
            return;
        }

        std::vector<GuildSkillEntry> rows;
        rows.reserve(static_cast<size_t>(count));
        for (int32_t i = 0; i < count; ++i)
        {
            GuildSkillEntry e;
            if (!read_entry(recv, e))
            {
                LOG(LOG_WARN, "[GSKILL] list truncated at " << i << "/" << count);
                return;
            }
            rows.push_back(std::move(e));
        }

        GuildSkillState::get().set_list(std::move(rows));
        LOG(LOG_DEBUG, "[DBG-GSKILL] list count=" << count);
    }

    void GuildSkillResultHandler::handle(InPacket& recv) const
    {
        if (recv.available() < 12)
            return;

        const int32_t mode = recv.read_int();
        const int32_t code = recv.read_int();
        const int32_t skill_id = recv.read_int();

        GuildSkillState::get().set_result(mode, code, skill_id);
        LOG(LOG_DEBUG, "[DBG-GSKILL] result mode=" << mode << " code=" << code
            << " skillId=" << skill_id);
    }
}
