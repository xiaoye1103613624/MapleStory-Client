# 192 魔女森林 / 突破石 完整移植 PLAN（2026-11-21）

> 状态：**待用户确认计划**。确认后按本计划一次性实现，不挤牙膏。
> 证据全部来自只读取证，行号级结论见 `notes/` 与 `evidence/`。

## 0. 目标 / 非目标 / 已确认决策

**目标**：把 192 客户端「怀旧岛192整合版」`Data/UI/UI_006.wz → UIWindowCN.img` 中的
`CN_50100`（魔女森林，抽奖窗）与 `CN_Limitbreakthrough`（突破石，提升破功值窗），
连同最小外部依赖资源，**完整移植**到当前 `E:\MXD\BeiDou-Client-xy`，并在 s9 服务端新增协议与真实玩法，接现有经济体系。

**非目标**：不引入 192 的 Jiaozi_minigame / Incubator_CN / CN_Web 等无关节点；不移植 192 的加壳二进制逻辑（静态不可取证）；不做多语言英文版（本功能为中文活动，文案随 WZ 烧图/中文）。

**用户已拍板（2026-11-21）**：
- D1 行为层 = **服务端新增协议 + 真实玩法**（接现有经济）。
- D2 规则 = **授权我按合理默认设计**，全部写入本文档与 README，可后续调。
- D3 资源范围 = **最小依赖链**（不整包搬 98MB UI_006.wz）。

## 1. 解析证据摘要（为什么这么做）

| 维度 | 结论 | 证据 |
|---|---|---|
| 资源存在 | `CN_50100`（803×713 底板/魔女森林抽奖）与 `CN_Limitbreakthrough`（196×285/突破石）真实存在，所有 canvas 可读 | `evidence/uicn_full_tree.json`、`evidence/imgs/*.png`、`evidence/canvas_manifest.txt` |
| 依赖可解析 | 突破石 32 条 `_outlink`→`UIWindow2.img/Cube_InGame1`、`Cube_Red`；魔女森林 11 条外链→`StatusBar3.img/chat/common/scroll` + 30 条内链。全部实测 RESOLVED | `notes/wz-deps.md:32-56,199-240,244-249`；本轮实测 `backgrnd4→176×97`、`Cube_Red/Effect/Normal/0→176×86`、`scroll prev0→11×9` |
| 原生逻辑 | 192 客户端加壳（熵 7.90、仅 3 函数/10 字符串、XOR+ADD 解密存根），关键词全 0 命中 → **静态拿不到 opcode/概率/包结构** | `notes/ida-192.md` |
| 服务端 | s9 对 7 关键词 0 命中；仅有无关 `LIMIT_BREAK_SYNC(0x3730)`/`WITCH_TOWER_SCORE_UPDATE(0x12D)`；其它服务端有"装备破界"但无目标窗口 | 服务端只读报告（见会话记录） |
| 客户端 | IO/Net/Data 对目标关键词 0 命中，全新实现。框架为 nlnx：`UiAssetResolve::window()` `Util/UiAssetResolve.cpp:45-58`；四态按钮 `IO/Components/MapleButton.cpp:55-78`；Type 枚举 `IO/UIElement.h:65-259` | 客户端只读报告（见会话记录） |
| 密钥门禁 | 192 零 IV `AAAAAA==` 与本端 GMS `TSPHKw==` 不通用，必须 `batch_convert_key(verify:true)` + probe，禁止裸拷 | `AGENTS.md §3.1` |

## 2. 规则默认设计（D2 授权，全部可配置/可后续调）

> ⚠️ 192 的概率/奖励表不在 WZ 里（二进制加壳 + 服务端无实现），以下是**依据 WZ 道具文案 + 现有经济**设计的默认值，非 192 静态还原值。已明确标注，避免伪装成原生。

### 2.1 魔女森林（CN_50100）
- **入口**：侧栏图标 + NPC 对话触发（二者其一即可先接侧栏；见任务 C6）。
- **玩法**：两种奖池，对应按钮 `button:50` / `button:100`（`notes/wz-deps.md:207-210`）：
  - `50`：消耗 **50 点券(NX_CREDIT)** 抽 1 次；`50X10`：消耗 **500 点券** 抽 10 次。
  - `100`：消耗 **100 点券** 抽 1 次（高级池）；`100X10`：**1000 点券** 抽 10 次。
  - 结果按 5 个奖励槽展示（`layer:reward50Bg0..4` / `reward100Bg0..4`，`:69-78`），10 连抽展示前 5 + 汇总。
