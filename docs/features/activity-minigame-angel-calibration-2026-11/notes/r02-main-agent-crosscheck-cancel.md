# r02 · 主 agent 独立销案复核（Angel 083 活动·小游戏 ↔ 当前端源码）

> 性质：主 agent 不依赖三路 subagent 结论的独立逐字复核（grep/read 只读）。全部结论标注 [观察]=源码逐字 / [推断]=跨文件归并 / [未知]=证据不足。
> 基准固定 083/v83；095 线符号/IDB 不作功能依据（MEMORY.md critical）。当前端协议以 s9 为准（`s9-protocol-align/README.md:45`），凡「改回原生会断联机」一律交用户裁决、不得盲改。
> 工作区 `E:\project\MapleStory-Client`，分支 `feature/dev_0.0.1`。全程未改产品源码、未构建、未实机、未提交。

## §0 六条结论

1. **六面 EA 一致性成立**：Angel(13339) 与 BeiDou(13337) 是同一份 v83 二进制不同符号状态。本面 crosscheck 报 `844/841 / missing 3 / "layout differs"` 是 `tools/ida_angel_activity_crosscheck.py:107` 阈值 `same = present >= len(items) - 2` 的机械误判；3 个 missing EA（`0x462738 CBookDlg::OnKey` / `0x5617a1 CField_MassacreResult::OnPacket` / `0x6cec47 CNoticeQuestProgress::Update`）双端 `get_wide_byte` 8 字节逐字相同、BeiDou 侧仅 `get_func_attr` 未识别为函数 → **844/844 字节级一致**（见 `evidence/angel-beidou-activity-ea-bytelevel-audit.json` 与 r01 §1.1）。六面：登录 327/328、HUD 547/548、核心窗口 439/441、社交 752/752、宠物 968/968、活动 844/844。
2. **Angel 只补符号语义层，不补功能/版本**：`angel-activity-ea.json` 844 具名函数 / 73 类、`angel-activity-decompile-full.json` 267 函数逐字 Hex-Rays（3 个 `/output/` 补全 + 1 个 `CGrenade::JustBomb` disasm 兜底）、`angel-activity-facts.md` 3987 行 / `SP_` 238 个（54 个 `UI_UIWINDOWIMG_*`）。这些都是"命名 + 行为层"，不是新玩法。
3. **c2s 方向 8 个 Angel 锚点当前端落 4 个、整族零枚举 4 个**：Angel 267 函数内 `COutPacket::COutPacket(a1: N)` 全扫描得 `60/107/113/123/212/213/215/218`。当前端 `Net/OutPacket.h` 实存 `60 NPC_TALK_MORE`、`107 QUEST_ACTION`、`113 MAKER_SKILL`、`123 PLAYER_INTERACTION`；**零枚举 4 个 = 212/213/215/218（活动场地攻击/请求上行）**。★ 校正第一路子代理「60 缺失」误判：60 **实存、同值异名、语义复用**（v83 脚本应答通道被复用为 SoftKey tip 通道，`IO/UITypes/UIMonsterCarnival.cpp:97-137`、`IO/UITypes/UIPartyRace.cpp:51-70` 用 `NpcTalkMorePacket(tip).dispatch()`）。4 个零枚举是 **s9 适配冲突**，非"漏写"。
4. **s2c 方向全闭环**：`Net/PacketSwitch.cpp` 枚举 + emplace 已注册嘉年华 289–295 / 雪球 281–283 / 椰子 285–286 / 扭蛋 117,333,334 / RPS 312 / 公会Boss 287–288 / 新年卡 118 / 制造 217 / 活动提示 77,122,145,146,148,189,204,219,263,301,314 全套 handler；`opcode-matrix.json`（`generated_at=2026-11-08T00:56:27` / `s9_git_head=3d5ccc6f8e`）相应段 `HANDLED`/`MATCHED`。Q2 F1「211/213/218 明确不做」指的是 c2s 出站枚举，未区分 c2s/s2c 方向 = 文档口径债。
5. **「校准≠改回原生」六面一致再证**：Angel 用 `CMiniRoomBaseDlg`/`COmokDlg`/`CMemoryGameDlg`/`CRPSGameDlg`/13 个 `CField_*`/`CUI*Dlg`/`CScriptMan` 单体体系；当前端用 `UIMinigame` + `UIMinigameTable`（合并进 `UIMiscShells`）+ `UIRpsGame` + `MapGuildBoss` + `WaveAShellHandlers` 的 DIY 壳族，13 个 `CField_*` 仅 1 个 `MapGuildBoss` 有对应（且 `MapGuildBoss.cpp:19`「本端用时长近似」）。非一一对应，不得据 Angel 类名强改。
6. **真缺三层 + 095 污染**：整族缺（12 个 `CField_*`/4 个 MiniRoom 独立类/4 个活动 c2s 值/活动数据模型/多个独立 UI 窗）；壳占位（FieldMarker 黄字、UIMonsterCarnival 4-setter HUD、MatchCard 不画牌面、RpsGameHandler 全降级状态文本）；文档一致性（本面新增 `IO/UIStateGame.cpp:532` + `IO/UITypes/UISpecial.h:9` 两处 095 术语污染 + Q2 F1/§14.1/§9.5.4/§17 四处口径漂移 + `Net/Packets/PlayerInteractionPackets.h:36` 注释 314 vs 实际 123）。

