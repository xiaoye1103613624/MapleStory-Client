# r04 · 当前端「活动·小游戏」源码盘点（第一路 subagent 全文归档 + 主 agent 销案复核）

> 来源：subagent `e96b0dbb-8600-44f0-ab07-aef91fbc6773`（已加载 `xiaoye-maple-story-dev`，全程只读、未改文件/未构建/未提交；基准 083/v83，095 线不作依据）。
> 结论标注：[观察]=源码逐字 / [推断]=跨文件归并 / [未知]=证据不足。
> 工作区 `E:\project\MapleStory-Client`，分支 `feature/dev_0.0.1`。

## §0 主 agent 前置复核块（★ 一处校正 + 七条销案）

1. **★ 校正子代理「60 脚本问答当前端 0 命中」这一条推断**：主 agent `grep Net/OutPacket.h` 得 **`Net/OutPacket.h:100 NPC_TALK_MORE = 60,`**（逐字）。且 `IO/UITypes/UIMonsterCarnival.cpp:97-137`、`IO/UITypes/UIPartyRace.cpp:51-70` 均用 `NpcTalkMorePacket(tip).dispatch()` 发包。→ Angel `CScriptMan::OnSay/OnAsk* COutPacket(60)` 与当前端 `NPC_TALK_MORE(60)` **值一致、命名不同、语义复用**（v83 脚本应答通道被当前端复用为 SoftKey tip 通道）。判「**同值异名 + 语义复用**」，**不是整族缺失**。故 8 个 c2s 锚点当前端落 **4 个**（60/107/113/123），真正整族零枚举的是 **4 个**（212/213/215/218 活动场地攻击上行）。
2. **4 个零枚举是 s9 适配冲突，不是「漏写」**：Angel 的 `CField_SnowBall::Update COutPacket(212)`/`CField_Coconut::BasicActionAttack COutPacket(213)`/`CField_GuildBoss::BasicActionAttack COutPacket(215)`/`CUIMonsterCarnival::RequestSend COutPacket(218)` 是 **v83 GMS 原生 c2s**；当前端协议以 s9 为准（`s9-protocol-align/README.md:45`），这些活动上行在 s9 侧走别的通道/由 s2c 包驱动。→ 一律标「**改回原生会断联机，须交用户裁决，不得盲改**」（六面一致红线）。
3. **s2c 面全闭环销案**：`Net/PacketSwitch.cpp:185-190,314-316,379-383,407-408` 枚举 + `:476-480,531-535,662-666` emplace 已注册 `MONSTER_CARNIVAL_*`/`SNOWBALL_*`/`COCONUT_*`/`GUILD_BOSS_*` 全套 handler，`opcode-matrix.json` 标 `HANDLED`。Q2 `feature-gap-inventory-2026-11/README.md:232` F1 / `:488` §14.1 把「211/213/218 明确不做」**未区分 c2s/s2c 方向**（211/213/218 是 c2s，281+/289+ 是 s2c），属文档口径债，列 D 决策点后统一回写（不擅改 Q2）。
4. **「校准≠改回原生」六面一致再证**：Angel 用 `CMiniRoomBaseDlg`/`CField_*`/`CUI*Dlg`/`CScriptMan` 单体体系；当前端用 `UIMinigame`+`UIMinigameTable`（合并进 `UIMiscShells`）+`UIRpsGame`+`MapGuildBoss`+`WaveAShellHandlers` 的 DIY 壳族，13 个 `CField_*` 仅 1 个 `MapGuildBoss` 有对应。
5. **UI 类型枚举齐全且自带 IDA 锚点**：`IO/UIElement.h:65-193` 活动相关 Type 全在（CASHGACHAPON/REMOTEGACHAPON/ENERGYBAR/MONSTERCARNIVAL/RPSGAME/MINIGAME/MINIGAMETABLE/MAKER/MONSTERBOOK/SPEEDQUIZ/INITIALQUIZ/ASKPET/DIMENSIONALMIRROR/SLIDEMENU/FLOATNOTICE/QUESTALARM/EVENTWINDOW/GAMEMENU/MAPLEEVENT/MONSTERKILLING/GUILDBOSS/NEWYEARSCARD/ARIANTMATCH/PARTYRACE/TREASUREBOX/…）；**`:103-106` 逐字**「NPC AskPet / AskPetAll（msgtype 9/10，IDA 7474A2 / 74775C）」「维度镜 SlideMenu（msgtype 14，IDA 76B3F0）」→ 与 Angel §5 action code 9/10/14 一一对应 ✅。
6. **095 污染本面新增 2 处**：`IO/UIStateGame.cpp:532`「095 自定义中文 UI：暂时复用 EVENT 键打开福利中心」+ `IO/UITypes/UISpecial.h:9`「095 自定义中文 UI 入口：福利中心」→ UISpecial 是自造 DIY 非 v83 原生，却用 095 术语作注释依据；处置 = 只换注释依据/标注「DIY 非原生」，不改行为。（跨面既有的 `UIMessenger.*`、`UIItemInventory.cpp:124/738/943`+`.h:50`、`UIStatsInfo.cpp:146`、`EquipTooltip.h:114`、`UIMessenger.h:106-133` 不属本面，见前五面记录。）
7. **真缺三层**：整族缺（12 个 CField_* / 4 个 MiniRoom 独立类 / 活动 c2s 4 值 / 活动数据模型 / UIMedalQuest*·UIEventAlarm·UITitle·UIRevive·UIEnchantDlg·UISlideNotice·UIAntiMacroCharacterName 独立窗）／壳占位（FieldMarker 黄字、UIMonsterCarnival 4-setter HUD、UIPartyRace、UIRpsGame 7 按钮薄壳、MatchCard 只数字 slot 不画牌面、WaveAShellHandlers LOG+跳过余量）／文档一致性（F1 c2s/s2c 混说、`:626` Angel 方法数被当现状、`:379`「两者都有」与 wire later 张力、`PlayerInteractionPackets.h:36` 注释 314 vs 实际 123）。

## 第 1 组 · 小游戏房间（MiniRoom / Omok / MemoryGame / RPS）

**文件清单**：`IO/UITypes/UIMinigame.h`/`.cpp`、`IO/UITypes/UIRpsGame.h`/`.cpp`、`IO/UITypes/UIMiscShells.h:925`/`.cpp:7324`（`UIMinigameTable` 合并于此）、`Net/Packets/PlayerInteractionPackets.h`、`Net/Packets/RpsPackets.h`、`Net/Handlers/TradeHandlers.cpp`（miniroom 分流）、`Net/Handlers/WaveAShellHandlers.{h,cpp}`（RpsGameHandler）、`Net/OutPacket.h:186,263`、`Net/PacketSwitch.cpp:163,190,481,539`。

