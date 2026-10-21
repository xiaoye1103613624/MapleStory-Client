# 地图信息侧栏 — 实现计划

> 状态：决策已锁定（见 `DESIGN.md`）；本文件为一次性完整实现计划  
> 优先交付：**资源 PNG + layout + 本文档**；C++ 接线次优先（有余力再做，且必须并存旧面板）  
> 日期：2026-09-30（含反馈修订：标题 Y、32px 格、5×5、上区加高、掉率角标）

---

## 0. 已确认决策摘要

| ID | 锁定 |
| --- | --- |
| D1 | 宽 **240**，高默认 **537** |
| D2 | **并存**：保留 `UIMapMobPanel`/`UIMapDropPanel` 代码；feature 开时左槽改挂 `UIMapInfoPanel`，旧左右栏隐藏 |
| D3 | `boss`→BOSS，否则普通 |
| D4 | 底栏背包式 **金币栏**；「其他」Tab **含 CASH** |
| D5 | 标题/Tab **烤进 PNG**（标题仅顶 chrome 脊，不进白井） |
| D6 | 有数据接线；掉落 **5×5**、格边 **32**（=背包 icon）；掉率角标绑 `chance` |
| D7 | **现出图**；上下 **双滚动条**；上区加高（属性/抗性占位） |

---

## 1. 阶段划分

| 阶段 | 内容 | 闭环/预留 |
| --- | --- | --- |
| **A. 资源** | `_gen_map_info_ui.py` → chrome/Tab/preview/`layout.json` → 拷贝 `UI/MapInfo/` | **本轮闭环** |
| **B. 文档** | `DESIGN.md` 锁定版 + 本 `PLAN.md` + `README.md` | **本轮闭环** |
| **C. 数据扩展** | `MapMobInfo` +exp/+boss；`get_mobs_on_map` 读 WZ | 实现期 |
| **D. UI 类** | 新建 `UIMapInfoPanel`；LocalPng + Slider×2 + Tab×4 + 网格 | 实现期 |
| **E. WorldMap 接线** | flag、吸附、选中光点/选怪、隐藏旧栏 | 实现期 |
| **F. 掉落/金币** | `MobDropData` 过滤 + meso 绑定；stub 时空态 | 有数据则接；stub 可空 |
| **G. 验证构建** | 手工点世界地图；MSBuild 相关 TU | 实现期门禁 |

本轮（当前会话）目标：**A + B** 完成；C–G 按余力启动，不得破坏旧面板代码路径。

---

## 2. 任务分解（有序）

### A1 — 生成脚本与资产

- [x] 脚本：`docs/features/map-info-panel/resources/_gen_map_info_ui.py`
- [x] 产出：
  - `chrome/info_panel_backgrnd.png`（240×537，标题顶脊上抬）
  - `chrome/list_row_selected.png`
  - `chrome/slot.png`（32×32）
  - `Tab/enabled|disabled/{all,equip,consume,etc}.png`
  - `preview/info_panel_alone.png`、`preview/dock_context.png`
  - `layout.json`（`cols=5` `rows=5` `slot_size=32`；上区加高）
  - 同步 `UI/MapInfo/` + 客户端 `UI/MapInfo/`（若存在）
- Border 源：`docs/features/map-mob-drops/resources/source/UIWindow_WorldMap/`
- 标题：`draw_title` 墨水钳 `[3, top_h-12]` + 光学上抬，禁止进白井
- 反馈修订（同轮）：5×5、上区优先高度、preview 掉率角标占位

### A2 — README / 资源说明

- [x] `docs/features/map-info-panel/README.md`
- [x] `resources/README.md`（指向脚本）

### B — 设计锁定

- [x] `DESIGN.md` 写入 D1–D7 与并存策略

### C — 数据（实现期）

1. `Util/Misc.h`：`MapMobInfo` 增加 `int32_t exp`、`bool boss`（或 `is_boss`）。  
2. `Util/Misc.cpp`：`get_mobs_on_map` 读 `Mob.img/{id}/info/exp`、`info/boss`。  
3. 类型文案仅 UI 层：`boss ? "BOSS" : "普通"`。  
4. 掉落：继续 `MobDropData::get_drops`；Tab 用 `InventoryType::by_item_id`。  
5. 金币栏默认：选中怪掉落中的 meso（若 schema 有）；否则空/`0`。备选玩家 `get_meso()`。

### D — `UIMapInfoPanel`（实现期）

建议文件：

| 文件 | 职责 |
| --- | --- |
| `IO/UITypes/UIMapInfoPanel.h` | 宽高常量、API：`set_map` / `set_mob` / `clear` / `draw` / `update` / 鼠标 |
| `IO/UITypes/UIMapInfoPanel.cpp` | 加载 `UI/MapInfo/`；上列表；下网格；双 Slider；Tab；meso 字 |

