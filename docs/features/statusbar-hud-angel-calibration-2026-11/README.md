# 状态栏 HUD 面 · Angel 083/v83 带符号 IDB 全维校准（2026-11-08）

> 承接 `docs/features/login-flow-angel-calibration-2026-11/README.md` 的同一流水线：goal `goal-11388c42-2502-43f0-b722-70abeb8d4307` 第 2 个功能面。
> 权威校准源 = **Angel(13339) 083/v83 带符号 IDB**（`E:\MXD\反编译IDA\v83\`Angel.i64`，module `` `Angel.exe ``，imagebase `0x400000`）；对照端 = 当前 `E:\project\MapleStory-Client`（分支 `feature/dev_0.0.1`）。
> 基准固定 **083/v83**；**禁止引 095 线符号/IDB**（`Maplestory95.i64` / port-095 映射 / ExpeditionIntermediary 等）作依据。源码内现存一行 095 字样仅如实记录、不作校准依据。
> 全程**只读取证**：未改任何产品源码、未构建、未实机、未提交。按 `xiaoye-maple-story-dev` 门禁，交决策点 + 一次性 Plan 等用户裁决后才进入实施。

---

## 0. 一句话结论

1. **Angel 是同一份 v83 二进制的不同符号状态，不是版本更新**：HUD 面 548 个具名函数 EA 中 **547 个在 BeiDou(13337) 同 EA 存在**（542 `sub_*` 无名 / 5 具名），仅 `0x6cec47` 缺失（IDA 分析覆盖差异）→ 登录面 327/328 结论在 HUD 面同样成立（`evidence/angel-beidou-hud-ea-crosscheck.json`）。**上一轮用 BeiDou 无符号 IDB 校准的排版常量全部可复用，Angel 只补「符号语义层 + 行为层」**。
2. **不缺窗口文件，也不缺排版**：当前端 `IO/UITypes/UIStatusBar.cpp:1901-1907` 注释里记录的 IDA `sub_8D01B2` 11 个 `CreateCtrl` 坐标，与 Angel `CUIStatusBar::OnCreate @ 0x8d01b2` 逐字提取的 11 个坐标**逐点吻合**（`notes/r02` §0）。Angel 补上的是按钮 id 语义、`OnButtonClicked` 分支、聊天三态、脏词规则、IME 状态机等**行为层**。
3. **结构性差异（最值得校准）**：v83 原生状态栏是**单体窗 `CUIStatusBar` 41 方法**（聊天/快捷栏/HP·MP·EXP 条/菜单/密语/浮公告/按钮/工具提示全在一个类内），当前端 `IO/UIStateGame.cpp:78-99` 把它拆成 **10 个构造期 HUD 类 + 2 个按需类**（`notes/r04` §9 / `notes/r05` G1）。拆分是工程取舍，功能面等价，但带来**点击优先级、生命周期、配置落点**三处一致性代价。
4. **真缺口收敛为三类**（98 条判定里 ✅ 50 / ⚠️ 31 / ❌ 16 / ⊘ 1）：① **业务规则整族缺失**（脏词过滤 / 2000ms 刷屏限流 / 连续 4 条同句检测 / 禁言）；② **原生独有交互整段省略**（`SetChatType` 三态、`CUIPartyHP`、`SetButtonBlink` 通知链、`CCtrlMLEdit` IME 候选窗状态机）；③ **客户端自造 / 旧账误标**（`change_target()` 空 stub、按钮 id 注释误标、QS 几何常量另起一套、文档旧基线未回查源码）。
5. **s9 适配红线**：所有 083↔s9 差异（字段宽度、opcode 编号、包体拆法）一律标注「须裁决」，**不得盲改回原生**——改回会断当前联机（沿用登录面 D1 结论）。IMG 层（orange-wz MCP）**仍阻塞**，资源证据以仓库内 `.nx` 节点引用 + 正确端截图 + `StatusBar.img.xml` 替代。

---

## 1. 取证基线与门禁

### 1.1 端点与工具（本轮 `server_health` 实测）
- 13339 = `E:\MXD\反编译IDA\v83\`Angel.i64`，imagebase `0x400000`，`auto_analysis_ready/hexrays_ready/strings_cache_ready` 全 true，`strings_cache_size=1281`，uptime 66106s。
- 13337 = `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`，`strings_cache_size=3889`，uptime 714977s。两端各 66 工具（`decompile/lookup_funcs/list_funcs/xrefs_to/py_eval/analyze_batch/...`）。
- 13338 = headless `Maplestory95.i64`（本任务不用；旧记录已过时）。

### 1.2 三联门禁（REA → IDA → IMG）
| 层 | 状态 | 依据 |
|---|---|---|
| REA | 本会话未注册 REA MCP；沿用 CLI 路径 `D:\software\nodejs-24\rea.cmd` + `REA_ANALYSIS_PROVIDER=ida` 挂 13337（历史证据 `notes/evidence/r17/rea_decompile_DualMobGauge_56C9B7.txt`） | `notes/r03` §5 |
| IDA | ✅ 已直连 13339/13337，批量反编译 + `py_eval` 索引 + EA 交叉验证 | 本轮全部 evidence |
| IMG | ⚠️ **阻塞**：`mcp_connector_status` 仅 `json-photoshop`；`C:\Users\11036\.cursor\mcp.json:3-6` 有 `orange-wz` 但 DSH 未注册；`docs/features/login-flow-regression-fix/notes/img-evidence-audit.md:7` 记「orange-wz MCP 对这批私服改版 img 内部节点有已知解析缺陷，未使用」 | `notes/r02` §6 / `notes/r03` §7 |

**IMG 替代证据链**：`docs/features/v83-hud-quest/evidence/StatusBar.img.xml:1-519`（完整节点表）、`tools/nx_inspect.py` 运行时 dump、`IO/UITypes/UIStatusBar.cpp:1908-1921` 实测属性、`docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/进入游戏页.png`（1280×720）、`docs/features/ui-layout-v83-unify/evidence/*.png` 14 张 v83 底栏真件裁图、Angel 316 个 `SP_*` StringPool 节点索引（`evidence/angel-hud-facts.md` §1）。

### 1.3 本轮产物（6 份 evidence + 5 份 notes + 3 个采集脚本）
- `evidence/angel-hud-ea.json` — **548 具名函数 / 47 类**（`tools/ida_angel_hud_ea.py`）
- `evidence/angel-hud-decompile.json` / `angel-hud-decompile-full.json` — **247 函数逐字 Hex-Rays**，其中 10 个超大函数经 `/output/<uuid>.json` 端点补全后归一化（值 237 个是嵌套 JSON 字符串，需二次 `json.loads().get("code")`）
- `evidence/angel-hud-facts.md` — 693 行：316 个 `SP_*` 节点 / CUIStatusBar 逐函数关键行 / 坐标候选行（`tools/ida_angel_hud_facts.py`）
- `evidence/angel-statusbar-oncreate-full.json`（`OnCreate` 126188 B 全文）
- `evidence/angel-beidou-hud-ea-crosscheck.json` — 547/548 同 EA
- `notes/r01-angel-native-statusbar-facts.md`（Angel 一手语义 10 节）
- `notes/r02-hud-img-style-evidence.md`（IMG/样式 7 节）
- `notes/r03-prior-hud-doc-audit-and-cancel.md`（既有文档四态 + 主 agent 销案块 7 节）
- `notes/r04-current-hud-source-inventory.md`（当前端源码盘点 9 节）
- `notes/r05-increment-hud-source-details.md`（增量：`send_key` 分支 / Buttons 枚举数值 / UIElement Type 数值 / `[DBG-CLK]` 销案）
- 采集脚本：`tools/ida_angel_hud_ea.py`、`ida_angel_hud_decompile.py`、`ida_angel_hud_facts.py`（模板复用 `tools/ida_angel_login_*.py`；`py_eval` 用 `new_locals=True` + `def _scan(): ...` 规避 exec/eval 作用域 bug；截断补全/归一化复用同一 MCP 客户端内联逻辑）

### 1.4 证据优先级（与登录面一致）
`协议/IDA 一手逐字伪代码 > 当前端源码逐字（最优先销案） > 2026-11 现场重算 > 正确端实机截图 > Wave 销案记录 > 2026-10-21 基线矩阵 > r15/r16/r19 历史账`。
**注意**：`docs/features/feature-gap-inventory-2026-11/README.md` 若干条目标 ❌ 是 2026-10 旧账，未经源码回查（详见 §5.4），本轮一律以源码逐字为准。

---

## 2. 口径与判定定义

### 2.1 六维 × 三判定
| 维度 | 覆盖 | | 判定 | 含义 |
|---|---|---|---|---|
| A 功能 | 窗口/类/按钮/handler/包是否齐全 | | ✅ | 当前端与 Angel 083 语义一致或功能等价 |
| B 业务 | 脏词/刷屏/禁言/密语/组队血条/警报/频道 | | ⚠️ | 一致性差异 / 形态不同 / 须裁决 |
| C 逻辑 | 状态转移/时序/门闩/补间/IME 状态机 | | ❌ | 当前端真缺失（083 有、当前端无） |
| D UI | 窗口/控件/节点引用/单体 vs 拆分 | | ⊘ | s9 适配 或 已裁决「明确不做」 |
| E 排版 | 坐标/尺寸/网格/快捷栏几何/字库 | | ⏳ | 待实机截图/IMG 复核 |
| F 样式 | 字体/颜色/九切片/字库/位图节点 | | | |
| G 交互 | 点击/拖拽/滚动/焦点/快捷键/IME | | | |

