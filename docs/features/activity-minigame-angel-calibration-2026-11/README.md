# 活动·小游戏 — Angel 083 反编译全维校准（2026-11）

> 第六功能面（goal round 4，§7 P3 计划最后一面）。以 Angel(13339) 083/v83 带符号 IDB 为权威校准源，对当前端 `MapleStory-Client` 的迷你游戏房间 / 活动场地 / 活动 UI / 投掷物 / 脚本问答系统逐条校验「功能 / 业务 / 逻辑 / UI / 排版 / 样式 / 交互」六维的正确性、缺失、一致性。
> 前五面（登录 → 状态栏 HUD → 核心窗口 → 社交经济 → 宠物召唤龙）已按同一方法闭环，本文沿用其口径与证据优先级。
> 基准固定 **083/v83**；全程只读取证，未改任何产品源码、未构建、未实机、未提交。

## §0 五条结论

1. **Angel 与 BeiDou 是同一份 v83 二进制的不同符号状态 —— 六面字节级一致**：本面 `angel-beidou-activity-ea-crosscheck.json` 机械报 `844/841 / missing 3 / "layout differs"`，但双端 `get_wide_byte` 逐字比对 3 个 EA（`0x462738 CBookDlg::OnKey` / `0x5617a1 CField_MassacreResult::OnPacket` / `0x6cec47 CNoticeQuestProgress::Update`）8 字节全同、BeiDou 侧仅 `get_func_attr` 返回 `0xffffffffffffffff`（自动分析未定义为函数）→ 结论是 **IDA 分析覆盖差异，非二进制布局差异**，`tools/ida_angel_activity_crosscheck.py:107` 的 `same = present >= len(items) - 2` 阈值造成机械误判。正解见 `evidence/angel-beidou-activity-ea-bytelevel-audit.json`：**844/844 by bytes**（登录 327/328、HUD 547/548、核心窗口 439/441、社交 752/752、宠物 968/968、活动 844/844）。Angel 只补符号语义层（267 函数逐字 Hex-Rays + SP_ 238），**不补功能、不是版本更新**。[观察]
2. **协议面分方向闭环，且「校准 ≠ 改回原生」**：Angel 267 函数内 `COutPacket::COutPacket(a1: N)` 全集仅 **8 个 c2s**（60 脚本应答 / 107 QUEST_ACTION / 113 MAKER_SKILL / 123 PLAYER_INTERACTION / 212 雪球 / 213 椰子 / 215 公会Boss / 218 嘉年华）。当前端落 **4 个**（60 同值异名 `NPC_TALK_MORE`、107、113、123），整族零枚举 **4 个**（212/213/215/218）——这 4 个是 **s9 适配冲突**（`s9-protocol-align/README.md:45`「opcode/协议以 s9 为准」，改回原生会断联机，须交用户裁决，不得盲改），非「漏写」。s2c 侧 `Net/PacketSwitch.cpp` 281–295 活动族 + 10 处 `WaveAShellHandlers.cpp` handler 全注册、matrix 全 `HANDLED`，`148/148 client_out` 全 `MATCHED`。[观察+推断]
3. **结构性差异 = 原生独立类树 → 当前端 DIY 合并壳**：Angel `CMiniRoomBaseDlg`(15)/`COmokDlg`/`CMemoryGameDlg`/`CRPSGameDlg` + 13 个 `CField_*` 活动场地 + `CUIMonsterCarnival`(16)/`CUIItemMaker`(23)/`CUIQuestInfo`(15) 是独立对象；当前端 `CMiniRoom*`/12 个 `CField_*` 源码 **0 命中**（唯一 `MapGuildBoss.cpp:19`「本端用时长近似」真对象），小游戏合并进 `IO/UITypes/UIMinigame.h:2`「wire later」+ `UIMiscShells.cpp` `board[15][15]` DIY 壳，嘉年华/椰子/雪球退 `FieldMarker.cpp:19` 黄字。全库**无独立 UIPet 式活动窗** → **不是一一对应，也不全是功能缺失**，是架构重写 + 壳占位。[观察+推断]
4. **真缺口三层 + 三处新增硬冲突**：① 整族缺（Omok 胜负/禁手状态机、MemoryGame 牌面 `if(game_kind==1)` 才画、活动场地对象族、三处 c2s 冲突值、case 296、332 Stamp、每日签到仅 UI 假态、XyLottery/MTS 双端皆无、Quiz 304 子类型）；② 壳占位（RPS 7 按钮薄壳 + `while(recv.available()) recv.skip_byte()`、CUIMonsterCarnival 16→4 setter HUD、295 LEAVE 空壳、locale 英文 stub、ColorBox 降级）；③ 文档一致性（`IO/UIStateGame.cpp:532` + `IO/UITypes/UISpecial.h:9` 本面 2 处 095 注释污染、Q2 `feature-gap` F1「客户端零命中」措辞须限定 c2s、`s9-protocol-align/README.md:10,11,32,60` 旧读数、`codebuddy-baseline.txt:173,193-204` 旧基线）。**★ 三处新增「Angel↔当前端」硬冲突**：212 命名/语义（SNOWBALL 发包 vs `wave4-battle-packets/README.md:22` LEFT_KNOCKBACK）、Carnival case 296 缺口（Angel 289–296 vs 当前端只到 295）、299/300 归属（Angel OnScoreUpdate/OnTeamChanged vs matrix SHEEP_RANCH_*）——全部列 D 决策点交裁决，不擅改。[观察]
5. **判定分布（§3 共 98 条）**：✅29 / ⚠️51 / ❌11 / ⊘2 / ⏳5。**⚠️ 偏多正是本面特征**：与前五面「协议/渲染真实现」不同，活动小游戏整族是 DIY 合并壳 / 降级占位 / s9 适配冲突，绝大多数校准点属「壳在、语义简化、待裁决」。IMG 层 orange-wz 阻塞（六面一致）+ 正确端截图库活动窗本体 0 → 排版/样式维 ⏳/⚠️ 属「待资源」而非「代码错误」。s9 适配冲突一律标注「**改回原生会断联机，须交用户裁决，不得盲改**」，与五面红线一致。[观察+推断]

## §1 取证基线与三联门禁

### §1.1 IDA 端点（每轮先 server_health）

