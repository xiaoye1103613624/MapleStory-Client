# 分类移植方法论 + 代表样例

> 配合 `README.md` 分级矩阵。凡涉 img 一律以运行时部署 `Data/<ns>/*.img` 为准（红线 R1/R2/R5/R6）。

## A 类 — 纯资源落地方法论

**步骤**
1. 识别资源命名空间：武器/装备→`Data/Item/`；皮肤/UI→`Data/UI/`；地图→`Data/Map/`；BOSS/Mob→`Data/Mob/`；怪物卡→`Data/Item/` 或 `Data/String/`。
2. 将 `.img/.wz` 解包/直放至对应 `Data/<ns>/`，`.png` 若为 UI 图集需确认是否已被 `UIWindow.img` 等节点引用（nlnx 走 `UiAssetResolve::window(name)`）。
3. 用 `MapleStory.exe --resource-probe` 跑 `RT-CATALOG`/`RT-DUMP` 校验节点存在与 `_width/_height`（R1/R2）。
4. 按 R5 逐像素核验布局/动画节点（类型 bitmap/animation、帧序列），确认无 `none` 容器被当 bitmap 传。

**代表样例 — 200级武器**
- 资源：`.img`×28，推测为 `Item/Equip/Weapon/<itemid>.img` 或独立武器贴图包。
- 校验：`--resource-probe` 确认 `nl::nx::Item["Equip"]["Weapon"]` 下新增 itemid 节点存在；客户端 `UIEquipInventory` 按 itemid 取图标走 `nl::nx::Item` 资源解析，无需改代码（除非 itemid 超出当前 `get_item` 范围 → 需确认 `ItemConstants`）。
- 服务端：`Item` 表需存在对应 itemid（B 类依赖），否则仅显示空槽。

**代表样例 — 妖精学院/遗忘山谷（地图资源+脚本）**
- 资源：`img`+`xml`+`js`（地图 .img + 脚本）。`js` 多为地图生命周期/刷怪脚本，须对照 s9 脚本引擎（NPC/Map script）适配 → 落入 B 类处理。

## B 类 / E 类 — 服务端 Java + SQL 迁移方法论

**步骤**
1. 在 s9 `gms-server/src/main/java/org/gms/` 下定位：发包枢纽 `util/PacketCreator.java`（276KB）、对应 `handler/`、实体 `model/`（如 `Character`、`Account`）、SQL 迁移 `src/main/resources/db/migration/Vx.x__*.sql`。
2. `.patch` 文件按 hunks 与 s9 当前文件 `diff`；`.java` 片段移植为 s9 对应类方法。
3. 新表/新列走 **Flyway 迁移**（新增 `V<next>__<feature>.sql`），绝不手改已上线库。
4. Opcode 已与客户端对齐（以服务端 `RecvOpcode`/`SendOpcode` 为准），新增包须两端同步登记。

**代表样例 — 美容院（跨 B+E+A+C）**
- 服务端：`BeautySalon*.java`（12 个）含发型/脸型/皮肤商城逻辑；`sql=6` 可能为美容记录/优惠券表。
- 迁移：新增美容相关表（参照现有 `accounts`/`characters` 风格），`PacketCreator` 增加美容结果包。
- 客户端：`cpp=3 h=24` 为美容 UI（`UIBeauty` 或并入 `UICharSelect`/`UICreateChar`），须映射到本工程 `IO/UITypes`。
- 资源：`wz=13` 为美容 UI 资源。

**代表样例 — 角色槽位（跨 C+B+E）**
- 服务端：`java=1` + `sql=2`（账号角色数上限表/列）+ `CharlistHandler` 槽数。
- 客户端：`cpp=2`（`UICharSelect` 槽位布局，原版多槽分页）→ 本工程 `IO/UITypes/UICharSelect.cpp` 重写槽矩阵。
- 资源：`wz=1`。

## C 类 — 客户端源码重实现方法论

