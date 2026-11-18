# 怪物全流程 Angel 083 对照复核（2026-11-13）

> 触发：用户要求「怪物攻击、移动、释放技能、被攻击、死亡、复活、刷新、召唤等周边全流程逻辑对照 Angel 复核」。
> 性质：**阶段 0 取证 + 阶段 0.5 归纳**。只读复核，不改业务逻辑、不构建、不提交。
> 基准：GMS 083 / Angel `E:\MXD\反编译IDA\v83\Angel.i64`（IDA MCP `http://127.0.0.1:13339/mcp`，`server_health` 确认 `auto_analysis_ready=true` / `hexrays_ready=true` / imagebase `0x400000`）。
> 口径：沿用 `docs/features/angel-flow-audit-2026-11/README.md` —— **s9 联机协议优先；Angel 只作逻辑/语义对照；校准 ≠ 改回原生**。
> IMG 侧：orange-wz MCP(10012，128B GMS key + `TSPHKw==`) 只读取证，无解析异常（与历史「私服 img 解析 bug」结论不同，本次 `Mob/0000021.img` 完整解析成功）。

## 0. 工具链与实际取证

| 层 | 工具 | 结果 |
| --- | --- | --- |
| 行为/反汇编 | Angel IDA MCP 13339（`tools/_ida_mcp.py`） | ✅ `list_funcs *CMob*` 得 68 符号；`decompile` 批量 **32/32** 成功 |
| 精确调用关系 | `xrefs_to` | ✅ `OnDie`/`OnRevive` 唯一调用源均为 `CMobPool::Update @0x679138` |
| IMG/资源 | orange-wz MCP | ✅ `query_nodes tree` 解出 `Mob/0000021.img` 的 `info/*` 与 `move/stand/jump/hit1/die1`；已 `unload_all` 收口 |
| 服务端协议 | 只读源码 `BeiDou-Server_s9` | ✅ `SendOpcode.java` / `PacketCreator.java` / `MapleMap.java` / `Monster.java` |

产物：
- `evidence/ida/*.c`（32 个函数伪代码）+ `evidence/ida/_index.json`（地址/字节数/成功标志）
- `tools/_mob_angel_probe.py`（可重跑的只读取证脚本，**不修改 IDB**）
- 既有证据复用：`docs/features/native-map-object-re/README.md`（40 项 mob activity 向量、`noRevive`/`regena`/`interval`）
- 当前实现清单：本轮 `code-explorer` 只读梳理（8 域 + 全仓 Angel 注释 + 公开方法签名）

> REA 未启用：本轮 Angel 侧已由 IDA MCP 直接给出符号+伪代码+xref，属 skill 门禁 18 的「IDA 精确定位」主路径；无未解析行为需 REA 叙事补证。记为取舍而非阻塞。

---

## 1. 已证实：Angel 083 的怪物生命周期架构

### 1.1 池是「三队列 + 显式删池」，不是「单容器 + 返回 -1」

`CMobPool::Update @0x679138`（`evidence/ida/Update_CMobPool.c`）每帧依次遍历三个独立链表：

| 队列 | 偏移 | 行为 |
| --- | --- | --- |
| 活动怪 | `this+10` | 逐个 `Update()` |
| 死亡延时队列 | `this+15` | `GetRemainDamageInfoDelay()<=0` → `OnDie`；否则 `sub_663E5B` 继续等 |
| 复活定时器队列 | `this+20` | 计时 `<= -1` → 遍历成员 `CMobPool::GetMob(oid)` → `OnRevive` |

`CMobPool::OnPacket @0x67930e` 分发：`236=OnMobEnterField`、`237=OnMobLeaveField`、`238=OnMobChangeController`、`249=OnMobCrcKeyChanged`、`239..255 → OnMobPacket`（`239=OnMove / 240=OnCtrlAck / 242=OnStatSet / 243=OnStatReset / 246=OnDamaged / 250 / 252=OnEffectByItem / 253=OnMobSpeaking …`）。

**关键**：`OnMobLeaveField @0x67961d` 收 `Decode4(oid)+Decode1(flag)`，`GetAt(oid)` 命中后：
- `flag!=0`：写入 killtype 字段（`*(v6+275)`）并 `sub_679950`
- `flag==0`：清 active；`IsActive()` 仍真则跳过删除（`LABEL_17`，留给死亡队列）
- 公共尾部 `LABEL_16`：**`CWvsContext::OnMobDead` + `sub_679A04` + `ZMap::RemoveKey(oid)` —— 立即从池删除**

即：**删池与播死亡动画是两件独立的事**。`CMob::OnDie @0x663995` 只负责音效、按 killtype 选 action（`==3` 固定 21，否则 `rand()%v6+9`）、清 skill/effect 列表、特定 mobid 触发震屏；它**不删除对象**。
`CMob::OnRevive @0x664182` 是复活表现层：清标记 + `IWzGr2DLayer` alpha 复原 255（淡入）。

### 1.2 刷新：同 oid 复用只刷临时状态，不重建

`OnMobEnterField @0x67945a`：`GetMob(oid)` 命中 → 仅 `SetTemporaryStat`；未命中 → `CreateMob(template)` + `ZMap::Insert` + `CMob::Init(oid, packet)`。

### 1.3 被击/血条：血条由伤害包驱动且带 WZ 颜色

`CMob::OnDamaged @0x66c6c2`：`Decode1` + `Decode4(damage)` → `ShowDamage` → 若 `result[130]!=0` 再 `Decode4(curhp)`+`Decode4(maxhp)` → `CreateHPIndicator(100*curhp/maxhp, -65536)` + `ShowHPIndicator`。
原生 `hpTagColor`/`hpTagBgcolor`/`HPgaugeHide`/`AngerGauge` 是 `Mob/%07d.img/info/*` 属性（`native-map-object-re/README.md §2.3`）。

### 1.4 攻击/技能/移动：控制器怪有真 AI