| 端口 | IDB | module | strings | 角色 |
|---|---|---|---|---|
| 13339 | `E:\MXD\反编译IDA\v83\`Angel.i64` | `` `Angel.exe `` | 1281 | 083/v83 带符号校准源（本面权威） |
| 13337 | `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64` | `BeiDou.exe` | 3889 | 083/v83 无符号排版基准 / EA 交叉验证 |
| 13338 | headless 095 | — | — | **不用**（禁引 095 线作 083 依据） |

端点 uptime 复核：13339 = 79508.649s（`auto_analysis_ready:true` / `hexrays_ready:true` / imagebase `0x400000`）、13337 = 728379.662s，均 healthy。`/health` 路径 404（错路径），改走 MCP `/mcp` initialize + `server_health`（`tools/ida_instance_probe.py:122-127`）。`Angel.exe` 是 IDB root filename（`ida_mcp.py:317-327`），磁盘无本体。

### §1.2 REA → IDA → orange-wz(IMG) 三联门禁

| 层 | 状态 | 说明 |
|---|---|---|
| REA（当前端源码） | ✅ | `xiaoye-maple-story-dev` + `reverse-engineer-anything` 已加载；checked-in source 逐字盘点（r04） |
| IDA（Angel 083 + BeiDou 交叉） | ✅ | 5 脚本串跑全 exit 0；267 函数逐字 Hex-Rays；844 函数 EA + 字节级交叉 |
| orange-wz(IMG) | ⚠️ **阻塞（六面一致）** | 本 DSH Host `mcp_connector_status` 仅 `json-photoshop`；`mcp_connector_catalog(keyword=orange)`=0；`C:\Users\11036\.cursor\mcp.json:3-6` 仅声明未注册 Host；`codebuddy-transcript-947f8ba0.md:37` 已知 `Index 16 out of bounds for length 4` + `list_children` 空 |

IMG 阻塞替代链 = 仓库 `.nx` 节点字面量 + `tools/nx_inspect.py` dump（`UIWindow.img.top-level.txt` 112 顶层节点 / `slidemenu.txt` 22 行 / `raw-nx-dumps/` 9 文件）+ 正确端截图库 `C:\Users\11036\Desktop\正确客户端截图\`（78 唯一文件，**活动窗本体 0**）。详见 r03 §7。

### §1.3 5 个采集脚本 + 7 evidence + 5 notes

脚本由 `tools/_gen_activity.py` 从第五面 5 模板生成（`common()` 替换 pet-summon→activity-minigame / `angel-petsummon-`→`angel-activity-` / PETSUMMON→ACTIVITY），`py_compile` 全 SYNTAX OK，`EXACT` 宽集 73 类 / `TARGETS` 53 类 / `FOCUS` 53 类：

| 脚本 | 产物 | 结果 |
|---|---|---|
| `tools/ida_angel_activity_ea.py` | `evidence/angel-activity-ea.json` | 844 具名函数 / 73 类 |
| `tools/ida_angel_activity_decompile.py` | `evidence/angel-activity-decompile.json` | 267/267 OK |
| `tools/ida_angel_activity_normalize.py` | `angel-activity-decompile-full.json` / `-truncated.json` | total 267 / trunc_fixed 3 / empty 1（后补全） |
| `tools/ida_angel_activity_facts.py` | `evidence/angel-activity-facts.md` | 3987 行 / 189016 B / SP_ 238 个（UI_UIWINDOWIMG 54） |
| `tools/ida_angel_activity_crosscheck.py` | `evidence/angel-beidou-activity-ea-crosscheck.json` | 844/841 / 3 missing / 阈值机械误判（见 §1.4） |

**★ §1.4 本面两个独有技术点（前五面从未出现）**

1. **3 missing EA 字节级根因**（正解 `evidence/angel-beidou-activity-ea-bytelevel-audit.json`，2884 B）：`0x462738 ?OnKey@CBookDlg@@UAEXII@Z` / `0x5617a1 ?OnPacket@CField_MassacreResult@@UAEXJAAVCInPacket@@@Z` / `0x6cec47 ?Update@CNoticeQuestProgress@@UAEXXZ`。双端 `py_eval`（全部逻辑包进 `def _scan():` 规避 `NameError: name 'json' is not defined` 双作用域陷阱）：Angel 侧都是带修饰名的函数头，BeiDou 侧 `get_func_attr` = `0xffffffffffffffff` 但 8 字节逐字相同 `[246,68,36,11,128,117,25,131]` / `[129,124,36,4,158,0,0,0]` / `[86,139,241,232,47,26,49,0]`。→ **纯 IDA 分析覆盖差异**；crosscheck 的 `"layout differs"` 是 `same = present >= len(items) - 2` 阈值误判，README 结论更正为「844/844 字节级一致」。审计文件 `six_face_consistency` 逐字「login 327/328, HUD 547/548, corewindow 439/441, social 752/752, petsummon 968/968, activity 844/844 by bytes」。
2. **CGrenade::JustBomb decompile 失败 → disasm 兜底**：`@ 0x58fbfc` decompile 返回 `{"addr":"0x58fbfc","code":null,"error":"Decompilation failed at 0x58fbfc (address: 0x58fc17)"}`，重试仍失败；`tools/list` 发现 `disasm`（schema `addr`/`max_instructions`/`offset`/`include_total`），取 12 指令写回 `decompile-full.json`（`keys=267 empty=0`）：
```
push esi / mov esi,ecx / push [esi+44h]
mov ecx,dword_BE78DC / call ?GetSkill@CSkillInfo@@QAEPBUSKILLENTRY@@J@Z(0x75c755)
mov ecx,[esi+10h] / push esi / push 320h / push eax
call ?ShowSkillSpecialEffect@CUser@@QAEXPBUSKILLENTRY@@JPAVCGrenade@@JJJ@Z(0x936528)
pop esi / retn
```
语义 = `CUser::ShowSkillSpecialEffect(CSkillInfo::GetSkill(dword_BE78DC, this+0x44), 0x320, this, ...)`。重跑 facts → 3987 行 / 189016 B。

notes：`notes/r01-angel-native-activity-facts.md`（Angel 一手语义）、`r02-main-agent-crosscheck-cancel.md`（主 agent 逐字销案）、`r03-activity-img-style-evidence.md`（IMG/样式）、`r04-current-activity-source-inventory.md`（源码盘点 + §0.1 校正）、`r05-prior-activity-doc-audit.md`（既有文档四态 + 常量协议表 + 15 复核点）。

## §2 口径

### §2.1 六维 × 五判定

- 六维：**A 功能 / B 业务 / C 逻辑 / D UI / E 排版 / F 样式 / G 交互**（§3 共 98 条：14/14/14/14/16/12/14）。
- 判定：**✅ 一致且正确**（Angel 语义 ↔ 当前端 source 销案）；**⚠️ 部分 / 降级 / 待裁决**（壳在、语义简化、s9 适配冲突）；**❌ 缺失 / 错误**（整族缺或显式预留）；**⊘ 结构性不做**（095 专有 / 结构性不接）；**⏳ 待实机 / 待资源 / 待展开帧**。

### §2.2 证据优先级（沿用五面）

`运行时行为 > 抓包 > 活跃资产 > 当前进程配置 > checked-in source > 生成证据 > 注释/死码`。协议层以 `docs/features/s9-protocol-align/README.md:45`「1. **opcode/协议以 s9 为准（R3）**」+ `feature-gap-inventory-2026-11/README.md:63` 优先级为准：`opcode-matrix.json`（`generated_at=2026-11-08T00:56:27` / `s9_git_head=3d5ccc6f8e`）> 源码 `.dispatch()` > 生成 evidence > `s9-protocol-align/notes/gap-inventory.md:3` 旧 head `44d0d5d9ca` / `client_out_sent=103`（作废）。**★ `MATCHED-HANDLED` 态不存在**：matrix 全库 `304 HANDLED / 146 MATCHED / 9 DEFINED_NOT_EMITTED / 1 CLIENT_ONLY / 1 S9_DUP_VALUE`，0 处 `UNHANDLED_EMITTED`、0 处 `MATCHED-HANDLED`（grep 实证）→ c2s 用 `MATCHED`、s2c 用 `HANDLED` **必须分列**。

### §2.3 083 vs s9 红线 + MEMORY.md 术语

- 基准固定 **083/v83**；禁引 095 线符号/IDB（`Maplestory95.i64`、port-095 映射、`_OnPacket_CSummonedPool...` 095 反编译、ExpeditionIntermediary、`docs/features/port-095/` 全目录、`c4-btsort-gather-semantics/README.md:10`「锚定 GMS v95 符号库」）作 083 依据。注：`angel-activity-facts.md:2594` 的 `lpSystemTime: &v95` 是 Hex-Rays 局部变量名、`:2571` 的 1095 是偏移常量，均误报排除。
- 083 术语：F 键 action26 = FAMILY =「学院」窗（UIFamily）；G 键 = GUILD =「家族」= 公会（UIUserList 第 3 页签）。判「不做」的只有 095 专有的 v95 导师 Academy 与远征 BossParty 独立面板。
- **改回原生会断 s9 联机**：`login-flow:26-27` / `corewindow:286` / `social-economy:309-310` / `pet-summon` 五面红线，本面续用 → 212/213/215/218 冲突项一律「交用户裁决，不得盲改」。

### §2.4 本面 8 处冲突取值表

| 冲突 | 旧账 | 本面取值依据 | 结论 |
|---|---|---|---|
| EA 一致性结论 | `angel-beidou-activity-ea-crosscheck.json` `"layout differs"` | `angel-beidou-activity-ea-bytelevel-audit.json` 3 EA 8 字节逐字相同 + BeiDou `get_func_attr=0xffffffffffffffff` | ✅ 844/844 字节级一致，阈值误判 |
| c2s 60 是否缺失 | r04 子代理「60 脚本问答 0 命中」 | `Net/OutPacket.h:100 NPC_TALK_MORE = 60,` + `UIMonsterCarnival.cpp:97-137`/`UIPartyRace.cpp:51-70` `NpcTalkMorePacket(tip).dispatch()` | ✅ 实存·同值异名·语义复用，非整族缺失 |
| c2s 8 锚点落点 | 「只落 3 个」 | `Net/OutPacket.h` 60/107/113/123 全在 | 落 4 / 零枚举 4（212/213/215/218 属 s9 适配冲突） |
| F1「客户端零命中」 | `feature-gap:493` / `parity-gap:83` | `Net/PacketSwitch.cpp:185-190,314-315,379-383` + matrix 281–295 全 HANDLED + `UIWindow.img.top-level.txt:64` MonsterCarnival | 措辞须限定 **c2s 方向**；裁决本身维持 |
| 212 命名/语义 | `wave4-battle-packets/README.md:22` LEFT_KNOCKBACK(212) 不做 | `angel-activity-facts.md:1098` `CField_SnowBall::Update COutPacket(212)` | ⚠️ 硬冲突 → D5（交裁决） |
| Carnival case 296 | `Net/PacketSwitch.cpp:185-189,314-315` 只到 295 | `angel-activity-facts.md:973-986` case 289–296（296=OnShowGameResult） | ⚠️ 真缺 → D5（交裁决） |
| 299/300 归属 | matrix `SHEEP_RANCH_INFO/SHEEP_RANCH_CLOTHES` | `angel-activity-facts.md:606-613` `CField_Battlefield` OnScoreUpdate/OnTeamChanged | ⚠️ 硬冲突 → D5（交裁决） |
| 协议 status 读数 | `s9-protocol-align/README.md:10,11,32,60` 103/299-3/UNHANDLED_EMITTED=3 | `opcode-matrix.json` 304 HANDLED / 146 MATCHED / 148/148 client_out | c2s=MATCHED / s2c=HANDLED 分列，旧读数作废 |

## §3 六维对照总表（98 条）

> 每行：`# | 校准点 | Angel 083 一手语义（r01） | 当前端 checked-in source | 判定 | 处置 / 决策`。行号均为仓库相对路径。

