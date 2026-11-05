# UI manifest — MapInfo v2 组件组

- 样式来源：原生（银蓝 WorldMap Border 语言，v1 的 PIL 占位图按用户要求重绘为 PS 出图）
- scale：1x
- 生成工具：Photoshop MCP（`@alisaitteke/photoshop-mcp` v1.7.37 + Photoshop 2024，2026-11-05）
- 采样：`docs/features/map-mob-drops/resources/source/UIWindow_WorldMap/WorldMap_Border_{0..7}.png` 实测取色（#557799 / #99BBCC / #BBCCDD / #F8FAFC）
- 客户端：`E:\MXD\BeiDou-Client-xy`（见 `notes/client-path.md` 口径）
- 目标节点路径：插件运行时 PNG（`UI/MapInfo/`，非 WZ 入库；LocalPngTexture 直读）

## 组件清单

| 组件 | 文件 | 尺寸 | 状态 | 目标 WZ/运行时节点 |
| --- | --- | --- | --- | --- |
| 底板 | `info_panel_backgrnd/v2/info_panel_backgrnd.png` (+`.psd`) | 240×537 | static | `UI/MapInfo/info_panel_backgrnd.png` |
| 怪物行底纹 | `mob_row_bg/v2/mob_row_bg_a.png` / `_b.png` (+`.psd`) | 189×78 | static（奇偶交替，图标灰底 40×40 烤在行图内） | `UI/MapInfo/mob_row_bg_a|b.png` |
| 行选中 | `list_row_selected/v2/list_row_selected.png` (+`.psd`) | 189×78 | selected | `UI/MapInfo/list_row_selected.png` |
| 掉落格 | `slot/v2/slot.png` (+`.psd`) | 32×32 | static（对齐 UIItemInventory disabled 默认） | `UI/MapInfo/slot.png` |
| 页签 | `tab/v2/{all,equip,consume,etc}_{disabled,enabled}.png` (+`.psd`) | 50/53×18 | **二态**（TwoSpriteButton 契约：NORMAL=disabled，MOUSEOVER/PRESSED=enabled） | `UI/MapInfo/Tab/disabled|enabled/{key}.png` |

## 生成说明

- 底板/行/格/页签均由 Photoshop MCP 分层绘制（`frame/*`、`well/*`、`scroll/*`、`grid/recess`、`tab/*` 命名层，PSD 保留可编辑结构）。
- **底板标题不烤字**：PSD 保留 `title_text` SimSun 文字层（设计源可见），PNG 导出时隐藏该层；
  运行时由 `UIMapInfoPanel::title_text` 绘制并接 `mapinfo.title` Locale 键 —— 与
  「reload 广播、窗自取」的 i18n 热重载通道一致（v1 把「地图信息」烤死在 PNG 里，切语言不换字，本版修正）。
- 页签文字（全部/装备/消耗/其他）烤进 PNG：背包同款先例（ItemTab 页签烤字），文案不随 ui.language 变。
- 掉落格数量字不画资源：运行时 `Charset(Basic.img/ItemNo)` 位图字库绘制（8×11，背包/商店同源）。
- 状态推断：页签=二态切换（§9.0 TwoSpriteButton）；底板/行底/格=静态件；选中态=list_row_selected 覆盖绘制。

## 验收

- §13.1 尺寸与 layout.json 常量一致（UPPER 18,42,189,237 / TAB 18,68,118,168 y285 / GRID 30,312 5×5×34 / MESO 18,489）✔
- 硬边像素、无渐变/柔光/圆角胶囊 ✔；页签激活与未激活并排可辨 ✔
- PSD 分层保留（组件层命名 `frame/`、`well/` 等前缀）✔
- 运行时同步：`E:\MXD\BeiDou-Client-xy\UI\MapInfo\`（PNG 全套 + Tab 两态目录）✔