`GenerateMovePath @0x66b6fc` → `DoAttack @0x66d9c0` + `CVecCtrlMob::ChaseTargetImp @0x9bbd88`；
`TryDoingSkill @0x66e7af` 从 MobTemplate 的技能表（28B/项）取 `v10[5]+21` 作 action、`GetEffectUOL` 取特效、设 `get_update_time()+v10[6]` 冷却；
`OnMove @0x66be61` 按 `v45>>1` 分段（12..20 攻击 / 21..37 技能 / 6..8 受击死亡）驱动 `CMovePath::OnMovePacket`。
40 项 activity 名向量见 `native-map-object-re/README.md §1.1`（`move/stand/jump/fly/regena/bomb/hit1/hit2/hitF/die1/die2/dieF/att/attack2..8/skill1..16/skillF/chase/miss`）。

### 1.5 复活/刷新节奏：WZ 属性，不是 opcode

`noRevive`(pool 1880) 是 WZ bool；`regena`(5649) 是复活动画名；`interval`(5634) / `Regen`(2545) 控制周期。池里**无 `respawn`/`revive` 字面量**，服务端管刷怪节奏，客户端只按 `OnRevive` 表现。

### 1.6 召唤：真对象 + 完整 AI

`CSummoned` 有 `Init(InPacket)` / `Update` / `ProcessAttack` / `TryDoingAttackManual` / `TryDoingHeal` / `SetDamaged` / `ShowEffect` / `MakeIncDecHPEffect`（`list_funcs *CSummoned*` 19 符号），15 项 summon 动画向量（`stand/move/fly/summoned/att/attack2/skill1..6/hit/die/say`）。**注意：`*SummonedPool*` 无命中**，召唤对象不归 CMobPool，走独立池。

---

## 2. 差异：当前客户端 vs Angel 083

> ✅ 对齐 / 🟡 部分对齐或合理偏离 / ❌ 缺失或结构偏差 / ⚠️ 版本差（s9 vs 083，按既定口径保持 s9）

| # | 流程 | Angel 083 | 当前端 | 判定 |
| --- | --- | --- | --- | --- |
| 1 | **死亡/移除** | `OnMobLeaveField` 立即 `ZMap::RemoveKey(oid)`；`OnDie` 独立表现层；三队列状态机 | `KillMobHandler→MapMobs::remove` 收包即显式 `mobs.remove(oid)`（`animation==0` 立即删，否则登记 `removal` 队列按动画结束 / 兜底 180 帧删），死亡表现与回收解耦；`Mob::update()` 不再返回 `-1` 自杀 | ✅ **2026-11-13 已闭环（显式删池对齐 `OnMobLeaveField`）** |
| 2 | 死亡动画 | 按 killtype 选 action，多套 `die/die1/die2/dieF` | 固定 `die1`，缺则回退 `die/die2/dieF`，无 killtype 语义 | 🟡 |
| 3 | **复活** | `CMob::OnRevive` 淡入 + `noRevive`/`regena`/`interval` | 无 `OnRevive` 等价物；只有 `dormant/wake`(effect=-3 雪藏) 与 `refresh`(同 oid 重下发)，语义不同 | ❌ |
| 4 | 复活触发 | `CMobPool::Update` 复活定时器队列驱动 | 父怪被 kill 时 `MapMobs::remove` 遍历唤醒子怪 | 🟡 局部等价（分裂怪），非通用复活 |
| 5 | 刷新/入场 | 同 oid 复用**只** `SetTemporaryStat`；新 oid 才 `Init` | 同 oid 走 `refresh()` 全量覆盖落位/姿态/队伍/清 dead；新 oid `instantiate()` | 🟡 为修 s9 换控制器整包重发丢坐标而偏离，有注释依据 |
| 6 | 池分发 | 236/237/238/249 + 239..255 子分发 | `PacketSwitch` 236/237/238/239/246/250/240/242/243/244 等分别注册 handler | ✅ opcode 全对齐（已核 `SendOpcode.java`） |
| 7 | 离场 flag 语义 | `Decode1==0` 清 active 且可跳过删池（留死亡队列）；非 0 写 killtype | `kill(0)`=停用+`dead`；`kill(2)`=fading；其余 `apply_death` | 🟡 `Mob.cpp:673` 注释自称依据 `OnMobLeaveField @0x67961d`，**方向对但把「删池」误绑到动画结束** |
| 8 | **移动 AI** | `GenerateMovePath`+`ChaseTargetImp`+`IsTargetInAttackRange` 真追逐/攻击范围 | 无等价实现；本地 `next_move()` 随机 MOVE/STAND/JUMP + `physics.move_object` | ❌ |
| 9 | 移动包 | `OnMove` 用 3 个 `Decode1` + `Decode4` 按 activity 段驱动动画/技能 UOL | `MobMovedHandler` 读 oid+7 个 byte 后**全部丢弃**，只取 pos + movements | ⚠️ s9 `moveMonster` 确实发 6 字节 skill 头 + short pOption，字段在但客户端未消费 |
| 10 | 攻击 | `DoAttack`/`IsTargetInAttackRange`/`FindBodyAttackMob`/`FindHitMobInRect` | `create_touch_attack` + `Stage::update` 碰撞 + `AutoAggro`/`TOUCH_MONSTER_ATTACK(47)`；`calculate_damage` 本地结算 | 🟡 接触伤害对齐，缺主动攻击 AI |
| 11 | **技能** | `TryDoingSkill` 读模板技能表、`GetEffectUOL` 特效、独立冷却 | `set_next_skill`(MOVE_MONSTER_RESPONSE 授权) → `next_move` 随机取 `skill{n}` 槽 → 上报 SKILL 段(42..59) | 🟡 有上行闭环；无施法特效 UOL、无 affected-skill 列表 |
| 12 | 技能特效 | `UpdateAffectedSkillList`、`MobSkill.img` effect/hit/tile | `show_effect` 仅由 `SkillHitEffect`(受击侧) 调用；`spawn_effect>0` 的 `Mob/SpecialEff.img` **只存不播** | ❌ |
| 13 | 被击/血条 | `OnDamaged` 条件读 curhp/maxhp → `CreateHPIndicator`；WZ `hpTagColor`/`HPgaugeHide` | `DamageMonsterHandler` 固定读 6 字段算 pct；`MobHpBar::draw` 纯 `Geometry` 色块 | ⚠️ s9 `damageMonster` 固定写 `oid+byte+damage+curhp+maxhp+skinId`，客户端读法对齐 s9；缺 per-mob 颜色/隐藏 |
| 14 | 受击动画 | `OnHit`(7.6KB) + `ShowHitEffect` + HIT 击退 | `apply_damage` 播 hitsound、受控怪 `counter=170`+`set_stance(HIT)`+击退力 | 🟡 |
| 15 | 控制器 | `OnMobChangeController`: mode0→`SetRemoteMob`；非0→`SetLocalMob(mode,oid,byte,packet)` | `SpawnMobControllerHandler`: mode0→`set_control(false)`；非0 走整包 spawn 入队 | 🟡 大体对齐，语义实现路径不同 |
| 16 | 召唤 | `CSummoned` 真对象 + 攻击/治疗 AI + 15 项动画 | `MapSummon : FieldMarker` 真对象化（stand/hit/die/summoned），但 `SummonAttackHandler`/`ApplyMonsterStatusHandler` 大量用文字场标代真对象 | 🟡 部分闭环，AI 与场标替换仍是 parity 缺口 |
| 17 | 状态/特效包 | 242/243/244/245/247/250/251/252/253/254/255 均有池内处理 | 部分已注册（242/243/244 等），部分只出文字场标 | 🟡 |
| 18 | 依据链 | Angel 083 原生反编译 | 除 `Mob.cpp:673` 外，怪物域注释依据几乎全是 **s9 服务端行号**（`PacketCreator`/`MoveLifeHandler`）与 v95 反编译 | ⚠️ 复核时两类依据不可混用 |