## §1 取证基线与口径

- 端点：13339 `E:\MXD\反编译IDA\v83\`Angel.i64`（module `` `Angel.exe ``、`server_health` uptime 79508.649s、strings_cache 1281、hexrays_ready true）；13337 `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe.i64`（module `BeiDou.exe`、uptime 728379.662s、strings_cache 3889）；13338 headless 095 不用。
- 三联门禁：REA ✅（attached IDA profile 按 SHA-256 绑定、每轮先 `server_health`）／IDA ✅（`/mcp` initialize + `py_eval`，`def _scan()` 规避双作用域 `NameError`）／IMG ⚠️ **阻塞**（orange-wz 未注册本 DSH Host：`mcp_connector_status` 仅 `json-photoshop`、`mcp_connector_catalog(keyword=orange)`=0、`C:\Users\11036\.cursor\mcp.json:3-6` 仅 Cursor 侧声明；替代链 = `.nx` 节点引用 + `tools/nx_inspect.py` dump + 正确端截图库 `C:\Users\11036\Desktop\正确客户端截图\`）。
- 证据优先级（沿用 `feature-gap-inventory-2026-11/README.md:63`）：live runtime > 抓包 > 服务端当前 source（`s9 3d5ccc6f8e`）> 客户端 checked-in source > `.nx` dump > 既有文档 > 注释/死码。本面冲突一律按 checked-in source 取值。
- 六维 = A 功能 / B 业务 / C 逻辑 / D UI / E 排版 / F 样式 / G 交互；判定 ✅正确 / ⚠️偏差·需裁决 / ❌真缺 / ⏳待实机 / ⊘判不做。

## §2 c2s 八个 Angel 锚点逐字销案

Angel 侧（r01 §5，267 函数内 `COutPacket` 全扫描）：
| 值 | Angel 出处（逐字） |
|---|---|
| 60 | `CScriptMan::OnSay/OnAsk*`；`CScriptMan::OnAskSlideMenu` `COutPacket(60)+Encode1(14)+Encode1(1)+Encode4(v14)+Encode1(0)+SendPacket` |
| 107 | `CQuest::StartQuest` |
| 113 | `CUIItemMaker::RequestItemMake` `COutPacket(113)` + 11 个 `Encode4/Encode1` 字段序 |
| 123 | `CMiniRoomBaseDlg::SendInviteResult/SendCashInviteResult/OnCheckSSN2Static` |
| 212 | `CField_SnowBall::Update` |
| 213 | `CField_Coconut::BasicActionAttack` |
| 215 | `CField_GuildBoss::BasicActionAttack` |
| 218 | `CUIMonsterCarnival::RequestSend` `COutPacket(218) + Encode1(v3) + Encode4(*(this+727)-1) + SendPacket(dword_BE7914)` |

当前端 `Net/OutPacket.h:37-318`（148 枚举，头文件逐字「Opcodes for OutPackets associated with version 83 of the game」）：
- [观察] `:100 NPC_TALK_MORE = 60,` → **60 实存**。语义复用证据：`IO/UITypes/UIMonsterCarnival.cpp:97-137`（C/S/E/M 键走 `UISoftKey` 手输 + `NpcTalkMorePacket(tip).dispatch()`）、`IO/UITypes/UIPartyRace.cpp:51-70`（同模式）。判 ✅ 同值异名·非缺失。
- [观察] `:193 QUEST_ACTION = 107,` → ✅ 有。
- [观察] `:282 MAKER_SKILL = 113,` + `Net/Packets/MakerPackets.h:26` 逐字「制造出站包：Opcode MAKER_SKILL(0x71)，对接 s9 MakerProcessor(type=1 创建)」；`:30 MakerCreatePacket(int32_t toCreate) : OutPacket(OutPacket::Opcode::MAKER_SKILL)`、`:32 write_int(1) // type = create`、`:33 write_int(toCreate)`、`:35-40 if((toCreate/1000000)==1){write_byte(0); write_int(0);}` → ✅ 有构造点；⚠️ **字段序与 Angel 11 个 Encode4 不同（s9 适配简化）**，属"改回原生会断联机"类，交裁决，不盲改。
- [观察] `:261-263` 逐字「F5-2：出站必须用 s9 RecvOpcode.PLAYER_INTERACTION=0x7B=123（RecvOpcode.java:131）。/ 314 同样是抄了 Send 侧值；本枚举里 6 个构造点（交易/雇佣商人/迷你游戏）此前全部进不了服务端。/ `PLAYER_INTERACTION = 123,`」→ ✅ 有；迷你房/小游戏/交易合并此通道。`Net/Packets/PlayerInteractionPackets.h:93-144` 子 action 逐字 `MinigameReadyPacket 0x3A` / `MinigameStartPacket 0x3D` / `MinigameExitPacket 0x0A` / `MinigameOmokPlacePacket 0x3E (byte x,y,piece)` / `MinigameMatchCardSelectPacket 0x3F (byte turn,slot)`。
- [观察] 212/213/215/218 **OutPacket.h 零枚举**；`feature-gap-inventory-2026-11/README.md:232/:488` 把 211/213/218 记「明确不做」。→ ❌ 整族零枚举；**但** s2c 面 281+/289+ 已铺 handler（§3），故属 c2s 单向 s9 适配冲突，非服务端不支持玩法。判「交 D 决策点裁决，不盲改」。
- [观察] 其它相关枚举存在但 Angel 本面未显式命中：`:186 RPS_ACTION = 136`、`Net/Packets/RpsPackets.h:2 RPS_ACTION(136)`（当前端 RPS 走 136，Angel `CRPSGameDlg::OnPacket` 未取到 c2s 构造点 → [未知]，不臆造）；`:278 NEW_YEAR_CARD_REQUEST = 159`；`:130 GRENADE_EFFECT = 109`；`:190 NPC_ACTION = 197`；`:120 SCRIPTED_ITEM = 78`；`:310-317 RING_ACTION=137/WEDDING_ACTION=138/WEDDING_TALK=139`。