### A 功能（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| A01 | 小游戏房间基类 CMiniRoomBaseDlg | `OnPacketBase` 子命令 3/4/6/9/10 + `dword_BE7910` 单例（15 方法） | `IO/UITypes/UIMinigame.h:2`「wire later」+ `Net/Packets/PlayerInteractionPackets.h` 6 构造点 | ⚠️ | DIY 单壳，弱接线 → D3 |
| A02 | 五子棋 Omok 落子/棋盘 | `COmokDlg::OnKey` 仅 Enter13/Esc27 + `CheckAndSendChat` | `UIMiscShells.cpp:7367 board[15][15]` + `:7424 MinigameOmokPlacePacket(x,y,next_piece)` | ⚠️ | 有棋盘贴图，无胜负/禁手状态机 → D3 |
| A03 | 记忆翻牌 MemoryGame | `CMemoryGameDlg::OnKey` 与 Omok 逐字同一份 | `UIMiscShells.cpp:7367 if(game_kind==1)` 才画棋盘，==2 牌面完全不渲染 + `:7412 MinigameMatchCardSelectPacket(0,slot)` | ❌ | 纯数据占位 → D3 |
| A04 | 猜拳 RPS | `CRPSGameDlg::OnPacket` v1<6 return / <=7 / ==8 抛 `CDisconnectException 0x21000003` + `Alloc(1768)` CUtilDlgEx | `Net/Packets/RpsPackets.h:2`(136) + `WaveAShellHandlers.cpp:716-759` 全降级状态文本 + `while(recv.available()) recv.skip_byte()` | ⚠️ | 7 按钮薄壳 → D3 |
| A05 | 怪物嘉年华 CP/召唤 UI | `CUIMonsterCarnival`(16)/`CField_MonsterCarnival`(9) case 289–296 | `Net/PacketSwitch.cpp:185-189,314-315`(289–295) + `WaveAShellHandlers.cpp:619,646,661,681` 4 setter HUD 壳 | ⚠️ | 壳在，case 296 缺 → D5 |
| A06 | 椰子/雪球/战场活动场地 | `CField_Coconut`(6)/`CField_SnowBall`/`CField_Battlefield`(8)/`CField_Dojang`/`Massacre`/`Witchtower`/`SpaceGAGA`/`ContiMove`/`PartyRaid*` | 12 个 `CField_*` 源码 0 命中；`WaveAShellHandlers.cpp:1169-1178,3019-3020` 借 `FieldMarker::Kind::SUMMON` 挂黄字 | ❌ | 整族缺 → D4 |
| A07 | 公会 PQ Boss 场地 | `CField_GuildBoss::BasicActionAttack COutPacket(215)` | `Gameplay/MapleMap/MapGuildBoss.h:3-5`(IDA sub_5587B3/558B0F/558C0C/558C3B) + `.cpp:19`「本端用时长近似」唯一真对象 | ✅ | 语义对应（时长近似简化），销案 |
| A08 | 远程扭蛋 USE_REMOTE(116) | `SP_5066/5067` 现金扭蛋族 | `Net/Packets/ItemAClassPackets.h:65` .dispatch() + matrix c2s 116 MATCHED（旧账 `parity-gap:56`「发不出」作废） | ✅ | 协议闭环，销案 |
| A09 | 道具制作 Maker(113) | `CUIItemMaker::RequestItemMake` 11 个 Encode4/Encode1 | `Net/Packets/MakerPackets.h:30-40` 仅 `write_int(1)`+`write_int(toCreate)`+装备类占位（s9 简化） | ⚠️ | 字段序不同，s9 适配 → D6 |
| A10 | 新年卡发送 | `CUINewYearCardSenderDlg`(10) 控件 0–5 + `GW_NewYearCardRecord::Decode` | `Net/OutPacket.h:278`(159) + `Net/PacketSwitch.cpp:194`(118) + `UIWindow.img:81 NewYearsCard` 壳 | ⚠️ | 壳在，深度待裁决 → D4 |
| A11 | 现金扭蛋结果 | `CUICashItemGachapon::OnCashItemGachaponResult` Decode4/Decode1/Decode1 + 2500ms 动画 | `Net/PacketSwitch.cpp:180-181`(333/334) + `:464-465` handler + `UIWindow.img:97 CashGachapon` | ⚠️ | 壳在，动画简化 → D4 |
| A12 | 扭蛋券 Stamp(332) | 083 原生 result 通道 | matrix s2c 332 `CASHSHOP_GACHAPON_STAMP_RESULT` status=`DEFINED_NOT_EMITTED` / client_names=[] | ❌ | 客户端零 handler → D9 |
| A13 | 脚本问答 Quiz/SpeedQuiz/SlideMenu | `CScriptMan`(17) OnAskQuiz/OnAskSpeedQuiz/OnAskSlideMenu + `OnPacket if(a2==304)` | `IO/UIElement.h:103-106` AskPet(9)/AskPetAll(10)/SlideMenu(14) 自带 IDA 锚点（7474A2/74775C/76B3F0） | ✅ | action code 一一互证，销案 |
| A14 | 每日签到/XyLottery/MTS | 083 无 XyLottery 双端 | `UIDailyGift.cpp` 615 行仅 UI 假态；`feature-gap:184` D10 XyLottery 双端皆无；`Net/OutPacket.h:291` 仅 ENTER_MTS=156 | ❌ | 维持不做（裁决性）→ D9 |

### B 业务（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| B01 | MiniRoom 子命令 6 CHAT(123) | `CMiniRoomBaseDlg::CheckAndSendChat COutPacket(123)+Encode1(6)+EncodeStr+SendPacket` | `Net/Packets/PlayerInteractionPackets.h:42,55,71,85,97,106` 6 构造点 | ✅ | 销案 |
| B02 | 入场错误码 22 分支 | `OnEnterResultStatic` case 1..24 + `SP_394..406` / `SP_5567_THE_PASSWORD_IS_INCORRECT` 等 20 串 | 当前端 tip 文案未建错误码表 | ❌ | 整族缺 → D9 |
| B03 | 邀请/SSN2 包形 | `OnCheckSSN2Static COutPacket(123)+Encode1(4)+Encode4+Encode1(0)+Encode1(v11)`；`SendCashInviteResult` `Encode1(3)/Encode1(14)/Encode1(4)/Encode1(6)` | `Net/OutPacket.h:261-263` 逐字「6 个构造点（交易/雇佣商人/迷你游戏）此前全部进不了服务端」已修 | ✅ | 销案 |
| B04 | MiniRoomFactory 6 类映射 | case 1..6 → `COmokDlg`/`CMemoryGameDlg`/`CTradingRoomDlg`/`CPersonalShopDlg`/`CEntrustedShopDlg`… | `UIMinigame` 单壳只覆盖 Omok/MatchCard（交易/雇员/个人商店在社交面） | ⚠️ | 非一一对应，架构重写 → D3 |
| B05 | c2s 60 脚本应答 | `CScriptMan::OnSay/OnAsk* COutPacket(60)`（action 0/2/3/4/7/8/9/10/13/14） | `Net/OutPacket.h:100 NPC_TALK_MORE = 60,` 实存 + `UIMonsterCarnival.cpp:97-137`/`UIPartyRace.cpp:51-70` `NpcTalkMorePacket(tip).dispatch()` | ✅ | 校正 r04 子代理误判：同值异名·语义复用 |
| B06 | c2s 107 QUEST_ACTION | `CQuest::StartQuest COutPacket(107)` | `Net/OutPacket.h:193 QUEST_ACTION = 107` + `CQuest` | ✅ | 销案 |
| B07 | c2s 113 MAKER_SKILL | `CUIItemMaker::RequestItemMake` 11 字段 | `Net/OutPacket.h:282` + `MakerPackets.h:30-40` 简化 2 int | ⚠️ | s9 适配字段序 → D6 |
| B08 | c2s 123 PLAYER_INTERACTION | `CMiniRoomBaseDlg` 邀请/SSN2/CHAT | `Net/OutPacket.h:261-263` 逐字「出站必须用 s9 RecvOpcode.PLAYER_INTERACTION=0x7B=123…314 同样是抄了 Send 侧值」 | ✅ | 已修，销案 |
| B09 | c2s 212/213/215/218 零枚举 | `CField_SnowBall::Update(212)`/`CField_Coconut::BasicActionAttack(213)`/`CField_GuildBoss(215)`/`CUIMonsterCarnival::RequestSend(218)` 全有发包 | `Net/OutPacket.h` 全文无 211/212/213/215/218 定义；`s9-protocol-align/notes/gap-inventory.md:105,107,108` 判「明确不做」 | ⚠️ | **s9 适配冲突**（非漏写），改回原生断联机 → D5 |
| B10 | c2s/s2c 撞值分列 | 083 两栏独立 | 123↔314 PLAYER_INTERACTION、189 AUTO_AGGRO↔ENERGY_ATTACK、117 WATER_OF_LIFE↔SUCCESS_IN_USE_GACHAPON_BOX、130↔63 BUDDYLIST | ✅ | 口径已建立，保持分列 → D7 |
| B11 | `MATCHED-HANDLED` 态 | — | matrix 全库 `304 HANDLED / 146 MATCHED / 9 DEFINED_NOT_EMITTED / 1 CLIENT_ONLY / 1 S9_DUP_VALUE`，0 处合并态 | ✅ | c2s=MATCHED / s2c=HANDLED 分列，旧读数作废 → D11 |
| B12 | 148/148 client_out | — | `opcode-matrix.json:26-41` `client_out_sent=148` / `generated_at=2026-11-08T00:56:27` / `s9_git_head=3d5ccc6f8e` | ✅ | `parity-gap:12` 103/差集71 作废 → D11 |
| B13 | 嘉年华 CP 召唤 218 发包 | `RequestSend CanSendExclRequest(500)+COutPacket(218)+Encode1+Encode4(*(this+727)-1)` | 当前端 s2c 289–295 handler 有实体、c2s 218 无枚举 | ⚠️ | s9 适配，反证 F1「零命中」→ D5 |
| B14 | 095 专有 v95 Academy/BossParty | 083 无 | MEMORY.md 硬约束判「不做」 | ⊘ | 结构性不做 |

