# UI manifest — MapSearch v2 组件组

- 样式来源：原生（与 MapInfo v2 同一套银蓝 Border 语言；v1 底板标题烤字 + 详情卡残留，本版重绘）
- scale：1x
- 生成工具：Photoshop MCP（`@alisaitteke/photoshop-mcp` v1.7.37 + Photoshop 2024，2026-11-05）
- 采样：同 `map-info-panel/ui/manifest.md`（WorldMap Border 实测色板）
- 客户端：`E:\MXD\BeiDou-Client-xy`
- 目标节点路径：插件运行时 PNG（`UI/MapSearch/`，非 WZ 入库）

## 组件清单

| 组件 | 文件 | 尺寸 | 状态 | 运行时节点 |
| --- | --- | --- | --- | --- |
| 底板 | `search_panel_backgrnd/v2/search_panel_backgrnd.png` (+`.psd`) | 260×537 | static | `UI/MapSearch/search_panel_backgrnd.png` |

## v2 相对 v1 的变化（对应 todo 5）

1. **标题区留白**：脊带（7,6,246,24）不再烤「地图搜索」四字；运行时 `title_text`
   接 `mapsearch.title` Locale 键绘制（此前是 ctor 写死中文字面量 = 用户指出的
   「代码上写死的」问题，现已入绑定表，切语言自换）。
2. **详情卡区域删除**：底板不再为 detail 卡（112..228 一带）画框线；代码侧
   `detail_frame/detail_name/detail_street` 死成员一并移除。
3. 列表区扩净：18,250 起 223×227 无中隔框线（v1 的中部横带是详情卡残留）。
4. 费用槽/传送钮位保留（18,484 槽 + 177,484 凹陷衬底；费用文案 `mapsearch.cost`
   已绑 100000，BtTeleport 四态 PNG 沿用 v1，未重绘）。

## 生成说明

- Photoshop 分层 PSD（`frame/*`、`title_ridge`、`well/*`、`search/slot`、`tab/shelf`、
  `subtitle/slot`、`scroll/list`、`cost/slot`、`teleport/recess` 命名层）。
- 页签/按钮沿用 v1 资源（Tab 五类二态、BtBack/BtTeleport 四态），本轮未重绘。

## 验收

- 尺寸 260×537 与 `UIMapSearchPanel::WIDTH/HEIGHT` 及 layout.json 一致 ✔
- 硬边像素、无禁区观感 ✔
- PSD 分层保留 ✔
- 运行时同步：`E:\MXD\BeiDou-Client-xy\UI\MapSearch\search_panel_backgrnd.png` ✔