## §3 s2c 全闭环销案

`Net/PacketSwitch.cpp`（枚举 → emplace → handler 三段）：
- [观察] 枚举：`:103 WEEK_EVENT_MESSAGE=77`、`:176 SUCCESS_IN_USE_GACHAPON_BOX=117`、`:180-181 CASHSHOP_CASH_ITEM_GACHAPON_RESULT=333/CASHSHOP_CASH_GACHAPON_OPEN_RESULT=334`、`:185-190 MONSTER_CARNIVAL_START=289/OBTAINED_CP=290/PARTY_CP=291/MESSAGE=293/LEAVE=295/RPS_GAME=312`、`:194 NEW_YEAR_CARD_RES=118`、`:207 OX_QUIZ=145`、`:216 SCRIPT_PROGRESS_MESSAGE=122`、`:224 GMEVENT_INSTRUCTIONS=146`、`:226 CONTI_MOVE=148`、`:234 THROW_GRENADE=204`、`:240 ENERGY_ATTACK=189`、`:308 MAKER_RESULT=217`、`:310 SET_NPC_SCRIPTABLE=263`、`:314-316 MONSTER_CARNIVAL_SUMMON=292/MONSTER_CARNIVAL_DIED=294/WITCH_TOWER_SCORE_UPDATE=301`、`:365 KOREAN_EVENT=219`、`:379-383 SNOWBALL_STATE=281/HIT_SNOWBALL=282/SNOWBALL_MESSAGE=283/COCONUT_HIT=285/COCONUT_SCORE=286`、`:407-408 GUILD_BOSS_HEALER_MOVE=287/GUILD_BOSS_PULLEY_STATE_CHANGE=288`、`:163 PLAYER_INTERACTION=314`（s2c 侧 314 与 c2s 侧 123 **撞值方向差**，必须分列）、`:336 WEDDING_PHOTO=43`。
- [观察] emplace：`:464-466`（扭蛋三路）、`:476-481`（嘉年华 5 + RPS_GAME）、`:485` NEW_YEAR_CARD_RES、`:498/:507/:515/:525`（活动提示族）、`:531-535`（雪球/椰子 5）、`:539` PLAYER_INTERACTION→TradeHandler、`:583/:656/:658`（OX/Maker/NPC scriptable）、`:662-666`（公会Boss 2 + 嘉年华 Summon/Died + WitchTower）、`:686` WEDDING_PHOTO、`:715` KOREAN_EVENT、`:749` WEEK_EVENT_MESSAGE；`:1104-1110` opcode_msg 字符串表。
- [观察] handler 实现 `Net/Handlers/WaveAShellHandlers.cpp`：`:619 MonsterCarnivalStartHandler`、`:646 ObtainedCP`、`:661 PartyCP`、`:681 Message`、`:710 Leave`、`:716-759 RpsGameHandler`、`:1125 CoconutScore`、`:1137 HitSnowball`、`:1149 SnowballMessage`、`:1161 CoconutHit`、`:1184 SnowballState`、`:1212 OxQuiz`、`:2834 MakerResult`、`:2992 MonsterCarnivalSummon`、`:3025 MonsterCarnivalDied`（声明 `Net/Handlers/WaveAShellHandlers.h:89-113/219-242/587/611/617`）。`Net/Handlers/TradeHandlers.cpp:142 case 5: // ROOM：交易 / 个人店 / 雇佣商人 / 小游戏 / MiniRoomError 共用` 分流 MiniRoom/Omok/MatchCard。
- [观察] `opcode-matrix.json`：`:2129-2151` RPS_ACTION `client_value: 136 / s9_handler: RPSActionHandler / status: MATCHED`；`:3364-3401` PLAYER_INTERACTION `client_value: 123 / client_sent_count: 18 / s9_handler: PlayerInteractionHandler`；`:3607-3626` NEW_YEAR_CARD_REQUEST `client_value: 159 / MATCHED`；s2c 段 `:7874 SNOWBALL_STATE`、`:7994 MONSTER_CARNIVAL_START`、`:8297 RPS_GAME`、`:8327 PLAYER_INTERACTION(s9 314)`、`:5849 NEW_YEAR_CARD_RES`、`:6297 CONTI_MOVE`、`:8142 WITCH_TOWER_SCORE_UPDATE` 全 `HANDLED`。
- [推断] s2c 枚举/注册/handler 三者自洽、矩阵一致 → ✅ 全闭环；Q2 F1/§14.1「明确不做」只适用于 c2s 211/213/218，属**方向口径债**，列 D 决策点统一回写（不擅改 Q2）。

