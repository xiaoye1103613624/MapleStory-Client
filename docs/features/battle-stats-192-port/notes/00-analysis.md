# 战斗统计（Battle Analysis）192 原生移植 — 阶段 0/0.5 解析摘要

日期：2026-11-25　作用域：只读解析（orange-wz MCP / IDA MCP / 源码），**未改代码、未改资源**。

## 1. 目标与截图判读

用户截图（227×201 预览）显示：标题「战斗统计数据」、战斗时间 00:11:26、击杀怪物 1813、
累计金币 3.36W、获得经验值 53.37%、按钮「结束统计」。

**尺寸判读**：192 原生 `battleStats/mini/backgrnd` = 211×182，与截图 227×201 预览比例接近（约 1.075×）。
该截图极可能是 **mini 面板/结束态**或按 mini 面板风格制作的简化视图；
原生 main 面板为 981×615，含逐技能明细与 7 个排序项，远大于截图。
→ 本次实现须同时覆盖 **main（完整）与 mini（简版/结束态）**，不能只做截图那 4 个字段。

## 2. 已证实（orange-wz MCP，192 客户端）

端点 `http://127.0.0.1:10012/mcp`，key = `_gms_key_128.b64` + 零 IV `AAAAAA==`。

| 资源包 | 节点 | 说明 |
|---|---|---|
| `Data/UI/UI_002.wz` | `StatusBar2.img/mainBar/Menu/BtBattleStats` | 状态栏按钮 |
| `Data/UI/UI_002.wz` | `StatusBar3.img/mainBar/submenu/menu/button:battleStats` | 子菜单按钮 |
| `Data/UI/UI_005.wz` | **`UIWindow4.img/battleStats`** | 面板真身 |
| `Data/String/String_000.wz` | `Map.img/etc/993026800/mapName` = 战斗分析室 | 相关地图 |

`UIWindow4.img/battleStats` 结构：
- `main/backgrnd` 981×615 ARGB4444，origin(0,0)；`backgrnd2` 958×571 origin(-11,-23)
- 按钮：`button:Close`(1000, ToolTip「战斗分析系统结束。」)、`button:Mini`(1001)、
  `button:Help`(1002)、`button:Start`(1003)、`button:End`(1004)、`button:Left`(1005)、`button:Right`(1006)
- `page/on`、`page/off` 16×16 分页图标
- `combo:Sort`(1100)：队列 / 累计伤害 / 使用次数 / 平均伤害(1次) / 攻击次数 / 最高伤害(每次攻击) / 最低伤害(每次攻击)，
  `sButtonUOL = UI/Basic.img/ComboBox7`
- `mini/backgrnd` 211×182；按钮 Close(1000)/Main(1001)/Help(1002)/Start(1003)/End(1004)

截图中的 4 个字段名（战斗时间/击杀怪物/累计金币/获得经验值）**烘焙在 canvas 背景里**，非独立文本节点
（与 MapleStory 原生 UI 惯例一致）。完整节点树落盘：`_tmp_battlestats_tree.json`（69,740 字符）。

## 3. 已证实（IDA MCP，192 IDB，hexrays ready）

| 函数 | 地址 | 要点 |
|---|---|---|
| `CUINewBattleRecord::OnCreate` | `0x11248a0` | `CLayoutMan::AutoBuild("UI/UIWindow4.img/battleStats/main")`；取 combo 0x44C；`PushStackForTab`；读 `CBattleRecordMan` 单例 |
| `CUINewBattleRecordMini::OnCreate` | `0x1125cd0` | mini 面板 |
| `CBattleRecordMan::RequestOnCalc(bOn,bIsNew,bClear)` | `0x62ee00` | **发包 opcode 956 (0x3BC)**，Encode1×3 + 条件 Encode2(时长秒) |
| `CBattleRecordMan::OnServerOnCalcRequestResult` | `0x628900` | 收 1B `m_bServerOnCalc`；为 0 时关闭 UI 35/276/277 并清 m_bOnCalc |
| `CUIShortCutMenu::OnCreate` | `0x140e720` | 引用 `StatusBar2.img/.../BtBattleStats` |