### 2.2 083 vs s9 红线
Angel 是 GMS v83 原生；当前端联机对象是 s9 服务端。**编号对齐但字节/字段布局不同处，一律不视为「当前端错」**，标注「s9 适配，改回原生会断联机，须交用户裁决」。例：登录面 `GW_CharacterStat::Decode` level 1B vs 当前端 2B（999 级服务端改造）、VAC opcode 15 vs s9 13/14/32；HUD 面 QS 几何、聊天窗尺寸常量。

### 2.3 观察 / 推断 / 未知
所有条目带 `[观察]`（源码/伪代码直读）或 `[推断]`（据直读推导）。HUD 面**未知**：s9 是否下发组队血条数据、s9 是否已在服务端做脏词/刷屏过滤、原生 `sub_A12522`/`sub_A06CBC` 两个未命名分支的具体语义、1008 按钮为何无分支。

---

## 3. 六维对照总表（98 条）

> 每条格式：`维度编号 | Angel 083 证据（EA / 伪代码） | 当前端证据（相对路径:行号） | 判定 | 说明`。
> 汇总：**✅ 50 / ⚠️ 31 / ❌ 16 / ⊘ 1**（其中 12 条 ⚠️ 属 s9 适配须裁决、4 条可零风险先改、6 条历史账销案）。

### 3.1 A · 功能（14 条）

| # | Angel 083 证据 | 当前端证据 | 判定 | 说明 |
|---|---|---|---|---|
| A1 | `CUIStatusBar` 单体 41 方法（`evidence/angel-hud-ea.json`） | `IO/UITypes/UIStatusBar.h:35-37` + 10 次独立 `emplace`（`IO/UIStateGame.cpp:78-99`） | ⚠️ | **形态差异**：原生单体 → 当前端 10+2 类拆分；功能面等价，代价见 A2/C7/D1/E7 |
| A2 | 聊天在 `CUIStatusBar` 内：`SetChatType 0x8d4a6d`/`StartChat 0x8d5a4d`/`EndChat 0x8d5aaf`/`ChatLogAdd 0x8db070`/`ChatLogDraw 0x8dbb93`/`ChangeChatWndSize 0x8d616c`/`MakeCtrlEdit 0x8dfb36` | `IO/UITypes/UIChatBar.h:39-41` 独立类（2673 行 .cpp） | ⚠️ | 聊天彻底独立；靠 `IO/UIStateGame.cpp:139-152` elementorder 重排协调点击优先级 |
| A3 | 快捷栏 `GetShortCutIndexByPos 0x8de8d5`/`ReDrawQuickslot 0x8d82a7`/`ToggleQuickSlot 0x8df4b1`/`SetShortcutTooltip 0x8e03f1` | `IO/UITypes/UIStatusBar.cpp:2162-2231` `classic_qs_slot_at`/`activate_classic_qs_slot` | ✅ | 命中→`Stage::get().send_key(...)` 已接通（r15/r16 标 ❌ 已销案） |
| A4 | HP/MP/EXP `SetStatusValue 0x8d82b8` + `Draw 0x8d80a4`→`sub_8D850B` | `IO/UITypes/UIStatusBar.cpp:1932-1959` 三段 `Gauge(CLIP, classic_gauge_bar, SEG_W_*, 0)` | ✅ | 三段量规 + 位图字库数值完整 |
| A5 | `OnButtonClicked 0x8d423d` 10 分支（1000–1007 / 1009 / 2000–2003） | `IO/UITypes/UIStatusBar.cpp:747-1176` `button_pressed` 全 switch | ✅ | **销案**：r16:147「回调未闭环」已不成立；但 id 语义映射不同（见 B5/C3） |
| A6 | 密语 `TryBeginWhisper 0x8d5bfb`/`ChangeWhisperTarget 0x8d4dbf`/`SetWhisperTarget 0x8db01c`/`GetWhisperTarget 0x4c6db2`/`ConvertWhisperToNormal 0x8d4348` | `IO/UITypes/UIChatBar.cpp:2000-2059` `focus_whisper`/`set_whisper_target` + `IO/UITypes/UIWhisperTargetDlg` | ✅ | 入口等价（`IO/UIStateGame.cpp:273-288` WHISPER 键同一条路） |
| A7 | 浮公告 `FloatNotice 0x8d5acd`/`SetProblem 0x8d5b2b` | `IO/UITypes/UIStatusMessenger.cpp:58-80`（6 条×8s）+ `IO/Components/ScrollingNotice.cpp:22-30`（全宽 23px） | ✅ | 拆成两组件，功能等价 |
| A8 | 工具提示 `ProcessToolTip 0x8d76d7`/`ClearToolTip 0x8cfd7b`/`ResetButtonToolTip 0x8defae`/`SetButtonBlink 0x8dfe21` | `IO/UITypes/UIStatusBar.cpp` 无 blink 联动；按钮 tooltip 走 `IO/Components` | ⚠️ | `SetButtonBlink` + `OnChildNotify` 通知链当前端无对应（见 C6） |
| A9 | **`CUIPartyHP::Create 0x91f934`**（循环 `v26<6`、宽 `max(150,maxwidth+100)`、`CreateUIWndPosSaved` 持久化） | 全仓无组队血条 UI 类 | ❌ | **原生 6 人组队血条当前端整块缺失**；须先确认 s9 是否下发数据（D8） |
| A10 | Boss 血条 Angel 无明确 CUI* 类；当前端用 `sub_56C9B7`(DualMobGauge) REA 证据 | `IO/UITypes/UIBossHP.cpp:31-46` `window("DualMobGauge")` 回退 `window("MobGage")` | ⚠️ | 当前端已按 BeiDou REA 实现填充条；Angel 侧需再查 `CUIMonsterCarnival`/`CField_*`（Q3 后续面） |
| A11 | `CUIEnergyBar::SetEnergy 0x7f9bad`/`OnMouseMove 0x7faa49` | `IO/UITypes/UIEnergyBar.cpp:45-86` `MAX_ENERGY=10000`/`BAR_INNER_W=70` | ✅ | |
| A12 | `CTemporaryStatView` 8 方法（Clear/SetTemporary/ResetTemporary/AdjustPosition/ShowToolTip/FindIcon/UpdatePassively） | `IO/UITypes/UIBuffList.cpp:66-96,224-250` + `IO/UITypes/UITemporaryStatView.cpp:42-141` | ✅ | 拆成「列表取消 vs 明细板」，后者构造即停用（`IO/UIStateGame.cpp:94-95`） |
| A13 | `CClock::SetTimer 0x49ae6a`/`Start 0x49aeee` | `IO/UITypes/UIClock.cpp:26-79` mode1/2 时钟倒计时 | ✅ | |
| A14 | `CUIMiniMap` 10 方法 | `IO/UITypes/UIMiniMap.h:29-34` / `.cpp:120-183,490-544` 三态 + NPC 列表 | ✅ | 缺件回退 MIN 条（`has_map`，`notes/r05` A） |

### 3.2 B · 业务（14 条）

