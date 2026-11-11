# round89 · W/L/E 三桶总清单 + 一次性 P1 Plan

> 触发：m09729「字段级 body 改造过的字段、类型不影响，但是一样字段的原始逻辑可以对齐一下……保证基础功能一致，扩展功能可以不对齐；」+ m09737「其余功能也要按照这个方向进行开发、修复、完善；」
> 权威源：Angel(13339) 083/v83 带符号 IDB；s9 服务端源码仅约束 W 桶字段宽度；本端 HeavenClient 自研重实现。

## 0. 口径（对 round88 §0/§2.2/§5 的收窄，历史证据链保留）

- **W**（6 个改造字段宽度/类型）→ 保持 s9，零改动。改回 083 原生会平移包中间字段、复现 `LoginParser.cpp:150-174` 白屏。
- **L**（同字段原始逻辑/状态机/分支，不碰 wire）→ 按 Angel 083 最完全反编译还原，保证基础功能一致。
- **E**（s9 私有扩展功能）→ 可不对齐。
- 红线：083/v83 only 禁 095；数值重合≠语义重合；门禁 5 禁想象修改；数值重合≠语义重合。

## 1. W 桶 — 保持 s9 宽度/类型（6 字段 + 27 条 must_keep）

| # | 行 | s9 源码 | 083 差异 | 原因 |
| --- | --- | --- | --- | --- |
| 1 | 192 | `p.writeShort(chr.getLevel());` | 083 原生 level 1B；s9 999 级上限改 2B | s9 改造，保持 |
| 2 | 201 | `p.writeInt(chr.getHp());` | 083 原生 short；s9 99999 上限改 int | s9 改造，保持 |
| 3 | 211 | `p.writeLong(chr.getExp());` | 083 原生 exp 4B；s9 999 级累积经验改 8B | s9 改造，保持 |
| 4 | 213 | `p.writeInt((int) chr.getGachaExp());` | 083 无此字段；s9 扩展通道 | s9 改造，保持 |
| 5 | 217 | `p.writeInt(0);` | 083 无此尾部；s9 spawnpoint 后固定补 int(0) | s9 改造，保持 |
| 6 | 2624 | `public static Packet removeItemFromMap(int objId, int animation, int chrId)` | animation>=2 恒写 4B cid；083 OnPacket v2==4 读 Decode2 delay，数值重合≠语义重合 | s9 改造，保持 |

**碰 wire 的 27 条 must_keep（round88 evidence 直接引用，全部归 W 轨）**：

- `login-flow/D1` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `login-flow/D4` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `statusbar-hud/D2` K3 [KEEP_S9] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `corewindow/D3` K3 [KEEP_S9] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `corewindow/D4` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `social-economy/D4` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `social-economy/D5` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `activity-minigame/D2` K4 [DOC_ONLY] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `activity-minigame/D5` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `activity-minigame/D6` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `activity-minigame/D10` K3 [KEEP_S9] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `cashshop/D1` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `cashshop/D5` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `cashshop/D8` K4 [DOC_ONLY] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `skill/D1` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `skill/D13` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `npc/D1` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `npc/D2` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `npc/D9` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `npc/D10` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `map/D3` K2 [BATCH_0_BASELINE] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `map/D5` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `map/D14` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `quest/D1` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `quest/D3` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `drop/D1` K2 [BATCH_1_KEEP] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束
- `drop/D5` K3 [P3_NO] — 改回 083 原生包形会断 s9 联机，保持 s9 硬约束

## 2. L 桶 — 按 Angel 083 还原原始逻辑（reopen 27 + 本轮 1 + drop 2）

### 2.1 本轮唯一落地补丁（零 wire）

