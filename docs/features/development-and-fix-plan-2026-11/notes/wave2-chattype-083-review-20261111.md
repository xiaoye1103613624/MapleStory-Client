# Wave 2 · 2-24 · D7 聊天三态按 Angel 083 只读取证复核（round69 / 2026-11-11）

## 0. 性质与授权
- 目标：以 Angel(13339) 083/v83 带符号 IDB 为权威，逐功能校验当前端 UI/动画/交互。
- 用户 m09859 硬指令：「若碰到ui布局、动画一定要确认是否修复了，如果不是按照Angel 布局修复过的，可以重新修复；之前有很多参照BeiDou或者自行修复的ui、动画都有些问题；」
- 用户 m10642：「记录一次有可见 PUA 输出的交付，不记录评分。」（feedback 无 rating）
- m04292 授权：改源码 / 自测 / 测试完成提交 / 遇占用直关 / 不能自测记 backlog。
- **本轮定性**：D7 属「交互状态机语义整体套错到 083 名下」（第 5 例同族病，但错法≠前四例的虚构资源根）。校准 README D7 决策点原记「先五屏实机截图核对再定」，属决策点；本轮只做 REA→IDA→orange-wz→.nx 只读取证 + 对照清单 + 决策点 + 一次性 Plan + 零风险注释口径纠正，**不改行为**。

## 1. 083 权威语义（逐字，`tools/_ida_chattype/*.dec.txt`）
- `?SetChatType@CUIStatusBar@@QAEXJ@Z @0x8d4a6d (0x2d6)`：
  - `if (nType == *(this+832)) return;` → `*v4=nType; *(CConfig+21)=nType;`（持久化到 CConfig+21）。
  - nType==1 @0x8d4c50：`h=24; rows=1; top(509-h)=513; MakeCtrlEdit(a2:0)` 无输入框；滚动条 id=1010 / a7:513 / a8:h+1。
  - nType==2 @0x8d4bac：`h=24; rows=1; top=486; MakeCtrlEdit(a2:1)` 有输入框；滚动条 a7:510-h / a8:h+1。
  - nType==3 @0x8d4aab：`v6=*(CConfig+23); if(v6<26||v6>507) h=70 else h=v6;`（**越界回落默认 70，非 clamp**）；`rows=h/13; if(h%13==0) h+=2; top=509-h; MakeCtrlEdit(a2:1)`；滚动条 id=1010 / a7:510-h / a8:h-2。
- `?ChangeChatWndSize@CUIStatusBar@@QAEXJJJ@Z @0x8d616c (0x22f)`：`if(*(this+749)==0) return;`；a4==1 时 h>=507 早退 / a4==0 时 h<26 早退；`v9=h±13`；`if(v9>=507) h=507; elif(v9<=26) h=26;`；`rows=h/13; if(h%13==0) h+=2`（**在 clamp 之后执行 → 507→509 / 26→28，越过名义上下限**）；`top=509-h`；重建滚动条 id=1010；`*(CConfig+23)=h` 写回。
- `?OnButtonClicked@CUIStatusBar@@UAEXI@Z @0x8d423d`：`case 0x3EBu(1003) → SetChatType(1)` @0x8d42bd、`case 0x3ECu(1004) → SetChatType(3)` @0x8d42c3；**全函数无 nType:2 按钮入口**。
- xrefs SetChatType 7 点：`0x8d3a87`(OnCreate 从 CConfig 恢复) / `0x8d42c3`(按钮) / `0x8d5a6d`(StartChat `if(nType==1)→2`) / `0x8d5aba`(EndChat `if(nType==2)→1`) / `0xa04161`(OnGameStageChanged 切图 `if(nType==2)→1`) / `0xa07905`+`0xa0790e`(UseFuncKeyMapped case16 `3→1` else `→3`)。
- **083 三态真值**：1 收起(h24/无输入框/509-h=513) / 2 输入中(h24/1 行历史+输入框/509-h=486，StartChat 焦点驱动、EndChat 清回、切图清回，**不持久化、非按钮触发**) / 3 完整(h=CConfig+23 在 26..507 内可拖高)。`26..507` 只在 nType3 拖拽有意义；按钮只有 1003→1 / 1004→3；功能键 case16 3↔1；**无「逐级 3→2→1」、无「紧凑态固定高度」**。

