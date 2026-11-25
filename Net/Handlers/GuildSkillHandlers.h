////////////////////////////////////////////////////////////////////////////////
//  公会技能 2 个 S→C handler（0x3791 / 0x3792）
//  字段读取顺序严格按服务端 impl-guildskill agent 下发的协议：
//   0x3791 GUILD_SKILL_LIST：int count + ×{int skillId, int level,
//                            long timestamp, string purchaser}
//   0x3792 GUILD_SKILL_RESULT：int mode + int code + int skillId
//   （write 顺序即 read 顺序）
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../PacketHandler.h"

namespace ms
{
    // 0x3791：int count + ×{int skillId, int level, long timestamp, string purchaser}
    class GuildSkillListHandler : public PacketHandler
    {
    public:
        void handle(InPacket& recv) const override;
    };

    // 0x3792：int mode + int code + int skillId
    class GuildSkillResultHandler : public PacketHandler
    {
    public:
        void handle(InPacket& recv) const override;
    };
}