| # | Angel 083 证据 | 当前端证据 | 判定 | 说明 |
|---|---|---|---|---|
| B1 | `CChatHelper::TryChat 0x4904be`：`CCurseProcess::ProcessString` 脏词过滤 | `IO/UITypes/UIChatBar.cpp` grep `脏\|curse\|swear` 零命中 | ❌ | **脏词过滤整块缺失** |
| B2 | 同上：2000ms 刷屏限流 + `SP_892_TOO_MUCH_CHATTING...` | 同上 | ❌ | **刷屏限流整块缺失** |
| B3 | 同上：连续 4 条同句检测 + `SP_891_REPEATING_THE_SAME_LINE...` | 同上 | ❌ | **重复句检测整块缺失** |
| B4 | 同上：禁言 `SP_893_YOU_ARE_CURRENTLY_BLOCKED_FROM_CHATTING` + `CHATLOG_ADD(sString,nType:8)` + `SP_272_CURSING_IS_NOT_ALLOWED` | `IO/UITypes/UIChatBar.cpp:1831-1923` `show_message` 无 type8 系统通道 | ❌ | **禁言/系统提示通道缺失** |
| B5 | `CUIStatusBar::OnButtonClicked`：1003→`SetChatType(1)`、1004→`SetChatType(3)` | `IO/UITypes/UIStatusBar.cpp:849 SHORT→UIShortCut`、`:865 WHISPER→focus_whisper()` | ⚠️ | 原生「聊天类型切换」两按钮当前端无对应语义（`notes/r03` §0 C 段） |
| B6 | `CUIStatusBar::SendClaim 0x8d4a22` / `SendGroupMessage 0x8df155` | `IO/UITypes/UIStatusBar.cpp:871 CLAIM→UISlideMenu.toggle_expand()` | ⚠️ | 语义不同（原生发 claim 包 vs 当前端开菜单）；s9 是否有 claim 包待核 |
| B7 | `CConfig::GetShowPartyHP 0x49f1ac` / `SP_4433_SHOWPARTYHP` / `SP_4190_MARK_HP_ONLY` / `SP_4191_MARK_NAME_AND_HP` | `Configuration.h` 无 showPartyHP 项 | ❌ | **组队血条开关 + 两种标记模式当前端无** |
| B8 | `CConfig::ApplySysOpt 0x49ea33` / `ApplyGameOpt 0x49ea1c` / `GetOpt_BOOL 0x49eddb` | `Configuration.h:814-887` `ui.gameopt.mask=4095` / `ui.sysopt.quality=98` / `hp_warn=52` / `mp_warn=52` | ⚠️ | 配置面存在但字段体系不同；`hp_warn/mp_warn` 已被 `IO/UITypes/UIStatusBar.cpp:2508` 消费（注释 `:834-846`「当前无消费方」滞后） |
| B9 | `CChatHelper::HistoryAdd 0x490701`/`HistoryUp 0x49081b`/`HistoryDown 0x4908a4`/`HistoryAddforCommand 0x4907d5` | `IO/UITypes/UIChatBar.h:221-227` `MESSAGE_HISTORY_CAP=200`（用户 2026-11-04 裁决）+ `.cpp:2585-2673` 方向键历史 | ✅ | 历史条数 200 是用户裁决值，非原生常量（原生无上限常量） |
| B10 | `CQuickslotKeyMappedMan::DefaultQuickslotKeyMap 0x72b8e6 = memcpy(this+4, dword_BDAFAC, 0x20)`（32B 默认键表） | `IO/UITypes/UIStatusBar.cpp:120-138` `CLASSIC_QS_KEYS[26]` + `IO/UITypes/UIQuickslotConfig.cpp:125-144` `kb.commit_quickslots()`+CHANGE_QUICKSLOT(183) | ⚠️ | 默认键表来源不同（原生 32B ROM 表 vs 当前端硬编码 26 键）；183 已接（`parity-gap-2026-11/README.md:50`） |
| B11 | `CUIShortCutMenu::OnCreate 0x84a6bc`（8 颗 shortcut 钮 alloc 1444B/按钮，节点 `SP_2228..2237_UI_UIWINDOWIMG_SHORTCUT_BT{ITEM,EQUIP,STAT,SKILL,QUEST,COMM,MOBBOOK,MESSENGER}`） | `IO/UITypes/UIStatusBar.cpp:849 SHORT→UIShortCut` + modern 27 submenu | ⚠️ | 当前端菜单项更多（含 NO-NATIVE 项）；原生 8 项语义可核对 |
| B12 | HP/MP 警报：登录面 `SET_HPMPALERT(0x1000)` 已落地 | `IO/UITypes/UIOptionMenu.cpp:1216` `SetHpMpAlertPacket(...).dispatch()` + `IO/UITypes/UIStatusBar.cpp:2482-2541` 5 帧闪光 | ✅ | 出站 + 消费方全通（Q2 §16 #13 校正） |
| B13 | `CUIStatusBar::OnButtonClicked` 入口 `if (CUserLocal::ms_pInstance && *(+12596)) CUserLocal::OnKeyDownSkillEnd(...)` | `IO/UITypes/UIStatusBar.cpp:747` `button_pressed` 无此入口 | ⚠️ | 原生理按键结束态的动作，当前端无 |
| B14 | `CUIStatusBar::SetChatType` 写 `this+832` 与 `CConfig+21`（聊天类型持久化） | `IO/UITypes/UIChatBar.h:302-341` classic 常量 + `Configuration.h:641-658 chat.max/x/y` | ⚠️ | 持久化键不同 |

### 3.3 C · 逻辑（14 条）

| # | Angel 083 证据 | 当前端证据 | 判定 | 说明 |
|---|---|---|---|---|
| C1 | `OnChildNotify 0x8d419c`：a2==1010(滚动条) 且 a3∈[0x12C,0x130]→`ChatLogDraw()`；a2==1012&&a3==600→`sub_8D4D43`;a2∈[0x7D0,0x7D4)&&a3==100→`SetButtonBlink(a2-2000,0)`；尾 `if(a3==100) 转发父` | `IO/UITypes/UIChatBar.cpp:731-785` `button_pressed` + `send_cursor` 自处理，无统一 OnChildNotify 转发 | ⚠️ | 原生「子控件通知→父」链当前端由各控件直接处理，缺统一 blink 清除 |
| C2 | `SetChatType 0x8d4a6d` 三态：nType1 `h=24/行=1/509-h=513/MakeCtrlEdit(0)`；nType2 `h=24/行=1/509-h=486/MakeCtrlEdit(1)`；nType3 `h=CConfig+23`(26..507)/`行=h/13`(`h%13==0→h+=2`)/`MakeCtrlEdit(1)` | `IO/UITypes/UIChatBar.h:302-341` 收起/展开两态 + `CLASSIC_LOG_H_DEFAULT=80/MIN=40/MAX=420/LOG_LINE_H=13` | ❌ | **三态 → 两态**：原生三档聊天窗类型当前端无，行高 13 一致但尺寸体系不同（`notes/r03` §0 部分销案） |
| C3 | `ChangeChatWndSize 0x8d616c`：拖拽 `h±13`、clamp 26..507、`this+833=h/13`、`this+835=this+748=509-h`、重建 `CCtrlScrollBar`(`CreateCtrl(this,1010,1,4,566-scrollbarSize,510-h,h-2,0)`)、`SetScrollRange(m_aChatLog-行+1)`、h 写回 `CConfig+23` | `IO/UITypes/UIChatBar.cpp:525-597/858-870/941-942/982-1445` 三向 DOWN/LEFT/DOWNLEFT 拖拽 + `UIChatBar.h:55 send_scroll(double)` | ⚠️ | 拖拽链完整但**常量体系不同**（原生 26..507 / 509-h / 滚动条 id 1010 vs 当前端 80/40/420/13/Slider）；须实机核对（D7） |
| C4 | `SetButtonBlink 0x8dfe21`：`a2<4`，按 0/1/2/3 取 `SP_1986..1989_..._KEY_ANI` | `IO/UITypes/UIStatusBar.cpp:747-1176` 各按钮分支无 blink 联动 | ❌ | **4 个主窗按钮闪烁提示链缺失**（原生 `UI_Toggle` 后由 `OnChildNotify` 清 blink） |
| C5 | `ToggleQuickSlot 0x8df4b1`：改 `this+836` + 写 `CConfig+22` + 动 `IWzGr2DLayer` LT/Origin | `IO/UITypes/UIStatusBar.cpp:834 QS/QSD→quickslot_active 翻转` + `:2038-2040 sync_classic_qs_buttons()` | ✅ | 折叠/展开已实现，持久化落点不同 |
| C6 | `CUIStatusBar::Draw 0x8d80a4`：`_ZtlSecureFuse<unsigned char>(v5+51,*(v5+53)) <= 0x32`（等级≤50）分支 + `(*(*field+28))(field)==22` 特殊地图分支 | `IO/UITypes/UIStatusBar.cpp:2233-2480` `draw_classic` 无等级≤50/特殊地图分支 | ⚠️ | 原生两个条件绘制分支当前端无（低优先，待实机确认是否可见差异） |
| C7 | 单体窗内 `CWnd` 子控件统一生命周期 | `IO/UIStateGame.cpp:139-152` 视口变化 `remove(STATUSBAR)` 再 `emplace` 并把 CHATBAR 移队尾 | ⚠️ | 拆分带来的额外重排逻辑（逐字注释「吞掉整条底栏 y>=649 点击…重排回原生优先级」） |
| C8 | `CCtrlMLEdit::OnIMEComp 0x4d33c2` + `CreateIMECandWnd 0x4d56b4` + `DestroyIMECandWnd 0x4d5be0`（完整 IME 候选窗状态机） | `IO/Window.cpp:901-925` `SDL_StartTextInput` + `IO/UI.cpp:522` 依 `accepts_unicode()` 开关 + `IO/Components/Textfield.cpp:274-336` UTF-8 提交 | ❌ | **原生 IME 候选窗状态机缺失**（当前端 = 系统 IME + UTF-8 提交，无候选窗） |
| C9 | `CCtrlMLEdit` 聊天输入可组合中文 | `IO/UITypes/UIChatBar.cpp:139-165` Textfield 未 `set_accept_unicode(true)`（全工程仅 4 处调用，均非聊天框）；`IO/Components/Textfield.h:84` 默认 false | ❌ | **聊天框中文输入仍被拦**（`IO/UI.cpp:522` 依 `accepts_unicode()` 关 IME；与 `IO/Window.cpp:901-925` 系统 IME 开着不冲突） |
| C10 | `CUIStatusBar::OnKey 0x8d536c` / `GetEmotionKey 0x8d57ca` | `IO/UITypes/UIStatusBar.cpp:1178-1437` `send_key` + `IO/UIStateGame.cpp:251-259` ESC 兜底 | ✅ | 快捷键链完整（含 2026-11-06 五死键接线 `:269-272`） |
| C11 | `CUIStatusBar::OnButtonClicked` 1000→`SendMigrateToShopRequest(2000,0)`；2000..2003→`UI_Toggle(1/0/2/3)+SetButtonBlink(0..3)` | `IO/UITypes/UIStatusBar.cpp:751 SHOP→EnterCashShopPacket`、`:785/795/805/815 EQUIP/INV/STAT/SKILL→各 emplace` | ⚠️ | 语义映射不同（s9 适配）：原生发迁移包/切窗 + blink，当前端发现金商城包/开各窗、无 blink |
| C12 | `CUIStatusBar::OnChildNotify` 滚动条 a3∈[0x12C,0x130] → `ChatLogDraw()` + `*(this+788)=time()` | `IO/UITypes/UIChatBar.cpp:119-133` Slider + `:341-478` 历史倒排 | ⚠️ | 滚动重绘链等价但触发码不同 |
| C13 | `CClock::SetTimer 0x49ae6a = if(*(this+30)==0){*(this+32)=a2; InvalidateRect(0);}` | `IO/UITypes/UIClock.cpp:62-79` `remain_ms` 递减 → `clock.zero` | ✅ | |
| C14 | `CUIPartyHP::Create` 6 人循环 + 名宽测量 + 位置持久化 | 无 | ❌ | 同 A9（逻辑侧） |

