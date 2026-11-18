# 全盘开发计划与架构设计 — 套装权威化收尾 + 高版本战斗属性体系

> 日期：2026-11-18　性质：Plan 模式施工蓝图（先确认，后实现）
> 输入：① deepseek harness 会话 `session-1c97e618`（套装系统服务端缓存热更新，18 轮）；② `E:\MXD\扩展改动\伤害玩法\` 三份设计文档；③ 附图 `Character_Stats_Detail_CN.png`；④ 本目录 `README.md`（高版本属性可行性分析）
> 证据口径：本轮已对会话 zstd 日志解压复核 + 双端 git/产物/配置现场核实，不采信"memory 陈述"当真相

> **【用户 2026-11-18 新决策 · 覆盖本文件旧 Plan】** 见文末「第七部分 v2 定稿」：先验证 → 接官方完整伤害公式（去掉配置开关、属性驱动："装备没有公式就不计算它"）→ 服务端/客户端同步开发 + 防篡改服务端校验 → 面板暂不换（只补全属性能力供扩展）→ 周边一并移植（xy `org.gms.combat` 核心层）→ 学习官方做法、架构高扩展。**第六部分之前的阶段表为历史方案，以第七部分为准。**

> **【实施状态 2026-11-18 10:30】** Phase 1+2 双端代码已落地、编译/单测全绿、产物刷新：
> - 服务端：`org.gms.combat` 包 22 类（stat/damage/provider/format）+ CombatStatResolverTest 8 例全绿；`Character` profile 缓存；`AbstractDealDamageHandler` 校验接 Profile（alert 全开、addPoint 仅 legacy 灰度）；`PacketCreator.setItemFinalDamageBonus` 0x175；登录/换装发包。jar 10:29:52。
> - 客户端：`Attack.finaldamage`；`Player/Skill/Mob` 官方乘区（1.35 暴击基准 + fdR）；**`damage.formula_v2` 开关已彻底删除**（声明/注册/实现/SAVE_ORDER/example/locale）。exe SHA `8860b38c` 10:29:14。
> - **待用户真机验收**（Phase 0 三图）：见 §7.5。真机前不要下结论。
>
> **【公式校正 2026-11-18 11:30】** 按 BWIKI 官方公式补完两处：`OFFICIAL-FORMULAS.md §5` 的无视防御（加法 cap → 连乘 `1-Π(1-x)`）与暴击（固定1.35 → 区间 `[1.2+cd,1.5+cd]` 随机）。双端编译/单测全绿，jar 11:24:21、exe SHA `6d80e644` 11:30:03。192 服务端可 javap 反编译（`Server.jar` 未混淆）、192 UI 素材可移植（key=零IV+GMS128B）、左侧菜单需独立立项——详见 `ASSET-AND-REVERSE-ANALYSIS.md §7`。

---

## 第一部分：会话任务完成度审计（逐条，不笼统说"完成"）

### 1.1 会话结局
18 轮；goal 标记 `complete`；**最后 3 轮（turn 16/17/18）"继续"全部因 API 错误中断（429 限流 / qoder model_error），未执行任何工具**。即 turn 15 之后没有新工作发生，会话是被"卡断"而非"做完收尾"。

### 1.2 已落地且现场核实为真（代码层）

| 项 | 现场证据 |
|---|---|
| 套装服务端权威化 P1–P4 | 服务端提交 `517eb0d48b`（缓存/发包/热更/web）、`d09cc96d7b`（0x176 tier + 换行/全量映射）；`SetItemController.java` 存在，含 list/detail/save/delete/reload |
| 客户端 0x176 文案/映射 | 提交 `deeac074` |
| 客户端 tier 加成 + 百分比 | 提交 `dbf3ee3e` |
| damage.formula_v2 默认开 | 提交 `b20fa501`；`Configuration.h:1036` = `BoolEntry("damage.formula_v2","true")` |
| 构建产物 | 服务端 `BeiDou.jar` 08:45（117MB）；客户端 `MapleStory_setv2.exe` SHA `8857fc21` 09:25（turn15 开 v2 后重编，已部署具名副本） |

### 1.3 未完成 / 待续（这是本次要全盘规划的对象）

| # | 未完成项 | 根因/现状（现场核实） | 阻塞性 |
|---|---|---|---|
| U1 | **真机验收全程未做** | 部署目录无任何套装截图；主 `MapleStory.exe` 仍是旧包 `0fcb2173`(08:10)，具名副本 `setv2.exe` 需手动切；服务端 jar 需用户重启 | 🔴 只能用户做，agent 无 GUI |
| U2 | **fdR（最终伤害%）未接入伤害公式** | `CombatPanelStats` 全库仅定义/读包/日志，`Gameplay/**` 零引用；`Mob.cpp` 伤害公式无 final damage 乘区 | 🟠 需拍板口径 |
| U3 | **damage.formula_v2 开启后无回归验证** | turn15 刚把默认值 false→true，全服伤害切 v2（魔法独立/无视防御/暴击区间/MAD钳制同时启用），未真机验证 | 🔴 高爆炸半径，必须先测 |
| U4 | **0x175 实际是空 opcode** | 服务端全仓仅 `SendOpcode.java:367` 一条定义，无任何发包方；客户端 handler 空等 → 面板 17 战斗字段无服务端数据源 | 🟠 面板点亮的前提 |
| U5 | **角色面板 7 处硬编码 + 字段缺失** | `UIStatsInfo.cpp:706-716` 7 个 `"0%"`；`CharStats` 无 critdam/block/hp_recovery/magic_speed，crit/resist 无 setter；附图三栏分区条 v4 已取消 | 🟠 见 README §3 |
| U6 | **web 管理端只有后端 API，无前端 UI** | `SetItemController` 有 CRUD/reload，但 gms-ui 前端页面未移植；且 web 写库 DML 尚未获明确授权 | 🟡 可选 |
| U7 | **装备强化系统缺失** | 无强化窗，`starForce` 仅存储无规则无产出；附图属性面板的强化来源无落点 | 🟡 可选 |

> 一句话：会话把"套装链路的代码"做完了，但**真机验收（U1）一个字没验、伤害公式收尾（U2/U3）没验、面板展示（U4/U5）没接线**。规划必须先把"已写的代码证明真的生效"，再谈新增属性。

---

## 第二部分：目标架构设计（统一端到端）

### 2.1 分层与数据流（最终形态）

```
┌───────────── 配置权威层（服务端唯一真理源）─────────────┐
│  beidou.xy_set_item (61 行, tiers_json v2)             │
│  + [拟新增] xy_equip_enhance_rule（强化规则表）          │
│  + [拟新增] 全局 cap 配置（GameConfig）                  │
└──────────────────────┬──────────────────────────────────┘
                       │ 启动全量加载 / web CRUD 热更 / 广播
                       ▼
┌───────────── 服务端聚合层（拟建 org.gms.combat）─────────┐
│  SetItemManager (已有, DB权威缓存 + reload)              │
│  CombatStatType/Modifier/Profile/Resolver  (U2 新建)     │
│  ├─ SetBonusProvider (已有 combatStats Map)             │
│  ├─ EquipProvider (装备白字/卷轴/强化)                    │
│  └─ Buff/PassiveProvider (可选)                          │
│  产出不可变 combatStats 快照：damR/bdR/nbdR/fdR/         │
│  ignorePDR/ignoreMDR/cr/cd/block/asr/hpRecover/…         │
└──────────────────────┬──────────────────────────────────┘
                       │ 发包（关键：两条包各司其职）
        ┌──────────────┴───────────────┐
        ▼                              ▼
  0x176 (已实现)                  0x175 (U4 待实现发包方)
  setId/enabled/text/itemIds/     17 字段战斗面板（按 CommonHandlers.cpp:68-107
  tier列表{count,statKey,value}   严格对齐：fdR/damR/boss/normal/ignorePDR/
  → 套装 tooltip + 本地属性注入      ignoreMDR/cr/cd/padR/madR/drop/meso/
                                   减伤/asr/buffTime/stance）
        │                              │
        ▼                              ▼
┌───────────── 客户端消费层 ───────────────────────────────┐
│  SetTooltip (服务端文案优先, 已实现)                      │
│  Player::apply_set_item_bonuses (tier→CharStats, 已实现)  │
│  CombatPanelStats::get() (0x175 缓存, 已有)              │
│    ├─ [U5] → UIStatsInfo::update_all_stats()  ← 当前断线  │
│    └─ [U2] → Mob.cpp 伤害公式 fdR 乘区（需拍板）          │
└──────────────────────────────────────────────────────────┘
```

### 2.2 三条设计原则（避免走回老路）
1. **服务端是唯一权威，但客户端算伤害**（s9 现实，`Mob.cpp:901`）：百分比走 0x176 tier→本地注入（已跑通），fdR 若要生效只能客户端公式加乘区——这是架构现实，不强行服务端重算。
2. **包布局即契约**：0x175 服务端发包必须严格按 `CommonHandlers.cpp:68-107` 已定字段序，否则全盘错位（README §4.5 已标红）。
3. **开关可回滚**：`damage.formula_v2` 已是现成开关范式；任何新增公式乘区沿用 `GameConfig`/配置开关，默认可关。

### 2.3 属性来源 → 生效通道对照表

| 属性类别 | 权威来源 | 生效通道 | 面板显示通道 | 当前状态 |
|---|---|---|---|---|
| 套装固定值 + HP%/MP% | xy_set_item.tiers_json | 0x176 tier → Player 本地注入 | 主栏/详情 | ✅ 已实现，待真机验 |
| damR/bdR/ignoreDef | tiers_json.combatStats | 0x176 tier → Player 注入 → CharStats | 详情 | ✅ 已实现（依赖 v2 开），待验 |
| fdR 最终伤害 | tiers_json.combatStats.fdR | U2：Mob.cpp 加乘区 | 0x175 → 详情 | 🟠 未接公式 |
| 暴伤/格挡/减伤/掉落/金币/异常抗性 | 拟 combatStats 扩展 | U4：0x175 发包 + U2 公式 | 0x175 → 详情 | 🟠 无发包方 |
| 装备强化附加属性 | 拟 xy_equip_enhance_rule | 拟 EquipProvider → 本地注入 | tooltip + 详情 | 🟡 未建 |

---

## 第三部分：分阶段开发计划（按依赖排序，一次性成型不挤牙膏）

### Phase 0 — 冻结现状 + 真机验收（🔴 最高优先，阻塞后续一切）
> 理由：U3 的 v2 全开是全局玩法改动，未验就叠新功能 = 在流沙上盖楼。
- **P0-1** 用户用新 `BeiDou.jar`(08:45) 重启服务端；客户端切 `MapleStory_setv2.exe`(SHA 8857fc21)，核对 SHA。
- **P0-2** 真机 hover 1003242：截图验证 ①套装名显示 ②属性行含 HP%/MP% ③穿对应件数档位点亮。
- **P0-3** v2 伤害回归：截图验证暴击/魔法/BOSS 伤害数值正常（防暴击归零/数值异常）。
- **P0-4** web 热更验证：`SetItemController /reload` 或改一条 tier → 在线玩家 hover 即时变（需 web 写库 DML 授权，否则用 GM 命令/GUI 改表）。
- **验收门**：4 项真机证据齐（本端截图，非参考端）才进 Phase 1。证据落 `docs/features/setitem-server-authority-2026-11-18/evidence/`。

### Phase 1 — 面板点亮：0x175 发包 + CombatPanelStats 接线（对应 U4/U5）
- **P1-1** 服务端实现 `PacketCreator.setItemFinalDamageBonus(CombatStatProfile)`，按 `CommonHandlers.cpp:68-107` 17 字段序发包；从 `SetBonus.combatStats` 聚合取值；fd_count 先发 0。
- **P1-2** 换装/登录/热更时触发发包（复用 `Character.equipChanged()` / `PlayerLoggedinHandler` 现有挂载点）。
- **P1-3** 客户端 `UIStatsInfo::update_all_stats()` 接 `CombatPanelStats::get()`，替换 `UIStatsInfo.cpp:706-716` 的 7 处硬编码（damR/fdR/暴伤/掉落/金币/减伤/异常抗性/Buff持续/stance）。
- **P1-4** `CharStats` 补 critdam/block 等成员 + setter（缺字段见 README §1.2）。
- **验收门**：真机 hover 面板显示真实数值（非 0）；双端字段序蓝军核对；构建 exit 0。
- **改动面**：服务端 PacketCreator/SetItemManager/挂载点；客户端 UIStatsInfo/CharStats。**不碰伤害公式**。

### Phase 2 — fdR 进伤害公式 + 战斗属性生效（对应 U2，需拍板口径）
- **P2-1** 口径确认（见待决项 Q2）：fdR 乘区挂在哪、是否只对套装来源生效、是否进 calcDmgMax 校验。
- **P2-2** `Mob.cpp` 伤害公式新增 fdR 乘区，`CombatPanelStats.final_damage_percent` 参与计算；沿用 v2 开关可回滚。
- **P2-3** 补齐叠加规则：damR/bdR/nbdR 加法池、fdR 独立乘、ignorePDR/MDR 加法封顶（按三份文档 §1）。
- **P2-4** 单测（稀释示例/连乘/无视防御 cap）+ 真机伤害数值回归。
- **验收门**：套装 fdR 真机增伤可见；原怪物伤害无异常；单测全绿。

### Phase 3 — 角色面板样式对齐附图（对应 U5 三栏，可选/需拍板）
- **P3-1** 样式决策（待决 Q3）：沿用 v4 双列只填数值（低成本）vs 恢复附图三栏分区条（需 UI 出图 + img 入库）。
- **P3-2** 若选三栏：Photoshop 出分区条/底板 → orange-wz 入库 → `UIStatsInfo` 分组枚举 + 重排 20+ 标签坐标。
- **P3-3** 附图特有字段（EP/元素抗性/异常 8 项细分）按职业与版本确定清单，避免做无用字段。
- **验收门**：真机截图与附图观感对齐。

### Phase 4 — 装备强化 + 更多来源（对应 U7，可选/需拍板）
- **P4-1** DDL：`xy_equip_enhance_rule` 规则表（按 DDL 备注规范：表/列 COMMENT 齐全）。
- **P4-2** 服务端 `EquipProvider` + starForce setter/规则产出；web 规则编辑器。
- **P4-3** 客户端装备强化窗（`UIElement.h` 无对应 Type，`ENHANCEEQUIP` 键无 handler，需新建）。
- **验收门**：强化等级变化 → 属性/面板/tooltip 同步；构建通过。

### Phase 5 — Web 管理前端（对应 U6，可选）
- **P5-1** gms-ui 套装编辑页（照 xy 分支 `SetItemEditor`/`CombatStatForm`，Tab②③④ 字段见设计文档）。
- **P5-2** web 写库 DML 授权确认（skill 门禁 17）；热更 preview。
- **验收门**：web 改 tier → 保存 → reload → 游戏内 hover 即时变。

---

## 第四部分：待你拍板（进入实现前，仅列真正需要决策的）

| # | 决策点 | 选项 | 推荐 |
|---|---|---|---|
| Q1 | Phase 0 真机验收能否先做 | A：你先切包重启截图验完再继续 / B：先并行推 Phase 1 代码，验收穿插 | **A**（v2 全开未验风险高） |
| Q2 | fdR 是否进伤害公式（U2 口径） | A：进公式，对所有来源生效 / B：只对套装来源 / C：暂不进，只显示 | 先 C 验面板，再定 A/B |
| Q3 | 角色面板样式（Phase 3） | A：v4 双列只填数值 / B：按附图三栏重绘 | A 先行，B 可选 |
| Q4 | 装备强化系统（Phase 4） | A：本轮做 / B：预留接口以后做 | B（先收尾套装链路） |
| Q5 | Web 前端 + 写库（Phase 5） | A：集成 dbx/自备 MCP 且授权 DML / B：本版只保留后端 API | B |
| Q6 | 失败项交接 | 服务端 jar/客户端 exe 部署仍由你手动切，是否维持 | 维持 |

---

## 第五部分：风险与回滚

- **最大风险**：damage.formula_v2 默认 true 后全服伤害切 v2（暴击区间/MAD钳制/魔法独立同时启用）——Phase 0 若发现数值异常，立即 `xy-config.yaml` 设 `damage.formula_v2: false` 回滚（注释见 Configuration.h:1030-1036）。
- **包布局风险**：0x175/0x176 字段序错位会导致全盘属性错乱；每次改包都做双端逐字段蓝军核对（会话已有此机制，保留）。
- **部署风险**：主 exe 被登录器回滚到旧包（会话实测 `0fcb2173`）；一律用具名副本 + SHA 核对，不覆盖主 exe。
- **提交风险**：双端工作区都有大量并行 WIP（客户端 ~90 个 M 文件、服务端 scripts/Character/Equip 等 M）；必须 hunk 级挑选，避免把别人的半成品带进提交（会话踩过两次，已修正）。

---

## 附：本轮证据索引
- 会话解压：`C:\Users\11036\.dsh\_s1c97_dump.txt`（4702 行）、`_turn13/14/15.txt`
- 现场核实：客户端 log `b20fa501/dbf3ee3e/deeac074`；服务端 log `d09cc96d7b/517eb0d48b`；`Configuration.h:1036` v2=true
- 产物：`BeiDou.jar` 08:45；`MapleStory_setv2.exe` SHA `8857fc21` 09:25；主 `MapleStory.exe` `0fcb2173` 08:10
- 缺口复核：服务端 0x175 仅 opcode 定义（`SendOpcode.java:367`）；客户端 `CombatPanelStats` 在 `Gameplay/**` 零引用
- 规划输入：本目录 `README.md`、`E:\MXD\扩展改动\伤害玩法\` 三份文档、服务端 `docs/features/setitem-server-authority-2026-11-18/README.md`

---
---

# 第七部分 · v2 定稿：官方公式 + 服务端权威校验 + 高扩展架构

> 用户 2026-11-18 新决策，覆盖前文旧 Plan。已按此方针重做取证：官方 Wiki 伤害公式 + xy 分支 `org.gms.combat` 可移植资产 + s9/Mob.cpp 现状。

## 7.1 用户决策原文拆解（6 条方针）

| 方针 | 原文 | 落地含义 |
|---|---|---|
| ① 先验证 | "先做验证" | Phase 0 真机验收优先，验不过先回滚 |
| ② 官方完整公式 + 去开关 | "接伤害公式，最好有官方完整的伤害计算、加成等计算公式；正常情况应该不需要配置进行切换的" | 按官方 Wiki 乘区实现完整公式；**删除/废弃 `damage.formula_v2` 开关**，官方公式成为唯一路径 |
| ③ 属性驱动 | "装备没有公式就不计算它呗" | Modifier 零值不注入、Profile 无来源即短路——没有该属性就不产生乘区，不为不存在的属性写死分支 |
| ④ 双端同步 + 防篡改 | "服务端、客户端同步开发，设计一定要合理、验证、防客户端篡改、服务端校验" | 服务端建独立 CombatStatProfile + calcDmgMax，客户端伤害上报做上限/倍率校验，异常告警 + AB 记分 |
| ⑤ 面板暂不换 | "属性面板暂时不换，提供面板只是为了需要补全这些属性和能力，用于扩展和后期扩展开发用" | UIStatsInfo 三栏不动；本轮目标是"属性字段 + 能力管线"齐备，展示留后期 |
| ⑥ 周边一并移植 + 高扩展 | "涉及到的周边可以一并移植；整体计划要学习官方做法、架构一定要合理、高扩展" | 移植 xy `org.gms.combat` 核心层 + 战力/排行周边；Provider 注册表式扩展，新增来源零改公式层 |

## 7.2 官方完整伤害公式（MapleStory Wiki `Damage_Formula`，2026-08-31 版）

> 全部数值为小数（50% = 0.5）。乘区顺序即官方 `approximate average output`：

```text
Damage ≈ 0.01
  × 武器常数(Weapon Constant, 1.2~1.75 按武器)
  × 属性倍率 [4×(主属性1+2+3) + 副属性1+2]
  × 攻击力/魔力倍率 [floor(Base×(1+%)) + 未应用%]
  × 伤害倍率 [打Boss: 1+%Damage+%BossDamage / 打普通: 1+%Damage+%NormalDamage]   ← 加法池
  × 最终伤害倍率 [各来源独立连乘 Π(1+FD_i)，来源内加算]
  × 暴击伤害倍率 [暴击:1.35+%CritDam / 非暴击:1]
  × 无视防御倍率 [1 − 怪物防御%×(1−无视防御%)，无视来源间 1−∏(1−src) 连乘]
  × 无视元素抗性倍率 [多数Boss 0.5×(1+%), 普通怪 1]
  × 等级倍率 [同级1.1, 每高1级+0.02, 高5级1.2; 低于怪物按表, 低40级→0]
  × 地图倍率 [Sacred/Arcane/Star Force 与推荐值比]
  × 技能倍率 [技能 %p 加到每 hit damage%]
  × 熟练度倍率 [平均 0.5×(1+Mastery%), 每 hit 在 Mastery%~1 随机]
```

**叠加规则总纲**（与三份设计文档 + xy 实现一致）：
- 同类百分比（damR/bossR/nbdR/暴伤）进**加法池**，一次乘算（有稀释）
- 最终伤害%**各来源独立连乘**（无稀释）
- 无视防御%来源间 `1−∏(1−src)` 连乘（不是简单相加），加法 cap 是简化模型
- 暴击基础 1.35（不是旧客户端固定 1.5）
- 属性/攻击力百分比先加到基础值再取整，不是最终乘法

> 交叉印证：xy `CombatPowerCalculator.java:15-28,79-96` 的"官方乘区顺序"注释与 Wiki 一致；`maplestorywiki.net`、`NamuWiki`、`strategywiki` 三站口径吻合。经典 v83 分支（Artale）无 Boss伤/最终伤/无视防御等，故 s9 现状是"v83 基底 + 高版本扩展属性"，公式要按高版本补。

## 7.3 目标架构（高扩展、服务端权威、属性驱动）

```
┌──────────── 权威配置层（DB 唯一真理源，无本地写死公式）────────────┐
│ xy_set_item / [新]xy_equip_enhance_rule / [新]xy_carry_item_stat   │
│ GameConfig caps（combat_ignore_pdr_cap 等，显式接线 fromGameConfig）│
└────────────────────────┬───────────────────────────────────────────┘
                         │ 启动加载 / web CRUD 热更 / 广播
                         ▼
┌──────────── 服务端 CombatStat 聚合层（移植 xy P0 核心层）──────────┐
│  CombatStatType(10+键) + StackRule(ADD/MULT/ADD_CAP)              │
│  CombatStatModifier(type,value,source,sourceId,priority)           │
│  CombatStatResolver.resolve() → 不可变 CombatStatProfile           │
│    零值过滤 + 无来源短路 + cap 动态读 GameConfig                    │
│  Provider 注册表（高扩展核心）：                                    │
│    SetBonusProvider(已有) / EquipProvider(新) / EnhanceProvider(新) │
│    CarryProvider(新) / Potential(新) / Buff / Passive(新)           │
│    → 新增来源 = 新增 Provider，零改公式层                           │
└───────┬─────────────────────────────┬──────────────────────────────┘
        │ ① 校验线（每次攻击）         │ ② 展示线（换装/登录/热更）
        ▼                             ▼
┌────────────────────┐    ┌──────────────────────────────────────┐
│ DamageCalculator   │    │ 0x176 扩展包(已通) + 0x175 战斗面板    │
│ + DefenseCalculator│    │ setId/text/tier(含combatStats全字段)  │
│ calcDmgMax 全乘区   │    │ → 客户端 SetTooltip / CombatPanelStats │
│ × attack% × damage% │    │ → UIStatsInfo(本期不换面板,只喂数据)   │
│ × fd% × crit × def  │    └──────────────────────────────────────┘
│ 防篡改校验：        │
│   >1.5× → alert     │
│   >5×  → addPoint   │
│   段数>max → addPoint│
└─────────┬──────────┘
          │ 校验客户端上报 damage（不强行全服务端重算，保留客户端算伤害的架构现实）
          ▼
┌──────── 客户端伤害公式（Mob.cpp，去 v2 开关后唯一路径）─────────┐
│ 官方乘区全量：damR+bossR(加池) / fdR(连乘) / ignorePDR /         │
│ crit(1.35+cd) / 属性克制 / 等级 / 技能 / 熟练度                    │
│ 数据源：Attack 结构（由 CharStats + CombatPanelStats 聚合）        │
│ 报伤害 → 服务端校验                                               │
└──────────────────────────────────────────────────────────────────┘
```

**防篡改设计的三个关键**（对齐 xy `AbstractDealDamageHandler:773-1046`，但修复其"Profile 未接入"缺陷）：
1. **calcDmgMax 必须乘 Profile**：`calcDmgMax × attackPercentMultiplier × damagePercentMultiplier × finalDamageMultiplier × maxCritMultiplier`——否则 fdR/bossR 玩家一开就被误判为外挂（xy 的已知缺陷，文档 §5.3 明确要求但未实现）。
2. **双重计算防护**：v83 客户端公式已含 WATK/MATK/SharpEyes/技能倍率/Buff damage；服务端**只补客户端没算的乘区**（padR/madR 潜能%、damR 加法池增量、fdR、无视防御差额），已算过的绝不重复乘（xy `BuffStatProvider:12-13` 的教训）。
3. **灰度开启**：先 alert-only 跑一段看误报率，再开 addPoint；cap 必须接线，避免配置错误放大伤害上限。

## 7.4 可移植周边清单（xy 分支，按优先级）

**P0 核心（直接搬，改包名/依赖）**：`org/gms/combat/stat/{CombatStatType,CombatStatModifier,CombatStatProfile,CombatStatResolver,CombatStatCaps,CombatStatSource,CombatStatJson}.java`、`combat/damage/{DamageCalculator,DefenseCalculator}.java`、`combat/provider/{CombatStatProvider,CombatProfileService}.java`、`combat/format/CombatStatFormatter.java`。路径前缀 `E:\project\BeiDou-Server\gms-server\src\main\java\`。

**P1 来源与缓存（按 s9 结构改）**：`server/setitem/{SetBonus,SetTiersV2Parser,SetItemManager}` 的 combatStats 段、`client/Character.java` 的 `combatStatProfile` 缓存与 `refreshSetBonus`、`util/PacketCreator.java:7747-7784` 0x175 扩展封包。**新增** `MonsterStats.pdr/mdr` 字段 + WZ `PDRate` 解析（xy 有，s9 无）。

**P2 周边（一并移植）**：`combat/power/{CombatPowerCalculator,EquipRankCategory,EquipScoreCalculator}.java`、`net/server/task/CombatPowerRankingTask.java`、`service/RankingService` 战力段、战斗排行 DTO/Entity/Mapper、`V1.11.30__combat_equip_ranking.sql`（两张表，DDL 按备注规范）。

**反作弊参照（改造目标，非直接搬）**：xy `AbstractDealDamageHandler:773-1046`（calcDmgMax + 1.5 alert/5 addPoint）、`AutobanFactory.DAMAGE_HACK`。

**明确不存在、需新写**：`EquipStatProvider`、`xy_equip_enhance_rule.sql`、`xy_carry_item_stat.sql`、Enhance/Carry/Potential/Buff 真实 Provider、`DamageContext`、`DamageCalculator.computeMax`、CombatStat 单测（稀释/连乘/无视防御三个官方示例）。

## 7.5 实施阶段（按依赖排序，双端同步）

### Phase 0 — 验证先行（🔴 阻塞后续）
现况已核实：8484 服务端 PID 47644 启动于 9:46:55 > jar 08:45（**已用新套装代码重启**）；MySQL(3306) 在线；产物 `BeiDou.jar`08:45 / `MapleStory_setv2.exe` SHA `8857fc21`09:25 就位。
- **P0-1（用户）**：切 `MapleStory_setv2.exe`，hover 1003242 截图——验套装名 + 属性行(HP%/MP%) + 档位点亮。
- **P0-2（用户）**：v2 伤害回归截图——暴击/魔法/BOSS 数值正常，防暴击归零。
- **P0-3（用户）**：web `SetItemController /reload` 改一条 tier → 在线 hover 即时变（写库 DML 需授权，否则 GM 命令）。
- **P0-4（agent）**：本端可做——用 packet_dump + auto_login 抓 0x176 下发字段序，蓝军核对与客户端解析一致；无需 GUI。
- **门**：P0-1~3 三张真机图齐才进 Phase 1；异常则 `damage.formula_v2:false` 回滚（但注意：Phase 2 起该开关将被移除，回滚改走配置 cap=0 / 灰度开关）。

### Phase 1 — 核心层移植 + 测试护栏（服务端，无行为变更）
- 移植 P0 核心 10 类；补 `CombatStatCaps.fromGameConfig()` 接线；补三示例单测（稀释/连乘/无视防御 cap）。
- 接 `SetBonusProvider`（复用现有 s9 SetBonus.combatStats）；Enhance/Carry/Potential 先空 Provider（零值短路，属性驱动）。
- **门**：单测全绿 + mvn compile/package exit 0；此时游戏行为不变（公式未接入）。

### Phase 2 — 伤害公式统一 + 去开关（双端同步）
- **服务端**：`MonsterStats` 加 pdr/mdr；`AbstractDealDamageHandler.calcDmgMax` 接 Profile 全乘区 + 1.5/5 阈值 + 段数校验（灰度 alert-only）。
- **客户端**：`Mob.cpp:901+` 按官方乘区重写 `calculate_damage`，**删除 `damage.formula_v2` 分支**，官方公式成唯一路径；fdR 乘区接 `CombatPanelStats.final_damage_percent`；暴击基准 1.5→1.35+cd。
- **封包**：服务端 0x175 发包按 `CommonHandlers.cpp:68-107` 17 字段序（fd_count 支持多来源）；0x176 已通。
- **门**：双端公式单测/蓝军字段核对；真机伤害数值对照官方公式（选一个已知装备组合手算验证）。

### Phase 3 — 周边移植（战力/排行/强化/携带物）
- 战力计算 + 装备评分 + 战斗排行（P2 清单 + SQL，DDL 带备注）。
- `xy_equip_enhance_rule` / `xy_carry_item_stat` 表 + Provider（复用 `CombatStatJson.fromEnhanceStatsJson`）+ web 编辑；强化窗客户端（可选，面板暂不换的前提下先在 tooltip 展示）。
- **门**：新增来源只加 Provider，公式层零改动（架构验收点）；DDL 按备注规范；web 写库授权。

### Phase 4 — 面板数据补全（不换面板样式）
- `CombatPanelStats` 17 字段接进 `CharStats`/`Attack` 聚合（供公式消费，不改 UIStatsInfo 布局）；`UIStatsInfo.cpp:706-716` 硬编码接真实值（仅数值，布局不动）。
- 属性字典/Formatter 供后期扩展；异常状态细分抗性按职业/版本确定清单。
- **门**：面板样式零改动；属性数值真实；后期新增属性只需加 Type + Provider。

## 7.6 待确认（进入实现前，仅列真正决策点）

| # | 决策点 | 选项 | 推荐 |
|---|---|---|---|
| V1 | Phase 0 真机三图谁做 | A：你切包截图验完再动代码 / B：我并行推 Phase 1 核心层，验收穿插 | **A**（用户已定"先做验证"） |
| V2 | 伤害计算终局形态 | A：服务端校验客户端上报（保留客户端算，推荐，改动可控）/ B：服务端全量重算怪物扣血 | A |
| V3 | 暴击基准是否改官方 1.35 | A：改 1.35+cd（贴官方）/ B：维持 v83 1.5+cd | A（跟官方） |
| V4 | 无视防御模型 | A：官方连乘 `1−∏(1−src)` / B：设计文档的加法+cap | A（跟官方），v83 无 PDR 用 `convertFixedDefenseToRate` 兜底 |
| V5 | 去 `damage.formula_v2` 开关的兼容 | A：直接删分支，官方路径唯一 / B：保留开关但默认 true、标注废弃 | A（用户明确"正常不需要切换"），回滚靠灰度开关 |
| V6 | web 写库 DML 授权 | A：授权（配 dbx/自备 MCP）/ B：本轮只 backend API + GM 命令 | B 先行 |

## 7.7 风险与回滚（v2）
- **双重计算**是头号风险：服务端只补客户端未算乘区，逐字段核对（Wiki 乘区 vs Mob.cpp 现状）。
- **误封**：profile 未接入 calcDmgMax 时开幅度校验 = 灾难；先 alert-only 灰度。
- **无测试护栏**：xy 自己都没单测；移植先补三示例单测再接公式。
- **面板不改**：本期所有属性能力落在数据/公式/tooltip，UIStatsInfo 布局零改动，符合"面板暂不换"。
- **提交 hygiene**：双端大量 WIP，hunk 级挑选，沿用会话修正后的做法。
