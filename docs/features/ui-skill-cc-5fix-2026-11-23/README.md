# 客户端 5 项问题一次性修复（2026-11-23）

分支 `feature/dev_0.0.1`。用户一次性提出 5 项问题，要求点到面、一次性全改完。本文记录证据、逐条结论、改动与真机验收清单。

## 截图来源端确认
用户明确确认三张截图均来自**当前本端（修复后客户端）**，可作为本端问题证据。

---

## 问题1 — 换频道回到"登录/上次地图点位"

### 根因（代码实证，服务端 BeiDou-Server_s9）
- 换频道落点由 `Character.newClient()` 决定（全仓唯一调用点 `PlayerLoggedinHandler.java:226`，仅 `!newcomer` 分支=换频道/断线重连触发，正常登录走 DB 加载不经此）。
- 原逻辑 `Character.java:969`：`findClosestPlayerSpawnpoint(getPosition())` 用换线前**旧坐标**吸附到最近 spawn 点（`MapleMap.java:2680-2691`，筛选 type 0/1 且 targetMapId=NONE），导致落点飘忽/回到上次点位。
- `change_channel_force_return` 开关（`Client.java:1552`）对绝大多数地图是 no-op（`:1558` hp<1 分支因 `:1506` isAlive 守卫成死代码），改它无效。

### 改动（只改源码，未编译——按用户选择）
- `gms-server/.../client/Character.java` `newClient()`：把"按旧坐标找最近出生点"改为"取 id 最小（固定）的 SPAWN/INVISIBLE 且无目标 portal"（通常是 id=0），即该地图固定出生点；找不到再回退 `getPortal(0)`。
- 影响面：仅换频道/断线重连落点；正常登录不受影响（不经过 newClient）。
- 生效方式：需用户自行 Maven 重编译打包并重启服务端。未做真机验收。

---

## 问题2 — 明珠港空中"四个传送门光柱"

### 定性（orange-wz 直读 live 资源实证）
- 本端 live `Data/Map/Map/Map1/104000000.img` 有 6 个 `pt=6 (WARP)` 门；红框 4 道光柱精确对应 portal 42=x4738/y316、43=x4823/y317、44=x4882/y316、45=x4956/y316。
- 这 4 个门 `tm=999999999`（`Portal.h:72 WarpInfo::valid = mapid < 999999999` → 无效），是**地图原生装饰性传送点**，非服务端下发、非别的门渲染错位。
- 红框平台附近无 NPC life / 地图 obj / 背景光柱（已用 orange-wz 扫描天空区域 x4600-5100 确认）。
- "活动向导"名字标签对应 NPC 图像缺失：`Npc.cpp:164-165` 按 `animations.count(stance)` 画图像、`:201-206` 无条件画名牌，两条独立数据源；图像缺失属 link.img 缺帧资源问题，"□"是 A13B 字形覆盖问题（此项用户本轮未要求修复）。

### 改动
- 用户选择"隐藏空中光柱"。`Gameplay/MapleMap/MapPortals.cpp:52`：构造时过滤 `type==WARP && target_id>=999999999` 的无效装饰门，不进渲染列表。对所有地图通用，对玩法零影响。

---

## 问题3 — 瞬移/二段跳特效、间隔、卡地图

### 根因与对照
- Angel IDA（`docs/features/skill-angel-calibration-2026-11/evidence/angel-skill-facts.md`）：`TryRegisterTeleport@0x957b74` 播 `BasicEff.img/Teleport` + 120ms 节流 + foothold 落点吸附；`DoActiveSkill_BoundJump@0x96be38` 播 `BasicEff.img/Flying`。
- orange-wz 实测：live `Effect/BasicEff.img` 中 Teleport / Flying / Flying1 节点均存在（资源齐全，缺的是代码播放）。
- 本端：`Skill` 构造对瞬移/二段跳落 `NoUseEffect`；`apply_teleport` 用 `set_x` 无落点校验；无本地节流（全靠服务端回 CD 包）。