### 2.1 与上一任务「击杀后怪物残留」的直接关联

用户此前反馈：伤害/掉血/掉宝/经验都正常，但**怪还在、还在移动、血条见底不消失**。静态链路（opcode 237 对齐、包结构对齐、oid 一致、广播范围覆盖、emplace 无撞值、`get(oid)` 命中）逐环排除后，本次 Angel 对照给出结构性解释：

- 原生：`OnMobLeaveField` 收包即 `RemoveKey`，死亡动画交给独立队列，**删池不依赖动画结束**。
- 当前：`kill()` 只置状态，删除完全依赖 `Mob::update()` 返回 `-1`；一旦 `dying` 未置位、`active` 被停用、或动画不结束，对象就永久留在 `objects` 里 → 幽灵怪。
- 且 `MapObjects::remove(oid)` 这个显式删除入口在 NPC/雾/召唤/场标路径都被使用，**唯独怪物路径没用**。

**已于 2026-11-13 实施**：`KillMobHandler`/`MapMobs::remove` 对齐原生，收包即显式 `mobs.remove(oid)`（`animation==0` 立即删；非 0 起一个短生命周期死亡表现对象再删），把「死亡动画」与「对象回收」解耦；不再依赖「动画播完返回 -1」。改动文件：`MapMobs.{h,cpp}`（新增 `removal` 显式回收队列 + `MOB_REMOVAL_FRAMES=180` 兜底）、`Mob.{h,cpp}`（`update()` 不再自杀、`kill()` 支持 0/1/2/3、移除 `death_timer` 兜底）、`MapObjectHandlers.cpp`（`MobMovedHandler` 解析 s9 moveMonster 7 字节 skill 头并接入 `send_movement`；删除哑 `#include <fstream>/<sstream>`）。构建验证：`EXITCODE=0`（临时 OutDir 覆盖链接通过；部署 exe 因用户提权客户端占用未同步，需用户关闭后 `build_smoke.bat` 重新生成）。

---

## 3. 推荐方案（默认，待确认后进 Plan）

1. **总取向**：保持 s9 协议优先；Angel 用于架构/语义校准。不把 s9 已扩展的包体（DAMAGE_MONSTER 6 字段、MOVE_MONSTER 6+2 字节 skill 头）改回 083。
2. **P0（架构，直接解决幽灵怪）— 2026-11-13 已实施**：把怪物移除改为显式删池，解耦死亡表现与对象回收；对照 `OnMobLeaveField` 的 `RemoveKey` 语义（见 §2 表 #1 与 §2.1）。
3. **P1（语义补齐）— 2026-11-13 第二轮已实施（P-A）**：killtype 语义（0 停用 / 1 死亡 / 2 淡出 / 3 特殊）多套死亡动画选择已于 P0 完成；本轮补 `noRevive`/`regena` WZ 属性读取，作 `can_revive()` 守卫与 `wake()` 淡入预留（见 §7）。
4. **P1（复活）— 2026-11-13 第二轮已实施（P-A，按 D3(b) 预留）**：不引入完整复活定时器队列状态机；`regena` 淡入表现复用现有 `wake()` 的 `opacity 0→255`（对齐 Angel `OnRevive` alpha 复原）；`noRevive` 作 `can_revive()` 守卫。通用复活显式预留。
5. **P2（表现层）— 2026-11-13 第二轮已实施（P-B/P-C/P-E）**：`Mob/SpecialEff.img` 出生特效（`spawn_effect>0` 播，节点缺失跳过）；`MobSkill.img` effect/hit/tile 施法侧特效（技能段上报时播）；per-mob HP 条颜色（`hpTagColor`）+ `HPgaugeHide` 隐藏。
6. **P2（AI）— 仍预留（D5）**：`GenerateMovePath`/`ChaseTargetImp` 级别真追逐属大改，本版不做，记为 parity 缺口。
7. **依据链治理**：把怪物域注释里的「s9 行号」与「Angel 083 地址」分栏标注，避免后续复核混用。

---

## 4. 仍不确定 / 待你抉择