- **奖励表**（默认，Java 内可配置数组 `WitchForestRewards`）：普通池权重式随机（消耗品/卷轴/强化材料，itemId 均取 s9 `ItemInformationProvider` 已存在的物品，避免发不存在的道具）；高级池含稀有奖励 + `specialRewardCover0/1` 特殊奖励标识。具体 itemId/权重在 `WitchForestRewards.java` 集中定义并写 README。
- **Timer/Grade/Type/Number**：Timer 用于活动剩余时间展示（默认从服务端下发剩余秒数，0=常驻）；Number/Grade/Type 用于结果评级图标（按抽中稀有度 0..4 映射 `Grade/0..4`、`Type/0..4`）。
- **Notice/popUp**：Notice 显示活动说明（中文文案）；popUp 为确认/取消二次确认（消耗点券前弹）。

### 2.2 突破石（CN_Limitbreakthrough）
- **玩法**：消耗背包中的**突破石道具**，成功则提升角色**破功值(limitBreak)**。
- **道具来源**：192 `String_000.wz` 有「100万突破石 30%」「50万突破石 100%」「500万突破石100%」等（`notes/wz-deps.md:177-179`）。设计：突破石 itemId 用 s9 现有/可配置列表；成功率和增量按道具档位配置（默认 30%/100%，增量 50万/100万等，与文案一致）。
- **成功**：`Character.gainLimitBreak(delta)`（`BeiDou-Server_s9/.../Character.java:2779-2781`）→ 内部 `syncLimitBreak()` `:2798-2800` → `PacketCreator.limitBreakSync` `PacketCreator.java:4319-4321` 下发 0x3730，客户端 `LimitBreakSyncHandler` `Net/Handlers/CommonHandlers.cpp:52-61` 已能处理，**无需改客户端破功显示链路**。
- **动画**：`Effect/Normal` 12 帧（`:234`）为使用过程；`Effect/GradeUp`/`Use` 12 帧（外链 `Cube_Red`，`:235-236`）为成功/失败分支。`BtOk`=确认使用（`:237`）、`BtOnemore`=再抽一次（`:238`）。
- **读取当前破功值**：新窗口不直接读 `Window`（其 `limit_break` 为 private 且无 getter，`IO/Window.h:134`），改用 `damage::server_limit_break()`（`Gameplay/Combat/Damage.h:44-53`），避免反向依赖。

## 3. opcode 分配（全库已验证无冲突）

| 功能 | 方向 | s9 枚举 | 值 | 客户端常量 |
|---|---|---|---|---|
| 魔女森林 | C→S | `RecvOpcode.WITCH_FOREST_ACTION` | `0x3719` | `OutPacket.h:314` 后加 `WITCH_FOREST_ACTION = 14105` |
| 魔女森林 | S→C | `SendOpcode.WITCH_FOREST_SNAPSHOT` | `0x371A` | `PacketSwitch.cpp:411-434` 加枚举、`:818` 后注册 handler |
| 突破石 | C→S | `RecvOpcode.BREAKTHROUGH_STONE_ACTION` | `0x371B` | `OutPacket.h` 加 `BREAKTHROUGH_STONE_ACTION = 14107` |
| 突破石 | S→C | `SendOpcode.BREAKTHROUGH_STONE_SNAPSHOT` | `0x371C` | `PacketSwitch.cpp` 同上 |

沿用现有 C→S 奇 / S→C 偶配对惯例（`0x3715/0x3716`、`0x3717/0x3718`）。

