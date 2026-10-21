# 地图怪物/掉落 UI — 资源归档

> 中间世界地图：**客户端自带**（`UIWindow*.img/WorldMap`，边框为多片 `Border/0..7` 拼接）。  
> 左右侧栏：用 **`UIWindow.img/WorldMap/Border` 官方切片** 拼出的独立底板 PNG（浅色银蓝，不是黑色搜索栏）。

## 运行时

优先加载：

- `UI/MapMobDrops/mob_panel_backgrnd.png`
- `UI/MapMobDrops/drop_panel_backgrnd.png`

开发回退：`docs/features/map-mob-drops/resources/chrome/`

## chrome/（左右独立底板）

| 文件 | 尺寸 | 说明 |
| --- | --- | --- |
| [`mob_panel_backgrnd.png`](chrome/mob_panel_backgrnd.png) | 168×537 | 左栏「怪物」 |
| [`drop_panel_backgrnd.png`](chrome/drop_panel_backgrnd.png) | 210×537 | 右栏「掉落」 |

边框来源切片：`source/UIWindow_WorldMap/WorldMap_Border_0.png` … `_7.png`  
（对应 `UIWindow.img/WorldMap/Border/0..7`：角+上下左右边，无整框单图）

## preview/

| 文件 | 说明 |
| --- | --- |
| [`side_panels_only.png`](preview/side_panels_only.png) | **仅左右两栏**对照（不含中间地图） |

## 重新生成

```bat
python docs\features\map-mob-drops\resources\_gen_side_panels.py
```

需先有 `source/UIWindow_WorldMap/WorldMap_Border_*.png`（可用 `_tools/DumpMapMobUi` 从 `UI.wz` 导出）。