### 3.4 D · UI（14 条）

| # | Angel 083 证据 | 当前端证据 | 判定 | 说明 |
|---|---|---|---|---|
| D1 | 单体 `CUIStatusBar` 画布 `GetCanvas(0,0,800,578,0xFFFFFF)` | `IO/UITypes/UIStatusBar.cpp:189-198` `classic_mode` 分 modern(StatusBar3)/classic(StatusBar.img) 双实现 | ⚠️ | 结构差异主项（同 A1） |
| D2 | `OnCreate 0x8d01b2` 11 个 `CreateCtrl`（1000/1001/1002/1007/2000/2001/2002/2003/1005/1006/1009） | `IO/UITypes/UIStatusBar.cpp:2055-2074` 13 颗 classic 按钮 + modern 27 submenu（`:441-486`） | ✅ | 控件数量更多（含 NO-NATIVE 项） |
| D3 | `SP_1973 BTSHOP`/`SP_1974 BTSHORT`/`SP_1975 BTMENU`/`SP_1976 BTCLAIM`/`SP_1979..1983 EQUIPKEY/INVENKEY/STATKEY/SKILLKEY/KEYSET`/`SP_1984 QUICKSLOT`/`SP_1985 QUICKSLOTD` | `Util/UiAssetResolve.cpp:79-82` `UI["StatusBar3.img"]` 回退 `["StatusBar.img"]`；`IO/UITypes/UIStatusBar.cpp:1867-2104 init_classic` 节点树 | ✅ | 节点引用齐全（`notes/r02` §1） |
| D4 | `CCtrlScrollBar` 14 方法 / `CCtrlButton` 9 / `CCtrlEdit` 9 / `CCtrlComboBox` 5 / `CCtrlMLEdit` 12 | `IO/Components/MapleButton.h:28-46` 四态 / `MapleFrame.cpp:26-40` 九切片 / `Textfield.h:32-37` 三态 / `NameTag.h:29-50` w/e/c/clr / `Charset.cpp:24-43` 转义 | ✅ | 通用组件等价（原生控件体系更细） |
| D5 | `CSlideNotice::OnCreate 0x7e1686` 画布 800×600 / `CFloatNotice::CreateFloatNotice 0x58c53b` | `IO/Components/ScrollingNotice.cpp:22-30` width=800/黑底 23px/A12M YELLOW + `UIStatusMessenger` | ✅ | 公告条等价 |
| D6 | `CUIShortCutMenu::OnCreate 0x84a6bc` 8 钮 + `ResetButtonState 0x84af14` | `IO/UITypes/UIStatusBar.cpp:1627-1820 toggle_menu/setting/community/character/event/remove_menus` | ✅ | |
| D7 | `CUISoftKeyboard`/`CSoftKeyboardDlg`/`CUIAntiMacroCharacterName`（Angel 有类） | `IO/UITypes/UISoftKey.h:27-32`（V17.7 原生 `UIWindow.img/SoftKeyboard` 宽 QWERTY，与 `Login.img/Common/SoftKey` 两套） | ✅ | |
| D8 | `CUIToolTip` 26 方法 / `CUIQuestAlarm` 6 / `CUIMedalQuestInfo` 10 | `IO/Components/*Tooltip*` / `IO/UITypes/UIQuestAlarm` | ✅ | Q3 后续面细校 |
| D9 | **`UIMobGage` 对应原生类缺位** | `IO/UIElement.h:147 MOBGAGE=102` 有 Type 值但 `IO/UITypes/UIMobGage.*` **未找到**（glob 零命中）；`UIBossHP.cpp:31-33` 仅把 `"MobGage"` 当资源节点回退名 | ⚠️ | 枚举占位无实体类（低优先，可保留） |
| D10 | `CUIKeyConfig` 7 / `CQuickslotKeyMappedMan` 3 | `IO/UITypes/UIQuickslotConfig.cpp:92-144` + `IO/UITypes/UIKeyConfig` | ✅ | |
| D11 | Angel 无 `StatusBar3.img` 概念（v83 只有 `StatusBar.img`） | `IO/UITypes/UIStatusBar.cpp:201-534 init_modern` 五档宽 800/1024/1280/1366/1920 | ⚠️ | **modern 皮肤是插件/高版本扩展**（`01-parity-matrix-and-plan.md:22` 33 项扩展全在 `ijl15.dll`）；按 083 基准应标「扩展面，非原生缺口」 |
| D12 | `CPinCodeDlg`/`CUILoginStart` 属登录面 | — | ⊘ | 不在 HUD 面范围（登录面已校准） |
| D13 | `CUIGameOpt` 4 / `CUISysOpt` 4（v83 两扇独立设置窗） | `IO/UITypes/UIOptionMenu.h:70-73`「本包没有 StatusBar3.img/OptionMenu…只剩「画一张底板」的退化壳」 | ❌ | 原生两扇设置窗当前端退化为单空壳（Q2 §13 已登记，属 UI 资源阻塞 D10） |
| D14 | `CUIStatusBar::MakeCtrlEdit 0x8dfb36`（原生编辑控件） | `IO/Components/Textfield.cpp:274-336` ASCII 限制 | ⚠️ | 见 C9 |

### 3.5 E · 排版（16 条）