**包结构（自定义，双端严格对齐）**
- `WITCH_FOREST_ACTION`：`byte action`（0=OPEN 请求状态，1=DRAW）+（action=1 时）`byte drawType`（0=50×1,1=50×10,2=100×1,3=100×10）。
- `WITCH_FOREST_SNAPSHOT`：`byte respType`（0=状态，1=抽奖结果）；状态=`long nxCredit` + `int totalDraws` + `int secondsLeft`；结果=`byte drawType` + `byte count` + count×(`int itemId`+`int qty`+`byte grade`) + `long nxAfter`。
- `BREAKTHROUGH_STONE_ACTION`：`byte action`（0=OPEN，1=USE）+（USE 时）`int stoneItemId`。
- `BREAKTHROUGH_STONE_SNAPSHOT`：`byte respType`；状态=`long limitBreak` + `int stoneCount`；结果=`byte success` + `long before` + `long after` + `int stoneItemId` + `int remaining`。

## 4. 任务分解（有序，依赖串行；每步都带验证）

### 阶段 A — 资源导入（先做，客户端要读资源）
- **A1** orange-wz 导出：`UIWindowCN.img` 全节点（含 canvas PNG）；`UIWindow2.img/Cube_InGame1`、`Cube_Red`；`StatusBar3.img/chat/common/scroll` 依赖。源 key = 128B + 零 IV `AAAAAA==`。
- **A2** 在 staging 用 `create_img_file`/`copy_paste_nodes` 组装产物：新建 `UIWindowCN.img`；把 Cube 节点并入 staging 的 `UIWindow2.img`；scroll 依赖核对 live `StatusBar3.img` 是否已存在（大概率已有）。
- **A3** **强制** `batch_convert_key(源零 IV → 目标 GMS `TSPHKw==`, verify:true)` 输出独立目录，核对 SHA；禁止裸拷/禁止 `save_as` 回写 loaded 根。
- **A4** 关闭客户端 → 原子替换进 live `E:\MXD\BeiDou-Client-xy\Data\UI\` → `--resource-probe` + 短启：UI 根出现 `UIWindowCN.img`，进程不 `0xc0000374`。失败即回滚（保留 `.bak`）。

### 阶段 B — 客户端 UI 与协议
- **B1** `IO/UIElement.h`:258-259 之间加 `WITCHFOREST` / `BREAKTHROUGHSTONE` 两个 Type（**整工程 Rebuild**）。
- **B2** `Configuration.h:1015` 后加 `PosWITCHFOREST` / `PosBREAKTHROUGHSTONE`（仿 `:975-984`）；`Configuration.cpp:182` 后加三级键表项。
- **B3** 新增 `IO/UITypes/UIWitchForest.{h,cpp}`（模板：`UIDailyCheckin` 数据驱动 + `UIEvent` 网格/tooltip + `UICashGachapon` 抽奖壳）：资源走 `UiAssetResolve::window("UIWindowCN.img")` 需扩展解析器支持指定 img（或直接用 `nl::nx::UI["UIWindowCN.img"]`）；按钮四态 `MapleButton`；结果 5 槽 + 10 连汇总；Timer/Number/Grade/Type；popUp 二次确认；Notice 滚动。
- **B4** 新增 `IO/UITypes/UIBreakthroughStone.{h,cpp}`：196×285 底板、Effect 12 帧动画、BtOk/BtOnemore、成功/失败分支动画；读 `damage::server_limit_break()`。
- **B5** 开窗口入口：`IO/UITypes/UISideIconBar.cpp:437-438` 之间加两个 `emplace<>()`（并配 `side_icons.ini`）；同时 Net handler 收到 OPEN 回包时 `emplace`（仿 `CashShopHandlers.cpp:645`）。
- **B6** 协议：`Net/OutPacket.h` 加两个 C→S 值 + `Net/Packets/{WitchForestPackets,BreakthroughStonePackets}.h`；`Net/PacketSwitch.cpp` 加两个 S→C 枚举 + `:818` 后 `emplace`；`Net/Handlers/WaveAShellHandlers.{h,cpp}` 加两个 handler 解析并调 UI 的 `apply_snapshot(...)`（仿 `:3374-3401`）。

### 阶段 C — 服务端协议与玩法
- **C1** `RecvOpcode.java:212` 附近加 `0x3719`/`0x371B`；`SendOpcode.java:375` 附近加 `0x371A`/`0x371C`。
- **C2** `PacketProcessor.java:145-146` 之间注册两个 handler（仿 `DailyCheckinHandler` `:142`）。
- **C3** 新建 `WitchForestHandler.java`（OPEN 回状态、DRAW 校验+扣券 `CashShop.gainCash(NX_CREDIT,-cost)` `CashShop.java:289` + `InventoryManipulator.addById` `:73` 发货，仿 `CashOperationHandler.java:180-181`）；`WitchForestRewards.java`（§2.1 默认奖励表，集中可配）。
- **C4** 新建 `BreakthroughStoneHandler.java`（OPEN 回状态；USE 从背包消耗突破石，按配置概率 `chr.gainLimitBreak(delta)` `Character.java:2779` 自动同步 0x3730）；`BreakthroughStoneConfig.java`（道具 id/概率/增量集中可配）。
- **C5** `PacketCreator.java` 加 4 个 snapshot 构造方法（仿 `dailyCheckinSnapshot` `:341-356` 与 `limitBreakSync` `:4319-4321`）。
- **C6** 不新增 DB 表：点券余额与 limitBreak 均为现有持久化字段；总抽奖次数可用内存/角色变量（写 README 说明非持久化边界）。

### 阶段 D — 构建 / 验证 / 部署 / 文档
- **D1** 客户端 `tools/build_smoke.bat` EXIT 0、build_log 0 error；服务端 `mvn -q -DskipTests package` 通过。
- **D2** opcode 门禁：`tools/gen_opcode_matrix.py --check` 通过（双端无撞号）。
- **D3** 按部署铁律：exe **直接覆盖 live 正式** `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（禁止变体名/禁止滞留在 staging）；resource 已在 A4 落 live。部署前关闭客户端，部署后实时核对 live SHA（防登录器回滚）。
- **D4** 短启 + 真机人工点测（**我不宣称真机已验收**；用户截图必须来自 live 本端，按 USER.md 证据规则）。
- **D5** 文档：`docs/features/ui006-cn-port-2026-11/README.md`（最终实现逻辑 + 默认规则表 + 未还原项）、`evidence/` 清单、`notes/` 三份报告归档；删除临时调试日志，保留上线级日志。
- **D6** 单功能原子 commit（仅当用户要求/已授权自动提交时）。