- `IO/UITypes/UIQuestLog.cpp::change_tab`：083 `CUIQuestInfo::OnTabChanged @0x87fd4f` 首行 `*(this + 369) = -1;`（清选中）→ 当前端在 `tab = tabid;` 后补 `selected_id = -1;`，由 `rebuild_list()` 自动选第一条可见任务，对齐 `TryShowTopQuest` / `ToggleDetail(0)` 语义。零 wire / 零协议 / 零 WZ / 零新封包。
- `quest/D5` 的 `UIQuestLog::send_key` 空实现**不改**：083 全库 `*Quest*OnKey*` / `*Quest*SendKey*` 符号 total=0，为对齐而发明键盘逻辑违反门禁 5。

### 2.2 round86 一刀切 KEEP 的纠偏对象（reopen 27，逐条 P1 拍板）

| face/id | kind | 原 verdict | 类别 |
| --- | --- | --- | --- |
| `login-flow/D3` | K3 | `KEEP_S9` | PURE |
| `login-flow/D9` | K4 | `KEEP_CURRENT` | PURE |
| `login-flow/D12` | K4 | `DOC_ONLY` | MIXED |
| `login-flow/D13` | K2 | `BATCH_0_BASELINE` | MIXED |
| `statusbar-hud/D1` | K2 | `BATCH_1_KEEP` | PURE |
| `statusbar-hud/D12` | K2 | `BATCH_1_KEEP` | PURE |
| `corewindow/D2` | K2 | `BATCH_0_BASELINE` | MIXED |
| `corewindow/D8` | K3 | `DOC_ONLY` | PURE |
| `corewindow/D9` | K2 | `BATCH_1_KEEP` | PURE |
| `corewindow/D10` | K2 | `BATCH_0_BASELINE` | MIXED |
| `social-economy/D3` | K2 | `BATCH_1_KEEP` | PURE |
| `social-economy/D9` | K2 | `BATCH_1_KEEP` | PURE |
| `social-economy/D12` | K2 | `BATCH_1_KEEP` | PURE |
| `pet-summon/D2` | K2 | `BATCH_0_BASELINE` | MIXED |
| `activity-minigame/D1` | K4 | `KEEP_WITH_NOTE` | PURE |
| `cashshop/D2` | K2 | `BATCH_1_KEEP` | PURE |
| `skill/D4` | K2 | `BATCH_1_KEEP` | PURE |
| `skill/D9` | K2 | `BATCH_1_KEEP` | PURE |
| `skill/D11` | K2 | `BATCH_1_KEEP` | PURE |
| `skill/D12` | K2 | `BATCH_1_KEEP` | PURE |
| `map/D1` | K2 | `BATCH_0_BASELINE` | MIXED |
| `map/D4` | K2 | `BATCH_1_KEEP` | PURE |
| `map/D8` | K2 | `BATCH_1_KEEP` | MIXED |
| `quest/D5` | K3 | `KEEP_CURRENT` | PURE |
| `quest/D8` | K2 | `BATCH_1_KEEP` | PURE |
| `drop/D2` | K2 | `BATCH_1_KEEP` | PURE |
| `drop/D12` | K2 | `BATCH_1_KEEP` | PURE |

### 2.3 已有 083 活 IDA 证据、登记 P1（本轮不落补丁）

- ~~`drop/D2` / `drop/D12` 按 L 桶下一批还原~~ **round90 已证伪并注销（见 §7）**：两条决策点都在 **269 离场** `CDropPool::OnDropLeaveField @0x506590`，不在 268 进场 `OnDropEnterField @0x505900`；逐字 `tools/_ida_r90/OnDropLeaveField.code.txt:83-99` = `v2=Decode1(animation)` → `Decode4(oid)` → `if(v2==2||3||5) Decode4` / `else if(v2==4){ Decode2(delay); *(v4+68)=get_update_time()+delay; *(v79+72)=4; }` / `v2==0` 走 `IWzGr2DLayer` COM 消亡动画。s9 `PacketCreator.java:2624-2642` 对 `animation>=2` 恒写 `writeInt(chrId)` 4B ⇒ **D12 = W 桶 wire 宽度分叉**（083 2B delay vs s9 4B cid，数值重合≠语义重合，改回读 short 会平移字段、断联机）、**D2 = F 维 P2**（COM 画布栈当前端没有，48 帧淡出语义等价）。两者均非 L 桶，从 `L.p1_registered` 注销；168 条历史裁定与 §4 算术不动。
- `quest/D8`、`skill/D4`、`corewindow/D8` 等状态机分支同理，逐面 REA→IDA→IMG 后再落，不凭空写。