## §4 当前端源码逐类销案（与 r04 互证）

- [观察] `IO/UIElement.h:65-193` 活动 Type 枚举全在：`:65-66` CASHGACHAPON/REMOTEGACHAPON、`:70-72` ENERGYBAR/COMBOCOUNTER/BOSSHP、`:75-82` PARTYRACE/WEDDING/MONSTERCARNIVAL/RPSGAME/ARIANTMATCH/NEWYEARSCARD/MINIGAME/TREASUREBOX、`:101-102` SPEEDQUIZ/INITIALQUIZ、`:103-106` 逐字「NPC AskPet / AskPetAll（msgtype 9/10，IDA 7474A2 / 74775C）」「维度镜 SlideMenu（msgtype 14，IDA 76B3F0）」→ 与 Angel `CScriptMan` action code 9/10/14 **一一对应 ✅**；`:122 MAPLEEVENT`、`:125 QUESTALARM`、`:137-138 EVENTWINDOW/SLIDEMENU`、`:144 GAMEMENU`、`:151-152 MINIGAMETABLE/FLOATNOTICE`、`:174-175 EVENT/DAILYGIFT`、`:189-190 GUILDBOSS/MONSTERBOOK`、`:192-193 MAKER`、`:203`「实体化（Wave 6 素材线），本枚举+包类先行登记，暂不接线」、`:195-199 NUM_TYPES 尺寸警示`。
- [观察] 小游戏：`IO/UITypes/UIMinigame.h:2` 「Minigame lobby shell (Omok / MatchCard via PLAYER_INTERACTION — wire later)」、`UIMinigame.cpp:97/118` SoftKey 输 "x,y"/"turn,slot"；`IO/UITypes/UIMiscShells.h:925 UIMinigameTable`（合并进 UIMiscShells）/`:943-945 board[15][15]`；`UIMiscShells.cpp:7367 if(game_kind == 1)` 才画棋盘 → **game_kind==2 记忆翻牌牌面完全不渲染**、`:7385` 字母兜底 `Text stone(...,(p==1)?"B":"W")`；`IO/UITypes/UIRpsGame.cpp:31 LOG(LOG_WARN,"UIRpsGame: missing");` + 7 按钮薄壳。→ ⚠️/❌ 壳占位，非独立窗。
- [观察] 活动场地：`Gameplay/MapleMap/MapGuildBoss.h:3-5` 自带 IDA 锚点 `sub_5587B3/558B0F/558C0C/558C3B`、`.cpp:19`「原生 RelMove 由 Gr2D 插值；本端用时长近似（ms）」= 唯一真对象化；`Gameplay/Stage.h:80`「Field placeholders (summon/mist/door/kite/dragon) until full sprites」；`Gameplay/MapleMap/FieldMarker.cpp:19` `label(Text(..., Color::Name::YELLOW, ...))` 黄字；`WaveAShellHandlers.cpp:1169-1178/3019-3020` 借 `FieldMarker::Kind::SUMMON` 挂黄字。12 个 `CField_*` 的 `Map*` 类全 0 命中。→ ❌ 整族缺 + 壳占位。
- [观察] 活动 UI：`IO/UITypes/UIMonsterCarnival.h:25-28` 仅 `set_cp/set_enemy_cp/get_local_team/set_message` 4 setter（Angel 16 方法）；`UIMonsterCarnival.cpp:31 LOG(LOG_WARN,"UIMonsterCarnival: missing");`；`IO/UITypes/UIPartyRace.cpp:31`、`UINewYearsCard.cpp:2`、`UIEnergyBar.h:2` 同类薄壳。全库无 `UIMedalQuest*`/`UIEventAlarm`/`UIRevive`/`UIEnchantDlg`/`UITitle` 独立窗 Type 与文件。
- [观察] 095 污染本面 2 处：`IO/UIStateGame.cpp:532`「095 自定义中文 UI：暂时复用 EVENT 键打开福利中心（后续补独立 SPECIAL 键位 + 服务端 keymap）」、`IO/UITypes/UISpecial.h:9`「095 自定义中文 UI 入口：福利中心」→ UISpecial 是自造 DIY 非 v83 原生，却用 095 术语作注释依据；处置 = 只换注释依据/标注「DIY 非原生」，不改行为。跨面既有 `IO/Components/EquipTooltip.h:114`、`IO/UITypes/UIItemInventory.cpp:124/738/943`+`.h:50`、`UIMessenger.cpp` 10 处 + `.h:46/106/121-122/126/133`、`UIStatsInfo.cpp:146` 不在本面重复处理。
- [观察] `IO/UITypes/UIMonsterCarnival.cpp`、`IO/UITypes/UIEnergyBar.cpp` grep `v95|095|Maplestory95|port-095` **0 命中** → 本面污染只落在 UIStateGame/UISpecial 两处 + 文档层。