**逐字关键行**
- [观察] glob 全库 `*MiniRoom*`/`*Omok*`/`*Memory*`/`*RPS*`/`*Game*Dlg*`：C++ 源码 **0 命中**；仅命中 `docs/features/port-095/evidence/opcode-v95/dec/_OnPacketBase_CMiniRoomBaseDlg__SAXAAVCInPacket___Z.c`、`_OnPacket_CRPSGameDlg__SAXAAVCInPacket___Z.c`（095 反编译，禁止作依据）、`docs/features/ui-v83-window-restore/evidence/r15-newrefs/_nx_all/UIWindow.img_Minigame_{Omok,MemoryGame}_backgrnd*.png`（资源图）、`includes/bass24/vb/Memory/*`、`Graphics/MemoryTexture.{h,cpp}`、`tools/level999/**/RPSActionHandler.class`（s9 服务端）。
- [观察] `IO/UITypes/UIMinigame.h:2` `//	Minigame lobby shell (Omok / MatchCard via PLAYER_INTERACTION — wire later)`；`:15` `static constexpr Type TYPE = UIElement::Type::MINIGAME;`；`:31` `enum Buttons : uint16_t { BtReady, BtStart, BtExit };`
- [观察] `IO/UITypes/UIMinigame.cpp:32` `nl::node root = UiAssetResolve::window("Minigame");`；`:34` `LOG(LOG_WARN, "UIMinigame: missing");`；`:54` `LOG(LOG_DEBUG, "UIMinigame: shell open");`；`:63-71` draw 仅 info 贴图 + tip_label + buttons；`:81-89` R/S/X 发 `MinigameReadyPacket()`/`MinigameStartPacket()`/`MinigameExitPacket()`；`:97-117` 'O' 键 `// SoftKey x,y[,piece] → Omok place` + `MinigameOmokPlacePacket(...).dispatch()`；`:118-130` 'M' 键 `// SoftKey turn,slot → MatchCard select` + `MinigameMatchCardSelectPacket(turn, slot).dispatch()`；`:131-139` 'L'→`UIMuruengRaid`、'P'→`UIClassMatch`。
- [观察] `IO/UITypes/UIRpsGame.h:12` `class UIRpsGame : public UIDragElement<PosNOTICE>`；`:15` `TYPE = UIElement::Type::RPSGAME`；`:33` `BtRock, BtPaper, BtScissor, BtStart, BtContinue, BtRetry, BtExit`。
- [观察] `IO/UITypes/UIRpsGame.cpp:29` `nl::node root = UiAssetResolve::window("RpsGame");`；`:31` `LOG(LOG_WARN, "UIRpsGame: missing"); return;`（has_assets false）；`:60-63` `void UIRpsGame::set_status(const std::string& msg) { status.change_text(msg); }`；`:65-69` `set_round_result(uint8_t selection, uint8_t answer)` 仅 `status.change_text(Locale::format_all("rps.round_result", {...}))`；`:71-79` draw 仅 backgrnd + status 文本 + draw_buttons；`:81-134` send_key 1/R/2/P/3/S/C/Y/X/T 全 `RpsActionPacket(...).dispatch()`；`:138-169` button_pressed 同样只发包。
- [观察] `IO/UITypes/UIMiscShells.h:925` `class UIMinigameTable : public UIDragElement<PosNOTICE> {`；`:927` `TYPE = UIElement::Type::MINIGAMETABLE`；`:935` `void set_game_kind(uint8_t kind); // 1=Omok 2=MatchCard`；`:943-945` `// Omok 15x15：0 空 / 1 黑 / 2 白` / `uint8_t board[15][15] = {};` / stone_black/stone_white/has_stones。
- [观察] `IO/UITypes/UIMiscShells.cpp:7328-7332` `has_assets = load_window_bg("MinigameTable", backgrnd, dimension);` → 回退 `load_window_bg("Minigame", ...)` → `else LOG(LOG_WARN, "UIMinigameTable: missing");`；`:7334` `// 棋子：Minigame/Omok 下常见 stone0/stone1 或 black/white`；`:7345` `LOG(LOG_DEBUG, "UIMinigameTable: Omok stone textures loaded");`；`:7358-7363` `note_place` 写 `board[y][x] = piece ? piece : 1;`；`:7364-7390` draw：`if (game_kind == 1)` 才画 15×15 棋盘，有贴图画 stone_black/white，否则 `:7385` 逐字 `Text stone(Text::Font::A11B, Text::Alignment::CENTER, c, (p == 1) ? "B" : "W");` 字母兜底（**game_kind==2 记忆翻牌不画任何牌面**）；`:7406` `// SoftKey "x,y" → Omok place；MatchCard 用纯数字 slot`；`:7424` `MinigameOmokPlacePacket(x, y, next_piece).dispatch();`；`:7412` `MinigameMatchCardSelectPacket(0, slot).dispatch();`；`:7439-7445` `// 15x15 Omok 粗映射到内容区；MatchCard 用行/列 slot` / `uint8_t cols = 4, rows = 5;`。
- [观察] `Net/Packets/PlayerInteractionPackets.h:36` `// 雇佣商人 / 个人商店出站包：Opcode PLAYER_INTERACTION(314)，与 s9 PlayerInteractionHandler Action 对齐。`（注释 314 vs 实际 123 → **文档债**）；`:93` `// 小游戏：READY(0x3A) / START(0x3D) / EXIT(0x0A)`；`:94-101` `MinigameReadyPacket` write_byte(0x3A)；`:103-110` `MinigameStartPacket` 0x3D；`:112-119` `MinigameExitPacket` 0x0A；`:121-132` `// 五子棋落子：action 0x3E → byte x, byte y, byte pieceType` + `MinigameOmokPlacePacket(uint8_t x, uint8_t y, uint8_t piece)`；`:134-144` `// 翻牌：action 0x3F → byte turn, byte slot` + `MinigameMatchCardSelectPacket(uint8_t turn, uint8_t slot)`。全部 `OutPacket(OutPacket::Opcode::PLAYER_INTERACTION)`。
- [观察] `Net/Packets/RpsPackets.h:2` `//	RPS_ACTION(136) — s9 RPSActionHandler`；`:10-23` `RpsActionPacket` 两 ctor，body `write_byte(mode)` / `(mode, answer)`。
- [观察] `Net/Handlers/TradeHandlers.cpp:142` `case 5: // ROOM：交易 / 个人店 / 雇佣商人 / 小游戏 / MiniRoomError 共用，靠后续字节区分`；`:145` `// s9 getMiniRoomError：ROOM + byte0 + status（15=仅自由市场等）`；`:171` `if (sub == 1) // getMiniGame：type=1`；`:173` `// s9 visitMiniGame 大致：slot + charLook + name + gameType(+wins…)`；`:188-189` `if (game_type == 1) tip += " · Omok"; else if (game_type == 2) tip += " · MatchCard";`；`:199-204` 打开 `UIMinigame`；`:525-534` 0x3D 分支 `table->clear_board(); table->set_tip("Playing");`；`:536-553` `case 0x3E: // Omok place broadcast` + `note_place(x,y,piece)` + `uimg->set_tip("Omok ("+x+","+y+")")`；`:556-562` `case 0x3F: // MatchCard select broadcast` + `std::string tip = "Card turn=" + ... + " #" + ...`。
- [观察] `Net/Handlers/` 无 MiniRoom/Omok/Memory/RPS 独立 handler 文件；`Net/Handlers/WaveAShellHandlers.h:132` `class RpsGameHandler : public PacketHandler`；`.h:2` `//	Wave A 壳开窗 handler：Hammer/Vega/Gacha/Family/Memo`。
- [观察] `Net/Handlers/WaveAShellHandlers.cpp:716-759` `RpsGameHandler::handle`：`LOG(LOG_DEBUG, "RpsGameHandler: mode=" ...)` → `emplace<UIRpsGame>()` → switch(mode) case 8 `"RPS ready"` / 0x06 `"Need ... mesos"` / 0x0B `ui->set_round_result(sel, ans)` / 0x0D `"Game closed"` / default `"RPS mode=" + ...` → `while (recv.available()) recv.skip_byte();`（全降级状态文本）。
- [观察] `Net/PacketSwitch.cpp:163` `PLAYER_INTERACTION = 314,`；`:539` `emplace<PLAYER_INTERACTION, TradeHandler>();`；`:190` `RPS_GAME = 312,`；`:481` `emplace<RPS_GAME, RpsGameHandler>();`。无 MINIROOM/OMOK/MEMORY s2c 枚举或 emplace；miniroom 走 314/TradeHandler 分流。

