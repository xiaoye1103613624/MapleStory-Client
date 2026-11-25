# 战斗统计（Battle Analysis）192 原生移植 — 完整开发计划（B2）

状态：待用户确认 Plan　分支：`feature/dev_0.0.1`
决策：B2 —— 该走本地走本地（计时/开关/渲染/本地兜底），该走服务端就改服务端（经验/金币/击杀归因、组队聚合、许可语义）。
需求补充：挂载到左侧功能按键（side_menu + FeatureRegistry）。

## 0. 证据锚点（已取证，逐条行号）

### 客户端
- `IO/UITypes/UIStatusBar.cpp:575` 按钮已建（用 192 同款 `button:battleStats`）；`:1210-1219` 回调错接成 `UIMonsterCarnival`；`:1926` 已注册。
- `Util/FeatureRegistry.cpp:69-110` 注册表；`:115-137` 查找/别名；`:151-175` open 未注册→UIOk。
- `IO/UITypes/UISideIconBar.cpp:401` leaf → `FeatureRegistry::open(arg|id)`。
- `side_menu.json:8` folder_combat；`:13-17` 现有 5 leaf。
- `IO/UITypes/UIDamageRank.cpp:7,47-49,110,927-928,973,996` 控制包 `0x3713` open=1/reset=2/close=3 范式（可复用）。
- `Character/DamageRank.h:20-86` 单例统计源；`Net/Handlers/WaveAShellHandlers.cpp:3331-3355` dpt 包解析。
- `Net/Handlers/PlayerHandlers.cpp:178-180` set_exp；`:181-183` set_meso（本地兜底采样点）。
- `Net/Handlers/MapObjectHandlers.cpp:646-654` KillMobHandler（本地兜底计数点）。
- `Util/UiAssetResolve.cpp:91,131,142` / `UiThemeLayout.cpp:82,106` UIWindow4.img 资源解析范式。

### 服务端（BeiDou-Server_s9）
- `net/opcodes/RecvOpcode.java:25` `CUSTOM_PACKET(0x3713)`；`SendOpcode.java:358` `DPT_TRACKER(0x3714)`。
- `net/PacketProcessor.java:140` 注册 CustomPacketHandler；`CustomPacketHandler.java:30-48` subType 1/2/3。
- `client/Character.java:5603-5620` damageRankOpen/Close；`:5633-5658` dptRecordObservedPlayerDamage；`:5659-5700` dptOnDamage（`:5696-5700` 遍历 map.getAllPlayers 广播）；`:3122` gainExpInternal；`:3066-3078` gainExp 重载。
- `server/life/Monster.java:559` giveExpToCharacter 调用；`:743-790` 经验分配（MVP/组队加成）；`:548-559` distributePlayerExperience。
- `util/PacketCreator.java:480-510` dptReset/dptPlayerUpdate/dptSkillUpdate；`:1986-2002` killMonster(objId,animation)（不带击杀者/经验）。

### 参考端（192，只读）
- orange-wz：`UI_005.wz → UIWindow4.img/battleStats`，main 981×615 + mini 211×182；按钮 1000-1006；combo:Sort 7 项；`UI_002.wz → StatusBar2.img/.../BtBattleStats`、`StatusBar3.img/.../button:battleStats`。
- IDA：`CUINewBattleRecord::OnCreate@0x11248a0`、`Mini::OnCreate@0x1125cd0`、`RequestOnCalc@0x62ee00`(opcode 956，Encode1×3+条件Encode2时长)、`OnServerOnCalcRequestResult@0x628900`(1B 许可)。

## 1. 目标与非目标

**目标**：192 原生战斗统计（main + mini 双面板、Start/End/Mini/Help/翻页、逐技能明细与 7 项排序）在 B2 语义下完整闭环；个人数据由服务端准确归因，组队数据由服务端聚合下发；修复状态栏错接回调；挂载左侧功能键。

**非目标**：不改现有 UIDamageRank 伤害榜（DPT 通道保持原样）；不做 UI 位图重绘（复用 192 原生 canvas，经转 key 迁移）；不动无关玩法。

## 2. 任务分解（有序，含依赖）