## §5 IMG / 排版 / 样式（门禁 + 待 r03 归档）

- [观察] orange-wz(IMG) MCP 未接入本 Host（§1）→ 六面一致记 ⚠️ 阻塞；替代证据 = `docs/features/ui-v83-window-restore/evidence/r15-newrefs/_nx_all/UIWindow.img_Minigame_{Omok,MemoryGame}_backgrnd*.png`、`UIWindow.img_UtilDlgEx_*`、`tools/nx_inspect.py` dump、正确端截图库。像素级视觉复核因 `read_image` 返回 `model "hy4free" does not declare image input` 未做，记 §8 限制。
- [观察] Angel 控件 id/坐标（r01 §4）与当前端壳无逐点对应：`CUIMonsterCarnival::OnCreate` 用 `SP_4102..4105/4098..4113/1262/1410/5527` + `CCtrlButton(3,126,8)/(2,140,8)/(4,149,28)` + `CCtrlTab(0,5,4,107,162,0)`；当前端 `UIMonsterCarnival.cpp:86-95` draw 仅 backgrnd + 3 行 Text。→ 排版层"已按无符号 BeiDou + 正确端截图对齐"这一跨面结论在本面**不成立**（活动窗无原生展开帧），须记 ⏳ 待实机/待 orange-wz。

## §6 冲突取值表（一律按 checked-in source / 当前 s9 source 取值）