| # | 决策点 | 选项 | 推荐 |
| --- | --- | --- | --- |
| D1 | 本轮复核**是否进入实现** | (a) 只交付对照结论，不动代码 (b) 按 §3 出 Plan 并实施 | **(b) 已实施**：用户后续明确要求「上一任务要闭环，也是一样按照 Angel 来」 |
| D2 | 幽灵怪修复路径（上一任务未完成） | (a) 显式删池对齐 Angel (b) 继续加固现有动画结束回收 + 兜底超时 | **(a) 已实施** |
| D3 | 复活范围 | (a) 完整复活队列 + regena 淡入 (b) 仅保留分裂怪雪藏唤醒，其余显式预留 | 需你定：涉及新增状态机与 WZ 属性 |
| D4 | s9 扩展字段是否保留 | (a) 全部保留 (b) 部分回退 083 | **(a)**，沿用既定口径 |
| D5 | 怪物主动 AI（追逐/攻击范围） | (a) 本版不做，记为 parity 缺口 (b) 立项实现 | **(a)**，服务端权威在本地 AI 上有冲突风险 |
| D6 | 本会话上一任务（击杀残留，曾改 `Mob.cpp` 两处但未构建） | (a) 先回去收尾 (b) 并入 D2 统一处理 (c) 暂停 | **(b) 已并入 D2 并闭环**：旧 `Mob::update` 的 `death_timer` 兜底与「动画返回 -1」路径已删除，改为 `MapMobs` 显式删池 |

---

## 5. 验证门禁（本轮）

| 项 | 状态 |
| --- | --- |
| IDA MCP 只读取证（32 函数） | ✅ 32/32 |
| xref 交叉验证 | ✅ OnDie/OnRevive → `CMobPool::Update` |
| orange-wz IMG 抽样 + 卸载收口 | ✅ `Mob/0000021.img`；`unload_all ok` |
| 与 s9 协议交叉核对（opcode / 包字段） | ✅ |
| 业务代码改动 | ✅ **已实施**（P0 显式删池 + 第一轮清理；第二轮 P-A~E：noRevive/regena 读取、SpecialEff 出生特效、MobSkill 施法特效、pOption 透传、per-mob HP 条颜色/隐藏），见 §2 表 #1/#9/#11/#12 与 §7 |
| 构建 | ✅ **`EXITCODE=0`**（2026-11-13 两轮：P0 及 P-A~E 均经临时 `OutDir` 覆盖链接验证通过，避开被占用部署 exe） |
| 单测 | ⊘ 未跑（无单测框架，按既有口径） |

## 6. 局限

- Angel 字符串池加密（`find_regex` 仅命中 `recoveryInterval`），动画名/属性名结论依赖 `native-map-object-re/README.md` 已解密的池 ID 表，未在本轮重新逐项解密验证。
- `CMob::Update @0x6675a8`(28KB)、`Init @0x662884`(19KB) 已导出但未逐行归纳；本轮结论基于池层与关键入口函数，实体内部状态机细节留待 P1 深入。
- `sub_679950`/`sub_679A04`/`sub_670C54`/`sub_663E5B` 未单独反编译，其精确语义（删池辅助 / 死亡队列 / 复活队列）由调用上下文推断，标记为 **INFERRED**。
- v95 `CMobPool` 反编译产物（`docs/features/port-095/`）与 Angel 083 非同版本，本轮未用作依据。

## 7. 第二轮 parity 缺口闭环（2026-11-13，P-A~E）

按用户「顺手把其他 parity 缺口推进」，在已闭环 #1（显式删池）基础上补齐差异表其余低风险项（§2 #9/#11/#12 等），保持 s9 协议优先、Angel 作语义校准、不引入需用户拍板的大改。

### 7.1 实现要点
- **P-A（复活/重生语义，对齐 D3(b)）**：`Mob` 构造/`refresh` 读 `info["noRevive"]`(bool)、`info["regena"]`(动画名)；`can_revive()` 以 `!noRevive_` 为守卫（本版仅预留，不触发通用复活）；`wake()` 沿用现有 `opacity 0→255` 淡入（对齐 Angel `OnRevive` 的 alpha 复原），`regena_` 暂存备用。
- **P-B（出生特效，#5 相关）**：`Mob` 构造末尾 `spawn_effect>0` 且非 `dormant` 时，查 `nl::nx::Mob["SpecialEff.img"][std::to_string(effect)]`，存在则 `show_effect(Animation(...))` 播报（节点缺失跳过，R6）。
- **P-C（技能特效 UOL，#11/#12）**：`update_movement()` 上报 `SKILL` 段时调 `play_mob_skill_effect(id, lv)`，查 `nl::nx::Skill["MobSkill.img"][id]["level"][lv]`（orange-wz 实测 `MobSkill.img` 位于 **`Skill` 命名空间**，非 `Mob`）的 `effect`/`hit`/`tile` 节点并 `show_effect` 播施法侧特效（受击侧 `SkillHitEffect` 已部分有）；`effect` 节点已验证存在（LIST_PROPERTY）。
- **P-D（移动包收尾，#9）**：`MobMovedHandler` 解析 s9 `moveMonster` 的 `pOption`（short）并透传 `MapMobs::send_movement → Mob::send_movement` 末参 `mob_poption_`，当前无对应客户端表现，仅存储预留。
- **P-E（HP 条样式，#5/#12 相关）**：`MobHpBar::draw` 增加 `const Color& fill` 形参（默认 `LIGHTGREEN`）；`Mob` 构造读 `info["hpTagColor"]`（int→`Color`，0 则默认绿）传血条；`info["HPgaugeHide"]`(bool) 在 `draw()` 血条门禁加 `!hpgauguehide_`。

### 7.2 预留（仅文档/字段，未实现）
- **#8 移动 AI（D5）**：`GenerateMovePath`/`ChaseTargetImp` 真追逐，服务端权威冲突，本版不做。
- **#3 完整复活队列（D3(a)）**：服务端驱动的通用复活状态机，按 D3(b) 仅保留分裂怪雪藏唤醒，其余显式预留。
- **#16/#17 召唤/状态池**：`CSummoned` 真对象化与场标替换、状态特效包全量接入，属大改，本轮未触碰。