**回答「是否存在独立 UIOmok/UIMemoryGame/UIRPSGame 窗类，还是合并进壳」**：[观察] **UIOmok / UIMemoryGame 独立窗类不存在**（0 命中、无 Type 枚举）；五子棋与记忆翻牌合并进 `UIMinigameTable`（DIY 二合一，`game_kind` 1/2），且 `UIMinigameTable` 合并进 `UIMiscShells.{h,cpp}` 大壳族；房间壳为 `UIMinigame`。[观察] **UIRPSGame**：存在独立类 `UIRpsGame`（`UIRpsGame.h:12`，类名非 Angel `CRPSGameDlg`），但仅底板+状态文本+7 按钮薄壳，无动画/回合状态机。整体 = **DIY 合并重写，非 Angel 一一对应**。

## 第 2 组 · 活动场地（Field 子类）

**文件清单**：`Gameplay/MapleMap/MapGuildBoss.{h,cpp}`（**唯一**活动 Field 子类）、`Gameplay/MapleMap/FieldMarker.{h,cpp}`、`MapFieldMarkers.{h,cpp}`、`MapObjects.h`、`Gameplay/Stage.{h,cpp}`。其余 `MapCoconut*`/`MapBattlefield*`/`MapMonsterCarnival*`/`MapDojang*`/`MapSnowball*`/`MapWitchtower*`/`MapSpaceGAGA*`/`MapMassacre*`/`MapContiMove*`/`MapPartyRaid*`/`MapWedding*`/`MapLimitedView*`：**全部 0 命中**（全库 grep 仅 20 命中，全是 MapGuildBoss + `CField_GuildBoss` 注释）。

**逐字关键行**
- [观察] `Gameplay/MapleMap/MapGuildBoss.h:2` `//	公会 PQ（CField_GuildBoss）地图物件 —— healer / pulley 真驱动`；`:3-5` `//	证据：IDA sub_5587B3 读地图属性；sub_558B0F 分发 287/288；` / `//	      sub_558C0C 把 short Y 落到 healer 层；sub_558C3B 存 pulley state 字节。` / `//	样板图：Map9/990000900.img（莎莲娜王室）。`；`:17` `class MapGuildBoss`；`:27` `/// s2c 287：绝对 Y（原生 sub_558C0C 读 short，相对旧 Y RelMove 后覆写）。`；`:29` `/// s2c 288：滑轮状态字节（原生仅写入 field+507，交互门控用）。`；`:43-44` `Animation healer_anim;` / `Animation pulley_anim;`。
- [观察] `Gameplay/MapleMap/MapGuildBoss.cpp:19` `// 原生 RelMove 由 Gr2D 插值；本端用时长近似（ms），按 |ΔY| 与 rise/fall 估速。`；`:59-70` ctor 读 `map_src["healer"]` 的 x/yMin/rise/fall。
- [观察] `Gameplay/Stage.h:24-25` `#include "MapleMap/MapFieldMarkers.h"` / `#include "MapleMap/MapGuildBoss.h"`；`:80-81` `// Field placeholders (summon/mist/door/kite/dragon) until full sprites` / `MapFieldMarkers& get_field_markers();`；`:82-83` `/// 公会 PQ healer/pulley 物件（有地图属性时才加载）` / `MapGuildBoss& get_guild_boss();`；`:163-164` `MapFieldMarkers field_markers;` / `MapGuildBoss guild_boss;`。
- [观察] `Gameplay/Stage.cpp:139-140` `// 公会 PQ：地图根节点 healer/pulley（仅 990000900 等有）` / `guild_boss = MapGuildBoss(src);`；`:175` `field_markers.draw(id, viewx, viewy, alpha);`；`:180` `guild_boss.draw(viewpos, alpha);`；`:267` `field_markers.update(physics);`；`:293` `field_markers.check_doors(player.get_position());`。**Stage 里活动相关绘制仅 field_markers + guild_boss 两路**。
- [观察] `Gameplay/MapleMap/FieldMarker.h:2` `//	BeiDou V17 parity — lightweight field placeholder (summon/mist/door/kite/dragon)`；`:20` `// Placeholder map object until full Summon/Mist/Door sprites land.`；`:21` `// Tries Skill.wz effect/summon animation when skill_id != 0.`；`:22` `class FieldMarker : public MapObject`；`:25-42` Kind 枚举 SUMMON/MIST/DOOR/KITE/DRAGON/PORTAL/CATCH/CHALKBOARD/CHARBOX/ITEMFX/GUILDNAME（**无 COCONUT/SNOWBALL/CARNIVAL/DOJANG 活动 kind**）；`:56` `// 故把 label / base_label 降到 protected。其余仍是占位实现细节。`。
- [观察] `Gameplay/MapleMap/FieldMarker.cpp:19` `label(Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::YELLOW, Text::Background::NAMETAG, text)),`（**黄字文本名牌**）；`:27` `// 捕获圈原生无黄字名牌；有动画则清空文案。`；`:271` `// Catch / ItemFX 有动画时不再叠英文名牌；风筝白字叠在上方；其余场标保留黄字兜底`；`:312` `hp_cur = 100000; // unknown pool until first hit`。
- [观察] `Net/Handlers/WaveAShellHandlers.cpp:1169` `std::string tip = "Coconut #" + std::to_string(id) + " t" + std::to_string(type);`；`:1177-1178` `markers.spawn(oid, FieldMarker::Kind::SUMMON, pos + Point<int16_t>(id * 2, -40), tip, 0, 1500 + delay);`；`:3019-3020` `markers.spawn(oid, FieldMarker::Kind::SUMMON, pos + Point<int16_t>(tab * 10, -50), label, 0, 2500);`；`:3008` `return; // Carnival 已显，不叠 field marker`。

