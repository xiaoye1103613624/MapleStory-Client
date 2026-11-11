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
## 11. round94 分流 - reopen 残余 17 条定 P1/P2/P3（m11928）

**算术先自证**：27 条 reopen 中前四轮已注销 **10** 条
（round90 `drop/D2`+`drop/D12`；round91 `corewindow/D8`+`skill/D4`+`quest/D8`；
round92 `corewindow/D9`+`skill/D12`+`login-flow/D9`+`statusbar-hud/D12`；round93 `activity-minigame/D1`），
**残余 17 条**本轮逐条落桶：**P1 = 0 / P2 = 1 / P3 = 16**，`0 + 1 + 16 = 17`。

| # | 面 | ID | 类 | 桶 | 处置 | 不落 P1 的理由 |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | `login-flow` | **D3** | K3 | **P2** | 等 s9 协议侧确认是否发追加段后再定；本轮不动 | 缺 s9 侧的追加段证据，按门禁 5 不发明解码分支 |
| 2 | `login-flow` | **D12** | K4 | **P3** | 只做逐字节核对文档，代码零改动 | 字段布局属 W 桶 6 字段范围，改则断联机 |
| 3 | `login-flow` | **D13** | K2 | **P3** | 保持 s9 固定 15 槽 | s9 服务端按固定槽发，改按 charcount 会错位 |
| 4 | `statusbar-hud` | **D1** | K2 | **P3** | 保持 10+2 类拆分 | 合并回单体会重写全部 HUD 类，风险极高且无行为收益 |
| 5 | `corewindow` | **D2** | K2 | **P3** | 保持 s9 联机优先，差异逐条标注 | 改回 083 原生包形会断联机（LoginParser 白屏实锤） |
| 6 | `corewindow` | **D10** | K2 | **P3** | 保持 s9 布局，右上玩家列表不判缺失 | 原生「无在线玩家列表」已坐实，右上列表是 s9 挂接 |
| 7 | `social-economy` | **D3** | K2 | **P3** | 保持双入口（round85 已收敛 4 处旁路入口） | 083 无独立窗，s9 已用双入口；合并风险极高 |
| 8 | `social-economy` | **D9** | K2 | **P3** | 明确不做/搁置 | 083 顶层清单无独立窗、s9 无后端，立项=发明能力 |
| 9 | `social-economy` | **D12** | K2 | **P3** | 保持英文原值、只建键 | 等 083 String.img / IDA 依据再译，否则自造文案 |
| 10 | `pet-summon` | **D2** | K2 | **P3** | 保持 s9 解包形态 | 改回原生读 8B 会整体平移后续字段 |
| 11 | `cashshop` | **D2** | K2 | **P3** | 不拆 | 单窗功能等价，拆窗是形态重写无收益 |
| 12 | `skill` | **D9** | K2 | **P3** | 保持双布局 | classic 分支服务不同分辨率，收敛会砍可用布局 |
| 13 | `skill` | **D11** | K2 | **P3** | 保持 DISABLED | s9 无对应技能通道，接管=发明封包 |
| 14 | `map` | **D1** | K2 | **P3** | 保持 s9 全量拆法 | s9 走全量包，083 走 89 键差分，改则断联机 |
| 15 | `map` | **D4** | K2 | **P3** | 保持当前端侧栏 + 付费传送 | MFC 大窗依赖 083 窗口框架，重写属架构级 |
| 16 | `map` | **D8** | K2 | **P3** | 保持既有注记，不回改 | 服务端 (byte)readInt 截断是 s9 真实行为，客户端回改会错位 |
| 17 | `quest` | **D5** | K3 | **P3** | 不补（083 全库 *Quest*OnKey* 命中 0） | 083 无 CUIQuestInfo 键盘 handler，补=发明逻辑 |

**P1 = 0 的判定口径**：P1 门槛是「083 反编译有状态机证据 + 零 wire + 当前端确有缺陷」三条同时成立。
残余 17 条的 canonical 裁定原文全部是「保持 / 不补 / 不做 / 不改」，属**已裁定不实施**而非「待做」，
因此没有一条够格进 P1；把它们塞进 P1 才是 round90/91/92 那类「矩阵文本→P1 登记」同族病。