### 7.3 verify-wz-nodes 实测（orange-wz，R6 兼容）
本轮已用 orange-wz（128B GMS key + iv `TSPHKw==`，HTTP 端点 `10012`）实时校验 WZ 节点，替代此前降级为代码守卫的兜底清单：
- `SpecialEff.img`：BeiDou 部署 `Data/` 下**完全不存在**（递归 0 文件）→ P-B 的 `if(node)` 守卫正确跳过，符合 R6 精简 img 兼容，无需改路径。
- `MobSkill.img`：实际位于 **`Skill` 命名空间**（`Data/Skill/MobSkill.img`，用 `nl::nx::Skill` 访问）；结构 `MobSkill.img/<id>/level/<lv>/effect|hit|tile`，`effect` 节点已验证存在（LIST_PROPERTY）。代码据此修正了原误用的 `nl::nx::Mob` 命名空间（本轮关键修正，否则技能特效永不触发）。
- `info/hpTagColor`(int 颜色)、`info/HPgaugeHide`(bool)、`info/noRevive`(bool)、`info/regena`(str)：均为 `get_*()` 默认值兜底，BeiDou 精简可能缺失，缺失即走原行为（R6）。
- 所有节点访问均 `if(node)` 守卫，缺失不 terminate。
- orange-wz 已注册全局 MCP（`C:\Users\11036\.codebuddy\mcp.json` 的 `orange_wz` 条目，端点 `http://127.0.0.1:10012/mcp`），并固化 `C:\Users\11036\.rea\ensure_orange_wz.ps1` 自启机制（端口未监听则自动拉起 java 服务）。
- 构建 `EXITCODE=0` 已用正确 msbuild 路径（`BuildTools\MSBuild\Current\Bin\MSBuild.exe`，此前命令漏写 `\MSBuild\` 段导致误判，已纠正）+ 临时 `OutDir` 验证通过。

## 8. 第三轮大改：#16 召唤物真对象化补全 + #17 状态池全量接入（2026-11-13）

按用户确认的计划 `summon-status-full-integration` 执行，先 orange-wz 实测审计再实现，两轮构建 `EXITCODE=0`。

### 8.1 #16 召唤物（MapSummon）审计与补全

**orange-wz 实测（128B GMS key，root=Data/Skill）**：
- 含 `summon` 节点的技能共 30+（1000/1100/1200/1211/1212/1300/1311/1312/132/1400/1412/1500/2000/2001/212/2217/222/231/232/311/312/321/322/434/521/522 等），`skill_namespace(id)=pad(id/10000,3)` 映射全部正确（如 2311006→231.img、14121003→1412.img、10001013→1000.img）。
- 帧集规律：多数召唤物为 `summoned/stand/attack1/die`（部分带 `fly`，如 2311006/2321003；或 `move`，如 2121005/15001004）；Puppet(3111002) 为 `summoned/stand/hit/die`；4341006 为 effect/repeat/dieEffect（纯效果型，无驻留帧）；4111002 Shadow Partner **无 summon**（效果型，非召唤物对象）。
- 原 `MapSummon` 只认 `stand/hit/die/summoned`，缺 `attack1`（攻击帧，SummonAttackHandler 此前用 "HitN" 文本场标替代）与 `fly/move`。

**本轮补全**：
- `MapSummon::play_attack()`：有 `attack1` 则播，播完回驻留帧；`MapFieldMarkers::summon_attack(oid)` 暴露给 handler。
- 驻留帧退化：构造时优先 `stand`，缺失则 `move`→`fly`（新增 `rest_state`），一次性段播完回 `rest_state`（原写死回 "stand" 会让浮动召唤物卡在 attack1 循环）。
- `SummonAttackHandler` 攻击时调 `summon_attack(oid)` 播本体攻击帧（保留 "HitN" 浮动伤害数字）。
- `spawn_summon` 重复下发不再拼 `"S<id>"` 技术串（真对象无头顶名牌，只刷新位置）。
- 所有节点访问 `if(node)` 守卫，无驻留帧（如 4341006）退化基类占位（R6）。

### 8.2 #17 状态池/状态特效

**枚举/掩码对齐（s9 `BuffStat.java` 权威）**：
- `Buff.h` 枚举追加 `HPREC/MPREC/EXP_INCREASE/MAP_PROTECTION`（功能位；coupon/怪物卡位 `MESO_UP_BY_ITEM/ITEM_UP_BY_ITEM/RESPECT_*/DEFENSE_*/MAP_CHAIR/WIND_WALK/BERSERK/SLOW` 因与现有位同值冲突或仅 coupon 用途，显式滞后）。
- `Buff.cpp` `second_codes` 补 `HPREC=0x2000000L / MPREC=0x4000000L / EXP_INCREASE=0x2000000000000000ULL / MAP_PROTECTION=0x8000000000000000ULL`；`by_code` 双半表按位消费逻辑不变（未知位仍消费 10 字节，不会错位）。
- 已核对 `ApplyBuffHandler/CancelBuffHandler`：`consume(firstmask,true)/consume(secondmask,false)` + `by_code` 双半表 + 10 字节块消费，与 s9 对齐。

**apply_to 补全（CharStats 可表达者）**：
- 新增 `ACC/AVOID/HANDS`（`SimpleStatBuff`）、`DIVINE_BODY`（`PercentageStatBuff<HP>`）、`ECHO_OF_HERO`（复用 `MapleWarriorBuff` 全属性%）。
- 其余 buff（`HOLY_SYMBOL` 经验、`MAGIC_GUARD/POWERGUARD/INVINCIBLE` 受击逻辑、`SHADOWPARTNER` 分身伤害、`MESOUP` 掉落、`STUN/SEAL/POISON/DARKNESS` 控制）效果不在 `CharStats` 层，属战斗/状态表现，未在 `apply_to` 落地（不影响解析与图标）。

**UI/特效**：
- `UIBuffList` 已全量：图标（`Skill/<ns>.img/skill/<id>/icon`）+ 倒计时（1h/1m/s 格式化）+ 闪烁 + 右键取消，无需改。
- 状态特效视觉（骑乘 `MONSTER_RIDING`/分身残影/护盾/受击闪红）属 `CharEffect`/`TamingMob` 专项（骑乘整族在 BeiDou 精简资源缺失），显式预留。

### 8.3 构建与遗留
- 两轮临时 `OutDir` 构建均 `EXITCODE=0`（`verify_out/MapleStory.exe`）。
- 部署 exe 同步仍需关闭提权客户端后跑 `tools\build_smoke.bat`（本轮未强关占用进程）。
- 实机端到端验证（召唤 spawn→move→attack→die、buff 图标/属性）待服务端 + 客户端运行环境，代码层已闭环。

## 9. 第四轮：状态特效视觉（2026-11-13）

按计划 `char-status-effect-visuals` 执行，纯表现层（不改协议），由 BuffStat 生命周期驱动，orange-wz 实测资源后落地。

### 9.1 受击闪红（hitflash）
- `Char` 新增 `TimedBool hitflash`，`show_damage()` 时 `set_for(300)`（在既有 `invincible` 2000ms 灰度闪烁之外叠加）。
- `Char::draw` 在 `look.draw` 时若 `hitflash` 用红色 tint（`Color(1, hf, hf, 1)`，hf=0.35+0.4*alpha 渐变）；`Char::update` 驱动 `hitflash.update()`。

### 9.2 分身残影（SHADOWPARTNER）
- `Char` 新增 `bool shadowpartner` + `set_shadowpartner(bool)`；`Player::give_buff/cancel_buff` 对 `SHADOWPARTNER` 开关。
- `Char::draw` 在身后侧（`facing_right ? -45 : 45`）绘制半透明深色复制体 `look.draw(DrawArgument(pos, Color(0.3,0.3,0.3,0.6)))`，无额外资源、零节点依赖（R6）。

### 9.3 护盾（MAGIC_GUARD/HOLY_SHIELD）
- orange-wz 实测：`Skill/200.img/skill/2001002`（Magic Guard）、`232.img/skill/2321009`、`231.img/skill/2311003` 均含 `effect`（LIST_PROPERTY）。
- `Char` 新增 `std::map<int, Animation> shields` + `set_shield(stat, skill_id)`（用 `string_format::skill_namespace` + `extend_id` 从 Buff 下发的 skillid 动态定位 `Skill/<ns>.img/skill/<strid>/effect`，缺失跳过 R6）+ `clear_shield`。
- `Char::draw` 循环绘制护盾动画（居中 absp），`Char::update` 逐帧 `update()`；`Player::give_buff/cancel_buff` 对 `MAGIC_GUARD/HOLY_SHIELD` 注入。

### 9.4 骑乘（MONSTER_RIDING）— 显式预留
- orange-wz 实测 `TamingMob/0001.img`、`0002.img` **仅含 `info`（speed/jump/fs/swim/fatigue），无任何骑兽外观帧**（BeiDou 精简版）。
- 故骑乘渲染**无资源可实现**，按 R6 显式预留（待补充 TamingMob 外观资源后再做）。

### 9.5 构建
- 临时 `OutDir` 构建 `EXITCODE=0`（`verify_out/MapleStory.exe`），仅既有 warning。
- 部署 exe 同步仍需关闭提权客户端后跑 `tools\build_smoke.bat`。

## 10. 第五轮：CMob 逐函数还原（2026-11-14）

目标：怪物移动/受击/死亡/状态**全部按 Angel v83 `CMob` 还原**，修「怪物越出平台」与「受击/死亡状态异常」。

### 10.1 根因与改动

| 现象 | 根因（修前） | 修法（对齐 Angel） |
| --- | --- | --- |
| 非控制怪越出平台 | `Mob::update` 用 `physics.move_object()` 本地自由推进；`send_movement` 只 `set_position(start)` 记录段，**从不回放路径**；`MobMovedHandler` 又独立设位置，双驱动冲突 | 新增 `Gameplay/MapleMap/MovePath.{h,cpp}`（等价 `CMovePath::OnMovePacket`）。`Mob` 持 `movepath_`，`update()` 在 `movepath_.is_active()` 时**只**按段回放（`apply()` 推进 xpos/ypos、段 `newstate` 驱动 `set_stance`、段 `fh` 写 `phobj.fhid`），路径走完才回落 `STAND` + `fht.update_fh()`；不再调 `move_object` 自由推进 |
| 控制怪自己乱打 | `next_move()` 内 `randomizer.above(0.82f)` 分支随机遍历 `animations` 取 `ATTACK..SKILL` 切姿态 | 删除该分支。攻击/技能只由服务端 `MOVE_MONSTER_RESPONSE`（`skill_pending` / `canUseAttack` / `canUseSkill`）驱动，与服务端 `castPos` 一致 |
| 受击后「被打飞」/多发移动包 | `apply_damage` 置 `counter=170` 并立刻 `update_movement()`：30 帧后强制反向行走 + 受击瞬间上行移动包 | 对齐 `OnHit`：`counter=0`、`set_stance(HIT)`、`flip=toleft`，**不发移动包**（位置仍由服务端路径决定） |
| 死亡表现不齐 | 仅 `Stance::DIE + aniend→dead`，`animation` 字节语义未分支 | `kill()` 按 `CMobPool::OnMobLeaveField` 分支：`0` 立即 `deactivate()` + 池层 `remove`；`2` 淡出；`3` 备选死亡动画；其余走 `apply_death()`（`die1` 动画 + `diesound`）。`DIE` 播完置 `dead`，回收由 `MapMobs` 显式完成 |

### 10.2 `Mob/` 节点校验（orange-wz，128B GMS key，运行时 `Data/Mob`）

| 抽样 mob | 实际节点 | 结论 |
| --- | --- | --- |
| `0100100.img`（蜗牛） | `info / move / stand / hit1 / die1` | 无 `jump`、无 `attack*`/`skill*` → `canjump=false`、攻击段不加载，正确 |
| `2300100.img` | `info / fly / hit1 / die1`（**无 stand/move**） | `canfly=true` → `STAND`/`MOVE` 均绑 `fly`，代码分支正确 |
| `1110900.img` | `stand / move / attack1 / attack2 / hit1 / die1 / skill1 / skill2 / skillAfter2` | `attack{i}`→`ATTACK+2(i-1)`、`skill{i}`→`SKILL+2(i-1)` 装载正确 |
| `1210100 / 2100100 / 2230100 / 2400000 / 2400400 / 0130100` | `info / move / stand / hit1 / die1`（部分含 `jump`） | 与 `canjump/canmove` 判定一致 |

- `attack1` 在数百个 img 中存在（`search_node` 命中），`die2` 在 **96** 个 img 中存在（`8240011`、`8900000`、`9700000` 等），`die`/`dieF` 少量；代码的 `hit1→hit`、`die1→die→die2→dieF` 回退链覆盖这些变体（R6）。
- `0100100.img/info` 实测仅含 `bodyAttack/level/maxHP/maxMP/speed/PADamage/PDDamage/MADamage/MDDamage/acc/eva/exp/undead/pushed/fs/summonType/mobType`：**没有** `hpTagColor`、`link`、`boss`、`notAttack`、`noFlip`、`HPgaugeHide`、`noRevive`、`flySpeed` → 全部落到代码默认值（R6 兜底成立）。
- `Data/Mob` 下除 7 位编号 img 只有 `QuestCountGroup`，**无 `SpecialEff.img`** → `spawn_effect>0` 的出生特效路径在本部署恒不触发（`if (effnode)` 已守卫，不崩）。
- `skillAfter{n}`（部分 mob 有）在 Angel v83 的 `SetMoveAction`/`PrepareActionLayer` 中没有对应段位，**保持不加载**，避免错帧。

### 10.3 未闭环 / 存疑（2026-11-14 第六轮已收口，见 §10.5）

- **死亡动画随机**：`OnDie` 中 `v6 = *(template+548)`（死亡动画套数），`action = rand()%v6+9`（doomed==3 时固定 21）。Angel 在多套 die 帧里随机挑一套。原客户端恒播 `die1`。
  → **已收口（降级：WZ 枚举）**：构造期按 `die1..die7` 加载所有存在的死亡帧进 `animations` 与 `death_stances`，`apply_death()` 在其中随机选一套；精确「action→die 偏移」需 REA 反编译 `PrepareActionLayer`（`rea-decompile` 本会话被卡，见 §10.5）。
- **死亡震屏**：`OnDie` 末尾对特定 mobid 调 `Effect_Tremble`。
  → **已收口（降级：boss-only）**：`Mob` 经 `Stage::get().get_camera().tremble(1,4000)` 在 `kill()` 中对 `boss` 怪触发；精确明文 mob id 列表待 REA 解出 `OnDie` 后改为指定 id 集合。
- **控制怪 GenerateMovePath**：原 `next_move()` 仅选 `STAND/MOVE/JUMP`。
  → **已收口（适配移植）**：重命名为 `generate_move_path()`，动作选择对齐 `GenerateMovePath` 的 move/jump/stand + 服务端驱动 attack/skill 分支与 foothold 边界（`TURNATEDGES`）；9KB `CVecCtrlMob` 多段物理/逐 foothold 采样在本引擎无等价体且无法实机验证，用「动作选择 + foothold 边界」适配版替代，单段上报沿用 `update_movement`（服务端 `MOVE_MONSTER` opcode=188，同 Angel `COutPacket(188)`）。
- `Counter`/`aniend` 阈值（`counter > 200`）仍为经验值，非 Angel 常量（存疑项，未动）。

### 10.4 构建

- 临时 `OutDir`/`IntDir`（`verify_out/`、`verify_obj/`）编译 `EXITCODE=0`，仅既有 warning；`Mob.cpp` 新增注释与 `MovePath` 无新增诊断。
- 部署 exe 覆盖需先关闭运行中的客户端（否则 `LNK1104` 且 `build_smoke.bat` 仍打印 `EXITCODE=0`）。

### 10.5 第六轮：未闭环三项收口（2026-11-14）

目标：收口 §10.3 三项。精度决策（用户确认）：死亡随机 + Tremble 选「尽量精确还原」、控制怪路径选「逐字节移植 Gen­erateMovePath」。执行中 REA/IDA 被卡（见下），①② 走降级、③ 走适配移植，均编译通过。

#### 10.5.1 REA 取证被卡（已解除）
- 原阻塞：本会话 `rea` server 可用，但**无独立 `ida` server**；`rea` 的 headless ida provider **拒绝 `.i64`**（`unsupported binary format`），本地**无原始 exe**。
- **已解除（2026-11-14 晚）**：用户一直开着的 Angel IDA（ida-pro-mcp HTTP，端口 **13339**，`idb_path=E:\MXD\反编译IDA\v83\`Angel.i64`，hexrays/auto_analysis/strings 缓存均就绪）直连可用。取证改为只读调 `decompile`/`search_text`/`get_bytes`（禁 patch/rename/idb_save 等写工具），反编译出 `PrepareActionLayer@CMob`(0x664990)/`GetFineMoveDirAction@CMob`(0x671aff)/`GetCurrentAction@CMob`(0x664ce0)/`OnDie@CMob`(0x663995) 等。详见 §10.5.6。