### C 逻辑（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| C01 | 六面字节级一致性 | 844 具名函数 / 73 类 EA | `angel-beidou-activity-ea-crosscheck.json` 844/841 + `angel-beidou-activity-ea-bytelevel-audit.json` 3 EA 字节表 | ✅ | 更正为 844/844 by bytes，阈值误判 → §1.4 |
| C02 | 3 missing EA 根因 | `0x462738`/`0x5617a1`/`0x6cec47` 带修饰名函数头 | BeiDou `get_func_attr=0xffffffffffffffff` 但 8 字节逐字相同 | ✅ | IDA 覆盖差异，非布局差异 |
| C03 | JustBomb decompile 失败兜底 | `@ 0x58fbfc` `Decompilation failed at 0x58fc17` | `disasm` 12 指令 → `ShowSkillSpecialEffect(GetSkill(...),0x320,...)` 写回 full.json `empty=0` | ✅ | 补全，重跑 facts 3987 行 |
| C04 | Carnival case 296 缺口 | `OnPacket case 289,290,291,292,293,294,295,296`（296=`OnShowGameResult` + `SP_4087..4090` 胜负文案） | `Net/PacketSwitch.cpp:185-189,314-315` 只到 295；matrix s2c 无 296 条目 | ❌ | **新增硬冲突** → D5 |
| C05 | 299/300 归属冲突 | `CField_Battlefield::OnPacket` 299=`OnScoreUpdate`/300=`OnTeamChanged`（`angel-activity-facts.md:606-613`） | matrix s2c 299/300=`SHEEP_RANCH_INFO/SHEEP_RANCH_CLOTHES` + `WaveAShellHandlers.cpp:1626` SheepRanchInfoHandler | ⚠️ | **新增硬冲突** → D5 |
| C06 | 212 命名/语义冲突 | `CField_SnowBall::Update COutPacket(212)`（`facts.md:1098`） | `wave4-battle-packets/README.md:22`「LEFT_KNOCKBACK(212)…不做（无脚本触发面）」+ matrix s2c 284 LEFT_KNOCK_BACK | ⚠️ | **新增硬冲突** → D5 |
| C07 | Carnival 字段序 | `OnEnter Decode1+6×Decode2+Decode1`；`OnPersonalCP Decode2×2`；`OnTeamCP Decode1+Decode2+Decode2` | `WaveAShellHandlers.cpp:619-708` Start/ObtainedCP/PartyCP/Message 均 `emplace<UIMonsterCarnival>()`+`set_cp` | ⚠️ | 289–295 具实体，296 缺 → D5 |
| C08 | 椰子计分 Decode2×2 | `CField_Coconut::OnCoconutScore *(this+512)=Decode2; *(this+513)=Decode2; DrawBoard` | `WaveAShellHandlers.cpp:1125-1136` CoconutScoreHandler | ✅ | 字段序销案 |
| C09 | 295 LEAVE 空壳 | `OnShowMemberOutMsg SP_4096/4097` | `WaveAShellHandlers.cpp:710-714`「`MonsterCarnivalLeaveHandler::handle { ... ui->deactivate(); }`」；`feature-gap:366` §9.5.3 #9「整组空」过度概括 | ⚠️ | 只 295 空壳，289–293 有实体 → D9 |
| C10 | RPS OnPacket 分支 | v1<6 return / v1<=7 LABEL_23 / v1==8 抛断线 + `Decode4` 报名费 `SP_3675` | `WaveAShellHandlers.cpp:716-759` `switch(mode) case 8 // open npc` + skip_byte | ⚠️ | 降级状态文本 → D3 |
| C11 | ItemMaker 113 字段序 | 11 个 Encode4/Encode1 + `SP_753_PRODUCTION_FAILED` + ComboBox 998/426/425 | `MakerPackets.h:30-40` `write_int(1)`/`write_int(toCreate)`/装备占位 | ⚠️ | s9 适配简化 → D6 |
| C12 | Quiz 304 子类型 | `CScriptMan::OnPacket if(a2==304) OnScriptMessage` + OnAskQuiz/OnAskSpeedQuiz → `sub_A26B09`/`sub_A26C66` | `pending-live-tests/README.md:36` 只 0/1/2/3/4/12 回归；`UIWindow.img:68 InitialQuiz`/`:72 SpeedQuiz` 有节点 | ⏳ | 待复核子类型是否接 quiz → D8 |
| C13 | 095 注释污染 2 处 | 083 无 | `IO/UIStateGame.cpp:532`「095 自定义中文 UI：暂时复用 EVENT 键打开福利中心」+ `IO/UITypes/UISpecial.h:9`「095 自定义中文 UI 入口：福利中心」 | ❌ | 只换注释依据引 Angel 083、不改行为 → D1 |
| C14 | Q2 文档债口径 | — | `feature-gap:493`「客户端零命中」须限定 c2s；`:366` #9 过度概括；`:292` USE_ITEMUI 238 vs `Net/OutPacket.h:299` 236 | ⚠️ | 交裁决后统一回写，不擅改 Q2 → D11 |