| # | Angel 083 证据 | 当前端证据 | 判定 | 说明 |
|---|---|---|---|---|
| E1 | `OnCreate 0x8d01b2` 画布 **800×578** | `IO/UITypes/UIStatusBar.cpp:1901-1907` 注释「窗体高 578、设计网格宽 800、底条相对 y=CreateY-507」 | ✅ | 逐点吻合 |
| E2 | 11 个 `CreateCtrl` 坐标：`(1000,573,543)/(1001,629,543)/(1002,685,543)/(1007,741,543)/(2000,618,515)/(2001,648,515)/(2002,678,515)/(2003,708,515)/(1005,738,515)/(1006,768,515)/(1009,573,515)` | `IO/UITypes/UIStatusBar.cpp:1901-1907` 记录的 `sub_8D01B2` 坐标 `BtShop=1000/(573,543)`/`BtNPT=(629,543)`/`BtMenu=(685,543)`/`BtShort=1007/(741,543)`/`EquipKey=2000/(618,515)`… | ✅ | **★ 排版层当前端已用无符号 IDB 对齐过**（`notes/r02` §0 关键印证） |
| E3 | `GetShortCutIndexByPos 0x8de8d5` 逐字 `v3=a1-647`、`a2-427`、每格 **32×32**、坐标表 `dword_BE2DB4..BE2DF4` → **8 格** | `IO/UITypes/UIStatusBar.cpp:106-118` `QS_SLOT=28/QS_PITCH_X=35/QS_PITCH_Y=34/QS_FIRST_X=9/QS_FIRST_Y=9` | ⚠️ | **几何常量口径冲突**：数值不同，但当前端已用正确端截图逐像素对齐（`:1926-1930`、`:1921` 注），改回原生会破坏实机排版 → 须裁决（D6） |
| E4 | `SetStatusValue 0x8d82b8`：`draw_number_by_image(50-v8, 554, 等级)`（v8=0/6/12 按位数）、`DrawTextA(87,545,get_job_name)`、`DrawTextA(87,560)` | `IO/UITypes/UIStatusBar.cpp:1936-1951` `lvbadge_pos=(50,47)/joblabel_pos=(87,40)/namelabel_pos=(87,56)` | ✅ | 相对底栏顶 507 换算后同源（554-507=47 / 545-507=38≈40 / 560-507=53≈56） |
| E5 | 聊天三态 `509-h` / h 26..507 / 行高 13 / 滚动条 id **1010** | `IO/UITypes/UIChatBar.h:302-341` `LOG_LINE_H=13`（一致）/ `CLASSIC_LOG_H_DEFAULT=80/MIN=40/MAX=420` | ⚠️ | 行高 13 一致；尺寸体系不同（同 C3） |
| E6 | `CSlideNotice::OnCreate 0x7e1686` 画布 800×600 | `IO/Components/ScrollingNotice.cpp:22-30` width=800 | ✅ | |
| E7 | `CUIPartyHP::Create` 宽 `max(150,maxwidth+100)`、高 `v22*(fontH+5)+5`、`CreateWnd(...,z=10,bScreenCoord=1)` | 无 | ❌ | 同 A9 |
| E8 | `*(this+2980)=IsAdminAccount()!=0 ? 256 : 70`（管理号聊天宽 256 / 普通 70 [推断]） | `IO/UITypes/UIChatBar.cpp:139-165` Textfield 70 字上限 | ✅ | 普通号宽度吻合 |
| E9 | 正确端截图 `进入游戏页.png` 底栏顶=649、graduation 跨 x218..557、边框 y700..717、HP 红填充 y702..715 | `IO/UITypes/UIStatusBar.cpp:1926-1930` 逐字记录 | ✅ | 实机排版已对齐 |
| E10 | `StatusBar.img.xml:4` `base/backgrnd 800×71`、`:6` `backgrnd2 570×71`、`:8` `quickSlot 151×80`、`:24,27` `gauge/graduation & bar 340×31`、`:443-486` `number 5×7`、`:487-518` `FontMemo 6×7` | `IO/UITypes/UIStatusBar.cpp:1908-1921` nx_inspect 实测 `817x71/570x71/340x31/109x18/6x7/151x80/466x80/571x80` | ✅ | `817×71` vs `800×71` 是不同节点（运行时实测 vs 设计网格），**非冲突**（`notes/r03` §5） |
| E11 | `SEG_HP=0/SEG_MP=109/SEG_EXP=222` `SEG_W=108/107/118` `GAUGE_X=218/GAUGE_Y=38`（Angel `SetStatusValue` 三段量规同源） | `IO/UITypes/UIStatusBar.cpp:90-99` 同名常量 | ✅ | |
| E12 | `ui.position.chat` 概念 | `Configuration.h:712-715` `ui.position.chat="(0, 572)"` / `:719-727` `ui.position.minimap="(5, 24)"` | ✅ | 572≈578-6，与 Angel 画布 578 同源 |
| E13 | `CUIStatusBar` 单窗 position 由 `CWnd` 管理 | `IO/UITypes/UIStatusBar.cpp:2038-2040,2096-2097` >1280 宽屏整组居中（`VW` 相关） | ⚠️ | 用户决策偏离原生左贴边（`notes/r04` §8 #21），须实机复核 |
| E14 | 快捷栏 `quickSlot08 151x80`(8格)/`quickSlot26 466x80`(26格)/`quickSlot32 571x80`(32格) | `IO/UITypes/UIStatusBar.cpp:2017-2052` `wide=VWIDTH>=1024`→26格/13列 否则 8格/4列 | ✅ | 选型逻辑与节点一致 |
| E15 | `CUIStatusBar::OnCreate` `IWzGr2D::CreateLayer uWidth:151,uHeight:80` 与 `uWidth:800,uHeight:578` | `IO/UITypes/UIStatusBar.cpp:2089-2101` `position=(hud_center_x,VH-71)/dimension=(VW,80)` | ✅ | |
| E16 | `CUIPartyHP` z=10 / bScreenCoord=1 | `IO/UITypes/UIBossHP.cpp:119-127` `position=((new_width-bw)/2,20)/dimension=(681,43)` | ⚠️ | 顶栏 y=20 两个不同窗（组队血条 vs Boss 血条），当前端只有后者 |

### 3.6 F · 样式（12 条）

| # | Angel 083 证据 | 当前端证据 | 判定 | 说明 |
|---|---|---|---|---|
| F1 | `OnCreate 0x8d01b2` 多色 IWzFont（Arial 12，颜色 `0xFF000000`/`-1`/`0xFF00FF00`/`0xFFBBBBBB`/`-256`） | `Graphics/Text.h:33-48` 12 字号档 A11M..A18B；HUD 用 A11M/A11B/A12M/A12B/A13M/A18M | ✅ | |
| F2 | `number 5×7` 白数字 + `Lbracket/Rbracket 2×9` 黄绿括号（原生字库） | `IO/UITypes/UIStatusBar.cpp:140-175` `hud_advance`/`draw_hud_number`；`IO/Components/Charset.cpp:24-43 charset_char()` 转义 `Lbracket→'['/percent→'%'` | ✅ | 逐字注释「旧实现只取节点名首字符导致 HP/MP/EXP 画错」已修 |
| F3 | `FontMemo 6×7` | `IO/UITypes/UIStatusBar.cpp:2085-2088`「FontMemo 无字母字形，键帽改描边文字回退」 | ⚠️ | 自造回退而非原生（低优先） |
| F4 | `hpFlash[0..1]/mpFlash[0..1] 109×18 delay=130`（StatusBar.img.xml:34-58） | `IO/UITypes/UIStatusBar.cpp:1961-1998` 5 帧 Sprite + `:2482-2541` 阈值触发 | ✅ | 帧数差异（原生 2 帧 vs 当前端 5 帧），当前端注「不再数值一变就闪」 |
| F5 | `MapleButton` 四态 `normal/pressed/mouseOver/disabled` | `IO/Components/MapleButton.h:28-46` 四态 + `MapleButton.cpp:55-71` 加载 + `:76-108 animations[state].update(6)` | ✅ | |
| F6 | 九切片 `c/e/ne/n/nw/w/sw/s/se` | `IO/Components/MapleFrame.cpp:26-40` 同名九切片 + `xtile/ytile` 取位图尺寸 | ✅ | |
| F7 | `SP_1957 GAUGE_BAR`/`SP_1958 GAUGE_GRAY`/`SP_1959 GAUGE_TEMPEXP` | `IO/UITypes/UIStatusBar.h:27-53 Gauge::CLIP`「自纹理左侧裁出 percentage*maximum 宽（不拉伸），用于 v83 StatusBar HP/MP」 | ✅ | |
| F8 | `SP_2892 BASE_CHAT`/`SP_2643 FONTMEMO`/`SP_1300_FLOATNOTICE_D` 等 316 个 `SP_*` 节点 | `Util/UiAssetResolve.cpp:79-82` + `IO/UITypes/UIStatusBar.cpp:1867-2104` 节点树 | ✅ | Angel 只给 StringPool 索引（节点名符号化），明文路径仍以 `.nx` dump 为准（`evidence/angel-hud-facts.md` §1） |
| F9 | `ScrollingNotice` 黑底公告条 | `IO/Components/ScrollingNotice.cpp:22-30` `ColorBox(800,23,BLACK,0.535f)` + `Text(A12M,LEFT,YELLOW)` + `:64 xpos-=0.5` | ✅ | 逐字「原生公告文字完整滚出左边界后连同 23px 黑底一起消失」 |
| F10 | `NameTag w/e/c/clr` 三切片 | `IO/Components/NameTag.h:29-50` 兼容 v83 `{w,e,c,clr}` 与旧式 triple（`Login.img/CharSelect/nameTag` 仍用旧式） | ✅ | |
| F11 | `OutlinedText` 描边（对话/地图名） | `IO/UITypes/UIMiniMap.cpp:107-109` `OutlinedText(A12B,LEFT,WHITE,BLACK)` / `IO/UITypes/UIStatusBar.cpp` `namelabel` | ✅ | |
| F12 | 原生 `SP_1990/1993/1997..2000 STRING_TOOLTIPHELPIMG_GAME_BUTTON_*` 6 个按钮提示节点 | `locale/zh-CN.ini:104-111` 频道文案 / `:238-241` dailygift 假数据 / `:344-348` stub 文案 | ⚠️ | **一致性缺口（零风险可改）**：`UIChatBar.cpp:1957-1959` 名字菜单三项中文硬编码、`UIComboCounter.cpp:49` `" Combo"`、`UITemporaryStatView.cpp:26` `"临时属性"` 未走 locale；`locale/en-US.ini:160` `charselect.rank_unavailable=排行不可用` 中文残留 |

### 3.7 G · 交互（14 条）