**逐项判定「真对象化渲染」还是「FieldMarker 黄字文本占位」**（[推断]）
| 场地 | 判定 |
|---|---|
| 公会 PQ (CField_GuildBoss) | **唯一真对象化渲染**：healer/pulley `Animation` + 态机（s2c 287/288），但 `:19`「时长近似」 |
| 怪物嘉年华 | **FieldMarker 黄字 / HUD 文本占位**：无 CField_MonsterCarnival 场地类；s2c 289-295 全降级 LOG + UIMonsterCarnival HUD + `markers.spawn(... SUMMON ...)` |
| 椰子 | **FieldMarker 黄字**：s2c 285/286 → LOG + `ensure_party_race()` set_score + `markers.spawn(... "Coconut #..." ...)` |
| 雪球 | **HUD 文本占位**：s2c 281/282/283 → LOG + UIPartyRace set_stage/set_score，无实体/滚动/碰撞 |
| 道场/战场/WitchTower/SpaceGAGA/Massacre/ContiMove/PartyRaid/Wedding/LimitedView | **整族 0 命中**：Map* 场地类源码不存在；仅 `WITCH_TOWER_SCORE_UPDATE=301`/`CONTI_MOVE=148` 等 s2c handler 空转 LOG/tip |

## 第 3 组 · 活动 UI

**文件清单**（`IO/UITypes/` 174 文件中活动相关）：存在 `UIEnergyBar`/`UICashGachapon`/`UIRemoteGachapon`/`UIRpsGame`/`UIMonsterCarnival`/`UINewYearsCard`/`UIMinigame`/`UIPartyRace`/`UIAriantMatch`/`UITreasureBox`/`UIEvent`/`UIDailyGift`/`UIGuildBoss`/`UIMonsterBook`/`UIMaker`/`UIMapleTV`/`UIViciousHammer`/`UIComboCounter`/`UIBossHP`/`UIClock`/`UIPlant`/`UIMTS`/`UISpecial`/`UIMiscShells`（约 70 壳类）。**不存在独立文件**：`UIMedalQuest*`/`UIEventAlarm`/`UICashItemGachapon`(实为 UICashGachapon)/`UIGameMenu`(在 UIMiscShells.h:802)/`UIRevive`/`UIEnchantDlg`/`UITitle`/`UISlideNotice`/`UISlideMenuDlg`(仅 UIFloatNotice 在 :949、UISlideMenu 在 :761、UIEventWindow 在 :750)/`UIAntiMacroCharacterName`/`UIQuestAlarm`(在 UIMiscShells.h:576)。

**逐字关键行**
- [观察] `IO/UIElement.h:65-66` CASHGACHAPON/REMOTEGACHAPON；`:70-72` ENERGYBAR/COMBOCOUNTER/BOSSHP；`:75-82` PARTYRACE/WEDDING/MONSTERCARNIVAL/RPSGAME/ARIANTMATCH/NEWYEARSCARD/MINIGAME/TREASUREBOX；`:98` ENCHANTSKILL；`:101-102` SPEEDQUIZ/INITIALQUIZ；`:103-106` ASKPET/DIMENSIONALMIRROR 带 IDA 锚点；`:122` MAPLEEVENT；`:125` QUESTALARM；`:137-138` EVENTWINDOW/SLIDEMENU；`:144` GAMEMENU；`:151-152` MINIGAMETABLE/FLOATNOTICE；`:174-175` EVENT/DAILYGIFT；`:189-190` GUILDBOSS 注释「healer Y / pulley state，s2c 287/288」；`:192-193` MONSTERBOOK/MAKER；`:203` NUM_TYPES。**无** UIOmok/UIMemoryGame/UITitle/UIRevive/UIAntiMacroCharacterName 枚举。
- [观察] `IO/UITypes/UIMonsterCarnival.h:12` `class UIMonsterCarnival : public UIDragElement<PosPARTYRACE>`；`:15` `TYPE = MONSTERCARNIVAL`；`:25-28` 仅 `set_cp`/`set_enemy_cp`/`get_local_team`/`set_message` 4 业务方法。
- [观察] `IO/UITypes/UIMonsterCarnival.cpp:29` `UiAssetResolve::window("MonsterCarnival")`；`:31` `LOG(LOG_WARN, "UIMonsterCarnival: missing"); return;`；`:45-60` 构造即 `UI::get().emplace<UISoftKey>(..., "avail/total|tip")` 手输 CP；`:86-95` draw 仅 backgrnd + 3 行 Text；`:97-137` send_key C/S/E/M 全走 SoftKey 手输 + `NpcTalkMorePacket(tip).dispatch()`。
- [观察] `IO/UITypes/UIPartyRace.h:12` `class UIPartyRace : public UIDragElement<PosPARTYRACE>`；`:25-27` set_stage/set_score/show_result；`UIPartyRace.cpp:31` `LOG(LOG_WARN, "UIPartyRace: missing");`；`:51-70` UISoftKey 手输 "win"/"lose"/"score:score"/stage + `NpcTalkMorePacket(tip).dispatch()`。
- [观察] `IO/UITypes/UINewYearsCard.h:14` `class UINewYearsCard : public UIDragElement<PosNOTICE>`；`UINewYearsCard.cpp:2` `//	NewYearsCard — SoftKey 收件人/留言 → NEW_YEAR_CARD_REQUEST`；`:26` `..., "New Year — SoftKey to/msg")`；`:77`/`:93` UISoftKey。
- [观察] `IO/UITypes/UIEnergyBar.h:2` `//	EnergyBar HUD — ENERGY_CHARGE gauge (UIWindow.img/EnergyBar)`；`:14` `TYPE = ENERGYBAR`；`:27` `static constexpr int16_t MAX_ENERGY = 10000;`；`:30` `bool has_assets = false;`。
- [观察] `IO/UIStateGame.cpp:530` `case KeyAction::Id::EVENT:`；`:532` `// 095 自定义中文 UI：暂时复用 EVENT 键打开福利中心（后续补独立 SPECIAL 键位 + 服务端 keymap）。` `emplace<UISpecial>();`；`:808-819` `auto silent_types = { ... ENERGYBAR, ... }`。[推断] UIStateGame 未逐窗注册 minigame/rps/carnival/newyearscard 的 send_key/emplace 入口；活动窗由 handler `UI::get().emplace<>()` 动态开窗。
- [观察] `Configuration.h`：`ui.position.*` 活动键 —— `:740` `PosNOTICE() : PointEntry("ui.position.notice", "(400, 285)")`（UIMinigame/UIRpsGame/UIMinigameTable 继承）；`:766-768` `PosEVENT` `"(99, 100)"`；`:775` `PosDAILYGIFT` `"(99, 80)"`；`:928-930` `PosCASHGACHAPON` `"(340, 160)"`；`:933-935` `PosREMOTEGACHAPON` `"(300, 120)"`；`:961` `PosPARTYRACE` `"(20, 80)"`（UIMonsterCarnival/UIPartyRace 继承）。**无** `ui.position.minigame`/`rps`/`monstercarnival`/`newyearscard`/`energybar`/`questalarm` 独立键（复用 notice/partyrace）。

