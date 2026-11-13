# 登录链路 · Angel 083 带符号 IDB 全维校准（2026-11）

> 目标（用户 m00690 逐字）：「这个ida反编译信息，是083的话，相较于之前的083反编译信息应该是详细了，我希望从登录功能开始，完整的校验和使用当前反编译内容进行校准一下所有的功能、业务、逻辑、ui、排版、样式、交互等所有内容是否正确、缺失、一致性；」
>
> 本文 = **阶段 0（只读取证）+ 阶段 0.5（全维对照）+ 阶段 1（决策点）** 的交付物。
> 阶段 2（一次性 Plan）已在 §7 完整写好，裁决后直接进入实施；本文写作过程**未改任何产品源码**。

---

## 0. 一句话结论

1. **Angel(13339) 确认是 083/v83 GMS 系**，且经 EA 交叉验证，与上一轮用的 BeiDou.exe(13337)
   是**同一份 v83 二进制的不同符号状态**（328 个登录具名函数里 327 个在 BeiDou 同 EA 存在，
   仅 1 个因 IDA 分析覆盖差异缺席）。→ 上一轮 BeiDou 校准的坐标/常量**全部可复用**，Angel
   只补上「类名 / 方法名 / 分派语义 / 字节宽度」这一层 [观察]。
2. 上一轮「BeiDou 无 CUI* 符号、全 sub_*」造成的**未决项，本轮由 Angel 符号层大批销案**：
   `CLogin::OnPacket` 21 个 case 全有具名处理函数、CHARLIST 逐字段 wire 宽度、VAC 开关
   opcode、5 个 `CUILoginStart` 按钮、7 个 `CUITitle` 按钮、`CUIRecommendWorld` 四节点、
   补间常量速度公式、GetStepHeight 的 step4 偏移分支，全部拿到一手逐字证据 [观察]。
3. **登录链路「不缺窗口文件、不缺收包 handler、不缺出站封包」**；真缺口收敛为三层：
   **(a) 083 原生与 s9 适配的字节/编号不一致**（level 1B↔2B、exp 4B↔8B、CHARLIST 外层结构、
   VAC opcode、SERVERLIST 拆包）；**(b) 客户端自行发明、无 IDA 依据的实现**（滚动 S 曲线缓动、
   ViewChoice 假 UIRegion、stub 文案）；**(c) 原生独有的交互/UI 分支当前端整段省略**
   （骑士团条件解码、case 26 上次登录世界高亮、case 27 推荐世界弹窗、case 23 SPW 结果、
   60 频道翻页）[观察 + 推断]。
4. ⚠️ **红线**：(a) 类冲突改回 083 原生会**直接断掉与 s9 服务端联机**，因此一律标注
   「s9 适配、须裁决」，不自动判当前端为错、不盲改。可零风险先做的是 (b) 类纯客户端实现。
5. **IMG 层为阻塞项**：orange-wz MCP 未在本会话注册（且既有文档记录其对私服改版 img 有解析
   缺陷），资源/样式证据以仓库内 `.nx` 节点字面量 + 运行时 nlnx dump + 正确端截图为准（§1.4）。

---

## 1. 取证基线与门禁

### 1.1 目标与范围

- **权威校准源**：`E:\MXD\反编译IDA\v83\`Angel.i64`（IDA 9.4 GUI，MCP `http://127.0.0.1:13339/mcp`），
  module `` `Angel.exe ``，input_path `C:\Users\Admin\Desktop\v83 Server\`Angel.exe`，
  imagebase `0x400000`，`auto_analysis_ready=true`、`hexrays_ready=true` [观察]。
- **交叉验证源**：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`（MCP `http://127.0.0.1:13337/mcp`），
  md5 `44c1030ecd8484e849c83892eb8fa025` [观察]。