## 3. E 桶 — s9 私有扩展（不对齐，5 项逐条带证据）

| 扩展项 | 证据 | 理由 |
| --- | --- | --- |
| GachaExp 扩展通道 | `PacketCreator.java:213 p.writeInt((int) chr.getGachaExp());` | 083 无此字段，s9 私有扩展 |
| 999 级上限 | `PacketCreator.java:192 p.writeShort(chr.getLevel()); // 999 级` | 083 原生 1B level，s9 私有数值扩展 |
| 99999 HP/MP 上限 | `PacketCreator.java:198-205 p.writeInt(chr.getHp()); ×4` | 083 原生 short，s9 私有数值扩展 |
| c2s 107 抓包工具 | `docs/features/pending-live-tests/README.md P3 长尾台账` | s9 侧运营工具，客户端基础功能不含 |
| 095 自定义中文 UI（福利中心） | `IO/UIStateGame.cpp:532 + IO/UITypes/UISpecial.h:9（round86 T2 已判为本端扩展身份标注）` | 本端扩展入口，非 083 原生功能，机械换地址=二次造假 |

E 为保守枚举：未列扩展项默认仍归 L 轨，逐条拍板时可追加，不硬凑数字。

## 4. 168 条路由闭合

| 轨道 | 组成 | 数量 |
| --- | --- | --- |
| **W 轨** | `class=WIRE`（含 must_keep 27） | 38 |
| **L 轨** | `class=PURE` 76 + `MIXED` 14 + `OTHER` 40 | 130 |
| **合计** | 38 + 130 | **168** |

round88 四分类计数 `PURE76/WIRE38/MIXED14/OTHER40` 直接复用（稳定、已证据化），不做语义再分类造假；E 为 L 轨内豁免标记，不参与算术。未决 0。

## 5. 一次性 P1 Plan（本轮 + 后续同方向批次）

1. **本轮（已落地）**：W/L/E 口径修订 + 三桶总清单 + `UIQuestLog::change_tab` 1 行真实对齐补丁（083 反编译逐字为凭）；harness + GATE + 真重编三证 + 零署名原子提交。
2. **下一批（round90 纠偏后）**：drop 面 D2/D12 已判 W / F 维 P2、**不进 P1**；改走真正有 083 状态机证据且零 wire 的 L 桶候选（`quest/D8` / `skill/D4` / `corewindow/D8`），每批先出 083 符号+反编译锚点，证据不足登记 P2/P3 不发明，再 REA→IDA→orange-wz(128B)→.nx 只读取证 → 纯逻辑补丁 → harness → GATE → 真重编 → 零署名提交。
3. **逐面推进**：reopen 27 条按 12 面分批，每批先出 083 符号/反编译锚点，证据不足的登记 P2/P3 不发明；每批一片一原子 commit。
4. **W 桶全程零改动**：6 字段宽度/类型 + 27 条 wire KEEP 不碰，避免复现白屏。
5. **真机归口**：五屏截图 60 节/142 处、`InPacket::available()` 393 处 §27-A 逐面回归、D2 聊天规则，由用户认领后销案。

## 6. 限制

