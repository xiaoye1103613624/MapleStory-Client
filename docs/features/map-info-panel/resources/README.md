# map-info-panel resources

银蓝 Border chrome「地图信息」侧栏（240×537）。

## 生成

```bash
python docs/features/map-info-panel/resources/_gen_map_info_ui.py
```

依赖：Pillow；Border 切片来自  
`docs/features/map-mob-drops/resources/source/UIWindow_WorldMap/WorldMap_Border_{0..7}.png`。

标题：`draw_title` 仅在 Border 顶脊居中（光学上抬，不进白井）。  
下区：**5×5**、格 **32×32**（= `UIItemInventory` disabled 默认）；上区加高。

## 产出

| 路径 | 说明 |
| --- | --- |
| `chrome/info_panel_backgrnd.png` | 240×537 底板（标题烤进；上加高井 + 下 5×5 + scroll + meso） |
| `chrome/slot.png` | 单格 **32×32**（对齐背包） |
| `chrome/list_row_selected.png` | 怪物行选中底 |
| `Tab/enabled/{all,equip,consume,etc}.png` | Tab 激活（白抬） |
| `Tab/disabled/{all,equip,consume,etc}.png` | Tab 未激活（银凹） |
| `preview/info_panel_alone.png` | 单面板预览（含样例文案） |
| `preview/dock_context.png` | 左栏 + 世界地图占位 |
| `layout.json` | 命中区 / 列表 / 格子 / meso 几何 |

## 运行时同步

脚本同时写入：

- 仓库：`UI/MapInfo/`
- 客户端（若 `E:\MXD\BeiDou-Client-xy\UI` 存在）：`E:\MXD\BeiDou-Client-xy\UI\MapInfo\`

运行时文件为扁平 chrome 名（`info_panel_backgrnd.png` 等）+ `Tab/enabled|disabled/`。

## 决策摘要

见上级 [`../DESIGN.md`](../DESIGN.md)（D1=240，D2 并存，D4 meso 底栏 +「其他」含 CASH，D5 烤字，D6 背包格，D7 双滚动条）。
