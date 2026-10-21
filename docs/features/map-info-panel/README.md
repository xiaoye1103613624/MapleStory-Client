# 左侧「地图信息」面板

> 状态：**决策锁定 + UI 资产已出**；C++ 接线见 [`PLAN.md`](PLAN.md)（本轮未强制实现）  
> 产物：`docs/features/map-info-panel/` · 运行时 `UI/MapInfo/`  
> 日期：2026-09-30

## 决策（摘要）

| ID | 答案 |
| --- | --- |
| D1 | 宽 **240** × 高 537 |
| D2 | **并存**：保留 `UIMapMobPanel` / `UIMapDropPanel` |
| D3 | `boss`→BOSS，否则普通 |
| D4 | 底栏背包式**金币栏**；「其他」含 CASH |
| D5 | 标题/Tab **烤进 PNG** |
| D6 | 有数据则接；掉落=背包格 |
| D7 | **现出图**；上下**双滚动条** |

详情：[`DESIGN.md`](DESIGN.md) · 实现步骤：[`PLAN.md`](PLAN.md)

## 预览

| 文件 | 说明 |
| --- | --- |
| [`resources/preview/info_panel_alone.png`](resources/preview/info_panel_alone.png) | 单面板（叠 Tab + 样例） |
| [`resources/preview/dock_context.png`](resources/preview/dock_context.png) | 左栏 + Border 占位 |
| [`resources/chrome/info_panel_backgrnd.png`](resources/chrome/info_panel_backgrnd.png) | 底板 240×537 |
| [`resources/layout.json`](resources/layout.json) | 命中区 / 网格常量 |

## 再生成

```bat
python docs\features\map-info-panel\resources\_gen_map_info_ui.py
```

详见 [`resources/README.md`](resources/README.md)。

## 未实现（接线）

`UIMapInfoPanel`、WorldMap 吸附、`MapMobInfo` 扩 exp/boss、`MobDropData` 真表 — 见 `PLAN.md` Phase B–C。