**步骤**
1. 通读原版 `.cpp/.h`，提取：UI 类名（`CUI*/CWnd*`）、资源节点路径（`UI/<file>.img/<node>`）、交互逻辑、协议包（发包 opcode + 字段）。
2. 映射到本工程：UI 组件放 `IO/UITypes/<UIXxx>.{h,cpp}`，资源经 `UiAssetResolve::window(name)` / `nl::nx::UI[...]`，坐标按运行时 img 取（R5）。
3. 协议：发包构造在 `Net/OutPacket.cpp`（如 `SelectCharPacket` 风格），opcode 取自 `OutPacket.h` 枚举；须与 s9 `RecvOpcode` 对齐。
4. 构建验证：`tools/build_smoke.bat` EXITCODE=0；`--resource-probe` 核验资源。

**代表样例 — 扩展装备栏（cpp=1 png=10）**
- 原版逻辑：扩展 `UIEquipInventory` 槽位（如增设护肩/徽章/口袋等 v83 后新增槽位）。
- 本工程落点：`IO/UITypes/UIEquipInventory.cpp` 的槽位枚举与坐标矩阵（参照 r05 `notes/r05-ui-layout.md` 的 Equip 5 列×7 行布局），扩充槽位须同步 `EquipSlot` 枚举与服务端 `Item` 装备位校验。
- 资源：`png=10` 为扩展槽背景。

**代表样例 — 世界地图 tip（cpp=8 h=5 java=1）**
- 原版：`UIWorldMap` 悬浮 tooltip（怪物/掉落/NPC）。本工程已有 `IO/UITypes/UIWorldMap.cpp`（世界地图三问题已实机修复）。
- 落点：在现有 `UIWorldMap` 的地图点 hover 逻辑上增加 tip 渲染（资源经 `nl::nx::UI`/Map 节点），`java=1` 为服务端地图信息包（若有）。

**代表样例 — 修复名片错位 NameTag（img=1）**
- 资源已给（img=1，名片背景）；实际修复是客户端 `NameTag` 绘制坐标（参照 r05 名片锚点），属 C 类小改。

## D 类 — 二进制补丁源码重写方法论（用 rea 逆向辅助）

**步骤**
1. `rea` 加载原版 `BeiDou.exe`/零售客户端，将硬编码地址（如 `ZtlSecureTear_short@0x004E80EB`、`ZtlSecureFuse_short@0x004746DD`）逆向定位其**功能语义**（HP/MP 编解码 + 校验和保险），而非照搬地址。
2. 在本工程源码找等价点：`Character/CharStats.h` 的 `basestats`（`EnumMap<MapleStat::Id, uint16_t>`）、`set_stat(..., uint16_t)`、收包解码（`GW_CharacterStat::Decode` 风格，本工程在 `Net/LoginParser`/`CharStats` 解析处）、发包编码（`PacketCreator`/`OutPacket`）。
3. 类型升级 `uint16_t → int32_t`，并同步**所有**读写站点（等价原版 FakeTear/Fuse/movsx 全站点），以及服务端 `writeShort→writeInt`、DB 列宽。

**代表样例 — 99999GW（HP/MP 突破）**
- 已核实：本工程 `CharStats.h` 的 `basestats` 为 `uint16_t`，HP/MaxHP/MP/MaxMP 上限 65535。升级为 `int32_t`。
- 客户端改动点：
  - `CharStats` 编解码（`Decode`/`set_stat` 调用处）改为 4 字节；
  - `UIStatusBar`/`UIStatsInfo` 显示（原版 `movsx` 截断站点对应本工程的 `int` 格式化，天然无截断，但数值格式化/裁剪需注意）；
  - `Stat` 上限常量（原版 99999 钳制）同步。
- 服务端改动点（s9）：`PacketCreator.addCharStats`/`updatePlayerStats`/`getWarpToMap` 的 HP/MP 由 `writeShort`→`writeInt`；`AbstractCharacterObject` 钳制 30000→99999。
- DB：`characters` 表 hp/mp/maxhp/maxmp 列宽（若为 `smallint` 升 `int`）。
- 验证：`debug.packet_dump` 真连 s9 抓包比对字节宽；`--resource-probe` 无资源影响。

## 跨栈强耦合端到端（摘要）

见 `README.md §4`。共同约束：**客户端与服务端字节宽/枚举/常量必须同步发布**，否则收发包错位导致登录/选角/进图异常（参考 r13 抓包对齐经验）。
