////////////////////////////////////////////////////////////////////////////////
//  UIForceState — 神秘之力 / 奥术之力 S→C 数据的单例落地
//               （供 handler 写入、UI 读取）
//
//  字段严格按服务端 impl-force agent 约定（4×int，无其它载荷）：
//   · FORCE_INFO 0x3781：
//       int arcaneForce + int authenticForce + int arcaneLevel + int authenticLevel
//
//  收包线程只写这四个 POD + seq，UI 从主线程读取；粒度与 UIFamiliarState /
//  UIAuctionState 同口径：Force 信息不是高速流，最坏读到上一帧快照，
//  均摊延迟低于 1 帧，故不引入额外队列/互斥。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>

namespace ms
{
    // 一条 0x3781 快照：字段顺序与服务端写出顺序一一对应。
    struct ForceInfo
    {
        int32_t arcaneForce = 0;      // 神秘之力
        int32_t authenticForce = 0;   // 奥术之力
        int32_t arcaneLevel = 0;      // 神秘之力等级
        int32_t authenticLevel = 0;   // 奥术之力等级
    };

    class UIForceState
    {
    public:
        // 单个进程内唯一实例；函数内静态跨 translation unit 只生成一份
        // （C++11 线程安全）。
        static UIForceState& get()
        {
            static UIForceState instance;
            return instance;
        }

        // 0x3781 完整覆盖：handler 解析完 4 个 int 后一次性提交；
        // seq 递增供 UI 的 draw 比对重建文本行。
        void set_info(const ForceInfo& info)
        {
            data = info;
            ++seq;
        }

        ForceInfo data;
        uint32_t seq = 0;
    };
}
