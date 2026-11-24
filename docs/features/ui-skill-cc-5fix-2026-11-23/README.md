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