**P2 = 1（`login-flow/D3`）**：唯一真·待证据项——缺 s9 侧是否发追加段的协议事实，
拿到 s9 抓包即可升 P1，拿到前不发明解码分支。

**P3 = 16**：全部为「不改即正确」的现状保持项，登记在册防止后续被重复翻案。

## 12. round95：D3 命名纠偏 + SP 表分支 + P3 16 条转重做

### 12.1 `login-flow/D3` 不是「骑士团」，是 Evan / 龙神系

- 083 `GW_CharacterStat::Decode @0x4e2a84` 判据逐字：`v17 = _ZtlSecureFuse<short>(a1: this + 57, a2: v24);`
  `if ( v17 / 100 == 22 || v17 == 2001 )` → `JOB/100 == 22` 是 **2200–2299**，即 Evan（龙神）系；
  骑士团 Cygnus 是 1000–1599（`NOBLESSE 1000` / `1100` / `1200` / `1300` / `1400` / `1500` 支）。
  旧文档「骑士团条件解码」命名错误，本轮纠偏。
- s9 `GameConstants.hasSPTable(Job)` 枚举同一集合：`EVAN(2001)` + `EVAN1..EVAN10(2200,2210..2218)`，
  字节形态 `writeByte(effectiveLength)` + 每项 `writeByte(i + 1) + writeByte(remainingSp[i])`。
- 结论：**s9 确实发追加段**，D3 采纳项 (a) 的前提成立 → 客户端缺分支 = 真缺陷（L 桶，零 wire）。

### 12.2 本轮补丁

`Net/Handlers/Helpers/LoginParser.cpp` 的 `parse_stats`：`job` 改存局部变量，SP 段按
`has_sp_table(job)` 分支读（Evan 系读 `1 + 2*count`、SP 槽置 0；其余 `read_short()`）。
零 wire / 零协议 / 零 WZ / 不碰 6 个改造字段。

### 12.3 P3 16 条转「按 083 重做」

round94 把 P3 16 条判为「保持现状」，用户 m12009 推翻该判定 → 转为
`P3-ANGEL-REBUILD-PLAN.md` 的 A/B/C 三批重做，四证齐全才实施。算术不变。

## 13. round96：P3 A 批前 3 条（D12 / D11 / D10）

用户 m12009 把 P3 16 条从「保持现状」改为「按 083 全部重做」。本轮先做 A 批前 3 条。

### 13.1 `login-flow/D12` 登录 stats 段逐字节核对（`DOC_ONLY`）

新增 `docs/features/angel-calibration-d-matrix-2026-11/BYTE-LEVEL-STATS.md`：26 个字段的
s9 写方 / 当前端读方 / 083 `Decode` 三维对照。结论：逐字段宽度一致；与 083 的 3 处差异
（`level 1B→2B`、`exp 8B→4B + tail4`、`gachaexp`）全部是 s9 有意改造（W 桶），保持不动。

### 13.2 `skill/D11` BT_HYPER / GUILDSKILL / RIDE（083 不存在该三钮）

- `UIWindow.img/Skill` 直接子节点（orange-wz 128B 实测）：`backgrnd / skill0 / skill1 / line /
  BtSpUp / Tab / tip0..tip2 / CoolTime / recommend` —— **无 `main`**，故 `main/BtHyper` 等不存在。
- 083 `CUISkill::OnCreate @0x8aaa88` 创建的按钮：`id 2010..2013`（x=119/159/199/239，y=131，
  步进 40）、`id 2023`（x=120, y=265）、页签 `2000/2001`。
- 全库 `*Hyper*` / `*GuildSkill*` 符号命中 0。
- 判定：三钮是**非 083 扩展**。按 m09729「扩展功能可以不对齐」保留为显式占位 + DISABLED，
  不删枚举（`send_key` switch 与索引耦合），不发明 083 行为。代码侧为纯注释。

### 13.3 `corewindow/D10` `CWorldMapDlg` 尺寸（083 硬编码 666×524 已坐实）

- 083 `CDialog::CreateDlg(this, 67, 0, 666, 524, 10, 1, 0)`（`CreateWorldMapDlg @0x9eb366:139`）
  → 外层 **666×524**；`OnCreate @0x9eb75b` 里 `sub_9ECC4A(12, 34, 642, 472)` → 内容区
  `(12, 34) 642×472`；`WorldMap` 直接子节点只有 `title` + `Border{0..7}`。
