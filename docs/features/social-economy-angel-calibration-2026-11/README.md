# 社交经济面 · Angel 083 带符号 IDB 校准（2026-11-08）

> 目标（用户 m00690 逐字）：「这个ida反编译信息，是083的话，相较于之前的083反编译信息应该是详细了，我希望从登录功能开始，完整的校验和使用当前反编译内容进行校准一下所有的功能、业务、逻辑、ui、排版、样式、交互等所有内容是否正确、缺失、一致性；」
> 本文件 = 第四功能面（社交经济：Party / Buddy / Guild / Alliance / Family / Messenger / Trade / Merchant / PersonalShop / MiniRoom / Storage / NPC Shop / Wedding / Delivery / MapleTV / BlackList / UserList）的**阶段 0 取证 + 阶段 0.5 全维对照 + 阶段 1 决策点 + 阶段 2 一次性 Plan** 交付物。
> 前三面已闭环：`login-flow-angel-calibration-2026-11/README.md`、`statusbar-hud-angel-calibration-2026-11/README.md`、`corewindow-angel-calibration-2026-11/README.md`。
> **全程只读取证，未改任何产品源码、未构建、未实机、未提交**；实施须等用户裁决 D1–D14。

---

## 0. 一句话结论

1. **Angel(13339) 与 BeiDou(13337) 是同一份 v83 二进制的不同符号状态** —— 本面 EA 交叉验证 **752/752 全部同 EA、0 缺失**（`evidence/angel-beidou-social-ea-crosscheck.json`：747 个 `sub_*` 无名 / 5 具名），结论逐字 `"same binary layout (752/752 EAs identical; differences are IDA analysis coverage, not code)"`。与登录面 327/328、HUD 面 547/548、核心窗口面 439/441 **四面一致**：Angel 只补符号/类名/调用关系，不补功能、不补版本。
2. **不缺窗口文件、不缺排版件表、不缺收包 handler、不缺出站封包** —— 当前端 30+ 独立社交窗类、88 个 `Net/Packets/` 发包类、`Net/PacketSwitch.cpp` 约 40 条社交 emplace、`UIWindow.img` 112 顶层节点树与 Angel `SP_*` 347 个（84 个 `UI_UIWINDOWIMG_*`）是**同一批节点两种表示**；Wave A 11 项全 ❌ 的旧账（`r16:188-203`）已被 `r810:78` T14 20 壳 + `r40:5` 15 壳 + `parity-gap:107` + 30+ 源码类销案。
3. **结构性差异（非缺陷）**：v83 原生是「少数几个大单体窗 + 一批 Tab/Room 子类」（`CUIUserList` 单体 10 方法 5 页签、`CUIMessenger` 26、`CTrunkDlg` 22、`CTradingRoomDlg` 17、`CUIGuildBBS` 17、`CCashShop` 66）；当前端**不是 56 类一一对应**，而是合并进 `UIUserList` 五页签 + `UIMiscShells.h` 六个 DIY 壳 + 单一 `WaveAShellHandlers.cpp`，属结构性重写 → **「校准 ≠ 改回原生」四面一致**。
4. **真缺口收敛三层**：① 原生 083 独有业务/交互整族（`UIFriendGroup` 独立窗、公会徽章编辑、MapleTV 完整播放、`PARTY_SEARCH_REGISTER(220)`、MTS/ITC 拍卖、家族聊天 `change_target` 死链）；② 显式预留 / DIY 壳 / 占位空态（UIParty 4 钮、UIBuddyList 仅关闭钮、UIStorage 3 钮、五页签 "(0/0)" 硬编码）；③ 文档一致性（**`UIMessenger.cpp` 10 处 + `.h` 4 处 v95 注释污染**、2 处独立 095 文档、Q2 清单同文件 6 处自相矛盾）。
5. **判定分布（§3.8 python 行级复核）**：98 条六维对照 **✅ 44 / ⚠️ 37 / ❌ 11 / ⊘ 3 / ⏳ 3**。⚠️37 里 12 条 s9 适配须裁决、9 条形态/映射差异、6 条显式预留、5 条文档未同步、5 条样式口径冲突；❌11 收敛 6 类真缺。s9 适配红线（改回原生会断联机，须交用户裁决，不得盲改）+ IMG 层三面阻塞（orange-wz MCP 未接入 Host，替代链 = 仓库 `.nx` 节点引用 + `tools/nx_inspect.py` + 正确端截图库 78 张）。

---

## 1. 取证基线与门禁

### 1.1 端点与权威源（每次先 `server_health` 读 `idb_path`，端口会变）

