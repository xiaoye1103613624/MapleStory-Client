# 官方装备号段 ↔ BodyPart ↔ 槽位编号 三方映射（一手取证）

> 日期：2026-11-12　状态：**已定案，可直接作为实现口径**
> 触发：用户追问「冒险岛高版本是否有？槽位号段是否要和装备 ID 开头保持一致？多方面考虑后结合参考文档编号，涉及服务端一并修改，保证槽位可用、装备 id 号段与官方一致、装备可穿戴和使用」。

---

## 0. 直接回答两个问题

### Q1：高版本有没有这些槽位？——**有，而且之前在本机就开发过完整实现**

不是「从零设计」，本机已有**两套**成品：

| 来源 | 内容 | 路径 |
|---|---|---|
| **服务端 S10（权威）** | `EquipSlot.java` 完整定义 6 戒 / 第二项链 / 护肩 / 口袋 / 辅助武器 / 徽章 / 图腾×4 / 纹章 / 机器人 / 心脏，含 cash mirror、`isPetEquipStorageSlot` 防串槽 | `E:\project\BeiDou-Server_S10\gms-server\src\main\java\org\gms\constants\inventory\EquipSlot.java` |
| **服务端 xy（原套装分支）** | 同名 `SetItemManager` + DB `V1.11.10__set_item.sql` + `SetItemController/Service` | `E:\project\BeiDou-Server\gms-server\...` |
| **客户端 ijl15（配套）** | `ezorsia/setitem/setitem.cpp`(94KB) + `SetItemBridge.cpp` + 槽位相关 Hook | `E:\project\BeiDou-ijl15\ezorsia\setitem\` |
| **095REA 独立档案** | v95 客户端/服务端/DB/WZ 全量静态取证（GMS v95 IDB 22,428 函数 + 62 表 + 67,663 XML-WZ） | `E:\MXD\BeiDou-Client-xy\REA反编译\095REA\` |
| **S10 资源** | `SetItemInfo.client-extra.xml`(697KB)、`set_item_names_zh.tsv`、双语 multiset migration | `E:\project\BeiDou-Server_S10\gms-server\src\main\resources\setitem\` |

**当前 s9 是唯一没有这些的分支**（`EquipSlot.java` 只到 `BELT(-50)`、无 `SetItem*` 类）。用户的「涉及服务端也一并修改」= 把 S10/xy 的成品移植进 s9。

### Q2：槽位编号要不要和装备 ID 开头一致？——**不要，两套编号，靠 `islot` 映射**

这是最容易踩的坑。官方（含 S10 私服实现）里：

- **装备 ID 号段**（itemid / 10000，或前 3 位）是**物品分类**：`100` 帽、`101` 脸饰、`115` 护肩、`116` 口袋、`119` 纹章、`166` 机器人、`167` 心脏、`134/135` 辅助武器、`180-183` 宠物装备。
- **槽位编号**（BodyPart / 负数 equipped position）是**穿戴位置**：`-20` 护肩、`-33` 口袋、`-54` 徽章、`-55..-58` 图腾、`-59` 纹章、`-60` 机器人、`-61` 心脏、`-62` 辅助武器。
  （以上为 S10 全量事实描述。本端 Round 4 只采纳其中用户 7 项对应的 `-20/-51/-54/-55..-58/-59/-60/-61`；`-33` 口袋与 `-62` 辅助武器不采纳，理由见 §1 表格「处置」列。）
- 两者**不同值、不同语义**，`EquipSlot.java` 的 `getName()` 返回的是 WZ `islot` 字符串（`Cp/Af/Ay/Sh/Po/Em/Dr/Ht/To/Ba/Aw`），`ItemInformationProvider` 负责把 itemid 分类映射到 islot，再由 `isAllowed(slot,cash)` 校验穿戴位。

S10 源码原文注释（逐字）：

```
SHOULDER("Sh", -20),           // 肩饰（115xxx；客户端 BP20；勿用 Sd/−8，与手套冲突）
POCKET("Po", -33),             // 口袋 116xxxx：主栏红 9 / BP33
EMBLEM("Em", -59, -159),       // 纹章 119xxxx（sidecar 可穿）
ANDROID("Dr", -60, -160),      // 机器人 166xxxx（sidecar BP60）
HEART("Ht", -61, -161),        // 心脏 167xxxx（sidecar BP61 / Machine Heart）
AUX_WEAPON("Aw", -62, -162),   // 辅助武器 134/135（经典装备栏红10）
// 119→−59 only（WZ islot Si 不得走 SHIELD）
// 机器人/心脏必须用 sidecar BP60/61（cash −160/−161），禁止 −21/−22
```

**结论**：`115xxxx` 的装备穿到 `-20` 位，不是 `-115`；`119xxxx` 穿到 `-59`，不是 `-119`。客户端 `EquipItemPacket` 发 `-dest`，所以枚举值必须等于 `|BodyPart|`，**不能**用 itemid 号段当槽位值。

---

## 1. 完整官方映射表（S10 全量口径 → 本端实际采纳，Round 4 修订）

> **本端最终采纳的新增前缀只有 6 个**：`115 护肩 / 118 徽章 / 119 纹章 / 120 图腾 / 166 机器人 / 167 心脏`（三端一致，机器校验见 `verify_no_conflict.py`）。S10 有但用户 7 项没有的槽位（POCKET / AUX_WEAPON / 六戒）一律不引入 —— 依据用户约束「不影响原始装备的逻辑」。


| 槽位 | islot | 正常位 | cash 镜像 | 装备 ID 号段 | BP 表 | 本项目 `EquipSlot.h` 现值 | 处置 |
|---|---|---:|---|---|---:|---:|---|
| HAT | Cp | -1 | -101 | 100xxxx | 1 | 1 | 一致 |
| SPECIAL_HAT | HrCp | -1 | -101 | 100xxxx | 1 | — | 服务端别名，客户端无需独立槽 |
| FACE | Af | -2 | -102 | 101xxxx | 2 | 2 | 一致 |
| EYEACC | Ay | -3 | -103 | 102xxxx | 3 | 3 | 一致 |
| EARACC | Ae | -4 | -104 | 103xxxx | 4 | 4 | 一致 |
| TOP | Ma | -5 | -105 | 104/105xxxx | 5 | 5 | 一致 |
| BOTTOM | Pn | -6 | -106 | 106xxxx | 6 | 6 | 一致 |
| SHOES | So | -7 | -107 | 107xxxx | 7 | 7 | 一致 |
| GLOVES | GlGw | -8 | -108 | 108xxxx | 8 | 8 | 一致 |
| CAPE | Sr | -9 | -109 | 110xxxx | 9 | 9 | 一致 |
| SHIELD | Si | -10 | -110 | 109xxxx | 10 | 10 | 一致；S10 明确「109 → −10 only」 |
| WEAPON | Wp | -11 | -111 | 13x/14x/16x/17x | 11 | 11 | 一致 |
| RING1 | Ri | -12 | -112 | 111xxxx | 12 | 12 | 一致 |
| RING2 | Ri | -13 | -113 | 111xxxx | 13 | 13 | 一致 |
| RING3 | Ri | -15 | -115 | 111xxxx | 15 | 15 | 一致（①右上第 3 戒） |
| RING4 | Ri | -16 | -116 | 111xxxx | 16 | 16 | 一致（①右上第 4 戒） |
| ~~RING5~~ | Ri | -52 | -152 | 111xxxx | — | **不采纳** | Round 4：用户 ① 只要右上 2 格；六戒是 S10 sidecar 位，无 UI 落位 |
| ~~RING6~~ | Ri | -53 | -153 | 111xxxx | — | **不采纳** | 同上 |
| PENDANT1 | Pe | -17 | -117 | 112xxxx | 17 | 17 | 一致 |
| **PENDANT2** | Pe | **-51** | -151 | 112xxxx | 51「无 UI 表项」 | **53** | **③ 改 53→51**（BP51 原生第二项链） |
| TAMEDMOB | Tm | -18 | -118 | 190xxxx | 18 | 18 | 一致 |
| SADDLE | Sd | -19 | -119 | 191xxxx | 19 | 19 | 一致 |
| **SHOULDER** | Sh | **-20** | -120 | **115xxxx** | 20「often repurposed」 | **54** | **② 改 54→20** |
| ~~POCKET~~ | Po | -33 | -133 | 116xxxx | 33 | **不采纳** | Round 4：`-33/-133` 归一化 = **BP33 ∈ 宠物座位**（S10 原文注 -133=2号宠拾取袋），会串宠物装备；且不在用户 7 项内 |
| ~~AUX_WEAPON~~ | Aw | -62 | -162 | 134/135xxxx | — | **不采纳** | Round 4：live 里 **prefix 134 = 111 个原始武器 .img**，override 它等于改判原始装备。`SUBWEAPON` 改回 Journey 原义 = 与 SHIELD 同值别名（wire -10，共格 (137,131)）|
| MEDAL | Me | -49 | -149 | 114xxxx | 49 | 49 | 一致 |
| BELT | Be | -50 | -150 | 113xxxx | 50 | 50 | 一致 |
| **BADGE** | Ba | **-54** | -154 | 118xxxx? | 54 | 57 | **④ 改 57→54** |
| **TOTEM1-4** | To | **-55,-56,-57,-58** | -155..-158 | 120xxxx? | 55-58 | — | **④ 新增 4 个枚举** |
| **EMBLEM** | Em | **-59** | -159 | **119xxxx** | 59 | 56 | **④ 改 56→59** |
| **ANDROID_SLOT** | Dr | **-60** | -160 | **166xxxx** | 60 | 55 | **④ 改 55→60** |
| **HEART** | Ht | **-61** | -161 | **167xxxx** | 61 | 59 | **④ 改 59→61** |

> ⚠️ **本节 Round 4 翻案（2026-11-12）**：Round 2 曾断言「`EnumMap` 与 `by_id()` 依赖连续枚举，必须整体重排为连续值」—— **该结论错误**，且据它改出的连续枚举把原始装备（MEDAL/BELT/RING3/RING4/TAMEDMOB/SADDLE）的图标显示与卸下路径改回归了。
> 一手依据：`ItemParser.cpp:110-113` 把下行负 slot 取反后作为 EQUIPPED 容器 key 入库，`Player.cpp:229-232` 的 `get_item_id(EQUIPPED, slot)` 与 `by_id(slot)` 用同一个数，`Inventory.cpp:270-273` 的 `has_equipped()` 直接 `count(slot)` —— 所以**枚举值必须等于 wire BodyPart 绝对值**。`EnumMap` 的空洞只多占几个空数组项，`iconpositions` 为 (0,0) 时 UI 会跳过（`UIEquipInventory.cpp:544-545`）。
> 空洞 21..48 恰是 v083 宠物座位 BP（`PET_BODY_PARTS`），本端**从不分配角色装备到这些值**，这正是用户「不与宠物装备冲突」的落实点。机器门禁见 `verify_no_conflict.py`。

### v83 原生 BP 表 vs S10 扩展位的区别

`槽位基址.txt` 的 BP1-50 是 **v083 客户端内存表**（`0x00BE2260..0x00BE23E8`），`-51..-62` 是 S10 私服通过 `sidecar` 机制新增的（不占原生 UI 表项，靠客户端 `GetItem/SetItem` 放行）。S10 注释逐字：

```
// 6 戒：原 −12/−13/−15/−16 + 新增 −52/−53（UI 红标 3/4）。
// 客户端 RING_34_BIND_20260726aa：GetItem/SetItem 放行 −52/−53 + draw→BP53 + get_bodypart×6。
// 第二吊坠 −51（083 原生 BP51）。UI：pendant2_ui=true 进图后挂槽。
// 扩展装备栏 Addon 2×4：Top: Totem×4 BP55–58；Bot: Emblem59 / Android60 / Heart61 / Badge54
```

---

## 2. 用户 7 项 → 官方槽位归位

| # | 需求 | 官方槽位 | 本仓库动作 |
|---:|---|---|---|
| ① | 右上增 2 戒指 | `-15/-16`（RING3/4） | **已就位**：`UIEquipInventory.cpp:115-116`(104,65)/(137,65)。仅需真机核验 + 若跑 6 戒再补 -52/-53 |
| ② | 护肩 | `-20`，itemid **1152000-1152999** | `SHOULDER 54→20`；`EquipData.cpp` 分类表补 index 15 `SHOULDER`；UI 空格落位 |
| ③ | 眼饰左侧第二项链 | `-51` | `PENDANT2 53→51`；UI 空格（眼饰 (71,98) 左侧即 (38,98)） |
| ④ | 扩展（辅助）装备段 8 格 | `-54/-55..-58/-59/-60/-61`（徽章/图腾×4/纹章/机器人/心脏） | 枚举重排 + v5 独立子窗（152×105）；注：用户文案「1 新增」= 心脏(167xxxx) |
| ⑤ | 套装系统 | 无独立槽位，按已穿戴 itemid 集合统计 | 移植 S10/xy `SetItemManager` 进 s9 + 客户端 tip/属性注入 |
| ⑥ | 掉落特效 | 无槽位 | grade 字节 + `BasicEff.img` 素材 + Animation |
| ⑦ | 095 强化 | 无槽位 | 补词条表/5 行绘制/火花/宝珠/星力 |

---

## 3. 保证「装备可穿戴和使用」的端到端清单（跨仓库）

按用户裁决「涉及服务端也一并修改」，以下是必须同时落地的最小集合，缺一件就穿不上：

### A. 服务端 `E:\project\BeiDou-Server_s9`

1. **`constants/inventory/EquipSlot.java`** — 以 S10 版本为准替换（保留 s9 特有的 `isPetEquipStorageSlot` 若已有）。这是 `isAllowed()` 的权威，决定穿戴包是否被拒。
2. **`ItemInformationProvider.java`** — 补 `115xxxx`（护肩）、`118xxxx`（徽章）、`119xxxx`（纹章）、`120xxxx`（图腾）、`166xxxx`（机器人）、`167xxxx`（心脏）的 `islot` 判定。
   > **Round 4 修订**：原清单里的 `116xxxx`（口袋）与 `134/135xxxx`（辅助武器）**已剔除** ——口袋的 `-33/-133` 归一化后是宠物座位 BP33（会串宠物装备），134/135 是 live 里 111 个原始武器 .img（override 即改判原始装备）；两者都不在用户 7 项内。
   护肩参考文档原文：
   ```
   itemId >= 1152000 && itemId < 1153000  →  shoulder
   ```
3. **`EquipType.java`** — 补 `SHOULDER(115)` 等类型常量（`护肩槽\README.txt:13-20`）。
4. **`String/Eqp.img` 侧** — 高版本装备名要能显示（S10 有 `set_item_names_zh.tsv` 与 `SetItemInfo.client-extra.xml` 可导）。
5. **DB** — s9 已有 `V1.11.7__add_inventoryequipment_potential.sql`（095 潜能 5 列）；套装需补 `V1.11.10__set_item.sql`（xy 版 `xy_set_item` 表，含 `tiers_json`）。
6. **`SendOpcode/RecvOpcode` + `PacketCreator`** — 套装 4 个自定义 opcode（注意 0x179=377 与 `WORLD_MAP_DROP_RESULT` 冲突，需换号或并入 0x176 的 mode 字节，见子代理②盘点）。
7. **`CashOperationHandler`** — 已有 0x40 魔方重洗；补火花/宝珠/星力 action。
8. **`PacketCreator.getDropItemGrade` + `dropEnterField`** — 掉落特效的 grade 1 字节（参考 `掉落特效\PacketCreator.java` 与 `收纳背包\...\PacketCreator.java:2074`）。

### B. 客户端 `E:\project\MapleStory-Client`

1. `Character/Look/EquipSlot.h` — 枚举整体重排到官方值（见 §1 表），保留 `LENGTH` 收敛。
2. `Data/EquipData.cpp:61-138` — 分类表补 15 项之外的高版本号段 → `eqslot` 映射（当前 115 掉进 `CASH/NONE` 是护肩穿不上的真根因）。
3. `IO/UITypes/UIEquipInventory.cpp:112-132` — `iconpositions` 补 ②护肩 / ③第二项链空格；④ 独立子窗新类（复用 v5 底板）。
4. `Character/Inventory/Inventory.cpp:27-33` — `slotmaxima[EQUIPPED] = EquipSlot::LENGTH` 自动跟随新枚举。
5. `Net/Handlers/Helpers/ItemParser.cpp:116-123` — 加 `recv.available()` 版本门控（防 pre-V1.11.7 库 10B 缺失）。
6. 套装：`Data/SetItemData.*`（仿 `ItemData` 的 `Cache<T>` + `nl::nx::Etc`）、`IO/Components/SetTooltip.*`（复用 `TooltipBackdrop`/`BodyLine`）、`Net/Handlers/SetItemHandlers.*`、`Net/Packets/SetItemPackets.h`、`Character/Player.cpp:117` 注入加成；**`EquipTooltip.cpp` 并发在途只插行不重写**。
7. 掉落：`MapObjectHandlers.cpp:858-863` 插 grade 读取（P0：当前 `read_bool()` 会吃到 grade 字节导致 playerdrop 错位）、`Spawn.h/.cpp` 透传、`Drop.h` 加 3 个 `Animation`（front/back/effect）、`ItemDrop.cpp` 前后分层绘制；素材经 MCP 入库 `Effect/BasicEff.img/dropItemAura/{Rare,Epic,Unique,Legendary}/{front,back}/{0..15}` + `dropItemEffect/{0..15}`（参考目录有 `0..5.webp` / `*_光柱.webp` / `Effect.wz` 快照）。
8. 095：`Data/PotentialData.*` + 服务端词条文案通道 + `EquipTooltip` 5 行绘制 + 火花/宝珠/星力字段/UI/请求包 + `Player::recalc_stats` 注入。

### C. 资源（MCP 安全流）

- `UIWindow.img` 新增辅助装备窗节点或程序化贴图；`Effect/BasicEff.img` 补光柱；`Etc/SetItemInfo.img` 从 S10 的 `SetItemInfo.client-extra.xml`(697KB) 转入库。
- 全部按 AGENTS.md §3：`load_files(exclusive:false)` → `mutate_nodes` → `save_as` **不同路径** → `unload_all` → 原子替换；禁止 `Copy-Item` 裸拷 `.img`；图标 ARGB4444；改 `Item` 时 `analyze_resource_links` 同步 `String`/`Character`。

---

## 4. 两个必须避开的坑（一手证据）

1. **opcode 0x179 冲突**：`Net/PacketSwitch.cpp:410-413` 已有 `WORLD_MAP_DROP_RESULT=377(0x179)` / `WORLD_MAP_PLAYERS=376`。参考 README 的 `SET_SKILL_BONUS(0x179)` 直接搬会撞车 → 换号或并入 `0x176` 的 mode 字节。
2. **cash 兜底漏洞**：S10 源码注释明确删掉了 `return cash && slot < 0` 的 catch-all，因为它会让「帽子时装装备到宠物」(`-121/-122`)。移植时**必须**保留 `isPetEquipStorageSlot` 与 dual-band 精确匹配，不能图省事恢复兜底。

---

## 5. 证据源清单

| 结论 | 一手来源 |
|---|---|
| 官方槽位编号/号段/cash 镜像/防串槽 | `E:\project\BeiDou-Server_S10\gms-server\src\main\java\org\gms\constants\inventory\EquipSlot.java`（全文 307 行，注释逐字引用） |
| 护肩 1152xxx 号段 + `SHOULDER(115)` + 3 个 hook 契约 | `E:\MXD\扩展改动\护肩槽\README.txt:4-28,31-83` |
| v83 原生 BP1-50 + `-51` 第二项链 + `-20` 可复用护肩 | `E:\MXD\扩展改动\护肩槽\槽位基址.txt`（与 `扩展装备栏\message.txt` 逐行相同） |
| s9 现状（只到 -50） | `E:\project\BeiDou-Server_s9\...\EquipSlot.java` |
| 既有套装实现 | `E:\project\BeiDou-Server\...\server\setitem\SetItemManager.java`(25KB)、`BeiDou-Server_S10\...\SetItemManager.java`(30KB)、`V1.11.10__set_item.sql`、`SetItemInfo.client-extra.xml`(697KB)、`E:\project\BeiDou-ijl15\ezorsia\setitem\setitem.cpp`(94KB) |
| 095 高版本取证 | `E:\MXD\BeiDou-Client-xy\REA反编译\095REA\README.md`、`notes\06-095vs083-差异.md`、`wz\client_wz_inventory.md`（17 个 .wz / 6GB / v95 IDB 22,428 函数 / 62 表） |
| 辅助装备 UI 文案与 v5 底板 | `docs/features/addon-equipment/{README.md,ui/window/v5/manifest.md,ui/window/v5/normal.png}` |
| MCP 资源实测 | 本会话 `query_nodes/list_children` 原始输出（见 `README.md` §1.1） |