## 第 4 组 · 协议 / 数据层

**c2s（`Net/OutPacket.h:37-318`）**
- [观察] `:36` `// Opcodes for OutPackets associated with version 83 of the game`；`:37` `enum Opcode : uint16_t`；`:100` `NPC_TALK_MORE = 60,`；`:186` `RPS_ACTION = 136,`；`:193` `QUEST_ACTION = 107,`；`:261-263` `/// F5-2：出站必须用 s9 RecvOpcode.PLAYER_INTERACTION=0x7B=123（RecvOpcode.java:131）。`/`/// 314 同样是抄了 Send 侧值；本枚举里 6 个构造点（交易/雇佣商人/迷你游戏）此前全部进不了服务端。`/`PLAYER_INTERACTION = 123,`；`:278` `NEW_YEAR_CARD_REQUEST = 159,`；`:282` `MAKER_SKILL = 113,`；`:310-317` RING_ACTION=137/WEDDING_ACTION=138/WEDDING_TALK=139；`:203` `/// 实体化（Wave 6 素材线），本枚举+包类先行登记，暂不接线。`；`:42-45` RELOG=0x1C 明确不做不占枚举位；`:299` OPEN_ITEMUI=236；`:295` USE_GACHA_EXP=158。全枚举 148 个。
- [观察] **OutPacket.h 中 0 命中**：MINIROOM/GAME_POLL/ENTER_MINIROOM/OMOK/MEMORY/MONSTER_CARNIVAL/COCONUT/SNOWBALL/BATTLEFIELD/DOJANG/PARTY_RAID/GACHAPON/EVENT/GUILD_BOSS/WITCH_TOWER 独立 c2s opcode。小游戏走 `PLAYER_INTERACTION(123)` 子 action（0x3A/0x3D/0x3E/0x3F）、RPS 走 `RPS_ACTION(136)`、新年卡走 `NEW_YEAR_CARD_REQUEST(159)`。
- **与 r01 §5 八个 c2s 锚点对照**（子代理原表 + ★ 主 agent 复核在 §0.1 校正 60）
| Angel 锚点 | 当前端 OutPacket.h | 判定（主 agent 校正后） |
|---|---|---|
| 60 脚本问答 | `:100 NPC_TALK_MORE = 60` | ✓ 同值异名（语义复用为 SoftKey tip），**非缺失** |
| 107 任务 | `:193 QUEST_ACTION = 107` | ✓ 有 |
| 113 道具制作 | `:282 MAKER_SKILL = 113` + `Net/Packets/MakerPackets.h:27-41` 构造点 | ✓ 有（字段序 s9 适配，见 r02 §4） |
| 123 迷你房邀请 | `:263 PLAYER_INTERACTION = 123` | ✓ 有（合并迷你房/小游戏/交易） |
| 212 雪球 | 无（文档 §9.2 的 0xD4 LEFT_KNOCKBACK(212) 是场地伤害族另一物） | 整族零枚举·s9 适配冲突 |
| 213 椰子 | 无 | 整族零枚举·s9 适配冲突 |
| 215 公会 Boss | 无（当前端公会 Boss 仅 s2c 287/288） | 整族零枚举·s9 适配冲突 |
| 218 怪物嘉年华 | 无 | 整族零枚举·s9 适配冲突 |

- [推断] 8 个 c2s 里当前端落 4 个（60/107/113/123），另 4 个（212/213/215/218）整族零枚举；Angel `CField_MonsterCarnival::OnPacket case 289–296` 是 s2c 方向 —— 当前端 s2c 289–295 已铺壳 handler，对应 c2s 一个未建。这是「活动场地 c2s 整族缺」硬证据，但须按 s9 适配冲突处理，交裁决。

**s2c（`Net/PacketSwitch.cpp`，枚举 + emplace）**
- [观察] `:103` WEEK_EVENT_MESSAGE=77；`:176` SUCCESS_IN_USE_GACHAPON_BOX=117；`:180-181` CASHSHOP_CASH_ITEM_GACHAPON_RESULT=333/CASHSHOP_CASH_GACHAPON_OPEN_RESULT=334；`:185-190` MONSTER_CARNIVAL_START=289/OBTAINED_CP=290/PARTY_CP=291/MESSAGE=293/LEAVE=295/RPS_GAME=312；`:194` NEW_YEAR_CARD_RES=118；`:224` GMEVENT_INSTRUCTIONS=146；`:226` CONTI_MOVE=148；`:314-316` MONSTER_CARNIVAL_SUMMON=292/MONSTER_CARNIVAL_DIED=294/WITCH_TOWER_SCORE_UPDATE=301；`:336` WEDDING_PHOTO=43；`:365` KOREAN_EVENT=219；`:379-383` SNOWBALL_STATE=281/HIT_SNOWBALL=282/SNOWBALL_MESSAGE=283/COCONUT_HIT=285/COCONUT_SCORE=286；`:407-408` GUILD_BOSS_HEALER_MOVE=287/GUILD_BOSS_PULLEY_STATE_CHANGE=288；`:163` PLAYER_INTERACTION=314。
- [观察] emplace：`:464-466` Gachapon 三路；`:476-481` MONSTER_CARNIVAL_START/OBTAINED_CP/PARTY_CP/MESSAGE/LEAVE + RPS_GAME；`:485` NEW_YEAR_CARD_RES；`:515` GMEVENT_INSTRUCTIONS；`:517` CONTI_MOVE；`:531-535` Snowball/HitSnowball/SnowballMessage/CoconutHit/CoconutScore；`:539` PLAYER_INTERACTION→TradeHandler；`:662-666` GUILD_BOSS_HEALER_MOVE/PULLEY/MONSTER_CARNIVAL_SUMMON/DIED/WITCH_TOWER_SCORE_UPDATE；`:686` WEDDING_PHOTO；`:715` KOREAN_EVENT；`:749` WEEK_EVENT_MESSAGE。
- [观察] **PacketSwitch.cpp 0 命中** MINIROOM/OMOK/MEMORY/DOJANG/BATTLEFIELD/PARTY_RAID/SPACE_GAGA/MASSACRE/LIMITEDVIEW s2c 枚举与 emplace。