- 本轮唯一源码改动 = `UIQuestLog.cpp` 1 行 + 4 行注释；零 wire / 零协议 / 零 WZ / 零新封包。
- 工作树 17 个外部程序在途 WIP 原样保留，精确 add 隔离、零夹带。
- 三桶只对「处置取向」分类，不改写 round86 DECISIONS.md 的 168 条历史裁定。
- 架构级重写（如独立 CUIPartyHP 浮窗、CUIStatusBar 拆 10+2 类）仍须单点裁决，不在自动落地范围。


## 7. round90 纠偏 — drop 面三方对账（m10601）

触发：用户「还有未完成的或者你可以自测的任务吗？」→ 按 m09211/m09729/m09737
「全部继续 / 按最完全反编译还原基础功能 / 其余同方向」授权，自测推进下一批时
先复核 §2.3/§5 的 P1 登记，发现两处误分类。**先把上一轮自己埋的错挖出来，再开工。**

| 项 | round89 登记 | round90 判定 | 决定性证据 |
| --- | --- | --- | --- |
| `drop/D12` animation=4 | L 桶 P1（按 083 还原延时爆炸帧） | **W 桶**（wire 宽度分叉，保持 s9） | 083 `OnDropLeaveField @0x506590:94-99` `Decode2(delay)`+`*(v4+68)`；s9 `removeItemFromMap` `animation>=2` 恒 `writeInt(chrId)` 4B |
| `drop/D2` animation=0 | L 桶 P1（按 083 还原消亡动画） | **F 维 P2**（渲染栈缺口，不补） | 083 `v2==0` 走 `IWzGr2DLayer::Animate/RemoveCanvas/GetCanvas`；当前端无 COM 画布栈，48 帧淡出业务结果一致 |
| 函数锚点 | `OnDropEnterField @0x505900`（268 进场） | `OnDropLeaveField @0x506590`（269 离场） | `CDropPool::OnPacket @0x5058db` 分派 268/269；反编译落 `tools/_ida_r90/` |

**reopen 27 闭合**：`27 = 25 P1 + 1 W + 1 F-P2`。168 条 `DECISIONS.md` 历史裁定
与 §4 的 `38+130=168` 算术**不动**——只在 P1 登记层注销 2 条，不做语义再分类造假
（round88 四分类是「推荐默认文本」归类，round90 是「物理 wire 事实」判定，两层不互相覆盖）。

**同族审计**：§2.3 其余候选 `quest/D8` / `skill/D4` / `corewindow/D8` 均为状态机/条件分支、
不碰 wire，属合法 L 桶候选，无同族误分类；逐面取证后按同一方法推进。

**限制**：零源码行为改动（`Drop.cpp` 仅补三方对账纯注释）/ 零 wire / 零协议 / 零 WZ /
零新封包；25 个外部程序在途 WIP 原样留工作树、精确 add 隔离；真机五屏截图 60 节/142 处仍由用户认领。

## 8. round91 纠偏 — 下一批 3 条候选的四方对账（m10739）

触发：用户「开始 下一批 的计划」→ 按门禁 1 先证据后计划，对 §2.3 :96 / §5 :123 /
§7 :152-153 登记的 3 条「合法 L 桶候选」做 083 活 IDA + s9 源码 + 当前端 + DECISIONS 四方复核。
**结论：三条全部不成立，不进 P1。这是继 round90 之后第三次同族误登记。**

