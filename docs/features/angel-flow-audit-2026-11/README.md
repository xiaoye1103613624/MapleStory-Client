# Angel 全流程审计 · 功能窗布局 / 按键 / 玩法闭环（2026-11-12）

> 触发：用户要求对接 Angel IDA（13339）全量核对功能窗口布局与按键，并检查副本、任务、远征、Boss 攻击特效击杀、任务全流程、学院、家族、组队、好友、怪物图鉴、勋章，以及追加的 **任务框 / NPC 脚本任务窗** 全流程是否实现、有无偏差。
> 性质：**阶段 0 取证 + 阶段 0.5 归纳 + 阶段 1 待决项**。只读，不改业务代码、不构建、不提交。
> 基准：GMS 083 / Angel `E:\MXD\反编译IDA\v83\`Angel.i64`（port 13339）；协议与联机以 s9 优先；**校准 ≠ 改回原生**。
> 禁引：`Maplestory95.i64` / port-095 / `ExpeditionIntermediary` / 韩系 Academy 导师系统。

---

## 0. 一句话结论

**不缺「窗类文件 / 出站 opcode / 入站 handler」主骨架；缺的是「共用窗数据绑定深度、083 包体/窗族形态差异、KeyConfig 槽位语义、真机验收」。**  
用户点名的 11+1 块里：**学院(Family) / 家族页签主路径 / 图鉴主链 / 勋章分号键路径 / NPC 对话主 msgtype** 已可跑；**UserList 好友·组员已灌模型（P0/P1）**；**远征独立面板按 083 裁决不做**；**Quest 107 包体与 083 结构性分叉须保持 s9**；**任务框 UINpcTalk 主链齐，P3 已收口 `#E/#I/#S/#K`→KEY_ANI 与 QuestAlarm 进度替代**。仅剩红线/无包项见 §7。

---

## 1. 取证基线

| 源 | 本轮状态 | 用途 |
| --- | --- | --- |
| Angel IDA `http://127.0.0.1:13339/mcp` | ✅ `idb_path=...\v83\`Angel.i64`，`auto_analysis_ready` / `hexrays_ready` | 权威符号 + 反编译 |
| Cursor `user-ida-pro-mcp` | ⚠️ 默认挂 BeiDou `13337` | **勿当 Angel**；本轮用 `tools/_ida_mcp.py` 直连 13339 |
| 既有十二面 Angel 校准 README | ✅ | 复用结论，避免重复反编译 |
| 源码现场 | ✅ `UIStateGame` / `KeyMapTranslate` / `UINpcTalk` / `NpcInteractionHandlers` / 各 UI* | 实现真相 |
| orange-wz | 本轮未强制重跑全量；沿用各面 IMG 门禁记录 | 布局资源以既有证据为准 |

本轮 Angel 定点：`CUIMedalQuestInfo::OnCreate @0x84056b`、`CUIFamily::OnCreate @0x80862b`、`CScriptMan::OnSay @0x7467ab` / `OnAskYesNo @0x74692c` / `OnAskMenu @0x746fad`、`CUtilDlgEx` 多 `OnCreate_*` 族（`list_funcs *CUtilDlgEx*`）。

---

## 2. 口径

| 标记 | 含义 |
| --- | --- |
| ✅ | 主链可玩 / 与 083 或 s9 约定一致 |
| 🟡 | 有实体但深度/绑定/样式/真机未齐，或与 083 形态差异（非必须改） |
| ❌ | 真缺或错误（相对本线基准） |
| ⊘ | 明确不做（083 无 / 095 专有 / 会断 s9） |
| ⏳ | 需真机或 IMG 内容层 |

---

## 3. 偏差总表（11+1 块并列）

### 3.1 任务框 / NPC 脚本任务窗（本轮追加单列）

> 权威路径：服务端 JS `npc`/`quest` 脚本 → s9 `NPCTalk`/`ScriptMessage` → 客户端 `NpcDialogueHandler` → **`UINpcTalk`**（及 Quiz/Pet/Avatar 壳）→ 用户点选 → **`NpcTalkMorePacket`** → 脚本 `action`；接/交/放弃任务另走 **`QUEST_ACTION(107)`**（任务窗/Alarm/软键），**不是** NpcTalk 内直接发 107。