### 改动（子代理实现，MSBuild 全量编译通过）
- `Gameplay/Combat/SkillUseEffect.h/.cpp`：新增 `BasicEffUseEffect`（从 `Effect/BasicEff.img/<path>` 加载动画，apply 走 `show_attack_effect`，尊重画面效果开关）。
- `Gameplay/Combat/Skill.cpp:170-178`：构造末尾按 `is_teleport`/`is_flash_jump` 覆盖 useeffect 为 `BasicEffUseEffect("Teleport")`/`("Flying")`。
- `Character/Player.h:198` + `Player.cpp`：新增 `teleport_throttle`（120ms，随 update 递减）；`apply_teleport` 开头节流门禁，水平位移用 foothold 树（1/4 递进回溯）校验落点，越界/无平台则保持原位。
- 未改 `is_effect_skill` 白名单（避免重复发包）、`Data/SkillData.h`。

---

## 问题4 — 快捷栏 hover 无 tooltip

### 根因
- `UIStatusBar.cpp` 全文 tooltip 调用命中 0；`send_cursor` hover 分支只返回 CANCLICK。
- 原生 083 `CUIStatusBar` 有 `SetShortcutTooltip/ProcessToolTip`（`P3-A-BATCH-REST-MAPS.md:147`）→ 原生会弹，本端从未实现。
- 注意：图2是游戏内快捷栏（`UIStatusBar`），非 KeyConfig 配置子窗（`UIQuickslotConfig`）。

### 改动
- `IO/UITypes/UIStatusBar.h:134` 声明 `show_qs_tooltip(int16_t slot)`。
- `UIStatusBar.cpp:2776` 实现：按当前模式（classic/modern）解析 slot→键位映射，SKILL→`show_skill`、ITEM→`show_item`（复用 `Tooltip::Parent::KEYCONFIG`，与 UIKeyConfig 同款）；skill 等级/熟练度/过期时间取自 `Stage::get().get_player().get_skills()`。
- `UIStatusBar.cpp:3116`：`send_cursor` 的非拖拽 hover 分支（slot>=0）挂 tooltip；每帧由 UIStateGame 先 clear 再重挂，移开自然消失。

---

## 问题5 — 083 classic 底部按键右移靠 QS

### 根因/机制
- 用户确认当前为 083 classic。按钮簇（下排 BtShop/NPT/Menu/Short + 上排 EquipKey/...）定位锚 `group_x = quickslot_pos.x() - HudLayout::qs_group_gap(VWIDTH)`。
- init_classic（`UIStatusBar.cpp:2295`）、update_screen（`:286`）、iconMemo（`:3450`）**三处共用** `qs_group_gap`，改一处即同步（避免上下排错位）。

### 改动
- `Util/HudLayout.h:45`：`QS_GROUP_GAP_WIDE` 241 → 228（右移 13px，末钮距 QS 左缘从约 19px 收到约 5-6px）。1280 宽屏走 wide 分支（is_qs_wide(1280)=true）。

### Layout Contract（1x）
- overlays：下排4钮/上排小键作为一个 group，整体相对 QS 左缘右移 13px；Fold/Extend、血条/EXP/QS 锚点不动。
- gap_to_quickslot 目标 4-8px；兄弟对齐误差 ≤1px。

---

## 验证状态
- **客户端**：MSBuild Release/x64 全量编译通过（EXITCODE=0，仅仓库既有告警），产物 `build-all-5fix-20261123/MapleStory.exe`。
- **服务端**：仅改源码，未编译（按用户选择）；需用户自行打包重启。
- **真机发包/观感验收：未做**。以下须真机验证：
  1. 瞬移/二段跳特效是否显示、120ms 间隔手感、落点是否不再卡图/穿墙。
  2. 快捷栏技能/消耗物品 hover 是否弹 tooltip。
  3. 空中 4 道光柱是否已隐藏、其他地图 portal 是否正常。
  4. 083 底栏上下排是否同步右移、是否压到 QS 板。
  5. 换频道后是否落到固定出生点（需服务端重新编译部署后）。
