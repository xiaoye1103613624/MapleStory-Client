# W11 小地图 / 大地图 版式还原（RC13）取证与算式

取证方式：`python tools/nx_inspect.py list "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx" <depth> <path>`（只读）。
origin 用 PKG4 内联向量（已可信），实测 MinMap/MaxMap/Min/Border 各件 origin 均为 `(0,0)`，
故 origin 补偿为恒等；代码仍按 NameTag 的 `draw_slice` 写法通用地加回 origin 以防后续包出现非零 origin。
PNG 见 `pngs/`，原始转储见 `dump/`。

---

## A. 小地图（收缩 / 展开两套）

### A.1 MinMap（NORMAL 展开框）九切片  节点根：`UIWindow.img/MiniMap/MinMap`

| 件 | 节点 | WxH | origin | 位置(窗口相对左上) | 策略 |
| --- | --- | --- | --- | --- | --- |
| nw | `MinMap/nw` | 60x29 | (0,0) | (0,0) | 原生尺寸，不拉伸（左上角，含 60px 装饰帽压在顶带） |
| n  | `MinMap/n`  | 1x29 | (0,0) | (60,0) | 单轴：横向拉伸到 `W-60-6` |
| ne | `MinMap/ne` | 6x29 | (0,0) | (W-6,0) | 原生尺寸（右上角） |
| w  | `MinMap/w`  | 6x1  | (0,0) | (0,29) | 单轴：纵向拉伸到 `H-29-14` |
| c  | `MinMap/c`  | 1x1  | (0,0) | (6,29) | 中心拉伸填充内容区 `(W-12, H-43)`（半透明底衬） |
| e  | `MinMap/e`  | 6x1  | (0,0) | (W-6,29) | 纵向拉伸 `H-43` |
| sw | `MinMap/sw` | 6x14 | (0,0) | (0,H-14) | 原生尺寸（左下角） |
| s  | `MinMap/s`  | 1x14 | (0,0) | (6,H-14) | 横向拉伸 `W-12` |
| se | `MinMap/se` | 6x14 | (0,0) | (W-6,H-14) | 原生尺寸（右下角） |

关键尺寸：真实左/右边框 = 6（`w`/`e`/`ne`/`sw`/`se` 的宽），顶带 = 29（nw/ne/n 的高），底带 = 14（sw/se/s 的高）。
`nw` 宽 60 是装饰帽（v83 小地图左上角的圆角块），只占顶带 y∈[0,29)，不参与主体左边框宽度。

### A.2 MaxMap（城镇展开框）九切片  根：`UIWindow.img/MiniMap/MaxMap`

同 MinMap 结构，仅顶带/底带变高：

| 件 | WxH | 顶/底带 |
| --- | --- | --- |
| nw | 60x72 | 顶带 72 |
| ne | 6x72 | |
| n  | 1x72 | |
| sw | 6x15 | 底带 15 |
| se | 6x15 | |
| s  | 1x15 | |
| w/e | 6x1 | 左右边仍 6 |
| c  | 1x1 | 内容区拉伸填充 |

### A.3 Min（收缩条）三切片  根：`UIWindow.img/MiniMap/Min`

| 件 | WxH | 位置 | 策略 |
| --- | --- | --- | --- |
| w | 8x20 | (0,0) | 原生左帽 |
| c | 1x20 | (8,0) | 横向拉伸 `W_min-12` |
| e | 4x20 | (8+W_min-12,0) | 原生右帽 |

高固定 20。

### A.4 框宽算式

```
body_w = max( max(106, mark_text_width-12), map_dims.x )        // mark_text_width = 48 + max(region,town) + 10
body_h = max( 40, map_dims.y )
W        = body_w + 6 + 6        // 左右各 6
H_normal = body_h + 29 + 14      // MinMap 顶/底带
H_max    = body_h + 72 + 15      // MaxMap 顶/底带
W_min    = combined_text_width + bt_map_width + 18   // 收缩条，含名字+1颗BtMap
```
地图画布居中：`map_draw_origin_x = 6 + (body_w - map_dims.x)/2`，`map_draw_origin_y = 29`。
MAX 由 draw 端叠加 `MAX_ADJ` 把画布/标记下移到 `29+MAX_ADJ = 72`（即 MaxMap 顶带高度），
所以 `MAX_ADJ` 由旧值 40 改为 **43**（72-29）。

### A.5 BtMap 归位

根：`UIWindow.img/MiniMap/BtMap`（`normal/mouseOver/pressed/disabled` 各一帧 `36x12`，origin(0,0)，见 `pngs/btmap_normal.png`）。
`BtMin/BtMax/BtSmall/BtBig/BtNpc` 本包不存在 → 不再实例化（`buttons` 里只有 `BT_MAP`；`toggle_buttons`/`update_buttons` 对缺失件 null-safe，宽度按 0 计）。