- 当前端 `UIWorldMap` 由 `Border` 件尺寸推导：`content 640×470` + `top 33 / bot 18 / side 7`
  → 外框 **654×521**，内容左上 `(7, 33)`。
- 差 **12×3**（`666-654` / `524-521`），来源是 083 把外框尺寸硬编码（含 12px 内边距），
  本端按贴图件推导。两者 `Border` 件实测一致（`0=60×33 / 1=1×33 / 2=7×32 / 3=7×1 /
  4=7×1 / 5=7×18 / 6=1×18 / 7=7×18`）。
- 判定：**不做几何改动**（改外框会在贴图内侧留 5px 空隙，属视觉破坏；且 `DECISIONS.md:101`
  的 `BATCH_0_BASELINE` 理由是 s9 布局优先）。改为在代码侧写清 083 硬编码值与差值来源，
  真机对齐记 PLT §55-3。

## 14. round97：P3 A 批余 6 条（D1 / D8 / D2 / D4 / D1 / D3）

> 触发：用户 m12009「P3（16）按 Angel 全部重做」。承接 round96（A 批前 3 条），
> 本轮做 A 批**余 6 条**，逐条 083 活 IDA 取证 + s9 源码 + 当前端字面量 三向对拍。
> 映射表全文见 `P3-A-BATCH-REST-MAPS.md`。

### 14.1 六条逐条结论

- `map/D1`：083 `SaveFuncKeyMap @0x58df2f` = `COutPacket(135)` + `Encode4(mode=0)` +
  `89` 槽 × `5` 字节与 `this+449` 的 **445B**（`0x1BD`）影子表逐槽差分，条目体
  `sub_4E43FA @0x4e43fa` = `EncodeBuffer(5u)`。s9 `KeymapChangeHandler` mode0 同样是
  「count + (key:int, type:byte, action:int)」，`Character.keymap` 是**稀疏 map**。
  本端 `ChangeKeyMapPacket` 已 mode0 + 差分 + 5B 条目 → **`DOC_ONLY`**，零改动。
- `map/D8`：083 `SaveQuickslotKeyMap @0x72b87e` = `COutPacket(183)` + `EncodeBuffer(0x20)`（32B raw），
  默认表 `dword_BDAFAC`；s9 严格 `QUICKSLOT_SIZE * Integer.BYTES` = 32B，逐槽 `(byte)readInt`。
  本端 8 × `write_int` = 32B，默认值 `{0x2A,0x52,0x47,0x49,0x1D,0x53,0x4F,0x51}` 与 s9 逐项一致
  → **`DOC_ONLY`**，零改动（截断只在键值 ≥ 0x100 发生，v83 键值域不含）。
- `cashshop/D2`：083 `*CCSWnd*` 实测 **70 符号 / 7 个类**（Best 5 / Char 18 / Inventory 9 /
  List 18 / Locker 8 / Status 4 / Tab 7），`CCashShop` 本体 **67 方法**；本端 `UICashShop` 单类
  + `MAX_ITEMS = 7*2+1 = 15` → **架构差异**，不拆窗。
- `map/D4`：083 `CWorldMapDlg` 21 符号（`SetWorldMapDeeper @0x9ee30e` /
  `Shallower @0x9ee507` / `LoadMapList @0x9ed38b`），IMG `UIWindow.img/WorldMap` 只有
  `title` + `Border` 两节点、`WorldMapSearch` **不存在** → **资源缺失 + 架构差异**，不搬。
- `statusbar-hud/D1`：083 `CUIStatusBar` **45 方法实测**，按簇 `lifecycle 8 / input 7 /
  status 1 / quickslot 8 / chat 14 / misc 7`（`8+7+1+8+14+7 = 45` 现场减算）；本端 18 个 HUD 面类
  逐簇有归属、无孤儿方法 → **架构差异**，不合并。
- `social-economy/D3`：083 `*CUIAlliance*` 符号 **0**，`CTabGuildAlliance` **9 符号**，
  联盟入口是 `CUIUserList::CreateAllianceButton @0x91eba3` 的**按钮**而非独立窗；
  本端 `UIAlliance` 浮窗 + `UIUserList` UNION 页签（round85 `4b2452c4` 已收敛）
  → **保持现状**，不删不合并。