- 按用户长期要求：真机截图须先确认来源端为本端，不得用参考端截图验收。

---

## 补充取证（2026-11-24）：编译失败修复 + 原生换频道说明

### 编译失败（上一轮的过错，已修复）
- 报错：服务端 `Character.java:977 找不到符号 getPortals()`（`mvn -pl gms-server compile` 实测，非猜截图乱码）。
- 根因：`MapleMap.java:2716-2720` 的 `getPortals()` 被 `/* ... */` 注释块包住，是死代码、编译期不存在。上一轮只 grep 到方法名（:2717）未检查注释块，且按用户选择"只改源码不编译"未做验证。
- 修复：`MapleMap.java:2716-2720` 取消注释（`Collections`/`Collection` 已在 :83-84 import，方法体完整），使 `Character.java:977` 调用合法。
- 验证：`mvn -pl gms-server compile -DskipTests` → 无 ERROR，exit=0。

### 换频道 ≠ 死亡重生（用户判断成立）
- 死亡重生：`ChangeMapHandler.java:99-115` → `Character.java:7255-7274 respawn() → changeMap(getReturnMapId())`，强制离开当前图回 returnMap。
- 换频道：`ChangeChannelHandler.java:37-51` → `Client.java:1500-1569 changeChannel()`，默认（:1552 配置 false）不改 map，留当前图。两者代码路径完全独立。
- "原始客户端有说明"的三处出处：
  1. `Client.java:1547-1551` 中文注释原文："玩家反馈切换频道不传送ForcedReturnId对应的地图反而比较友好，所以该参数默认为false，想贴近官方可以设置为true"。
  2. `V1.7.0__create_game_config.sql:207`："切换频道是否回到强制返回地图(Change channel will return the force_return_map, just like login again.)"，默认 false。
  3. `docs/features/channel-change-fix-2026-11/notes/three-way-comparison.md:35-36`：Angel IDA `CUIChannelShift::SetRet@0x9959bb → CField::SendTransferChannelRequest@0x5304af`、`OnSetField@0x776020` v102==0 分支复用快照。

### 落点机制（服务端下发 spawnpoint，客户端按 id 落地）
- `Character.java:964-992 newClient()` 设 `initialSpawnPoint` → `PacketCreator.java:215-216` 随 SET_FIELD 下发 mapid + spawnpoint → 客户端 `LoginParser.cpp:222-223` 读 → `SetFieldHandlers.cpp:424-431/:162` → `Stage.cpp:211-217` 按 portal id 落地。
- 因此落点唯一输入 = 服务端下发的 spawnpoint id。上一轮把 `newClient()` 改为"取 id 最小 spawn（固定点）"正是"固定出生点"实现；默认 `findClosestPlayerSpawnpoint(旧坐标)` 在多 spawn 点图落点随旧坐标变，单 spawn 点图与固定点等价。
- BeiDou 三个服务端版本（s9 / 基线 / S10）换频道落点逻辑逐字相同，无版本分歧。

### 当前状态
- 服务端源码已改、编译通过（target/classes 更新）；未 `mvn package` 成 jar、未重启服务端、未真机验收。
- 若 `mvn package` 部署需用户执行或授权；真机验收点：换频道后是否落到该图 id=0 出生点、多 spawn 点图是否符合预期。

---

## 补录（2026-11-25）：MinMS 源码校验 + 三端对照（频道更换 / 死亡重生）

用户要求"频道更换、死亡逻辑对照三端，minms源码校验"。以下直接读 `E:/project/MinMS/src` 源码核实，非文档转述。