## 2. 外部程序 `d6bdce97` D7 三处语义错
- `IO/UITypes/UIChatBar.h:320-327`：注释「D7：钳制范围对齐原生 SetChatType（nType 1/2/3 拖高 26..507，行高 13）」；`CLASSIC_LOG_H_MIN=26` / `MAX=507`（由 40/420 改）/ **`CLASSIC_LOG_H_COMPACT=120`（083 无此值）**；`:352 int8_t chat_type = 3;`（1收起/2紧凑/3完整）。
- `IO/UITypes/UIChatBar.cpp:85-95` 按 `Setting<ChatType>::get().load()` 恢复 1/2/3（2 时固定 120 高）；`:657/:675` BtMax/BtMin 逐级 3→2→1；`:1808-1831` `set_classic_log()` + `Setting<ChatType>::get().save(chat_type)` 持久化；`Configuration.h:660-666` `struct ChatType : ShortEntry("chat.type","3")`。
- **资源根用对了**：`:196-206` `sbroot=UiAssetResolve::status_bar_root()` / `basic=nl::nx::UI["Basic.img"]` / `try_maple_btn(..., basic["BtMax"]/["BtMin"], ...)` 同位互斥。→ D7 不是「找错节点自造兜底」型（区别于 round66/67/68 第三/四例），而是**状态机语义整体套壳**。
- 校准 README `docs/features/statusbar-hud-angel-calibration-2026-11/README.md`：`:124`(C2)/`:125`(C3)/`:107`(B5)/`:114`(B14)/`:165`(E5)/`:233-235`(§5)/`:288`(**D7 决策点「先五屏实机截图核对三态可见差异，有差异再定」**)/`:311`(外部程序写「✅ 已实施」但无逐字依据)。

## 3. orange-wz 门禁 + .nx 翻案（合规走完）
- `tools/_ct_probe.py`（endpoint `http://127.0.0.1:10012/mcp`，IMG `E:/MXD/BeiDou-Client-xy/Data/UI/StatusBar.img`）：`KEY b64_len=87384 raw_len=65536 sha256_16=268a8407a47ee76c iv=TSPHKw==` / `INIT ok=True` / `LOAD_FILES {"loadedCount":1,"rootCount":1,"exclusive":false,"generation":1}`（结构层 WORKS）；7 个 query（`chat`/`chat/ingame`/`chat/ingame/view`/`chat/ingame/view/max`/`BtMax`/`BtMin`/`EquipKey`）**全部 `IMG 解析失败: StatusBar.img`**（GBK 乱码原文留证）；`UNLOAD {"rootCount":0,"ok":true}` → AGENTS.md 合规 `.nx` 回退条件满足。
- `.nx`（`E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx` 86919577B）：`StatusBar.img` 顶层 19 节点；`StatusBar.img/chat` **NOT FOUND**、`StatusBar.img/BtMax|BtMin` **NOT FOUND**；`Basic.img` 顶层真有 `BtMax/BtMax2/BtMin/BtMin2/VScr..VScr8`。→ 当前端「本包无 chat/ingame，classic 必走 Basic.img」是**正确判断**，坐实 D7 属语义套壳而非资源错误。

## 4. 本轮最小修复（零风险，只纠注释）
- `IO/UITypes/UIChatBar.h`（CRLF 26235→26928B）：`:321-327` 改「083 SetChatType @0x8d4a6d：26..507 只在 nType3…nType1/2 的 h 恒 24…从 CConfig 恢复时越界回落默认 70（本端 DEFAULT=80 属 s9 适配）」+「083 没有『紧凑态固定高度』；此 120 是本端 s9 适配自造…行为重修属决策点 D7-A」；`:349-351` 改「本端 s9 适配三态（非 083 原生 SetChatType）…nType2 输入焦点驱动、不持久…按钮只有 1003→1/1004→3，无逐级」。
- `IO/UITypes/UIChatBar.cpp`（CRLF 103639→103770B）：`:657` 展开侧 / `:675` 收起侧 / `:1817-1818` set_classic_log 三处 D7 注释改「本端…（s9 适配）…083 无逐级，见 notes D7-A」。
- `Configuration.h`（LF 41107→41243B）：ChatType 注释改「非 083 原生 SetChatType」+ 引 notes 路径。
- **`git diff --stat` = `3 files changed, 17 insertions(+), 9 deletions(-)`；`git diff --check` = `DIFFCHECK_CLEAN`**。行为字面量（chat_type 赋值 / CLASSIC_LOG_H_* 常量 / button_pressed 分支 / Setting key）逐字不动，harness A4 逐条守卫「仍在」。