### 14.2 本轮口径

- 六条**全部 `DOC_ONLY`**：零 wire / 零协议 / 零 WZ / 零源码行为改动。
- A 批 9 条（3 + 6）至此完成；B 批 5 条（`skill/D9`、`social-economy/D12`、`pet-summon/D2`、
  `login-flow/D13`、`social-economy/D9`）与 C 批 2 条（`corewindow/D2`、`quest/D5`）待续。
- 算术不动：`168 = 76+38+14+40` / `reopen 27 = 已注销 10 + 残余 17` /
  `残余 17 = P1 0 + P2 1 + P3 16` 三条守恒式本轮全部未触及。
## 15. round98 · P3 B 批 5 条（三分 / DOC_ONLY / 本地类型扩展）

对 `skill/D9` / `social-economy/D12` / `pet-summon/D2` / `login-flow/D13` / `social-economy/D9` 做 083 符号+反编译、s9 对端、当前端字面量、DECISIONS 四项对拍，结论逐条：

- **`skill/D9` = KEEP**：083 `CUISkill::OnCreate @0x8aaa88` 全程单一布局（4 钮 2010..2013@y131 + 2023@(120,265)），`UIWindow.img/Skill` 11 直接子节点**无 `main`** → 本端 `classic` 恒 true，走的就是 083 经典路径。双布局是资源可用性降级（E13 环境适配），非缺陷。
- **`social-economy/D12` = P2**：`wedding.*` 实为 **27 键**（非 26），zh-CN 值也是英文；083 `CField_Wedding::OnWeddingProgress @0x58153d` 只有 4 组 SP_（`SP_4202/4203/4204` 教堂/大教堂/祝福问句 + `SP_1666` NPC 图），**非逐键对应** → 要换须先逐键对照 `String.img`，本轮不出键值。
- **`pet-summon/D2` = LOCAL_TYPE**：083 `CPet::Init @0x703314` 用 `DecodeBuffer(this+160, 8u)` 吃 8B uid，s9 `addPetInfo` 用 `writeLong` —— **wire 宽度相同无分叉**；丢的是本端 `PetLook.uniqueid` / `add_pet` / `:526` cast 三处 int32 收窄，而背包侧 `Character/Inventory/Pet.h:31` 已是 `int64_t`。发包 `MovePetPacket`/`PetCommandPacket` 用 int32 与 s9 `MovePetHandler.readInt` 一致，**绝不能改**。另两处 `(void)` 丢弃对应 083 根本不存在的 s9 扩展字段（nameTag/balloon、summon stance/movetype/attacking），按 m09729「扩展功能可以不对齐」保持。
- **`login-flow/D13` = DOC_ONLY**：s9 `getCharList` 按 `chars.size()` 循环、槽数只是尾部 int32（`getCharacterSlots`）；083 `SendSelectCharPacket @0x5f726d` 只有 19/29/30 三形态与容量无关；本端 `LoginHandlers.cpp:290-330` 按 `charcount` 循环且 `UICharSelect.cpp` 无 15 槽常量 → 「15 槽」是账号容量展示值，不是包体定长，无需改。
- **`social-economy/D9` = 三分**：`FriendGroup`（083 仅 3 符号、`OnOK` 90 行无 `COutPacket`、IMG `matches: []`）→ **维持不做**；`guildmark`（`SendSetGuildMarkMsg @0x530f2f` 17 行 = `126/15` + 7B 定长、IMG 真节点 `UserList/Guild/GuildInfo/guildmark`、本端**无出站封装**）→ 从「不做」改判「有节点无发包」登记 P3；`MapleTV`（`UIMapleTV.h` 类已实现、IMG 顶层 `MapleTV` LIST、`Data/UI/MapleTV.img` 在）→ 从「整族缺失」改判「壳层已对齐」，播放栈属 F 维。

**算术不动**：168 / 38+130 / reopen 27 = 10+17 / 17 = P1 0 + P2 1 + P3 16 全部零改；本轮只补判定层与事实层。

**零改动**：0 wire / 0 协议 / 0 WZ / 0 源码行为改动。
