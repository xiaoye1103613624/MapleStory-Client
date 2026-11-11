# round91 · 下一批一次性完整 Plan（计划轮 · 零源码改动）

> 触发：User said (m10739)「开始 下一批 的计划」；User said (m10738)「记录一次有可见 PUA 输出的交付，不记录评分。」
> 门禁 1：先证据 → 分析 → 抉择 → 一次性 Plan；**用户确认计划后才进入实现**。本轮不落任何源码补丁。
> 权威源：Angel.exe(13339) 083/v83 带符号 IDB；s9 源码只约束 W 桶字段宽度；本端 HeavenClient 自研重实现。

## 0. 结论先行（别把计划当交付）

**WLE-TRIAGE-PLAN.md §5/:123 与 §7/:152-153 登记的「下一批 L 桶 3 条」三条全部不成立，不进 P1。** 这是继 round90 `drop/D2`/`drop/D12` 之后**第三次**「纸面登记 → 现场取证证伪」的同族病。本轮把三条的桶属性用 083 活 IDA + s9 源码 + 当前端代码 + DECISIONS 裁定四方钉死，并给出真正干净的 L 桶候选（每条自带 083 反编译锚点）。

| 候选 | round89/90 登记 | round91 判定 | 是否进 P1 |
| --- | --- | --- | --- |
| `quest/D8` | L 桶 P1（搬 `sub_721008` 分支） | **W 桶**：107 包条件追加 4B x/y，wire 布局 | ❌ 保持 s9 |
| `skill/D4` | L 桶 P1（新增 `CWndSkillGuide` 子窗） | **架构级**单点裁决：是否新增整窗族 | ❌ 不新增 |
| `corewindow/D8` | L 桶 P1（覆盖度对照） | **DOC_ONLY 已发货**（round75 `c07b1a7b`） | ❌ 只补证据 |

## 1. 本轮证据链（门禁 18 全走完）

| 层 | 证据 | 落盘 |
| --- | --- | --- |
| REA / IDA | 083 活 IDB `http://127.0.0.1:13339/mcp`，12 份反编译 | `tools/_ida_r91/*.code.txt` |
| s9 源码 | `QuestActionHandler.java` / `DistributeSPHandler.java` / `AutoAssignHandler.java` / `KeymapChangeHandler.java` / `QuickslotKeyMappedModifiedHandler.java` | 逐字引于 §2 |
| 当前端 | `Net/Packets/QuestPackets.h`、`UISkillBook.cpp`、`UIStatsInfo.cpp`、`UIKeyConfig.cpp` | 逐字引于 §2 |
| 历史裁定 | `DECISIONS.md:99/190/251/293/311`、矩阵 `:152/:243/:304` | 逐字引于 §2 |
| orange-wz IMG | 128B `WzAESConstant.DEFAULT_KEY` base64（172 字符）+ iv `TSPHKw==`，`UIWindow.img` 的 `KeyConfig`/`Stat`/`Skill` 三根全可达、`isError=false` | `tools/_ida_r91/img_gate.json` → `GATE_OK_128B_IMG_REACHABLE` |
| .nx | 本轮判定不依赖资源布局（见下），无需回退 | — |

**IMG 门禁结论**：`GATE_OK_128B_IMG_REACHABLE`（KeyConfig len=548 / Stat len=2138 / Skill len=4242，全 `isError=false`）。三条判定靠**反编译 + s9 源码 + 当前端代码 + 裁定**，均不依赖 IMG 资源布局；本次仅按门禁 18 验证 128B 通道可达且三根存在。**注意口径**：orange-wz MCP 路径用 128B `DEFAULT_KEY`，`.nx`/nlnx/`tools/_wz.py` 才用 65536B `key_gms`（AGENTS.md:23 双规格）。

## 2. 三条候选四方对拍（逐条）

### 2.1 `quest/D8` — `sub_721008` 是 wire 条件，不是纯逻辑

**083 反编译逐字**（`tools/_ida_r91/sub_721008.code.txt`，`0x721008`）：

```c
  if ( CQuestMan::IsAutoStartQuest(this, a2) ) /*0x72100f*/
  return 1; /*0x721016*/
  result = sub_720FED(a1: a2); /*0x72101e*/
  return 1; /*0x721029*/
```

**083 发包链逐字**（`tools/_ida_r91/CUIQuestInfo_StartQuest_action.code.txt`，`CQuest::StartQuest @0x71751c`，action=1 start / action=2 complete）：