| 项 | round89/90 登记 | round91 判定 | 决定性证据 |
| --- | --- | --- | --- |
| `quest/D8` | L 桶 P1（搬 `sub_721008` 分支） | **W 桶 wire 条件追加段**，保持 s9 | 083 `CQuest::StartQuest @0x71751c` `COutPacket(107)` → `if (sub_721008(questid)==0){ Encode2(x); Encode2(y); }`；`sub_721008 = IsAutoStartQuest || sub_720FED`；s9 `QuestActionHandler.isNpcNearby` `if (p.available() >= 4)` 容忍有/无 x/y；当前端 `Net/Packets/QuestPackets.h` 从不写 x/y = 合法 s9 形态 |
| `skill/D4` | L 桶 P1（新增 `CWndSkillGuide` 子窗） | **架构级单点裁决**，不新增 | 083 全库仅 2 符号（ctor `0x8aa243` / `OnKey 0x8aa4e4` `a2==27`→`CloseSkillGuide`）；当前端 `UISkillBook.cpp:797` ESC 已闭环；矩阵 `:243` / `DECISIONS.md:190` 已裁「不新增」；§6 自排除架构重写 |
| `corewindow/D8` | L 桶 P1（覆盖度对照） | **DOC_ONLY 已发货**（round75 `c07b1a7b`） | 083 三份反编译（`CUIStat::OnButtonClicked 0x8c522f` 6 掩码 / `CUIKeyConfig::OnButtonClicked 0x8335d0` 5 分支 / `CUISkill::OnSkillLevelUpButton 0x8acf1a` + `CanSendExclRequest 0x485bf7` 500ms）；s9 `DistributeSPHandler` 无 500ms 限速 + `AssignAPProcessor` 不同编码；当前端 `PlayerPackets.h:48-72` 已注明两套编码勿复用；矩阵 `:152` / `DECISIONS.md:99/293/311` 裁「只出对照清单不改代码」 |

**作废旧结论（历史文本保留，以本节为准）**：§2.3 :96「等状态机分支同理」、§5 :123「改走真正有 083 状态机证据且零 wire 的 L 桶候选（`quest/D8`/`skill/D4`/`corewindow/D8`）」、§7 :152-153「同族审计…均为状态机/条件分支、不碰 wire，属合法 L 桶候选，无同族误分类」。

**同族病根因与硬规则**：从 D 矩阵「推荐默认文本」直接抄进 P1 登记层，没先跑该条的 083 反编译 + s9 handler + 当前端字面量 + DECISIONS 原裁定。今后任何「下一批 L 桶」登记必须先附这四项，不允许从矩阵文本推导批次。

**真正干净的下一批（每条自带 083 反编译锚点，详见 `NEXT-BATCH-PLAN.md §4`）**：

- P1 首选 `skill/D12`/`corewindow/D9` 宏选中态：`CUIMacroSysEx::OnSelected @0x8a984d`（20 行纯 UI，`*(this+361)=a2`/`GetMacroName`/`IsShoutMacro`/`ResetInfo`）+ `CUIMacroSys::OnKey @0x8a98dc`（ESC→`ProcessBasicUIKey(27,a2)`），零 `COutPacket`/零 `Encode`/零 `SendPacket`。
- P1 次选 `login-flow/D9`/`statusbar-hud/D12` 频道网格：`CUIChannelShift::OnKey @0x995863` 6 列方向键纯索引；`CUIChannelSelect::OnKey @0x628a1c` **须先拆 L/W**（导航=L，`SendViewAllCharacterPacket`=W 保持 s9）。
- P2/F 维 `CUISkillEx::ShiftMacroUIState @0x8bad5a`（243 行 COM 画布栈，当前端无 COM ⇒ 不补）。

**算术不动**：168 条历史裁定 / §4 `38+130=168` / reopen `27 = 25 P1 + 1 W + 1 F-P2` 全不变；本轮只在 P1 **登记层**注销 3 条，不做语义再分类造假。
**限制**：0 源码 / 0 wire / 0 协议 / 0 WZ / 0 新封包；计划轮，实施待用户确认一次性 Plan。
## 9. round92 纠偏 — 实施片1+片2 的编号错配（m10905）

**结论：round91 Plan §4 的 4 个 P1 编号全部错配 —— 同族病连续第 4 轮，且是系统性的一批，不是按片各算一次。**