- **被校准对象**：`E:\project\MapleStory-Client`（Git 分支 `feature/dev_0.0.1`），部署
  `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，对接服务端 s9（BeiDou-Server_s9）[观察]。
- **本轮范围**：登录链路 = 标题/账号密码 → 选世界 → 选频道 → 选角色 → 建角/取名 → 进游戏，
  含状态机、收/发包、UI 窗口、控件、坐标、资源节点、文案、补间、门闩。后续功能面按 §8.3
  同法推广（goal `goal-11388c42-2502-43f0-b722-70abeb8d4307`，maxGoalRounds=30）。

### 1.2 ★ EA 交叉验证（本轮最硬的溯源证据）

`evidence/angel-beidou-ea-crosscheck.json` / `evidence/beidou-ea-identity-check.json`：
Angel 登录相关 **328 个具名函数 / 42 类** 中，**327 个在 BeiDou(13337) 同 EA 存在**
（322 个 `sub_*` 无名、5 个具名），仅 `0x4b7404` 因 IDA 分析覆盖差异缺席 [观察]。样本：

| Angel 符号 | EA | BeiDou 同 EA | 大小 |
|---|---|---|---|
| `CLogin::ChangeStep` | `0x5f53c0` | `sub_5F53C0` | 0x59b |
| `CLogin::GetStepHeight` | `0x5fc0e4` | `sub_5FC0E4` | 0x27 |
| `CLogin::Init` | `0x5f42cd` | `sub_5F42CD` | 0x949 |
| `CLogin::OnPacket` | `0x5f80ff` | `sub_5F80FF` | 0x181 |
| `CLogin::SendCheckPasswordPacket` | `0x5f6952` | `sub_5F6952` | 0x320 |
| `CUIWorldSelect::OnCreate` | `0x624831` | `sub_624831` | 0x2b7 |
| `CUIWorldSelect::DrawWorldItems` | `0x624e13` | `sub_624E13` | 0x535 |
| `CUITitle::OnCreate` | `0x62054a` | `sub_62054A` | 0x75b |
| `CUILoginStart::OnButtonClicked` | `0x60e499` | `sub_60E499` | 0x10d |

**推论**：两端是同一份 v83 主程序的不同打包/符号状态，Angel 的符号可直接给 BeiDou 的
`sub_*` 命名。同时**钉死文档污染**：`docs/features/login-flow-ui-ida-align/notes/ida-coords.md:3`
自称坐标来自 `Maplestory95.i64`（095 线）属误记，实际坐标来自 BeiDou.exe 且无误 [推断]。
按 MEMORY.md 硬约束，本轮全程未引 095 线符号/IDB 作判定依据。

### 1.3 三联门禁（技能 `xiaoye-maple-story-dev`）

| 层 | 状态 | 产物 |
|---|---|---|
| REA | 本会话未注册 REA MCP；既有工作流为 `D:\software\nodejs-24\rea.cmd` CLI + `REA_ANALYSIS_PROVIDER=ida` 挂 13337 | 由 IDA MCP 直取等价证据 |
| **IDA** | ✅ **主路径已跑通**：13339 + 13337 各 66 工具，`decompile / lookup_funcs / xrefs_to / analyze_batch / py_eval / stack_frame / disasm / search_text` 全可用 | 95 个登录函数逐字伪代码 |
| **IMG** | ⚠️ **阻塞**：`mcp_connector_status` 仅注册 `json-photoshop`；`C:\Users\11036\.cursor\mcp.json:3-6` 有 `orange-wz` 配置但 Host 未注册 | 以 `.nx` 字面量 + nlnx dump + 截图替代 |

### 1.4 IMG 层阻塞说明与替代证据

- `docs/features/login-flow-regression-fix/notes/img-evidence-audit.md:7` 逐字：「orange-wz MCP
  对这批私服改版 img 内部节点有已知解析缺陷，未使用」；`docs/features/multi-resolution/README.md:79`
  / `notes/img-position-attributes.md:6` 记 `Index 16 out of bounds` / `list_children` 空是解析器假象，
  红线 R1/R2 一律以运行时 nlnx dump + `tools/nx_inspect.py` 为准 [观察]。
- **运行时 IMG 属性真值表**（`img-evidence-audit.md:11-27`）：`WorldSelect/chBackgrn 449x233 o(224,116)`、
  `scroll/0` 4 帧 513x152/416/408/401 delay 250/50/50、`channel/chSelect` 4 帧 23x11→59x33、
  `WorldNotice/BalloonForLoginProcess` 九宫拼框（无 184x74 单 canvas）、
  `CharSelect/charInfo1 183x151 o(45,57)` / `charInfo 183x115`、`scroll/2` 4 帧 217x30/196/195/193、
  `Login.img/Title` 仅 MSTitle 397x219 + 7 Bt* + check + effect（**无 mapleID/PW ⇒ bedou=true**）、
  `Obj/login.img/Title/signboard/0/0 368x236 o(184,118)` [观察]。
- **正确端截图**：`docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/` 7 张
  （登录界面 / 选择频道页 / 选中频道页 / 选择角色页 / 进入游戏页 / 打开了S-E-I-K页面 均 1280x720；
  键盘设置 629x369），来源 `C:\Users\11036\Desktop\正确客户端截图\`；`current_shots/` 为空 [观察]。
  **全部无配套 layout JSON**；`glob **/layout-*.json` 零命中；`evidence/screens/`、`_native-samples/`
  登录样板、登录 Photoshop 正式产物（PNG+.psd+manifest.md）全无 [观察]。
- **Angel 侧补充**：登录资源节点绝大多数走 **StringPool 编译期常量索引**（IDA 符号化为 `SP_*`），
  不是明文字符串 —— `evidence/angel-login-strings.json`：`total_strings=1281`、`node_paths=181`，
  登录相关明文仅 `UI/Login.img/ViewAllChar/Job/0 @ 0xaf6fd0` 一条。已提取 62 个 `SP_*` 节点符号，
  其中 30 个 `UI_LOGINIMG_*`（`SP_1345/1347/1348/1349/1350/1351/1352/1353/1354_TITLE_*`、
  `SP_1355/1357_WORLDSELECT_BTWORLD_D/E`、`SP_1313/1314_CHARSELECT_EFFECT_0/1`、
  `SP_1315/1316_CHARSELECT_PAGEL_/PAGER_`、`SP_1323_CHARSELECT_NAMETAG`、
  `SP_3982/3983/3986/3987_VIEWALLCHAR_*`、`SP_5362-5365_WORLDSELECT_ALERT_*`、
  `SP_3053_PINCODE_TEXT_D`、`SP_1306_UI_MAPLOGINIMG`、`SP_1307_UI_LOGINIMG_COMMON_FRAME`、
  `SP_1410_CANVASFONT`、`SP_5527_ARIAL`）[观察]。这解释了 Angel 字符串缓存 1281 < BeiDou 3889
  但符号信息反而更丰富。

### 1.5 证据产物清单（`evidence/`）

| 文件 | 内容 |
|---|---|
| `angel-login-ea.json` | 328 具名函数 / 42 类，含 decorated + demangled + EA |
| `angel-login-decompile.json` | 55 个登录关键函数逐字 Hex-Rays 伪代码（带 `/*0xNNNN*/`） |
| `angel-login-decompile2.json` | 40 个补充函数（OnPacket 分派引用 + CLoginUtilDlg + VAC 系列） |
| `angel-login-pseudocode.md` | 55 函数可读版（304 KB） |
| `angel-login-facts.json` | cases / decode_seq / encode_seq / notice_codes / create_ctrl / num_freq / strings 结构化 |
| `angel-login-controls.md` / `.json` | 控件 / 坐标 / 按钮 / 补间 / 封包 / 资源节点语义行 |
| `angel-login-branches.md` | 37 个函数的 case / 错误码 / 状态转移 |
| `angel-login-strings.json` | 62 个 `SP_*` 节点符号 + 181 明文节点路径 |
| `angel-login-chardecode.json` | `GW_CharacterStat::Decode @ 0x4e2a84` + `AvatarLook::Decode @ 0x4e749a` |
| `angel-login-chardecode2.json` | `sub_4E2745` / `sub_4E807A` / `sub_4E80EB`（骑士团分支 + ZtlSecure 混淆） |
| `beidou-ea-identity-check.json` / `angel-beidou-ea-crosscheck.json` | EA 交叉验证 |
| `notes/r01-prior-beidou-alignment-audit.md` | 上一轮 BeiDou 校准的四态清单（已校准/未闭环/待实机/推测）+ 常量表 + 优先复核 15 点 |
| `notes/r03-angel-onpacket-vs-current-dispatch.md` | `CLogin::OnPacket` 21 case vs 当前端 15 handler 全对照 |
| `notes/r04-charentry-byte-layout.md` | CHARLIST 逐字段一手字节对照 |

采集脚本（可复现）：`tools/ida_angel_login_ea.py`、`tools/ida_angel_login_decompile.py`、
`tools/ida_angel_login_decompile2.py`、`tools/ida_angel_login_facts.py`、`tools/ida_angel_login_controls.py`、
`tools/ida_angel_login_branches.py`、`tools/ida_angel_login_strings.py`、`tools/ida_ea_crosscheck.py`。
⚠️ `py_eval` 需 `new_locals=True` 且逻辑包进 `def _scan(): ...; result = _scan()`，否则
exec/eval 双作用域会报 `NameError("name 'low' is not defined")`；大输出由 IDA 侧写本地文件、
只回短 receipt（MCP stdout 有长度上限）。

---

## 2. 口径与判定定义

### 2.1 六维 × 三判定

按用户列的维度拆成 7 组（A 功能 / B 业务 / C 逻辑 / D UI / E 排版 / F 样式 / G 交互），
每条给三个判定之一：**✅ 正确（与 083 一致）/ ❌ 缺失（083 有、当前端无）/ ⚠️ 不一致（值或语义相异）**。
「一致性」另设一列标注是否属 **s9 适配**（是 ⇒ 须裁决，不自动改）。

### 2.2 083 基准 vs s9 适配（冲突处置红线）

MEMORY.md 硬约束：原版功能判定只以 083 系证据为准。本文件「原生」列全部来自 Angel 083 IDB。
但**被校准对象是跑在 s9 服务端上的客户端**，编号/宽度差异是服务端协议决定的：

> **凡标「s9 适配」的条目，一律交决策点裁决（§6 D1–D4、D12–D13），不自动判当前端为错、不盲改。**

### 2.3 状态与证据标记

`[观察]` = 逐字伪代码/源码可直接读出；`[推断]` = 由观察推出；`[未知]` = 需实机抓包/IMG 证据。
所有路径均为仓库相对路径；行号为本次阅读时的实际行号。

---

## 3. 六维对照总表

> Angel 列格式：`符号 @ EA` 或逐字常量；当前端列格式：`相对路径:行号` 或逐字常量。

### 3.A 功能（功能面有无）— 14 项

| # | 具体功能 | Angel 083 权威 | 当前端 | 判定 | 证据 / 备注 |
|---|---|---|---|---|---|
| A1 | 账号密码登录 | `CLogin::SendCheckPasswordPacket @ 0x5f6952` `COutPacket(1)` | `Net/Packets/LoginPackets.h:26-55` `LoginPacket : OutPacket(LOGIN)` | ✅ | opcode 1 一致 |
| A2 | 邮箱登录（当前端独有） | 无（Angel 有 `LOGIN_EMAIL=3` 等价 `OnGuestIDLoginResult` 但非邮箱） | `LoginPackets.h:57-80` `LoginEmailPacket` opcode 3 | ⚠️ | 当前端扩展；Q2 记 `LOGIN_EMAIL=3` 为客户端独有、不计入 gap |
| A3 | 选世界 | `CUIWorldSelect` 9 方法 @ `0x624831…0x6283c1` | `IO/UITypes/UIWorldSelect.cpp` | ✅ | 类已存在 |
| A4 | 选频道 | `CUIChannelSelect::OnKey @ 0x628a1c` | 登录期并入 `UIWorldSelect`，游戏内换频为 `UIChannel` | ⚠️ | 无独立 `UIChannelSelect` 类（D7） |
| A5 | 选角色 | `CUICharSelect::OnButtonClicked @ 0x601667` | `IO/UITypes/UICharSelect.cpp:1007-1237` | ✅ | 三按钮语义一致 |
| A6 | 建角 / 取名 | `CUINewCharNameSelect::OnButtonClicked @ 0x6175ea` → `SendCheckDuplicateIDPacket` | `UICommonCreation.cpp:1157-1264` + `UIRaceSelect` | ⚠️ | 无独立 `UINewCharNameSelect` 类；建角走 NX `Login.img/RaceSelect_new` 分支 |
| A7 | 角色删除 | `CLogin::SendDeleteCharPacket @ 0x5f7c4a` | `UICharSelect.cpp` BtDeleteChar → `DeleteCharPacket` | ✅ | 双端闭环 |
| A8 | 名字查重 | `OnCheckDuplicatedIDResult @ 0x5f9c72` | `CharnameResponseHandler` `LoginHandlers.cpp:364-377` | ✅ | case 13 双对齐 |
| A9 | View All Char / VAC | `CUIAvatarVAC` 13 方法 + `MakeVACDlg @ 0x5fb0e1` | `IO/UITypes/UIViewAllChar.cpp` + `ViewAllCharHandler:530-575` | ⚠️ | 出站编号不同（D12），窗在 |
| A10 | View Choice / 推荐世界 | `CUIRecommendWorld::OnCreate @ 0x62b451` 四节点 `SP_5362-5365` | `UIWorldSelect.cpp:1181-1196` 走 SoftKey → `UIRegion` 假窗 | ❌ | **原生真弹窗，当前端是假窗**（D8） |
| A11 | PIN / 二级密码（SPW） | `CPinCodeDlg::OnCreate @ 0x61dbbb`、case 6/7/23/28 | `CheckPincodeHandler:488-516` 5 类 `UILoginNotice` | ⚠️ | 收包 handler 有；case 23 `OnEnableSPWResult` 无（D7） |
| A12 | GUEST 登录 | `OnGuestIDLoginResult @ 0x5f8f27`（case 1） | 无 | ❌ | Q2 裁决「明确不做」，本轮不改 |
| A13 | 账号信息回包 | `OnAccountInfoResult @ 0x5f92df`（case 2） | 无 | ❌ | 原生独有（D7） |
| A14 | 上次登录世界高亮 | `OnLatestConnectedWorld @ 0x5f82f4`（case 26）→ `CUIWorldSelect::SetFocusWorld @ 0x6272d0` | 无 | ❌ | **交互缺口**（D7） |

### 3.B 业务（流程规则 / 状态机 / 错误码）— 12 项

| # | 具体业务规则 | Angel 083 权威 | 当前端 | 判定 | 证据 / 备注 |
|---|---|---|---|---|---|
| B1 | 登录步进 | `CLogin::ChangeStep @ 0x5f53c0` 6 步 `(v4+1)%6`：0 登录/1 选世界/2 频道·角色/3 新建/4 预览/5 其他 | `IO/UIStateLogin.cpp:303-316` `is_step_window` 认 5 种 | ✅ | 常量一致（上一轮已校准） |
| B2 | 步进高度 | `GetStepHeight @ 0x5fc0e4`：`if(*(this+90)==4) v2=*(this+133); else v2=0; return -8-600*(v2+a2)` | `IO/LoginBackground.cpp:35-39` `return -8 - 600 * step;` | ⚠️ | **缺 step==4 叠加 `this+133` 分支**（D6） |
| B3 | step5 复用 step1 高度 | `ChangeStep` 内 step5 分支 | `LoginBackground.cpp:43-45` `if (step==5) step=1;` | ✅ | 一致 |
| B4 | 补间时长 | `v62 = 300*abs(v64-v2) + 500`（存 `this+91`） | `LoginBackground.cpp:99` `300 * |value-previous| + 500` | ✅ | 逐字一致 |
| B5 | 补间缓动曲线 | `v62 = 7000*(v63-a2)/v62/2` **常量速度线性** + `sub_43E7C7(v27,2,&v41)`；`SP_1236_SCROLLUP` → `play_ui_sound` | `LoginBackground.cpp:78-79` `t = t*t*(3.0-2.0*t)` **smoothstep S 曲线** | ⚠️ | **客户端自造、无 IDA 依据**（D5，可零风险改）；`img-evidence-audit.md:34,46` 记「未记载任何 S 曲线缓动、应线性」 |
| B6 | 登录结果错误码 | `OnCheckPasswordResult @ 0x5f83ee`：switch 2/3/4/5/6/8/9/10→LABEL_75、7→`GotoTitle(0,0)`、13→Error、14→`YesNo2(27)`+官网、15→`YesNo2(26)` | `LoginResultHandler` `LoginHandlers.cpp:46-112`：2 BLOCKED_ID / 5 NOT_REGISTERED / 7 ALREADY_LOGGED_IN / 13 UNABLE_TO_LOGIN_WITH_IP / 23 UITermsOfService / default UNKNOWN_ERROR | ⚠️ | 码集合不同（s9 适配），当前端缺 3/4/6/8/9/10/14/15 分支文案 |
| B7 | 选世界结果码门禁 | `OnSelectWorldResult @ 0x5f9891` 仅 `v4==0||12||23` 进解析；其余 0xFF/6/8/9/2/3/4/5/7/10/11/13/14/15/16/17/21/25 各自 Error/官网提示 | `CharlistHandler:288-334` 无结果码门禁 | ❌ | 结构差异（s9 适配，D13） |
| B8 | 删除角色失败码 | `OnDeleteCharacterResult @ 0x5f9d15`：6/9/10/18/20/22/24/26/29 | `DeleteCharResponseHandler:404-452` s9 实发 `0x09/0x14/0x16/0x1A/0x1D/0` | ⚠️ | 码集合因服务端而异（已注明 s9 适配） |
| B9 | 查重结果 | 0 可用 / 1 `Error(5)` 已占用 / 2 `Error(10)` 非法名 / 其它 `Error(18)` | `read_bool() used` 二元 | ✅ | 语义一致（简化） |
| B10 | 建角结果 | 10→Error(19) / 26→Notice `SP_4016` / 30→Error(10) | `read_byte() result`，非 0 丢弃 | ⚠️ | 语义一致、错误码细化缺失 |
| B11 | 请求门闩 | `SendCheckUserLimitPacket @ 0x5f8078`：`if(*(this+92)==0)` → 发包 + `*(this+92)=1`；`(this+92)` DWORD = 字节偏移 368 | `UIStateLogin.cpp:77-98` 门闩看护 + `UIWorldSelect.cpp:1138-1143` 引 `IsRequestValid` | ✅ | 一致（上一轮已校准） |
| B12 | 世界人数上限 | `OnCheckUserLimitResult @ 0x5f92ae`：`Decode1`×2 → `UserLimitResult(v2,v3)` | `SERVERSTATUS=3 → ServerStatusHandler:176` `read_short()` → `set_server_status(0/1/2)` | ⚠️ | 同为 2 B 但解释不同（s9 适配，D1） |

### 3.C 逻辑（协议收发 / 字节布局 / 状态转移）— 16 项

| # | 具体逻辑 | Angel 083 权威 | 当前端 | 判定 | 证据 / 备注 |
|---|---|---|---|---|---|
| C1 | 登录收包分派 | `CLogin::OnPacket @ 0x5f80ff` **21 个显式 case**（0–28 含跳号） | `Net/PacketSwitch.cpp:417-441` **15 条 `emplace<OPCODE,Handler>()`** | ✅ | 见 `notes/r03` 全表；10 条双对齐 |
| C2 | 登录发包字段布局 | `COutPacket(1)` + `EncodeStr(acc)` + `EncodeStr(pass)` + `EncodeBuffer(MachineId)` + `Encode4(GameRoomClient)` + `Encode1(CWvsApp+36)` + `Encode1(0)`×2 + `Encode4(PartnerCode)` | `LoginPackets.h:26-55` `write_string(acc); write_string(pass); skip(6); write_byte(h); write_byte(w); write_byte(i); write_byte(d);` | ⚠️ | `skip(6)`+4×byte=10 B ↔ 原生 Buffer+4+1×3+4；须逐字节核（D12） |
| C3 | **CHARLIST level 宽度** | `Decode1` → `sub_4E807A(char …)` **1 B** | `LoginParser.cpp:138` `static_cast<uint16_t>(read_short())` **2 B** | ⚠️ | `LoginParser.cpp:136-138` 自证 s9 999 级改造（D4） |
| C4 | **CHARLIST exp 宽度** | `Decode4` → `_ZtlSecureTear<long>` wire **4 B**（内存存 8） | `LoginParser.cpp:160` `recv.read_long()` **8 B** | ⚠️ | 从 exp 起原生 19 B / 当前端 23 B（**D2，最高优先级**） |
| C5 | CHARLIST 注释/代码矛盾 | — | `LoginParser.cpp:155` 声称「两者从 exp 到 charLook 都是 19 字节」，代码实读 23 B | ⚠️ | 须真机抓帧裁决（D2）；`:156-158` 自述「进图白屏」根因 |
| C6 | **骑士团条件解码分支** | `GW_CharacterStat::Decode` `if (JOB/100==22 || JOB==2001)` → `sub_4E2745` 追加 `1+2*count` B 并省掉 SP 2 B | `LoginParser.cpp:139-149` 无条件连读 11×`read_short()` | ❌ | **真缺失**，非 s9 编号适配可解释（D3）；见 `notes/r04 §2` |
| C7 | 10 项 stat 宽度 | `Decode2` ×10 → `sub_4E80EB(__int16 …)` 各 2 B | `LoginParser.cpp:139-148` `read_short()` ×10 | ✅ | 逐字节一致 |
| C8 | CHARLIST 前段字段 | cid4/name13/gender1/skin1/face4/hair4/petids24 | `LoginParser.cpp:96,126-134` 同 | ✅ | 逐字节一致 |
| C9 | CHARLIST 尾段字段 | fame2/gacha4/mapid4/portal1/tail4 | `LoginParser.cpp:161-171` 同 | ✅ | 逐字节一致 |
| C10 | **CHARLIST 外层结构** | 首字节结果码 + `Decode1` charcount + **固定 15 槽 / 684 B** + `Decode1` bIsSPW + `Decode4` slots | `CharlistHandler:288-334` 首字节 channel_id + 按 charcount 循环 + pic + slots | ⚠️ | 结构差异（s9 适配，D13） |
| C11 | `AvatarLook::Decode` | 双 0xFF 循环 + `DecodeBuffer(0xC)` 3×petids @ `0x4e749a` | `LoginParser.cpp:175-209` `parse_look` | ✅ | 结构一致（当前端未做 `<=0x33`/`is_correct_bodypart` 校验，属简化 [推断]） |
| C12 | rankinfo 宽度 | `Decode1` 开关 + `DecodeBuffer(0x10)` 16 B / 槽 | `LoginParser.cpp:103-116` `read_bool() has_rank` + 4×`read_int()` 16 B | ✅ | 宽度一致 |
| C13 | SERVERLIST 拆包 | `OnWorldInformation @ 0x5f95b7` 单包含世界+频道全表，末尾 `DrawWorldItems` + `ChangeStep(1)` | `ServerlistHandler:255-286` 多包 `0xFF` 终止 → `draw_world()` | ⚠️ | s9 适配（D1） |
| C14 | 进游戏回包 | `OnSelectCharacterResult @ 0x5fb541`：`Decode1`×2 + switch 2–10/13–15/16,17,21,25 + 成功 `Decode4+Decode2+Decode4+Decode1+Decode4` | `ServerIPHandler` `parse_login` 4×IP + short port + `cid=read_int()` + `PlayerLoginPacket` | ⚠️ | 同为进游戏，字段布局完全不同（s9 适配，D1） |
| C15 | **VAC 出站编号** | `MakeVACDlg @ 0x5fb0e1` `COutPacket(15)+Encode1(1)` 开 / `ResetVAC @ 0x5fb155` `Encode1(0)` 关 | `VIEW_ALL_CHAR=13` / `PICK_ALL_CHAR=14` / `VIEW_ALL_WITH_PIC=32` | ⚠️ | 083 单 15 开关 vs s9 三 opcode（D12）；`UIViewAllChar.cpp:180-231` 自证「误发 19 会少一个 world int」 |
| C16 | 原生兜底分派 | default `125–127 → CStage::OnPacket(0x775fe6)` / `128–130 → CMapLoadable::OnPacket(0x644446)` | 无兜底 | ❌ | 原生独有（D7） |

### 3.D UI（窗口 / 控件存在性）— 14 项

| # | 具体 UI | Angel 083 权威 | 当前端 | 判定 | 证据 / 备注 |
|---|---|---|---|---|---|
| D1 | 标题窗 7 按钮 | `CUITitle::OnCreate @ 0x62054a`：`SP_1345` BtLogin / `SP_1347` IDSave / `SP_1348` IDLost / `SP_1349` PasswdLost / `SP_1350` BtNew / `SP_1351` HomePage / `SP_1352` BtQuit + 2 check `SP_1353/1354` | `UILogin.cpp:133-181` 七按钮同坐标 | ✅ | 逐项一致 |
| D2 | 标题按钮外链 | `OnButtonClicked @ 0x620ca5`：1002→`SP_1`_FORGOTID / 1003→`SP_2`_FORGOTPASSWORD / 1004→`SP_0`_AGECHECK / 1005→`SP_5633`_MAPLESTORYNEXONNET / 1006 | `UILogin.cpp` 按钮回调 | ⚠️ | 外链常量未逐条核（域名已失效，属样式/文案层） |
| D3 | 账号/密码输入框 | `CUITitle` 账号 `(67,12)` 132x15 / 密码 `(67,40)` 120x15；`TEXTFIELD_LIMIT=12` | `UILogin.h:50` `TEXTFIELD_LIMIT=12`；`UILogin.cpp:133-181` 同坐标 | ✅ | 一致 |
| D4 | 登录起始窗按钮数 | **`CUILoginStart::OnButtonClicked @ 0x60e499` 5 个按钮 1000–1004**（1000 `ViewRecommendWorld(0)` + 按 step 分支 `GotoWorldSelect`/`GotoTitle`；1001 `+dword_BF041C(0)`；1002 `ViewRecommendWorld(0)`；1003 `ViewRecommendWorld(1)`；1004 `sub_60EAD6`→`EnableLoginStartCtrl(1)`） | 上一轮记「3 个按钮」；当前端 `UILogo`/`UILogin` 未暴露 5 钮 | ⚠️ | **Angel 推翻上一轮「3 按钮」读数**（D8）；须复核当前端是否缺 2 个 |
| D5 | 世界选择窗按钮 | `CUIWorldSelect::OnButtonClicked @ 0x624ae8`：`IsRequestValid` → `EnableButtons(a2-1000)` + `SetKeyFocus(-1)` + `SendCheckUserLimitPacket(v3)` + `SetFocus` | `UIWorldSelect.cpp:1138-1143` 门闩注释引 `IsRequestValid` | ✅ | 语义一致 |
| D6 | 推荐世界弹窗 | `CUIRecommendWorld::OnCreate @ 0x62b451` 四节点 `SP_5362` BACKGRD / `SP_5363` BTARROWL / `SP_5364` BTARROWR / `SP_5365` BTCHOICE；`OnButtonClicked @ 0x62b708` 三按钮 1000 `MoveWorld(-1)` / 1001 `MoveWorld(1)` / 1002 `SelectWorld` | 无 `UIRecommendWorld` 类；`UIWorldSelect.cpp:1181-1196` BtViewChoice → SoftKey → `UIRegion` | ❌ | **原生真弹窗缺位**（D8） |
| D7 | 角色选择三按钮 | `CUICharSelect::OnButtonClicked @ 0x601667`：1000 `SendSelectCharPacket` / 1001,1002 `SendDeleteCharPacket` | `UICharSelect.cpp:1007-1237` 三态 PIC | ✅ | 一致 |
| D8 | 取名窗 | `CUINewCharNameSelect::OnButtonClicked @ 0x6175ea` → `SendCheckDuplicateIDPacket(Text)` | 无独立类；`UIRaceSelect` + `CharnameResponseHandler` 承担 | ⚠️ | 功能闭环但类结构不同 |
| D9 | VAC 窗 | `CUIAvatarVAC::OnCreate @ 0x608ca6`：`SP_3987` BACKGRND / `SP_3983` BTENTER / `SP_1323` NAMETAG / `SP_3986` WORLDICONS_D；`OnButtonClicked @ 0x60af0a` 仅 1001 → `SendSelectCharPacketByVAC` | `UIViewAllChar.cpp:22,108` `Login.img/ViewAllChar` | ✅ | 窗在 |
| D10 | PIN 窗 | `CPinCodeDlg::OnCreate @ 0x61dbbb` `SP_3053_PINCODE_TEXT_D` + `SetFocusChild(this+65)`×2 | `UILoginNotice` 5 类 | ⚠️ | 无独立 `CPinCodeDlg` 等价 |
| D11 | TOS / EULA 窗 | `OnConfirmEULAResult @ 0x5fc838`、`OnAcceptLicense/OnDenyLicense` | `Login.img/TOS/0 431x426` + `ConfirmEulaResultHandler:473-486` | ✅ | 在（s9 下不发，占位） |
| D12 | 角色详情板 | `CUICharDetailVAC @ 0x60745A` `SetOption(148,-710)` / `CreateFadeWnd(170,210,0,z=11)` 读 `CLogin+316` | `UICharSelect.cpp:107-130` `CS_DETAIL_*` 硬编码 | ⚠️ | 上一轮遗留「归属未明」（D14） |
| D13 | 频道页按键 → ViewAll | `CUIChannelSelect::OnKey @ 0x628a1c` → `CUILoginStart::SendViewAllCharacterPacket @ 0x629d0a` | `UICharSelect.cpp:1007-1237` BtCharacter → `ViewAllCharPacket().dispatch()` | ✅ | 语义一致 |
| D14 | 登录遮罩 / 告示 / Wait bar | `CUIWorldSelect::MakeAdvice @ 0x6281b9`、`SP_3184_WORLDNOTICE` 九宫拼框 | `UILoginNotice.cpp` / `UILoginWait.cpp` | ✅ | 在 |

### 3.E 排版（坐标 / 尺寸 / 容器 / 容量）— 16 项

| # | 具体排版 | Angel 083 权威（上一轮 BeiDou 校准 + 本轮 Angel 符号确权） | 当前端 | 判定 | 证据 / 备注 |
|---|---|---|---|---|---|
| E1 | 世界选择窗 | `CUIWorldSelect` ctor `sub_6246CE = CreateDlg(-140,-804,400,100,z:10,bScreenCoord:0)` | `UIWorldSelect.cpp` | ✅ | 上一轮已校准 |
| E2 | 频道选择窗 | `CUIChannelSelect 0x62B2CF = CreateDlg(136,51,540,167,z:211,bScreenCoord:1)` | 登录期并入 `UIWorldSelect` | ⚠️ | 无独立窗（D7） |
| E3 | 角色选择窗 | `CUICharSelect 0x60135B = CreateWnd(200,-1359,101,131,z:10,0)` | `UICharSelect.cpp:107-130` | ✅ | 已校准 |
| E4 | 建角窗 | `CUINewCharRaceSelect 0x615596 = CreateWnd(-372,-2128,700,460,z10,0)` | `UIRaceSelect.cpp:35-39` `kClassicMapOrigin(-372,-2128)` / `kClassicWnd 700x460` | ✅ | 逐字一致（`create-char-classic-dual-path/README.md:97` 已闭环，推翻 r01 待办） |
| E5 | step0 相机中心 | `CLogin::Init @ 0x5F42CD` 推出 `(28, GetStepHeight)` | `LoginBackground.cpp:47-56` `return {28.0, step_height(step)}` | ✅ | 上一轮已修（旧实现 `{137,-20}` 已证伪） |
| E6 | 选角槽位 | `x=130*(i%3)-220`；占用 `y=1384` / 空 `1348`；`CreateFadeWnd(183,151/115,…)` | `UICharSelect.cpp` `CS_SLOT_W=183` / `CS_SLOT_H_OCCUPIED=151` | ✅ | 一致 |
| E7 | 选角 FRAME_ORIGIN | `CS_FRAME_ORIGIN(-237,-54)` / `CS_LIST_T=-1218` / `CS_CANVAS_TO_FRAME_X=140` | `UICharSelect.cpp:107-130` 同名常量 | ✅ | 数值一致，来源仍标 [推断]（r01 §9 待复核） |
| E8 | 三按钮坐标 | `id 1000/1001/1002, x=4, y=-2/36/85` | `UICharSelect.cpp` | ✅ | 一致 |
| E9 | 世界槽位步进 | X 步进 27 / 30 槽 / 图标 +5 | `UIWorldSelect.cpp:60-64` `WS_SLOT_STEP_X=27` / `WS_SLOT_COUNT=30` | ✅ | 注释引 `?DrawWorldItems@CUIWorldSelect@@QAEXXZ (0x624e13)` |
| E10 | 世界容器容量 | 15 世界 | `UIWorldSelect.h:37` `WORLD_BUTTON_COUNT=15U` | ✅ | 一致 |
| E11 | **频道容器容量** | **60 频道**（`CLogin::ChangeStep` step1 分支 `15/60` 系列） | `UIWorldSelect.h:43` `CHANNEL_BUTTON_COUNT=30U`；`:438,:1362` 按 30 创建 | ❌ | **60 翻页未实现**（D9）；上一轮 README:173 已记 |
| E12 | 每项结构大小 | 684 B / 槽（`v24 += 684`；`171` DWORD） | `CharlistHandler` 按 charcount 循环，无定长槽 | ⚠️ | 结构差异（C10/D13） |
| E13 | 详情面板尺寸 | `183×151` / `183×115`，Y `1384/1348` | `CS_SLOT_H_OCCUPIED=151` | ✅ | 一致 |
| E14 | 标题窗偏移 | `TITLE_WINDOW_OFFSET=(58,16)` | `UILogin.cpp:133-181` 同 | ✅ | 一致 |
| E15 | 登录地图图元 | logo 397x219 o(198,109) / signboard 368x236 o(184,118) / eff 238x298 o(0,0) / chBackgrn 449×233 o(224,116) | `img-evidence-audit.md:11-27` 运行时 dump | ✅ | 与 IMG 属性真值表一致 |
| E16 | viewpos.y | 1280x720=968 / 1024x768=992 | `Configuration.cpp:428-432` 登录态恒 1280x720 | ✅ | 一致（无独立缩放项） |

### 3.F 样式（资源节点 / 按钮态 / 字体 / 九切片 / 文案）— 12 项

| # | 具体样式 | Angel 083 权威 | 当前端 | 判定 | 证据 / 备注 |
|---|---|---|---|---|---|
| F1 | 资源节点寻址 | **StringPool 编译期常量索引**（`SP_*`），非明文字符串；62 个 `SP_*` 已提取 | `nl::nx::UI[...]` 字符串字面量 | ⚠️ | 机制不同但等价；明文节点路径仅 1 条（§1.4） |
| F2 | 登录底板 | `SP_1307_UI_LOGINIMG_COMMON_FRAME` / `SP_1306_UI_MAPLOGINIMG` | `UILogin.cpp:79,140` `Login.img/Common/frame` + `Title_new`/`Title` | ✅ | 一致 |
| F3 | 地图背景 | `SP_1237_CHARSELECT`；`MapLogin.img/back` | `IO/LoginBackground.cpp:151,168,188-191` `MapLogin.img` | ✅ | 上一轮已修空白背景 |
| F4 | 按钮 4 态 | `MapleButton` 资源四态 | `IO/Components/MapleButton.h:28` `normal/pressed/mouseOver/disabled`；`:45-66` origin 取 `src["normal"]["0"]` | ✅ | 一致 |
| F5 | 九切片 | `MapleFrame` c/e/ne/n/nw/w/sw/s/se | `IO/Components/MapleFrame.cpp:28-39` 同九方向 + xtile/ytile | ✅ | 一致 |
| F6 | 字体 | `SP_1410_CANVASFONT` / `SP_5527_ARIAL` | `Graphics/Text.h:33-48` 12 字号档 | ✅ | 一致 |
| F7 | NameTag | `SP_1323_UI_LOGINIMG_CHARSELECT_NAMETAG` | `IO/Components/NameTag.h:59-62` 兼容 v83 `{w,e,c,clr}` 与旧式 triple | ✅ | 一致（`Login.img/CharSelect/nameTag` 仍用旧式） |
| F8 | 分页箭头 | `SP_1315/1316_UI_LOGINIMG_CHARSELECT_PAGEL_/PAGER_` | `UICharSelect.cpp` 分页 | ✅ | 一致 |
| F9 | 选中特效 | `SP_1313/1314_CHARSELECT_EFFECT_0/1`、`SP_3982_VIEWALLCHAR_SELECT` | `UICharSelect.cpp` | ✅ | 一致 |
| F10 | viewpos / 分辨率 | `Configuration` 登录态恒 1280x720 | `Configuration.h:470,477` `Width 1280` / `Height 720`；`Configuration.cpp:428-432` | ✅ | 一致 |
| F11 | **stub 文案** | 原生无「暂未开放」文案（ViewAll/ViewChoice 有真实行为，见 A10/D6） | `locale/zh-CN.ini:5-6` `worldselect.stub_view_map=查看世界地图功能暂未开放。` / `stub_view_all=查看所有角色功能暂未开放。`，但 `UIWorldSelect.cpp:1178` 已 `ViewAllCharPacket().dispatch()` | ❌ | **文案与实现不一致**（D11，可零风险改） |
| F12 | 排行不可用文案 | 原生 `CUICharDetail::Draw` 有 rank/jobrank 绘制 | `locale/zh-CN.ini:163` `charselect.rank_unavailable=排行不可用`；`UICharSelect.cpp:374-388` 硬编码；`en-US.ini:160` 保留中文原值 | ❌ | **原生有排行绘制、当前端显式预留**（D14）；`r10:101-111` rank/jobrank/burning 预留 |

### 3.G 交互（点击 / 按键 / 焦点 / 补间 / 门闩）— 14 项

| # | 具体交互 | Angel 083 权威 | 当前端 | 判定 | 证据 / 备注 |
|---|---|---|---|---|---|
| G1 | 登录按钮 | `CUITitle::OnCreate` BtLogin → `SendCheckPasswordPacket` | `UILogin.cpp:413-475` `login()` 双通道 | ✅ | 一致 |
| G2 | 自动登录预填 | — | `UILogin.cpp:327-347` `auto_login_pending`；`Configuration.h:311` `AUTO_LOGIN=false` | ✅ | 当前端扩展，闭环 |
| G3 | 账号记忆 / 口令不落盘 | — | `Configuration.h:584-636` 持久化键；`Configuration.cpp:109`「口令 login.pass 有意不落盘」 | ✅ | 当前端扩展 |
| G4 | View All 按钮 | `CUILoginStart::SendViewAllCharacterPacket @ 0x629d0a` 门 `*(v3+91)==0` → `SendViewAllCharPacket @ 0x5f6d6a` | `UIWorldSelect.cpp:1176-1179` `ViewAllCharPacket().dispatch()` | ✅ | 行为一致（编号不同见 C15） |
| G5 | View Choice 按钮 | `CUIRecommendWorld` 真弹窗 + `MoveWorld(±1)` / `SelectWorld` | `UIWorldSelect.cpp:1181-1196` SoftKey → `UIRegion` 假窗 | ❌ | **假交互**（D8） |
| G6 | 选世界点击 | `CUIWorldSelect::OnButtonClicked` → `SendCheckUserLimitPacket` | `UIWorldSelect.cpp:1206-1221` 世界按钮分支 | ✅ | 一致 |
| G7 | 频道按键 | `CUIChannelSelect::OnKey @ 0x628a1c` → ViewAllChar | `UIWorldSelect.cpp:755-831` 方向键选频道 | ✅ | 语义一致 |
| G8 | 选角点击 | 1000 进游戏 / 1001,1002 删角 | `UICharSelect.cpp:1007-1237` | ✅ | 一致 |
| G9 | 取名查重 | `CUINewCharNameSelect::OnButtonClicked` → `SendCheckDuplicateIDPacket` | `IO/Keyboard.cpp` + `CharnameResponseHandler` | ✅ | 一致 |
| G10 | 焦点管理 | `CUILoginStart::OnButtonClicked` 末尾 `if(CWndMan && *(v5+360)>0 && <=2) SetFocus(*(v5+392))`；`CPinCodeDlg` `SetFocusChild(this+65)`×2 | `UIStateLogin.cpp:252-289` `post_add` pending_activation | ✅ | 语义一致 |
| G11 | 窗口切换时序 | `CLogin::OnStepChanged @ 0x5f5962` + `CFadeWnd::Close` 条件 `(v64!=3||step!=4)&&(v64!=4||step!=3)` | `UIStateLogin.cpp:100-126` VIEWALLCHAR 关闭恢复 `viewall_hidden` + `pending_activation` | ✅ | 等价实现 |
| G12 | 旧窗口 ghost 绘制 | 原生 `CWnd::InvalidateRect(this+120)` | 上一轮 README:168 记「旧窗口滚动无 ghost 绘制」 | ⚠️ | 遗留（D14） |
| G13 | 60 频道翻页 | 原生 60 频道 + 翻页 | `CHANNEL_BUTTON_COUNT=30` 固定 | ❌ | **缺翻页交互**（D9） |
| G14 | 上次登录世界高亮 | `OnLatestConnectedWorld`（case 26）→ `SetFocusWorld @ 0x6272d0` | 无 | ❌ | **原生交互缺位**（D7） |

**总表计数**：A 14 + B 12 + C 16 + D 14 + E 16 + F 12 + G 14 = **98 条**；
✅ 55 / ⚠️ 32 / ❌ 11；其中标「s9 适配·须裁决」18 条，「可零风险先改（纯客户端）」4 条
（B5 缓动、B6 错误码文案、F11 stub 文案、F12 排行文案）。

---

## 4. 关键新发现（Angel 补上的语义层，上一轮 BeiDou 无符号时拿不到）

1. **`CLogin::OnPacket @ 0x5f80ff` 完整 21 case 分派表** —— 上一轮 r09 §4 说「opcode 4/5/6/7/8
   只在名称表无 handler」是**当前端**现状；Angel 原生 083 明确全有具名分支 [观察]。
2. **`GetStepHeight` 不是简单 `-8-600*step`** —— `step==4` 时叠加 `*(this+133)` 偏移 [观察]。
3. **补间是常量速度线性**（`7000*(v63-a2)/v62/2`），不是当前端自造的 smoothstep S 曲线 [观察]。
4. **`CUILoginStart` 有 5 个按钮（1000–1004）**，推翻上一轮「3 个按钮」读数 [观察]。
5. **`CUIRecommendWorld` 四节点 + 三按钮**：View Choice 在原生是**真弹窗**，不是 stub [观察]。
6. **`CUIWorldSelect::OnButtonClicked` 真实调用 `SendCheckUserLimitPacket`** —— 上一轮
   README:173「exe 搜不到字面量、语义无证据」被符号钉死 [观察]。
7. **CHARLIST 原生 level = 1 字节**（`sub_4E807A(char …)`），exp wire = 4 字节
   （`Decode4` → `_ZtlSecureTear<long>` 存 8）[观察]。
8. **骑士团职业条件分支** `if (JOB/100==22 || JOB==2001)` → `sub_4E2745` 追加字段 [观察]。
9. **VAC 原生用单 opcode 15 + 1 字节开关**（`MakeVACDlg`/`ResetVAC`），s9 用 13/14/32 [观察]。
10. **资源节点走 StringPool `SP_*` 常量索引**，非明文字符串 —— 解释了 Angel 字符串更少、
    符号更多 [观察]。

---

## 5. 与上一轮 BeiDou 校准的关系

| 关系 | 条目 | 处置 |
|---|---|---|
| **销案（Angel 符号确权）** | `CUIWorldSelect::DrawWorldItems @ 0x624e13`、`CUITitle::OnCreate @ 0x62054a`、`CLogin::ChangeStep` 六步进、请求门闩 `(this+92)`、step0 相机 `(28,GetStepHeight)`、`UIRaceSelect 700x460@(-372,-2128)` | 直接复用，不再列为缺口 |
| **推翻（旧读数错）** | 旧「3 按钮」→5 按钮；旧「60 频道翻页」仍未做但已有符号锚点；旧「ViewAll/ViewChoice 无证据」→ 符号钉死；旧 `ida-coords.md:3` 自称 095 同源 → 实为 BeiDou.exe（EA 交叉验证） | 写入 §3 与 `notes/r01` 复核块 |
| **新增（Angel 独有发现）** | §4 十条 | 本文 §3 已逐条入表 |
| **仍遗留** | `UIRaceSelect` 未锚相机（r01 待办 vs `create-char-classic-dual-path/README.md:97` 已闭合，状态冲突）、`CUICharDetail` ctor 地址 `0x602960` vs `0x60292F` 不一致、CHARLIST 根因两文档互斥、`+140`/`CS_FRAME_ORIGIN` 真实来源、旧窗口 ghost、step0 相机 X | 归入 D14，需实机/IMG 裁决 |

---

## 6. 决策点（技能门禁：须用户抉择，未裁决前不改产品源码）

> 每条给「推荐默认值」；用户选定后按 §7 一次性 Plan 进入实施。

| ID | 决策点 | 选项 | 推荐默认 | 影响面 |
|---|---|---|---|---|
| **D1** | **总体取向**：083 原生 vs s9 适配冲突怎么处置 | (a) 保持 s9 联机优先，仅在文档标注差异；(b) 客户端做双协议兼容层（按服务端版本切解析）；(c) 逐项回归 083 原生，同步改 s9 服务端 | **(a)** | 决定 C13/C14/B12/C10 等 18 条「s9 适配」条目是否动代码 |
| **D2** | **CHARLIST exp 宽度**：`read_long()` 8 B vs 注释自述「exp4+tail4」 vs 083 原生 wire 4 B | (a) 先抓一帧真机 CHARLIST + 读 s9 `PacketCreator.addCharStats` 源码再定；(b) 直接按 083 改 4 B；(c) 维持现状 | **(a)** | 决定 `LoginParser.cpp:160` 是否改；误改会「进图白屏」（`:156-158` 自述） |
| **D3** | **骑士团条件解码分支** | (a) 先确认 s9 是否发追加段再补；(b) 按 083 直接补 `JOB/100==22||2001` 分支；(c) 不补 | **(a)** | `LoginParser::parse_stats`；补错会导致整条 CHARLIST 错位 |
| **D4** | **level 1 B ↔ 2 B** | (a) 保留 2 B（999 级改造，代码自证）；(b) 加版本开关兼容 1 B | **(a)** | `LoginParser.cpp:138` |
| **D5** | **滚动补间 S 曲线 → 线性** | (a) 改为 Angel 常量速度线性；(b) 保留 S 曲线 | **(a)** | 纯客户端、零联机风险；`LoginBackground.cpp:78-79` |
| **D6** | **`GetStepHeight` step4 偏移分支** | (a) 补 `if(step==4) 叠加 this+133`；(b) 维持 | **(a)**（需先确认本端 step4 是否可达） | `LoginBackground.cpp:35-39` |
| **D7** | **原生独有交互/UI 分支**：case 26 上次登录世界高亮、case 27 推荐世界弹窗、case 23 SPW 结果、case 1/2 GUEST/AccountInfo、125–130 兜底 | (a) 按优先级补 26/27/23；(b) 全补；(c) 只登记不补 | **(a)**（GUEST 按 Q2 裁决不做、Academy/BossParty 按 MEMORY 不做） | 新 UI 类 + handler 注册 |
| **D8** | **View Choice / `CUILoginStart` 5 按钮** | (a) 按 `CUIRecommendWorld` 四节点补真弹窗 + 复核 2 个缺失按钮；(b) 维持假 `UIRegion` | **(a)** | `UIWorldSelect.cpp:1181-1196` + 新 `UIRecommendWorld` |
| **D9** | **60 频道翻页** | (a) 补翻页（15 世界 / 60 频道 + 翻页控件）；(b) 维持 30 | **(a)** | `UIWorldSelect.h:43` / `.cpp:438,1362` |
| **D10** | **IMG 层 orange-wz MCP** | (a) 接入并复验登录节点；(b) 继续用 `.nx` 字面量 + nlnx dump + `correct_shots/` 截图 | **(b)**（既有文档记录该 MCP 对私服 img 有解析缺陷） | 决定样式/排版层是否需重取证 |
| **D11** | **stub 文案与实现不一致** | (a) 删除 `locale/zh-CN.ini:5-6` stub 文案、改为真行为提示；(b) 保留 | **(a)** | 纯文案、零风险 |
| **D12** | **登录发包字段布局 / VAC 编号** | (a) 只做逐字节核对文档、不动代码；(b) 按 083 对齐发包 | **(a)** | `LoginPackets.h:26-55` / `Net/OutPacket.h` |
| **D13** | **CHARLIST 外层固定 15 槽 vs 按 charcount** | (a) 保持 s9；(b) 加兼容分支 | **(a)** | `CharlistHandler:288-334` |
| **D14** | **上一轮遗留 6 项**（r01 §9） | (a) 按优先级逐项实机复核；(b) 归档不追 | **(a)**（需实机/IMG） | `UIRaceSelect` 锚相机、`CUICharDetail` 地址消歧、`+140` 来源、ghost、rank/jobrank 绘制 |

---

## 7. 一次性 Plan（决策裁决后执行；按技能门禁，本轮不进入实现）

### P0 · 零风险、只依赖已有证据（D5 / D6 / D11 / D2-a / D10-b）

1. `IO/LoginBackground.cpp:78-79` smoothstep → Angel 常量速度线性（附 `SP_1236_SCROLLUP` 音效注释）。
2. `IO/LoginBackground.cpp:35-39` 补 `GetStepHeight` 的 step4 偏移分支（先确认本端 step4 可达）。
3. `locale/zh-CN.ini:5-6` stub 文案改为与 `ViewAllCharPacket().dispatch()` 一致的提示。
4. 抓一帧真机 CHARLIST 字节 + 读 `BeiDou-Server_s9` `PacketCreator.addCharStats`，裁决 D2。
5. 把 IMG 层阻塞与替代证据链固化进本文 §1.4（已完成）。

### P1 · 需 D1 / D3 / D7 / D8 / D9 裁决后实施

6. 按 D1 结论决定是否引入「协议版本适配层」；若 (a) 则只补文档标注，不改解析代码。
7. 按 D3 结论补/不补 `parse_stats` 骑士团条件分支（含 `1+2*count` 追加字节）。
8. 按 D8 补 `CUIRecommendWorld` 等价真弹窗（`SP_5362-5365` 四节点 + 三按钮），替换
   `UIWorldSelect.cpp:1181-1196` 的假 `UIRegion`；复核 `CUILoginStart` 5 按钮是否缺 2 个。
9. 按 D9 补 60 频道翻页（15 世界 / 60 频道 + 翻页交互）。
10. 按 D7 补 case 26（上次登录世界高亮 `SetFocusWorld`）、case 27（推荐世界消息）、
    case 23（SPW 开启结果）的 handler + UI 接线。

### P2 · 需 IMG / 实机

11. 按 D14 复核 6 项遗留（`UIRaceSelect` 锚相机、`CUICharDetail` 地址消歧、`+140` 来源、
    ghost 绘制、rank/jobrank 文案与绘制、step0 相机 X）。
12. 按 D10 结论决定是否接入 orange-wz MCP 并复验登录节点树。
13. 逐屏实机比对 `correct_shots/` 7 张与当前端截图，补 `current_shots/`（r21 遗留）。

### P3 · 后续功能面（同一方法推广）

14. 登录闭环后，按 §1.1 goal 继续下一功能面（建议顺序：状态栏 HUD → 核心窗口 → 社交经济
    → 宠物召唤 → 活动小游戏），每面复用「EA 索引 → 批量反编译 → 三路只读取证 → 六维对照
    → 决策点 → Plan」流水线，产物归档到 `docs/features/<功能名>-angel-calibration-2026-11/`。

### 验证门禁（每个实施回合）

- 单测：`tools/` 下对应解析用例（CHARLIST 字节回放、opcode 矩阵 `--check`）。
- 构建：`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`。
- 实机：登录→选世界→选频道→选角色→进游戏五屏截图，与 `correct_shots/` 比对。
- 原子提交：须用户明确授权后进行（技能门禁）；数据库默认只读。

---

## 8. 限制、未知与后续

### 8.1 限制

- 本轮**全部为只读取证**：未修改产品源码、未构建、未实机、未写库、未提交。
- `py_eval` 的 exec/eval 双作用域、MCP stdout 长度上限、Windows GBK 输出均已按 §1.5 规避。
- IMG 层（§1.4）为已知阻塞：结论里「样式/排版」的**节点级真值**来自运行时 nlnx dump 与截图，
  不是 orange-wz MCP 的树遍历。

### 8.2 未知（须实机/抓包/IMG 裁决）

- D2：s9 `addCharStats` 的 exp 真实宽度（注释与代码矛盾）。
- D3：s9 是否对骑士团写 `sub_4E2745` 追加段。
- C2：`EncodeBuffer(MachineId)` 实际长度（登录发包逐字节核对）。
- `CUICharDetail` ctor 地址 `0x602960` vs `0x60292F`、`+140`/`CS_FRAME_ORIGIN` 真实来源。
- `UIRaceSelect` 相机锚定状态（r01 待办 vs `create-char-classic-dual-path` 已闭合，文档冲突）。
- `CUICharDetailVAC` / `dword_BEDA4C` 归属。

### 8.3 后续功能面

 goal `goal-11388c42-2502-43f0-b722-70abeb8d4307` 仍 active/armed（maxGoalRounds=30）。
 登录链路校验闭环后按 §7 P3 继续。Q2 的 `docs/features/feature-gap-inventory-2026-11/README.md`
 提供 A–K 分类的后续功能面清单；Q1 的 `docs/features/ida-v83-angel-instance/README.md`
 提供 Angel 实例身份与 361 类 / 2612 方法全量索引 + 缺口类→EA 映射
 （`evidence/angel-gap-feature-ea.json`），可直接作为下一面的取证起点。

---

## 9. 证据索引与复现命令

```bash
# 端点确认（每次先跑，端口会变）
#   IDA MCP: http://127.0.0.1:13339/mcp (Angel v83) / http://127.0.0.1:13337/mcp (BeiDou)
#   server_health 读 idb_path / input_path / auto_analysis_ready / hexrays_ready