| # | Angel 083 证据 | 当前端证据 | 判定 | 说明 |
|---|---|---|---|---|
| G1 | `OnButtonClicked 0x8d423d` 10 分支 | `IO/UITypes/UIStatusBar.cpp:747-1176` button_pressed | ✅ | **销案**（r16:147 / r15:129-135 标 ❌ 已不成立） |
| G2 | `GetShortCutIndexByPos 0x8de8d5` hit→index | `IO/UITypes/UIStatusBar.cpp:2215-2231` `send_cursor→classic_qs_slot_at→activate_classic_qs_slot→send_key` | ✅ | **销案**（r16:148「仅绘制」已不成立；`refresh_classic_qs_icons` 已补 MENU/ACTION/FACE 图标） |
| G3 | `ChangeChatWndSize 0x8d616c` 拖拽改高 | `IO/UITypes/UIChatBar.cpp:787-1463` 三向拖拽 + 松手 `Configuration::get().save()` | ✅ | 拖拽链完整（常量体系差异见 C3） |
| G4 | `CCtrlScrollBar::DoThumbMove 0x4d8203`/`DoThumbEnd 0x4d827f`/`SetScrollRange 0x4d7dac` | `IO/UITypes/UIChatBar.cpp:119-133` Slider + `:341-478` 倒排历史 | ✅ | |
| G5 | `CUIStatusBar::OnKey 0x8d536c` / `SetKeyFocus` | `IO/UIStateGame.cpp:471-482` `SAY/PARTYCHAT/FRIENDSCHAT/GUILDCHAT/ALLIANCECHAT/TOSPOUSE` → `chatbar->change_target(action)` | ❌ | **落点是 `IO/UITypes/UIChatBar.h:123 void change_target(int32_t action) {}` 空 stub**（6 个频道快捷键是死链；实际靠 chatTarget 下拉/斜杠命令生效）；`IO/UITypes/UIUserList.cpp:916` 互证 |
| G6 | `CUIStatusBar::OnButtonClicked` 1003/1004 `SetChatType` | `IO/UIStateGame.cpp:497-512` TOGGLECHAT → `chatbar->toggle_input(true)` | ⚠️ | 原生「切聊天类型」当前端变成「开输入行」（s9 适配） |
| G7 | `CUIStatusBar::TryBeginWhisper 0x8d5bfb` | `IO/UIStateGame.cpp:273-288` WHISPER → `UIWhisperTargetDlg` + `set_whisper_target` 预填 `/w 名字 ` | ✅ | |
| G8 | `CCtrlMLEdit::OnIMEComp 0x4d33c2` 组合输入 | `IO/Components/Textfield.cpp:274-336` 默认仅 `32<=c<128`（`set_accept_unicode(true)` 聊天框未调用） | ❌ | 同 C9：聊天框中文/IME 组合输入未接通 |
| G9 | `CUIStatusBar::SetButtonBlink 0x8dfe21` + `OnChildNotify` 清除 | 无 | ❌ | 同 C4 |
| G10 | `CUIStatusBar::FloatNotice`/`SetProblem` 淡出 | `IO/UITypes/UIStatusMessenger.cpp:26-48` `FADE_DURATION=8000` / 每条 `shift_y(-14)` | ✅ | |
| G11 | `CUIWorldSelect` / `CUIMiniMap` 缩放 | `IO/UITypes/UIMiniMap.cpp:516 // TODO: Toggle scrolling map`、`:666`「SMALL/BIG 未注册（本包无件，显式预留）」 | ⚠️ | 本包无件 → 保持显式预留（D13） |
| G12 | `CUISoftKeyboard` 输入校验 | `IO/UITypes/UISoftKey.cpp:400 // TODO: Add check for required amount of characters`（`bool requirements=false;`） | ⚠️ | PIC 复杂度校验占位（低优先） |
| G13 | `CUIStatusBar::OnMouseButton 0x8d5b5d`/`OnMouseMove 0x8d795a`/`OnMouseEnter 0x8d4225`/`HitTest 0x8de7da` | `IO/UIStateGame.cpp:607-697` `send_cursor` 反向扫 `elementorder` + 命中后置队尾 | ✅ | 拆分后用统一命中链替代各窗 HitTest（等价） |
| G14 | `CUIStatusBar::SetProblem 0x8d5b2b` / `SendClaim 0x8d4a22` | `IO/UITypes/UIStatusBar.cpp:871 CLAIM→UISlideMenu.toggle_expand()` | ⚠️ | 语义不同（同 B6） |

### 3.8 判定汇总

| 维度 | 条数 | ✅ | ⚠️ | ❌ | ⊘ | ⏳ |
|---|---|---|---|---|---|---|
| A 功能 | 14 | 9 | 4 | 1 | 0 | 0 |
| B 业务 | 14 | 2 | 7 | 5 | 0 | 0 |
| C 逻辑 | 14 | 3 | 6 | 5 | 0 | 0 |
| D UI | 14 | 8 | 4 | 1 | 1 | 0 |
| E 排版 | 16 | 11 | 4 | 1 | 0 | 0 |
| F 样式 | 12 | 10 | 2 | 0 | 0 | 0 |
| G 交互 | 14 | 7 | 4 | 3 | 0 | 0 |
| **合计** | **98** | **50** | **31** | **16** | **1** | **0** |

> ⚠️ 31 条里 **12 条属 s9 适配须裁决**、**6 条属历史账已销案**（r15/r16/r19 标 ❌ 但源码已实现）、**4 条可零风险先改**（D5 按钮 id 注释误标、F12 文案 locale、G5 change_target 接线、C9 聊天框 unicode）、**9 条形态/映射差异**。❌ 16 条收敛为 **6 类**（见 §4）。

---

## 4. Angel 补上的 10 条关键新发现（BeiDou 无符号 IDB 无法给出）

1. **`CUIStatusBar::OnButtonClicked @ 0x8d423d` 完整 10 分支表**：1000 `SendMigrateToShopRequest(2000,0)` / 1001 `sub_A12522` / 1002 `UI_Menu()` / 1003 `SetChatType(1)` / 1004 `SetChatType(3)` / 1005 `UI_Toggle(5)` / 1006 `ToggleQuickSlot()` / 1007 `sub_A06CBC` / 1009 `SendClaim()` / 2000–2003 `UI_Toggle(1/0/2/3)+SetButtonBlink(0..3)`；**无 1008 分支**。
2. **`OnChildNotify @ 0x8d419c` 子控件通知链**：滚动条 a3∈[0x12C,0x130]→`ChatLogDraw()`；a2==1012&&a3==600→`sub_8D4D43`；2000..2003 的 a3==100→`SetButtonBlink(a2-2000,0)`；尾转发父 → 当前端全无此链。
3. **聊天窗三态**（`SetChatType @ 0x8d4a6d` + `ChangeChatWndSize @ 0x8d616c`）：nType1/2/3 三档、h 26..507 clamp、行高 13、`509-h`、滚动条 id 1010、`SetScrollRange(m_aChatLog-行+1)`、h 写回 `CConfig+23`。
4. **`CChatHelper::TryChat @ 0x4904be` 业务规则整族**：`CCurseProcess::ProcessString` 脏词 + 2000ms 刷屏限流 + 连续 4 条同句检测 + 禁言 `SP_893` + `CHATLOG_ADD(sString,nType:8)` + 三条提示文案 `SP_272/SP_891/SP_892`。
5. **`CCtrlMLEdit` 完整 IME 状态机**：`OnIMEComp 0x4d33c2` / `CreateIMECandWnd 0x4d56b4` / `DestroyIMECandWnd 0x4d5be0`（当前端仅系统 IME + UTF-8 提交）。
6. **`CUIPartyHP::Create @ 0x91f934`**：6 人组队血条、`宽=max(150,maxwidth+100)`、`高=v22*(fontH+5)+5`、`CreateUIWndPosSaved` 持久化 → 当前端整块缺失。
7. **`SetButtonBlink @ 0x8dfe21`**：`a2<4` 按 0/1/2/3 取 `SP_1986..1989_..._KEY_ANI` → 4 个主窗按钮闪烁提示。
8. **`GetShortCutIndexByPos @ 0x8de8d5` 逐字几何**：每格 32×32、X 偏移 647、Y 偏移 427、8 格（`(BE2DF4-BE2DB4)/8`）。
9. **`SetStatusValue @ 0x8d82b8` 绘制坐标 + 等级位数偏移**：`50-v8`(v8=0/6/12)、`nTop=554`、`DrawTextA(87,545)`/`(87,560)`；`Draw @ 0x8d80a4` 有等级≤50 与特殊地图两条件分支。
10. **316 个 `SP_*` StringPool 节点符号**（HUD 面），其中 20 个 `UI_STATUSBARIMG_*`（`GAUGE_BAR/GRAY/TEMPEXP`、`BTSHOP/BTSHORT/BTMENU/BTCLAIM`、`EQUIPKEY/INVENKEY/STATKEY/SKILLKEY/KEYSET`、`QUICKSLOT/QUICKSLOTD`、`*_KEY_ANI`、`BASE_CHAT/FONTMEMO`）→ 解释 Angel 字符串缓存仅 1281 但符号更丰富；明文节点路径仍须 `.nx` dump。

---

## 5. 与上一轮既有校准/文档的关系（销案 / 推翻 / 新增 / 遗留）

### 5.1 销案（旧账标 ❌ / P0，源码逐字证明已实现）
| 旧账 | 当前源码 | 结论 |
|---|---|---|
| `r16-rea-coverage-gap.md:147` 底栏按钮「回调未闭环 🟡」 | `IO/UITypes/UIStatusBar.cpp:747-1176` 全 switch | ✅ 销案 |
| `r16:148` 右侧快捷 8 格「❌ 仅绘制」 | `UIStatusBar.cpp:2215-2231` `hit→send_key` 全链 + `:2148-2157` MENU/ACTION/FACE 图标 | ✅ 销案 |
| `r15-baseline-ui-gap-vs-screenshots.md:104,110-115` `world.channel_count<2` 单频道世界跳过 | `IO/UITypes/UIWorldSelect.cpp:918-997` 无条件建钮、门槛降 `==0`；`r19-w0-baseline-ui-impl.md:13` 2026-10-22 已改 | ✅ 销案（**登录面**，但 `feature-gap-inventory-2026-11/README.md:594` 仍标 ❌） |
| `r15:131,140,151` / `r19:16` `[DBG-CLK]` 写 `_clk.log` | 全工程 `IO/` 零命中，`UIStatusBar.cpp` 单独 grep `DBG-CLK|ofstream|fopen|\.log` 零命中 | ✅ 销案（`notes/r05` G2） |
| `parity-gap-2026-11/README.md:50` `183 CHANGE_QUICKSLOT` / `4096 SET_HPMPALERT` | `IO/UITypes/UIQuickslotConfig.cpp:125-144` / `IO/UITypes/UIOptionMenu.cpp:1216` | ✅ 已接 |
| `ui-v83-window-restore/notes/open-issues.md:204-227` classic HUD `buttons[id]` 空指针 | `UIStatusBar.h:91-93` + `.cpp:1612-1740` `button_at()`/`set_buttons_active()` | ✅ 已修 + 线上复验 |

