////////////////////////////////////////////////////////////////////////////////
//  UIFamiliarState — 精灵收藏 S→C 数据的单例落地（供 handler 写入、UI 读取）
//
//  字段严格按 lead 下发的协议约定（服务端 impl-familiar agent 实现，按此读取）：
//   · FAMILIAR_LIST  0x3761：int count + ×{int familiarId, string name,
//                             byte grade, byte level, int exp, int skill,
//                             int opt1, int opt2, int opt3, byte summon}
//   · FAMILIAR_RESULT 0x3762：int mode + int code + int familiarId
//
//  收包线程只写这些 POD/vector，UI 从主线程读取；粒度与 UIAuctionState 同口径：
//  精灵列表不是高速流，最坏读到上一帧快照，故不引入额外队列/互斥。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
    // 单条精灵记录：字段顺序与 0x3761 写出顺序一一对应。
    struct FamiliarEntry
    {
        int32_t familiarId = 0;   // 精灵模板 id
        std::string name;         // 精灵名（服务端 writeAscii/writeMapleText）
        uint8_t grade = 0;        // 品级
        uint8_t level = 0;        // 等级
        int32_t exp = 0;          // 经验（4B，勿按 8B 读，参考 exp 4B/8B 错位教训）
        int32_t skill = 0;        // 技能 id
        int32_t opt1 = 0;         // 附加选项 1
        int32_t opt2 = 0;         // 附加选项 2
        int32_t opt3 = 0;         // 附加选项 3
        bool summon = false;      // 是否出战（byte 0/1）
    };

    class UIFamiliarState
    {
    public:
        // 单个进程内唯一实例；函数内静态跨 translation unit 只生成一份（C++11 线程安全）。
        static UIFamiliarState& get()
        {
            static UIFamiliarState instance;
            return instance;
        }

        // --- 0x3761 列表 ---
        std::vector<FamiliarEntry> list;
        uint32_t list_seq = 0;    // 列表每次完整覆盖后 +1，UI 据此重建渲染行

        // --- 0x3762 操作反馈 ---
        int32_t last_mode = 0;
        int32_t last_code = 0;
        int32_t last_familiar_id = 0;
        uint32_t result_seq = 0;

        // 列表完整覆盖：handler 解析完 count 条后一次性提交，避免半截列表被 UI 看见。
        void set_list(std::vector<FamiliarEntry>&& rows)
        {
            list = std::move(rows);
            ++list_seq;
        }

        // 单条更新（升级/召唤结果落地后刷新某条展示状态），未匹配 id 时无操作。
        void update_entry(int32_t familiar_id, uint8_t grade, uint8_t level,
                          int32_t exp, int32_t skill, int32_t opt1, int32_t opt2,
                          int32_t opt3, bool summon)
        {
            for (auto& e : list)
            {
                if (e.familiarId != familiar_id)
                    continue;
                e.grade = grade;
                e.level = level;
                e.exp = exp;
                e.skill = skill;
                e.opt1 = opt1;
                e.opt2 = opt2;
                e.opt3 = opt3;
                e.summon = summon;
                ++list_seq;
                return;
            }
        }

        void set_result(int32_t mode, int32_t code, int32_t familiar_id)
        {
            last_mode = mode;
            last_code = code;
            last_familiar_id = familiar_id;
            ++result_seq;
        }
    };
}