`OnCreate` 关键状态：`m_tStartTime/m_tEndTime`（来自 `m_DamageInfo`）、`m_nDurationSec = (now - start)/1000`、
`m_bOnCalc`（本地计算中）、`m_bServerOnCalc`（服务器许可）、`m_nStartFieldID/m_nEndFieldID`。
统计维度（函数族 @0x626520–0x628820）：总伤/召唤伤/属性率/攻击次数/暴击数/MISS/被击/HP·MP 药水量/每技能明细/仇恨监控。
Start/End 按钮 id 0x3EB/0x3EC，按 `m_bOnCalc` 切换 enable 与 Update 态。

## 4. 当前端（源码）现状

| 文件行 | 结论 |
|---|---|
| `IO/UITypes/UIStatusBar.cpp:575` | `BT_MENU_BATTLE` 按钮已建，用的正是 `submenu["menu"]["button:battleStats"]` |
| `IO/UITypes/UIStatusBar.cpp:1210-1219` | **回调错接**：点击后 `emplace<UIMonsterCarnival>()`（怪物嘉年华），不是战斗统计 |
| `IO/UITypes/UIStatusBar.cpp:1926` | `BT_MENU_BATTLE` 已在菜单按钮列表注册 |
| `Util/FeatureRegistry.cpp:69-110` | 侧菜单 `add(id,title,open_key,fn)` 表；**无 battlestats 条目** |
| `side_menu.json:8,13-17` | `folder_combat`（战斗）下 5 个 leaf；无战斗统计 leaf |
| `IO/UITypes/UISideIconBar.cpp:401` | leaf 点击 → `FeatureRegistry::open(item.arg ? item.arg : item.id)` |
| `Util/FeatureRegistry.cpp:151-175` | 未注册 id → `UIOk("OpenUI 未接线")` |
| `Character/DamageRank.h:20-86` | 现有单例伤害数据源（服务器经 dpt 包喂入），可做总伤/技能明细的本地替代 |

结论：**按钮外壳在位、回调错接；缺真正的 UIBattleStats 窗口、统计数据源、侧菜单挂载。**

## 5. 协议可行性（关键约束）

- 192 原生需要服务端 opcode **956**：`RequestOnCalc` 上行、`OnServerOnCalcRequestResult` 下行 1B。
- **s9 服务端无 opcode 956**（全库检索 956 命中均为道具/经验表 ID，非协议）；
  现有伤害统计协议只有 `PacketCreator.dptPlayerUpdate/dptSkillUpdate`（`dptReset`），即 DamageRank 插件通道。
- s9 `killMonster(objId, animation)`（`PacketCreator.java:1986-2002`）**不含击杀者与经验量** →
  服务端口径「击杀怪物数/获得经验值」无法从该包直接得到。

**客户端可本地采样的事件**：

| 字段 | 入口 | 说明 |
|---|---|---|
| 获得经验值 | `Net/Handlers/PlayerHandlers.cpp:178-180` `set_exp` | Δexp；含任务/道具经验，与原生「仅战斗获得」口径有差 |
| 累计金币 | `Net/Handlers/PlayerHandlers.cpp:181-183` `set_meso` | Δmeso；含交易/任务，原生排除交易与任务 |
| 击杀怪物 | `Net/Handlers/MapObjectHandlers.cpp:646-654` `KillMobHandler` | 本地可计数；但无法区分是否自己击杀（包无击杀者） |
| 总伤/技能明细 | `Character/DamageRank.h` | 已有服务器 dpt 数据，可直接复用 |

→ 走「纯客户端近似」可交付截图 4 字段 + 逐技能伤害；但**金币/经验口径与原生不完全一致**，
且无 opcode 956 的服务器许可/跨频道重置语义。