## 5. 决策点 D7-A + 一次性 Plan（交用户裁决，本轮不实施）
- **决策点 D7-A**：是否把聊天三态行为整体改回 083 `SetChatType` 状态机？
  - 影响面：删紧凑态 → nType2 改「输入焦点瞬态、不持久化」→ 去 BtMin/BtMax 逐级 → `chat.type` 持久化键语义 → 滚动条 id 1010 体系 → `MakeCtrlEdit(0/1)` 输入行有无 → 五屏实机截图核对。
  - 不动的理由：校准 README D7 决策点明写「先五屏实机截图核对三态可见差异，有差异再定」；用户 m09859 授权边界是布局/动画重修，不是交互逻辑连根拔；s9 联机协议优先，改状态机属行为变更。
- **一次性 Plan（裁决通过后执行，不在本轮）**：① 五屏实机截图（收起/输入中/完整最小/完整默认/完整拖高）→ ② 按 083 三态重写 `set_classic_log` / BtMin / BtMax / 焦点链 → ③ `ChangeChatWndSize` clamp+`h%13==0→+2` 边界单测 → ④ 去掉 `CLASSIC_LOG_H_COMPACT` 与 `chat.type` 的 2 态 → ⑤ 真机回归 s9 六频道聊天 + 拖拽 → ⑥ 独立原子提交。

## 6. 真机 backlog（pending-live-tests §35）
- 35-1 五屏截图（收起/输入中/完整最小/完整默认/完整拖高）核对三态可见差异 → D7-A 前置。
- 35-2 本端紧凑态 120 高 vs 083 输入中态 1 行 + 输入框的观感差异。
- 35-3 拖拽到上下界（26/507）时 083 的 `h%13==0→+2` 越界到 28/509 的视觉表现。
- 35-4 Basic.img BtMax/BtMin 同位互斥与 083 1003/1004 两按钮的键位对应。
- 35-5 D2 聊天规则（2000ms 限流/连续4同句/脏词/s9 !mute）真机回归（服务端已双改，待联机）。

## 7. 提交 scope（精确 git add 8 文件，与提交清单逐字一致）
- `IO/UITypes/UIChatBar.h` / `IO/UITypes/UIChatBar.cpp` / `Configuration.h`（3 源码，纯注释）
- `tools/test_round69_chattype_083.py`（harness）
- `docs/features/development-and-fix-plan-2026-11/evidence/chattype-083-round69.json`（证据汇总）
- `docs/features/development-and-fix-plan-2026-11/notes/wave2-chattype-083-review-20261111.md`（本文）
- `docs/features/development-and-fix-plan-2026-11/README.md`（§4 2-24）
- `docs/features/pending-live-tests/README.md`（§35）
- 一次性脚本（`tools/_ct_*.py`）/ IDA 反编译 `tools/_ida_chattype/*` / 门禁日志 仅本地证据链留存，**不进版本库**；evidence JSON 已汇总路径可追溯。

## 8. 教训（沉淀）
1. 同族病不止「虚构资源根」一种形态：本次「资源根正确、状态机语义套壳」更隐蔽，必须先反编译权威再判性质，别看到「本包无资源」就归类。
2. 反编译边界怪异点要逐字建模型：`h%13==0→+2` 在 clamp **之后**，507→509、26→28 会越过名义上下限；harness 若写「想当然的 clamp 稳定值」就是假绿。
3. 决策点不能自作主张连根拔：D7 校准 README 明写「先五屏实机截图」，用户授权是布局/动画，行为状态机改回须交裁决；本轮只纠错误依据注释，行为字面量用 harness 守卫「仍在」防误改。
4. 注释也是证据链：把「原生 SetChatType」这种错误依据留在代码里，下一次校准会继续被带偏；纠错注释 + 留决策点路径，才是端到端闭环。
