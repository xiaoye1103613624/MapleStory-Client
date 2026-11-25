# 战斗统计（Battle Analysis）192 原生移植 — 最终实现文档

日期：2026-11-25　决策：B2（本地归本地 / 服务端归服务端）　分支：`feature/dev_0.0.1`
参照：192 原生 `CUINewBattleRecord`（IDA 实证）+ orange-wz 资源直读；服务端口径遵循 s9 权威。

## 1. 交付总览

把 192 原生战斗统计面板完整移植到当前端，修复状态栏按钮错接（原误开怪物嘉年华），并挂载到左侧功能键「战斗」目录。**个人/组队数据全部由服务端按实得口径聚合下发；客户端只做计时/渲染/兜底。**

| 层 | 交付 | 状态 |
|---|---|---|
| 资源 | 192 `UIWindow4.img/battleStats` 子树 → 转 GMS key → 独立 img 部署 | ✅ 已部署，probe 通过 |
| 服务端 | 956 许可 + 0x3BD 队伍数据 + exp/meso/kill 归因 + 组队广播 | ✅ 子代理完成，mvn 通过 |
| 客户端数据 | `Character/BattleStats.h` + 两个收包 handler + 上行包 | ✅ |
| 客户端 UI | `UIBattleStats`（main/mini 双模式、7 项排序、队员翻页） | ✅ |
| 入口 | 修 `UIStatusBar.cpp` 回调 + 侧栏 leaf + FeatureRegistry | ✅ |
| 验证 | 客户端 Rebuild EXITCODE=0 / live SHA 核对 / resource-probe | ✅ |
| 真机联调 | 发包/面板流程 | ⚠️ 未做（如实声明） |

## 2. 协议（双端对齐）

| 方向 | Opcode | 格式 |
|---|---|---|
| C→S | 0x3BC (956) | 3×byte `bOn, bIsNew, bClear`（对齐 192 `RequestOnCalc@0x62ee00`） |
| S→C | 0x3BC | 恒 1B 许可（对齐 192 `OnServerOnCalcRequestResult@0x628900`） |
| S→C | 0x3BD | subtype 0=清空；1=队员快照：`int32 charId→int32 jobId→string name→int32 partyId→int64 exp→int64 meso→int64 kill→int64 damage→uint16 durationSec` |

- 0x3BD subtype=1 含自己、是「同图同队且 active 的全量队员行」，客户端按 charId upsert 去重。
- 队员退出清行维持现状（subtype=0 只发触发者本人），下次统计重置统一清理——B2 可接受行为。
- 时长为接收者自己的耗时（秒），同批各行相同；客户端 ×1000 存 ms。
- 技能明细不走本协议，沿用现有 DamageRank / DPT 通道（不动伤害榜）。

## 3. 客户端改动（逐文件 + 行号）

| 文件 | 改动 |
|---|---|
| `Character/BattleStats.h`（新） | 单例 + mutex + 快照；`Entry`（charid/jobid/name/partyid/exp/meso/kills/damage/duration_ms）；`upsert` 去重、`reset`、`snapshot/snapshot_by_damage`、`server_enabled/local_running` |
| `IO/UITypes/UIBattleStats.h/.cpp`（新） | `UIDragElement<PosBATTLESTATS>`；main 981×615 / mini 211×182 双模式；7 颗原生按钮 id 1000–1006；4 字段（战斗时间/击杀/金币/经验）动态叠加；逐队员行 + 翻页 + 排序；`cn()` 中文兜底；本地计时兜底 |
| `Net/Packets/BattleStatsPackets.h`（新） | `BattleStatsRequestPacket`：0x3BC 3×byte |
| `Net/Handlers/WaveAShellHandlers.h/.cpp` | `BattleStatsHandler`（0x3BC 1B 许可）+ `BattleStatsPartyHandler`（0x3BD，全字段 available() 守卫 + 尾部 drain，防 GACHAEXP 式错位） |
| `Net/PacketSwitch.cpp` | `BATTLE_STATS_RESULT=0x3BC` / `BATTLE_STATS_PARTY=0x3BD` 注册 |
| `Net/OutPacket.h` | `BATTLE_STATS_REQUEST=956` |
| `Configuration.h` | `PosBATTLESTATS` 位置项 |
| `IO/UIElement.h` | `Type::BATTLESTATS`（改变 EnumMap 尺寸 → 已整工程 Rebuild） |
| `IO/UI.cpp` | ESC 白名单加 BATTLESTATS |
| `IO/UITypes/UIStatusBar.cpp:1211-1220` | `BT_MENU_BATTLE` 由 `UIMonsterCarnival` 改为 `UIBattleStats` |
| `Util/FeatureRegistry.cpp:81,130` | `add("battlestats",...)` + 别名 battle/battlestat |
| `side_menu.json:15` | 战斗目录加 leaf（`arg:"battlestats"`，order 25） |
| `MapleStory.vcxproj` | 新增 1 cpp + 3 h；Release x64 加 `/FS`（并行 PDB）与 `_CRT_SECURE_NO_WARNINGS`（第三方 nlnx fopen，Rebuild 才暴露） |