## 6. 资源迁移硬门禁（AGENTS.md §3.1）

192 源为零 IV `AAAAAA==`；本端 live 为 GMS IV `TSPHKw==`。禁止裸拷 / 只 create+copy_paste 不转 key。
必须流水线：load(192 零 IV) → save_as(staging) → `batch_convert_key(GMS, verify:true)` →
独立输出 → unload_all → 客户端关闭下原子替换 → `--resource-probe` / 短启验收（不得 0xc0000374）。

注意：live `E:\MXD\BeiDou-Client-xy\Data\UI\UIWindow4.img`（220,436 B，2026-11-22）已含 equip192 移植内容，
旁有 `.pending_equip_192_gms` / `.bak_pre_equip192_20261122`。
**必须在此基础上合并 battleStats 子树，不得用原生 192 包整体覆盖**，否则回退装备槽位修复。

## 7. 用户决策：B（完整协议对齐，含组队）

用户 2026-11-25 选 B，并追问「涉及到组队的数据，必须走服务端吧？」——已取证确认，结论分两层：

### 7.1 截图 4 字段（个人数据）：原生不走服务端传数值
- `RequestOnCalc@0x62ee00` 的 956 包体只有 `Encode1(bOn/bIsNew/bClear)` + 条件 `Encode2(时长秒)`，**无 exp/meso/击杀/伤害数值**。
- `OnServerOnCalcRequestResult@0x628900` 只回 1B `m_bServerOnCalc` 许可。
- 数值在客户端本地累加：`DamageInfo::IncTotalDamage@0x626650`、`IncTotalAttackNum@0x626680` 等 Inc* 族。
- → 956 只管「能否开始/结束 + 切图重置语义」。

### 7.2 组队 / 他人数据：必须服务端（用户判断正确）
| 证据 | 结论 |
|---|---|
| s9 `Character.java:5696-5700` | `dptOnDamage` 内 `for (viewer : map.getAllPlayers()) viewer.dptRecordObservedPlayerDamage(this, dmg)`——他人伤害由服务端广播 |
| s9 `PacketCreator.java:486-510` | `dptPlayerUpdate/dptSkillUpdate` 服务端主动下发每角色累计值 |
| s9 `killMonster(objId,animation)` `:1986-2002` | 不带击杀者与经验量，客户端无法判定「我杀的」 |
| 客户端 `PlayerHandlers.cpp:178-183` | 客户端只看得到自己的 exp/meso Δ；队友的不到达本客户端 |
| 客户端 `UIDamageRank.cpp:7,47-49,110,996` | 现有伤害榜走控制包 `0x3713` + 服务端 DPT 回推 |

→ 任何「队员各自/队伍合计」数据客户端物理上拿不到，必须服务端聚合后下发。

### 7.3 B 的服务端改动范围（待 B1/B2 范围确认）
1. 956 RecvOpcode + handler（解析 3×byte，回 1B 许可），对齐切图重置语义。
2. 击杀/经验归因：`gainExpInternal@Character.java:3122` 与杀怪结算点记「怪物经验」，排除任务/道具经验；killMonster 带击杀者 cid 或服务端直接记数。
3. 组队聚合与广播：沿用 DPT 模式，按队伍/地图聚合，逐队员下发；面板翻页/排序对应原生 `button:Left/Right` + `combo:Sort`。
4. 客户端只渲染 + 本地计时兜底。

### 7.4 范围抉择：用户选 B2
用户 2026-11-25 明确「B2，该走本地走本地，该走服务端就改服务端」。
- 本地：计时/开关/渲染/无服务端时兜底
- 服务端：exp/meso/kill 归因（排除任务/交易）、组队聚合广播、956 许可语义
- 不动现有 UIDamageRank 伤害榜
完整计划见 `../PLAN.md`。