```c
  COutPacket::COutPacket(a1: 107); /*0x717087*/
  COutPacket::Encode2(a1: v50); /*0x7170b9*/
  COutPacket::Encode2(a1: v51); /*0x7170c4*/
  CClientSocket::SendPacket(this: dword_BE7914, a2: &v52); /*0x7170d3*/
  COutPacket::COutPacket(a1: 107); /*0x717104*/
  COutPacket::Encode2(a1: v50); /*0x717136*/
  COutPacket::Encode2(a1: v51); /*0x717141*/
  CClientSocket::SendPacket(this: dword_BE7914, a2: &v52); /*0x717150*/
  COutPacket::COutPacket(a1: 107); /*0x7174db*/
  if ( sub_721008(a1: *(this + 6)) == 0 ) /*0x71751c*/
  COutPacket::Encode2(a1: v50); /*0x717524*/
  COutPacket::Encode2(a1: v51); /*0x71752f*/
  CClientSocket::SendPacket(this: dword_BE7914, a2: &v49); /*0x71753e*/
  COutPacket::COutPacket(a1: 107); /*0x71745c*/
  if ( sub_721008(a1: *(this + 6)) == 0 ) /*0x71749d*/
  COutPacket::Encode2(a1: v50); /*0x7174a5*/
  COutPacket::Encode2(a1: v51); /*0x7174b0*/
  CClientSocket::SendPacket(this: dword_BE7914, a2: &v52); /*0x7174c8*/
```

即：`COutPacket(107)` → `Encode1(action)` → `Encode2(questid)` → `Encode4(npcid)` → **`if (sub_721008(questid)==0) { Encode2(x); Encode2(y); }`** → `SendPacket`。`sub_721008` 语义 = `IsAutoStartQuest(questid) || sub_720FED(questid)`（旧「函数名无符号语义待补 L2」本轮补齐）；自动开始/脚本链接任务**不追加 x/y**，普通任务追加 4B。

**s9 逐字**（`QuestActionHandler.java:48-64`）：

```java
private static boolean isNpcNearby(InPacket p, Character player, Quest quest, int npcId) {
    Point playerP; Point pos = player.getPosition();
    if (p.available() >= 4) {
        playerP = new Point(p.readShort(), p.readShort());
        if (playerP.distance(pos) > 1000) { playerP = pos; }
    } else { playerP = pos; }
    ...
}
```

s9 **同时容忍有/无 x/y 两种包体**（`available()` 在 Java 侧是真实字节数）。当前端 `Net/Packets/QuestPackets.h` 布局 `byte action, short questid [, int npcid]`，从不写 x/y = 合法 s9 形态。

**裁定**：`quest/D8` 是 **W 桶 wire 条件追加段**，不是「同字段原始逻辑」。搬进来 = 改 c2s 107 包形 = 协议级改动，且 s9 已接受当前形态。矩阵 `:304`「不搬」+ `DECISIONS.md:251` `BATCH_1_KEEP` 正确。**不进 P1，保持 s9，零改动。**

### 2.2 `skill/D4` — 架构级「是否新增整窗族」，不是 L 桶

**083 反编译逐字**（`tools/_ida_r91/CWndSkillGuide_OnKey.code.txt`，`0x8aa4e4`）：

```c
  if ( a3 >= 0 && a2 == 27 && dword_BF1080 != 0 ) /*0x8aa4fa*/
  return CUISkill::CloseSkillGuide(this: dword_BF1080); /*0x8aa4fc*/
```

全库 `*CWndSkillGuide*` 仅 2 符号（ctor `0x8aa243` / `OnKey 0x8aa4e4`）。当前端 `IO/UITypes/UISkillBook.cpp:797` `send_key` escape→`set_skillpoint(false)`/`close()` 已闭环，`docs/features/skill-angel-calibration-2026-11/README.md:79` A4 已判 ✅ 一致。

**裁定**：这是「要不要新增一整个独立子窗族」的**架构级单点裁决**，不是「同字段原始逻辑还原」。`WLE-TRIAGE-PLAN.md:133` 自己写明「架构级重写…不在自动落地范围」。矩阵 `:243`「不新增」+ `DECISIONS.md:190` `BATCH_1_KEEP` 正确。**不进 P1；只有用户单点重开才评估。**

### 2.3 `corewindow/D8` — K3 DOC_ONLY，round75 已发货对照清单

083 三份反编译本轮全部拉齐落盘：

