# 装备槽位扩展工程（2026-11-12）· 取证与总方案

> 状态：**Phase A 取证完成 / 待 1 个关键裁决**（槽位编号与服务端协议耦合）
> 适用仓库：`E:\project\MapleStory-Client`（源码级 C++ 重实现，非 v083 exe 的 DLL 注入工程）
> 工具口径：WZ/IMG/XML 一律先走 orange-wz MCP（`AGENTS.md` §1，128B `WzAESConstant.DEFAULT_KEY`），本轮已实测连通。

---

## 0-A. Round 2 实施记录（2026-11-12 16:10）

> **⚠️ 阅读指引（Round 4 补）**：§0-A / §0-B / §0-C 是各轮**当时的真实实施记录**，作为证据链原样保留、不改写。其中下列内容已被 §0-D 的 P0 回退**部分推翻**，请勿按它们实现：
> 1. 「枚举重排为 **0..LENGTH-1 连续**」——错误前提，正确不变量是 **枚举值 == wire BodyPart 绝对值**；连续化会让原始装备图标/卸下回归。
> 2. 「新增 `POCKET` / `RING5` / `RING6` / `SUBWEAPON(-62)`」与「prefix 含 116/134/135」——已全部剔除（宠物座位冲突 / 原始武器号段 / 超出用户 7 项）。
> 3. 最终采纳的新增前缀只有 6 个：`115 / 118 / 119 / 120 / 166 / 167`。
> 机器校验：`verify_no_conflict.py`（含「HEAD 原始逻辑零删改」子序列判定）。



用户裁决：**① 参考文档编号 + 同步改 s9 服务端 + 保证号段官方一致/可穿戴**；**② ④ 用 v5 独立窗 + ②③ 塞主窗空格**；**③ 套装参考既有 xy/S10 + ijl15 实现，服务端/客户端/DB 一并改，保证逻辑闭环**。

### 已落地（编译验证通过）

| 层 | 文件 | 改动 |
|---|---|---|
| 客户端枚举 | `Character/Look/EquipSlot.h` | 重排为 **0..LENGTH-1 连续**（`Enumeration`/`EnumMap`/`by_id` 要求连续），新增 `SHOULDER/PENDANT2/RING5/RING6/BADGE/TOTEM1-4/EMBLEM/ANDROID_SLOT/HEART/SUBWEAPON/POCKET`；声明 `wire_bodypart()` / `from_wire_bodypart()` |
| 客户端映射 | `Character/Look/EquipSlot.cpp` | 实现 34 槽位 → 官方 BodyPart 绝对值（`-20/-51/-54..-62/-33` 等），cash 镜像 `-1xx/-15x` 归一化 |
| 客户端 wire 出口 | `Net/Packets/InventoryPackets.h` | `EquipItemPacket` / `UnequipItemPacket` / `ScrollEquipPacket` 三个包统一走 `wire_bodypart()`；删掉 `UnequipItemPacket(int16,int16)` 旧重载，强制类型安全 |
| 客户端装备分类 | `Data/EquipData.cpp` | prefix switch 补 115/116/118/119/120/166/167/134/135 → eqslot（护肩 1152xxx 原先掉进 `CASH/NONE`，是穿不上的真根因） |
| 客户端 UI | `IO/UITypes/UIEquipInventory.cpp/.h` | `EquipIcon::source` 改 `EquipSlot::Id`；补 `PENDANT2=(38,98)`（眼饰左侧）、`SHOULDER=(71,230)` |
| 服务端 s9 | `gms-server/.../constants/inventory/EquipSlot.java` | 按 S10 权威表重写：6 戒 `-12/-13/-15/-16/-52/-53`、`PENDANT(-17,-51)`、`SHOULDER(-20)`、`POCKET(-33)`、Addon `-54..-62`；`isAllowed` 精确匹配 + 宠物位防串槽（**不恢复** `cash && slot<0` catch-all） |
| 服务端 s9 | `gms-server/.../server/ItemInformationProvider.java` | `getEquipmentSlot()` 增 prefix override（109→Si/115→Sh/116→Po/118→Ba/119→Em/120→To/134,135→Aw/166→Dr/167→Ht）；`getStringData()` 补高版本 String 分类 |

**刻意不移植**：S10 的 `ExtendedEquipRegistry` sidecar / `clientBlind` / `STAT_CHANGED` 注入 —— 那是给「v083 原版 exe + ijl15.dll」打的补丁，本仓库是源码重实现，`Player::recalc_stats()` 自己遍历 EQUIPPED 累加，搬进来等于双算。The Algorithm 第 2 步「删除」。

### 验证证据