要点：

- 非独立 `UIElement`（与 mob/drop/search 一致，由 `UIWorldMap` 拥有）。  
- Tab：`LocalTwoSpriteButton`（同 `UIMapSearchPanel`）。  
- 上区：`VISIBLE_ROWS=3`，行高对齐 `layout.json`（含属性/抗性占位）。  
- 下区：**5×5**、`slot_size=32`；Icon + **掉率角标**（`chance`）；滚动按行步进。  
- 金币掉落：见 DESIGN §6.2（stub 无区间真值；勿编造）。  
- 编码：烤字 PNG；运行时空态/数字/掉率用现有 Text 字体链路。  
- 注释 + 分级日志：选图、选怪、Tab、空态。

### E — `UIWorldMap` 接线（实现期）

1. 增加成员 `UIMapInfoPanel info_panel`（或等价）。  
2. Feature flag（编译期宏或静态配置，名称自定，如 `MapleStory_Config` / 局部 `constexpr bool kUseMapInfoPanel = true`）。  
3. `get_mob_panel_pos` 旁新增 `get_map_info_panel_pos()`：`position - UIMapInfoPanel::WIDTH`。  
4. `select_map_spot`：flag 开 → `info_panel.set_map`；隐藏 mob/drop；关 → 现逻辑。  
5. `sync_side_panels` / `ensure_panels_on_screen`：左宽按 flag 用 240 或 168；右不计 Drop 宽当 flag 开。  
6. 选怪回调：info 上列表点击 → 下网格 `set_mob`（旧 `drop_panel.set_mob` 仅 flag 关时）。

**禁止**：删除 `UIMapMobPanel.cpp/.h`、`UIMapDropPanel.cpp/.h` 或卸掉其工程项。

### F — 验证清单（实现期）

手工：

1. 开世界地图 → 点光点 → 左栏标题「地图信息」、宽约 240。  
2. 怪物 ≥4 时上区可滚；仅约 3 行可见。  
3. 点怪 → 下区变化；切 Tab 过滤。  
4. 掉落多时下区可滚；格子一格一图标。  
5. 底栏金币槽存在。  
6. flag 关：恢复左怪右掉。  
7. Escape/关地图：侧栏消失。

构建（改 C++ 后）：

```bat
MSBuild MapleStory.vcxproj /p:Configuration=Release /p:Platform=x64
```

相关 TU：`UIMapInfoPanel`、`UIWorldMap`、`Misc`（若扩 MapMobInfo）。  
已知无关阻塞：`Util/Patch/Sha256.cpp` C4996（勿借本功能改）。

### G — 产物与文档闭环

- 资源保留在 `docs/features/map-info-panel/`。  
- 实现完成后补 feature README「最终逻辑」小节（skill 模板）。  
- 原子提交：资源/文档与代码可分 commit。

---

## 3. 涉及文件预估

| 路径 | 动作 |
| --- | --- |
| `docs/features/map-info-panel/DESIGN.md` | 更新（已） |
| `docs/features/map-info-panel/PLAN.md` | 新建（本文件） |
| `docs/features/map-info-panel/README.md` | 新建 |
| `docs/features/map-info-panel/resources/_gen_map_info_ui.py` | 新建 |
| `docs/features/map-info-panel/resources/**` | 生成物 |
| `UI/MapInfo/**` | 运行时拷贝 |
| `IO/UITypes/UIMapInfoPanel.{h,cpp}` | 实现期新建 |
| `IO/UITypes/UIWorldMap.{h,cpp}` | 实现期改吸附/flag |
| `Util/Misc.{h,cpp}` | 实现期扩 MapMobInfo |
| `MapleStory.vcxproj` | 实现期加入新 cpp |

---

## 4. 风险与回滚

| 风险 | 缓解 |
| --- | --- |
| 左宽 240 挤出屏幕 | `ensure_panels_on_screen` 用新 WIDTH |
| 与旧栏双显占屏 | flag 互斥显示，禁止同时 visible |
| MobDropData stub | 空态文案；网格可空 |
| 无精英数据 | D3 已砍精英 |
| 出图与 hitbox 漂移 | 以 `layout.json` 为单一来源同步 C++ 常量 |

回滚：关 flag → 旧 mob/drop 路径；资源可留档不影响。

---

## 5. 本轮完成定义（DoD）

1. `DESIGN.md` 决策表与 D4/D5/D7 澄清完整。  
2. 本 `PLAN.md` 可执行。  
3. 生成脚本可重复运行；PNG + `layout.json` + preview + `UI/MapInfo/` 就绪。  
4. README 标明预览路径与再生命令。  
5. **不强制**本轮完成 C++；若动代码须保持旧面板可编译可切换。