### D UI（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| D01 | CUIMonsterCarnival::OnCreate 16 控件 | `SP_4102..4105 BACKGRND3_{TOP_0,MIDDLE1_0,BOTTOM_0,MIDDLE0_0}` + `SP_4098/4099/4112/4113 ICON0..3` + `SP_4106 BTSIDE` + `SP_4107/4108 TAB` + `SP_4109 BTDECIDE` | `UIMonsterCarnival` 4 setter HUD 壳（无完整控件表还原） | ⚠️ | 壳 → D4 |
| D02 | CUIItemMaker::OnCreate 两 ComboBox | `30000(3,77,71,78,18)`/`40000(3,157,71,143,18)` + `20000(82,356)`/`20001(163,356)` | `UIWindow.img:84 Maker` 壳（Category 998/426/425 部分） | ⚠️ | 壳 → D4 |
| D03 | CUIQuestInfo::OnCreate | `CCtrlTab(2000,1,3,23,239)`+`(2001,1,3,225,50,309)`+3×SetTab | `IO/UIElement.h` QUEST 枚举 + `UIWindow.img:22 Quest` + `Quest` 窗 | ✅ | 结构对应，销案 |
| D04 | CUINewYearCardSenderDlg 控件 0–5 | `0(295,65)/1(124,164)/2(181,164)/3(46,66,243,15)/4(13,105,333,45)/5(1,3,498,7,93)` | `UIWindow.img:81 NewYearsCard` 壳 | ⚠️ | 壳 → D4 |
| D05 | CUICashItemGachapon::OnCreate | `2000(68,149)/2001(150,8)` + `SP_5066/5067` | `UIWindow.img:97 CashGachapon` + `Net/PacketSwitch.cpp:180-181` | ⚠️ | 壳 → D4 |
| D06 | CUIMedalQuestInfo::OnCreate | `CCtrlTab(2000,1,4,24,300)`+`(2001,1,3,239,48,12)`+`SetTab(100)` | `UIWindow.img:101 Title` + 勋章框 255×372 截图 | ⚠️ | 壳 → D4 |
| D07 | UIWindow.img 活动顶层节点 30+ | `SP_*` 资源族 | `UIWindow.img.top-level.txt:26 Minigame/:28 RpsGame/:30 MinigameTable/:64 MonsterCarnival/:68 InitialQuiz/:72 SpeedQuiz/:73 EnergyBar/:76 MonsterBook/:81 NewYearsCard/:84 Maker/:94 RemoteGachapon/:97 CashGachapon/:101 Title/:110 SlideMenu` | ✅ | 反证「客户端零命中」，销案 |
| D08 | 无独立顶层 MiniRoom/Omok/Coconut/Snowball/Battlefield/Dojang 场地 | Angel 各为独立 `CField_*`/`CDlg` 类 | r03 §1 无独立顶层节点；r03 §7 活动截图/PS 产物 0 | ⚠️ | 资源层空白 → D8 |
| D09 | MiniRoom 6 类 → 单壳 | Factory case 1..6 | `UIMinigame.h:2` + `UIMiscShells.h:925 UIMinigameTable` 合并 | ⚠️ | 非一一对应，架构重写 → D3 |
| D10 | 活动 Type 枚举全集 | 083 各窗 | `IO/UIElement.h:65-193` CASHGACHAPON/ENERGYBAR/MONSTERCARNIVAL/RPSGAME/MINIGAME/SPEEDQUIZ/INITIALQUIZ/ASKPET/SLIDEMENU/MAKER/GUILDBOSS/EVENT 等全在 | ✅ | 存在性销案（`:195-199` NUM_TYPES 尺寸警示） |
| D11 | AskPet/SlideMenu IDA 锚点互证 | `CScriptMan` action 9/10/14 | `IO/UIElement.h:103-106` 逐字「NPC AskPet / AskPetAll（msgtype 9/10，IDA 7474A2 / 74775C）」「维度镜 SlideMenu（msgtype 14，IDA 76B3F0）」 | ✅ | action code 一一互证 |
| D12 | NpcTalkMorePacket 语义复用 | `CScriptMan COutPacket(60)` | `UIMonsterCarnival.cpp:97-137`/`UIPartyRace.cpp:51-70` `NpcTalkMorePacket(tip).dispatch()` | ✅ | 60 实存，销案子代理误判 |
| D13 | UISpecial 福利中心 DIY | 083 无 | `IO/UITypes/UISpecial.h:9` + `IO/UIStateGame.cpp:532` 095 自定义中文 UI | ⊘ | 结构性不做（仅换注释依据）→ D1 |
| D14 | UICharInfo/SlideMenu/EventAlarm 等壳 | `CScriptMan::OnAskSlideMenu` / `CUIEventAlarm::CreateEventAlarm` | `UIMiscShells.cpp` 软键硬编码 + `IO/UITypes/` 占位壳 | ⚠️ | 壳 → D4 |