| 维 | Angel 083 | 当前端 | 判定 |
| --- | --- | --- | --- |
| 分发器 | `CScriptMan::OnScriptMessage @0x74660A` → `OnSay/OnAskYesNo/OnAskText/…` | `NpcInteractionHandlers.cpp:41+` 按 msgtype case，注释对齐 `sub_74660A` | ✅ |
| 对话窗 | 单体 **`CUtilDlgEx`**（`OnCreate_YESNO/INPUT/LIST/AVATAR/PET/…`） | 主路径 **`UINpcTalk`** + `apply_dlg_skin(speaker)`；Avatar/Pet/Quiz 等进 `UIMiscShells` | 🟡 形态合并，非一一类对应 |
| msgtype 主集 | Say0 / YesNo1 / Text2 / Number3 / Menu4 / Quiz5 / Speed6 / Avatar7/8 / Pet9/10 / Accept12 / Box13 / Mirror14… | `UINpcTalk.h:38-56` 列主 wire；5/6/9/10/14 走专用壳（见 `npc-talk-utildlgex-align`） | ✅ 主集已接 |
| 出站 | `NpcTalkMore` 按 lastMsg 变长 | `NpcTalkMorePacket` + `end_talk` 分 case 2/13/5/6/-1（`:63-88`） | ✅ s9 契约对齐 |
| 布局/键 | `CUtilDlgEx::Draw/OnKey`；任务指引可 `SetButtonBlink` | 居中重锚已修；**P3：正文 `#E/#I/#S/#K`→KEY_ANI**（+ style 高字节回退）；菜单任务 blink 仍 P1 降级 | ✅ |
| 与 QUEST_ACTION | 083 接交多由脚本侧触发客户端再发 107（含 x/y） | 脚本推进靠 MoreTalk；**107 由 QuestLog/Alarm/壳软键发，NpcTalk 不直发**；包体定长无 x/y | ⚠️ 保持 s9（见 3.2） |
| 预留 | — | Quiz/SpeedQuiz 题面深度、AskPet gage 位图、Mirror 细项、UtilDlgEx 薄壳 SoftKey | 🟡 |

**全流程（接→进行中→交→放弃→失败）**

| 步骤 | 客户端路径 | 状态 |
| --- | --- | --- |
| NPC 触发对话 | s2c ScriptMessage → `UINpcTalk` | ✅ |
| 脚本多步 YesNo/Menu/Accept | MoreTalk 回程 | ✅ |
| 接任务 / 交任务 | 通常脚本 `forceStart/Complete` 或客户端 `QuestActionPacket(1/2)`；Alarm SoftKey F4→action1 | ✅ 主链；Npc 内无直发 |
| 进行中 UI | `UIQuestLog` tab1 + `UPDATE_QUEST_INFO` / Messaging 进度 | ✅ / 🟡 四钮预留 |
| 放弃 | `UIQuestLog` GIVEUP → action3（无 npc） | ✅ |
| 失败/过期 | handler mode 0x0A / 0x0F → alarm + 刷新 | ✅ |
| 083 `CNoticeQuestProgress` 浮层 | IMG 无独立节点；**P3 定案：UIQuestAlarm 5×3s 完整替代**（见 P3 等价边界） | ✅ 降级 |

### 3.2 任务（QuestLog 数据窗 + 107 协议）

| 项 | 判定 | 说明 |
| --- | --- | --- |
| 窗 | 🟡 | `UIQuestLog` 合并窗 vs 083 `CUIQuestInfo`+Detail+`CUIMedalQuestInfo` 族 |
| 按键 | ✅ | `QUESTLOG` 开窗 |
| 协议 | ⚠️ | opcode 107 同号；**083 变长+x/y+action0 buffer vs 本端定长+npcid** → 保持 s9 |
| 按钮 | 🟡 | GIVEUP 已发；`BtOK/BtNo/BtAlert/BtMarkNpc` 预留（无协议证据不发明） |
| 校准面 | | `quest-angel-calibration`：✅62 / ⚠️10 / ⏳26 |

### 3.3 副本（Instance / Field 玩法）

| 子类 | 判定 | 说明 |
| --- | --- | --- |
| Carnival / Wedding / Snow / Coconut / Dojo | 🟡 | Handler + 壳/HUD，无 `CField_*` 统一类 |
| GuildBoss | 🟡 | `MapGuildBoss`+`UIGuildBoss`+287/288 **已注册**（旧 gap C17「无 handler」过时）；pulley 上行预留 |
| 通用「进本→清怪→清场」 | 🟡 | 依赖 s9 脚本+换图；客户端无独立副本大厅窗 |

### 3.4 远征（Expedition）

| 项 | 判定 | 说明 |
| --- | --- | --- |
| 独立招募面板 | ⊘ | 083 UserList 五页无 BossParty；`BOSSPARTY` 键→**公会页签** |
| 玩法参与 | 🟡 | 队伍 + NPC + 事件；⏳ 真机未复拍 |

### 3.5 Boss 攻击特效 / 击杀