### MinMS 频道更换链路（源码实证）
| 环节 | MinMS 文件:行 | 逻辑 |
|---|---|---|
| 发包 | `Net/Packets/GameplayPackets.h:66-74` | `ChangeChannelPacket`：`write_byte(channel) + write_time()`（Cosmic 多读一个 tick int）。本端 `GameplayPackets.h:66-74` 为 `write_byte + write_int(0)`，等价。 |
| 收回包 | `Net/Handlers/LoginHandlers.cpp:347-390` | `ChangeChannelHandler::handle`：:349 读 success，!=1 弹 UIOk；:359-370 读 4B IP+2B port；:371 `cid=Stage::get_player().get_oid()`；:386-389 关 CHANNEL/禁 UI/UILoginwait + `queue_channel_join`。 |
| 延迟重连 | `Net/Session.cpp:185-221` | `queue_channel_join` 存 host/port/cid、state=Queued（:190-193）；注释 :187-189 明写"绝不在 read()/process() 内重连，否则 onmessage→read 丢 channel SET_FIELD"；`poll_channel_join` :205 等 `reading` 结束、:210-215 reconnect+回传 cid。 |
| tick 驱动 | `Journey.cpp:120-151` | update() 中 :126 `Session.read()` 之后 :130 `poll_channel_join()`；join==1 → :134 `PlayerLoginPacket(cid).dispatch()`；join==-1 → 回登录。 |
| SET_FIELD 落地 | `Net/Handlers/SetfieldHandlers.cpp:228-263` | :230 skip23 + :232 read cid；:237-245 UICharSelect 或 `Stage::has_char_entry` 快照双分支；:258-263 was_ingame → `change_state(GAME)`。 |
| 落地落点 | `SetfieldHandlers.cpp:118-168` + `Gameplay/Stage.cpp:321-350` | transition → fade → `Stage::load(mapid, portalid, …)` → `respawn`：`portals.get_portal_by_id(portalid)`（:342）取服务端下发 spawn → `physics.get_spawn_point` → `player.respawn`，**严格按服务端 portal id，不按换线前实时坐标**。 |

### MinMS 死亡重生链路（源码实证）
- `Net/Handlers/PlayerHandlers.cpp:215-259` `sync_player_death_state`：HP==0 → `player.die()` + 弹 `UIDeathNotice`（有命运之轮→WHEEL，否则 TOWN）；确认回调 :242-251 发 `ChangeMapPacket(true, player.get_stats().get_mapid(), "", use_wheel)`（第1参数 true=from dying）。
- :254-258 HP>0 且 is_dead → `player.revive()` + 移除死亡窗。
- :374-382 `CHANGE_MAP` 被拒（enableActions+空 mask）→ 清 pending map change + `PlayerUpdatePacket`，与既有文档一致。
- **MinMS 是纯客户端，死亡/换频道的地图落点都由服务端决定**（CHANGE_MAP / SET_FIELD 下发），客户端只按 mapid+portalid 落地。