### E 排版（16 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| E01 | CUIMonsterCarnival 按钮坐标 | `CCtrlButton(a3:3,a4:126,a5:8)`/`(a3:2,a4:140,a5:8)`/`(a3:4,a4:149,a5:28)` | `UIMonsterCarnival` 4 setter HUD（无逐控件坐标还原） | ⚠️ | 待展开帧 → D8 |
| E02 | CUIMonsterCarnival CCtrlTab | `CCtrlTab(a3:0,a4:5,a5:4,a6:107,a7:162,a8:0)+SetTab(*(this+376))` | 当前端 tab 壳 | ⚠️ | 待展开帧 → D8 |
| E03 | CUIItemMaker 两 ComboBox 坐标 | `30000(3,77,71,78,18)`/`40000(3,157,71,143,18)` | `UIWindow.img:84 Maker` 壳 | ⚠️ | 待资源 → D8 |
| E04 | CUIItemMaker 两按钮坐标 | `20000(82,356)`/`20001(163,356)` | `UIWindow.img:84 Maker` 壳 | ⚠️ | 待资源 → D8 |
| E05 | CUIQuestInfo 双 CCtrlTab | `CCtrlTab(2000,1,3,23,239)`+`(2001,1,3,225,50,309)` | `UIWindow.img:22 Quest` + 任务栏截图 548×396 | ✅ | 结构对应，销案 |
| E06 | CUINewYearCardSenderDlg 6 控件坐标 | `0(295,65)/1(124,164)/2(181,164)/3(46,66,243,15)/4(13,105,333,45)/5(1,3,498,7,93)` | `UIWindow.img:81 NewYearsCard` 壳 | ⚠️ | 待展开帧 → D8 |
| E07 | CUICashItemGachapon 按钮坐标 | `2000(68,149)/2001(150,8)` | `UIWindow.img:97 CashGachapon` 壳 | ⚠️ | 待展开帧 → D8 |
| E08 | CUIMedalQuestInfo 双 CCtrlTab | `CCtrlTab(2000,1,4,24,300)`+`(2001,1,3,239,48,12)`+`SetTab(100)` | `UIWindow.img:101 Title` + 勋章框 255×372 | ⚠️ | 待展开帧 → D8 |
| E09 | SpeedQuiz backgrnd 286×410 | `SP_4505..4513` slide + Quiz 资源 | `UIMiscShells.cpp:1105` 286×410(IDA 7EB303/StringPool 3924) + `a11k-speedquiz-ui-ida.md:25` | ✅ | 尺寸销案 |
| E10 | InitialQuiz 288×229 | 083 InitialQuiz 资源 | `UIMiscShells.cpp:1464` 288×229(IDA 7E99BC) | ✅ | 尺寸销案 |
| E11 | Slidemenu Choice/Cover/backgrd | `CScriptMan::OnAskSlideMenu` | `slidemenu.txt` 22 行（Choice 40×40 / Cover 142×38 / backgrd `[null]` 空壳 / 韩文 '차원의문'·'타임게이트'） | ⏳ | 空壳，待 orange-wz 实解析 → D8 |
| E12 | MinigameTable/Omok 棋盘尺寸 | `COmokDlg` 棋盘 | `UIMiscShells.cpp:7367 board[15][15]` + `:7385` 字母兜底 `Text stone(...,(p==1)?"B":"W")`；无 layout JSON | ⏳ | 无尺，待实机 → D8 |
| E13 | 正确端截图库活动窗本体 0 | — | `C:\Users\11036\Desktop\正确客户端截图\` 78 唯一文件（IHDR 实读），仅近邻任务栏/怪物手册/勋章框/番茄田/NPC 二级弹窗 | ⏳ | 待原生展开帧 → D8 |
| E14 | layout JSON 全非活动 | — | `**/layout*.json` 5 个全非活动（char-stats-ui-ps / map-search-ui / map-info-panel / UI/MapInfo） | ⚠️ | 无尺 → D8 |
| E15 | Configuration 7 个活动 ui.position.* | 083 各窗位置 | `Configuration.h` `PosEVENT "(99,100)"/PosDAILYGIFT "(99,80)"/PosCASHGACHAPON "(340,160)"/PosREMOTEGACHAPON "(300,120)"/PosPARTYRACE "(20,80)"/PosQUEST "(300,160)"/PosPARTYSEARCH "(260,60)"` | ⚠️ | 有但覆盖稀薄（30+ 活动窗只 7 键）→ D9 |
| E16 | 活动 PSD 55 个全非活动 / 活动 PNG 0 | — | r03 §3 `docs/features/**/*.psd` 55 个全非活动；活动命名 PNG 0 | ⏳ | 资源层空白 → D8 |

### F 样式（12 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| F01 | SP_ 238 全集（UI_UIWINDOWIMG 54） | `SP_394..406` 房间拒绝 / `SP_4078..4113` 嘉年华 / `SP_5105..5302` 勋章 / `SP_5371..5393` 杀计数 | 当前端 `.nx` 节点引用部分 + locale stub | ⚠️ | 节点集还原不全 → D8 |
| F02 | 嘉年华队色/CP/胜负件表 | `SP_4078_MAPLE_RED`/`4079_MAPLE_BLUE`/`4080..4090`/`4102..4113` | `UIMonsterCarnival` 4 setter 文本 HUD，无位图件表 | ⚠️ | 样式降级 → D4 |
| F03 | 椰子/道场资源族 | `SP_2776..2784` 椰子 / `SP_2501/2558/4552` 道场 / `SP_1000_BLAZE` | `FieldMarker.cpp:19` 黄字 `Color::Name::YELLOW` 兜底 | ❌ | 整族样式缺 → D4 |
| F04 | 扭蛋/勋章件表 | `SP_5066/5067` 扭蛋 / `SP_5105..5302` 勋章 | `UIWindow.img:97 CashGachapon/:101 Title` 壳 + locale stub | ⚠️ | 样式壳 → D4 |
| F05 | EnergyBar/SlideMenu 件表 | `SP_1883/1886` energy bar / `SP_4505..4513` slide menu | `UIEnergyBar`/`UISlideMenu` 壳 | ⚠️ | 样式壳 → D4 |
| F06 | 通用组件常量 | 083 Icon/Button/NameTag/Gauge/Slider/Tooltip | `Icon.h:105-106 cell=32×32` / `Button.h:28-36 6 态` / `MapleButton.cpp:64-66 四态` / `NameTag.h:41-50` / `Gauge.h:27-34 CLIP` / `Slider.cpp:43,46 VScr5` / `Text.h:33-48 12 字体` | ✅ | 通用常量齐，销案 |
| F07 | locale 活动文案 | 083 原生文案族 | `zh-CN.ini`/`en-US.ini` rps.*/gachapon.*/maker.*/minigame.*/carnival.*/miniroom.err_*/speedquiz.*/minigametable.*/eventwindow.* **两语言同值英文 stub 未译**，仅 monsterbook 5 键 zh-CN 已译 | ❌ | stub → D9 |
| F08 | UiAssetResolve 回退链 | 083 资源解析 | `Util/UiAssetResolve.cpp:42-55 window()` UIWindow2→UIWindow / `:57-75 window4()` 三级 / `:115-131 close_btn()` order `{"BtClose","BtClose2","BtClose3"}` / `:133-145 usable()` | ✅ | 销案 |
| F09 | MapleButton 四态 | 083 按钮四态 | `MapleButton.cpp:55-71` `src["normal"]/["pressed"]["0"]/["mouseOver"]["0"]/["disabled"]["0"]` | ✅ | 销案 |
| F10 | 095 线仅登记排除 | — | `docs/features/port-095/` 全目录 + `c4-btsort-gather-semantics/README.md:10` + `change-quickslot-persist/.../npc-action-generatemovepath-2026-11-06.md:3,10`（`Maplestory95.i64`）仅登记、不作 083 依据 | ✅ | 口径合规，销案 |
| F11 | orange-wz(IMG) 门禁阻塞 | — | r03 §6 Host 未注册 + `codebuddy-transcript-947f8ba0.md:37` `Index 16 out of bounds for length 4` | ⚠️ | 六面一致阻塞，替代链取证 → D14 |
| F12 | `read_image` hy4free 无 image input | — | PNG 返回 `cannot read ... as an image: model "hy4free" does not declare image input` → 未做像素级视觉复核 | ⚠️ | 尺寸取自 evidence 文本/源码常量 → D14 |

### G 交互（14 条）

| # | 校准点 | Angel 083 一手语义 | 当前端 source | 判定 | 处置 / 决策 |
|---|---|---|---|---|---|
| G01 | CMiniRoomBaseDlg 子命令分发 | `OnPacketBase` 无房间 2/3/4/9 分支 + 有房间 switch 3/4/6/9/10 + `(*(*v6+80))` 子类虚调用 | `TradeHandlers.cpp:142 case 5 ROOM` 分流 + PlayerInteraction 6 构造点 | ⚠️ | 壳，子命令覆盖不全 → D3 |
| G02 | Omok/Memory OnKey Enter/Esc | `if(a3>=0 && a2==13)` 回车 `CCtrlEdit::GetText`→`CheckAndSendChat`→清空+`SetFocus`；`a2==27` 吞掉 | `UIMinigame.cpp:97`「SoftKey x,y[,piece] → Omok place」/`:118`「SoftKey turn,slot → MatchCard select」 | ⚠️ | 软键替代键盘，无胜负交互 → D3 |
| G03 | RPS 报名费/UtilDlgEx | `OnPacket v1==8` + `Decode4` 报名费 + `SP_3675` + `Alloc(1768)` CUtilDlgEx | `WaveAShellHandlers.cpp:716-759` RpsGameHandler 降级状态文本 + skip_byte | ⚠️ | 壳 → D3 |
| G04 | 脚本 action code 通道 | OnSay=0/OnAskText=2/OnAskNumber=3/OnAskMenu=4/OnAskAvatar=7/OnAskMembershopAvatar=8/OnAskPet=9/OnAskPetAll=10/OnAskBoxText=13/OnAskSlideMenu=14 | `IO/UIElement.h:103-106` AskPet(9)/AskPetAll(10)/SlideMenu(14) + `Net/OutPacket.h:100`(60) | ✅ | 核心 action 互证，销案 |
| G05 | 嘉年华 CP 召唤交互 | `RequestSend CanSendExclRequest(500)+COutPacket(218)+Encode1+Encode4` | 当前端 c2s 218 无枚举、s2c 289–295 有 handler | ❌ | 真缺但属 s9 适配冲突 → D5 |
| G06 | 椰子攻击交互 | `CField_Coconut::BasicActionAttack COutPacket(213)+CUserLocal::TryDoingNormalAttack` | `Net/OutPacket.h` 无 213；`Net/PacketSwitch.cpp:382-383`(285/286) s2c 有 | ⚠️ | s9 适配 → D5 |
| G07 | 雪球 Update 交互 | `CField_SnowBall::Update COutPacket(212)` | `Net/OutPacket.h` 无 212；`Net/PacketSwitch.cpp:379-381`(281/282/283) s2c 有 | ⚠️ | s9 适配 + 212 命名冲突 → D5 |
| G08 | Battlefield 得分/换队交互 | `CField_Battlefield::OnScoreUpdate Decode1+Decode1`/`OnTeamChanged Decode4+Decode1`（299/300） | matrix 299/300 SHEEP_RANCH_* + `WaveAShellHandlers.cpp:1626` | ⚠️ | 归属冲突 → D5 |
| G09 | SlideMenu 应答交互 | `CScriptMan::OnAskSlideMenu COutPacket(60)+Encode1(14)+Encode1(1)+Encode4(v14)+Encode1(0)` | `IO/UIElement.h:103-106` SlideMenu(14) + `slidemenu.txt` 空壳 | ⚠️ | 壳 → D8 |
| G10 | ItemMaker 制造交互 | `RequestItemMake` 11 字段 + `SetComboBox_Category` 998/426/425 | `MakerPackets.h:30-40` `write_int(1)`/`write_int(toCreate)` 简化 | ⚠️ | s9 适配 → D6 |
| G11 | CQuest::StartQuest 交互 | `COutPacket(107)` | `Net/OutPacket.h:193 QUEST_ACTION = 107` + `CQuest` | ✅ | 销案 |
| G12 | Tutor/EventAlarm/QuestAlarm 交互 | `CUIEventAlarm::CreateEventAlarm` / `CUIQuestAlarm` / `CTutor` / `CUIMedalQuestInfo` | `IO/UITypes/` 占位壳 + `UIWindow.img:23 QuestAlarm/:105 MapleEvent` | ⚠️ | 壳 → D4 |
| G13 | 6 频道快捷 change_target 空 stub | `CChatHelper`/频道切换 | `IO/UITypes/UIChatBar.h:123 change_target(int32_t) {}` | ❌ | 跨面共性真缺 → D9 |
| G14 | 门禁 / 实机 / 提交 | — | 本轮未构建、未实机、未抓 `packet_dump.log`、未提交（原子提交须授权） | ⚠️ | 一次性 Plan P0–P2 排期 → D14 |

**§3 判定汇总**：✅29 / ⚠️51 / ❌11 / ⊘2 / ⏳5 = 98 条（行级状态机复核见 §9.2）。

## §4 Angel 083 十条硬发现（r01 一手语义）

1. **c2s 全集只有 8 个 opcode**：267 函数内 `COutPacket::COutPacket(a1: N)` 全扫描 = `60`(CScriptMan OnSay/OnAsk*)、`107`(CQuest::StartQuest)、`113`(CUIItemMaker::RequestItemMake)、`123`(CMiniRoomBaseDlg 邀请/SSN2/CHAT)、`212`(CField_SnowBall::Update)、`213`(CField_Coconut::BasicActionAttack)、`215`(CField_GuildBoss::BasicActionAttack)、`218`(CUIMonsterCarnival::RequestSend)。活动场地上行 4 个当前端零枚举、属 s9 适配冲突。
2. **脚本 action code 逐字**：OnSay=0、OnAskText=2、OnAskNumber=3、OnAskMenu=4、OnAskAvatar=7、OnAskMembershopAvatar=8、OnAskPet=9（带 Encode1(2)+二次 Decode1）、OnAskPetAll=10、OnAskBoxText=13、OnAskSlideMenu=14；OnAskYesNo/OnAskQuiz/OnAskSpeedQuiz/OnSayImage 未显式命中（记未知）。`IO/UIElement.h:103-106` 自带 IDA 锚点（7474A2/74775C/76B3F0）与 9/10/14 一一互证。
3. **CMiniRoomBaseDlg::OnPacketBase 双层分发**：`v1=Decode1; result=dword_BE7910;` 无房间分支 `v3=v1-2; if(v3==0)OnInviteStatic; v4=v3-1; ... OnEnterResultStatic/OnCheckSSN2Static`；有房间分支先 `(*(*(dword_BE7910+4)+72))` 再 switch case 3→OnInviteResultStatic / 4→OnEnterBase / 6→`(*(*v6+80))` 子类虚调用 / 9→OnAvatar / 10→`sub_65EDB5` / default→`(*(*v6+64))`。
4. **COmokDlg::OnKey 与 CMemoryGameDlg::OnKey 逐字同一份**：`if(a3>=0 && a2==13)` 回车取 `CCtrlEdit::GetText` → `CMiniRoomBaseDlg::CheckAndSendChat(this:this-1,...)` → 清空 + `CWndMan::SetFocus`；`a2==27` 吞掉；其余 `CDialog::OnKey`。落子/翻牌不在 OnKey。
5. **CRPSGameDlg::OnPacket**：`Decode1` v1<6 return / v1<=7 LABEL_23 / v1==8 且 `dword_BE7910!=0` 抛 `CDisconnectException 0x21000003`，否则 `Decode4` 报名费 + `SP_3675_EBROCK_PAPER_SCISSORS_NOTICE...` + `ZAllocEx::Alloc(1768)` 造 CUtilDlgEx。
6. **CField_MonsterCarnival::OnPacket case 289–296**（296=`OnShowGameResult`）：`OnEnter Decode1+6×Decode2+Decode1` / `OnPersonalCP Decode2×2` / `OnTeamCP Decode1+Decode2+Decode2` / `OnRequestResult SP_4082..4086` / `OnProcessForDeath SP_4078_MAPLE_RED/4079_MAPLE_BLUE` / `OnShowGameResult SP_4087`胜/`4088`负/`4089`加时平/`4090`对方离场 / `OnShowMemberOutMsg SP_4096/4097`。当前端只到 295 → **case 296 缺口**。
7. **CField_Battlefield / Dojang / Coconut**：`DecodeFieldSpecificData=Decode1`、`OnScoreUpdate=Decode1+Decode1`、`OnTeamChanged=Decode4+Decode1`、`OnClock=Decode1+Decode4`；`CField_Coconut::Init SP_2776..2780 + SP_956_CANVAS`；`CField_Dojang::UpdateTimer SP_1000_BLAZE/SP_2501/SP_2558`。**299/300 在 Angel 是 Battlefield 得分/换队、matrix 却是 SHEEP_RANCH_* → 归属冲突**。
8. **控件 id/坐标全集**：`CUIMonsterCarnival::OnCreate` 资源 `SP_4102..4105 BACKGRND3_*` + `SP_4098/4099/4112/4113 ICON0..3` + `SP_4106 BTSIDE` + `SP_4107/4108 TAB` + `SP_1262 BTMAX` + `SP_4109 BTDECIDE` + 字体 `SP_1410_CANVASFONT/SP_5527_ARIAL`；控件 `CCtrlButton(3,126,8)/(2,140,8)/(4,149,28)` + `CCtrlTab(0,5,4,107,162,0)+SetTab(*(this+376))`。`CUIItemMaker::OnCreate 30000(3,77,71,78,18)/40000(3,157,71,143,18)/20000(82,356)/20001(163,356)`。`CUIQuestInfo CCtrlTab(2000,1,3,23,239)+(2001,1,3,225,50,309)`。`CUINewYearCardSenderDlg 0(295,65)/1(124,164)/2(181,164)/3(46,66,243,15)/4(13,105,333,45)/5(1,3,498,7,93)`。`CUICashItemGachapon 2000(68,149)/2001(150,8)`。`CUIMedalQuestInfo CCtrlTab(2000,1,4,24,300)+(2001,1,3,239,48,12)+SetTab(100)`。
9. **CUIMonsterCarnival::RequestSend 与 CUIItemMaker::RequestItemMake 逐字**：前者 `result=CWvsContext::CanSendExclRequest(a1:500,a2:0); COutPacket(218); Encode1(v3); Encode4(*(this+727)-1); SendPacket(dword_BE7914)`；后者 `COutPacket(113)` + `Encode4(v5)/Encode4(*(this+371))/Encode1(*(this+1976))/Encode4(*(this+485))/Encode4(Data)/Encode4(3)/Encode4(4)/Encode4(v6)/Encode4(*(this+495))/Encode4(v14)` + `SendPacket`。
10. **844/844 字节级六面一致 + SP_ 238**：`SP_394..406` 房间拒绝文案、`SP_4078..4113` 嘉年华、`SP_2776..2784` 椰子、`SP_2501/2558/4552` 道场、`SP_387..389/3965/3966` 卷轴、`SP_4505..4513` slide menu、`SP_5371..5393` 怪物杀计数、`SP_598..600/753..761` 道具制作、`SP_5066/5067` 扭蛋、`SP_5105..5302` 勋章、`SP_1883/1886` energy bar、`SP_1300` float notice。Angel 只补「符号语义层」，不补功能/版本。

## §5 四态（销案 / 推翻 / 新增 / 遗留）

- **✅ 销案（旧账作废）**：`parity-gap:12` 103/差集71 → `opcode-matrix.json` 148/148；`parity-gap:56`「远程扭蛋发不出」→ `ItemAClassPackets.h:65` .dispatch() + c2s 116 MATCHED；`s9-protocol-align/README.md:10,11,32,60` 103/299-3/UNHANDLED_EMITTED=3 → 304 HANDLED / 146 MATCHED 分列；r04 子代理「c2s 60 零命中」→ `Net/OutPacket.h:100 NPC_TALK_MORE=60` 实存；`codebuddy-baseline.txt:173,193-204`「8 类仅场标」→ `map-object-audit/README.md:18,40-46` G5/G6/G7/G10/G11 已真对象化；`feature-gap:366` §9.5.3 #9「整组空」→ 289–293 有实体、只 295 空壳；`feature-gap:493`「客户端零命中」→ 限定 c2s 方向（s2c + UIWindow.img 节点全在）。
- **❌ 推翻 / 真缺**：Omok 胜负/禁手状态机、MemoryGame 牌面（`if(game_kind==1)` 才画）、12 个 `CField_*` 活动场地对象族、入场错误码 22 分支、case 296、332 Stamp handler、locale 英文 stub、每日签到仅 UI 假态、XyLottery/MTS 双端皆无、`IO/UIStateGame.cpp:532` + `UISpecial.h:9` 2 处 095 注释污染。
- **➕ 新增**：三处硬冲突（212 命名/语义、case 296 缺口、299/300 归属）；`MATCHED-HANDLED` 态不存在口径；c2s 8 锚点落 4 / 零枚举 4 属 s9 适配冲突；3 missing EA 字节级根因 + JustBomb disasm 兜底两个独有技术点；`feature-gap:292` USE_ITEMUI 238 vs `Net/OutPacket.h:299` 236 旧账；正确端截图库口径差（154 含重名副本 vs 78 唯一文件，取值按 78）。
- **⏳ 遗留 / 待实机**：`pending-live-tests` G11 场标贴图 / 风筝 iconReward / 金银宝箱 / CLICK_GUIDE / MapleTV 双屏 / Quiz 304 子类型 / SpeedQuiz 286×410 回归；「本环境历史抓屏全黑」`feature-gap:567`；orange-wz 未接入导致 IMG 层阻塞（六面一致）；活动窗本体截图/PS/layout 全 0。

## §6 十四个决策点（D1–D14，交用户裁决）

| # | 决策点 | 推荐默认 | 依据 |
|---|---|---|---|
| D1 | `IO/UIStateGame.cpp:532` + `IO/UITypes/UISpecial.h:9` 095 注释污染 | **做**：只换注释依据引 Angel 083，不碰福利中心行为 | §4 / r02 §4 / C13 |
| D2 | crosscheck「layout differs」阈值误判 | **更正**为 844/844 字节级一致（字节审计文件为准），修正 `ida_angel_activity_crosscheck.py:107` 阈值逻辑 | §1.4 / C01 |
| D3 | 小游戏 DIY 合并壳（Omok 胜负/禁手、MemoryGame 牌面、RPS 状态机、Factory 6 类） | 维持「wire later」壳，单列补齐计划**待裁决** | A01–A04 / B04 / G01–G03 |
| D4 | 活动场地对象族 / 活动 UI 完整控件表 / Tutor·EventAlarm 壳 | 维持 `FieldMarker` 黄字 + HUD 壳降级，**待裁决** | A06 / A10–A11 / D01–D06 / F02–F05 |
| D5 | **三处硬冲突 + 4 个 s9 零枚举值**（212 命名、case 296、299/300、212/213/215/218） | **保持 s9 联机优先**，改回原生会断联机，交裁决、不得盲改 | 五面红线 / B09 / C04–C07 / G05–G08 |
| D6 | Maker 113 字段序（Angel 11 字段 vs `MakerPackets.h` 2 int 简化） | 保持 s9 简化，出「字段序 ↔ s9 MakerProcessor」对照后**待裁决** | A09 / B07 / C11 / G10 |
| D7 | c2s/s2c 撞值分列口径 | **保持分列**（123↔314、189↔189、117↔117、130↔63） | `opcode-matrix.json` 两栏 / B10 |
| D8 | IMG 资源层空白（活动截图/layout/PS/顶层场地节点全 0、Slidemenu 空壳、Quiz 304） | 补 orange-wz 实解析或原生展开帧，**待资源/待实机** | r03 §7 / E11–E16 / F01 |
| D9 | 真缺整族（入场错误码 22、295 LEAVE、332 Stamp、locale stub、Configuration 7 键、`change_target` 空 stub、签到/XyLottery/MTS） | 维持不做 / 低优先补文案，跨面统一登记 | B02 / C09 / A12 / A14 / F07 / G13 |
| D10 | 295 LEAVE 是否补 `OnShowMemberOutMsg` 语义（SP_4096/4097） | 先补零资源依赖行为，**待裁决** | C09 |
| D11 | Q2 文档债统一回写（`feature-gap` F1/§14.1/§9.5.3 #9/§17、`:292` 238、`parity-gap:12,56`、`s9-protocol-align` 旧读数、`gap-inventory.md` 旧 head） | 交裁决后**统一回写**，本面不擅改 Q2 | §2.4 / r05 §4,§5 / C14 |
| D12 | CGrenade::JustBomb 是否补完整 decompile（当前 disasm 兜底已足语义） | 维持 disasm 兜底，低优先 | C03 |
| D13 | AskPet gage ColorBox / 通用组件常量已齐 | 维持降级占位，低优先 | F06 |
| D14 | 门禁与提交 / 续作 | 不接 orange-wz；先 P0 + 全量 Rebuild + 1280×720 活动屏实机；原子提交须授权；P3 后续功能面续作 | §7 Plan |

**D5+D6+D3+D4 共同决定「校准 ≠ 改回原生」，与前五面一致。**

## §7 一次性 Plan（P0–P3）

- **P0（零风险 · 纯文档/客户端注释 · 可直接做）**：① D1 只换 2 处 095 注释依据引 Angel 083；② D2 更正 crosscheck 结论 + 固化 `angel-beidou-activity-ea-bytelevel-audit.json` 为权威；③ D7 固化 c2s/s2c 撞值分列 + `MATCHED`/`HANDLED` 分列口径到本面 README + r05；④ D11 前半：出「Q2 旧账 ↔ 本面取值」逐条对照表（不擅改 Q2 正文）；⑤ 门禁 `python tools/gen_opcode_matrix.py --check`；⑥ r01–r05 + 7 evidence 归档完整性校验。
- **P1（需用户裁决）**：D3 小游戏状态机补齐、D4 活动场地/UI 完整表、D5 三处硬冲突 + 4 个 s9 零枚举值、D6 Maker 字段序、D9 真缺整族、D10 295 语义、D11 后半统一回写 Q2。逐点先交决策表，裁决后再动源码。
- **P2（需 IMG/实机）**：D8 补 orange-wz 实解析或原生展开帧定 30+ 活动窗控件坐标；随后单测 + `tools/build_smoke.bat` 全量 Rebuild（注意 `IO/UIElement.h:195-199` NUM_TYPES 尺寸警示）+ 1280×720 活动屏（嘉年华/扭蛋/Quiz/小游戏）比对 + 抓 `packet_dump.log` 验 60/107/113/123 字段序。
- **P3（goal 续作）**：复用本面 5 脚本生成器 + 三路只读取证模板 + 六维 98 条 / 14 决策点 / Plan 结构，继续后续功能面（商城 CashShop / 技能 / NPC 对话 / 地图 / 任务 / 掉落 等）；goal 保持 active 直至逐面闭环 + 各面 D1–D14 裁决 + 实机验收，实施须等用户裁决。

## §8 限制与未知

- **IMG 层阻塞（六面一致）**：orange-wz(IMG) MCP 未接入本 DSH Host，且已知 `Index 16 out of bounds for length 4` 解析缺陷；本面活动窗本体截图 / layout JSON / PS 产物 / UIWindow 顶层场地节点全 0 → 排版/样式判定中 ⏳4 条、⚠️ 多条属「待资源」而非「代码错误」。
- **`read_image` 限制**：模型 `hy4free` 未声明 image input，PNG 返回 `cannot read ... as an image: model "hy4free" does not declare image input` → 未做像素级视觉复核，截图尺寸/坐标取自 evidence 文本与源码常量。
- **Angel 侧未取**：OnAskYesNo/OnAskQuiz/OnAskSpeedQuiz/OnSayImage 的显式 action code（facts 未命中，记未知）；`CMovePath` 字节级移动布局（跨面限制）；211 SNOWBALL 在 Angel facts 未见发包（只 212），记 [未知]。
- **未做**：产品源码改动、构建、实机、抓包、git 提交（原子提交须用户授权）。所有结论为只读取证 + 静态销案，标注 [观察]/[推断]/[待实机]。
- **跨 workspace**：`session_event_read({seq:885})` 报 `session target is outside the caller workspace`，历史取证走已落盘 evidence，不依赖跨 workspace 事件。

## §9 证据索引与复现

### §9.1 文件清单

```
docs/features/activity-minigame-angel-calibration-2026-11/
├── README.md                                      ← 本文件（主交付物）
├── notes/
│   ├── r01-angel-native-activity-facts.md          Angel 083 逐字 Hex-Rays 语义（含 §1.1 3 missing EA 根因 / §1.2 JustBomb disasm）
│   ├── r02-main-agent-crosscheck-cancel.md         主 agent 当前端源码逐字销案
│   ├── r03-activity-img-style-evidence.md          IMG 节点/组件/截图/locale/Configuration
│   ├── r04-current-activity-source-inventory.md    当前端源码逐类盘点 + §0.1 校正 + 三结论
│   └── r05-prior-activity-doc-audit.md             既有文档四态 + 常量协议表 + 15 复核点
└── evidence/
    ├── angel-activity-ea.json                      844 函数 / 73 类 EA
    ├── angel-activity-decompile.json               267/267 原始
    ├── angel-activity-decompile-full.json          逐字伪代码（含 JustBomb disasm 兜底，empty=0）
    ├── angel-activity-truncated.json               /output/ 补全记录（trunc_fixed 3）
    ├── angel-activity-facts.md                     3987 行 / 189016 B / SP_ 238
    ├── angel-beidou-activity-ea-crosscheck.json   844/841 / 3 missing / 阈值机械误判
    └── angel-beidou-activity-ea-bytelevel-audit.json 3 EA 字节表 + six_face_consistency（权威正解）
