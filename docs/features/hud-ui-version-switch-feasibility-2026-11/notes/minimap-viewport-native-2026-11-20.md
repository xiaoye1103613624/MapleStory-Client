# 小地图视口对照原生收口 — 2026-11-20

## IDA 常量（MapleStoryT `CUIMiniMap`）

| 来源 | 常量 / 行为 |
|---|---|
| `SetLayer@0x14393D0` 非 BigShow | `W≥280 → pane_w=210`；`H≥150 → pane_h=112` |
| `ToggleMap@0x1443990` | canvas `≤280×150` 或 `>screen_w-50` / `>screen_h-150` → 强制 `m_bBigShow=0` |
| `CalculateScr@0x14316B0` | 玩家居中，`pane<<mag` 世界夹边后 `>>mag` → canvas scr |
| `IsPtInViewRange@0x1431DD0` | `0 ≤ rel ≤ pane`（含上界） |

旧半成品自定 `MAX_PANE=200×150` 已改为 **210×112** + 触发阈值 280×150。

## 本端落地

- [`IO/UITypes/UIMiniMap.h`](../../../IO/UITypes/UIMiniMap.h)：`MAX_PANE_W/H=210/112`，`BIG_TOGGLE_*=280/150`，`apply_big_map_policy`。
- [`IO/UITypes/UIMiniMap.cpp`](../../../IO/UITypes/UIMiniMap.cpp)：`update_canvas` 按 BigShow 算 pane；`BT_SMALL/BIG` 重建 canvas；`refresh_map_viewport` 跟玩家；`canvas_pos_in_view` 含上界。

## ListNpc 旁支（CodeBuddy `4e377f94…` 交叉）

orange-wz（GMS 128B key）核验 live `BeiDou-Client-xy/Data/UI/UIWindow.img/MiniMap` 子节点仅：

`title` / `MinMap` / `MaxMap` / `Min` / `BtMap`

**无** `ListNpc`、`BtNpc`、`iconNpc`。故「小地图 NPC 列表不显示」真因是**素材缺失**（非视口裁剪误伤）。

处置：构造期 `diag_once` 记阻塞；有 `BtNpc` 或 `ListNpc` 时再接线 `BT_NPC`。素材迁移与远距交互已于 2026-11-20 闭环，见 [minimap-remote-interact-2026-11-20.md](minimap-remote-interact-2026-11-20.md)。

## 验收点

| 场景 | 期望 |
|---|---|
| 竖长图（如 Ellinia canvas ~179×322） | 内容高钳 112，移动角色 Y 向滚动 |
| 宽大图（canvas W≥280） | 内容宽钳 210，移动角色 X 向滚动 |
| 小图（≤280×150） | 整图显示，无滚动 |
| 视口外 portal/NPC/他角 | 不溢出窗体 |
| ListNpc | 素材到位前不可开；日志有一次 diag |