## 5. 验证门禁（DoD）
- [ ] 资源：orange-wz GMS key 能读 live `UIWindowCN.img` 两节点且 canvas 非空；`--resource-probe` 通过、短启不崩溃。
- [ ] 逻辑：双端 opcode 对齐、矩阵检查通过；抽奖扣券/发货、突破石成功提升 limitBreak 并同步 0x3730 有日志可查。
- [ ] 构建：客户端 + 服务端均通过；NUM_TYPES 变更后已整工程 Rebuild。
- [ ] 部署：live exe SHA 与产物一致、实时核对；无变体名旁挂。
- [ ] 文档：规则默认值、非原生还原边界、真机未验收声明齐备。
- [ ] 未触碰无关功能；调试日志已清理。

## 6. 风险与回滚
- **加壳不可取证** → 规则为默认设计而非 192 精确值，已显式标注；若用户后续拿到规则表，`WitchForestRewards`/`BreakthroughStoneConfig` 单点可调。
- **跨端密钥** → 强制 `batch_convert_key` + probe；任何 probe 失败立即回滚 `.bak`，不宣称入库成功。
- **NUM_TYPES 变更** → 必须整工程 Rebuild，否则 UI 状态机错位。
- **经济安全** → 扣券/发货均走现有 API 并二次确认弹窗；默认值保守（低价值奖励权重高）。
- **真机** → 我不以参考端截图当本端验收；未点测前一律声明"未真机验收"。

## 7. 关键路径文件（预估）
- 新：`IO/UITypes/UIWitchForest.*`、`UIBreakthroughStone.*`、`Net/Packets/*Packets.h`、s9 `WitchForestHandler/Rewards`、`BreakthroughStoneHandler/Config`。
- 改：`IO/UIElement.h`、`Configuration.{h,cpp}`、`UISideIconBar.cpp`、`Net/OutPacket.h`、`PacketSwitch.cpp`、`WaveAShellHandlers.*`、`side_icons.ini`；s9 `RecvOpcode/SendOpcode.java`、`PacketProcessor.java`、`PacketCreator.java`、`CashShop.java`(仅复用)、`Character.java`(仅复用)。
