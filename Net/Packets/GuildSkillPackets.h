////////////////////////////////////////////////////////////////////////////////
//  GUILD_SKILL_ACTION (C→S 0x3790 = 14224) — 197 TMS280 公会技能请求包
//
//  与服务端 impl-guildskill agent 约定（lead 已确认）：
//   mode=0  查询：仅 int mode，无附加字段（服务端回 0x3791 列表）
//   mode=1  购买：int mode + int skillId
//   mode=2  升级：int mode + int skillId
//
//  注意：skillId 用 int32 写出（4B），与服务端 readInt 对齐；
//  严格遵循字段顺序，避免 4B/8B 错位。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <cstdint>

namespace ms
{
    class GuildSkillActionPacket : public OutPacket
    {
    public:
        // mode=0 查询公会技能列表：服务端回 GUILD_SKILL_LIST(0x3791)
        static GuildSkillActionPacket query()
        {
            return GuildSkillActionPacket(0);
        }

        // mode=1 购买技能：带 int skillId
        static GuildSkillActionPacket buy(int32_t skill_id)
        {
            GuildSkillActionPacket p(1);
            p.write_int(skill_id);
            return p;
        }

        // mode=2 升级技能：带 int skillId
        static GuildSkillActionPacket level_up(int32_t skill_id)
        {
            GuildSkillActionPacket p(2);
            p.write_int(skill_id);
            return p;
        }

    private:
        explicit GuildSkillActionPacket(int32_t mode)
            : OutPacket(OutPacket::Opcode::GUILD_SKILL_ACTION)
        {
            write_int(mode);
        }
    };
}
