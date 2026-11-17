# 原生选中高亮取证（orange-wz MCP，2026-11-17）

## 目的
核查「目录 / 界面」气球列表的**键盘选中高亮**是否有 v83 原生素材，替换此前自凑的
`ColorBox(GRAYOLIVE, 0.5f)` 半透明块（用户反馈「发虚」）。

## 取证方式
orange-wz MCP（serverInfo `orange-wz-http-mcp v1.157.49`，端点 `http://127.0.0.1:10012/mcp`）。
会话工具集未注入 `mcp__orange_wz__*` 前缀，走原始 JSON-RPC over HTTP 握手。
- key：`{"name":"GMS","userKeyBase64":<tools/_gms_key_128.b64 128B>,"ivBase64":"TSPHKw=="}`
- 文件：`E:\MXD\BeiDou-Client-xy\Data\UI\UIWindow.img`
- 工具：`load_files`(exclusive:false) → `query_nodes`(op=tree/detail, includePng=true) → `unload_all`

## 节点树证据（get_tree）
`ShortCut/BtItem`、`ShortCut/BtEquip`…、`GameMenu/BtChannel`、`GameMenu/BtSkin`…
每个按钮节点结构一致，只有 4 个子态：

```
BtItem/  (LIST_PROPERTY)
  normal/0    CANVAS 81x25  ARGB8888  origin(0,0)
  pressed/0   CANVAS 81x25  ARGB8888
  disabled/0  CANVAS 81x25  ARGB8888
  mouseOver/0 CANVAS 81x25  ARGB8888
```

**结论 1：UIWindow.img 里没有独立的 selected / focus / highlight 素材节点。** 原生列表
选中态的视觉只能由按钮自身的 `mouseOver` 贴图承载。

## 三态像素实测（导出 PNG 求不透明像素均色）
| 按钮 | 态 | 均色 RGBA（不透明像素） |
| --- | --- | --- |
| BtItem | normal | (64, 171, 224, 255) |
| BtItem | mouseOver | (28, 172, 240, 255) — 更暗的 R、更亮的 B，即亮蓝高亮 |
| BtItem | pressed | (46, 152, 200, 255) |
| BtChannel | normal | (75, 175, 222, 255) |
| BtChannel | mouseOver | (54, 175, 234, 255) |
| BtChannel | pressed | (60, 153, 198, 255) |

**结论 2：`mouseOver` 是比 `normal` 更「亮蓝」的原生高亮态**（R 降、B 升）。这才是
v83 选中项的原生观感。旧版 `ColorBox(GRAYOLIVE,0.5f)` 把灰绿半透明盖在深蓝钮上，
混出灰蓝（真机实测 B-R 从 140 掉到 62、R 从 79 升到 125），所以发虚、且非原生。

## 源码交叉验证
- `IO/Components/MapleButton.cpp:64-66`：`textures[PRESSED]=src["pressed"]["0"]`、
  `textures[MOUSEOVER]=src["mouseOver"]["0"]`、`textures[DISABLED]=...` —— 四态贴图映射与 MCP 树一致。
- `IO/Components/MapleButton.cpp:76-83`：`draw()` 按当前 `state` 取 `textures[state]` 画。
- `IO/Components/MapleButton.cpp:37-43`：`set_state()` 直接改 `state`（仅 IDENTITY 忽略）。
- `IO/Components/MapleButton.h:41`：`send_cursor()` 是空实现 → 按钮 state 由外部
  `UIElement::send_cursor` 管。故键盘高亮若直接 `set_state(MOUSEOVER)` 会与鼠标 hover
  抢同一 state 字段（键鼠同时动会两颗同时亮）→ **必须画完立即恢复原 state**。

## 落地方案（已在 UIMiscShells.cpp 两处 draw 实施）
临时把 `selected` 对应按钮切到 `MOUSEOVER` → `btn->draw(position)` 单独重画原生亮蓝贴图
→ `btn->set_state(saved)` 立即恢复。画在 `UIElement::draw(inter)` 之后以盖住该颗 NORMAL 钮。
- `UIShortCut::draw` ~4069、`UIGameMenu::draw` ~6537
- `MENU_IDS[selected]` 与 ctor 按钮 id 一一对应（ShortCut 0-7 / GameMenu 0-3）。
- 零公共 API 改动、零 WZ 改写（纯读原生素材）、不污染鼠标交互状态。