| 083 函数 | 地址 | 逐字要点 | 文件 |
| --- | --- | --- | --- |
| `CUIStat::OnButtonClicked` | `0x8c522f` | 10 case：6 掩码 `2048/0x2000/64/128/256/512`→`SendAbilityUpRequest`；2006 ToggleDetail；2007/8 AutoApUp(1)；2009 AutoApUp(0) | `tools/_ida_r91/CUIStat_OnButtonClicked.code.txt` |
| `CUIKeyConfig::OnButtonClicked` | `0x8335d0` | 5 分支：1=Save+UI_Close(5) / 2=InvalidateRect(StatusBar)+UI_Close(5) / 2000=DoModal→Default+ReDrawQuickslot / 2001=DoModal→memset(445)+InvalidateRect / 2002=sub_836C9B | `tools/_ida_r91/CUIKeyConfig_OnButtonClicked.code.txt` |
| `CUISkill::OnSkillLevelUpButton` | `0x8acf1a` | `CanSendExclRequest(500,0)` 独占门闩 + `SP_3509/3510/3511` 三档前置 | `tools/_ida_r91/CUISkill_OnSkillLevelUpButton.code.txt` |
| `CWvsContext::CanSendExclRequest` | `0x485bf7` | `&& get_update_time() - *(this + 2090) >= a2;` | `tools/_ida_r91/CanSendExclRequest.code.txt` |

**s9 对拍**：`DistributeSPHandler` 只 `readInt(); readInt(skillid)` → `AssignSPProcessor.SPAssignAction`，**无 500ms 限速**；`AutoAssignHandler` → `APAutoAssignAction` 走 s9 Stat 5-byte 编码；`KeymapChangeHandler` / `QuickslotKeyMappedModifiedHandler` 是 s9 私有形态。

**当前端对拍**：`Net/Packets/PlayerPackets.h:48-72` 注释已明写「本包 type 编码与 SPEND_AP 的 `MapleStat::codes` **不是同一套**（s9 Stat 5-byte 编码），勿复用」，`StatCode` 只 4 个（`STR=64/DEX=128/INT=256/LUK=512`）——083 的 `2048/0x2000` 是 HP/MP 系，s9 没有；`UISkillBook.cpp:545-560` BT_OKAY 无 500ms 门闩**正是 s9 正确实现**；`UIKeyConfig.cpp:731-810` 6 个 case 对齐 083 5 分支 + 本端适配。

**裁定**：矩阵 `:152` K3「先出逐条对照清单，**不改代码**」+ `DECISIONS.md:99/293/311`「round75 `c07b1a7b` 已复核，只出对照清单不改代码」= **DOC_ONLY 已发货**。本轮把 083 逐字对照补进 §2.3 表 + evidence（这正是原裁决要的产出），**零源码改动，不进 P1**。

## 3. 同族病模式（P8 揪头发：一类问题出去）

WLE-TRIAGE-PLAN.md 三处纸面登记被现场取证证伪：

- `:96`「`quest/D8`、`skill/D4`、`corewindow/D8` 等状态机分支同理，逐面 REA→IDA→IMG 后再落」
- `:123`「改走真正有 083 状态机证据且**零 wire** 的 L 桶候选」
- `:152-153`「**同族审计**：…均为状态机/条件分支、不碰 wire，属合法 L 桶候选，**无同族误分类**」

**根因**：从 D 矩阵「推荐默认文本」直接抄进 P1 登记层，没有先跑该条的 083 反编译 + s9 handler + 当前端字面量。round90 已在 `drop/D2`/`D12` 上犯过一次，round91 是**第三次**（§7 那句「无同族误分类」本身又埋了一次）。

**举一反三（写进 Plan 的硬规则）**：任何「下一批 L 桶」登记，必须先附 ① 083 符号 + 反编译文件路径与逐字关键行 ② s9 对应 handler 的 body ③ 当前端对应字面量 ④ DECISIONS 原裁定，四项齐了才准写进 §5。**不允许从矩阵文本直接推导批次。**

## 4. 下一批真正干净的 L 桶候选（每条自带 083 锚点）

| 优先 | 决策点 | 083 符号 / 反编译 | 为何干净 | 门槛 |
| --- | --- | --- | --- | --- |
| **P1 首选** | `skill/D12` / `corewindow/D9` 宏系统选中态 / ESC | `CUIMacroSysEx::OnSelected @0x8a984d`（20 行）、`CUIMacroSys::OnKey @0x8a98dc`（8 行） | 纯 UI 状态机：`*(this+361)=a2` → `GetMacroName` → `IsShoutMacro` → `SetChecked` → `ResetInfo`；ESC→`ProcessBasicUIKey(27,a2)`。**零 `COutPacket` / 零 `Encode` / 零 `SendPacket`** | REA→IDA→orange-wz(128B)→.nx 只读取证后，纯逻辑补丁，一片一原子 commit |
| **P1 次选** | `login-flow/D9` / `statusbar-hud/D12` 频道网格键盘导航 | `CUIChannelShift::OnKey @0x995863`（85 行）、`CUIChannelSelect::OnKey @0x628a1c`（135 行） | `CUIChannelShift` 是 6 列方向键纯索引计算（case `0x25/0x26/0x27/0x28`），零发包。**`CUIChannelSelect` 必须拆**：导航段=L，`SendViewAllCharacterPacket` 段=W（s9 优先保持） | **先拆 L/W 再动手**，只做导航 |
| **P2 / F 维** | 宏画布态 | `CUISkillEx::ShiftMacroUIState @0x8bad5a`（243 行） | COM `IWz` 画布 / `Animate` 栈，当前端无 COM 画布栈 ⇒ 与 round90 `drop/D2` 同族 F 维 | **不补**，登记 P2，禁想象修改 |