### 三端对照结论（频道更换 / 死亡重生）
| 维度 | s9 服务端 | Angel（IDA） | MinMS 源码 |
|---|---|---|---|
| 换频道请求 | `ChangeChannelHandler.java:37-51` → `Client.changeChannel:1500-1569` | `CUIChannelShift::SetRet@0x9959bb` → `CField::SendTransferChannelRequest@0x5304af`（COutPacket 0x27+channel+tick） | `GameplayPackets.h:66-74` write_byte+write_time |
| 换频道回包 | `PacketCreator.getChannelChange`（1B 成功码+4B IP+2B port，无 cid） | `OnMigrateCommand@0x496701` Decode1/4/2 → `CWvsContext::IssueConnect@0xa032df` | `LoginHandlers.cpp:347-390` 读 4B IP+2B port + oid 当 cid |
| 重连时机 | 客户端收 0x10 自行重连 | `IssueConnect` 先 Close 旧 socket 再建新连 | `Session.cpp:185-221` 延迟到 tick，`reading` 结束后 reconnect（注释 :187-189 防丢 SET_FIELD） |
| 落地重建 | SET_FIELD `getCharInfo`（:1012-1024）+ 后续 spawn 包 | `OnSetField@0x776020` v102==0 复用 `GetCharacterData` 快照 | `SetfieldHandlers.cpp:228-263` 双分支复用 `Stage` 快照 + spawn 包 |
| 落点决定 | 服务端 `initialSpawnPoint` → SET_FIELD portal 字节 | field 按 mapid 新建、portal 落地 | `Stage.cpp:342` 按服务端 portal id 落地 |
| 死亡重生 | `ChangeMapHandler.java:99-115` `respawn(map.getReturnMapId())` | （IDA 未单独取证死亡分支） | `PlayerHandlers.cpp:215-259` 弹死亡窗 → 发 `ChangeMapPacket(true, mapid, …)`，落点由服务端 CHANGE_MAP 决定 |

### 对照文档纠偏（依据 MinMS 源码）
- `docs/features/channel-change-fix-2026-11/notes/three-way-comparison.md` 的 MinMS 段（:23-27）行号基本准确：`LoginHandlers.cpp:347-389`（实为 347-390）、`Session.cpp:185-221`（准确）、`SetfieldHandlers.cpp:228-350`（set_field 起始 :228 准确）、`Stage.cpp:clear/load/has_char_entry`（准确）。无需实质纠偏，仅 :23 的区间末行 389→390 微差。
- 该文档 :24 记 "queue_channel_join(addr,port,cid)"（不在此重连）——源码确认准确：`LoginHandlers.cpp:344/:389` 仅入队，实际 reconnect 在 `Session.cpp:212`。
- 文档未覆盖"死亡重生"链路；本次补录填补该空白：MinMS 死亡 = `PlayerHandlers.cpp:215-259` 发 `ChangeMapPacket(true, mapid, …)`，与换频道（发 CHANGE_CHANNEL）是两条独立发包路径，**印证用户"换频道 ≠ 死亡重生"判断在三端均成立**。

### 对当前改动的影响（结论）
- MinMS 源码证明"客户端严格按服务端 portal id 落地"（`Stage.cpp:342`），本端客户端 `Stage.cpp:211-217` 同构；因此"换频道固定出生点"的实现正确位置就是服务端 spawnpoint（`Character.newClient()`），我上一轮改 `newClient()` 取最小 id spawn 的方向与 MinMS 服务端模型一致，无需改客户端。
- 三端均无"客户端自己按旧坐标决定换频道落点"的逻辑；落点权威永远在服务端。

---

## 再补录（2026-11-25 下午）：用户要求"原生官方 + 零配置" → 推翻上一轮自造改动

### 铁证：所谓"官方分支 change_channel_force_return"是 BeiDou 2024 自造补丁
- `git log -S returnedMapId -- Client.java` → 唯一提交 `877d2cd8c3 增加参数切换频道是否回到强制返回地图`（2024-08-05），即整个 block 是北斗新增。
- 补丁前原貌：`git show 877d2cd8c3^:.../Client.java` 的 `changeChannel()` 在 `saveCharToDB()` 后直接 `setSessionTransitionState()` + `sendPacket(getChannelChange(...))`，**无 returnedMapId block**。
- 上游旁证 `E:/project/SoloMapling/src/main/java/client/Client.java:1484+`：OdinMS 系原生换频道同样无 forcedReturn block = 留当前图。
- 三版（s9/基线/S10）`changeChannel` 逐字一致；`GameConfig.java:18-19` 自述"北斗动态参数计划"是自造机制。
- 结论：注释"想贴近官方可设为 true"是提交者自我宣称，非事实。原生 = 留当前图 + `findClosestPlayerSpawnpoint(旧坐标)`。

