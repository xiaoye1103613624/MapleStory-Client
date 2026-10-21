# WZ / NX 节点对照（地图怪物面板）

## 地图刷怪

- 根：`Map002/Map/Map{X}/{mapid:09d}.img`（经 `NxHelper::Map::get_map_node_name`）
- `life[]`：
  - `id` — 怪物/NPC id
  - `type` — `"m"` 怪物 / `"n"` NPC
  - `hide` — 非 0 则跳过

## 怪物属性

- `String/Mob.img/{id}/name`
- `Mob/{id:07d}.img/info/level`
- `Mob/{id:07d}.img/info/maxHP`（缺失则 UI 显示 `-`）
- `Mob/{id:07d}.img/info/link` — 外观链接到另一 mob id
- 站立图：`stand/0`（或任意子帧）；无则试 `fly`

## UI 色板（对齐 MapTooltip）

| 元素 | Font | Color |
| --- | --- | --- |
| 面板标题 | A12B | WHITE |
| 怪物名 | A12M | CHARTREUSE |
| 血量 | A11M | WHITE |
| 物品名 | A11M | WHITE |
| 物品 ID | A11M | SILVER |
| 掉率/数量 | A11M | YELLOW |

框：`UI/UIToolTip.img/Item/Frame2`（`MapleFrame`，锚点为底边中心）  
底图：`UIWindow*/WorldMap/WorldMapSearch/backgrnd` 拉伸填充  
标题角标：`ToolTip/WorldMap/Mob`

### 绘制注意

- `MapleFrame::draw(pos, w, h)` 的 `pos` 是**底边中心**，不是左上角；侧栏须传 `topLeft + (W/2, H)`。
- 怪物 `stand` 画布原点在脚底，列表绘制前需 `Texture::shift(origin)` 归零。
- 默认世界地图靠近左缘时左侧栏会出屏，选中地图后会自动右移窗口。
