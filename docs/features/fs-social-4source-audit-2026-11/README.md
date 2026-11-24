# 四源对照只读审计 · 社交经济域（2026-11-24）

> 子代理 fs-social 交付。审计对象：HEAD `a364931d307baf5a3236d4982ae1f5121db71fc9`。
> 域：仓库/交易/商店/公会/公会徽章/联盟/组队/组队搜索/好友分组/家族/家族树/信使/便签/快递/任务经济。
> **全程严格只读**：未改任何产品源码、未编译、未发包、未真机。真机未验逐条声明。

## 0. 四源取证状态（全部实际查证）

| 源 | 连接方式 | 实测加载 | 状态 |
|---|---|---|---|
| Angel v83 | HTTP `http://127.0.0.1:13337/mcp`（`tools/_fs_ida.py`/`_mcp_call.py`） | `server_health` = `E:\MXD\反编译IDA\v83\`Angel.i64``, module `` `Angel.exe ``, imagebase 0x400000, strings 1281, hexrays_ready=true | ✅ 具名函数可 decompile（`CUIUserList::OnCreate@0x919b73` 等） |
| 095 | HTTP `http://127.0.0.1:13338/mcp` | `E:\MXD\反编译IDA\v095\Maplestory95.i64`, module `Maplestory95.exe`, strings 2583 | ✅ 可查（含 Friend_Group/Guild 排序等符号） |
| 192 | HTTP `http://127.0.0.1:13339/mcp` | `E:\MXD\反编译IDA\MapleStoryT.i64`, module `MapleStoryT.exe`, strings 14409 | ✅ 可查（EmoticonInfo@CUIMessenger、BossName/PartyCandidate@TabPartySearch） |
| MinMS | 文件 `E:\project\MinMS\src\client\IO\UITypes\` | Storage/Trade/UserList/Party/Shop/QuestLog 等 | ✅ 源文件逐行可读 |
| 资源三端 | orange-wz HTTP `:10012`（29 工具，GBK 描述）+ 本端运行时 `nx_inspect.py` | 本端 `E:\MXD\BeiDou-Client-xy\Data\.nxcache\UI.nx`（243,718,957 B） | ✅ 实测节点（见 §2） |

**端口口径变更（重要）**：2026-11-08 `social-economy-angel-calibration` 文档记 13337=BeiDou / 13338=095 / 13339=Angel；本轮实测已被重排为 **13337=Angel / 13338=095 / 13339=192(MapleStoryT)**，以 `server_health.idb_path` 为准。原文档硬约束「禁引 095」指功能参照基准仍固定 083，本轮按要求补 095/192 只为高版本内容对照，不作基准改判。

**避重**：已读 `social-economy-angel-calibration-2026-11/README.md`、`social-loop-audit-2026-11/README.md`、`native-full-deviation-audit-2026-11-23/ROUND2-README.md`。上轮 commit `9ef96287` 已修仓库/交易/便签/快递发包函数体（`git show --stat 9ef96287` 证实 UIDelivery/UIMemo/UIStorage/UITrade 改动），本表**不重复报这些已修函数体**，只报结构/资源/其它未修面。

## 1. 偏差表

格式：功能项 | 判定 | 当前端 file:line | 四源证据 | 严重度 | 说明

| 功能项 | 判定 | 当前端 file:line | 四源证据 | 严重度 | 说明 |
|---|---|---|---|---|---|
| 仓库底板/件表节点 | ❌ 真缺(资源失配) | `IO/UITypes/UIStorage.cpp:47` `nl::nx::UI["Storage.img"]`；`:48-49` backgrnd；`:51-59` 三钮 | Angel `CTrunkDlg` 22 方法（`:7c8cd7` ctor / `:7c76fd` Draw 463×318 / `:7c6c84` OnButtonClicked 1000-1004）；本端运行时 `nx_inspect.py list UI.nx 1 Storage.img` = **NOT FOUND**；`463x318` 只命中 `UIWindow.img/Trunk/backgrnd`；`ui-v83-window-restore/notes/asset-node-inventory.md` 早已注明「Storage.img→UIWindow.img/Trunk」。MinMS `UIStorage.cpp:45` 走另一套渲染 | **P1** | 当前包根本没有 `Storage.img` 根，底板+3 按钮 sprite 全空，仓库窗只剩文字与共用关闭钮。上轮修的是存取/金币发包函数体（`:249/:277`），**未修此加载根**。属资源引用真缺，非行为已修项。真机未验。 |
| 交易 BtEnter 缺位 | ⚠️ 已显式兜底 | `IO/UITypes/UITrade.cpp:73-81` fallback 到 Messenger/Trade BtClaim | 本端 `UIWindow.img/TradingRoom` 实测节点 = BtCancel/BtCoin/BtReset/BtTrade/backgrnd(565×474)/icon1，**无 BtEnter**；Angel `CTradingRoomDlg` 17 方法无独立 BtEnter 节点引用（:7c2a71 OnButtonClicked） | P3 | 代码已用 Messenger 同款 BtEnter 兜底且注释到位，非崩坏，仅登记资源口径差异。真机未验。 |
| 家族树图形化/成员牌 | ⚠️ 功能降级 | `IO/UITypes/UIFamilyTree.cpp:59-93` set_chart/draw 把成员画成扁平文本行（`:88-91` 最多 12 行 ×18px）；`:31` `window("FamilyTree")` | 本端 `UIWindow.img/FamilyTree` 实测有 `PlateLeader`、`PlateOthers`、`selected(139×40)`、`BtJuniorEntry/BtLeft/BtRight` —— **当前端 draw 完全未用 PlateLeader/PlateOthers/selected 三个原生家族树牌位资源**。Angel `CUIFamilyChart` 8 方法（`:80ad31 _DrawChartItem`/`:80acd3 _FindItemByPoint`/`:80acd3` 图形点选），095 `:7b0d50 ZRef<FamilyItem@CUIFamilyChart>`，192 `:1233380` 同族 —— 四源一致指向真实树状图 + 图元点选 | **P2** | 当前端是"文字清单代替原版家谱图"的降级实现：资源在位却不用，点选走 `rel.y/ROW_H` 文本行命中（`:103`）而非原生 `_FindItemByPoint` 图元。属功能性形态偏差。真机未验。 |
| 组队搜索 Pause/Stop 合并 | ⚠️ 语义差异 | `IO/UITypes/UIPartySearch.cpp:193-197` `case BtStop/BtPause` 共用 `stop_search()`；`:118-129` stop_search 只发 PartySearchStop；`:169-173` 'R'/'J' 空分支 | Angel `TabPartySearch::OnButtonClicked@0x87918d`：6000→StartSearch / **6001→HoldSearch** / 6002→StopSearch，三态互斥；`HoldSearch@0x879364` 只 `SendCancelPartyWanted` 且置内部状态 2（保留注册）；`StopSearch@0x8793a1` 还调 `ClosePartySearch_Remocon` + Clear + SendCancel。当前端无 Hold 态、无 223 暂停语义、R/J 键空实现 | **P2** | "暂停"在当前端等同"停止"（都发 223 注销），与原版"保留报名的暂挂"不同；s9 侧若 223=HOLD 则行为不符。R/J 两个软键为空分支（死键）。真机未验。 |
| 信使 slash 指令 /q /invite | ⚠️ 原生有当前端无 | `IO/UITypes/UIMessenger.cpp:430-444` 仅 UI 按钮 `prompt_invite()`；全文件无 slash 解析 | Angel `CUIMessenger::HandleSlashCommand@0x851ae2`：识别 `SP_1868/1869`（"q"/"qq"）→ TryDelete，`SP_145` "invite" + `slashcmd_check_with_subcmd` 邀请；`SendInviteMsg@0x851c2e` 提示输入名字。当前端仅靠 UI 按钮发 `MessengerInvitePacket`，无聊天框 `/q` `/invite` 文本指令入口 | P2 | 原生 Messenger 支持聊天内斜杠指令退群/邀请；当前端无此交互层。属交互能力缺口，非协议缺（invite/leave 包都在）。真机未验。 |
| 信使表情(Emoticon) | ⚠️ 高版本特性未移植 | `IO/UITypes/UIMessenger.*` + `Net/Packets/MessengerPackets.h` 全无 emoticon 字段 | 192 IDB：`UEmoticonInfo@CUIMessenger`（`:1416f50/:1416f40/:141a200`）、`NormalChatTextInfo@CUIMessenger`（`:1416f70`）大量表情/普通聊天分离结构；Angel v83 与 095 `CUIMessenger` 无 EmoticonInfo → **192 专有增强** | P3 | 属 192 新增社交表达层，本端以 083 为基准；仅作"高版本内容"如实登记，不判本端缺陷。真机未验。 |
| 组队搜索 Boss/Candidate 增强 | ⚠️ 高版本特性未移植 | `IO/UITypes/UIPartySearch.cpp:22-31` 仅 min/max level+jobs | 192 IDB：`PartyCandidate`/`PartyCandidateMember` + `BossNameAscComp/DescComp`、`AvgLevel*Comp`、`MemberNo*Comp`（`:14e4e30-14e5a90`），即按 Boss 名/平均等级/人数排序的候选列表；Angel/095 无此族 | P3 | 192 高版本组队搜索 UI 增强，本端 083 基准不要求；登记供高版本移植决策。 |
| 公会徽章/纹章编辑 | ✅ 已修(不重复报) | `IO/UITypes/UIGuild.cpp:329-349` `prompt_emblem()` / `GuildChangeEmblemPacket` | Angel `CWndGuildGrade`；s9 GuildOperationHandler case 0x0f；ROUND2-README §1 已翻案"补客户端发包即可" | — | 上轮已修，本轮不重复。 |
| 仓库/交易/便签/快递发包 | ✅ 已修(不重复报) | commit `9ef96287` 改 UIDelivery/UIMemo/UIStorage/UITrade | 见 `git show --stat 9ef96287` | — | 上轮已修，本轮不重复。 |
| 家族聊天 change_target | ❌ 仍死链(跨面) | `IO/UITypes/UIChatBar.cpp:1997-2030` change_target 有实现但 `IO/UITypes/UIUserList.cpp:1134` 注释仍自承"预留空实现"；`IO/UIStateGame.cpp:658` 有调用点 | social-economy README C6/G2 + social-loop-audit §二 均记该跨面死链；Angel `CUIStatusBar::SetChatType` 三态 | P2 | 本轮复核发现 change_target 已有函数体且 UIStateGame 已调用（较旧文档"空 stub"有推进），但 UserList 注释未同步、家族聊天按钮实际回显仍存疑。需真机确认 `/g` 与 6 频道回显。真机未验。 |

## 2. 资源真缺三端复核（orange-wz / nx_inspect 实证）

| 节点 | 本端运行时 UI.nx | Angel v83 客户端 | 095/192 客户端 | 判定 |
|---|---|---|---|---|
| `Storage.img`（仓库） | **NOT FOUND**；真身 `UIWindow.img/Trunk/backgrnd` 463×318 + BtGet/BtGetAll/BtSort/BtInCoin/BtOutCoin/BtCoin/BtExit/Tab | Angel `CTrunkDlg` 对应 5 操作（取物/存物/排序/取钱/存钱） | 095 IDB 有 `CTrunkDlg`/`UITEM@CTrunkDlg`；192 有 `CITEM@CTrunkDlg` 同族 | 真缺（当前端引用错根）；1 处 P1 |
| `TradingRoom.img`（交易） | `UIWindow.img/TradingRoom` 在位，backgrnd 565×474 + 4 原生钮，无 BtEnter | Angel `CTradingRoomDlg` 17 方法 | 095/192 同类在册 | 在位；BtEnter 已兜底，P3 |
| `GuildMark.img` | `GuildMark.img` 在位（BackGround/Mark）；`UIUserList.cpp:1793` `GI["guildmark"]` 消费 | Angel 公会徽章编辑 | 095/192 在册 | 在位，非缺 |
| `FamilyTree.img` | `UIWindow.img/FamilyTree` 在位（backgrnd 578×386 + PlateLeader/PlateOthers/selected + 4 Bt），当前端 draw 未用成员牌 | Angel `CUIFamilyChart` 图元点选 | 095/192 `ZRef<FamilyItem@CUIFamilyChart>` 同族 | 资源在位、使用降级，P2 |

> orange-wz 会话本轮 `list_loaded_roots` 为空（未加载任何包）；按 AGENTS.md 约束，本次资源判定先以 orange-wz 探活（29 工具在线、10012 HTTP 400/200）+ 本端运行时 UI.nx 的 `nx_inspect.py` 逐节点取证为准，未做离线 WZ 明文假设。三端客户端 XML dump / 192 `UI_*.wz` 原始包未逐 img 展开（下表声明）。

## 3. 高版本内容（095/192）闭环结论

- 095 相对 083：好友分组仍是 `CTabFriend`/`CUIFriendGroup` 体系（`:8badc0 construct@CUIFriendGroup`），公会列表新增多级排序器（Name/Job/Level/Grade × Asc/Desc，`:8bd650-8d4520`）与 `Friend_GroupAscComp`（`:8d3450`）。当前端好友分组本地筛选已在 `UIUserList.cpp:899-902/1168-1181` 闭环，独立浮窗仍需双端（旧账，非本轮新偏差）。
- 192 相对 083/095：Messenger 增 `EmoticonInfo` + `NormalChatTextInfo` 分离表情/普通聊天；TabPartySearch 增 Boss/Candidate/AvgLevel/MemberNo 排序；GuildAlliance 项由 `GUILDITEM` 更名/扩展为 `GUILDALLIANCEITEM`。均属高版本客户端闭环增强，本端以 083 为基准不移植，仅登记。

## 4. 声明

1. 全程只读：未改产品源码、未编译、未发包；产物仅本 `docs/features/fs-social-4source-audit-2026-11/` 下报告与 evidence、以及 `tools/_fs_ida.py`/`_fs_batch.py` 两个只读 MCP 客户端脚本。
2. **IDA 取证成功**：三个 IDB 端口均实际连上并 decompile；无 IDA 失败项需记录。
3. **orange-wz 部分取证**：服务在线，`list_loaded_roots` 本会话为空，未逐 img 展开三端离线 WZ；资源判定以本端运行时 UI.nx 为主证，离线三端 WZ 逐节点展开列为未完成项。
4. **真机未验**：以上所有判定均为静态代码/IDA/NX 对照，未做屏前验收、未抓 `packet_dump.log`，按用户偏好如实声明。
5. 截图未采信（来源端未确认）。