**opcode-matrix.json 状态**
- [观察] `:39` `"generated_at": "2026-11-08T00:56:27"`；`:41` `"s9_git_head": "3d5ccc6f8e48f79be266448eee28cf1a72e75f30"`；`:29-30` `client_out_constants=148 / client_out_sent=148`。
- [观察] `:2129-2151` RPS_ACTION `client_value: 136` / `s9_handler: RPSActionHandler` / `status: MATCHED`；`:3364-3401` PLAYER_INTERACTION `client_value: 123` / `client_sent_count: 18` / `s9_handler: PlayerInteractionHandler`；`:3607-3626` NEW_YEAR_CARD_REQUEST `client_value: 159` / `status: MATCHED`；s2c-only 段均 `HANDLED`：`:7874` SNOWBALL_STATE、`:7994` MONSTER_CARNIVAL_START、`:8297` RPS_GAME、`:8327` PLAYER_INTERACTION(s9 314)、`:5849` NEW_YEAR_CARD_RES、`:6297` CONTI_MOVE、`:8142` WITCH_TOWER_SCORE_UPDATE。
- [推断] 矩阵与 PacketSwitch.cpp 注册一致，无文档债；「明确不做」的只是 c2s 出站枚举，非 s2c 收包面。

**活动 handler 构造/降级点（`Net/Handlers/WaveAShellHandlers.cpp`）**
- [观察] `:619-638` MonsterCarnivalStartHandler LOG + `emplace<UIMonsterCarnival>()` + set_cp/set_enemy_cp/set_message；`:716-759` RpsGameHandler（见第 1 组）；`:1125-1135` CoconutScoreHandler LOG + `ensure_party_race()->set_score(maple, story)` + `// note`；`:1137-1147` HitSnowballHandler LOG + set_stage；`:1149-1159` SnowballMessageHandler LOG + set_stage；`:1161-1182` CoconutHitHandler tip + FieldMarker spawn；`:1184-1209` SnowballStateHandler LOG + ensure_party_race；`:2834` MakerResultHandler；`:2992-3023` MonsterCarnivalSummonHandler LOG + UIMonsterCarnival/FieldMarker；`:3025-3037` MonsterCarnivalDiedHandler LOG + HUD tip。
- [观察] `:99` `Optional<UIPartyRace> ensure_party_race()`（13 处调用 `:1132,1144,1157,1205,1238,1586,1633,1646,1660,1679,2045,3063`）；`.h:338` `// s9 field / event shells`。
- [推断] 降级模式统一「读包 → LOG → `recv.skip_byte()` 吞余量 → `emplace<壳窗>` 或 `markers.spawn(...SUMMON...)` 黄字」，`// note` 空实现标记散见 `:621,1134,1156,1181,1186,1209`。

**数据模型（`Character/` + `Gameplay/`）**
- [观察] 全库 grep CARNIVAL/COCONUT/SNOWBALL/OMOK/MINIGAME/RPS/CARD/GACHAPON/DOJANG/BATTLEFIELD/EVENT/RAINBOW/CPQ/WITCH 在 `Character/` 仅 1 命中 `Character/CharEffect.h:33` `MONSTER_CARD,`（角色特效枚举）。`Gameplay/` 无活动数据模型类（唯一 MapGuildBoss 是渲染态机）。[推断] 当前端**无独立活动/小游戏数据模型**（无 CP 计数、无回合状态、无场地积分对象），状态全散在 UI 壳 int 成员与 handler 局部变量。

## 第 5 组 · 预留 / stub / TODO / placeholder 逐字行 + 文档债

**活动/小游戏相关命中（路径:行号 + 逐字）**
- `IO/UITypes/UIMinigame.h:2` `//	Minigame lobby shell (Omok / MatchCard via PLAYER_INTERACTION — wire later)`
- `IO/UITypes/UIMinigame.cpp:2` `//	Minigame lobby shell` / `:34` `LOG(LOG_WARN, "UIMinigame: missing");` / `:54` `LOG(LOG_DEBUG, "UIMinigame: shell open");` / `:97` `// SoftKey x,y[,piece] → Omok place` / `:118` `// SoftKey turn,slot → MatchCard select`
- `IO/UITypes/UIRpsGame.cpp:31` `LOG(LOG_WARN, "UIRpsGame: missing");`
- `IO/UITypes/UIMonsterCarnival.h:2` `//	MonsterCarnival HUD` / `UIMonsterCarnival.cpp:31` `LOG(LOG_WARN, "UIMonsterCarnival: missing"); return;`
- `IO/UITypes/UIPartyRace.h:2` `//	PartyRace HUD shell` / `UIPartyRace.cpp:31` `LOG(LOG_WARN, "UIPartyRace: missing");`
- `IO/UITypes/UINewYearsCard.h:2` `//	NewYearsCard UI shell` / `UINewYearsCard.cpp:2` `//	NewYearsCard — SoftKey 收件人/留言 → NEW_YEAR_CARD_REQUEST` / `:26` `..., "New Year — SoftKey to/msg")`
- `IO/UITypes/UIMiscShells.cpp:7332` `else LOG(LOG_WARN, "UIMinigameTable: missing");` / `:7385` `Text stone(Text::Font::A11B, Text::Alignment::CENTER, c, (p == 1) ? "B" : "W");` / `:7406` `// SoftKey "x,y" → Omok place；MatchCard 用纯数字 slot` / `:7439` `// 15x15 Omok 粗映射到内容区；MatchCard 用行/列 slot`
- `IO/UITypes/UIMiscShells.cpp:1105` `// 壳：UIWindow.img/SpeedQuiz/backgrnd 286×410（IMG + IDA 7EB303 / StringPool 3924）` / `:1464` `// 壳：UIWindow.img/InitialQuiz/backgrnd 288×229（IMG + IDA 7E99BC）` / `:1592` `// IDA 7EAE53：len ∈ [min,max]；tip StringPool 3920/3921（英文明文占位）`
- `IO/UITypes/UIMiscShells.cpp:5602-5603` `// 面交由服务端 ROOM(5)/3 驱动 UITrade；UITradingRoom 是不接实时数据的 DIY 壳，` / `// 快捷键统一指向真实窗，避免两个交易窗并存、DIY 壳发出错包。` / `:4188` `// UIOptionMenu 只剩空壳，所以直接落到 UIGameOpt（见 R27-B7-4）。`
- `Gameplay/Stage.h:80` `// Field placeholders (summon/mist/door/kite/dragon) until full sprites` / `:143` `// NO-NATIVE-EXACT）。Stage::clear() 换图时清空。`
- `Gameplay/MapleMap/FieldMarker.h:2` `//	BeiDou V17 parity — lightweight field placeholder (summon/mist/door/kite/dragon)` / `:20` `// Placeholder map object until full Summon/Mist/Door sprites land.` / `:21` `// Tries Skill.wz effect/summon animation when skill_id != 0.` / `:56` `// 故把 label / base_label 降到 protected。其余仍是占位实现细节。`
- `Gameplay/MapleMap/FieldMarker.cpp:19` `label(Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::YELLOW, Text::Background::NAMETAG, text)),` / `:27` `// 捕获圈原生无黄字名牌；有动画则清空文案。` / `:122` `// Evan dragon placeholder — skill effect or BasicEff` / `:271` `// Catch / ItemFX 有动画时不再叠英文名牌；风筝白字叠在上方；其余场标保留黄字兜底` / `:312` `hp_cur = 100000; // unknown pool until first hit`
- `Gameplay/MapleMap/MapGuildBoss.cpp:19` `// 原生 RelMove 由 Gr2D 插值；本端用时长近似（ms），按 |ΔY| 与 rise/fall 估速。`
- `Net/OutPacket.h:42-45` `//（RELOG=0x1C 已裁决"明确不做"，不占枚举位...门禁不变量——OutPacket 枚举全部有构造点。）` / `:203` `/// 实体化（Wave 6 素材线），本枚举+包类先行登记，暂不接线。`
- `Net/Packets/PlayerInteractionPackets.h:36` `// 雇佣商人 / 个人商店出站包：Opcode PLAYER_INTERACTION(314)...`（注释 314 vs 实际 123 → 文档债）
- `Net/Handlers/WaveAShellHandlers.h:2` `//	Wave A 壳开窗 handler：Hammer/Vega/Gacha/Family/Memo` / `:338` `// s9 field / event shells` / `.cpp` `// note` 空实现标记 `:621, :1134, :1156, :1181, :1186, :1209`
- `IO/UIStateGame.cpp:532` `// 095 自定义中文 UI：暂时复用 EVENT 键打开福利中心（后续补独立 SPECIAL 键位 + 服务端 keymap）。`
- `IO/UIElement.h:195-196` `// v83 目录气球的两颗独立设置窗...UIOptionMenu 只剩空壳，故另立两型。`