### T1 资源迁移（前置，门禁 §3.1）
1. 关客户端；复制 live `E:\MXD\BeiDou-Client-xy\Data\UI\UIWindow4.img`（220,436 B，含 equip192 修复）到 staging。
2. `load_files(192 UI_005.wz, key=零IV AAAAAA==, exclusive:false)` → `copy_paste_nodes` 只取 `UIWindow4.img/battleStats` 子树 → `save_as(staging, 同 192 key)` → `unload_all`。
3. `load_files(staging, key=零IV)` → `batch_convert_key(GMS IV TSPHKw==, verify:true)` → 独立输出 → `unload_all`。
4. 与 live 合并：把 `battleStats` 子树 paste 进 live 副本（**保留 Equip/ShortCutUI 等既有节点**）→ `save_as` 异路径 → 原子替换回 live（客户端未占用，先备份）。
5. 验收：`--resource-probe` UI 根包含 battleStats；短启不 0xc0000374；抽样节点 origin/尺寸与 §0 一致。
产物：`docs/features/battle-stats-192-port/resources/` 清单（路径+key 名+SHA，不整包复制）。

### T2 服务端：协议与归因（BeiDou-Server_s9）
1. `SendOpcode.java` 新增 `BATTLE_STATS(0x3BC=956)` 下行（许可/结果）；`RecvOpcode.java` 新增上行 956（若与现有冲突则用 CUSTOM_PACKET 子类型扩展，先探测 ID）。
2. 新增 `BattleStatsHandler`（`net/server/handlers/`），解析 `bOn/bIsNew/bClear` + 条件 2B 时长；回 1B 许可（对齐 `OnServerOnCalcRequestResult` 语义）；切图/换频道/下线重置（对齐原生）。
3. `client/Character.java` 新增 battleStats 运行时状态（`dptActive` 同款开关模式，`:5603` 参照）：起始/结束时间、startField/endField、个人累计 exp/meso/kill。
4. **经验归因**：在 `Monster.java:743-790 giveExpToCharacter` 与 `distributePlayerExperience:548-559` 记录"仅怪物经验"（`gainExp` 的 party 段与 personal 段都算，任务/道具经验在 `Character.java:3058 gachaExp` 等路径不记）。
5. **击杀归因**：`killMonster(objId, animation)`（`PacketCreator.java:1986-2002`）在服务端击杀结算处给击杀者记数（不依赖包带 cid）。
6. **金币归因**： `%` 拾取路径记录 meso 增量，排除交易/任务（交易在 TradeHandlers、任务奖励走 quest 路径，均不记）。
7. **组队聚合与广播**：沿用 `dptOnDamage:5696-5700` 的 `map.getAllPlayers()` 广播范式，按队伍/地图聚合逐队员 exp/meso/kill/damage，新增 `PacketCreator.battleStatsPlayerUpdate/battleStatsSummary`（新 SendOpcode 或复用 DPT_TRACKER 子类型 3/4）。
8. 生命周期：`CustomPacketHandler`/新 handler 的 open/reset/close 三态，退出/切图清理运行时状态。

### T3 客户端：数据层
1. 新增 `Character/BattleStats.h/.cpp`：进程级单例，字段对齐服务端下行（时长/击杀/金币/经验/总伤/技能明细/队员列表）；`mutex` + 快照返回（照 `DamageRank.h` 范式）；本地计时兜底（服务端未回包时用 `std::chrono` 显示战斗时间）。
2. `Net/Handlers/` 新增 battleStats 收包 handler，注册进 `Net/PacketSwitch.cpp`（DPT_TRACKER 旁，参照 `:328,:727` 写法）。
3. `Net/OutPacket`/控制包：open/reset/close（照 `UIDamageRank.cpp:47-49,996` 的 `0x3713` 派生结构写法）。
4. 采样点仅作兜底：`PlayerHandlers.cpp:178-183`、`MapObjectHandlers.cpp:646-654`（服务端为权威，本地仅在没有服务端数据时启用，避免口径污染）。

### T4 客户端：UI 与入口
1. 新增 `IO/UITypes/UIBattleStats.h/.cpp`：`UIElement` 派生，双模式 main/mini（981×615 / 211×182）；背景走 `resolve_ui_path("UIWindow4.img/battleStats/...")`；按钮 1000-1006（Close/Mini/Help/Start/End/Left/Right）；combo:Sort（7 项，`UI/Basic.img/ComboBox7`）；文本 `Locale::text`，GBK/UTF-8 分层。
2. 逐技能明细行 + 翻页（`button:Left/Right`）+ 队员分页（B2 组队维度）。
3. `IO/UIElement.h`：加 `Type::BATTLE_STATS` 枚举 + `UI::get().emplace` 模板实例化点（全库检索同类 TYPE 注册位置一次性补齐）。
4. 修复 `UIStatusBar.cpp:1210-1219`：`BT_MENU_BATTLE` 改 `UI::get().emplace<UIBattleStats>()`（去 `UIMonsterCarnival`）。
5. 左侧挂载：`side_menu.json` 在 `folder_combat` 下加 leaf（如 `leaf_battlestats`，title 战斗统计，iconKey 选现有图标，arg `battlestats`）；`Util/FeatureRegistry.cpp` 的 `add(...)` 表加 `add("battlestats", u8"战斗统计", "BattleStats", []{ UI::get().emplace<UIBattleStats>(); })`，别名 `battle`。
6. `MapleStory.vcxproj`/`.filters`：新增 .cpp/.h 编译项（对齐现有 UITypes 条目写法）。

