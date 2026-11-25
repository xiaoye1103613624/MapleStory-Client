# 瞬间移动技能语义审计与修复（2026-11-25）

> 用户提问：瞬移是否按技能等级提升移动距离？距离不够是否不瞬移到指向位置？向上瞬移是否没有跳跃动作？并要求检查各技能逻辑是否原生完整。
> 三端 IDA 只读取证（tp-deep，13337=Angel v83 `Angel.exe` / 13338=095 `Maplestory95.exe` / 13339=192 `MapleStoryT.exe`，hexrays 全 ready）。**真机未验**。

## 0. 三语义逐条结论

| 语义 | 三端原生 | 当前端（本轮修复前） | 判定 |
|---|---|---|---|
| ①距离随技能等级提升 | 三端 `nRange = SKILLLEVELDATA(当前lv).nRange`（v83 +412 校验+420，`_ZtlSecureFuse`；095 `_ZtlSecureTear_nRange`；192 `_ZtlSecureGet_nX`），无 `nRange+=等级*常数`；095 + Teleport Mastery 二级加成 | `Combat.cpp:371-374` 按 `get_stats(level).hrange*100` 取，随等级变化，单位一致（v83 nRange≈130 ↔ hrange≈1.3×100） | ✅ **符合核心语义**；Mastery 加成属高版本（s9/当前端均无 2111007 等技能），不移植 |
| ②距离不足/落点非法不移动 | 三端落点须 foothold 吸附、y=ground-1；目标 foothold 上下皆无、或路径怪体(CanGoThrough ±20/+65)阻挡 → **完整施放放弃，保持原位**（v83 :228/:297 return 0） | 水平无站脚点→原样保持(return)，✅；但只查下方 foothold、无上方、不吸附 y、无怪体过滤 | ⚠️ 部分符合：本轮补上下取近 + y=ground-1；怪体过滤/倒立取反因本端无原语显式降级 |
| ③向上瞬移无跳跃动作 | 三端垂直分支**全程无 vforce/impact/FALL/跳跃动画**，直接写坐标到 ground-1 + SetMovePathAttribute(4)（v83 :234-302、095 :341-396、192 调用图同构），只播 Teleport 光圈 | `Player.cpp` 垂直分支 `vforce += vdir*(dist/20)` + `set_state(FALL)` —— **物理上就是跳跃+落地** | ❌ **真 bug**，本轮修复 |

## 1. 本轮修复（代码行号）
- `Gameplay/Physics/FootholdTree.h:54` / `.cpp`：新增 `get_fhid_above`/`get_y_above`，对称于下方查询，供向上瞬移取上方落脚点。
- `Character/Player.cpp apply_teleport`：
  - 水平分支：同时查上下 foothold 取 |ground-feet_y| 近者，`set_x` + `set_y(ground-1)` + `onground=true` + `snapped=true`（瞬时吸附，不再交物理下坠）。
  - 垂直分支：删 `vforce` 冲量；UP 取上方、DOWN 取下方 foothold，无落点→保持原位(return)；有→`set_y(ground-1)`、`vspeed/vforce=0`、`onground=true`、`snapped=true`。
  - 共享 `moved` 块：`if(!snapped)` 才设 `onground=false` + `set_state(FALL)`；吸附分支跳过，只保留传送光圈与 120ms 节流。
- 语义①高版本 Mastery 加成不移植：s9 服务端与当前端均 0 命中 2111007/2211007/2311007/32111010。

## 2. 《各技能逻辑是否原生完整》盘点（skill-audit）
根本事实：当前端 `Skill.cpp` **396 行** vs MinMS **883 行**——当前端是 Journey 5 方法骨架 + 位移/动效补丁。缺口集中在**表现层**与**伤害数字近似**：
- 表现层（原生共同、应修）：finalattack 动作池、蓄力元素增幅(charge)、爬梯/绳施法帧(ladder2/rope2)、RANGED/MAGIC 姿态强制(4121007 等)、NINJA_STORM 击退 / BACK_SPIN_BLOW 聚怪、get_bullet 旋转周期。
- 数字层（s9 权威结算，改前须确认服务端是否已算，避免双重计算）：魔法 mastery 下限、被动增幅族(BURST_FIRE/ADVANCED_CHARGE/BULLSEYE/ELEMENTAL_BOOST)、SACRIFICE 强制无视防御、ASSASSINATE 潜行倍率。
- 已实现闭环：元素克制、二段跳/瞬移+施放动效、战神连击(Combat.cpp:256-266)、finish 挂接、5201006 后跃。
- s9 适配保持：去 job.can_use、Rush 磁吸不修（三端证实 MinMS 独门）、元素克制 1 点免疫兜底。
- 版本分叉不盲改：Demon/Evan/Dual/Mech 核心技能（v83 后职业，s9 范围外）。
- 独立立项：KeyDown 蓄力状态机（三端原生齐、当前端 Net/ 零 charge 命中；s9 有 attack.charge 通道但需"本地蓄力条+松手发包"整体设计 + 真机手感）。

> 上述表现层/数字层盘点为**审计结论，本轮未改**（用户本轮问题聚焦瞬移三语义；技能广谱缺口需单独拍板范围，避免把 ~16 项表现层改动混进一次提交）。

## 3. 声明
全部静态 IDA 反编译 + 源码行号证据，**真机未验**：向上/下瞬移吸附落点、光圈、无跳跃动作需用户在 s9+MySQL 环境点验。192 端 hexrays 因 `positive sp value` 无法产出函数体，其语义为 disasm/调用图推断（已标注）。截图未采信（来源端未确认）。
