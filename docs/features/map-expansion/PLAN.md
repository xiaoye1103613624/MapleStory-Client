# 大地图扩展功能 — 可行性研究与实施计划

> 范围：基于已有的两份资料完成"大地图（世界地图 WorldMap）扩展"  
> 资料：`docs/features/map-search-ui/`（右侧地图搜索面板，壳已落地）  
> 　　　`docs/features/map-info-panel/`（左侧地图信息面板，资产已出、C++ 未写）  
> 日期：2026-11-01  
> 适用：`E:\project\MapleStory-Client` → 部署 `E:\MXD\BeiDou-Client-xy\`（BeiDou gms083，1280×720）

---

## 0. 结论速览（可行性）

- **整体可行**。大地图基础已在 v83-restore W11 对齐原生：边框九切片（654×521）、默认居中位置 `(313,64)`、左侧怪物栏与右侧掉落栏已闭环。
- 两份资料对应两块扩展：
  - **右侧地图搜索面板 `UIMapSearchPanel`**（260×537）：壳 + Tab/返回/传送已接好；**搜索/过滤/结果列表/详情卡未实现**。
  - **左侧地图信息面板 `UIMapInfoPanel`**（240×537）：决策 D1–D7 已锁定、PNG + `layout.json` 已生成；**C++ 完全未写**。
- **主要约束 = 数据**：`MobDropData` 是 stub（无真表）→ 搜索/信息面板里的"掉落"只能空态，需数据源才能填充。`MapMobInfo` 缺 `exp`/`boss` 字段（信息面板需要）。
- **原生缺失资产**（本 v83 包无素材，只能显式预留，无法完全对齐原生）：`WorldMap/WorldMapSearch` 搜索栏、`BtSearch`/`BtAutoFly_1`/`BtNaviRegister`、`MiniMap/ListNpc`、`MiniMapSimpleMode`。

---

## 1. 当前状态盘点

| 模块 | 状态 | 说明 |
| --- | --- | --- |
| 大地图边框九切片 | ✅ 已对齐 | W11，`Border{0..7}`，654×521 |
| 默认位置 | ✅ 已对齐 | `PosMAP (313,64)` 居中偏上 |
| 左侧怪物栏 `UIMapMobPanel` | ✅ 可用 | 读 `get_mobs_on_map` → 图标/Lv/名/HP |
| 右侧掉落栏 `UIMapDropPanel` | △ 壳+空态 | 数据 stub，永远 "No drop data" |
| 旧搜索栏 `WorldMapSearch` 轨 | △ 缺资产 | 背景空 → 漂浮搜索框文字伪影 |
| 右侧搜索面板 `UIMapSearchPanel` | △ 壳 | Tab/返回/传送接好；搜索/过滤/结果未实现 |
| 左侧信息面板 `UIMapInfoPanel` | ❌ 未实现 | 资产 + `layout.json` 就绪，无 C++ |
| `MapMobInfo`(exp/boss) | ❌ 缺字段 | 信息面板 EXP/BOSS 前置 |
| `MobDropData` 真表 | ❌ stub | 掉落内容无法填充 |
| 原生 ListNpc/AutoFly/NaviRegister | ❌ 缺资产 | 预留，不强行实现 |

---

## 2. 相较于原生的错误/缺口（Phase 0 修复项）

| ID | 问题 | 修复 | 风险 |
| --- | --- | --- | --- |
| **E0-1** | 旧搜索栏 `WorldMapSearch` 素材缺失却默认 `set_search(true)`，导致世界地图右侧漂浮一个无框搜索框文字；`BT_SEARCH_CLOSE` 与 `BT_CLOSE` 同位（虽两者都缺件未创建，但文字仍画）。 | 仅在 `search_background` 有效时启用旧搜索轨；本包缺件 → 默认隐藏，由新 `UIMapSearchPanel` 接管搜索。 | 低（已确认 `WorldMap/BtSearch`/`WorldMapSearch` 均无素材，按钮本就不创建） |
| **E0-2** | 信息面板缺 `exp`/`boss` 数据字段 | `MapMobInfo` 增 `int32_t exp` / `bool boss`；`get_mobs_on_map` 读 `Mob/{id}.img/info/exp`、`info/boss`（WZ 有节点） | 低（纯增字段，不影响现有怪物栏） |
| **E0-3** | 原生子功能 `ListNpc`/`BtAutoFly_1`/`BtNaviRegister` 无素材 | 确认 `if(node)` guard 不崩即可；保持显式预留，不编造行为 | 无（已 guard） |

> 说明：W11 已把大地图边框/位置/怪物/掉落对齐原生，**未发现代码级回归错误**。剩余项多为"缺资产预留"或"未实现的扩展功能"，而非 bug。E0-1 是唯一明确的视觉伪影，优先清掉。

---

## 3. 扩展一：右侧地图搜索面板（map-search-ui）收口

### 3.1 可行性
- **搜索/过滤/结果列表**：纯客户端。数据源可用——地图索引可由 `Map.nx` 懒建；怪物由 `get_mobs_on_map(mapid)`；NPC 由 `get_life_on_map(mapid)`；掉落由 `MobDropData`（stub）。
- **传送**：`BtTeleport` + `ChangeMapPacket` 已通（选中地图即 `enabled`），无需新协议。
- **资产**：`UI/MapSearch/` 21 张 PNG 已生成，与 `resources/layout.json` 对齐。**无需 Photoshop**。

### 3.2 实施步骤
1. **数据索引**：首次打开世界地图时懒建内存索引——全部地图（`NxHelper::Map::get_map_info_by_id` 遍历可达 WorldMap）、怪物（按当前地图 `get_mobs_on_map`）、NPC（`get_life_on_map`）、掉落（`MobDropData`）。
2. **结果列表渲染**（复用 `layout.json` 的 `list` 区）：按 `active_tab`（全部/地图/怪物/掉落/NPC）过滤数据集，渲染行；点击行 → `select_map_spot(mapid)` / `set_map`。
3. **搜索输入**：`search_text` 内容驱动实时过滤。
4. **子标题/详情卡**（`detail` 区）：选中结果填充立绘/名/级/HP/EXP（地图 tab 显示地图信息，怪物 tab 显示怪物详情）。
5. **空态/加载态文案**（资料 §2 已留空）。

---

## 4. 扩展二：左侧地图信息面板（map-info-panel）实现

### 4.1 可行性
- 资产已出（`UI/MapInfo/` 11 PNG + `layout.json`），决策 D1–D7 锁定。**无需 Photoshop**。
- C++ 仿 `UIMapMobPanel`/`UIMapSearchPanel`：`LocalPngTexture` + `Slider×2` + `Tab×4` + 5×5 网格。
- 前置：E0-2（`MapMobInfo` 扩 `exp`/`boss`）。掉落网格依赖 `MobDropData`（stub）→ 空态，接真表后填充。

### 4.2 实施步骤
1. 新建 `IO/UITypes/UIMapInfoPanel.{h,cpp}`。
2. `UIWorldMap` 增 `UIMapInfoPanel info_panel` 成员 + 编译期/配置 flag（如 `constexpr bool kUseMapInfoPanel`，默认 true）。
3. 吸附（D2 并存/互斥）：flag 开 → 左槽挂 `info_panel`，隐藏旧 `mob_panel`/`drop_panel`；`get_map_info_panel_pos() = position - 240`。`ensure_panels_on_screen` 左宽改用 240。
4. 上区怪物列表（3 行，含 HP/EXP/boss/属性/抗性占位）+ 右 `Slider`；下区 5×5 掉落格（32×32）+ Tab（全部/装备/消耗/其他）+ 掉率角标（`chance`→百分文案）+ 右 `Slider`；底 meso 白框（运行时数字）。
5. `select_map_spot` → `info_panel.set_map`；`on_mob_selected` → `info_panel.set_mob`。
6. 工程：`MapleStory.vcxproj` 加入新 cpp（确认是否 `GLOB_RECURSE` 自动收录，否则手动加 `<ClCompile>`）。

---

## 5. 数据层依赖与解决

| 数据 | 现状 | 解决 |
| --- | --- | --- |
| `MapMobInfo.exp` / `boss` | 缺字段 | E0-2 直接读 WZ，无需外部数据 ✅ |
| `MobDropData` 真表 | stub（`valid=false`，恒空） | 需数据源：(a) 由 s9 服务端导出掉落 JSON 放 `Data/MobDrops/`；(b) 客户端反推（不可靠，不建议）。**真表到位前面板显示空态，不编造** |
| 地图/NPC 索引 | 现按需查，无全局索引 | 世界地图打开时懒建内存索引（仅 WorldMap 可达图，量级可控） |

---

## 6. 验证

- **构建**：`tools/build_smoke.bat`（EXITCODE=0；先 `taskkill /F /IM MapleStory.exe` 避免 LNK1104）。
- **手工**：开世界地图 → 点光点 → 左信息面板(240)、右搜索面板(260) 出现 → 切 Tab/搜索/点结果 → 传送 → 关地图两侧消失。
- **截图对照原生**（1280×720，关键边/中心差 ≤2px）。
- **E0-1 验证**：缺件下世界地图右侧不再有漂浮无框搜索文字。

---

## 7. 风险

- 掉落真表缺失 → 搜索/信息面板的"掉落"内容空态（不编造）。
- 左侧 240 可能挤出屏幕 → `ensure_panels_on_screen` 已按各面板 WIDTH 计算，新增 240 走同一逻辑。
- 与旧 `mob`/`drop` 栏双显 → flag 互斥（D2）。
- **Photoshop MCP**：当前未注册；若需新资产（如 `ListNpc` 面板、精修贴图）需先启用 PS MCP（用户授权后再开）。

---

## 8. 分阶段交付（建议顺序）

| 阶段 | 内容 | 闭环判定 |
| --- | --- | --- |
| **Phase 0** | E0-1 旧搜索轨净化 + E0-2 `MapMobInfo` 扩 `exp`/`boss` + 确认 E0-3 guard | 构建 EXITCODE=0，右漂浮文字消失 |
| **Phase 1** | map-search-ui 搜索/过滤/结果列表 + 详情卡 | 切 Tab/搜索/点结果/传送可用，抓帧对照 |
| **Phase 2** | map-info-panel C++ 实现 + UIWorldMap 接线 | 左 240 面板出现，上怪物/下掉落格/底 meso，抓帧对照 |
| **Phase 3** | 掉落真表数据源接入（依赖 s9 导出）→ 填充搜索/信息掉落内容 | 掉落格与搜索掉落行有数据 |
| **Phase 4（可选）** | 原生 ListNpc/AutoFly/NaviRegister — 需先获素材，届时用 PS MCP 出图 | 原生子功能补齐 |

---

## 9. 待用户决策

1. **Phase 0 是否现在执行**？（建议执行，低风险且为后续前置）
2. **信息面板与旧 mob/drop 栏的并存策略**：默认互斥（flag 开 → 信息面板替代左怪右掉），还是三栏同显？（D2 文档倾向互斥/可配置）
3. **掉落真表数据源**：是否由 s9 导出 JSON（Phase 3 前置），还是本阶段只做空态 UI？
4. **是否启用 Photoshop MCP**：仅当需要扩展/精修原生贴图（如 ListNpc 面板）时才需。
