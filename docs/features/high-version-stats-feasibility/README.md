# 高版本战斗属性 / 强化 / 套装 / 角色面板 — 可行性分析

> 日期：2026-11-18　性质：需求受理阶段 0/0.5（只解析、不写业务实现）
> 输入：`E:\MXD\扩展改动\伤害玩法\{伤害设计的简单逻辑,实施清单,套装管理界面设计}.md` + 用户附图 `Character_Stats_Detail_CN.png`
> 证据：客户端 `E:\project\MapleStory-Client`、服务端 `E:\project\BeiDou-Server_s9` 只读梳理 + 公开资料检索

---

## 0. 结论速览（先给答案）

| 你的问题 | 结论 |
|---|---|
| ① 高版本新增属性有哪些 | 已列全（§1），共 ~20 项，分输出 / 防御 / 状态 / 资源 / 速度五类；本地文档只覆盖了其中 8 项 |
| ② 强化 / 套装加额外属性可行吗 | **可行且比预期省力**。套装数据层服务端已就绪（累加 + 字符串 combatStats + 0x176 下发）；最快路径是「显示层接线」，无需改协议，约 1 天 |
| ③ 角色面板能显示附图这些属性吗 | **能**。但「图片样式」与「现有 v4 面板」有差异（三栏分区条被取消）；大多数数值字段已有位置，只差接线 / 补字段，分三档方案（§3） |

最大的一个发现：**客户端 0x175 战斗面板 17 个字段已经完整收到并存进 `CombatPanelStats`，但 UI 层零引用——面板里 7 个位置硬编码 `"0%"`**。这是"数据已在手上、只差接线"的最低成本切入点。

---

## 1. 高版本新增属性清单（对比 v83 基底）

### 1.1 输出 / 伤害乘区

| 属性 | 字段 | 叠加规则 | v83 有 | 本地文档覆盖 |
|---|---|---|---|---|
| 伤害% | `damR` | 加法池，一次乘算 | ✗ | ✓ |
| Boss 伤害% | `bdR` | 加法池，仅 Boss | ✗ | ✓ |
| 对普通怪伤害% | `nbdR` | 加法池，仅非 Boss | ✗ | ✓ |
| 最终伤害% | `fdR` | **各来源独立相乘**（无稀释） | ✗ | ✓ |
| 无视物理防御% | `ignoreMobpdpR` | 加法后封顶 | ✗ | ✓ |
| 无视魔法防御% | `ignoreMobmdR` | 加法后封顶 | ✗ | ✓ |
| 暴击率% | `cr` | 加法 + cap(100%) | ✗(部分职业) | ✓ |
| 暴击伤害% | `cd` | 加法池 | ✗ | ✓ |
| 攻击力% / 魔法力% | `padR` / `madR` | 加法池 | ✗ | ✗(0x175 已收) |
| 冷却缩减% | `cdr` | 加法 + cap | ✗ | 备忘 |

### 1.2 防御 / 生存（附图重点，本地文档基本未覆盖）

| 属性 | 字段 | 说明 | 本地文档 |
|---|---|---|---|
| 状态异常抗性（总） | `asr_r` | 附图"异常状态"8 项由它派生 | ✗(0x175 已收) |
| 避免流血/晕眩/中毒/冰冻/暗黑/窒息/虚弱 | `asr` 细分 | 附图 7 项；协议只有总值，无逐项 | ✗ |
| 诅咒减免 | curse reduce | 附图第 8 项 | ✗ |
| 格挡率 | `block` | 加法 + cap | ✗ |
| 击退抗性 / 稳如泰山 | `stance_prop` | 0x175 已收 | ✗ |
| 伤害减少（物理减伤） | `damage_reduce` | 0x175 已收；附图左上 6 格之一 | ✗ |
| 每秒 HP 恢复 | HP recovery/s | 附图"防御属性" | ✗ |
| 元素抗性（火/冰/雷/圣） | element resist | 附图左上 6 格 | ✗ |
| Buff 持续时间% | `buff_time_r` | 0x175 已收 | ✗ |

