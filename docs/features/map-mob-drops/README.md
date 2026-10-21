# 地图怪物与掉落展示 UI

> 状态：部分预留（左侧怪物闭环；右侧掉落 UI 搭壳，数据装载预留）  
> 适用客户端：MapleStory-Client（Journey 系 / 本地资源运行时）  
> 产物目录：`docs/features/map-mob-drops/`  
> 日期：2026-09-28

## 1. 背景与目标

打开世界地图后，点击地图光点：

- **左侧吸附**：该地图怪物列表（站立图、等级、名称、最大血量）
- **右侧吸附**：选中怪物的掉落列表（图标、物品 ID、名称、掉率、数量区间）

本版左侧闭环；右侧 UI 与 `MobDropData` 接口已就绪，**无真实掉落表**（显式空态，不编造数据）。

明确不做：服务端封包拉表、改 MapTooltip 悬停逻辑、改搜索栏行为以外的世界地图玩法。

## 2. 影响范围

| 层级 | 路径/模块 | 变更类型 |
| --- | --- | --- |
| 客户端数据 | `Util/Misc`（`get_mobs_on_map`）、`Data/MobDropData` | 新增/扩展 |
| 客户端 UI | `UIWorldMap`、`UIMapMobPanel`、`UIMapDropPanel` | 新增/改造 |
| 工程 | `MapleStory.vcxproj` / `.filters`；CMake `GLOB_RECURSE` 自动收录 | 接入 |
| WZ | 只读 `Map` life / `Mob` info / `String/Mob` / `stand` | 无写补丁 |
| 服务端 | — | 无 |

## 3. 最终实现逻辑

### 3.1 主流程

1. 打开 `UIWorldMap`（热键 / 小地图按钮）。
2. **点击**地图光点 → `selected_map_id = map_ids[0]` → `UIMapMobPanel::set_map`；右侧显示「Select a monster」。
3. **点击**左侧怪物行 → `UIMapDropPanel::set_mob` → `MobDropData::get_drops`（本版恒为空）→ 「No drop data」。
4. 搜索栏开关时右侧面板起点 = 地图宽 + 搜索宽（5A）。
5. 关世界地图 / Escape 回退父图 / 关搜索前的关闭路径会 `clear_side_panels`。

### 3.2 数据

| API | 语义 |
| --- | --- |
| `NxHelper::Map::get_life_on_map` | 原 MapTooltip 用；保留 |
| `NxHelper::Map::get_mobs_on_map` | 仅 type=`m`、去重、按 level/id 排序；含 `maxHP` |
| `MobDropData::get_drops(mob_id)` | 按 `item_id` 升序；本版空且 `valid=false` |

站立图：UI 读 `Mob/{id}.img` 的 `stand`（或 `link` / `fly`）首帧，缩放到约 40px。

掉落字段约定见 [`notes/drop-json-schema.md`](notes/drop-json-schema.md)。

### 3.3 模块

- `UIMapMobPanel` / `UIMapDropPanel`：**非**独立 `UIElement`，由 `UIWorldMap` 持有，保证拖拽与开关同步。
- 样式：暗底 + `UIToolTip/Item/Frame2`；怪物名 `A12M`/`CHARTREUSE`（对齐 MapTooltip）；掉落行对齐商店图标+文字。
- 悬停掉落：`UI::show_item(Tooltip::Parent::WORLDMAP, item_id)`。

## 4. 资源与同步

- 无新增 WZ/IMG 写入。
- 后续掉落 JSON 建议放客户端可读目录（如 `Data/MobDrops/` 或补丁清单路径），与服务端 drop 表同源导出。

## 5. 编码与 i18n

- 源码 UTF-8；面板占位文案暂为英文（避免 narrow UI 编码风险）。后续可走项目 i18n。

## 6. 产物清单

| 类型 | 路径 | 说明 |
| --- | --- | --- |
| 文档 | `README.md` | 本文件 |
| 笔记 | `notes/wz-paths.md` | WZ 节点 |
| 预留 | `notes/drop-json-schema.md` | 掉落 JSON 草案 |
| 资源 | [`resources/`](resources/README.md) | 两侧完整底板 PNG（`chrome/`）+ 运行时 `UI/MapMobDrops/` |

## 7. 验证记录

- 构建：`MapleStory.vcxproj` Release|x64（见 builds 说明）
- 手工：开世界地图 → 点光点 → 左列表有图/Lv/HP → 点怪 → 右空态 → 开搜索右面板外扩 → 关闭两侧消失

## 8. 预留、风险与回滚

**预留**

- `MobDropData` 真实装载（JSON / 服务端导出）
- 掉率展示单位与服务端万分比对齐后改格式化

**风险**

- 多 `mapNo` 的 spot 取 `[0]`（与 tooltip 一致）
- 缺 `maxHP` / `stand`：显示 `-` / 空图标

**回滚**

- 移除两侧面板调用与三个新编译单元即可；`get_life_on_map` 未破坏。