| 项 | 判定 | 说明 |
| --- | --- | --- |
| Mob hit/die/skill 帧 | ✅ / 🟡 | `Mob.cpp` 主链；出生 SpecialEff 仅存未播（G8） |
| BossHP | ✅ / ⏳ | `UIBossHP`+FIELD_EFFECT mode5；**P1 多 oid 叠画**（真机 #38） |
| 伤害数字 | 🟡 | s9/Journey 路径；非 083 原生类 → 校准≠改回 |
| wave4 场地互伤等 | ⊘ / 依赖 | 按 wave4 文档，非本批必做 |

### 3.6 学院（Family，中文「学院」= action26）

| 项 | 判定 | 说明 |
| --- | --- | --- |
| 窗/协议 | ✅ | `UIFamily`/`UIFamilyTree` + FamilyPackets 145–153 + s2c |
| 按键 | ✅ | `MEDALS` 槽 → Family（枚举名错位、行为正确） |
| 韩系 Academy | ⊘ | 非 083 |

### 3.7 家族（Guild，G 键 → UserList 第 3 页）

| 项 | 判定 | 说明 |
| --- | --- | --- |
| 主路径 | ✅ | `UIUserList` GUILD + `GuildModel` + 12 钮 |
| 浮窗 `UIGuild` | 🟡 | dead-code / 底栏保留；非 083 主入口 |
| BBS / 联盟页 | 🟡 | 有窗；⏳ 真机 |
| GuildBoss HUD | 🟡 | 见 3.3 |

### 3.8 组队 Party

| 项 | 判定 | 说明 |
| --- | --- | --- |
| 协议 | ✅ | `PARTY_OPERATION` 全链 |
| UI | 🟡 | **P0 已灌 PartyModel**；P1 行选中+Whisper；083 全钮深度仍预留 |
| 组队搜索 | 🟡 | O 键 `UIPartySearch`；`REGISTER(220)` 用 Start 代理 |

### 3.9 好友 Buddy

| 项 | 判定 | 说明 |
| --- | --- | --- |
| 协议 | ✅ | `BUDDYLIST=130` 增删接受 |
| UI | ✅ / ⊘ | **P0 已灌 BuddyListModel**；P1 行选中+Whisper/Where/Delete；拒绝列表 **P3 正式 ⊘**（s9 无 Ignore，FindPlayer 代理） |

### 3.10 怪物图鉴

| 项 | 判定 | 说明 |
| --- | --- | --- |
| 窗布局 | ✅ | D2 双页+Angel 锚点；**P1 String 链**；**P3 便签分类=`suffix/1000`**（LoadCard 真公式）+ 筛选 |
| 按键 | ✅ | v83 `22` → `MONSTERCOLLECTION` → `UIMonsterBook`（`KeyMapTranslate`） |
| 协议 | ✅ | MonsterBook handlers/packets |

### 3.11 勋章

| 项 | 判定 | 说明 |
| --- | --- | --- |
| 窗 | 🟡 | `UIMedal` 260×374；**P1 文本 Detail 305×373**；114 图标列仍预留 |
| 运行时键 | ✅ | v83 `27` → `MEDAL(28)` → 开窗 |
| KeyConfig | ✅ | **P0 已改挂 MEDAL**；BOSSPARTY 槽移除 |

### 3.12 按键总览（跨功能）

| v83 action | 语义 | 翻译/行为 | 风险 |
| --- | --- | --- | --- |
| 22 | MONSTERBOOK | → MONSTERCOLLECTION → 图鉴 | 低 |
| 23 | CASHSHOP | → CASHSHOP → 进商城 | 低 |
| 24 | TOALLIANCE | → ALLIANCECHAT | 低 |
| 25 | PARTYSEARCH | MANAGELEGION → PartySearch | 低（命名） |
| 26 | FAMILY | MEDALS → UIFamily | 低（命名） |
| 27 | MEDAL | → MEDAL(28) → UIMedal | **KeyConfig 仍暴露 BOSSPARTY** |
| — | BOSSPARTY 枚举 | → UserList GUILD | 与远征/勋章名均不对齐（083 裁决可接受） |

---

## 4. 推荐方案（默认，待你确认后进 Plan）

1. **总体取向**：保持 **s9 联机优先**；Angel 只作对照与布局/语义校准，不盲改 107 包体、不建 095 远征窗、不做韩系 Academy。
2. **P0（键位/文档债）**：KeyConfig 暴露 `MEDAL(28)`、弱化或改标 `BOSSPARTY` 槽；回写 gap 文档中过时的勋章❌ / GuildBoss 无 handler。
3. **P1（社交共用窗）**：UserList 好友/组员页灌 `BuddyList`/`Party` 数据（对齐 Angel `CUIUserList` 单窗），或明确「双窗可接受」并改入口避免空态。
4. **P1（任务框）**：维持 `UINpcTalk` 单体；补任务指引 blink 是否立项交裁决；NpcTalk 不强制内嵌发 107。
5. **P2**：BossHP 多 oid；QuestLog 预留钮仅在有协议证据后接；图鉴 String 链；勋章 Detail；真机冒烟矩阵。
6. **本轮不实现代码**，等阶段 1 抉择 + Plan 确认。