### 1.3 资源 / 速度

| 属性 | 字段 | 说明 |
|---|---|---|
| 物品掉落率% | `item_drop_prop` | 0x175 已收 |
| 金币获得% | `meso_drop_prop` | 0x175 已收 |
| 攻击速度 | attack speed | 附图"攻击属性" |
| 魔法速度 | magic speed | 附图"攻击属性"，高版本新增 |
| EP（能量） | energy | 附图主栏，特定职业（零/恶魔复仇者等） |

**叠加规则总纲**（公开资料 + 本地文档一致）：同类百分比先进加法池一次乘算（有稀释）；只有"最终伤害%"各来源独立相乘（无稀释）；无视防御 / 暴击率 / 格挡 / 状态抗性加法后封顶。

> 公开资料来源：NamuWiki《MapleStory/Stats》、StrategyWiki《MapleStory/Formulas》、MapleStory M Wiki《Glossary》的检索摘要确认了 Boss Damage% / Final Damage% / Crit Rate / Crit Damage / Ignore Defense / Attack Speed / Buff Duration / Item & Meso Drop 等属性及"加法池 vs 乘法池"规则；两站点正文抓取被 403 拦截，故属性清单同时以本地三份设计文档为准。

---

## 2. 装备强化 / 套装增加额外属性 — 参考与可行性

### 2.1 服务端 s9 现状（数据层已就绪，计算层空白）

**已有（可直接对接）：**
- `SetBonus.java:19-32`：已有 `Map<String,Integer> combatStats` + 固定值 + 基础百分比 + 技能。
- `SetTiersV2Parser.java:13-14,42-126`：`tiers_json` v2 已解析 `damR/bdR/nbdR/fdR/ignoreMobpdpR/ignoreMobmdR` 等及 `statsPercent`、`skillMods`。
- `SetItemManager.java:212-227,235-242`：`getBonusCumulative()` 已是低档累加（不是文档里说的 REPLACE），与客户端 Tooltip 一致。
- `SetItemManager.java:142-169,245-268`：已把 `damR/bdR/ignoreMobpdpR` 映射成客户端 key（DAMAGE/BOSS/IGNOREDEF）并生成文案，经 0x176 下发。
- `Equip.java:83-86`：`starForce`(0~25)/soul/spark 已存储在装备上。

**缺口（按优先级）：**
1. **0x175 是空 opcode**：`SendOpcode.java:364-369` 定义了 `SET_ITEM_FINAL_DAMAGE(0x175)`，但全仓无人发包。这是把战斗属性同步到客户端面板的**最直接对接点**。
2. **服务端伤害计算不消费任何新属性**：`AbstractDealDamageHandler.java:802-814` 的 `calcDmgMax` 是 v83 固定公式；s9 现在不重算伤害，只校验客户端上报值（12 字节 longLines 下已移除幅度上限校验）。要让新属性"真正生效"，需建计算层。
3. **无 `org.gms.combat` 框架**：文档设计的 17 个类（CombatStatType/Modifier/Profile/Resolver/Provider/Calculator 等）**全部未实现**。
4. **强化只有存储无系统**：`starForce` 无 setter、无规则表、无属性产出；文档建议的 `xy_equip_enhance_rule` 表 + `EnhanceStatProvider` 未建。
5. 0x17A `SET_SKILL_BONUS` 已解析未下发；`damageSkin` 未被 tiers_json 解析。

### 2.2 客户端现状（协议已收，UI 断线）