| # | 冲突 | 旧账/子代理说法 | 取值（本面） | 依据 |
|---|---|---|---|---|
| 1 | c2s 60 是否缺失 | 子代理「60 脚本问答 0 命中·整族缺」 | ❌ 校正为「实存·同值异名·语义复用」，当前端落 4 个 c2s | `Net/OutPacket.h:100` + `UIMonsterCarnival.cpp:97-137` + `UIPartyRace.cpp:51-70` |
| 2 | 212/213/215/218 | Q2 F1「211/213/218 明确不做」 | c2s 整族零枚举·**s9 适配冲突**交裁决；s2c 281+/289+ 已闭环 | `OutPacket.h` 零命中 vs `PacketSwitch.cpp:379-383,476-480,531-535` |
| 3 | MAKER_SKILL(113) 字段序 | Angel 11 个 Encode4 | 当前端 s9 简化（type=1 + toCreate + 装备占位），不盲改 | `Net/Packets/MakerPackets.h:26-41` |
| 4 | 小游戏"两者都有" | `feature-gap:379` §9.5.4「两者都有（小游戏弱接线）」 | 实为 DIY 合并壳 + wire later + MatchCard 不画牌面 | `UIMinigame.h:2`、`UIMiscShells.cpp:7367` |
| 5 | Angel 方法数被当现状 | `feature-gap:626` §17 表列「CUIMonsterCarnival 16 / CField_Wedding 4」 | 这是 **Angel 侧目标证据**，当前端 `UIMonsterCarnival` 4 setter、无 `CField_Wedding` | `UIMonsterCarnival.h:25-28` + glob 0 命中 |
| 6 | c2s/s2c 撞值 | 混说 211/213/218「不做」 | 分列：c2s 123↔s2c 314 PLAYER_INTERACTION；c2s 212/213/218↔s2c 281+/289+ | `OutPacket.h:263` vs `PacketSwitch.cpp:163` |
| 7 | 083 原生 vs s9 字节布局 | 直接照搬 Angel | 一律 s9 联机优先，原生差异只作语义标注，改回须裁决 | `s9-protocol-align/README.md:45` + 前五面一致红线 |
| 8 | PLAYER_INTERACTION 注释 | `PlayerInteractionPackets.h:36` 写 314 | 实际出站 123（F5-2 已修），注释未同步 = 文档债 | `OutPacket.h:261-263` |

## §7 跨面共性与本面新增

- 跨面共性：`IO/UITypes/UIChatBar.h:123 change_target(int32_t) {}` 空 stub 死链；`CChatHelper::TryChat` 脏词/刷屏/重复句整族缺；`CCtrlMLEdit` IME 候选窗；095 注释污染累积（本面 +2）；Q2 清单多处同文件矛盾待统一回写（本面 +4）。
- 本面新增：**3 missing EA 的字节级根因**（首次出现 crosscheck 机械误判，已用双端 `get_wide_byte` 证伪，写 `evidence/angel-beidou-activity-ea-bytelevel-audit.json`）；**`CGrenade::JustBomb` decompile 失败的 disasm 兜底**（首次出现 `Decompilation failed at 0x58fbfc (address: 0x58fc17)`，改用 `disasm 0x58fbfc` 12 指令还原 `CUser::ShowSkillSpecialEffect(CSkillInfo::GetSkill(dword_BE78DC, this+0x44), 0x320, this, ...)`）；**4 个活动 c2s 单向 s9 适配冲突**（前五面 c2s 基本闭环，本面首次出现整族零枚举且属合理适配）。

## §8 限制与未知

- [未知] Angel `CRPSGameDlg::OnPacket` 未取到 c2s 构造点；`CScriptMan::OnAskYesNo/OnAskQuiz/OnAskSpeedQuiz/OnSayImage` 的 action code 未显式命中 → 不臆造。
- [未知] `CMovePath` 类同第五面未入 TARGETS/FOCUS 精选集，活动场地移动字节布局未取。
- [未知] IMG 层 orange-wz 门禁阻塞 + 无活动窗原生展开帧 + `read_image` 不可用 → 排版/样式像素级判定全部记 ⏳ 待实机/待 MCP 接入。
- [未知] 全部判定基于静态只读取证，未跑 `tools/build_smoke.bat` 全量 Rebuild、未做 1280×720 实机比对、未抓 `packet_dump.log`；`pending-live-tests` 历史「本环境抓屏全黑」限制沿用。

---
（主 agent 独立销案完毕；全程只读，未写产品源码、未构建、未实机、未提交。基准 083/v83，095 线不作功能依据。）