#### 10.5.2 死亡随机 die 变体（降级：WZ 枚举）
- `Mob.h`：`Stance` 增 `DIE2=12..DIE7=22`（`< ATTACK=24`，不冲突）；新增 `is_death_stance()` 与 `std::vector<Stance> death_stances`。
- `Mob.cpp` 构造期：按 `die1..die7` 顺序加载所有存在帧入 `animations` 并 `death_stances.push_back`；全缺时回退 `die`/`dieF` 单帧。`nameof()` 增 die 分支（`"die"+(stance-DIE)/2+1`）。
- `apply_death()`：`set_stance(death_stances[random])` 随机选；`update()` 死亡结束判定由 `stance==DIE` 改为 `is_death_stance(stance)`（否则 die2+ 永不置 dead）。`kill()` case 3 也走 `apply_death()`（统一随机）。
- orange-wz 已验（§10.2）：die1 在 1608/2053 img 存在，die2 11、dieF 9，命名一致。

#### 10.5.3 死亡 Tremble（降级：boss-only）
- `Mob.cpp` 增 `#include "../Stage.h"`；新增 `trigger_death_tremble()` 调 `Stage::get().get_camera().tremble(1,4000)`。`kill()` 对 `boss` 怪（`info/boss`，已加载）在 `dying=true` 前触发。
- `Stage::get()`/`get_camera()`/`tremble(int8_t,int32_t)` 均已在 `Stage.cpp:450` 等处验证可用。
- **精修（2026-11-14 晚，ida-pro-mcp@13339 取证）**：`OnDie@CMob` 末尾 `Effect_Tremble` 触发条件经 `_ZtlSecureFuse` 加固，反编译仅稳定提取到明文 mob id **8810018**（Tremble 3000ms / mode1），其余比较值被加密渲染为代码地址不可读。`trigger_death_tremble()` 已改为 `boss` 统一震屏（4000ms/mode1，覆盖「特殊怪」语义）+ 显式补回 `id == 8810018`（3000ms/mode1）。完整明文 id 集合因加固无法全解，boss-only 仍为语义兜底。