### 5.2 推翻（旧推断错，Angel 给真值）
| 旧推断 | Angel 逐字 | 结论 |
|---|---|---|
| `IO/UITypes/UIStatusBar.cpp:1905` 注释「`QuickSlot=2005/(768,515)` `QuickSlotD=2006/(573,515)`」 | `OnCreate` 逐字 `(1006,768,515)` 与 `(1009,573,515)`；`1009→SendClaim()`，节点 `SP_1976_BTCLAIM`；`SP_1985_QUICKSLOTD` 更可能对应 `1006/(768,515)` 的 `ToggleQuickSlot()` | **当年无符号推断的按钮 id 命名/编号误标** → 零风险改注释（D5） |
| `r16:152`「坐标近 C，交互行为未达 C」 | Angel `OnButtonClicked`/`OnChildNotify`/`SetChatType`/`CChatHelper` 全可 decompile | 行为层已可取证，旧「未达 C」口径作废 |
| `feature-gap-inventory-2026-11/README.md:118` B9 `TemporaryStatView ❌ 全缺` | `IO/UITypes/UITemporaryStatView.cpp` 192 行存在且构造即停用（`IO/UIStateGame.cpp:94-95`） | 文档旧账与源码相反 |

### 5.3 新增（Angel 独有、当前端无 → 真缺口 6 类）
① 聊天业务规则（脏词/刷屏/重复句/禁言/系统提示通道，B1–B4）；② `SetChatType` 三态 + 原生拖拽常量体系（C2/C3）；③ `CUIPartyHP` 6 人组队血条 + `SHOWPARTYHP`/`MARK_HP_ONLY`/`MARK_NAME_AND_HP` 配置（A9/B7/C14/E7）；④ `SetButtonBlink` + `OnChildNotify` 通知链（A8/C1/C4/G9）；⑤ `CCtrlMLEdit` IME 候选窗 + 聊天框 `accept_unicode`（C8/C9/G8）；⑥ `change_target()` 空 stub 使 6 个频道快捷键成死链（G5）。

### 5.4 遗留（文档一致性冲突，须回写）
`docs/features/feature-gap-inventory-2026-11/README.md:110` B1、`:111` B2、`:587` §16 #2、`:594` §16 #9 仍标 ❌ / P0，与当前源码相反（`:594` 还引用已失效的 `UIWorldSelect.cpp:697-698`，该行现为 `clear_selected_world()`）。`notes/r03` §0 已列「按证据优先级（源码逐字 > 现场重算 > 旧基线）销案」。**本轮不擅自改 Q2 文档**（属另一交付物），列入决策点 D11 由用户裁决是否回写。

### 5.5 与登录面结论的一致性
- EA 交叉验证：登录 327/328、HUD 547/548 → **同一份 v83 二进制的不同符号状态**再次坐实。
- `ida-coords.md:3` 自称 `Maplestory95.i64` 同源属文档污染（登录面已钉死，HUD 面同样不引 095）。
- IMG 层阻塞、s9 适配红线、只读取证门禁三者与登录面完全一致。

---

## 6. 决策点（D1–D14，每条带推荐默认值）

| # | 决策点 | 推荐默认值 | 依据 |
|---|---|---|---|
| **D1** | 单体 `CUIStatusBar` → 当前端 10+2 类拆分，是否合并回单体？ | **保持拆分**（功能性等价优先，合并会重写全部 HUD 类、风险极高） | A1 / D1 / `notes/r04` §9 |
| **D2** | 脏词 / 刷屏 / 重复句 / 禁言业务规则是否补？ | **先确认 s9 服务端是否已实现**（避免双罚 / 与服务端判定冲突），确认后再定客户端补不补 | B1–B4 / `CChatHelper::TryChat` |
| **D3** | IME 候选窗：重建 `CCtrlMLEdit` 状态机，还是只开聊天框 unicode？ | **先零风险开 `UIChatBar.cpp` 的 `set_accept_unicode(true)` + 复用系统 IME**，不重建候选窗状态机 | C8/C9/G8 |
| **D4** | `change_target(int32_t action) {}` 空 stub 是否接线？ | **接线到 chatTarget 下拉选择**（零风险，6 个频道快捷键复活） | G5 / `notes/r05` C |
| **D5** | 按钮 id 注释误标（`2005/2006` vs `1006/1009`）是否改？ | **直接改注释**（零风险，纯文档） | §5.2 / `notes/r02` §0 |
| **D6** | QS 几何常量：改回原生 32×32/647/427/8 格，还是保持当前端 28/35/34/9/9？ | **保持当前端**（已用正确端截图逐像素对齐，改回破坏实机排版） | E3 / s9 适配红线 |
| **D7** | 聊天窗三态 + 原生尺寸体系（26..507/509-h/id1010）是否对齐？ | **先五屏实机截图核对三态可见差异**，有差异再定 | C2/C3/E5 |
| **D8** | `CUIPartyHP` 6 人组队血条是否实现？ | **先确认 s9 是否下发组队血条数据**，无数据则不做（避免假窗） | A9/B7/C14/E7 |
| **D9** | `SetButtonBlink` + `OnChildNotify` 通知链是否补？ | **低优先，先记录**（视觉提示，不影响功能闭环） | A8/C1/C4/G9 |
| **D10** | orange-wz IMG MCP 是否接入？ | **不接**（已知解析缺陷，证据以 `.nx` dump + 截图 + `StatusBar.img.xml` 为准） | §1.2 / `notes/r02` §6 |
| **D11** | 是否回写 `feature-gap-inventory-2026-11/README.md` 的 4 条旧账（B1/B2/#2/#9）销案？ | **回写**（属文档一致性，不改产品源码；本轮不擅自改，交用户裁决） | §5.4 |
| **D12** | `UIMobGage.*` 空 Type（MOBGAGE=102）是否补实体类？ | **保持**（`UIBossHP` 已覆盖 Boss 血条，空 Type 无害） | D9 |
| **D13** | 小地图 SMALL/BIG 缩放钮 / `TODO: Toggle scrolling map` 是否补？ | **保持显式预留**（本包无件，「不发明客户端能力」门禁） | G11 |
| **D14** | 是否进入实施阶段？ | **先按 §7 P0 做零风险纯客户端校准，P1 待上述裁决**；全程单测 + `tools/build_smoke.bat` + 五屏实机比对；原子提交须单独授权 | 门禁 |

> **最关键 = D1（保持拆分）+ D2（s9 服务端先确认）+ D6（保持当前端几何）**：三条共同决定「校准 ≠ 改回原生」。

---

## 7. 一次性 Plan（P0 零风险 → P1 需裁决 → P2 需 IMG/实机 → P3 后续面）

### P0 · 零风险纯客户端校准（源码实测：校准前已落地，本轮复验通过）
> 2026-11-10 复验：以下 P0 项在源码中**早已实现**（此前会话完成但未回写文档），本轮逐字核对确认：
1. **D4** ✅ `UIChatBar::change_target(int32_t)` 已完整实现并接线 `pick_classic_target` / `set_classic_target_id`（让 `UIStateGame.cpp` 6 个频道快捷键复活）。
2. **D5** ✅ `UIStatusBar.cpp` 按钮 id 注释误标已修正（`1005/1006 ToggleQuickSlot` / `1009 SendClaim` + `SP_1976_BTCLAIM`）。
3. **D3 前半** ✅ 聊天框 Textfield 已加 `set_accept_unicode(true)`。
4. **F12** ⚠️ 不改：项目无 i18n 子系统（`locale/*.ini` 无任何代码消费方），中文 UI 串对中文客户端合理，非缺陷。
5. **§5.4 文档** ✅ `notes/r03` 销案结论已归档。

### P1 · 需裁决后实施（D2/D7/D8/D9）— 用户裁决「全部实现，可同步调整 s9 服务端」，2026-11-10 已实施
6. **D2 脏词/刷屏/重复/禁言** ✅ 已实施（客户端 + 服务端双改）：
   - 客户端 `UIChatBar::try_chat()`：刷屏 2000ms 限流 + 连续 4 同句拦截 + 脏词脱敏（`curse_words()` 为客户端兜底，真词库以服务端为准）。
   - 服务端 `GeneralChatHandler`：`isMuted()` 玩家禁言拦截（SP_893 等价）+ `filterCurseWords()` 可配置脏词过滤（配置键 `chat_curse_words`，逗号分隔）。
   - 服务端新增 GM 命令 `!mute <玩家> [分钟]` / `!unmute <玩家>`（gm3），及 `Character.muteUntil` 字段。