### 据此的回退（服务端，已落地 + 编译 exit=0）
1. `Client.java` 删除整个 `change_channel_force_return` block（15 行），恢复原生 `saveCharToDB → setSessionTransitionState → sendPacket`。
2. `Character.java newClient()` 回退到原生 `Portal portal = map.findClosestPlayerSpawnpoint(getPosition()); if null → getPortal(0); setPosition + initialSpawnPoint`。上一轮我自造的"最小 id spawn 固定出生点"已撤销。
3. `MapleMap.java getPortals()` 恢复原 `/* ... */` 注释（上一轮为自造逻辑取消的注释）。
- 未改 SQL 配置行（该 key 无其它引用，`getServerBoolean` 缺 key 返回 false，运行时等价；保留仅作历史记录）。

### 死亡完整链路三端对照 + P0 硬 bug 修复
**P0（实证，必修）**：本端 `Gameplay/Stage.cpp:692` 原发 `ChangeMapPacket(true, -1, "", false)`；服务端 `ChangeMapHandler.java:98` 用 `if (targetMapId != -1)` 门控整个死亡复活分支 → `-1` 跳过 `:99-117 respawn`，**玩家卡死死亡态**。MinMS `PlayerHandlers.cpp:243-251` 用当前 mapid 正是规避此坑（源码 :243-244 有明确注释）。
- 已修：`Stage.cpp` 死亡态复活改传 `get_mapid()`，并按背包是否持有原地复活术 `5510000`（MinMS `WHEEL_OF_DESTINY_ID=5510000`）决定 `usewheel`；服务端 `ChangeMapHandler:101-108` 原生支持 wheel 分支。客户端编译 exit=0。

**死亡三端对照**（子代理 death-chain 取证，文件:行号见其报告，已并入本文结论）：
- 服务端：TakeDamageHandler.java:323 addMPHP → Character.java:8487-8508 hp<=0→playerDead():7143（掉经验/清buff）；无专门死亡 opcode，HP=0 走 STAT_CHANGED（PacketCreator.java:1185）。死亡回城 `ChangeMapHandler.java:99-117 respawn(map.getReturnMapId())`，returnMap 来自 WZ `info/returnMap`（MapFactory.java:161）——**零配置、原生**（全库无玩家死亡落点配置开关）。
- MinMS：CHANGE_STATS HP=0 → `PlayerHandlers.cpp:215-259 sync_player_death_state` → `player.die()` + UIDeathNotice(WHEEL/TOWN 双模式) → `ChangeMapPacket(true, 当前mapid, "", use_wheel)`；墓碑动画 `Char.cpp:1492-1529 Tomb.img`；死亡计数 DeathCount。
- 本端：`PlayerHandlers.cpp:114-118` HP<=0→`die()`（有）；`Player.cpp:520-530 die()` 只切 DIED + dead stance（无墓碑动画）；**无 UIDeathNotice/DeathCount 接线**（资源 UIWindow.img/Notice/0、1 与 DeathCount.img 已 orange-wz 实证存在，未接线）；复活靠死亡态按 JUMP。
- Angel IDA（原生 v83）：`CUserLocal::OnSetDead`/`OnRevive` + 专门 `CUIRevive` 窗口 + 10 分钟超时自动复活（angel-activity-decompile-full.json:204-206）。

**结论**：死亡触发/落点三端同构且零配置原生；死亡**表现**（墓碑动画、复活弹窗、死亡计数、10 分钟超时）本端最简，非原生同构，属 P1/P2 增强项，未在本轮改（用户本轮焦点是"原生零配置 + 逻辑正确"，P0 已修）。

### 本轮最终状态
- 客户端：death fix 编译 exit=0，产物 `build-deathfix-20261125/MapleStory.exe`。
- 服务端：换频道回退原生，编译 exit=0（target/classes 更新，未 package/未重启）。
- 真机发包/部署/验收：均未做。死亡复活是否闭合、换频道落点是否=原生 nearest spawn，需真机确认（截图须当前端）。