- 收缩态：`bt_min_x = combined_text_width + 11`，BtMap anchor=(bt_min_x,4)，即紧跟地图名右侧。
- 展开态：`middle_right_x = W`，BtMap anchor = `W - bt_map_width`，y=4，落在 29px（MinMap）/72px（MaxMap）顶带右端、框内。
- 点击语义不变（`button_pressed(BT_MAP)` 仍走 MIN→NORMAL→MAX 循环）。

### A.6 地图名 / 头像 / 观测点

> **2026-10-31 验证轮更正**：本小节原判定「NORMAL 在 29px 顶带画地图名」是**错的**，已按抓帧改代码。

- 地图名：`update_text()` 从 `NxHelper::Map::get_map_info_by_id(mapid)` 取 `full_name/name/street_name`，
  分别灌进 `combined_text / region_text / town_text`。
  - **MIN**：浅灰底 (221,221,221) ⇒ `combined_text` 用 `A12M + Color::Name::BLACK`。
  - **MAX**：顶带底色实测 **(153,187,204)**（= `MaxMap/n` 中心色），两行字是**白字 + 1px 黑描边**
    （参考帧 `evidence/refcrops/minimap.png` 逐行量化：`min(rgb)>240` 的像素落在 y=31..42 与 y=47..58，
    行距 16px）⇒ `region_text`/`town_text` 类型从 `Text` 改为 **`OutlinedText(A12B, LEFT, WHITE, BLACK)`**。
    注意：中途一度把三处一律改 BLACK 属过度修正，浅蓝底上黑字可读但不是原生观感。
  - **NORMAL 不再画地图名**。29px 顶带左端的「小地图」是 `MinMap/nw`（60×29）**烤死的艺术字**，
    再叠 `Text` 等于把两行 A12B（间距只有 11px）压在艺术字上直接糊。参考端 NORMAL 顶带只有标题
    与缩放/大地图钮。
  - **MAX 两行顺序**：第一行街道名 `town_text(52,20)`、第二行地图名 `region_text(52,36)`
    （参考端：彩虹岛 / 小蘑菇）。原实现反了，已换。
- 头像/坐标点：小地图无独立头像件（MinMap/MaxMap 下无 Portrait 类节点）。玩家/他人/NPC/传送门点位由
  `MapHelper.img/minimap` 的 `user/another/npc/portal` 经 `draw_movable_markers`+`update_static_markers` 绘制（沿用既有机制，已正确按 origin 半宽定位）。
- `MapHelper["mark"][mapMark]` 的地图图标（MAX）放顶带 (8,8)。
- **顶带不透明度**：本窗属于「把主循环的帧插值系数当不透明度转发」的 14 个文件之一（9 处），
  抓帧实测 `Min/c` 的 (221,221,221) 被画成 (103,103,103)。根因与修法见
  [notes/open-issues.md §A1](../../notes/open-issues.md)。

### A.7 simpleMode 死取用清理

`simpleMode` 旧分支取 `MiniMap["Window"]["Min/Normal/Max"]`、`MiniMap["Window"]["Max"]["MiddleCenter"]` —— 本包 `UIWindow.img/MiniMap` 无 `Window` 子树（inventory 证实），必然空。
现改为：无论 simpleMode 与否都用 `MinMap/MaxMap/Min`，并在进入时 `diag_once("minimap simplemode", ...)`。

### A.8 2048px canvas 守卫（保留，未放宽）

`update()` :170-215 的 `kMaxCanvasEdge=2048` 是防超大 tutorial 画布（如 5944x1311）上传 atlas 导致主线程冻结黑屏。
它拦的是「地图 canvas 位图」本身，与框体拼法是两回事；本批只改拼框，未触碰该阈值。
当某图 canvas>2048 时仍降级为 MIN（`has_map=false`）——属预期防御行为，不改。

---

## B. 大地图（WorldMap）

### B.1 Border 件 → 边框位置 映射

根：`UIWindow.img/WorldMap/Border`，8 件。形状经 `pngs/border_*.png` + `dump/border_shapes.txt`（逐行 alpha 扫描）确认：

| 件 | WxH | 形状（朝/边） | 判定为 |
| --- | --- | --- | --- |
| 0 | 60x33 | 满块，仅左上角有圆角缺口 | 左上角 |
| 1 | 1x33 | 满列（顶带高 33） | 上边（横拉伸） |
| 2 | 7x32 | 满，右上角斜切 | 右上角 |
| 3 | 7x1  | 满行（宽 7） | 左边（纵拉伸） |
| 4 | 7x1  | 满行（宽 7） | 右边（纵拉伸） |
| 5 | 7x18 | 满，右下角斜切（左列 15/18） | 左下角 |
| 6 | 1x18 | 满列（底带高 18） | 下边（横拉伸） |
| 7 | 7x18 | 满，左下角斜切（右列 15/18，与 5 镜像） | 右下角 |