| 端口 | 实例 | 用途 | 状态 |
|---|---|---|---|
| 13339 | `E:\MXD\反编译IDA\v83\`Angel.i64`（module `` `Angel.exe ``，imagebase `0x400000`，strings 1281） | **本轮权威校准源**（带 MSVC 修饰名） | ✅ auto_analysis_ready / hexrays_ready / strings_cache_ready 全 true |
| 13337 | `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`（module `BeiDou.exe`，strings 3889） | 交叉验证「同一份二进制」+ 旧校准常量复用 | ✅ healthy |
| 13338 | headless `Maplestory95.i64` | **095 线，本任务不用**（MEMORY.md 硬约束禁引） | ⛔ 排除 |

### 1.2 三联门禁（`xiaoye-maple-story-dev` 强制 REA → IDA → orange-wz(IMG)）

| 层 | 本轮状态 | 证据 |
|---|---|---|
| REA | ✅ 历史已用（登录/HUD/核心窗口三面 `rea.cmd` decompile 无符号 BeiDou） | `r16-rea-coverage-gap.md`、`ida-layout-tables.md` |
| IDA | ✅ **本轮直接跑**（13339 带符号 + 13337 交叉） | 5 个采集脚本 + 6 份 evidence（§9） |
| IMG (orange-wz) | ⚠️ **阻塞**：`mcp_connector_status` 仅 `[json-photoshop] photoshop`、`mcp_connector_catalog(keyword=orange)`=0、`~/.cursor/mcp.json:3-6` 有声明但 Host 未注册；`login-flow-regression-fix/notes/img-evidence-audit.md:7`「对这批私服改版 img 内部节点有已知解析缺陷，未使用」、`rea-ijl15-identity.md:30` `Index 16 out of bounds` | 与前三面一致，记阻塞 |
| 替代链 | ✅ 仓库内 `.nx` 节点引用 + `tools/nx_inspect.py` 运行时 dump + `C:\Users\11036\Desktop\正确客户端截图\` 78 张 + `docs/features/userlist-five-tabs/evidence/userlist_runtime_nodes.txt:1-802` | 本面 IMG 证据比前三面充分（6 个 raw-nx-dumps + 五页签 oracle 帧） |

### 1.3 产物清单

- **5 个采集脚本**（照核心窗口面模板改）：`tools/ida_angel_social_{ea,decompile,normalize,crosscheck,facts}.py`。
- **6 份 evidence**：`angel-social-ea.json`（752 具名函数 / 56 类）、`angel-social-decompile.json`（277 函数）、`angel-social-decompile-full.json`（277 归一化纯伪代码，5 个大函数经 `/output/<uuid>.json` 补全）、`angel-social-truncated.json`（receipt）、`angel-social-facts.md`（4716 行 / 235,841 B：`SP_` 全集 347 个 / 49 类逐函数关键行 / 坐标候选）、`angel-beidou-social-ea-crosscheck.json`（752/752）。
- **5 份 notes**：`r01-angel-native-social-facts.md`（Angel 一手语义）、`r02-main-agent-crosscheck-cancel.md`（主 agent 跨端销案）、`r03-social-img-style-evidence.md`（IMG/样式证据 + §9 增量）、`r04-current-social-source-inventory.md`（当前端源码盘点 21K 归档）、`r05-prior-social-doc-audit.md`（既有文档四态 + 常量表 + 15 复核点）。
- 复用避坑：`py_eval` 用 `new_locals=True` + `def _scan(): ...; result = _scan()`（规避 exec/eval 双作用域 `NameError`）；`decompile` 大函数返回截断响应 + `/output/<uuid>.json` 端点，必须批量补全；MCP stdout 有长度上限，大 JSON 由 IDA 侧写本地文件只回短 receipt。

---

## 2. 口径与判定定义

### 2.1 六维 × 三判定

| 维 | 覆盖范围 |
|---|---|
| A 功能 | 原生能力是否存在（窗口/协议/handler/数据层） |
| B 业务 | 流程/规则/服务端闭环/子命令字段 |
| C 逻辑 | 编号/状态机/冲突取值/文档与源码一致性 |
| D UI | 窗类结构/底板/按钮/绘制 |
| E 排版 | 坐标/尺寸/容量/页签显隐/持久化 |
| F 样式 | `SP_*` 资源节点/组件常量/locale 文案/占位 |
| G 交互 | 键位/点击/发包/OPEN_UI/实机验收 |

| 标记 | 含义 |
|---|---|
| ✅ | 正确性·一致性已闭环（源码逐字 / IDA 逐字 / 现场重算互证） |
| ⚠️ | 形态/编号/口径差异或需裁决（含 s9 适配、显式预留、文档未同步），不等于错误 |
| ❌ | 真缺或错误（原生有当前端无 / 自造无依据 / 基准违规） |
| ⊘ | 明确不做（083 基准裁决 / 095 专有 / s9 无后端） |
| ⏳ | 待实机（需屏前复拍 / 双角色 / 抓帧） |

### 2.2 证据优先级（六面统一）

1. **协议层 2026-11-08 权威矩阵**（`protocol/opcode-matrix.json` + `tools/gen_opcode_matrix.py --check`）；
2. **checked-in source 逐字**（`IO/UITypes/*.cpp` / `Net/*.cpp` / `Configuration*` / `locale/*.ini`）；
3. **Angel 083 IDA 逐字**（本轮 `decompile` 伪代码 + `SP_*` 符号）；
4. 2026-11 现场重算（`parity-gap-2026-11/README.md`、`social-loop-audit-2026-11/README.md`）；
5. 真机 oracle 帧 / `userlist_runtime_nodes.txt` / 正确端截图库；
6. Wave 销案（`r810` / `r40` / `r05` 四态）；
7. 2026-10-21 旧基线（`r00`/`r14`/`r16`/`01-matrix`）；
8. `gap-inventory.md`（旧 head `44d0d5d9ca`，`client_out_sent=103`，协议层已过时，仅历史参考）。

### 2.3 083 vs s9 红线 + 术语硬约束

- **参照基准固定 083/v83**；禁止引 095 线符号/IDB（`Maplestory95.i64`、port-095 映射、`ExpeditionIntermediary` 等）作依据。
- **F 键 action26 = FAMILY =「学院」窗（`UIFamily`）**；**G 键 = GUILD =「家族」= 公会系统，原版入口是「玩家列表 & 家族」共用窗 `UIUserList` 第 3 页签**（好友/组员/家族/家族联盟/拒绝列表），不是独立浮窗。
- 判「不做」的只有 095 才有的 v95 导师 `Academy` 与远征 `BossParty` 独立面板。
- 冲突处一律标注「s9 适配，改回原生会断联机，须交用户裁决，不得盲改」；`校准 ≠ 改回原生`。

### 2.4 七处冲突取值表（本面）

| # | 冲突 | 取值 | 依据 |
|---|---|---|---|
| 1 | `PARTY_SEARCH_START(222)`/`UPDATE(223)` 字段 | 并列保留，实机抓帧裁决 | `r23:13-14`「222 `int min,max,members,jobs` / 223 空体 Stop」vs `s9-social.md:74-76`「222 空包 / 223 `write_string+byte`」 |
| 2 | GUILD 是第 3 页签 vs 独立 UIGuild 浮窗 | 主入口=第 3 页签 ✅；浮窗保留列 s9 适配 | `UIStateGame.cpp:361-363,374-377` + Angel `2030–2041` vs `UIGuild.h:37` |
| 3 | `TooltipBackdrop` 配色（跨面） | 以 checked-in `TextTooltip.h:28-47` 紫底为真值 | 核心窗口面 r04 §9 |
| 4 | Q2 清单同文件 6 处自相矛盾 | 全按 checked-in source 取值，列 D11 交用户裁决后统一回写 | `:175/:176/:185/:486/:489/:597` vs `:342/:343/:346/:493/:606`/`guild-boss-hud/README.md:8` |
| 5 | Messenger 关闭钮 `(276,6)` 坐标对、依据错 | 坐标保留，依据换引 Angel `CUIMessenger::SetCtrl 0x8505e2` | `messenger-btclose-coords/README.md:16` 引 095 `0x7F34C0` |
| 6 | `WEDDING_TALK_MORE(0x8B)` 明确不做 vs 已落地 | 取「等价 `WEDDING_TALK=139` 已落地」销案 | `wedding-flow/README.md:43-44`、`feature-gap-inventory:346/:493` |
| 7 | 公会副本 HUD「287/288 无 handler ❌」vs 已接线 | 取已接线销案 | `guild-boss-hud/README.md:8`、`PacketSwitch.cpp:662-663` |

---

## 3. 六维对照总表（98 条）

> 格式：`编号 | 对照点 | v83 权威（Angel / 083 文档） | 当前端（checked-in source） | 判定 | 依据 / 处置`。
> 判定列为纯标记（§3.8 用 python 行级解析复核计数）。

### 3.1 A · 功能（14 条）

| 编号 | 对照点 | v83 权威（Angel / 083 文档） | 当前端（checked-in source） | 判定 | 依据 / 处置 |
|---|---|---|---|---|---|
| A1 | G 键家族入口 | `CUIUserList` tab index 2 公会控件组 2030–2041 | `UIStateGame.cpp:359→:374-377→:388-389` → `UIUserList::Tab::GUILD` | ✅ | r01 §3 + r02 §1 + r04 §0，MEMORY.md 术语在源码+IDA 双侧闭环 |
| A2 | F 键学院窗 | `CUIFamily` 3 方法 / `CUIFamilyChart` 8 / `UFamilyItem` | `UIStateGame.cpp:296-309` → `UIFamily`（`UIFamily.h:39 TYPE=FAMILY`）+ `OpenFamilyPacket(146)` | ✅ | `FamilyPackets.h:10-13`、`FamilyModel.h:28-76` 常驻数据层 |
| A3 | 玩家列表五页签 | `CUIUserList::Draw 0x91bec3` `v8=*(this+369)` 5 层 if → tab 0..4 | `UIUserList.h:45-58` + `TAB_POS[5]` + `tab_button() :1441-1456` | ✅ | 与 MEMORY.md「好友/组员/家族/家族联盟/拒绝列表」逐项吻合 |
| A4 | MESSENGER c2s 五 mode | `CUIMessenger::OnCreate` 逐字 `COutPacket(122)+Encode1(0)+Encode4(a2)` | `MessengerPackets.h:13/:24/:34/:45/:56` + `:18/:29/:39/:50/:61` = 0/2/3/5/6 | ✅ | r22-messenger-protocol.md:11-22 + r01 §4 |
| A5 | MESSENGER s2c 分派 | `CUIMessenger::OnPacket` case 0/1/2/4/5/6/7/8 | `PacketSwitch.cpp:453 <MESSENGER, MessengerHandler>()` + s2c 313 | ✅ | r02 §4，双枚举 122/313 与 v83 标准一致 |
| A6 | 队伍 6 发包类 | `CTabParty` 10 / `PARTYDATA` / `CUIPartyHP` | `PartyPackets.h:27-75` + `PacketSwitch.cpp:446/:447` | ✅ | 创建/离开/加入/邀请/逐出/换队长 + UPDATE_PARTYMEMBER_HP |
| A7 | 好友增删 | `CTabFriend` 12 / `CFriend` 6 / `GW_Friend` | `BuddyPackets.h:29/:40/:50` + `UIBuddyList.cpp:94..283` | ✅ | c2s 130 / s2c 63 非对称：`s9-social.md:79-85` 2026-11-02 已修 |
| A8 | 公会 8 发包类 | `CTabGuild` 7 / `CTabGuildAlliance` 7 / `CUIGuildBBS` 17 / `CWndGuildGrade` 3 | `GuildPackets.h:27-104` + `PacketSwitch.cpp:554` | ✅ | `GuildModel.h:5-6` 共用窗第 3 页签共读同一模型 |
| A9 | 联盟双入口 | Angel `CUIUserList::CreateAllianceButton` 动态 id（联盟页在共用窗内） | `UIUserList.h:57 Tab::UNION` 页签 + `UIAlliance.h:38` 浮窗（`UIGuild.cpp:211` 打开） | ⚠️ | [形态差异] 083 原生无独立联盟窗；保留双入口交 D3 裁决 |
| A10 | 学院 FAMILY 9 包 | `CUIFamily`/`CUIFamilyChart` 与公会类树分离 | `FamilyPackets.h:10-91` 全 9 类 + `PacketSwitch.cpp:467-471,650-652` | ✅ | `FamilyInfoData`/`FamilyPrivilegeDef` 登录期下发落模 |
| A11 | 婚礼协议面 | `CField_Wedding` 4 方法 0x5814fd..0x581b78 | `WeddingPackets.h:63-207` 11 类 + s2c 72/73/74/75/320/321/43 | ✅ | `wedding-flow/README.md:29-55` 2026-11-06 双端闭环，旧「壳/不做」前提已证伪 |
| A12 | 交易/面交 | `CTradingRoomDlg` 17 / `CMiniRoomBaseDlg` 15 / `CShopDlg` 9 | `TradePackets.h:36-125` 9 类 + `UITrade.cpp:245-268` 5 钮 + PLAYER_INTERACTION 123 | ✅ | `UIMiscShells.cpp:2976-2977` 双方 CONFIRM(0x11) 握手 |
| A13 | NPC 商店 | `CShopDlg` 9 / `CAdminShopDlg` 14 / `SP_1542..1548_SHOP_*` | `UIShop.cpp:64-66/:180-240` 买/卖/页签/CHECKBOX 完整 | ✅ | `Util/UiAssetResolve.cpp:105-113 shop_root()` 回退链 |
| A14 | 公会公告板 | `CUIGuildBBS` 17 方法 0x8133a9..0x817369 | `BbsPackets.h:13-93` 7 类 + `UIGuildBBS.cpp:74-98` 11 钮 + `:696 emplace<59>` | ✅ | c2s `BBS_OPERATION=155` / s2c 59 |

### 3.2 B · 业务（14 条）

| 编号 | 对照点 | v83 权威（Angel / 083 文档） | 当前端（checked-in source） | 判定 | 依据 / 处置 |
|---|---|---|---|---|---|
| B1 | PARTY_SEARCH_REGISTER | 原生列表板回包（Angel `CUIPartySearch::StartSearch 0xa12073`） | `UIUserList` 无 220；`UIPartySearch.cpp:246`「s9 空 stub → Start 代理」；`Net/OutPacket.h` 无 220 | ❌ | 真「完全无」，§9.5.3 #3 / §16 ★ P1；列 D4 |
| B2 | PARTY_SEARCH 222/223 字段 | Angel `TabPartySearch` 7 方法可定序 | `r23:13-14` vs `s9-social.md:74-76` 两套字段互斥 | ⚠️ | [文档冲突] 先抓帧 + 读 s9 服务端 `addCharStats`，列 D2 |
| B3 | MTS / ITC 拍卖 | `CUIMTS 0x6fb07b..` / `CAdminShopDlg` 14 / `CUIAdminShopWishList` 7 | `UIMTS.cpp:86-95` draw 仅 5 行 Text、`:97-99 update(){}` 空体；仅 `ENTER_MTS=156` | ❌ | s9 无拍卖后端，c2s `0xFD` 完全无；列 D10（维持明确不做 or 立项） |
| B4 | 每日签到闭环 | 083 原生无此系统（NO-NATIVE） | `UIDailyGift.h:32-58` 纯客户端假数据 28 天 7×4，`.dispatch` 零命中 | ❌ | 相邻福利壳；s9 两侧零 DAILY/SIGN/CHECKIN opcode；列 ⊘/D10 |
| B5 | 交易所 TradeHouse | 083 `UIWindow.img` 112 顶层**无** TradeHouse | `docs/features/trade-house-083/DESIGN.md:529-537`「本版明确不做（预留）：业务代码/Handler/DDL/DML/PSD/opcode 锁定」 | ⊘ | 083 基准裁决「不做」，非缺口 |
| B6 | 公会改职衔 0x08 | Angel `CWndGuildGrade` 3 | `social-loop-audit-2026-11/README.md:30`「服务端有 handler，客户端缺钮，低优先」 | ⚠️ | [低优先] 列 D8 |
| B7 | NOTE_ACTION(131) 便签 | `SP_1260/1261/1262_UI_BASICIMG_BTCLAIM/BTMIN/BTMAX` | `pending-live-tests/README.md:234-239` s9 仅 `getAvailableNotes()>0` 才落库 | ⏳ | 需真机复核是否真发，列 D8 |
| B8 | 好友分组独立窗 | Angel `CUIFriendGroup` 2 / `CFriend` 6 | glob 无 `UIFriendGroup`；`UIUserList.h:103/:106` 仅两颗按钮桩，无数据模型/opcode/handler/文案组 | ❌ | 整族缺失（非壳/占位），列 D9 |
| B9 | 公会徽章/标志编辑 | Angel `CWndGuildGrade`/`CWndAllianceGrade` | `WaveAShellHandlers.h:941/:947` 仅 `GuildNameChangedHandler`/`GuildMarkChangedHandler` 声明，PacketSwitch 未见 emplace | ❌ | 整族缺失，列 D9 |
| B10 | MapleTV 完整播放/送礼 | Angel `CUIMapleTV` 1 / `CMapleTVMan` 3 | `UIMapleTV.h:19` 仅 BtOk/BtCancel；`UIMapleTV.cpp:76` "No 512* map effect"；4 handler 只 LOG+开窗 | ❌ | 整族缺失；`SEND_TV` 显式预留「待 REA/IDA 取证后另立 Plan」 |
| B11 | GuildBoss pulley 上行 | Angel `CField_GuildBoss::BasicActionAttack 0x558b45` | `guild-boss-hud/README.md:28`「无服务端脚本消费证据，显式预留」 | ⚠️ | s2c 287 `short y` / 288 `byte state` 已接；上行未接，列 D8 |
| B12 | 商店/仓库/玩家商店列表加长 | Angel `CTrunkDlg` 22 / `CTradingRoomDlg` 17 / `CPersonalShopDlg` 10 | `feature-gap-inventory:160-161`「基础仓库/商店已通；加长/更高列表未见改动；5 壳深度未达 C」 | ⚠️ | 深度未达 C，列 D8 / D10 |
| B13 | BBS 界面完整度 | Angel `CUIGuildBBS` 17 方法 | `pending-live-tests/README.md:202-214`「布局无 WZ layout 节点，真机可微调」 | ⏳ | 列表/读帖/发帖 11 钮已建，完整度未验收 |
| B14 | UIWedding 玩法判定 | Angel `CField_Wedding::OnWeddingProgress 0x58153d` | `UIWedding.cpp:280`「本端不做判定，只发 action=1，服务端必回 enableActions」 | ⚠️ | [玩法裁定] 协议闭环、判定留服务端；列 D10 |

### 3.3 C · 逻辑（14 条）

| 编号 | 对照点 | v83 权威（Angel / 083 文档） | 当前端（checked-in source） | 判定 | 依据 / 处置 |
|---|---|---|---|---|---|
| C1 | 页签 id 非线性映射 | Angel `SetButton` 5 次全隐藏 + 按 `*(this+369)` 单组显示 | `UIUserList.cpp:1131`「UNION 在枚举末尾，走显式映射」+ `:1441-1456 tab_button()` | ✅ | 五页签显隐逻辑与原生同构 |
| C2 | tab index 编号 | Angel `this+369` 连续 0..4 | `UIUserList.h:45-58` `UNION` 排末尾（第 9 个枚举值） | ⚠️ | [形态差异] 非缺陷；用 `tab_button()` 显式映射消化，列 D5 |
| C3 | 三张互斥底显隐 | Angel `SP_2090_USERLIST_BACKGRND` + `SP_2670/2671_TAB_*` | `UIUserList.cpp:1101-1140 change_tab`（好友=bg3 / 家族·联盟=bg2 / 组员·拒绝=bg） | ✅ | `userlist_runtime_nodes.txt:797-801` 三底均 312×389，五张 oracle 帧 meanSAD 量出禁叠底 |
| C4 | MESSENGER 双枚举 | c2s 122 / s2c 313 | `Net/OutPacket.h:197`（注释「入站 PacketSwitch 313」）+ `PacketSwitch.cpp:161/453` | ✅ | v83 标准非对称，无冲突 |
| C5 | BUDDYLIST 非对称 | c2s `BUDDYLIST_MODIFY` / s2c `BUDDYLIST` | `Net/OutPacket.h:236 BUDDYLIST=130`（`:232-235` 注释「63 是漂移」）+ `PacketSwitch.cpp:159/450` | ✅ | `s9-social.md:79-85` 2026-11-02 修正记录 |
| C6 | 家族聊天 / 6 频道回显 | Angel `CUIStatusBar::SetChatType` 三态（跨面） | `UIUserList.cpp:916`「change_target 仍是预留空实现」+ `UIChatBar.h:123` 空 stub + `UIStateGame.cpp:471-482` 6 频道 | ❌ | **跨面共性真缺**（HUD 面同款），列 D1（P0 零风险接线） |
| C7 | NOTIFIER 键 | 083 原生 L 键 notifier 列表 | `UIStateGame.cpp:314-316`「服务端不存在 ⇒ 按门禁 7 不做假窗 / intentionally inert」 | ⊘ | 按门禁显式不做，非漏实现 |
| C8 | `UIMessenger` 095 注释污染 | 依据应为 Angel `CUIMessenger` 26 方法 | `UIMessenger.cpp:136/:159/:168/:237/:278/:290/:410/:486/:537/:549` 10 处 `v95` 注释 + `UIMessenger.h:106-108/:121-122/:126-127/:133` 4 处 v95 IDA 立即数注 | ❌ | **MEMORY.md 基准违规**；处置 = 只换注释依据、不改行为/协议，列 D1 |
| C9 | 2 处独立 095 文档 | 同上 | `messenger-btclose-coords/README.md:16`（`0x7F34C0` 来自 Maplestory95）、`guild-bbs-ui/README.md:20,27`（port-095 `CUIGuildBBS`→c2s 179） | ❌ | 只换注释依据（本端 `BBS_OPERATION=155`），列 D1 |
| C10 | 核心窗口面 095 污染 | Angel `CUIStat::OnCreate 0x8c4c7f` | `UIStatsInfo.cpp:146`「Maplestory95 `0x867b90`」+ `UIItemInventory.h:49-50`/`.cpp:124` + `InventoryHandlers.cpp:56/:71` | ⚠️ | 跨面同类，核心窗口面已列 D5；本面登记不重复处置 |
| C11 | `WEDDING_TALK_MORE(0x8B)` 撞值 | Angel `OnPacket` 无独立 0x8B 分支（走 case 12/13 内嵌） | `wedding-flow/README.md:43-44`「s9 也 0x8B、同 handler，客户端用等价 `WEDDING_TALK=139`，不重复建枚举」 | ✅ | `feature-gap-inventory:346/:493` 已落地，§14 旧账销案 |
| C12 | `WEDDING_GIFT_RESULT(74)` 字段 | Angel `CField_Wedding::OnWeddingCeremonyEnd 0x581b49` | `wedding-flow/README.md:54`「原实现把 int 当 mode 后第一字段，完全错位」→ 重写 0x09/0x0A/0x0B/0x0F/0x0C/0x0E | ✅ | 逐字节修复记录完整 |
| C13 | Q2 清单同文件 6 处冲突 | — | `:175 vs :342`、`:176 vs :343/:606`、`:185 vs :189`、`:486 vs :346/:493`、`:489 vs :606`、`:597 vs guild-boss-hud:8` | ⚠️ | 全按 checked-in source 取值；**交用户裁决后统一回写，不擅改 Q2**，列 D11 |
| C14 | 收包空实现 / 死注册 | Angel 各 `On*Result` 均有错误码分支 | `WaveAShellHandlers.cpp:507 WEDDING_CEREMONY_END` / `:710 MONSTER_CARNIVAL_LEAVE` / `:3551,3565 MTS_OPERATION(348/347)` / `TestingHandlers.cpp:186 FIELD_EFFECT` 仅 deactivate/LOG/return | ⚠️ | s9 无对应后端，属显式空实现；列 D10 |

### 3.4 D · UI（14 条）

| 编号 | 对照点 | v83 权威（Angel / 083 文档） | 当前端（checked-in source） | 判定 | 依据 / 处置 |
|---|---|---|---|---|---|
| D1 | 玩家列表单体窗 | `CUIUserList` 10 方法（OnCreate/Draw/SetButton/CreateAllianceButton…） | `UIUserList.h:27-234` 单体窗五页签 + `UIUserList.cpp:606-1091 button_pressed` | ✅ | 结构同形（单体大窗），非一一拆分类 |
| D2 | 是否 56 类一一对应 | Angel 56 类 / 752 方法 | 当前端 ~30 独立窗 + `UIMiscShells.h` 6 壳 + 5 页签 + 单一 `WaveAShellHandlers.cpp` | ⚠️ | [结构性重写] 非缺陷，「校准≠改回原生」，列 D1 |
| D3 | UIGuild 独立浮窗 | 083 `UIWindow.img` 顶层**无 Guild/Alliance** | `UIGuild.h:37 TYPE=GUILD` + `UIGuild.cpp:39` `UIElement(300,40,300,380)` | ⚠️ | [s9 适配] 保留底栏社区钮入口，主入口已迁第 3 页签；列 D3 |
| D4 | UIParty 底衬+4 钮 | 083 无 `Party.img` 独立窗根 | `UIParty.cpp:39-41` 显式预留邀请/离开/队长/开除；`:49-56` btnpos 4 颗 | ⚠️ | 显式预留（非漏实现），列 D8 |
| D5 | UIBuddyList 仅关闭钮 | 083 `UserList/Friend` 子节点，无 `BuddyList.img` | `UIBuddyList.cpp:42-46`「只有关闭钮能落地」+ `:49 buttons[BtClose]` | ⚠️ | 显式预留；BtAdd 无贴图，列 D8 |
| D6 | UIGuild 4 钮无贴图 | Angel `SP_2144..2153_USERLIST_GUILD_*` 在共用窗内 | `UIGuild.cpp:49`「四颗钮无原生贴图，只能显式预留」 | ⚠️ | 显式预留，列 D8 |
| D7 | UIStorage 3 钮 | 083 真身 `UIWindow.img/Trunk 463x318`（`SP_1822..1825_TRUNK_*`） | `UIStorage.cpp:44`「既无参考截图也无 IDA 证据 ⇒ BtArrange/BtMesoIn/BtMesoOut 显式预留」 | ⚠️ | 节点失配（`UIStorage.cpp:46 ["Storage.img"]` × 083 `:29 Trunk`），列 D7 |
| D8 | 六个 DIY 集合壳 | Angel `CPersonalShopDlg` 10 / `CTradingRoomDlg` 17 / `CMiniRoomBaseDlg` 15 / `CShopDlg` 9 / `CCashTradingRoomDlg` 5 | `UIMiscShells.h:442-509` PersonalShop/MemberShop/TradingRoom/EntrustedShop/CashTradingRoom/ItemSearch 全族 `has_assets + backgrnd + tip_label`，**无 button_pressed / 无 Buttons 枚举** | ⚠️ | `UIMiscShells.cpp:2807-2811` draw 仅底图+一行 tip；列 D8 / D10 |
| D9 | UITrade BtChat | Angel `CTradingRoomDlg` 无独立聊天钮 | `UITrade.h:76`「原生无此钮，仅由快捷键触发」 | ⚠️ | 自造钮但已显式标注，列 D8 |
| D10 | UIGuildBoss 纯 HUD | Angel `CField_GuildBoss` 2 方法驱动地图物件 | `UIGuildBoss.h:15-45`「本窗仅同步 HUD」，全文件无 button_pressed / Buttons | ⚠️ | s2c 287/288 已接，地图物件真驱动在 `MapGuildBoss`；列 D8 |
| D11 | UIMapleTV 壳 | Angel `CUIMapleTV` 1 / `CMapleTVMan` 3 | `UIMapleTV.h:19` 两钮 + `UIMapleTV.cpp:76` 一行 tip | ⚠️ | 4 handler 主体仅 LOG + emplace，列 D9 |
| D12 | 悄悄话目标弹层 | 083 原生「请输入悄悄话的对象。」（`SP_782` 跨面） | `UIMiscShells.h:963-1036 UIWhisperTargetDlg` Notice4 竖拼 + 候选名单 + `:1018 refocus_field()` | ✅ | `UIStateGame.cpp:273-288` H 键接线完整 |
| D13 | `update()` 空实现 | Angel 各 `*Dlg::Update` 均有逻辑 | `UIParty.cpp:98-100` / `UIBuddyList.cpp:84-86` / `UIMerchant.h:43` 三处 `void update(){}` | ⚠️ | 空实现（非崩溃），列 D8 |
| D14 | UIMTS 空 update | Angel `CUIMTS 0x6fb07b..` 有列表行为 | `UIMTS.cpp:86-95` draw 仅 5 行 Text + `:97-99 void UIMTS::update() {}` | ⚠️ | 与 B3 同源，s9 无后端；列 D10 |

### 3.5 E · 排版（16 条）

| 编号 | 对照点 | v83 权威（Angel / 083 文档） | 当前端（checked-in source） | 判定 | 依据 / 处置 |
|---|---|---|---|---|---|
| E1 | 五组控件 id | Angel `2010–2020`(好友)/`2200–2208`(组员)/`2030–2041`(公会)/`2050+`(联盟)/`2150+`(拒绝) | `UIUserList.h:83-144 Buttons` 五组 + `tab_button() :1441-1456` | ✅ | 分组逐点吻合；`r01 §3` 逐字坐标表 |
| E2 | 页签条坐标 | Angel ctrl 2001 `(a4:1,a5:6,a6:23,a7:270)` | `UIUserList.cpp:88-110 TAB_POS[5]={(4,23),(35,22),(66,22),(96,23),(149,22)}` | ✅ | 正确端截图 oracle 实测（比 IDA 立即数更贴实机），保留不回改 |
| E3 | 列表行高 | Angel `a3: 18*vN + 43/572/110/116/…`，x=281/312 | `UIBuddyList.h:79 ROW_H=24` / `UIUserList.h:51 ROW_H=18` / `RT_NODE` 行高 18 | ⚠️ | [口径差异] 混合 18/24，需逐页 oracle 量测，列 D6 |
| E4 | 三张互斥底 312×389 | Angel `SP_2090` + `SetButton` 显隐 | `userlist_runtime_nodes.txt:797/:799/:801 backgrnd/2/3 均 312x389 origin(0,0)` + `change_tab` 归属 | ✅ | 五张 oracle 帧 meanSAD：好友=bg3(11.59)/联盟=bg2(7.61)/拒绝=bg1(11.07) |
| E5 | 公会页 12 按钮落位 | Angel BtInvite(9,316)/BtKick(159,336)/BtGuildBBS(80,363)… | `UIUserList.h:110-121` 12 钮 + `UIUserList.cpp:813-926` 路由 | ⚠️ | `diagnosis-and-plan.md:105-106` `layout.py mark/bias` 逐格未跑，列 D6 / D13 |
| E6 | 权限显隐 | Angel `DestroyGuildButton(0/2/3)` / `(1/4/5)` 分级销毁 | 当前端未见按职位分级销毁的对表 | ⚠️ | `r01:168-169`；列 D6 |
| E7 | 好友/拒绝容量 | Angel `CTabFriend` 12 / `CBlackListDlg` 3 可定上限 | `UIUserList.cpp:1416 friend_count "(0/25)"`（源码注释另记硬编码 0/50）/ `:1423 blacklist "0/26"` | ⚠️ | `diagnosis-and-plan.md:73-74` 容量口径冲突，列 D6 |
| E8 | GuildUnion 12 按钮语义 | Angel `CreateAllianceButton 0x91eba3` 动态 id | `diagnosis-and-plan.md:170-174`「`BtMate`/`BtMod`/`BtMessage`/`BtGroupWhisper` 对应关系**无原生证据**，暂统一落包」 | ⚠️ | 按节点名推断，需实机逐钮确认；列 D6 |
| E9 | Messenger 关闭钮 | Angel `CUIMessenger::SetCtrl 0x8505e2` | `UIMessenger.h:109-110 BT_CLOSE_X=276,Y=6` + `close_btn()` 12×12 回退 | ⚠️ | **坐标对、依据错**（当前注释引 095 `0x7F34C0`），列 D1（只换依据） |
| E10 | Messenger 立绘/名牌 | Angel `SP_1858_MESSENGER_BLINK` / `SP_1860_CHATBALLOON` / `SP_1867_NAMEBAR_D` | `UIMessenger.h:71-76 NAMEBAR_COUNT=4/SLOT_COUNT=4`（按绘制面收敛）/ `:115 LOOK_SCALE=0.70f` / `:123-124 BALLOON_DX=-31,DY=-70` | ✅ | `pending-live-tests:18` 双角色 0.70/foot -10 已实机 |
| E11 | UIStorage 尺寸 | 083 `UIWindow.img/Trunk 463x318` | `UIStorage.cpp:38 UIElement(Point(200,20), Point(348,360))` + `:46 ["Storage.img"]` | ⚠️ | [节点失配] 无参考截图；列 D7 / D13 |
| E12 | 五页签计数硬编码 | Angel 运行时 `Decode1 charcount` 填槽 | `UIUserList.cpp:1416-1428` 全 "(0/0)"/"0/26"/"[0/0]"/"none" 空态初值 | ⚠️ | `UIUserList.h:233`「行数据待真机联调」，列 D6 / D13 |
| E13 | 窗位置持久化 | Angel `CUIWnd::CreateUIWndPosSaved` | `Configuration.h:703-962` 14 项社交 Pos*；**UIParty/UIBuddyList/UIGuild/UITrade/UIMerchant/UIWedding/UIAlliance/UIStorage 无 `ui.position.*` 项**（硬编码构造坐标） | ⚠️ | 列 D13（低优先补键，不动坐标） |
| E14 | W5–W9 逐窗定案 | 跨面共用 `ui-v83-window-restore/README.md:68-79` | 快捷格 151/466/571 + 格 28/列距 35/行距 34/首格 (9,9)；能力窗 `208×347`；背包 tab `x=5+34i,y=24`；技能 5 tab | ✅ | 核心窗口面 r05 §9.1 已逐值复核 |
| E15 | GuildBBS 底板 | 083 `GuildBBS.img` | `guild-bbs-ui/README.md:17 backgrnd 734×526 / backgrnd2,3 327×288` | ✅ | `UIGuildBBS.h:95-98 LIST_ROW_H=22/LIST_MAX_ROWS=12/BTN_Y_LIST=488` |
| E16 | 联盟按钮动态 id | Angel `a3: 2*a2+2085` / `2*a2+2086` | `UIUserList.h:123 BT_TAB_UNION` 静态 + `tab_button()` 显式映射 | ⚠️ | [形态差异] 非缺陷；列 D5 |

### 3.6 F · 样式（12 条）

| 编号 | 对照点 | v83 权威（Angel / 083 文档） | 当前端（checked-in source） | 判定 | 依据 / 处置 |
|---|---|---|---|---|---|
| F1 | `SP_*` ↔ 件表同批 | 347 个 `SP_`（84 个 `UI_UIWINDOWIMG_*`） | `UIWindow.img.top-level.txt` 112 顶层节点（`:49 UserList`/`:90 Family`/`:91 FamilyTree`/`:59 Wedding`/`:19 TradingRoom`/`:29 Trunk`…） | ✅ | `asset-node-inventory.md`；排版层已对齐，Angel 只补符号语义 |
| F2 | 聊天气泡九切片 | `SP_1388..1425_CHATBALLOONIMG_*`（NW/N/NE/E/W/C/SW/S/SE/ARROW/B/BA/CNUM/MNUM/PROGRESS/OMOK/MINIROOM） | `IO/Components/MapleFrame.cpp:26-40` 九切片 + `NameTag.h:29-50` 三切片 | ✅ | 组件层与原生同构 |
| F3 | UserList 节点 | `SP_2090_USERLIST_BACKGRND` / `SP_2670/2671_TAB_ENABLED/DISABLED` / `SP_2097..2166_*_BT*` | `userlist_runtime_nodes.txt:774-796 Tab/enabled|disabled/0..4`、`:3 BlackList :28 Friend :240 Guild :527 GuildUnion :653 Party` | ✅ | `node_ref_audit.txt:3`「158 节点 / 48 引用 / 25 失配」另列 D7 |
| F4 | Messenger 节点 | `SP_1858..1868_MESSENGER_*` + `SP_800/801/802` 帮助文案 | `UIMessenger.h:85-134` 版式常量 + `UIMessenger.cpp:416-644` i18n | ✅ | 资源引用对齐；v95 注释污染另计 C8 |
| F5 | 商店/仓库节点 | `SP_1822..1825_TRUNK_*` / `SP_1818_TRADINGROOM_BTTRADE` / `SP_1542..1548_SHOP_*` | `Util/UiAssetResolve.cpp:42-55 window()` / `:105-113 shop_root()` 回退链 | ✅ | `SP_1253_UI_BASICIMG_ITEMNO`/`SP_1260/1261/1262_BTCLAIM/BTMIN/BTMAX` 同批 |
| F6 | WorldMap 边框（跨面） | Angel `SP_2281..2288_WORLDMAP_BORDER_7..0`（正好 8 片）+ `SP_2289_TITLE` | `UIWorldMap.cpp:60` diag "Border missing" 疑走 MapleFrame 替代 | ✅ | 核心窗口面 r03 §7 已三证互证 |
| F7 | 通用组件常量 | Angel `CCtrl*` / `CConfig` 字体色 | `MapleButton.h:28-45` 四态 / `Charset.cpp:30-43` Lbracket→'[' / `Gauge.h:27-34` CLIP / `Graphics/Text.h:33-48` 12 字号档 | ✅ | 与前三面同一套组件层 |
| F8 | 婚礼文案 | 083 String.img 英文原值 | `locale/zh-CN.ini:299-325 wedding.* 26 键全英文` | ⚠️ | NO-NATIVE 决策 B「保持英文原值、只建键」；列 D12 |
| F9 | 壳窗批次文案 | 同上 | `zh-CN.ini:327-332`「zh 侧先落英文、不编中文译文」+ `:480-483` 134 键/261 站点 | ⚠️ | 列 D12（保持，等 083 依据） |
| F10 | en-US 中文硬编码 | — | `en-US.ini:198-214 buddylist.title=好友 ({})/common.channel=频道{}/party.title=队伍 #{}/trade.*` 与 zh-CN 同值 | ⚠️ | [文档/样式口径] 未英译，列 D12 |
| F11 | UserList NO-NATIVE | Angel `SP_782` 原生「请输入悄悄话的对象」 | `zh-CN.ini:179-187 userlist.none=none/cur_location=My Location - {}/friend_group=Default Group` | ⚠️ | 英文 stub；列 D12 |
| F12 | 三处 stub 文案 | — | `zh-CN.ini:344-348 cashshop.stub_job=Illium/stub_name=ShomeiZekkou/charinfo.medal_stub=Junior Adventurer` | ⚠️ | 占位文案；列 D12 |

### 3.7 G · 交互（14 条）

| 编号 | 对照点 | v83 权威（Angel / 083 文档） | 当前端（checked-in source） | 判定 | 依据 / 处置 |
|---|---|---|---|---|---|
| G1 | 五死键接线 | 083 F(26 FAMILY)/G(17 GUILD)/H(11 WHISPER)/O(25 PARTYSEARCH)/L(NOTIFIER) | `UIStateGame.cpp:273-318/:296-309/:356-393` F→UIFamily / G→Tab::GUILD / H→UIWhisperTargetDlg / O→UIPartySearch / L intentionally inert | ✅ | `pending-live-tests:45` §#35 五死键 exe 已部署待复拍（G14） |
| G2 | 6 频道快捷键 | Angel `CUIStatusBar::SetChatType` 三态（HUD 面） | `UIStateGame.cpp:471-482 SAY/PARTYCHAT/FRIENDSCHAT/GUILDCHAT/ALLIANCECHAT/TOSPOUSE → change_target(action)` 落点 `UIChatBar.h:123` 空 stub | ❌ | **跨面共性死链**；列 D1（P0 零风险接线） |
| G3 | MAPLECHAT 开窗 | Angel `CUIMessenger::OnCreate` 建房包 | `UIStateGame.cpp:403-425` 优先 `emplace<UIMessenger>()` + `try_create_room()`，无资源回退 `socialChatEnter/UIChat` | ✅ | 双通道降级链完整 |
| G4 | OPEN_UI 总路由 | Angel 各 `On*Result` 内 `ChangeStep`/开窗 | `WaveAShellHandlers.cpp:1244-1447 OpenUIHandler::handle` 大 switch 15 窗逐 id emplace | ✅ | 含多窗并存；`UIGuild/UIFamily/UIParty/UIBuddyList/UIMessenger/UIStorage/UIDelivery/UIPersonalShop/UIPartySearch/UIFamilyTree/UIMemo/UIAlliance/UIWedding/UIStoreBank` |
| G5 | 家族页 12 钮交互 | Angel 2030–2041 12 按钮 OnChildNotify | `UIUserList.cpp:813-926`（`:914 BT_GUILD_CHAT`/`:926 BT_GUILD_BBS`/`:1091 BT_GUILD_BOARD`） | ✅ | 与 `GuildModel` 共读共写 |
| G6 | 好友 Messenger / 家族聊天回显 | Angel `CUIMessenger` 邀请链 + `CUIStatusBar::SendGroupMessage` | `UIUserList.cpp:650 BT_FRIEND_MESSENGER` 有；`:916`「家族聊天由斜杠别名路由，change_target 仍是预留空实现」 | ❌ | 本地回显整段缺；列 D1 |
| G7 | 空白地图右键 | Angel `CUIContextMenu` 1 | `UIStateGame.cpp:216-229 cm->open_at(pos,0,get_name())` + `UIMiscShells.h:729-749 UIContextMenu{BtInfo,BtWhisper,BtClose}` | ✅ | 三态上下文菜单 |
| G8 | 悄悄话弹层交互 | 083 原生 `SP_782` | `UIMiscShells.h:983` 打开即刷新候选（队友+好友在线）+ 聚焦输入框 + `:1018 refocus_field()` | ✅ | `UIStateGame.cpp:275-276` 回调 `ChatBar::set_whisper_target` |
| G9 | NPC 商店交互 | Angel `CShopDlg` 9 / `CAdminShopDlg` 14 | `UIShop.cpp:180-240 button_pressed`（BUY_ITEM/SELL_ITEM/EXIT/CHECKBOX/EQUIP/USE/ETC/SETUP/CASH） | ✅ | `rightclicksell` / `show_weekly` 配置项 `Configuration.h:156-163` |
| G10 | BBS 交互 | Angel `CUIGuildBBS` 17（List/Open/Post/Reply/…） | `UIGuildBBS.cpp:471-516 button_pressed` 11 钮 + `Page{List,View,Write}` | ✅ | c2s 155 mode 2/3/0/4 + s2c 0x06/0x07 |
| G11 | UIParty SoftKey | Angel `CTabParty` 10 | `UIParty.cpp:151-187` L/E/I/C/X/J/S/P/R 全套 + `Party*Packet` | ✅ | 4 钮虽预留贴图，SoftKey 行为链完整 |
| G12 | 面交握手 | Angel `CTradingRoomDlg` 17 | `UIMiscShells.cpp:2963-2979` C/T→`TradeConfirmPacket`；`:2976-2977`「双方各发一次 CONFIRM(0x11)，无 0x14 分支」 | ✅ | s9 handler 无 TRANSACTION(0x14) 分支，显式对齐 |
| G13 | 083 键位 / 095 剔除 | Angel 083 无 `Academy`/`ExpeditionIntermediary` 类 | `UIStateGame.cpp:374-377` BOSSPARTY 并入 GUILD 页签；`social-loop-audit:20,22,70-75` Academy/BossParty 独立面板判「不做」 | ⊘ | 非缺口，按 MEMORY.md 基准裁决 |
| G14 | 五页签实机验收 | Angel `SetButton` 显隐 + `Draw` 5 分支 | `diagnosis-and-plan.md:105-106,138` `layout.py mark/bias/check` 未跑；`pending-live-tests:67-70` 四类全局阻塞（GM 注入/角色前置/第二角色/并行会话） | ⏳ | 需用户屏前复拍 5 页 + 抓 `packet_dump.log`；列 D13 |

### 3.8 判定汇总（python 行级复核）

| 维 | 条数 | ✅ | ⚠️ | ❌ | ⊘ | ⏳ |
|---|---|---|---|---|---|---|
| A 功能 | 14 | 13 | 1 | 0 | 0 | 0 |
| B 业务 | 14 | 0 | 5 | 6 | 1 | 2 |
| C 逻辑 | 14 | 6 | 4 | 3 | 1 | 0 |
| D UI | 14 | 2 | 12 | 0 | 0 | 0 |
| E 排版 | 16 | 6 | 10 | 0 | 0 | 0 |
| F 样式 | 12 | 7 | 5 | 0 | 0 | 0 |
| G 交互 | 14 | 10 | 0 | 2 | 1 | 1 |
| **合计** | **98** | **44** | **37** | **11** | **3** | **3** |

⚠️37 里：12 条 s9 适配须裁决、9 条形态/映射差异、6 条显式预留/DIY 壳、5 条文档未同步/冲突、5 条样式/口径冲突。
❌11 收敛 6 类：① `change_target()` 空 stub 死链（C6/G2/G6 跨面）；② 095 注释污染（C8/C9）；③ `PARTY_SEARCH_REGISTER(220)` / MTS 0xFD / 每日签到（B1/B3/B4）；④ 整族缺失（B8/B9/B10）；⑤ 文档基准违规；⑥ 无依据空态。
**判定分布与 §0 结论 5 一致（§0/§3.8 双处均由 python 行级解析复核）**。

---

## 4. Angel 补上的 10 条关键新发现

1. **`CUIUserList` 单体窗 5 页签铁证**：`Draw @ 0x91bec3` `v8=*(this+369)` 5 层 if → tab 0..4；`SetButton` 5 次全隐藏再按页签单组显示；控件 id 五组 `2010–2020`/`2200–2208`/`2030–2041`/`2050+`/`2150+`。→ 钉死「G 键家族=第 3 页签」不是推断而是 IDA 逐字。
2. **公会页 12 按钮逐字坐标**：BtInvite(9,316)/BtKick(159,336)/BtWhere(159,316)/BtWhisper(238,316)/BtNotice(287,80)/BtInfo(238,363)/BtGuildBBS(80,363)…；`DestroyGuildButton(0/2/3)`/`(1/4/5)` 按职位分级销毁（当前端未对表）。
3. **`CreateAllianceButton` 动态 id** `a3: 2*a2+2085` / `2*a2+2086` → 联盟页按钮是运行时生成，当前端 `BT_TAB_UNION` 静态映射属形态差异非缺陷。
4. **`CUIMessenger::OnCreate` 逐字 `COutPacket(122)+Encode1(0)+Encode4(a2)+SendPacket`** + `SP_800/801/802` 帮助文案；`OnButtonClicked` 5 case（1000 TryDelete / 1001+1002 SetState(v2%3) / 1003 ProcessChat / 1004 SendClaim）；`OnPacket` case 0/1/2/4/5/6/7/8。→ v95 注释可整体换引 Angel。
5. **`SP_*` 347 个（84 个 `UI_UIWINDOWIMG_*`）** 与 `UIWindow.img.top-level.txt` 112 顶层节点是同批节点两种表示；`SP_2090`/`SP_2670/2671`/`SP_2097..2166` 与 `userlist_runtime_nodes.txt:774-801` 逐点吻合。
6. **752/752 EA 全同**（0 缺失）比前三面（327/328、547/548、439/441）更硬：社交面全覆盖，Angel 与 BeiDou **完全同一份代码**，无覆盖差异。
7. **083 顶层无 Guild/Alliance/Academy/BossParty/GuildBBS/TradeHouse 独立窗** → 三重互证 MEMORY.md 术语（家族=UserList 子节点、学院=独立 Family 顶窗、095 面板 083 侧不存在）。
8. **`CChatBalloon`/`CChatHelper` 跨面共性**：`CChatHelper::TryChat 0x4904be` 的脏词/刷屏/重复句规则（HUD 面已记）在本面 `CUIMessenger::OnChat` 同样适用，当前端整族未实现。
9. **`CField_Wedding` 4 方法 / `CField_GuildBoss::BasicActionAttack 0x558b45`** 给婚礼 s2c 错位修正与 GuildBoss pulley 上行提供可 decompile 的 083 一手地址。
10. **`CUIPartySearch::StartSearch 0xa12073` + `TabPartySearch` 7 方法** 为 `222/223` 字段冲突提供裁决锚点（须抓帧 + 读 s9 服务端后定）。

---

## 5. 与上一轮 / Q2 清单的关系（四态）

### 5.1 销案 ✅
- **Wave A 11 项全 ❌**（`r16:188-203`：Messenger/PartySearch/Delivery/StoreBank/ViciousHammer/VegaSpell/CashGachapon/RemoteGachapon/Family/FamilyTree/Memo）→ `r810:78` T14 `f5af924` 一次补齐 20 壳 + handler + 出站包（54 path / social 33 / commerce 15）+ `r40:5` 15 壳 + `parity-gap:107` + 30+ 源码类，全部存在。
- **G 键第 3 页签 / F 键学院窗 / 5 页签 / 学院≠家族 / MESSENGER 122 / 学院 9 包 / 婚礼双端** 七项全 ✅（r02 §1–§5 逐字销案）。
- **`TemporaryStatView 全缺` / `channel_count<2` / `[DBG-CLK]`**（HUD/登录面旧账）在本面无同型硬缺陷；`guild-boss-hud 287/288 无 handler` 由 `PacketSwitch.cpp:662-663` 销案。
- **Q2 §6 D1 好友增删** 由 `BuddyPackets.h:29-50` + `UIBuddyList.cpp:94..283` 销案。

### 5.2 推翻 ⚠️
- 「社交经济缺窗口 / 缺出站封包 / 缺 handler」→ 30+ 窗类 / 88 发包类 / ~40 emplace / 148 c2s 全有构造点（Q2 §9.5 已覆写，本面复核一致）。
- 「`UIGuild` 浮窗是错误」→ 判 [s9 适配] 双入口保留（主入口已迁第 3 页签），不回改。
- 「Messenger 关闭钮 `(276,6)` 来自 095」→ 坐标对、依据错，换引 Angel 即可，不动像素。
- 「`userlist-five-tabs/README.md` 存在」→ glob + read 双证**不存在**（目录唯一文档 `notes/diagnosis-and-plan.md`），属文档漂移。

### 5.3 新增 ❌/⚠️
- `UIMessenger.cpp` 10 处 + `.h` 4 处 v95 注释污染（比核心窗口面 `UIStatsInfo.cpp:146` 范围更大）。
- 2 处独立 095 文档：`messenger-btclose-coords/README.md:16`、`guild-bbs-ui/README.md:20,27`。
- Q2 清单**同文件 6 处自相矛盾**（`:175/:176/:185/:486/:489/:597`）→ 列 D11 交用户裁决后统一回写。
- `222/223` 字段冲突（`r23` vs `s9-social.md`）、`0/25 vs 0/50` 容量冲突、`SP_`/件表 25 个失配名（`node_ref_audit.txt:3` 52%）。

### 5.4 遗留 ⏳
- 五页签 oracle `layout.py mark/bias/check` 未跑（`diagnosis-and-plan.md:105-106,138,164-166`）；婚礼七步 / 联盟补完 / FM CREATE / BBS 完整窗 / GuildBoss 实机（`pending-live-tests` #32–#36）；四类全局阻塞未解。
- 7 个 >50KB 超大函数（`CUIUserList::OnCreate` 59169B / `Draw` 35089B 等）已 `/output/` 补全但**未逐行人工核完**；277 函数是精选（非全部 752）。

### 5.5 四面一致性
EA 交叉 327/328 → 547/548 → 439/441 → 752/752 四面钉死；拆分模式四面一致（单体窗 → 多窗/组件/壳）；`change_target` 死链、脏词整族缺失、`校准 ≠ 改回原生`、s9 红线、IMG 阻塞四面一致。

---

## 6. 决策点 D1–D14（每条带推荐默认值）

| 决策点 | 问题 | 推荐默认 | 依据/风险 |
|---|---|---|---|
| **D1** | `change_target()` 空 stub + 10+4 处 v95 注释污染是否本轮清理？ | **做**：零风险先接线 `UIChatBar::change_target`（本地回显 6 频道）+ 只换注释依据引 Angel `CUIMessenger`/`CUIFamily`，**不改行为/协议/坐标** | C6/C8/C9/G2/G6；P0 |
| **D2** | `PARTY_SEARCH_START(222)` / `UPDATE(223)` 字段以哪套为准？ | **先抓帧 + 读 s9 服务端 `addCharStats` 再定**，不盲改 | B2 / §2.4 #1；改错断组队搜索 |
| **D3** | 联盟双入口（页签 + `UIAlliance` 浮窗）/ `UIGuild` 浮窗是否合并回原生？ | **保持现状**（083 原生无独立窗，但 s9 已用双入口）；合并风险极高 | A9/D3；`校准 ≠ 改回原生` |
| **D4** | `PARTY_SEARCH_REGISTER(220)` / MTS 0xFD / 每日签到是否立项补？ | **保持 s9 联机优先**：220 用 222/223 代理、MTS/ITC 维持「仅壳」、每日签到维持 NO-NATIVE 假态 | B1/B3/B4/B5；改回原生会断联机 |
| **D5** | `UNION` 排枚举末尾 / 动态 id 是否收敛为 Angel 连续 0..4？ | **保持 `tab_button()` 显式映射**（非缺陷） | C2/E16；改编号牵动 `NUM_TYPES` 须全量 Rebuild |
| **D6** | 五页签落位/容量/权限显隐是否按 Angel 逐格回改？ | **先跑 5 张 oracle 帧 `layout.py` 量测**，只在 Δ>2px 处微调；`TAB_POS` 是截图实测，比 IDA 立即数更贴实机 | E3/E5/E6/E7/E8/E12；改回原生会破坏已对齐排版 |
| **D7** | `UIStorage ["Storage.img"]` × 083 `:29 Trunk` / 25 个节点名失配是否改节点路径？ | **先出「代码字面量 ↔ 运行时节点」对照清单**，只改有运行时 dump 佐证的；无证据的保持显式预留 | D7/E11/r03 §0；`node_ref_audit.txt:3` 52% 失配 |
| **D8** | UIParty 4 钮 / UIBuddyList / UIGuild 4 钮 / UIStorage 3 钮 / 六 DIY 壳 / pulley 上行 / 列表加长是否补深度？ | **先补零资源依赖的行为**（SoftKey 已有）；需要 IMG 件的列 P2，不凭空造贴图 | D4–D10/B6/B11/B12；门禁「不发明客户端能力」 |
| **D9** | `UIFriendGroup` / 公会徽章编辑 / MapleTV 完整 UI 三个「整族缺失」是否立项？ | **保持明确不做/搁置**：083 顶层清单无独立窗、s9 无后端；MapleTV 待 REA/IDA 取证后另立 Plan | B8/B9/B10/D11/G13；`Academy`/`BossParty` 同理判不做 |
| **D10** | MTS / ITC 拍卖、UIWedding 玩法判定、s2c 空实现是否补？ | **保持**：s9 无拍卖后端；UIWedding 判定留服务端；空实现属显式预留 | B3/B14/C14/D14；列 §14 ⊘ 总表 |
| **D11** | Q2 清单 6 处自相矛盾 + 095 文档污染 + `TemporaryStatView 全缺` 旧账是否回写？ | **交用户裁决后统一回写**（本面只出销案证据，不擅改 Q2 / 登录面 / 核心窗口面文档） | C10/C13/§5.3；与核心窗口面 D11 同型 |
| **D12** | `wedding.*` 26 键全英文 / en-US 中文硬编码 / stub 文案是否译？ | **保持决策 B「保持英文原值、只建键」**，等 083 String.img / IDA 依据再换 | F8–F12；不臆造译文 |
| **D13** | 8 个社交窗缺 `ui.position.*` 项 / 五页签实机验收是否补？ | **补键为低优先（不动硬编码坐标）**；实机验收需用户屏前复拍，本环境历史抓屏全黑不假装已验证 | E13/G14；`diagnosis-and-plan.md:138` |
| **D14** | 是否接 orange-wz(IMG) MCP / 是否进入实现？ | **不接**（Host 未注册 + 已知解析缺陷，替代链已够）；**先完成 P0 零风险 + 全量 Rebuild + 五屏实机比对**，原子提交须用户授权 | §1.2 / §7；全程只读取证 |

**最关键 D1 + D3 + D4 + D6 共同决定「校准 ≠ 改回原生」**（与登录面 D1、HUD 面 D1/D6、核心窗口面 D1/D2/D9/D10 四面一致）。

---

## 7. 一次性 Plan

### P0 · 零风险纯客户端 / 文档（可直接做，不依赖裁决）
1. D1 前半：接线 `IO/UITypes/UIChatBar.h:123 change_target(int32_t)` + `UIUserList.cpp:916` 家族聊天本地回显（不改协议）。
2. D1 后半：把 `UIMessenger.cpp` 10 处 + `.h` 4 处 + `messenger-btclose-coords/README.md:16` + `guild-bbs-ui/README.md:20,27` 的 095 依据统一换引 Angel `CUIMessenger(26)` / `CUIUserList(10)` / `CUIGuildBBS(17)` / `CUIFamily(3)`，**坐标与协议一字不动**。
3. D11 前半：生成本面「Q2 旧账销案证据表」（6 处冲突 + Wave A + 公会副本 HUD），供用户裁决。
4. D12：登记 `locale` NO-NATIVE 台账（不译文、不删键）。
5. 门禁复核：`python tools/gen_opcode_matrix.py --check` 必须 GATE OK。

### P1 · 需裁决（等 D2/D4/D8/D9/D10/D11 回复）
6. D2：抓 `PARTY_SEARCH_START/UPDATE` 真机包 + 读 s9 服务端写包，定 222/223 字段后只改客户端发包（或标注不改）。
7. D4/D9/D10：确认 220 / MTS / 每日签到 / 三个整族缺失维持「不做或代理」。
8. D8：从六 DIY 壳里挑 1–2 个有 083 EA 的（`CPersonalShopDlg` 10 / `CTradingRoomDlg` 17）补行为深度，不造贴图。
9. D11 后半：用户裁决后统一回写 Q2 / 登录面 / 核心窗口面 / HUD 面四份 README 旧账。
10. D7：出「代码字面量 ↔ 运行时节点」25 个失配名的对照清单，只改有 dump 佐证的。

### P2 · 需 IMG · 实机（等 D6/D13/D14）
11. D6：屏前复拍 5 页签 oracle 帧，`layout.py mark/bias` 逐格量测，只在 Δ>2px 处微调。
12. D13：补 8 个社交窗 `ui.position.*` 键（不动坐标）；逐窗按 G/OPEN_UI 复拍。
13. **验证门禁**：单测 + `tools/build_smoke.bat` **全量 Rebuild**（`IO/UIElement.h:195-199` 警示 `NUM_TYPES` 变更须整工程重建，增量编译会留旧尺寸 obj）+ 1280×720 五屏实机比对 + 抓 `packet_dump.log`；原子提交须用户授权。

### P3 · goal 续作（按同一流水线）
14. 下一功能面「**宠物召唤龙**」→ 再「**活动小游戏**」：复用 5 个采集脚本（`tools/ida_angel_social_*.py` 改关键词/路径即成）+ 三路只读取证（当前端源码盘点 / 既有文档四态 / IMG 资源样式）+ 六维对照 98 条 + 14 决策点 + 一次性 Plan 模板。goal `goal-11388c42-2502-43f0-b722-70abeb8d4307` 保持 active/armed。

---

## 8. 限制 / 未知

- **IMG 层阻塞**（四面一致）：orange-wz MCP 未接入本 DSH Host；替代链 = 仓库 `.nx` 节点引用 + `tools/nx_inspect.py` 运行时 dump + 正确端截图库 78 张 + `userlist_runtime_nodes.txt:1-802`。本面 IMG 证据比前三面充分（6 个 raw-nx-dumps + 五页签 oracle 帧 + 112 顶层节点清单），但**仓库内无 `*_runtime.json` 整窗 dump / 社交专用 `layout*.json` / Photoshop manifest**。
- `read_image` 对 PNG 返回 `model "hy4free" does not declare image input`，**未做像素级视觉复核**；正确端截图仅按文件尺寸/文件名/oracle meanSAD 引用。
- 5 个 >50KB 超大函数（`CUIUserList::OnCreate 59169B` / `Draw 35089B` 等）经 `/output/` 补全但**未逐行人工核完**；277 反编译函数是精选（非全部 752）。
- 实机全部 ⏳：四类全局阻塞（GM 注入 `IO/UI.cpp:264-269` 吞 `!`/空格、角色前置 lv1/job0/ap0、需第二角色/公会/FM/商城额度、并行会话）+ 历史抓屏全黑。
- 三路 subagent 均声明全程只读、未改文件、未连库、未改 MCP 配置、未构建、未实机、未提交。
- 性质：本文是**只读取证 + 对照 + 决策点 + Plan**，不是实施记录；未改任何产品源码。

---

## 9. 证据索引与复现命令

```
# 端点（端口会变，先读 idb_path）
python tools/ida_instance_probe.py            # 13339 Angel / 13337 BeiDou server_health + survey + py_eval

# 社交经济面 5 个采集脚本（顺序可复算）
python tools/ida_angel_social_ea.py            # -> evidence/angel-social-ea.json (752/56)
python tools/ida_angel_social_decompile.py     # -> evidence/angel-social-decompile.json (277)
python tools/ida_angel_social_normalize.py     # -> angel-social-decompile-full.json (5 大函数 /output/ 补全) + truncated receipt
python tools/ida_angel_social_crosscheck.py    # -> angel-beidou-social-ea-crosscheck.json (752/752)
python tools/ida_angel_social_facts.py         # -> angel-social-facts.md (4716 行 / SP_ 347)
# decompile 脚本支持覆盖参数：[Class | Class::Method | 0xEA ...]

# 门禁（必须 GATE OK）
python tools/gen_opcode_matrix.py --check      # GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)

# 构建 / 实机（P2，须授权后执行）
tools\build_smoke.bat                          # 全量 Rebuild -> E:\MXD\BeiDou-Client-xy\MapleStory.exe
```

| 类别 | 文件 |
|---|---|
| notes | `r01-angel-native-social-facts.md` / `r02-main-agent-crosscheck-cancel.md` / `r03-social-img-style-evidence.md` / `r04-current-social-source-inventory.md` / `r05-prior-social-doc-audit.md` |
| evidence | `angel-social-ea.json` / `angel-social-decompile.json` / `angel-social-decompile-full.json` / `angel-social-truncated.json` / `angel-social-facts.md` / `angel-beidou-social-ea-crosscheck.json` |
| 前三面 | `docs/features/login-flow-angel-calibration-2026-11/README.md` / `statusbar-hud-angel-calibration-2026-11/README.md` / `corewindow-angel-calibration-2026-11/README.md` |
| 083 权威件表 | `docs/features/v83-hud-quest/evidence/UIWindow.img.top-level.txt` / `docs/features/userlist-five-tabs/evidence/userlist_runtime_nodes.txt` / `C:\Users\11036\Desktop\正确客户端截图\` |
| 现场重算 | `docs/features/parity-gap-2026-11/README.md` / `social-loop-audit-2026-11/README.md` / `feature-gap-inventory-2026-11/README.md` |

---

## 10. 性质标注

- **观察** = 文档明写判定 / 源码逐字行 / IDA 逐字伪代码 / 运行时 dump；**推断** = 跨文档去重、大类归并、冲突裁决、优先级排序；**待实机** = ⏳ 需屏前复拍或双角色。
- 取值优先级见 §2.2；083 vs s9 红线与术语硬约束见 §2.3；七处冲突取值见 §2.4。
- 对照总表 98 条判定由 python 行级解析复核（§3.8），与 §0 结论 5 双处一致；手算汇总易错，一律以逐表实测为准（HUD 面 / 核心窗口面同款教训）。
- **全程只读取证**；未改产品源码、未构建、未实机、未提交、未创建 git commit；原子提交须用户显式授权。
- goal `goal-11388c42-2502-43f0-b722-70abeb8d4307` 保持 active/armed：登录面 + HUD 面 + 核心窗口面 + 社交经济面四面已闭环交付，后续按 §7 P3 续作「宠物召唤龙 → 活动小游戏」。