**已有：**
- `CombatPanelStats.h:11-31`：0x175 的 **17 个字段**全部定义（fdR/damR/bdR/nbdR/ignorePDR/ignoreMDR/crit_rate/crit_dam/padR/madR/drop/减伤/asr/buff_time/stance）。
- `CommonHandlers.cpp:68-107`：`SetItemFinalDamageHandler` 已按既有布局逐字段读取并置 `synced=true`。
- 套装 tooltip 链路闭合：`SetTooltip.cpp:122-158` 服务端文案优先，0x176 的 tier 写入本地并重算属性。
- 星之力 tooltip + 保守线性注入：`EquipTooltip.cpp:989-1079,1152-1170`、`Player.cpp:63-112`。

**致命断层：**
- `CombatPanelStats.cpp:19-23` 的 `notify_ui()` 只调 `UIStatsInfo::update_all_stats()`；而 `update_all_stats()`（`UIStatsInfo.cpp:659-729`）读的是 `CharStats`，`IO/` 全目录对 `CombatPanelStats` **零引用**。→ 收到的 17 个字段一个都没显示。
- `UIStatsInfo.cpp:706-716` 有 7 个硬编码：`DAMAGE_BONUS="0%"`、`FINAL_DAMAGE="0.00%"`、`BUFF_DURATION`、`ITEM_DROP_RATE`、`MESOS_OBTAINED`、`CRITICAL_DAMAGE="0.00%"`、`KNOCKBACK_RESISTANCE`。
- `CharStats.h:113-122` 有 bossdmg/ignoredef/critical/resiststatus/damagepercent 标量，但 `critical`、`resiststatus` **没有 setter**（恒默认 5% / 0）；缺 critdam/block/hp_recovery/magic_speed 等成员。

### 2.3 可行性分级（推荐路径）

| 层 | 动作 | 涉及文件 | 改动量 | 风险 |
|---|---|---|---|---|
| **P0 显示层（最快见效）** | 把 `CombatPanelStats` 接进 `update_all_stats()`，替换 7 处硬编码 | `UIStatsInfo.cpp`（单文件）+ `CharStats` 补 setter | 小，约 1 天 | 极低，**无需改协议/DB** |
| **P1 服务端发包** | 按 `CommonHandlers.cpp:68-107` 既有布局补 0x175 发包方，聚合 `SetBonus.combatStats` 填入 | `PacketCreator.java` + `SetItemManager.java` | 中 | 低，布局已定 |
| **P2 伤害计算** | 建 `CombatStatResolver/DamageCalculator`，接入 `AbstractDealDamageHandler`；怪物加 PDR/MDR | `org/gms/combat/*`（新）+ Handler + `MonsterStats` | 大（文档估 3~5 周） | 高，动核心玩法需配置开关 |
| **P3 强化系统** | `xy_equip_enhance_rule` 表 + `EnhanceStatProvider` + starForce setter/规则 | 新表 + 新类 + Web + 客户端强化窗 | 大 | 中，涉及 DDL（需先授权） |

**推荐先做 P0+P1**：让面板显示真实数值（伤害%/最终%/暴伤/无视/减伤/掉落/金币/异常抗性/ Buff 持续/稳如泰山），伤害计算暂不动。这样"看得见"先闭环，再评估要不要 P2。

---

## 3. 角色个人属性显示附图属性 — 分析

附图是 CMS 高版本面板：**左"角色资料"**（名称/等级/公会/HP/MP/EP/经验/名声/四维 + 物理减伤/火/冰/雷/圣抗 6 格）；**右"详细资料"** 三栏：攻击属性 7 项、防御属性 6 项、异常状态 8 项。