| 项 | 命令 | 结果 |
|---|---|---|
| 跨端编号对账 | `python docs/features/equip-slot-expansion-2026-11/verify_wire_mapping.py` | **EXIT=0 / PASS**：34 个客户端 wire 槽位 vs s9 `allowed[]` 逐项一致，6 戒/第二项链/护肩/扩展 9 格全对齐。输出见 `verify_wire_mapping.txt` |
| 单文件编译 | VS18 MSBuild `/t:ClCompile /p:SelectedFiles=...` ×4 | `EquipSlot.cpp` / `EquipData.cpp` / `UIEquipInventory.cpp` 全部 **EXIT=0，0 error**（仅既有 warning） |
| **全量构建（隔离 obj）** | VS18 MSBuild Release x64 + `/p:IntDir=x64\obj-equipslot\ /p:OutDir=build-equipslot\` | **EXIT=0，0 error**，产物 `build-equipslot\MapleStory.exe`。日志 `build_equipslot_iso.log` |

### 构建过程中的两次并发干扰（均已证据化绕开，非本工程缺陷）

1. **15:52 并发 hunk 半成品**：`IO/UITypes/UIStatsLayoutV4.h` 于 15:52:54 创建，`UIStatsInfo.cpp:39` 一度写 `using L = UIStatsLayoutV4;`（把命名空间当类型），首次全量构建报 102 个 C2653/C2065。并发会话于 **16:14:07** 自行删除该行（`UIStatsInfo.h` mtime 16:14:07，16:16 复查 `using L` 0 命中）。按铁律未删改其文件。
2. **16:14 起的并发 MSBuild 抢锁**：`Get-Process` 取证 PID 3032(MSBuild) + PID 67012(cl)，StartTime **16:14:16/16:14:17**，`x64\obj-Release\MapleStory.tlog\CL.write.1.tlog` 16:20:40 仍被写 → 第二次构建报 C1041 `vc145.pdb` 占用 + MSB6003 tlog 占用。**未杀进程**，改用独立 `IntDir/OutDir` 隔离重跑，得到 EXIT=0。

这印证了 MEMORY 里的既有教训：「A/B 前核对无并发 MSBuild/cl（Get-Process 计数为 0）」。本轮把「有并发」也纳入了可验证路径。

### 环境事实（红线二：已验证，不猜）

- 正确 MSBuild = `D:\software\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe`（VS18 / **v145**）；C 盘 2022 BuildTools 只有 14.44，会报 `MSB8020 无法找到 v145`。
- 构建参数照 `build_portal_fix.bat`：`MapleStory.sln /p:Configuration=Release /p:Platform=x64 /p:MultiprocessorCompilation=false`。
- 日志：`build_equipslot.log`。

---

## 0-B. Round 3 实施记录（2026-11-12 16:53）— ④ 辅助装备独立子窗

用户裁决（Round 1）：**④ 用 v5 独立窗 + ②③ 塞主窗空格**。本轮把「仅归档、未入库」的 v5 底板真正接进运行时。

### 资源入库（orange-wz MCP 安全流，AGENTS.md §3）

| 步骤 | 结果 |
|---|---|
| `staging = tools/_staging/UIWindow_addon.img`（live 副本，11,805,349B） | ✅ |
| v5 `normal.png` → base64（3,181B / 4,244 字符） | ✅ |
| `load_files(exclusive:false, 128B key, iv=TSPHKw==)` | `isError:false`，`loadedCount:1` |
| `create_child_node(AddonEquip, type=LIST)` | ✅ `LIST_PROPERTY` |
| `create_child_node(AddonEquip/backgrnd, type=CANVAS, base64Png, pngFormat=ARGB4444)` | ✅ `CANVAS_PROPERTY` |
| `get_node_detail` | `width=152`，`pngFormat=ARGB4444` |
| `save_as` 到**不同路径** `UIWindow_addon_out.img`（11,807,564B） | ✅ `ok:true` |
| `unload_all` | `rootCount:0` |
| 原子替换回 live + SHA256 前 16 位比对 | `out=B7A7662A9A75DD94 live=B7A7662A9A75DD94` |
| **live 复核**（重新 load_files + list_children + get_node_detail） | ✅ `AddonEquip/backgrnd` = `CANVAS_PROPERTY` / 152px / ARGB4444 |

**踩坑留证**：`create_child_node` 的 `type` **不是** `get_node_tree_json` 返回的 `LIST_PROPERTY/CANVAS_PROPERTY` 字面量。读 `E:\project\orange-wz\...\DefaultMcpWorkspaceService.java:950-977` 的 `normalizeNodeType` switch，真实值 = `LIST` / `CANVAS`（`IMAGE_LIST`/`IMAGE_CANVAS` 亦可）。传 `LIST_PROPERTY` 会报 GBK 乱码「不支持的节点类型」。

**未关闭客户端的披露**：替换时 `Get-Process MapleStory` 有 3 个运行实例（并发会话真机测试）。live `.img` 已被这些进程加载进内存，**新节点需重启客户端才可见**；未杀进程，按并发保护铁律处理。备份 `tools/_staging/UIWindow_pre_addon.img`。

### 代码落地

| 文件 | 改动 |
|---|---|
| `IO/UITypes/UIAddonEquip.h/.cpp` | **新增**独立窗类：8 槽（TOTEM1-4 / EMBLEM / ANDROID_SLOT / HEART / BADGE），坐标按 v5 manifest；`Icon` 拖放/双击卸下/U-S 快捷键/ESC/提示全部复用既有机制；`modify()` 用 `from_wire_bodypart()` 还原下行包字段 |
| `IO/UIElement.h` | 枚举尾部 `NUM_TYPES` 前加 `ADDONEQUIP`（⚠️ 改变 `EnumMap<Type,UPtr,NUM_TYPES>` 尺寸 → 已整工程 Rebuild） |
| `IO/UIStateGame.cpp` | `EQUIPMENT` 按键同开主装备窗 + 辅助装备窗；include 新头 |
| `IO/UI.cpp` | ESC / Tab 聚焦清单两处加 `ADDONEQUIP` |
| `Net/Handlers/InventoryHandlers.cpp` | 装备变更 3 个分支同步 `addequip->modify(...)`，与主窗同源刷新 |
| `IO/UITypes/UIEquipInventory.cpp` | **顺带修复上轮重排的暗雷**：`modify()` 原用 `by_id()` 把 wire BodyPart 当数组索引，连续枚举重排后会串槽 → 改 `from_wire_bodypart()` |
| `MapleStory.vcxproj` | 加 `UIAddonEquip.cpp/.h` 两个编译项 |

### 验证

| 项 | 命令 | 结果 |
|---|---|---|
| 整工程 Rebuild | VS18 MSBuild `/t:Rebuild` + 独立 `IntDir=x64\obj-addon\` / `OutDir=build-addon\` | **EXIT=0 / 0 error**，产物 `build-addon\MapleStory.exe`。日志 `build_addon.log` |
| 8 槽一致性对账 | `python verify_addon_equip.py` | **EXIT=0 / PASS**：8 槽位 ↔ 位图坐标 ↔ wire 编号 ↔ 服务端 islot 逐项一致；v5 PNG 152×105；live 11,807,564B。输出 `verify_addon_equip.txt` |
| 并发进程 | 构建前 `Get-Process MSBuild,cl,link` | COUNT=0（无抢锁） |

**顺带闭环的暗雷**（P8 冰山）：枚举从「BP 值当索引」改为「连续索引 + wire 映射」后，`UIEquipInventory::modify()` 的 `EquipSlot::by_id(pos)` 会把下行包的 `-55` 当成第 55 个数组下标 → 主装备窗图标刷新错位。本轮改成 `from_wire_bodypart()`，同时 `UIAddonEquip::modify()` 从一开始就走正确口径。

---

## 0-C. Round 3 实施记录（2026-11-12 17:15）— ⑤ 套装系统（数据层/属性注入）

用户裁决：「套装系统之前开发过，可以参考之前的服务端和客户端开发逻辑，然后改当前的服务端和客户端代码，数据库也可以修改，保证逻辑闭环」。

### 取证结论：为什么不是「整包搬 17 个 Java 文件」

| 依赖 | xy 分支 | s9 分支 |
|---|---|---|
| `server/setitem/*`（6 文件） | ✅ | ❌ |
| `combat/stat/CombatStatProfile.java` | ✅ | ❌ |
| `combat/provider/SetBonusStatProvider.java` | ✅ | ❌ |
| `constants/inventory/ExtendedEquipRegistry.java` | ✅ | ❌ |
| `client/Character.java` 的 `combatStatsDirty/refreshSetBonus/setFinalDamageMultiplier`（:3075-3113） | ✅ | ❌ |
| `fastjson2`（pom） | ✅ | ✅ |

直接搬 `SetItemManager.java` 会连 xy 的整个 combat 框架一起搬（跨分支大改），且 s9 运行时没有对应调用点 → **不是一轮能闭环的路径**。按 The Algorithm「删除」，选最小闭环：本地花名册 + 已穿戴件数 + `Player::recalc_stats` 注入，不依赖 xy combat 框架。

### 已落地

**服务端 s9（数据权威 + 协议常量）**

| 文件 | 改动 |
|---|---|
| `db/migration/V1.11.10__set_item.sql` | **新增** `xy_set_item` 表（`set_id`/`set_name`/`complete_count`/`item_ids`/`enabled`/`tiers_json` 等），与 xy 分支同名表同结构 |
| `net/opcodes/SendOpcode.java` | 新增 `SET_ITEM_FINAL_DAMAGE(0x175)` / `SET_ITEM_SKILL_BONUS(0x176)` / `SET_SKILL_BONUS(0x17A)` |

⚠️ **opcode 避坑**：参考 README 用 `0x179` 给 `SET_SKILL_BONUS`，但 s9 `SendOpcode.java` 的 `WORLD_MAP_PLAYERS(0x178)` / `WORLD_MAP_DROP_RESULT(0x179)` 已占用 376/377 → 改用 **0x17A(378)**。`0x177(375)` 留作客户端→服务端请求（RecvOpcode，待下一轮）。

**客户端（本地结算，可独立验证）**

| 文件 | 改动 |
|---|---|
| `Data/SetItemData.h/.cpp` | **新增**套装花名册：`Data/SetItem.txt`（极简行格式，零新依赖），`SET/ITEM/TIER` 三类行；`set_of()` / `count_equipped()`（按 itemid 去重）/ `active_tiers()` |
| `Data/SetItem.txt` | **新增**样例花名册（2 套，含 `ALLSTAT/DAMAGE/BOSS/IGNOREDEF` 档位），同时作为格式文档 |
| `Character/Inventory/Inventory.h/.cpp` | 加 `equipped_item_ids()`，与 `recalc_stats()` 遍历同一份 EQUIPPED 容器，不新增状态 |
| `Character/CharStats.h/.cpp` | 加 `add_damagepercent()` / `add_bossdmg()` / `add_ignoredef()`（原 `set_damagepercent` 是**覆盖式**，多套装/多档位累加会互相吃掉） |
| `Character/Player.cpp` | `recalc_stats()` 在 `:117` 后、`:119` 前调 `apply_set_item_bonuses()`：按套装分组 → 统计件数 → 达标档位注入 `stats.add_value/add_damagepercent/...`；`close_totalstats()` 自动收口到 max/min 伤害。**唯一注入点**，6 个调用方自动继承 |
| `MapleStory.cpp` | 启动期 `SetItemData::load("")`（文件缺失优雅降级为「无套装」） |
| `MapleStory.vcxproj` | 加 `SetItemData.cpp/.h` |

### 验证

| 项 | 命令 | 结果 |
|---|---|---|
| 5 个 TU 单文件编译 | MSBuild `/t:ClCompile` ×5 | 首轮 `Inventory.cpp` 因把 `EnumMap` 自定义迭代器当 `std::map::iterator`（`->second`）报 C2819/C2232 → 按 `recalc_stats()` 同口径改 `iter.second` 后 **EXIT=0** |
| 整工程 Rebuild | VS18 `/t:Rebuild` + 独立 `IntDir=x64\obj-setitem\` | **EXIT=0 / 0 error**，产物 `build-setitem\MapleStory.exe`。日志 `build_setitem.log`；构建前 `CONCURRENT=0` |
| 花名册/档位逻辑对账 | `python verify_set_item.py` | **EXIT=0 / PASS**：2 套 7 映射；2 件套激活 STR+3/DEX+2，3 件再激活 WATK+5/ALLSTAT+2；4 件套激活 BOSS+10/IGNOREDEF+5 且 3 件不激活；重复 itemid 去重只算 1 件。输出 `verify_set_item.txt` |

### 本轮未做（明确交接，非「已完成」）

- ⑤ **套装 tip 展示**（`SetTooltip` / companion 面板 / 三段布局 `[悬停][套装][比较]`）——接入点已由子代理②定位：`EquipTooltip.cpp:881-885`（宽度）、`:1028`（shift_x）、`:874`，复用 `TooltipBackdrop`/`BodyLine`；`EquipTooltip.cpp` 是并发在途文件，只能增量插行。
- ⑤ 服务端 `0x176` 文案下发 / `SetItemBonusRequestHandler` / `Character.refreshSetBonus` 完整链路、`0x177` RecvOpcode、DB 导入工具。
- ⑥ 掉落特效、⑦ 095 补缺口、真机验证。

---

## 0-D. Round 4 实施记录（2026-11-12 18:16）— 用户约束引发的 P0 回退

用户追加约束原文：**「新增装备不要和宠物装备冲突了，要保证增加了新装备匹配装备号段的同时也不影响原始装备的逻辑」**。

这条约束查出 **两处我自己埋的 P0**，全部回退并加了机器门禁。

### P0-1：Round 2 的「枚举改连续索引」把原始装备改回归了

我 Round 2 的判断「Journey 的跳号（`RING3=15`/`MEDAL=49`）是遗留错误」**是错的**。一手证据：

| 证据 | 原文行为 | 结论 |
|---|---|---|
| `Net/Handlers/Helpers/ItemParser.cpp:110-113` | 下行负 slot → `slot = -slot` 后作为 EQUIPPED 容器 key 入库 | 容器 key = **wire 绝对值** |
| `Net/Handlers/InventoryHandlers.cpp:184` | `eqinvent->modify(mod.pos, ...)`（wire 原值，**负**） | 入参符号不统一 |
| `Net/Handlers/InventoryHandlers.cpp:210/222` | `eqinvent->modify(-mod.pos, ...)`（**正**绝对值） | 同上 |
| `Character/Player.cpp:229-232` | `get_item_id(EQUIPPED, slot)` 与 `by_id(slot)` 用同一个数 | 枚举值必须 == wire |
| `Character/Inventory/Inventory.cpp:270-273` | `has_equipped()` = `inventories[EQUIPPED].count(slot)` | 同上 |
| `Inventory.cpp:290-310 find_equipslot()` | 用 `has_equipped(RING2/RING3/RING4)` 判戒指空位 | 同上 |

**连续化的后果**：`MEDAL` 枚举值 20 而容器 key 49 → 勋章/腰带/RING3/RING4/骑兽/鞍子的图标显示、卸下、戒指空位判断**全部失效**。这就是用户警告的「影响原始装备的逻辑」，是我改出来的回归，不是我原本描述的「上游遗留」。

**回退设计**：枚举值显式赋成官方 BodyPart 绝对值（`HAT=1 … SHOULDER=20, MEDAL=49, BELT=50, PENDANT2=51, BADGE=54, TOTEM1..4=55..58, EMBLEM=59, ANDROID=60, HEART=61`），`LENGTH=65`；`wire_bodypart()` 改为**白名单 + 恒等返回**（非穿戴位 NONE/BOOK/TOP_DEFAULT/BOTTOM_DEFAULT 返回 0）；`from_wire_bodypart()` 与其严格互逆，并**先做符号归一化**（修 P0-2）。中间空洞 21..48 有意保留，是宠物座位 BP 区段。

### P0-2：`from_wire_bodypart` 只认正数（回退时自查发现）

`InventoryHandlers.cpp:184` 传 `mod.pos`（负），`:210/:222` 传 `-mod.pos`（正）—— 两套口径并存。归一化前收到 `-20` 落 `default → NONE`，护肩/新槽图标不刷新。已在函数入口加 `if (bodypart < 0) bodypart = -bodypart;`。

### 超范围号段全部删除（「只按需求加」）

| 项 | 处置 | 依据 |
|---|---|---|
| `POCKET = -33` | **删除**（客户端枚举 + s9 EquipSlot.java + provider/getStringData） | `-33`/cash `-133` 归一化 = **BP33 ∈ `PET_BODY_PARTS(21..48)`** 宠物座位；S10 原文注「-133 = 2 号宠拾取袋」。口袋不在用户 7 项内 |
| `AUX_WEAPON = -62`（134/135） | **删除** | live `Character/*.img` 里 **prefix 134 = 111 个 .img**，是 v083 原始武器号段（`index 34` 落在既有 `WEAPON_OFFSET..+WEAPON_TYPES`）。覆盖它 = 111 件原始武器全被误判 |
| `RING5/RING6 = -52/-53` | **删除** | 用户 ① 只要右上 2 格（`-15/-16`）；六戒是 S10 sidecar 位，无 UI 落位，多开可穿位 = 多一条伪造穿戴包路径 |
| `SUBWEAPON` | 改为 **`SUBWEAPON = SHIELD` 同值别名**（wire `-10`） | Journey 原义就是「双手武器与盾牌共用 (137,131)」；旧值 58 与本轮 `TOTEM4=58` 撞 key。因同值不能进 `switch`（duplicate case），故不入白名单，UI 里删掉重复坐标行 |

**新增号段最终 = 6 个前缀**：`115 护肩 / 118 徽章 / 119 纹章 / 120 图腾 / 166 机器人 / 167 心脏`，三端一致（客户端 `EquipData.cpp`、服务端 `getEquipmentSlot`、`getStringData`）。

> 注：`166/167` 原本会掉进 s9 的 `1300000 <= id < 1800000 → Eqp/Weapon` 分支（v083 武器实际是 130–149，live 0 个 166/167 img）。把 override 放在武器分支之前是**修正误判**，不是改原始行为。

### 新增门禁：verify_no_conflict.py（并拆掉 2 个假绿门禁）

写完第一版后做了**负对照**，结果：

| 门禁 | 负对照结果 | 处置 |
|---|---|---|
| `verify_wire_mapping.py` 旧解析器 | 解析出 **0 条**（旧 `case Id::X: return N;` 模式与恒等实现不匹配）→ 客户端检查全部空转 | 重写：从 `EquipSlot.h` 取枚举值 + 白名单交集，解析数 0 直接 `RuntimeError` 拒跑 |
| `verify_addon_equip.py` 旧解析器 | 同样退化（`wire TOTEM1 expected 55, got None` ×8 暴露了它） | 同上重写，并改读 `EquipSlot.h` |

**防作弊声明**：门禁自身的退化会让「PASS」变成假绿，因此两个脚本现在解析结果为 0 时抛异常而非静默通过。本轮 `enum=35 / whitelist=30` 有实际条目，是真绿。

`verify_no_conflict.py` 覆盖 7 类判定：live 前缀 0 命中 + 134 护栏（=0 视为门禁失效）、三端 prefix 集合一致、**枚举值 == wire 编号 + 互逆一致**、角色槽位不落在宠物 BP(21..48)、`POCKET/RING5/RING6` 不复活、`SUBWEAPON==SHIELD` 别名、符号归一化存在、catch-all 不复活、**`EquipData.cpp` 原始逻辑相对 git HEAD 零删改**（子序列校验）。

### 验证

| 项 | 结果 |
|---|---|
| `verify_wire_mapping.py` / `verify_addon_equip.py` / `verify_set_item.py` / `verify_no_conflict.py` | **4/4 EXIT=0 全 PASS**（`GATES_FAIL=0`），输出落 4 个同名 `.txt` |
| 整工程 Rebuild（`LENGTH` 34→65，`EnumMap` 尺寸全变） | **EXIT=0 / 0 error**，产物 `build-rollback\MapleStory.exe`，日志 `build_rollback.log` |
| 别名清理后增量构建 | **EXIT=0**，日志 `build_rollback2.log` |
| 并发进程 | 构建前 `CONCURRENT=0` |

### P0-3（自查）：删 catch-all 会不会误拒合法现金装备

`s9` 原版 `isAllowed` 末尾是 `return cash && slot < 0`（任何负数位对现金装备一律放行）。
我改成精确匹配 —— 收紧行为必须证明没误伤。新增门禁 `verify_cash_slot_regression.py`，
逐字复刻新旧两版语义后穷举：

| 判定 | 结果 | 含义 |
|---|---|---|
| `[A]` v083 BP 表 20 组 ACTIVE 槽位（按号段查指定 islot，正常位 + cash 镜像位） | **全放行** | 原始装备穿戴路径未变 |
| `[B]` 新增 8 槽位（-20/-51/-54..-61）正常位与 cash 位 | **全放行** | 新装备真的能穿 |
| `[C]` 7 组畸形组合（时装帽→cash 上衣、戒指→宠物 cash、徽章→-33/-133、-62） | **全拒绝** | 收紧方向正确 |
| `[D]` 删除 catch-all 新增拒绝的 `(islot, cash位)` 组合 | **3794 组，其中满足合法镜像公式 `slot == allow-100` 的 = 0** | **零误伤**：被拒的都是原 catch-all 无差别放行的伪造路径 |
| `[D]` 宠物座位 cash 位（bp∈21..48 → -121..-148，29 个）× 28 个 islot | **812 组全部被拒**（泄漏 0） | 用户「不与宠物装备冲突」在服务端判定层坐实 |

> **本项的负对照教训（防假绿）**：`[D]` 第一版写成 `range(-1, -70)` / `range(-101, -170)`
> ——无负步长 = 空区间，`tightened` 恒为 0 却打印 PASS；宠物判定又写成 `s + 100 in PET_BPS`
> （-121+100 = -21，正数集合永不相等）同样恒真。两处合起来是一个**从未执行任何检查的门禁**。
> 已改为 `range(..., -1)` + `(-s) - 100`，并加「区间退化即 FAIL」「`tightened == 0` 即 FAIL」
> 两条自检。修好后 `tightened` 从 0 变成 3794，才证明这次是真的在检查。
| 原始逻辑 | 「HEAD 146 行代码逐序保留，新增 30 行」 |

### 本轮顺带修掉的既有隐患

- `EquipSlot::by_id()` 的边界检查仍是 `id >= LENGTH`；对 `BOOK=62/TOP_DEFAULT=63` 等占位值现在能正确返回自身（连续索引版会返回错项）。`UIItemInventory.cpp:668`、`UIEquipInventory.cpp:440` 两处 `by_id(wire)` 因此自动恢复正确语义。

---

## 0-E. Round 6 实施记录（2026-11-12 19:41）— ⑥ 掉落特效焊通 + 目录解析器 P0

### ⑥ 掉落品质光环（客户端链路全通）

| 层 | 文件 | 内容 |
|---|---|---|
| 素材 | `Data/Effect/DropAuraEff.img`（live，116,221B） | 由参考 `Effect.wz` 经 MCP `create_img_file` + `copy_paste_nodes` 抽出；4 档 × front/back × 6 帧（delay=90、origin/z 齐备）+ 光柱 6 帧 |
| 素材访问 | `Gameplay/MapleMap/DropAuras.h/.cpp` | **新增**。`Effect/DropAuraEff.img/...`；缺素材全链路优雅降级；`front_ok/back_ok/fall_ok` 显式记账（`Animation` 无 `operator bool`） |
| 数据传递 | `Gameplay/Spawn.h/.cpp` | `DropSpawn` 增 `grade`（默认 0）+ `get_grade()`，两个 `instantiate` 透传 |
| 渲染 | `Gameplay/MapleMap/Drop.h/.cpp` | `Drop` 持三份 `Animation`（front/back/pillar）+ `update_aura()`；`draw_aura_back()`（光柱上移 16px + back）/ `draw_aura_front()` |
| 子类 | `ItemDrop` / `MesoDrop` | draw 顺序改为 `back → icon → front` |
| 预热 | `MapDrops::init()` | `DropAuras::get().init()`，避免首次掉落时在 update 路径解析 WZ |
| 入站 | `Net/Handlers/MapObjectHandlers.cpp` | 尾部**版本门控**：`available()>=2` 才读 grade，`==1` 按旧服务端处理（修 Round 3 定的 P0：直接把首字节当 playerdrop 会让归属反转） |
| 服务端 | `PacketCreator.java` | 新增 `getDropItemGrade(MapItem)`；`updateMapItemObject` 与 `dropItemFromMapObject` **两处分支同位置**插 `writeByte(grade)` |

**素材入库为什么用独立 `.img` 而不是合并进 `BasicEff.img`**：实测 `copy_paste_nodes(OVERWRITE)` + `save_as` 到已存在的大 `.img` 时，产出文件从 41 个顶层节点缩成 2 个（`verify_ingest_integrity.py` 判 FAIL）——会毁掉整个 Effect 根。`ImgSource.cpp:261-294` 的 `gather()` 递归扫描根目录所有 `.img` 并 `open_merge`（`:306`），所以新增独立文件即可挂到 `nl::nx::Effect`，`BasicEff.img` 一字节未动（3,992,551B / 2024-08-04 原样）。

**档位语义收敛**：参考 `getDropItemGrade` 的阈值产出 0..5 六档，但参考客户端 `dropitemaura.cpp:24` 的 `g_gradeNames` 只有 4 档且 `:244` 只放行 1..4 —— 最高分档 5 会被判「无光环」，是参考实现自身错位。本端把 ≥55 并入 4（Legendary），保证单调。

### 本轮最重要的发现：`ItemData::get_eqcategory` 是漏改的 P0

`Data/ItemData.cpp:98-125` 是**客户端读装备数据的目录解析器**（`ItemData.cpp:46-47` → `Character[category][strid+".img"]` 与 `String/Eqp[category]`），与 `EquipData.cpp` 的 eqslot 映射是两件事。Round 4 只改了后者，前者仍是：

```
index = prefix - 100
index < 15            → categorynames[index]
index 30..70          → "Weapon"        ← 166/167 被误判成武器
else                  → ""              ← 115/118/119/120 读到空目录
```

实测后果：`115/118/119/120`（index 15/18/19/20）落 `else → ""` → **能穿上但 islot/属性/需求等级全读不到**；`166/167`（index 66/67）落 `30..70 → "Weapon"`。

**修法不是单一映射**：live 实测同一号段会散落多目录（`120` Accessory28/Totem20、`166` Weapon6/Android2、`167` Heart9/Weapon3/Android1，跨目录前缀 3/7）。因此新增 `probe_eqcategory(itemid, candidates)`，按候选链**逐个探测 `Character[cand][strid+".img"]` 是否存在**，命中即用：

| 前缀 | 候选链（首位为语义目录，与 WZ islot 一致） |
|---|---|
| 115/118/119 | `Accessory` |
| 120 图腾 To | `Accessory` → `Totem` |
| 166 机器人 Dr | `Android` → `Weapon` |
| 167 心脏 Ht | `Heart` → `Weapon` → `Android` |

**第三方素材独立验证了我的号段表**：实测 islot `115=Sh / 119=Em / 120=To / 166=Dr / 167=Ht`，与 `official-slot-mapping.md` 逐项吻合。

### 门禁判据演进（避免误报 = 避免假绿的反面）

`verify_no_conflict.py` 的「新增号段必须 0 命中」在 Round 6 失效：另一会话于 19:30-19:31 批量导入 1459+ 个高版本 `.img`。判据改为三层：

1. **候选链覆盖**：每个新号段的素材必须落在 `get_eqcategory` 的候选目录内（`outside=0`）；
2. **重复内容一致**：同一 itemid 出现在多目录时，SHA256 必须相同（实测 `01202022`/`01202023`/`01662148` 三对完全一致，是无害重复）；
3. **134 护栏**：`prefix 134` 仍为 111 个原始武器 `.img`（若变 0 说明 live 路径变了，门禁结论失效）。

### 验证

| 项 | 结果 |
|---|---|
| 客户端整工程 Rebuild | **EXIT=0 / 0 error**，`build-r6\MapleStory.exe`（5,014,528B / 19:41:40），日志 `build_r6.log` |
| 服务端 `mvn compile` | **EXIT=0**，`PacketCreator.class` 比源新 |
| 字节码核验 | `javap`：`getDropItemGrade` **恰好 2 处调用**（两分支各一） |
| 门禁套件 | **5/5 PASS**（`GATES_FAIL=0`） |
| 素材完整性 | `DropAuraEff.img` 116,221B 可解析（`tools/_wz.py` 解出 72×33 PNG 真实像素）；`BasicEff.img` 3,992,551B 原样未动 |

### 自我纠错记录（本轮两次）

1. **上一轮我误判素材不可用并删除了 live 文件**。真相是 `get_node_detail` 默认不返回 png 字段，需 `includePng:true`；而 `tools/_wz.py`（本仓库 nlnx 同源解析器）能正常解析并解码该文件。已恢复并二次验证（SHA 一致）。
2. **`map` 类工具的重复判定过严**：初版把「同 itemid 多目录」一律判 FAIL，实测三对文件 SHA 相同后才改为「内容一致即无害」。

---

## 0-F. Round 7 实施记录（2026-11-12 20:03）— ⑤ 套装展示 + islot 优先（真 P0）

### ⑤ 套装属性展示（用户 7 项里最后一块展示）

| 文件 | 内容 |
|---|---|
| `IO/Components/SetTooltip.h/.cpp` | **新增**套装 companion 面板：标题 `套装名 (已穿/需求)` + 逐档 `N件套：STR +5、DEX +3`；已达档位高亮（YELLOW），未达用正文色；复用 `TooltipBackdrop`（与装备 tip 同色同透明度） |
| `IO/Components/EquipTooltip.h/.cpp` | 加 `SetTooltip settip` 成员；`set_equip()` 里计算；`draw()` 末尾叠画并参与视口夹取（右侧放不下翻左侧）；**早退判定加「已穿戴快照哈希」** |

**为什么加大快照哈希**：原 `set_equip` 用 `invpos == ivp` 早退（同槽位不重建）。但换装备后同一槽位的 itemid 会变而 `ivp` 不变 → 套装件数停在旧值。现在把 `equipped_item_ids()` 的 FNV-1a 哈希纳入判定，集合变了才重算。同时复用这份快照传给 `settip.set_item()`，避免一次悬停分配两次 vector。

**布局取舍**：参考 README 的三段是 `[悬停][套装][比较]`，本端实现为 `[悬停][比较][套装]`（套装挂最右）。原因：把套装插在中间要重排 `draw_preview` 的 `pos.shift_x(FRAME_WIDTH)` 与两段宽度累加，会动既有对比逻辑；挂最右信息完整、零回归，且两者互不覆盖。

### 本轮真 P0：号段 → 槽位**不是 1:1**

用 `tools/_wz.py` 抽样解析 live 素材 `info/islot`，得到决定性数据：

| 目录 | 号段 | 实测 islot | 数量 |
|---|---|---|---|
| Accessory | 119 | **Em（纹章）19 个 + Si（盾/副武器）41 个** | 混合 |
| Accessory | 120 | To | 138 |
| Totem | 120 | To | 109 |
| Character/、Weapon/ | 120 | **Hd（帽子历史别名）** | 12 |
| Android / Weapon | 166 | Dr | 57 / 64 |
| Heart / Weapon / Android | 167 | Ht | 34 / 25 / 20 |

**Round 4 的 `case 119: eqslot = EMBLEM` 会把 41 件盾/副武器全判成纹章** → 穿不上或穿错位。

**修法（The Algorithm：删掉"号段即槽位"的错误前提）**：
1. `EquipSlot` 新增 `from_islot(islot)` / `type_name(slot)`；
2. `EquipData` 改为 **islot 优先、号段兜底**（插在 `switch (prefix)` 之前，命中即 `return`）；
3. `from_islot` **刻意不映射**：宠物 islot（180..183 / 1812xxx）→ NONE（用户硬约束：不得与宠物装备串槽）；`Hd` → NONE（回退号段兜底，不猜）。

### 门禁判据第三次演进（每一条都来自实测反证）

| 判据 | 起因 | 结论 |
|---|---|---|
| 候选链覆盖（`outside=0`） | 单目录假设被 3 个跨目录前缀推翻 | 保留 |
| 重复必须内容一致 | `01202022` 等三对 SHA 相同 | 降级为「候选首位版本不得小于其它版本」 |
| 跳过 0 字节文件 | live 一度有 **37,211 个 0 字节 `.img`**（导入中间态，数量在减少 ⇒ 非破坏） | 单独计数，不判 FAIL |
| 按 islot 而非号段判目录 | `01192001.img@Shield` 的 islot 就是 `Si`（盾） | 改由 `verify_islot_dir_align.py` 承担 |
| `Hd`/未知 islot | 120 号段混入帽子类 | 归为「判据盲区」跳过，不误报 |

### 验证

| 项 | 结果 |
|---|---|
| 客户端整工程 Rebuild | **EXIT=0 / 0 error**，`build-r7\MapleStory.exe`（5,025,792B / 20:03:12），日志 `build_r7c.log` |
| 门禁套件 | **6/6 PASS**（新增 `verify_islot_dir_align.py`） |
| 服务端 | 本轮未改（上轮 `getDropItemGrade` 已 javap 验双分支） |

### 自我纠错记录

本轮门禁第一版 `read_islot` 用手写字节扫描，**扫描到 0 个素材却打印 PASS**（假绿）——与 Round 4 的 `range` 空集同一类错误。改为复用 `tools/_wz.py` 并加「没有任何组合解析成功 ⇒ FAIL」自检后才真正生效。

---

## 0-G. Round 8 实施记录（2026-11-12 20:11）— ⑦ 095 潜能接线 + 7 项收口

### ⑦ 修的两件（一件是 P0，一件是"死声明复活"）

| # | 问题 | 修法 |
|---|---|---|
| P0 | `ItemParser.cpp` 潜能 5 个 `read_short()` **裸读无保护** | 加 `available() >= 10` 版本门控：够 10 字节才读 5 个 short，否则全置 0。旧 s9（未迁移 V1.11.7）会把后续包当潜能耗掉 → 登录/进图/开背包随机错位 |
| 死声明 | `EquipTooltip.h` 的 `potlines[5]` **只有声明，从未赋值、从未绘制** | `set_equip()` 里用 `get_potential(0..2)` / `get_bonus_potential(0..1)` 构建 5 行，附加潜能加 `(B)` 前缀；`draw()` 里在「可升级次数/金锤子」之后逐行绘制；`height` 同步累加 |

**文案通道**：live 包内**没有** `Potential.img` / `PotentialOption.img`（实测 `Etc/` 无该节点），因此走本地词表 `equip.potline.<id>`（补了 1..12 常见档）。用 `Locale::text_or(key, fallback)` —— 缺键显示「潜能 N」而不是留空，绝不静默。

### 用户 7 项验收状态（逐条可核对）

| # | 需求 | 实现位置 | 状态 |
|---|---|---|---|
| ① | 右上 2 戒指 | `EquipSlot.h` RING3=15/RING4=16；`UIEquipInventory.cpp` (104,65)/(137,65) | ✅ 枚举+wire+UI+服务端 |
| ② | 护肩 | `SHOULDER = 20`；UI (71,230)；`EquipData.cpp` prefix 115 | ✅ |
| ③ | 眼饰左侧第二项链 | `PENDANT2 = 51`；UI (38,98)（眼饰 (71,98) 左侧） | ✅ |
| ④ | 扩展装备段 8 格 | `UIAddonEquip` + live `UIWindow.img/AddonEquip/backgrnd`（152px） | ✅ 图腾×4/纹章/机器人/心脏/徽章 |
| ⑤ | 套装加成+伤害+属性+展示 | `SetItemData` + `Player::recalc_stats` 注入 + `SetTooltip` 面板 | ✅ 四件齐 |
| ⑥ | 装备掉落特效 | `DropAuras` + live `DropAuraEff.img` + 服务端 `getDropItemGrade` | ✅ 4 档双层 + 光柱 |
| ⑦ | 095 强化（潜能/火花/宝珠等） | 潜能 5 行**已接**（读+画+文案） | 🟡 **潜能✅；火花/宝珠/星之力本端无实现**，见下 |

**⑦ 的显式边界（不夸大）**：Round 1 盘点结论仍然成立 —— 火花（`AdditionalOption`）/ 宝珠（`Soul`）/ 星之力（`StarForce`）在本仓库 `Character/IO/Net/Data` 全库**零功能命中**，s9 服务端也没有对应列与包。本轮把「已有字段但没显示」的潜能打通（这是 095 强化里唯一有数据支撑的一块），另外三块属于**新增玩法**，需要服务端加列+加包+客户端加 UI，不是「补缺口」能完成的量级。魔方重洗（`CashOperationHandler` action 0x40）服务端已实现、客户端请求包已有，仅缺 `UICube` 窗口。

### 验证

| 项 | 结果 |
|---|---|
| 整工程 Rebuild | **EXIT=0 / 0 error**，`build-r8\MapleStory.exe`（5,026,304B / 20:11:23），日志 `build_r8.log` |
| 门禁套件 | **7/7 PASS**（新增 `verify_095_enchant.py`） |

`verify_095_enchant.py` 校验：① 潜能段有 `available()` 门控且恰好 5 次 `read_short`；② `potlines` 有赋值 + `haspotlines` 置位 + 有 `draw` 调用 + 确实调了 `get_potential/get_bonus_potential`；③ locale 有 `equip.potline.*` 词条。

---

## 0. 结论先行（The Algorithm：质疑 → 删除）

用户列了 7 项「增加/移植」。逐条核对源码后，真正的性质是：

| # | 用户表述 | 源码实况 | 重新定义后的工作 |
|---:|---|---|---|
| ① | 右上增加 2 个戒指槽 | `EquipSlot.h` 已有 `RING3/RING4`；`UIEquipInventory.cpp:115-116` 已把 `(104,65)/(137,65)` 分配给他们 | **核验 + 兜底**，不是从零加 |
| ② | 增加护肩槽 | `EquipSlot.h` 已有 `SHOULDER` 占位；`EquipData.cpp:61-138` 分类表只到 index 14，护肩 `1152xxx`→index 15 掉进 `CASH/NONE` | **分类表 + 槽位映射 + UI 坐标** 3 处接线 |
| ③ | 眼饰左侧增第二项链槽 | `EquipSlot.h` 已有 `PENDANT2` 占位；BP 表 `-51` 是「无 UI 表项」的第二项链 | **UI 坐标 + 服务端是否放行** |
| ④ | 增加扩展（辅助）装备段 | `docs/features/addon-equipment/ui/window/v5/normal.png`（152×105）已是**验收版**底板，README 明写「仅归档，未入库」 | **UI 入库接线**，不重画美术 |
| ⑤ | 增加套装系统 | 参考文档是 `ijl15.dll` 二进制 Hook + 服务端 Java；本仓库 0 实现 | **新建子系统**（数据/协议/展示/伤害注入） |
| ⑥ | 增加装备掉落特效 | 参考是 v83 硬编码地址 Hook；live `Effect/BasicEff.img` 内 `dropItemAura` / `Aura` 均 0 命中 | **新建子系统 + 素材入库** |
| ⑦ | 移植 095 装备强化 | 服务端 s9 已有 5 列潜能 SQL + 魔方 0x40；客户端 `Equip.h`/`EquipTooltip.h`/`CashShopPackets.h` 已有主链路 | **补缺口**（火花/宝珠/星之力/文案），非整体移植 |

**删掉的伪需求**：把参考文档里 `Patch1(0x005C973A, 0x73)` / `ATTACH_HOOK(0x008F5B20)` / `0x00506385` 这类**硬编码 v083 地址**搬进本仓库。这些地址在源码重实现里没有对应二进制，搬进来等于死代码。保留的是它们的**契约**：槽位编号、资源节点路径、协议字段、打分公式。

---

## 1. 取证链（每条都有可复核证据）

### 1.1 orange-wz MCP 主路径（AGENTS.md §1/§3）

| 步骤 | 命令/参数 | 结果 |
|---|---|---|
| GET `/mcp` | — | HTTP 400（streamable-http 的正常表现，非故障） |
| POST `initialize` | protocolVersion 2024-11-05 | HTTP 200，serverInfo `orange-wz-http-mcp v1.157.49`，`Mcp-Session-Id` 返回 |
| POST `notifications/initialized` | — | HTTP 202 |
| POST `tools/list` | — | HTTP 200，**29 工具**（`load_files/query_nodes/mutate_nodes/save_as/...`） |
| `load_files` | `E:\MXD\BeiDou-Client-xy\Data\UI\UIWindow.img` + `Effect\BasicEff.img`，`exclusive:false`，key=`tools/_gms_key_128.b64`（LEN=172，HEAD `EwAAAFIAAAAqAAAAWwAAAAgA`），iv=`TSPHKw==` | `isError:false`，`loadedCount:2`，`rootCount:2` |
| `query_nodes(keyword=Equip, autoParse=true)` | — | `isError:false`，命中 `Equip`、`Equip/BtPetEquipShow`、`Equip/BtPetEquipHide`、`ToolTip/Equip`、`ShortCut/BtEquip`、`UtilDlgEx_MultiPetEquip` |
| `list_children(Equip)` | — | `backgrnd`(175×304 CANVAS)、`pet`、`FullBackgrnd`、`BtDetail`、`BtPet1-3`、`BtPetEquipShow/Hide`、`BtCashshop` |
| `query_nodes(BasicEff, dropItem)` / `(Aura)` | — | `matches:[]` → **掉落光柱素材未入库** |
| `unload_all` | — | `rootCount:0, ok:true`（按 §3 收口，无根锁残留） |

关键口径：`list_children` 首次返回 `children:[]` 是因为 IMG 未 `autoParse`；加 `autoParse:true` 后全部正常。**不是 key 错、不是服务端故障**——与 AGENTS.md §3「128B 才对」一致。

### 1.2 槽位编号：服务端 / 客户端 / BP 表三方对账

- 客户端唯一真源 [`Character/Look/EquipSlot.h`](../../../Character/Look/EquipSlot.h)：
  `HAT..RING2` 连续，`RING3=15, RING4=16, PENDANT1=17, TAMEDMOB=18, SADDLE=19, MEDAL=49, BELT=50, POCKET=51, BOOK=52, PENDANT2=53, SHOULDER=54, ANDROID_SLOT=55, EMBLEM=56, BADGE=57, SUBWEAPON=58, HEART=59, TOP_DEFAULT..LENGTH`。
  **注意**：`PENDANT2/SHOULDER/ANDROID_SLOT/...` 是 Journey 上游的「顺序占位值」，不是 v83 的 BodyPart 负数编号；`POCKET=51` 与 BP 表 `-51`（第二项链）语义冲突。
- 服务端 [`E:\project\BeiDou-Server_s9\...\EquipSlot.java`](../../../../BeiDou-Server_s9/gms-server/src/main/java/org/gms/constants/inventory/EquipSlot.java)：
  `RING("Ri", -12,-13,-15,-16)`（四戒指）、`PENDANT("Pe", -17)`（**仅一条项链**）、`MEDAL(-49)`、`BELT(-50)`、`PET_EQUIP` 收尾。**没有 SHOULDER、没有第二项链、没有图腾/纹章/机器人/心脏/徽章**。
- BP 表（`E:\MXD\扩展改动\护肩槽\槽位基址.txt` 与 `扩展装备栏\message.txt` 逐行相同）：
  BP12/13/15/16 = 戒指四格；BP17 = `112` 项链；BP20 = `-20`，注释「vanilla; often repurposed as shoulder」；BP51 = `-51`，`112` second pendant option，**`no UI table entry`**；BP49/50 = 勋章/腰带。
- 参考文档（`护肩槽\README.txt`）：SOURCE 侧 `SHOULDER("Sh", -20)`、`EquipType.SHOULDER(115)`、`itemId 1152000..1153000` 是护肩；CLIENT 侧三个 hook 修 `get_equip_data_path` / `is_correct_bodypart` / `get_bodypart_from_item`，把 category `115` 映射到 `nBodyPart==20`。

**冲突点（待裁决）**：客户端枚举值、服务端负数槽位、BP 表三套编号并存。装备穿戴包 `EquipItemPacket(int16 src, EquipSlot::Id dest)` 走 `-dest`（`InventoryPackets.h:71`），所以**枚举值直接就是 wire 上的负数绝对值**。当前 `SHOULDER=54` 会发 `-54`，而服务端/参考口径是 `-20`。

### 1.3 UI 版式唯一真源

[`IO/UITypes/UIEquipInventory.cpp:112-132`](../../../IO/UITypes/UIEquipInventory.cpp)：

```
iconpositions 表（175×304 backgrnd，列 5/38/71/104/137，行 32/65/98/131/164/197/230，步进 33）
HAT(71,32) MEDAL(5,65) FACE(38,65) RING3(104,65) RING4(137,65)
EYEACC(71,98) EARACC(104,98)
CAPE(5,131) TOP(38,131) PENDANT1(71,131) WEAPON(104,131) SHIELD(137,131)=SUBWEAPON
GLOVES(5,164) BOTTOM(38,164) BELT(71,164) RING1(104,164) RING2(137,164)
SHOES(71,197) TAMEDMOB(5,230) SADDLE(38,230)
```

这张表同时服务**绘制 / 命中区 / 调色矩形**（`draw():262`, `slot_by_position():527`, `build_disabled_cells():565`），是三件事的唯一真源。空格 `(38,98)/(137,98)/(5,197)/(38,197)/(104,197)/(137,197)/(71,230)/(104,230)/(137,230)` 是可用落位。

### 1.4 辅助装备底板（④）

- 验收版：`docs/features/addon-equipment/ui/window/v5/normal.png`（**152×105 RGBA**，实测），配套 PSD + `manifest.md`。
- 文案（v5 manifest 锁定）：标题「辅助装备」；上排图腾×4；下排 纹章 / 机器人 / 心脏 / **徽章**。
- 槽位内区约 31×30，原点 `(11,32)/(44,32)/(77,32)/(110,32)` 与 `(11,65)…`，水印偏移 (2,2)。
- 历史 v1→v5 全在 `docs/features/addon-equipment/ui/window/`；README 状态「WZ 入库：未做」、`notes/client-path.md`「客户端路径：未提供」。
- live `UIWindow.img/Equip` 下**无**独立辅助装备窗节点 → 需要新增节点或程序化贴图。

### 1.5 套装系统（⑤）

参考 [`E:\MXD\扩展改动\套装系统\README.md`](../../../../MXD/扩展改动/套装系统/README.md) 的契约：

- 数据权威：服务端 `SetItemManager`（`Etc/SetItemInfo.img` 底稿 + DB `set_item` 覆盖）；客户端本地再解析 `SetItemInfo.img` 做列表/档位展示。
- 自定义 opcode：`0x175 SET_ITEM_FINAL_DAMAGE`（short 最终伤害% + int 皮肤 id）、`0x176 SET_ITEM_SKILL_BONUS`（count + `{setId, enabled, mapleString}`）、`0x179 SET_SKILL_BONUS`（技能 +N）、`0x177 SET_ITEM_BONUS_REQUEST`（客户端按需拉文案）。
- 加成链：`getTotalSetBonus` → 固定值+基础% → `CombatProfileService.resolve(damR/bdR/fdR/无视)` → `applySetActiveSkills` → `updateLocalStats/updatePlayerStats` → 发包。
- UI：`[悬停装备 tip] → [套装 tip] → [已穿比较 tip]`，套装 tip 在 `mainX+mainW+4`；装备 tip 保持原版不改。
- 已知坑：启动期创建 WZ 字体/COM → `0xc0000142`；`AddInfoEx` by-value detour 导致悬停闪退，已禁用。

**本轮实况**：live `Etc/` 目录 20 个 img，**无 `SetItemInfo.img`**；`Effect/SetEff.img`（3.58MB）在。服务端 s9 全库检索**无 `SetItem*` Java 类**（只有 `PacketCreator.java`）。即参考文档的套装实现存在于**另一个服务端分支**（README 写的是 `BeiDou-Server_xy`），当前 s9 没有。

### 1.6 掉落特效（⑥）

参考契约：
- `PacketCreator.java::getDropItemGrade`：对 Equip 相对 base 的 STR/DEX/INT/LUK/WATK/MATK/WDEF/MDEF/ACC/AVOID/HP/MP 差值打分 → `<0:-1, <6:0, <23:1, <40:2, <55:3, <70:4, else:5`。
- 客户端 `dropitemaura.cpp`：hook `OnDropEnterField(0x00506385)` 读包内 1B grade → 4 档 `Rare/Epic/Unique/Legendary` → 从 `Effect/BasicEff.img/dropItemAura/{grade}/{front|back}/{0..15}` 预载 16 帧，120ms 切换 `Putcolor`；另有 `dropItemEffect/{0..15}` 光柱。
- `Effect.wz` 快照在 `掉落特效\Effect.wz`（120KB），`0..5.webp` + `*_光柱.webp` 是 6 档 × 2 组素材缩略。

**本轮实况**：live `Effect/BasicEff.img` 内 `dropItem`/`Aura` 搜索 0 命中 → 素材**未入库**。`Gameplay/MapleMap/Drop.cpp` 的 `Drop` 有 `oid/owner/dest/pickuptype/playerdrop/state/angle/opacity`，**无 grade 字段**；`MapDrops::update()` 在 `spawn.instantiate(icon)` 处实例化（只有 `Texture` 或 `Animation`）。

### 1.7 095 强化（⑦）

- 服务端 `V1.11.7__add_inventoryequipment_potential.sql`：`potential1-3` + `bonusPotential1-2` 五列（GBK 注释，值语义是「潜能条目 id，0=空」，魔方 `CashOperationHandler action 0x40` 随机 1..50），`PacketCreator` 在 equip 包尾以 **5 个 `writeShort`** 下发。
- 客户端 [`Character/Inventory/Equip.h`](../../../Character/Inventory/Equip.h)：`enum Potential {NONE,HIDDEN,RARE,EPIC,UNIQUE,LEGENDARY}`、`potential1-3`/`bonusPotential1-2`、`get_potential/get_bonus_potential` 齐备。
- [`IO/Components/EquipTooltip.h:170-177`](../../../IO/Components/EquipTooltip.h)：`prank/prank_preview`、`haspotlines[5]` / `potlines[5]`（注释「值为 PotentialOption.img 行 id」）。
- [`Net/Packets/CashShopPackets.h:182`](../../../Net/Packets/CashShopPackets.h)：095 魔方重洗请求 `action 0x40 + 装备库存槽位(short)`。
- `Net/Packets/SpecialPackets.h:7`、[`IO/UITypes/UISpecial.h:10`](../../../IO/UITypes/UISpecial.h) 有「095 自定义中文 UI」相关但后者注释明确标为**依据链污染、零行为改动**。

**缺口**：live `Etc/` 无 `PotentialOption.img`；火花(`AdditionalOption`)/宝珠(`Soul`)/星之力(`StarForce`) 是否有痕迹，由子代理盘点矩阵补。

### 1.8 工作树边界（并发会话保护）

`git status --short` 共 **654 项**（`feature/dev_0.0.1`）。逐文件核对本次主战场：

| 目标文件 | 状态 |
|---|---|
| `Character/Look/EquipSlot.h/.cpp` | 干净 |
| `IO/UITypes/UIEquipInventory.h/.cpp` | 干净 |
| `Data/EquipData.cpp/.h` | 干净 |
| `Character/Inventory/Inventory.cpp` | 干净 |
| `Gameplay/MapleMap/Drop.h`, `MapDrops.cpp` | 干净 |
| **`IO/Components/EquipTooltip.cpp/.h`** | **`M`（并发会话在途）** |

口径沿用既有教训：**并发会话在途 hunk 完整留在工作树未动**。本工程若改 `EquipTooltip.cpp` 必须走增量 patch 且先读当前内容，禁止整文件覆盖。

---

## 2. 待裁决的唯一关键问题

槽位编号是 wire 协议的一部分（`EquipItemPacket` 发 `-dest`），服务端 `EquipSlot.java` 只认 `-1..-19/-49/-50/-12/-13/-15/-16`。客户端单方面把 `SHOULDER` 设成 54 会发 `-54`，服务端不认。

另外 ①②③④ 的版式存在两种互斥解读：
- **A 原生扩展**：把新槽画进 `Equip/backgrnd` 那张 175×304 底图的空格（②护肩 / ③第二项链 / ④辅助装备 8 格），需要重绘底图或用 v5 独立子窗。
- **B 独立子窗**：④ 用 v5 的 152×105 辅助装备窗（跟参考图一致），②③ 塞进主窗空格，① 已就位。

这两个选择会实质改变交付物（是否改 WZ 底图、是否新增 opcode、服务端要不要同步改），必须先确认。详见向用户提出的裁决项。

---

## 3. 落地顺序（裁决后执行）

1. **①-④ 槽位**：`EquipSlot.h`（编号对齐）+ `EquipData.cpp`（护肩分类 115 + eqslot 映射）+ `UIEquipInventory.cpp`（iconpositions 补格）+ ④ 辅助装备窗类（复用 v5 底板）+ `Inventory.cpp`（槽位容器上限）。构建门禁：`EnumMap<EquipSlot::Id,...>` 全枚举有构造点。
2. **⑤ 套装**：新增 `Data/SetItemData`（读 `SetItemInfo.img`，仿 `EquipData` 的 `nl::node` 用法）+ `Net` 自定义 opcode 注册 + `SetItemTooltip`（复用 EquipTooltip 的 compare 右侧布局）+ `CharStats` 属性注入点。前置：确认服务端分支（s9 vs xy）。
3. **⑥ 掉落特效**：`Drop` 增 grade 字段 + 下行包解析 + `Effect/BasicEff.img` 素材入库（MCP `mutate_nodes`/`save_as` 安全流）+ `MapDrops::draw` 双层光环。
4. **⑦ 095 补缺口**：按子代理矩阵补字段/文案/UI，不重做已有主链路。
5. **闭环**：MSBuild Release x64 → 产物落 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` → `cmd //c start ""` 真机 → 抓帧 → 证据归档。

---

## 4. 证据文件索引

| 类别 | 路径 |
|---|---|
| 参考·护肩 | `E:\MXD\扩展改动\护肩槽\{README.txt, 槽位基址.txt, shoulders.cpp}` |
| 参考·扩展装备栏 | `E:\MXD\扩展改动\扩展装备栏\{README.txt, message.txt, addon-equipment-backgrnd.png, 扩展装备栏.png, 神秘徽章装备栏.png}` |
| 参考·套装 | `E:\MXD\扩展改动\套装系统\{README.md, SetItemBridge.cpp, SetItemApi.h, SetItemData.h, SetItemBonusRequestHandler.java}`、`E:\MXD\扩展改动\伤害玩法\{套装管理界面设计.md, 伤害设计的简单逻辑.md, 实施清单.md}` |
| 参考·掉落 | `E:\MXD\扩展改动\掉落特效\{dropitemaura.cpp, itemaura.cpp, PacketCreator.java, Effect.wz, 0..5.webp, *_光柱.webp, 原文截图.png}` |
| 参考·095 | `E:\project\BeiDou-Server_s9\gms-server\src\main\resources\db\migration\V1.11.7__add_inventoryequipment_potential.sql` |
| 本仓库·美术 | `docs/features/addon-equipment/ui/window/v5/{normal.png, addon-equipment.psd, manifest.md}` |
| 本仓库·取证 | `docs/features/ui-v83-window-restore/evidence/r27-b3-equip/`（底图逐格实测） |
| live 资源 | `E:\MXD\BeiDou-Client-xy\Data\{UI,Effect,Etc,String}\*.img` |
| 服务端 | `E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\constants\inventory\EquipSlot.java` |