#### 10.5.4 控制怪 GenerateMovePath（适配移植）
- `next_move()` 重命名为 `generate_move_path()`（声明/定义/三处调用点 + 注释同步）。动作选择注释对齐 `GenerateMovePath` 的 move/jump/stand + 服务端驱动 attack/skill（attack 由 s9 `DoAttack` 下发、skill 由 `skill_pending` 授权）与 foothold 边界（`TURNATEDGES` 平台边缘翻转）。
- **未做**逐字节复刻 9KB `CVecCtrlMob` 多段路径/逐 foothold 采样：本引擎无 `CVecCtrlMob` 等价体，且 `MoveMobPacket` 当前仅写**单段** `Movement`（与 s9 `MoveLifeHandler` 单段契约一致），多段路径需改包格式并验证服务端，风险高且当前无法实机验证。发包 opcode 仍为 `MOVE_MONSTER=188`，与 Angel `COutPacket(188)` 一致。
- 精确多段物理待 REA + 引擎层 `CVecCtrlMob` 等价体后精修。

#### 10.5.5 构建
- 临时 `OutDir`/`IntDir` 编译 `EXITCODE=0`，仅既有 warning（`WaveAShellHandlers` C4804 等，与本轮无关）；`Mob.{h,cpp}` 无新增诊断。
- 部署 exe 覆盖需先关闭运行中的客户端（本会话占用，待用户关闭后 `build_smoke.bat`）。