### 3.1 客户端面板现状
- 实现：`UIStatsInfo.h/.cpp`，坐标表 `UIStatsLayoutV4.h:12-69`。
- 现有布局是 **v4 双栏**：主栏 300×392 + 详情 352×392，通栏物攻 + 双列 20 项；已有 3×2 抗性值框（`RES_X0-2`/`RES_Y0-1`）正好对应附图左上 6 格，主栏也有 EP 行（`MAIN_Y_EP=214`）。
- 差异：附图有"攻击属性 / 防御属性 / 异常状态"**三栏分区条**，这个分区条在 v4 被取消了（`docs/features/char-stats-ui-ps/README.md:49,60,89`）；`UIStatsInfo.cpp:517-528` 只是把 20 个标签按两列顺序画，无分组枚举。
- 字段：附图里"暴击伤害 / 格挡率 / 每秒HP恢复 / 攻击速度 / 魔法速度 / 异常状态 8 项细分"——客户端要么硬编码 0%，要么根本无字段（critdam/block/hp_recovery/magic_speed 无成员；asr 只有总值无 8 项）。

### 3.2 三档方案

| 方案 | 做法 | 对齐度 | 成本 |
|---|---|---|---|
| **A 数值点亮** | v4 布局不变，接线 0x175 + 补 setter，填已有值的框 | 数值对、样式是双列非三栏 | 低 |
| **B 三栏还原** | 在 A 基础上恢复"攻击/防御/异常"分区条 + 新底板，新增 CharStats 字段；异常 8 项需扩协议或本地展开 asr | 高，接近附图 | 中高（含 UI 出图 + img 入库） |
| **C classic 底板** | 用 v83 原始底板 | 无法加新行（标签烘焙在图里） | 不适用 |

**推荐 A 先行、B 可选**。注意附图 "EP / 元素抗性 / 异常 8 项" 是版本/职业特定显示，需确认到底要哪几项，避免做无用字段。

---

## 4. 待你拍板（进入实现前）

| # | 决策点 | 选项 |
|---|---|---|
| 1 | 本次范围 | A：仅面板显示（P0+P1） / B：显示 + 真正参与伤害计算（含 P2） |
| 2 | 面板样式 | 沿用 v4 双列只填数值 / 按附图三栏重绘（需 UI 出图 + img 入库） |
| 3 | 数值来源 | 服务端 0x175 权威下发（推荐） / 客户端本地计算 |
| 4 | 强化系统 | 这次一并做（含新 DB 表） / 先预留、只做套装 |
| 5 | 数据库 | 若涉及新表：集成 dbx / 自备 DB MCP / 本版先不碰库（仅 SELECT） |

> 按仓库规范，公共 API / 协议 opcode / DB 表结构改动需先确认；本次分析阶段未改任何代码、未写库。

---

## 5. 关键证据索引

客户端
- `IO/UITypes/UIStatsInfo.cpp:659-729`（面板赋值，706-716 硬编码）
- `IO/UITypes/UIStatsLayoutV4.h:12-69`（布局坐标）
- `Character/CombatPanelStats.h:11-31`、`CombatPanelStats.cpp:19-23`（0x175 数据 + 断线点）
- `Net/Handlers/CommonHandlers.cpp:68-107`（0x175 接收）
- `Character/CharStats.h:27-124`（属性标量 + 无 setter）
- `IO/Components/SetTooltip.cpp:58-166`、`EquipTooltip.cpp:947-1079`（tooltip）
- `Character/Player.cpp:63-169,281-292`（星力/套装注入）

服务端
- `org/gms/client/Character.java:7236-7479`（属性聚合，套装注入 7293-7310）
- `org/gms/server/setitem/{SetBonus,SetItemManager,SetTiersV2Parser}.java`
- `org/gms/net/server/channel/handlers/AbstractDealDamageHandler.java:802-814`
- `org/gms/util/PacketCreator.java:1043-1101,7764-7819`
- `org/gms/net/opcodes/SendOpcode.java:364-369`

设计输入
- `E:\MXD\扩展改动\伤害玩法\伤害设计的简单逻辑.md`（§1~12）
- `E:\MXD\扩展改动\伤害玩法\实施清单.md`（D/S/W 条目）
- `E:\MXD\扩展改动\伤害玩法\套装管理界面设计.md`（Tab②③④ 字段）
