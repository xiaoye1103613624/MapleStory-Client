////////////////////////////////////////////////////////////////////////////////
//  VCoreState — V 矩阵/核心 S→C 数据的单例落地（供 handler 写入、UIVMatrix 读取）
//
//  字段严格按服务端协议约定（impl-vcore agent 实现；0x3771/0x3772 回包布局不变）：
//   · VCORE_LIST   0x3771：int count + ×{int vcoreid, byte slot, byte level,
//                           int skill1, int skill2, int skill3, int index}
//   · VCORE_RESULT 0x3772：int action + int code + int vcoreid
//
//  【字段对齐铁律】每条记录 4+1+1+4+4+4+4 = 22 字节，slot/level 为 byte(1B)、
//  skill1-3/index 为 int(4B)，切勿按 int 读 slot/level 或把 index 读成 byte
//  （参考历史上 exp 4B/8B 错位教训）。
//
//  收包线程只写这些 POD/vector；UI 从主线程读取；粒度与 UIAuctionState /
//  UIFamiliarState 同口径——核心列表不是高速流，最坏读到上一帧快照，
//  故不引入额外队列/互斥。set_list 一次性整体覆盖，避免半截列表被 UI 看见。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <vector>

namespace ms
{
    // 单条核心记录：字段顺序与 0x3771 写出顺序一一对应（共 22B）。
    struct VCoreEntry
    {
        int32_t vcoreid = 0;   // 核心 id（int，4B）
        uint8_t slot = 0;      // 槽位（byte，1B）
        uint8_t level = 0;     // 等级（byte，1B）
        int32_t skill1 = 0;    // 技能 1（int，4B）
        int32_t skill2 = 0;    // 技能 2（int，4B）
        int32_t skill3 = 0;    // 技能 3（int，4B）
        int32_t index = 0;     // 索引（int，4B）
    };

    class VCoreState
    {
    public:
        // 单个进程内唯一实例；函数内静态跨 translation unit 只生成一份（C++11 线程安全）。
        static VCoreState& get()
        {
            static VCoreState instance;
            return instance;
        }

        // --- 0x3771 列表 ---
        std::vector<VCoreEntry> list;
        uint32_t list_seq = 0;    // 列表每次完整覆盖后 +1，UI 据此重建渲染行

        // --- 0x3772 操作反馈 ---
        int32_t last_action = 0;
        int32_t last_code = 0;
        int32_t last_vcoreid = 0;
        uint32_t result_seq = 0;

        // 列表完整覆盖：handler 解析完 count 条后一次性提交，避免半截列表被 UI 看见。
        void set_list(std::vector<VCoreEntry>&& rows)
        {
            list = std::move(rows);
            ++list_seq;
        }

        // 操作反馈（装备/卸下/强化/分解）；UI 可据此提示成功/失败。
        void set_result(int32_t action, int32_t code, int32_t vcoreid)
        {
            last_action = action;
            last_code = code;
            last_vcoreid = vcoreid;
            ++result_seq;
        }
    };
}