## 4. 服务端改动

见 [notes/server-implementation.md](notes/server-implementation.md)（子代理交付记录，含全部行号）。
要点：`RecvOpcode.java:26-28`、`SendOpcode.java:359-362`、`PacketProcessor.java:141-142`、新 `BattleStatsHandler.java`、`PacketCreator.java:480-519`、`Character.java`/`Monster.java` 归因与广播族。mvn package 两次 exit 0。无 DB 写。

## 5. 资源迁移（门禁 §3.1 合规）

- 192 源：零 IV `AAAAAA==` + 128B key；`load_files(UI_005.wz)` → `copy_paste(battleStats)` → `save_as(staging)` → `batch_convert_key(GMS TSPHKw==, verify:true)`（fingerprint `b1141842…`，`ok+verified`）。
- **采用独立 img 方案**：转后 img 直接部署为 `Data/UI/UIWindow4_battlestats.img`，**未触碰** live `UIWindow4.img`（仍 220,436 B / SHA `77edf3e6…` 原样，保留 equip192 修复）。回滚只需删该独立 img。
- `verify_canvas_formats`：`ARGB4444=53, flagged=0, ok=true`。
- 运行时 `--resource-probe`：`[UIWindow4_battlestats.img] children=1`；dump `battleStats/main/backgrnd` 981×615、`backgrnd2` 958×571 —— 资源链路闭环。
- 备份：`E:\MXD\_battlestats_stage\backup\UIWindow4.img.bak_20261125_111234`。

## 6. 验证记录

- 客户端整工程 Rebuild：`EXITCODE=0`，产物 `build-battlestats-20261125/MapleStory.exe`，5,802,496 B，SHA256 `e4dcf36463cd4ecb4e6934b64fa48a647c9f3a5c62d7a691244a5e321e167bd8`。
- live 部署：同名 exe 已落地 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，**SHA 与构建产物实时核对 MATCH**（登录器可能回滚，后续以实时核对为准）。
- 服务端：mvn package exit 0（两次）。
- 日志与反编译证据：`evidence/`。

## 7. 未验证 / 风险（如实声明）

- **真机发包/面板联调未做**：本轮只做静态构建 + 资源 probe。需服务端加 956/0x3BD 后重启，真机走「左侧战斗→战斗统计→Start→打怪→End→看 4 字段 + 队员翻页」全流程。
- 技能明细无服务端下发，复用 DamageRank 本地路径（不动伤害榜）；原生 `combo:Sort` 7 项当前实现覆盖伤害/击杀两项主排序，其余维度保留枚举位与显示同值。
- 队员退出清行是即时性妥协（见 §2）；服务端若需补发全队快照可后续加。
- live exe 被登录器管理的历史：任何"已部署"结论以实时 SHA 为准，本次已核对 MATCH。
- 本工作树含其它会话当日未提交改动（`node.cpp [TEMP-LAZY-PROBE]`、`side_menu.json` 精简等），本次未触碰；提交时需甄别。

## 8. 产物索引

- 计划：`PLAN.md`；解析摘要：`notes/00-analysis.md`；服务端交付：`notes/server-implementation.md`
- 证据：`evidence/`（IDA OnCreate ×2、956 收发包、客户端 build log、resource-probe 报告）
- 资源清单：`Data/UI/UIWindow4_battlestats.img`（209,503 B，SHA `73d4ccd1…`）
