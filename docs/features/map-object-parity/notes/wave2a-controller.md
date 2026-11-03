# 波2-A · 控制器怪物出招 / 技能响应 / 分裂怪雪藏

日期：2026-11-03 · 分支：feature/dev_0.0.1 · 状态：代码完成，编译通过；全量链接被外部字体批次 UIAdmin.cpp:60 的 `Text::Font::A8M`（未提交在途改动）阻塞，按用户抉择等待其收尾后再提交。

## 1. 问题（修前）
- `Mob::update_movement` 永远以 rawActivity=0 上报 → s9 `MoveLifeHandler.java:96-103` 对 0 走 `canUseAttack(castPos=-1)` 判定失败，把动作打回 -1 → **被控怪物从头到尾只会走，攻击/技能动画整轮缺失**，怪物技能永不触发。
- `MoveMonsterResponseHandler` 只弹一个 "MobSk<id>" 文字场标，从不把服务端授权的下一技能交回怪物本体。
- spawn 尾段 `effect==-3`（父怪在场、父怪死后才出的分裂怪，扎昆之手/品克缤类）此前被错位读掉；即便波1正确解析了，也没有本地雪藏/唤醒逻辑——服务端对 -3 怪**不再补发 SPAWN_MOB**（PC:1554-1558 只写 `-3 + parentOid`），不接这一步这类怪永不出现。

## 2. 依据（服务端源码，逐字节仿真核对）
- 上行 `MoveMobPacket` 参数序（GameplayPackets.h:201）：oid, type, skillb(=pNibbles), skill0(=rawActivity), skill1(=skillId), skill2(=skillLv), skill3/4(=pOption 区) ↔ `MoveLifeHandler.java:69-78`。
- 状态字节段：attack `24..41`（下标 i=(b-24)/2）、skill `42..59`（下标 i=(b-42)/2）——`Mob.h Stance` 注释引 `MoveLifeHandler.java:84-110` + `LifeFactory.java:485-495`；与 IDA 子代理曾给的 12-20/21-37 段矛盾，**以服务端源码为准**（IDA 档案 §0–1.4 自认基于截断解码，不可信，见 native-map-object-re/README.md 顶部）。
- 技能授权：`moveMonsterResponse`（PC:1643-1651）`bool useSkills + short mp + byte skillId + byte level`，仅在 `canUseSkill` 通过时携带 → 客户端据此在下一节拍上报 SKILL 段 + skillId/level。

## 3. 实现
| 文件 | 改动 |
| --- | --- |
| Mob.h | 新增 public `set_next_skill(bool,int8_t,int8_t)`、`parented()/get_parent_oid()/is_dormant()/wake()`；private 字段 `skill_pending/next_skill_id/next_skill_level/dormant`；ctor 形参加 `spawn_effect/parent/parent_oid` |
| Mob.cpp ctor | `dormant = parent && effect==-3`；雪藏怪 `deactivate()+opacity 0`；skill_pending/next_skill 初始化 |
| Mob::next_move | 控制态优先消费 skill_pending（随机取已加载 skill{n} 段）；否则 `notattack` 保护下按 `randomizer.above(0.82)` 随机取 attack{n} 段出招 |
| Mob::update_movement | 上报当前 stance 段字节；SKILL 段带回 next_skill_id/level 并清 skill_pending |
| Mob::set_next_skill | 无 skill 帧的怪不承诺（避免 set_stance 钳回浪费节拍） |
| Mob::wake | dormant→false，淡入 makeactive |
| Mob::refresh | 重下发覆盖 effect/parent/dormant；仍 -3 才继续雪藏 |
| MapMobs.cpp/.h | `set_next_skill(oid,...)` 转交；`remove()` 内遍历唤醒 parent_oid==死亡 oid 的雪藏怪 |
| WaveAShellHandlers.cpp | MoveMonsterResponseHandler 改调 `get_mobs().set_next_skill(...)`，删除文字场标占位 |

## 4. 验证
- MSBuild Release x64（VS18）：上述全部 TU 编译通过；唯一残留错误为无关文件 `IO/UITypes/UIAdmin.cpp:60 error C2065 A8M`（外部字体批次在途改动，非本次引入）。
- 逻辑不变量：出招只发生在 `control==true`（本端受控怪）；`notattack`（info.notAttack）怪与 HIT/JUMP 被动态不出招；服务端 canUseAttack 冷却不通过时把 rawActivity 打回 -1，表现为“只走不打”，无崩溃、无作弊风险（未改评分/协议安全面）。

## 5. 待办（后续切片）
- 出招时机/概率与原生对齐度：等 IDA 修正后的字符串池证据或联机实测调整 0.82 阈值与 attack 槽分布。
- CharSpawn 尚无 team 字段 → SpawnChar 修正里 team 暂 `(void)`，分队着色波 4 前接入。
- 出生特效（effect>0，Mob/SpecialEff 类）：本数据 Mob 目录只有逐怪 img + QuestCountGroup，无 SpecialEff.img → 资源缺失，波 3 显式预留或从资源站补齐后再播。