| Plan §4 编号 | round91 挂的锚点 | DECISIONS 真议题 | 本轮判定 |
| --- | --- | --- | --- |
| `skill/D12` | `CUIMacroSysEx::OnSelected` 选中态 | `deactivate()` vs `ShiftMacroUIState` 形态差异 | DOC_ONLY 编号错配 |
| `corewindow/D9` | `CUIMacroSysEx::OnSelected` 选中态 | 32 格 → 5 槽数量 | DOC_ONLY 编号错配 |
| `login-flow/D9` | `CUIChannelShift::OnKey` 网格导航 | 60 频道翻页控件（P3） | 编号错配 + 挖到真缺陷 |
| `statusbar-hud/D12` | `CUIChannelShift::OnKey` 网格导航 | `UIMobGage.*` 空 Type | 编号错配 |

- 片1 三证（128B 门禁 `Shout` 零命中 / 083 `CCtrlCheckBox` COM 栈 / 当前端无 Checkbox 实现类）坐实
  「083 是 32 格 COM 编辑窗、当前端是 5 槽组合栏」，`shown_name` 已等价 `SetText` → **零源码改动**。
- 片2 对应窗 `CUIChannelShift`(083) ↔ `UIChannel`(当前端 6 列) 门控对拍，挖到 `step_channel(±6)`
  在 `channel_count=3` 时 uint8_t 下溢 → `send_key` 加 083 `count>6`/`count>1` 门控（**本轮唯一真实 L 桶补丁**）。
- **规则回扫（P8 自我批判）**：§8 已写「登记前必附四项取证」，但同批 Plan §4 未回扫即发货。
  本轮升级：任何 P1 候选登记必须四项齐备，且由 harness 静态守卫逐条断言，不靠人肉记忆。
- 168 条 DECISIONS / §4 `W 38 + L 130 = 168` / reopen `27 = 25 P1 + 1 W + 1 F-P2` **全不动**；
  仅在 P1 登记层注销 4 个错配编号，不做语义再分类。W 桶 6 字段 + 27 wire KEEP 零改动。

## 10. round93 纠偏 - 5 处 095/v95 依据链（m11680 盘点后自测）

**结论：reopen 清单里唯一「该做且能自测」的一条 `activity-minigame/D1`（KEEP_WITH_NOTE）扩展成 5 处同类。**

| 文件:行 | 旧依据 | 083 实测 | 判定 |
| --- | --- | --- | --- |
| `IO/UIStateGame.cpp:533` | 095 自定义中文 UI：福利中心 | `*UISpecial*`/`*Fairy*`/`*Welfare*`/`*CheckIn*`/`*Attendance*` 全 0 | 本端扩展（E 桶） |
| `IO/UITypes/UISpecial.h:9` | 095 自定义中文 UI 入口 | 同上 | 本端扩展（E 桶） |
| `Gameplay/MapleMap/MapPortals.h:40` | v95 `CPortal::CheckKey` / `ProcessPortal` | `*CPortal@@*`/`*CPortal::*` 全 0；真锚点 `CPortalList::FindPortal @0x712ab1` / `FindPortal_Collision @0x712b61` | 依据链错（符号名不存在） |
| `Gameplay/MapleMap/MapPortals.cpp:146` | v95 同上 | 同上 | 依据链错 |
| `Gameplay/Stage.cpp:284` | v95 `CPortal::CheckKey` | 同上 | 依据链错 |

- 五处均为**注释**，零行为 / 零 wire / 零协议 / 零 WZ；`MapPortals` 两通道抽象（PortalScope）保留，不把 083 的 type 分派搬进来（那属行为级，另立决策点）。
- `DECISIONS.md:130` 写「引 Angel 083 `CPortal`」同样引了不存在的符号名 —— 该行是历史裁定，不改写，仅在此记明。
- 规则升级（承接 8/9 节）：注释引 083 符号前必须 `entity_query` 实证，符号名不存在即不得写入。
- reopen `27 = 25 P1 + 1 W + 1 F-P2` 算术不动；168 条 DECISIONS 裁定不动（本轮只改注释依据）。