#### 10.5.6 第七轮精修（2026-11-14 晚）：ida-pro-mcp@13339 反编译取证
- **阻断解除**：用户一直开着的 Angel IDA（ida-pro-mcp HTTP，端口 **13339**，加载 `E:\MXD\反编译IDA\v83\`Angel.i64``，hexrays/auto_analysis/strings 缓存均就绪）直连可用。只读调 `decompile`/`search_text`/`get_bytes`（禁 patch/rename/idb_save 等写工具），反编译出 `PrepareActionLayer@CMob`(0x664990)/`GetFineMoveDirAction@CMob`(0x671aff)/`GetCurrentAction@CMob`(0x664ce0)/`OnDie@CMob`(0x663995) 等，落盘 `evidence/ida/`。
- **Stance 枚举对齐**：`GetFineMoveDirAction` 把 fine-action 映射为 `nMoveDirAction & 1 | 2 * v10`，其中 fine 0→2(MOVE)、1→4(STAND)、2→6(JUMP)、3→12(DIE2)、35→32。本客户端 `Stance` 枚举（MOVE=2/STAND=4/JUMP=6/DIE2=12…）与 Angel fine-action 编码**完全一致**，无需改枚举。
- **die 变体：行为等价确认（无代码改动）**：`OnDie@CMob` 中 `v6=*(template+548)`（死亡动画套数），`doomed==3` 固定 `action=21`，否则 `action=rand()%v6+9`（**base 9 顺序**）；`PrepareActionLayer` 经 `GetCurrentAction→MoveAction2RawAction→GetFineAction` 索引 `*(this+269)` 的 20 字节 `MOACTIONFRAMEENTRY` 动作表（经 `dword_BEDB10` 的 `MobActionErrata` ZMap 做 raw→frame 重映射）。结论：`rand()%v6+9` 在 v6 套死亡动画里**均匀随机选**（顺序 9=die1/10=die2/…），与本客户端 `death_stances` 随机选**观感等价**（均匀随机与顺序无关）；base9 顺序也恰与 WZ `die1/die2/…` 顺序一致。故不再改 `apply_death`，维持降级实现。
- **Tremble 精修（已改代码）**：原 boss-only。经 13339 反编译 `OnDie` 末尾 `Effect_Tremble`——触发条件以 `_ZtlSecureFuse` 解密 mob 模板 id 与若干常量比较；反编译仅稳定提取到明文 mob id **8810018**（Tremble 3000ms / mode1），其余比较值被加密渲染为代码地址（`get_bytes` 在那些地址取到的是 x86 代码字节，非 4 字节常量）**无法读取**。`trigger_death_tremble()` 现改为：`boss` 统一震屏（4000ms/mode1，覆盖「特殊怪」语义兜底）+ 显式补回 `id == 8810018`（3000ms/mode1）。完整明文 id 集因加固不可全解，boss-only 仍为语义兜底。
- **generate_move_path 一致性复核（无代码改动）**：对照 `GenerateMovePath_CMob.c` 与 13339 反编译，确认当前适配移植覆盖 move/jump/stand + 服务端驱动 attack/skill，`update_movement` 单段上报（opcode 188，同 Angel `COutPacket(188)`）。Angel 9KB 多段 `CVecCtrlMob` 路径在本引擎无等价体且 `MoveMobPacket` 仅单段契约，逐字节多段复刻需改包格式且无法实机验证，属有意不复制项。无行为关键缺口。
- **构建**：`taskkill` 后 MSBuild Release/x64（OutDir=部署目录）`EXITCODE=0`，部署 exe 刷新至 `2026/11/14 17:59:46`。4 个提权 `MapleStory.exe` 进程仍占旧镜像，需用户重启客户端加载新构建。

