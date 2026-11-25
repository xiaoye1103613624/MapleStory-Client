////////////////////////////////////////////////////////////////////////////////
//  GuildSkillState — 197 TMS280 公会技能 S→C 数据的单例落地
//                     （供 handler 写入、UIGuildSkill 读取）
//
//  字段严格按服务端 impl-guildskill agent 下发的协议约定（按此读取）：
//   · GUILD_SKILL_LIST   0x3791 = 14225：int count
//                          + ×{int skillId, int level, long timestamp, string purchaser}
//   · GUILD_SKILL_RESULT 0x3792 = 14226：int mode + int code + int skillId
//                          （code 0成功 / 1权限不足 / 2积分不足 / 3已满级）
//
//  【字段对齐铁律】每条记录 int(4)+int(4)+long(8)+string(2B 长度头+N) =
//  固定 18B + purchaser 内容；timestamp 是 8B long，勿按 int 读；
//  string 前有 2 字节长度头（InPacket::read_string 已含），勿漏。
//
//  收包线程只写这些 POD/vector，UI 从主线程读取；粒度与 UIAuctionState /
//  UIFamiliarState / VCoreState 同口径——公会技能列表不是高速流，
//  最坏读到上一帧快照，故不引入额外队列/互斥。set_list 一次性整体覆盖，
//  避免半截列表被 UI 看见。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
    // 单条公会技能记录：字段顺序与 0x3791 写出顺序一一对应。
    struct GuildSkillEntry
    {
        int32_t skillId = 0;      // 技能 id（int，4B）
        int32_t level = 0;        // 当前等级（int，4B）
        int64_t timestamp = 0;    // 购买/升级时间戳（long，8B）
        std::string purchaser;    // 购买人（string，2B 长度头 + 内容）
    };

    class GuildSkillState
    {
    public:
        // 单个进程内唯一实例；函数内静态跨 translation unit 只生成一份（C++11 线程安全）。
        static GuildSkillState& get()
        {
            static GuildSkillState instance;
            return instance;
        }

        // --- 0x3791 列表 ---
        std::vector<GuildSkillEntry> list;
        uint32_t list_seq = 0;     // 列表每次完整覆盖后 +1，UI 据此重建渲染行

        // --- 0x3792 操作反馈 ---
        int32_t last_mode = 0;
        int32_t last_code = 0;
        int32_t last_skill_id = 0;
        uint32_t result_seq = 0;

        // 列表完整覆盖：handler 解析完 count 条后一次性提交，避免半截列表被 UI 看见。
        void set_list(std::vector<GuildSkillEntry>&& rows)
        {
            list = std::move(rows);
            ++list_seq;
        }

        // 操作反馈（购买/升级）；UI 可据此提示成功/失败。
        void set_result(int32_t mode, int32_t code, int32_t skill_id)
        {
            last_mode = mode;
            last_code = code;
            last_skill_id = skill_id;
            ++result_seq;
        }
    };
}