反编译全文：`tools/_ida_r91/CUIMacroSysEx_OnSelected.code.txt`、`CUIMacroSys_OnKey.code.txt`、`CUIChannelShift_OnKey.code.txt`、`CUIChannelSelect_OnKey.code.txt`、`CUISkillEx_ShiftMacroUIState.code.txt`。

## 5. 一次性执行 Plan（**待用户确认后**逐片实施）

| # | 批次 | 动作 | 验收信号 |
| --- | --- | --- | --- |
| 1 | 本轮（已落地） | 三条候选四方取证 + 桶属性纠偏 + 本一次性 Plan + evidence/notes/dev 2-39/PLT §50 + harness + GATE + build 门禁回归 + 零署名提交 | 计划轮**零源码改动**；harness 全绿；`GATE OK s9 a78ba681a4`；提交签名行 0 |
| 2 | 下一批 · 片 1 | `skill/D12`/`corewindow/D9` 宏选中态：先 REA→IDA→orange-wz(128B)→.nx 只读取证，再落纯逻辑补丁（零 wire） | 083 `CUIMacroSysEx::OnSelected` 逐字为凭；harness + GATE + 真重编三证（touch + build_log size/mtime + EXE mtime + 覆盖）+ 零署名原子提交 |
| 3 | 下一批 · 片 2 | `CUIChannelShift` 6 列导航纯索引计算；`CUIChannelSelect` 先拆 L/W，只落 L 段 | 同上；`SendViewAllCharacterPacket` 段零改动 |
| 4 | 逐面推进 | reopen 剩余 P1 按 12 面分批，**每批先补 §3 四项证据** | 每片一原子 commit；证据不足登记 P2/P3 不发明 |
| 5 | 全程 | W 桶 6 字段宽度/类型 + 27 条 wire KEEP **零改动**；`quest/D8`/`skill/D4`/`corewindow/D8` 零改动 | 不复现 `LoginParser.cpp:150-174` 白屏 |
| 6 | 真机归口 | 五屏截图 60 节/142 处、`InPacket::available()` 393 处 §27-A、D2 聊天规则 | 用户认领后销案 |

## 6. 明确不做（写死，防止半路扩张）

- ❌ 不改 `Net/Packets/QuestPackets.h` 给 107 包追加 x/y（W 桶 / 断联机风险）
- ❌ 不新增 `CWndSkillGuide` 独立子窗（架构级，需单点裁决）
- ❌ 不给 `UISkillBook.cpp` 补 500ms 门闩（s9 无限速，补了反而是错）
- ❌ 不把 083 的 `2048/0x2000` 掩码塞进 `AutoAssignApPacket::StatCode`（编码体系不同）
- ❌ 不在计划轮改任何 `.cpp` / `.h`（门禁 1：确认计划后才实现）
- ❌ 不写 commit hash / `git log` 进 evidence 或 Plan（round88 B9b 漂移教训）

## 7. 算术与限制

- 168 条 `DECISIONS.md` 历史裁定**不动**；WLE §4 `38 + 130 = 168` **不动**；reopen `27 = 25 P1 + 1 W + 1 F-P2`（round90）**不动**。本轮只在 **P1 登记层**注销 3 条，不做语义再分类造假（登记层 ≠ 裁定层）。
- 本轮 **0 源码行为改动 / 0 wire / 0 协议 / 0 WZ / 0 新封包**；`tools/_ida_r91/` 只读取证留痕。
- 外部程序在途 WIP 原样留工作树，精确 `git add` 隔离、零夹带。
- 真机五屏截图 60 节/142 处、`InPacket::available()` 393 处 §27-A 逐面回归、D2 聊天规则，仍由用户认领。
- 下一批实施前须用户确认本 Plan；确认后按 §5 逐片推进，每片一片一原子 commit。