---

## 补录（2026-11-25 晚）：死亡表现层补齐（用户选 A 全做 + HP归零自动弹窗）

用户确认：083/192 都应有墓碑+复活弹窗+死亡计数+倒计时这些基础逻辑，质疑"本端最简"结论；拍板 A 全做、复活触发改 HP 归零自动弹窗。

### DeathCount 语义澄清（用户问"是不是副本用的"——成立）
- 服务端唯一死亡计数类逻辑：`Character.java:7124-7133 playerDead()`：`if (getMap().isCPQMap()) { int losing = getMap().getDeathCP(); ... gainCP(-losing); }` —— 仅 CPQ 副本按死亡扣 CP、广播 `playerDiedMessage`；普通图死亡走常规分支，**无全局死亡次数累加**。
- 全服务端无 deathCount 字段/DB 列/opcode；MinMS 源码 DeathCount.img 零引用；`ext-mods-integration/README.md:83` 把"死亡次数"列为独立外部 mod（img=2 md=1）。
- **结论**：DeathCount 是副本/mod 专用，非 083 基础逻辑，**未接全局 UI**（避免无数据源硬编码）。

### 已实现的死亡表现层（客户端，编译 exit=0）
| 项 | 文件:行 | 实现 |
|---|---|---|
| 新 UI 组件 | `IO/UITypes/UIDeathNotice.h`(83行)/`.cpp`(192行) | 原生 v83 复活弹窗：Notice/0(回城单钮)/Notice/1(回城+原地双钮)、BtOK2/BtCancel2 四态、视口居中、10 分钟超时自动回城（Angel CUIRevive）、倒计时 Text；FOCUSED=true 吞输入 |
| 类型注册 | `IO/UIElement.h`(ANVILDIALOG 后、NUM_TYPES 前) | 新增 `DEATHNOTICE`（EnumMap 尺寸变化→整工程 Rebuild）；`MapleStory.vcxproj:361` 加 UIDeathNotice.cpp |
| HP归零自动弹窗 | `Net/Handlers/PlayerHandlers.cpp:116-140` | HP<=0 且非 DIED → die() + 按背包是否持有原地复活术 5510000 选 WHEEL/TOWN → `emplace<UIDeathNotice>`，回调 `ChangeMapPacket(true, get_mapid(), "", use_wheel)` |
| 墓碑动画 | `Character/Char.h:214-217` + `Char.cpp` set_state/init/静态定义 | `Effect/Tomb.img/fall` → effects.add + TOMBSTONE 音效（MinMS Char.cpp:1492-1529 同源）；用 `get_frame_count()>0` 判有效 |
| 去 JUMP 触发 | `Gameplay/Stage.cpp:684-692` | 死亡态吞所有输入，复活改由弹窗接管（不再盲按 JUMP、不写死 -1） |

### 资源实证（orange-wz 直读 live）
- `Effect/Tomb.img/fall/0..N`（ARGB4444 墓碑帧）、`UI/DeathCount.img/backgrd`(89×56)+`number/0..9`、`UI/UIWindow.img/Notice/0..4`(286×146)+`Notice/btOK` 四态、`UI/Basic.img/BtOK2/BtCancel2` 四态、`Audio.h:67 TOMBSTONE` —— 全部存在。

### 验证状态
- 整工程 Rebuild 编译 **exit=0、0 errors**（C1041/C1090 均因反复后台构建叠加抢 vc142.pdb，非代码错误；清理进程单次同步构建即过）。产物 `build-deathui-20261125/MapleStory.exe`（5.76MB）。
- **真机发包/部署/截图验收未做**。真机检查项：死亡是否自动弹窗、确定/取消/超时、墓碑动画、原地复活术分支、复活后能否正常站立、弹窗 vs 其它 UI 焦点互斥。截图须当前端。