**文档债（读数与 checked-in source 不一致）**
- [观察·一致] `docs/features/feature-gap-inventory-2026-11/README.md:5` `> 基准裁决：...功能与资源一律参照 **GMS 083 原版**...095 线（浩浩岛095 / PLUS2 / 怀旧岛192 / Maplestory95.i64）不作本线参照与入库源。`
- [推断·债] `feature-gap-inventory-2026-11/README.md:232` §8 F1 `| F1 | 雪球 SNOWBALL(211) / 椰子 COCONUT(213) / 怪物嘉年华 MONSTER_CARNIVAL(218) | ⊘ 明确不做 | 活动小游戏/内容，维持既有裁决 |`、`:488` §14.1 `| 0x00D3/0x00D5/0x00DA | SNOWBALL / COCONUT / MONSTER_CARNIVAL | 活动小游戏/内容 |`、`parity-gap-2026-11/README.md:83` `211/213/218 雪球/椰子/嘉年华`（均「明确不做」）—— **但** `Net/PacketSwitch.cpp:379-383,476-480,531-535,664-665` 已注册 SNOWBALL_*/COCONUT_*/MONSTER_CARNIVAL_* 全套 s2c handler 且 `opcode-matrix.json` 标 HANDLED。「明确不做」的其实只是 **c2s 出站枚举**（OutPacket.h 确实零命中），s2c 收包面已铺壳 handler —— 文档未区分 c2s/s2c 方向（211/213/218 是 c2s，281+/289+ 是 s2c）→ 口径混说属文档债；`:493` §9.5 已自我校正婚礼链（137/138/139 ✅），但 F1 未同步这一层区分。
- [推断·债] `feature-gap-inventory-2026-11/README.md:626` `| 活动小游戏 | CUIMonsterCarnival / CField_Wedding | 0x86e047..0x8707c4 / 0x5814fd..0x581b78 | 16 / 4 |` 是 §17「Angel 083 带符号 IDB 可直接补的原生证据」表（`:610-633`），列的是 **Angel 侧方法数**（16/4）；当前端 `UIMonsterCarnival` 无 16 方法、`CField_Wedding` 源码 0 命中 → 需标「目标证据非现状」。
- [推断·债] `feature-gap-inventory-2026-11/README.md:233` F2 `r40 已铺 MinigameTable 并接 PLAYER_INTERACTION ROOM/READY/START/EXIT` 与源码一致，但未记 `UIMinigameTable` 实为合并进 `UIMiscShells` 而非独立窗文件、且无 MemoryGame 独立实现（MatchCard 借 game_kind==2 数字 slot）。
- [推断·债] `feature-gap-inventory-2026-11/README.md:379` §9.5.4 `| MonsterBook / 现金礼物 / 个人商店 / 小游戏 Omok·MatchCard | ✓ | ✓ | 两者都有（小游戏弱接线） | ... UIMinigame.h:2「via PLAYER_INTERACTION — wire later」 |` —— 「两者都有」与 wire later 张力；当前端无 MemoryGame 独立实现。
- [观察·一致] `feature-gap-inventory-2026-11/README.md:31` `WaveAShellHandlers.cpp 4802 行壳窗入站专批（168 个 XHandler::handle(InPacket&)，仅 4 空参）`、`:30` `UIMiscShells.h（1037 行）约 70 个 UIXxx 壳类（.../UIMinigameTable/UIFloatNotice/...）`、`:220` E20 `| E20 | Summon/Dragon/Door/Portal/... 场标 → 真精灵 | ❌ 真缺·...余者仅 FieldMarker 文本 | Gameplay/Stage.h:80；...` 与 `Gameplay/Stage.h:80` 逐字一致。
- [观察·一致] `opcode-matrix.json` 计数与 OutPacket.h / PacketSwitch.cpp 源码注册一致，无债。

## 三个结论（明确回答）

### ① 当前端是否是 Angel 类群（CMiniRoomBaseDlg 15 / COmokDlg 1 / CMemoryGameDlg 1 / CRPSGameDlg 1 / 13 个 CField_* / CUIMonsterCarnival 16）的一一对应？
**否。** [推断] CMiniRoomBaseDlg/COmokDlg/CMemoryGameDlg/CRPSGameDlg 全库源码 0 命中（唯一命中是 `docs/features/port-095/evidence/opcode-v95/dec/*` 095 反编译，禁止作依据）；对应实现是 `UIMinigame`（lobby 壳）+ `UIMinigameTable`（合并进 UIMiscShells 的 15×15 棋盘，Omok/MatchCard 二合一）+ `UIRpsGame`（独立薄壳）。13 个 `CField_*` 中 12 个源码 0 命中，唯一 `MapGuildBoss`（对应 CField_GuildBoss，但类名体系 Map* vs CField_* 不同，且 `MapGuildBoss.cpp:19`「时长近似」）。`CUIMonsterCarnival` 16 方法 → 当前端 `UIMonsterCarnival.h:25-28` 仅 4 setter 的 HUD 壳。Angel 用 CXXX/CField_/CUI 体系，当前端用 UIXXX/MapXXX 体系 = **DIY 重写 + 壳族合并**，非符号一一移植。