PY="C:/Users/11036/.dsh/dsh-runtimes/dsh-primary-runtime/dependencies/python/python.exe"
cd E:/project/MapleStory-Client

# 登录类 / EA 索引（IDA 侧写文件、只回短 receipt）
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_login_ea.py

# 55 个登录关键函数逐字伪代码；带 ea 参数可指定任意函数
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_login_decompile.py \
  docs/features/login-flow-angel-calibration-2026-11/evidence/angel-login-decompile.json
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_login_decompile.py \
  docs/features/login-flow-angel-calibration-2026-11/evidence/angel-login-chardecode.json \
  0x4e2a84 0x4e749a
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_login_decompile.py \
  docs/features/login-flow-angel-calibration-2026-11/evidence/angel-login-chardecode2.json \
  0x4e2745 0x4e807a 0x4e80eb

# 结构化事实 / 控件 / 分支 / 字符串 / EA 交叉验证
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_login_facts.py
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_login_controls.py
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_login_branches.py
PYTHONIOENCODING=utf-8 "$PY" tools/ida_angel_login_strings.py
PYTHONIOENCODING=utf-8 "$PY" tools/ida_ea_crosscheck.py

# 协议矩阵门禁（Q2 已验证：GATE OK / s9 3d5ccc6f8e）
PYTHONIOENCODING=utf-8 "$PY" tools/gen_opcode_matrix.py --check
```

**主证据源优先级**（与 Q2 §2.3 一致）：
协议层 2026-11-08 权威矩阵 > 源码逐字 `.dispatch()` / `emplace` 行 > Angel IDA 逐字伪代码
> 2026-11 现场重算 > 真机截图 > Wave 销案文档 > 2026-10-21 基线 > `gap-inventory.md`（旧 head）。

---

## 10. 性质标注

- 本文是用户 m00690「从登录开始完整校验并校准所有功能/业务/逻辑/UI/排版/样式/交互」的
  **阶段 0 / 0.5 / 1 交付物**；阶段 2（实施）在 §6 决策点裁决后按 §7 一次性 Plan 执行。
- 三路只读 subagent（`49c24c62` 当前端源码盘点、`7d02e984` 既有文档四态审计、
  `f0157972` IMG 资源与样式）结果已并入本文与 `notes/r01`；三者均声明只读、未改文件。
- 全部判定按 `[观察]/[推断]/[未知]` 标注；「s9 适配」条目一律不自动判错、不盲改。
- 参照基准固定 083/v83；全程未引 095 线符号/IDB 作判定依据（MEMORY.md critical）。
