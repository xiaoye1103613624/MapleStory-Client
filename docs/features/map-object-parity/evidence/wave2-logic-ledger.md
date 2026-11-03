# 波2 · 行为逻辑还原对账单

日期：2026-11-03 · 状态：代码完成，全部 TU 编译通过；全量链接被外部字体批次 UIAdmin.cpp:60 的 `Text::Font::A8M` 阻塞（用户已裁决「等字体批次收尾」）。

## 已完成并编译通过

| 子系统 | 文件 | 依据（权威） | 修前 → 修后 |
| --- | --- | --- | --- |
| 控制器怪出招 | Mob.h/.cpp, MapMobs.h/.cpp, WaveAShellHandlers.cpp | s9 MoveLifeHandler.java:84-110（attack 24..41 / skill 42..59）；moveMonsterResponse PC:1643-1651 | 受控怪只走不打 → 本地随机 attack 段、服务端授权后上报 skill 段 |
| 技能响应接线 | WaveAShellHandlers.cpp MoveMonsterResponseHandler | PC:1643-1651 | 弹文字场标 → set_next_skill 交回怪本体 |
| 分裂怪雪藏/唤醒 | Mob.cpp, MapMobs.cpp | spawnMonsterInternal PC:1554-1558（-3 不再补发 spawn） | -3 怪永不出现 → 雪藏至父怪死亡 wake() |
| 出生特效存储 | Mob.h/.cpp, Spawn.h/.cpp | spawnMonsterInternal PC:1449-1564 | effect 字节丢失 → spawn_effect/parent/parent_oid 存储（波3 播特效，资源缺失待补） |
| Boss 血条填充 | UIBossHP.h/.cpp | REA 反编译 sub_56C9B7 + 运行时 JSON（gauge 1×9） | fill_w 算后 (void) 丢弃 → gauge 分件横向拉伸绘制 |
| 反应堆打击音效 | Reactor.h/.cpp | IDA sub_989ADD（字符串 3073 "Sound/Reactor.img/%s/%s" + 5543 "Hit"，动作码 51） | TODO 静音 → hit/break 播放 Sound/Reactor.img/{rid}/Hit |
| ShowForeignEffect 等级 | MapObjectHandlers.cpp | PC:3488-3537 三形态（rem=10 旧 / rem=3 Rien/berserk） | effectId 误当等级 → 按剩余长度取真等级 |
| DamagePlayer -4 不显示 | MapObjectHandlers.cpp | PC:2635（skill==-4 无显示） | -4 也弹数字 → 静默 |
| RemoveLoot 宠物拾取 | MapObjectHandlers.cpp | PC:2586-2613 + Character.java:2078（anim=5+pet slot） | 宠物拾取 looter 恒空 → 宠物拾取也设 looter |
| SpawnChar 店招/team | MapObjectHandlers.cpp | spawnPlayerMapObject PC:2033-2056 + addAnnounceBox PC:2203-2223 | 店招读不出、team 错位 → 公告箱正确解析、team 取包尾 |

## 显式预留（证据不足，不瞎猜）

| 项 | 阻塞原因 | 下一步 |
| --- | --- | --- |
| 传送门全类型动画 | Portal::Type 15 种 → 动画节点映射缺完整 IDA 证据（只确证 pg/pv/ph 存在，映射表未定位） | 补 IDA 取证原生 CField::Load 或 portal pool 的 type→anim 表后实现 |
| 反应堆 default 悬浮动画 | Reactor.cpp 注释 TODO（horntail floating），原生 sub 未定位 | 同上 |
| 出生特效实际播放 | Mob/SpecialEff.img 在本数据缺失（Mob 目录只有逐怪 img + QuestCountGroup） | 资源补齐后再接 effect>0 播放 |
| Boss 血条多目标队列 | 原生缓存规则（多 mob 轮换）未取证 | 补 IDA 后实现 |

## 编译状态
- 本波全部 TU：编译通过（0 error）。
- 全量链接阻塞：`IO/UITypes/UIAdmin.cpp:60 error C2065 'A8M'` —— 未提交的外部字体批次（R28-B5），非本波引入；HEAD 无此行，Text.h 无 A8M 枚举项。等该批次收尾后重链。