### T5 验证门禁
1. 服务端 `mvn -q -DskipTests package`（BeiDou-Server_s9）；相关单测若存在则跑。
2. 客户端完整构建（VS2019/现有 build 脚本），0 error；产物 SHA 实时核对。
3. 单测：`BattleStats` 单例聚合/重置/快照（新增轻量测试或复用现有测试框架）。
4. 资源：orange-wz `verify_canvas_formats` + 抽样节点（origin/size/格式）；`--resource-probe` + 短启无 0xc0000374。
5. 真机：登录→左侧「战斗」→战斗统计→打开面板（main/mini 切换）→Start→打怪→End→看 4 字段 + 技能明细 + 队员分页。**服务端字段改动需重启服务端**；真机未验则如实声明，不写"已验收"。

### T6 产物与文档
1. `docs/features/battle-stats-192-port/README.md`（最终实现逻辑文档）。
2. `notes/`：client-path、资源迁移 key 步骤、协议字段表。
3. `evidence/`：已落盘的 IDA 反编译 + 节点树；补构建日志与 SHA。
4. `patches/`：diff 摘要；`resources/`：清单（含 SHA256）。
5. 清理临时调试日志；保留上线级节点日志与完整注释。
6. 原子提交（用户授权时）：客户端一个 commit，服务端一个 commit。

## 3. 涉及文件清单

**客户端**：`IO/UITypes/UIBattleStats.h/.cpp`(新)、`IO/UIElement.h`、`IO/UITypes/UIStatusBar.cpp`、`Util/FeatureRegistry.cpp`、`side_menu.json`、`Character/BattleStats.h/.cpp`(新)、`Net/PacketSwitch.cpp`、`Net/Handlers/BattleStatsHandler.cpp`(新)、`Net/OutPacket.h`(如需要)、`MapleStory.vcxproj/.filters`、live `Data/UI/UIWindow4.img`。

**服务端**：`net/opcodes/SendOpcode.java`、`RecvOpcode.java`（如需要）、`net/PacketProcessor.java`、`net/server/handlers/BattleStatsHandler.java`(新)、`client/Character.java`、`server/life/Monster.java`、`util/PacketCreator.java`。

## 4. 风险与回滚

- **资源**：live UIWindow4.img 含 equip192 修复，禁止整体覆盖；失败回滚到 `.bak_*`，battleStats 子树可单独摘除。
- **协议**：服务端新增 opcode 与现有 DPT(0x3714)/CUSTOM(0x3713) 不撞号；handler 解析失败要有 drain/日志，避免包错位（参考 GACHAEXP 4B 教训）。
- **并发**：经验/击杀归因点在 `gainExpInternal`(synchronized) 与怪物结算处，注意不引入死锁；统计状态按角色隔离。
- **口径**：组队 exp 分配有 MVP/等级差/家族加成（`Monster.java:743-790`），统计记录点放在分配之后，记录"实得经验"而非理论值。
- **回滚**：服务端改动可用 git revert；客户端 UI 新增文件可整体移除，UIStatusBar 回调恢复原状；live img 用备份还原。

## 5. 验收定义（DoD）

- [ ] 资源 battleStats 子树在 live 且 probe 通过、短启不崩
- [ ] 服务端 build 通过、客户端 build 通过、SHA 实时核对
- [ ] 956/CUSTOM 开关三态生效，切图/下线重置正确
- [ ] 4 字段数值来自服务端归因（排除任务/交易经验）
- [ ] 组队维度按队员下发并在面板翻页可见
- [ ] 左侧功能键可打开、状态栏按钮回调已修复
- [ ] 真机回归完成；未验项如实声明
- [ ] 文档/evidence/resources 归档、调试日志清理、原子提交