---

## 5. 阶段 1 · 待决项（仅证据消不掉的）

| # | 待决 | 选项 | 推荐 |
| --- | --- | --- | --- |
| **T0** | 总体取向 | (a) s9 优先只校准差异文档 (b) 尽量改回 083 包体/窗族 | **(a)** |
| **T1** | UserList 好友/组员空壳 | (a) 灌数据进五页签 (b) 保留双窗、改键位直开 Buddy/Party 窗 (c) 暂挂 | **(a)** |
| **T2** | KeyConfig 勋章/BOSSPARTY | (a) 槽位改挂 MEDAL(28)、BOSSPARTY 隐藏或改名 (b) 维持现状只改注释 | **(a)** |
| **T3** | 任务 107 x/y / action0 | (a) 不补 (b) 补 083 包体 | **(a)** 不补 |
| **T4** | 任务框指引 blink / 独立 Progress 浮层 | (a) P2 立项对齐 Angel (b) 明确不做 | **(a)** 挂 P2 |
| **T5** | NpcTalk 是否直发 QUEST_ACTION | (a) 保持脚本+Log/Alarm 双通道 (b) 在 Accept 钮旁加发 107 | **(a)** |
| **T6** | 远征 | (a) 维持队伍+NPC、面板不做 (b) 自研招募面板 | **(a)** |
| **T7** | 本批 Plan 范围 | (a) 仅 P0 键位+文档+UserList 数据绑定 (b) 含任务框 blink+BossHP+图鉴 String (c) 全量 P0–P2 | **(a)** 先收口可验收切片 |
| **T8** | 真机冒烟 | (a) 你屏前按清单测 (b) 本环境只做静态/harness | **(a)** 关键路径需你测 |

**无待决（已有证据+既有裁决，不必再问）**：韩系 Academy ⊘；095 BossParty 面板 ⊘；orange-wz 内容层失败时 `.nx` 回退；校准≠改回原生。

---

## 6. 产物索引

| 路径 | 内容 |
| --- | --- |
| 本文件 | 总审计 + 待决 |
| `notes/r01-scope-and-baseline.md` | 范围与端点 |
| `notes/r02-npc-script-quest-talk.md` | 任务框/NPC 脚本窗专页 |
| `evidence/angel-health-13339.json` | 本轮 Angel health 快照 |
| 复用 | `quest-angel-calibration` / `social-loop-audit` / `social-economy-angel-calibration` / `npc-talk-utildlgex-align` / `feature-gap-inventory` |

---

## 7. 计划状态（2026-11-12）

- **T0–T8**：用户已全部按推荐确认。
- **P0**：见 [PLAN.md](PLAN.md) — **已交付**（KeyConfig MEDAL + Buddy/Party 模型 → UserList）。
- **P1**：见 [P1.md](P1.md) — BossHP 多 oid / 图鉴 String / 勋章 Detail / T4 降级 / UserList 行选。
- **P2**：见 [P2.md](P2.md) — KEY_ANI API+style 门闩 / Progress 槽 / 便签筛选 / 114 图标；Ignore ⊘。
- **P3**：见 [P3.md](P3.md) — **可做项全部收口**：`#E/#I/#S/#K`→KEY_ANI；NQP=QuestAlarm 完整替代；图鉴 `suffix/1000` 真公式；Ignore 正式 ⊘；文档+构建。
- **真机**：`pending-live-tests` **#37+#38+#39+#40**。
- **仅剩红线/外部阻塞**：107 x/y·action0；NpcTalk→107；远征面板；095；真 Ignore 包；独立 NQP 美术资源；手指精灵画布（功能已由 KEY_ANI 收口）。

### 可做项全部收口声明

端内不碰红线的剩余项已全部：证据 → 实现或显式 ⊘/替代 → 注释 → 构建 → 验收步骤。无静默半成品、无发明协议。

### 核查补记（2026-11-12 r07）

只读复核 NpcTalk / Reactor·脚本·换频广播·喇叭 / Equip·Item Tooltip → 见 [notes/r07-verify-npctalk-reactor-broadcast-megaphone-tooltip.md](notes/r07-verify-npctalk-reactor-broadcast-megaphone-tooltip.md)。

### 销案（2026-11-12 r08）

喇叭 type8 / 0x0A / Avatar 包头、EquipTip cash+交易限制+分隔线下冒号居中白字 inc、ItemTip TOOLTIPBODY、DamageReactor stance/skill → [notes/r08-megaphone-tooltip-fix.md](notes/r08-megaphone-tooltip-fix.md)。真机见 pending **#41**。