```

### §9.2 复现命令

```bash
# 门禁（协议矩阵对账）
python tools/gen_opcode_matrix.py --check     # → GATE OK: opcode matrix matches both sources (s9 3d5ccc6f8e)

# 采集链（每台先 server_health 13339/13337）
python tools/ida_angel_activity_ea.py
python tools/ida_angel_activity_decompile.py
python tools/ida_angel_activity_normalize.py
python tools/ida_angel_activity_facts.py
python tools/ida_angel_activity_crosscheck.py

# §3 判定计数（python 行级状态机，禁止手算）
python - <<'PY'
import re, collections
t = open('docs/features/activity-minigame-angel-calibration-2026-11/README.md', encoding='utf-8').read()
sec = t.split('## §3')[1].split('## §4')[0]
rows = re.findall(r'^\| ([A-G]\d{2}) \|', sec, re.M)
verdicts = [m.group(1) for m in re.finditer(r'^\| [A-G]\d{2} \|[^\n]*?\| (✅|⚠️|❌|⊘|⏳) \|', sec, re.M)]
print('rows=', len(rows), 'TOTAL=', dict(collections.Counter(verdicts)))
PY
# → rows= 98 TOTAL= {'✅': 29, '⚠️': 51, '❌': 11, '⊘': 2, '⏳': 5}
```

## §10 性质标注

- 本文为**校准 / 对照 / 决策点 / 一次性 Plan**文档，不是实现提交；不修改任何产品源码，不触发构建与实机。
- 结论区分 [观察]（逐字可读）/ [推断]（由观察推出）/ [待实机]（需真机 / IMG / 抓包）。
- 基准 083/v83；095 线符号/IDB 仅登记排除，不作依据；「改回原生会断 s9 联机」项一律交用户裁决。
- 与前五份 README（`login-flow-angel-calibration-2026-11` / `statusbar-hud-angel-calibration-2026-11` / `corewindow-angel-calibration-2026-11` / `social-economy-angel-calibration-2026-11` / `pet-summon-angel-calibration-2026-11`）同方法、同口径、同证据优先级，**六面 EA 字节级交叉验证一致**，§7 P3 逐面计划至此全部闭环。