### B.2 拼框算式（`draw` 顺序：角→边→中心→底图，均按 origin 补偿）

```
content_w = 640, content_h = 470        // WorldMap BaseImg/0 实测（WorldMap.img/010/020 全一致）
top_h  = Border[0].h = 33
bot_h  = Border[5].h = 18
side_w = Border[3].w = 7
tl_w   = Border[0].w = 60
frame_w = content_w + side_w*2 = 654
frame_h = content_h + top_h + bot_h = 521
```

| 位置 | 件 | 可见左上 | 拉伸 |
| --- | --- | --- | --- |
| 左上角 | 0 | (0,0) | 原生 |
| 上边   | 1 | (60,0) | x = 654-60-7 = 587 |
| 右上角 | 2 | (647,0) | 原生 |
| 左边   | 3 | (0,33) | y = 470 |
| 中心   | — | — | 由底图 640x470 直接铺 (7,33) |
| 右边   | 4 | (647,33) | y = 470 |
| 左下角 | 5 | (0,503) | 原生 |
| 下边   | 6 | (7,503) | x = 640 |
| 右下角 | 7 | (647,503) | 原生 |

底图 `base_position = (7,33)`（内容区左上，供 spot/marker/path 共用）。
`BaseImg/0` 带 origin (320,0)，绘制时补回：`base_img.draw(position + base_position + base_img.get_origin())`，
使可见左上角落在 (7,33)（旧代码把 60x33 的 `Border[0]` 当整窗底板且把底图按窗口中心居中 → 表现为跑到左上角 / 版式错乱）。
关闭钮沿用 `Basic.img/BtClose3`→`BtClose2`（BtClose3 本包不存在，回退 BtClose2 存在），anchor=(frame_w-22,4) 在顶带右端。

### B.3 区域素材

`update_world()` 取 `Map.nx/WorldMap/<WorldMap0XX.img>/BaseImg/0` + `MapList/MapLink`；根名与包内实际一致
（mapid/1e7 前缀拼 `WorldMap0`+prefix，如 106000000→`WorldMap010`，实测存在；缺失回退 `WorldMap.img`）。无缺件需预留。

### B.4 默认位置算式（Configuration.h，仅改默认值）

```
大地图 PosMAP:   x = (1280 - 654)/2 = 313,  y 偏上取 64            → "(313,64)"   （旧 "(100,35)" 贴左上）
小地图 PosMINIMAP: 参考端贴左上角，(0,0) 会裁掉九切片边距，留 5px    → "(5, 5)"     （旧 "(0, 0)"）
```

---

## C. 中文 / 字体

地图名、区域名全部走既有 `Text`（`Text::Font::A12B/A12M`）+ `NxHelper::Map::get_map_info_by_id` 路径，
未新增任何 UTF-8→narrow/GBK 字面量接口调用。度量/清单结果一律写 UTF-8 文件（`dump/*.txt`）后 Read 查看。

---

## D. 本包没有、已放弃或预留的件

| 件 | 处置 |
| --- | --- |
| `MiniMap/BtMin` / `BtMax` / `BtSmall` / `BtBig` / `BtNpc` | 越代件，包内无 → 不再实例化；`toggle_buttons`/`update_buttons` 对其 null-safe（宽度按 0）。切换只由唯一存在的 `BtMap` 驱动。 |
| `MiniMap/ListNpc`（NPC 列表面板） | 包内无 → `listNpc` 空，NPC 列表子面板不显示（BT_NPC 钮也随之失效）。预留：需 ListNpc 九切片才能恢复。 |
| `MiniMap/Window/*`、`MiniMapSimpleMode` 结构 | simpleMode 分支的死取用已删，统一回落到 MinMap/MaxMap，并 `diag_once`。 |
| `WorldMap/backgrnd`（整窗底板） | 包内无（旧代码的回退目标不存在）→ 改为 Border 九切片拼框。 |
| `WorldMap/WorldMapSearch`（搜索栏 backgrnd/notice/BtAllsearch） | 包内无 → `search_background/search_notice` 为空（透明，draw 时 no-op），`set_search(true)` 仅令 dimension 不变。大地图搜索面板功能预留缺件。 |
| `WorldMap/BtSearch` / `BtAutoFly_1` / `BtNaviRegister` | 包内无 → 原代码已 `if(node)` 守卫，不创建，不崩。 |
| `WorldMap/BtClose3` | 不存在 → 回退 `Basic.img/BtClose2`（存在）。 |
| `MiniMap/title`(1x1) / 独立头像 Portrait 件 | 小地图无头像件；点位标记用 MapHelper.img/minimap（既有机制），非缺件。 |
| 2048px canvas 守卫 | 未放宽（保留原防御）。超大地图 canvas 时降级为 MIN。 |