### ② 小游戏房间与活动场地是「真玩法+真渲染已实现」还是「壳/文本场标占位」？
- **五子棋 Omok** = 壳/DIY 合并。有真 `board[15][15]` + 棋子贴图渲染（`UIMiscShells.cpp:7364-7390`），但无独立窗类、无胜负/禁手/计时状态机、落子靠 SoftKey 输 "x,y"、无资源 `"B"/"W"` 字母兜底（`:7385`）。
- **记忆翻牌 MemoryGame** = 纯数据/文本占位。无独立窗类，复用 `UIMinigameTable`（game_kind==2），`draw()` 仅 `if (game_kind == 1)` 画棋盘 → **翻牌牌面完全不渲染**，仅 4×5 slot 数字映射 + 发包。
- **猜拳 RPS** = 独立薄壳 `UIRpsGame`。backgrnd + status 文本 + 7 按钮，`set_round_result` 只 change_text，无手势动画/回合状态机/胜负判定，全靠 RpsActionPacket 发包。
- **MiniRoom 房间框架** = 壳。无独立基类，`TradeHandlers.cpp:142` `case 5: // ROOM：交易 / 个人店 / 雇佣商人 / 小游戏 / MiniRoomError 共用` 分流到 UIMinigame。
- **公会 PQ / CField_GuildBoss** = **唯一真对象化渲染**（healer/pulley Animation + 态机，s2c 287/288），但「时长近似」。
- **怪物嘉年华 / 椰子 / 雪球** = **FieldMarker 黄字文本 / HUD 文本占位**（`WaveAShellHandlers.cpp:1169-1178,3019-3020` 借 FieldMarker::Kind::SUMMON 挂黄字；`FieldMarker.cpp:19` Color::Name::YELLOW），无场地实体。
- **武陵道场 / 战场 / WitchTower / SpaceGAGA / Massacre / ContiMove / PartyRaid / Wedding / LimitedView** = **整族 0 命中**，仅 s2c handler 空转 LOG/tip。
- [观察] 总判据：`Stage.h:80` `// Field placeholders (summon/mist/door/kite/dragon) until full sprites`；`FieldMarker.h:20` `// Placeholder map object until full Summon/Mist/Door sprites land.`；`Stage.cpp:175,180` 仅 `field_markers.draw(...)` + `guild_boss.draw(...)` 两路活动绘制。

### ③ 三类分列
**[整族缺失]**（Angel 有、当前端源码 0 命中）
- `CMiniRoomBaseDlg`(15)/`COmokDlg`(1)/`CMemoryGameDlg`(1)/`CRPSGameDlg`(1) 独立类族及方法群 → glob 0 命中（095 反编译除外）。
- 13 个 `CField_*` 中 12 个（Coconut/Battlefield/MonsterCarnival/Dojang/Snowball/Witchtower/SpaceGAGA/Massacre/ContiMove/PartyRaid/Wedding/LimitedView）→ Map* 类 0 命中（仅 MapGuildBoss）。
- `UIOmok`/`UIMemoryGame` 独立窗类 + UIElement::Type 枚举 → 0 命中。
- **活动场地 c2s 4 值**（212/213/215/218；经 §0.1 校正后 60 不在此列）→ OutPacket.h 零枚举，须按 s9 适配冲突交裁决。
- `CUIMonsterCarnival` 16 方法族 → 仅剩 4 setter 的 HUD 壳。
- `UIMedalQuest*`/`UIEventAlarm`/`UIAntiMacroCharacterName`/`UISlideNotice`/`UIRevive`/`UIEnchantDlg`/`UITitle` → IO/UITypes 174 文件 + UIElement.h Type 枚举双 0 命中。
- `Character/`+`Gameplay/` 活动数据模型 → 0 命中（仅 `Character/CharEffect.h:33` `MONSTER_CARD,`）。

**[壳占位 / DIY 合并重写]**
- `UIMinigame`（lobby 壳 `wire later`）、`UIMinigameTable`（Omok 15×15 + MatchCard 数字 slot DIY 二合一，合并进 UIMiscShells）、`UIRpsGame`（独立薄壳）、`UIMonsterCarnival`/`UIPartyRace`（HUD 文本壳，SoftKey 手输）、`UINewYearsCard`/`UICashGachapon`/`UITreasureBox`/`UIEvent`/`UISpecial`（薄壳）、`MapGuildBoss`（唯一真对象场地但时长近似）、`FieldMarker`/`MapFieldMarkers`（黄字文本占位，Kind 无活动 kind）。
- 全部活动 s2c handler 集中 `WaveAShellHandlers.cpp`（4802 行壳窗入站专批），降级 LOG + `emplace<壳窗>` + FieldMarker 黄字；`TradeHandlers.cpp:142` 把 MiniRoom/Omok/MatchCard 并入 `case 5 ROOM`。
- `IO/UIStateGame.cpp:532` 复用 EVENT 键开自造「福利中心」UISpecial（DIY）。

**[095 专有判不做]**
- `docs/features/port-095/evidence/opcode-v95/dec/_OnPacketBase_CMiniRoomBaseDlg__...c`、`_OnPacket_CRPSGameDlg__...c` → 095 反编译，逐字裁决 `feature-gap-inventory-2026-11/README.md:5` 不作依据。
- `feature-gap-inventory-2026-11/README.md:499` 韩系导师 Academy（只在 095 线符号出现，v83 中文「学院」=家族 action26 UIFamily 已闭环）、`:500` 远征独立招募面板 BossParty/ExpeditionIntermediary（095 线证据）、`:501` 095 PLUS2/怀旧岛192/浩浩岛 系资源 → 不做。
- `IO/UIStateGame.cpp:532` `// 095 自定义中文 UI：暂时复用 EVENT 键打开福利中心...` → 自造 UISpecial 非 v83 原生（`feature-gap-inventory-2026-11/README.md:237` §8 F6 `| UISpecial 自造「福利中心」5 钮发 SpecialRequestPacket(0..4) | 🟡 推断非 v83 原生 | 95 行薄窗 |`）。

---
（第一路交付完毕；全程只读，未写任何文件、未构建、未提交。基准 083/v83，未引 095 线符号/IDB 作功能依据。）