7. **D7 聊天三态** ✅ 已实施：钳制范围对齐原生 `SetChatType`（26..507，行高 13）；新增 `chat.type`(1/2/3) 持久化；`BtMin` 逐级收起（完整 3→紧凑 2→收起 1）、`BtMax` 逐级展开；滚动条沿用原生 id 1010（内部 Slider）。
8. **D8 组队血条** ✅ 已实施（关键纠正）：**s9 早已通过 `UPDATE_PARTYMEMBER_HP(0xC9)` 向同图组队成员下发血量**（此前文档误判「不下发」系检索漏匹配）。客户端补齐 `UIParty::draw` 渲染成员血条（背景暗条 + 按比例填充，≤30% 转红）；收包端 `PartyMemberHPHandler` + `update_member_hp` 原已就位。
9. **D9 按钮闪烁** ✅ 已实施：`UIStatusBar` 主窗 4 钮（装备/背包/属性/技能）切换时脉冲高亮叠层（原生 `SetButtonBlink` 用 SP_1986..1989 KEY_ANI，本包无资源，用 7.5Hz 半透明白块替代）。

> 构建验证：`tools/build_smoke.bat` 客户端 EXITCODE=0（含 opcode 矩阵门禁 OK）；s9 `mvn compile` BUILD SUCCESS（1131 文件）。

### P2 · 需 IMG / 实机（D10/D14）
10. **D13** 小地图 SMALL/BIG 与 `UIOptionMenu` 两扇设置窗：本包无 `StatusBar3.img/OptionMenu` 节点 → 保持显式预留；若后续补资源须走 Photoshop MCP + 原生 UI 规范出图。
11. **D14 验证门禁**：单测（HUD 各 `send_cursor`/`button_pressed` 分支）+ `tools/build_smoke.bat` 全量构建（注意 `IO/UIElement.h:203 NUM_TYPES=150` 变更须整工程 Rebuild，增量编译会留旧尺寸 obj → 参见登录面 `r807-j8-rebuild-gate.md`）+ 五屏实机比对（登录/选世界/选频道/选角色/进游戏态 HUD）。

### P3 · 后续功能面（goal 续作，同一流水线）
按登录面 §7 P3 顺序继续：**核心窗口**（Skill/SkillEx/Macro/Enchant/QuestAlarm/ToolTip/CashShop 深度）→ **社交经济**（Party/Buddy/Guild/Trade/Merchant/Maker/Alliance/MonsterBook）→ **宠物召唤龙**（Q2 类 E，协议已发、缺玩法深度/真渲染/UI 分支）→ **活动小游戏**（MonsterCarnival/Minigame RPS/Wedding/PartyRace/TreasureBox/MapleTV）。每面复用本轮 3 个采集脚本（`ida_angel_hud_ea.py` / `ida_angel_hud_decompile.py` / `ida_angel_hud_facts.py`，改关键词即可）+ 三路只读取证 + 六维对照 + 决策点 + 一次性 Plan 模板。

### 验证门禁（沿用 `xiaoye-maple-story-dev`）
- 单测：HUD 命中链 / 按钮分支 / 拖拽 clamp / 频道路由。
- 构建：`tools/build_smoke.bat` → OutDir `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，**全量 Rebuild**。
- 实机：五屏 1280×720 比对 `docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/`。
- 原子提交须用户单独授权；本轮及 P0 前不改任何产品源码。

---

## 8. 限制 / 未知 / 后续

**限制**：① IMG 层阻塞（orange-wz MCP 未注册 + 已知解析缺陷），资源节点真值以仓库内 `.nx` dump + `docs/features/v83-hud-quest/evidence/StatusBar.img.xml` + 正确端截图替代；② 10 个超大函数原 `decompile` 响应被截断，已通过 `/output/<uuid>.json` 补全并归一化，但个别函数伪代码仍是 Hex-Rays 近似（`sub_A12522`/`sub_A06CBC`/`sub_8D4D43` 未命名）；③ 未做实机像素级视觉复核（`read_image` 对 hy4free 模型返回「does not declare image input」）；④ 全程只读，未构建、未实机、未提交。

**未知**：原生 `1008` 按钮为何无 `OnButtonClicked` 分支、原生 `SetProblem 0x8d5b2b` 与 `SendClaim 0x8d4a22` 的 s9 对应包、`CUIMonsterCarnival`/`CField_*` 是否承载 Boss 血条（Q3 后续面查）。

**已澄清（2026-11-10）**：① s9 **已**通过 `UPDATE_PARTYMEMBER_HP(0xC9)` 向同图组队成员下发血量（此前误判「不下发」系检索漏匹配）；② s9 服务端原本**无**脏词/刷屏(重复句)/玩家禁言规则（仅 200ms 服务端节流 + 地图禁言），本轮已补客户端 `CChatHelper` 四规则 + 服务端 `filterCurseWords` + `!mute/!unmute`。

**goal**：`goal-11388c42-2502-43f0-b722-70abeb8d4307` 保持 active/armed（登录面 + HUD 面已闭环，后续功能面待续；maxGoalRounds=30）。

---

## 9. 证据索引与复现命令

### 9.1 一手证据（本目录 `evidence/`）
| 文件 | 内容 |
|---|---|
| `angel-hud-ea.json` | 548 具名函数 / 47 类（含 decorated + demangled + EA） |
| `angel-hud-decompile.json` | 247 函数原始采集（237 嵌套 JSON 字符串 + 10 截断指针） |
| `angel-hud-decompile-full.json` | 247 函数归一化纯伪代码（1.84 MB） |
| `angel-statusbar-oncreate-full.json` | `CUIStatusBar::OnCreate` 126188 B 全文 |
| `angel-hud-facts.md` | 316 个 `SP_*` 节点 + CUIStatusBar 逐函数关键行 + 坐标候选 |
| `angel-beidou-hud-ea-crosscheck.json` | 547/548 同 EA 交叉验证 |

### 9.2 笔记（本目录 `notes/`）
`r01-angel-native-statusbar-facts.md`（Angel 一手语义）· `r02-hud-img-style-evidence.md`（IMG/样式/截图/配置）· `r03-prior-hud-doc-audit-and-cancel.md`（既有文档四态 + 销案复核）· `r04-current-hud-source-inventory.md`（当前端 12 类盘点）· `r05-increment-hud-source-details.md`（`send_key` 分支 / Buttons 枚举 / UIElement Type / `[DBG-CLK]` 销案）。

### 9.3 复现命令（Windows / `win_bash`）
```bash
cd /e/project/MapleStory-Client
export PYTHONIOENCODING=utf-8
PY="C:/Users/11036/.dsh/dsh-runtimes/dsh-primary-runtime/dependencies/python/python.exe"

# EA 索引（IDA 侧 py_eval 写文件，只回短 receipt）
"$PY" tools/ida_angel_hud_ea.py

# 批量反编译（247 函数，大函数走 /output/ 端点补全）
"$PY" tools/ida_angel_hud_decompile.py docs/features/statusbar-hud-angel-calibration-2026-11/evidence/angel-hud-decompile.json > tools/_hud_decompile_run.log 2>&1

# 语义抽取
"$PY" tools/ida_angel_hud_facts.py

# 仓库协议门禁（与登录面一致）
"$PY" tools/gen_opcode_matrix.py --check
```
MCP 端点：`http://127.0.0.1:13339/mcp`（Angel）、`http://127.0.0.1:13337/mcp`（BeiDou）；`decompile {addr, include_addresses:true}`；`py_eval {code, new_locals:true}`（逻辑包进 `def _scan(): ...` 再调用，规避 exec/eval 作用域 bug）。**端口会变，每次先 `server_health` 读 `idb_path`**。

---

## 10. 性质标注

- 本文为 **只读取证 + 全维对照 + 决策点 + 一次性 Plan** 交付物，对应 goal 第 2 个功能面（状态栏 HUD）。**用户裁决「全部实现」后，P0 复验 + P1（D2/D7/D8/D9）已落地到产品源码并构建通过**（客户端 `build_smoke` EXITCODE=0；s9 `mvn compile` BUILD SUCCESS）。原子提交待用户单独授权。
- 所有判定带 `[观察]`（源码/伪代码直读）或 `[推断]`（据直读推导）；083↔s9 冲突一律标「须裁决，不得盲改」。
- 参照基准固定 **083/v83**；未引 095 线符号/IDB 作依据（`docs/features/port-095/notes/opcode-v95.md:758,760` 的 `CUIStatusBar::SendGroupMessage/SendCoupleMessage` 仅登记存在、不作校准依据；`IO/UIStateGame.cpp:530-535` 一行 095 字样如实记录、不作依据）。
- 中文术语遵循 MEMORY.md：F 键 = 学院（FAMILY/UIFamily），G 键 = 家族（GUILD/公会，UIUserList 第 3 页签）。本面无相关术语冲突。
- 三路 subagent（`e0827232` 源码盘点 / `1605f5df` 文档审计 / `c202a760` IMG 样式）均声明全程只读、未改任何文件；全部结果已归档为 r02/r03/r04/r05。
- 下一步：等用户裁决 D1–D14，按 §7 P0 进入零风险实施；P3 由 goal 续作推进下一功能面。

（完）
