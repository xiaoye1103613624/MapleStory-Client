# W11 / R14-8 小地图：顶带分区、地图标记与「大地图」钮语义

测量对象是**切片美术本身**（`UIWindow.img/MiniMap/{MinMap,MaxMap,Min}`、
`MapHelper.img/mark/*`），不是截图 —— 截图配准在本包只有两张极小裁图
（`d15f70a2` 126×99、`9d28d3c5` 125×126，且后者顶部被切掉），SAD 分别只有
8.5 / 23.9，达不到本项目「SAD≈0.00 才算像素级」的配准门槛，所以文字基线用
**切片分区 + 已定版的 A12B 字量模型（汉字墨迹 = draw_y + 6）** 反推，
并把最终确认留给真机抓帧（§6）。

## 1 MiniMap 节点全量清单（本包只有这些）

`tools/nx_inspect.py list Map/UI.nx 6 UIWindow.img/MiniMap`：

```
MiniMap
  BtMap      36x12  origin(0,0)   {normal,mouseOver,pressed,disabled}
  MaxMap     nw 60x72 | n 1x72 | ne 6x72 | w/e 6x1 | c 1x1 | s 1x15 | sw/se 6x15
  Min        w 8x20 | c 1x20 | e 4x20
  MinMap     nw 60x29 | n 1x29 | ne 6x29 | w/e 6x1 | c 1x1 | s 1x14 | sw/se 6x14
  title      1x1
```

三个结论：

1. **所有 origin 都是 (0,0)** —— 小地图这一族的 `slice()` origin 补偿是恒等的，
   `UIMiniMap.cpp` 里那段补偿代码保留无害，但不要指望它解释任何偏移。
2. `UIMiniMap::update_canvas()` 用的常量 `n_top=29 / n_bot=14 / m_top=72 /
   m_bot=15 / side=6 / n_cap=60`、以及 Min 的 `8 / 1x / 4`，**与切片逐一对上**，
   九切片拼法本身没有错。`MAX_ADJ = 43` 也正好是 `72 - 29`。
3. **`UIWindow.img/MiniMap` 子树没有 BtMin / BtMax / BtSmall / BtBig / BtNpc**
   —— 小地图**这一棵子树**上只有一颗 `BtMap`。之前渲染出「透明空壳钮」的问题
   （W11 已修）根因就在这。
   ⚠ **2026-11-05 翻案**：这个「只有一颗钮」不能外推到全包 —— 全包
   `Basic.img/BtMin`（蓝 −）/`BtMax`（灰 +）12×12 四态是现成真件（聊天栏已用，
   `evidence/w4-chatbar/coords.md:18` 逐像素一致），已绑回 `UIMiniMap BT_MIN/BT_MAX`
   并实机验证三态切换 + 持久化 + 重启保持，见
   [docs/features/minimap-mode-switch/README.md](../../../minimap-mode-switch/README.md)。
   下文 §4/§7 基于「全包只有一颗钮」的推论与 `minimap_5x.png` 排除结论同批作废。

## 2 顶带分区（决定性证据）

对 `MaxMap/nw`(60×72) 与 `MaxMap/n`(1×72) 取 x=52..59（避开烤死的「小地图」
艺术字）逐行平均色，两者完全一致：

| y | 颜色 | 含义 |
| --- | --- | --- |
| 0 | (0,0,0) | 最外黑边 |
| 1 | (255,255,255) | 亮边 |
| 2 | (85,119,153) | 深蓝描线 |
| 3 | (153,187,204) | 蓝描线 |
| 4 | (136,153,187) | 过渡 |
| **5..18** | **(255,255,255)** | **白色标题条（14 行）** |
| 19 | (136,153,187) | 分隔 |
| **20..63** | **(153,187,204)** | **浅蓝信息带（46 行，含 64/65 的浅色收尾）** |
| 64..65 | (204,221,221)/(187,204,221) | 带底浅化 |
| 66..69 | (85,119,153) | 深蓝分隔 |
| 70..71 | (238,238,238) | 与内容区的亮边 |

`MinMap/nw`(60×29) 同法：`y5..18` 白色标题条，`y19..22` 浅蓝收尾，
`y23..26` 深蓝，`y27..28` 亮边 —— **NORMAL 态没有浅蓝信息带**，
所以原生 NORMAL 只有一行街道名、且它是**压在地图画布上**的描边白字，
不是放在色带里。这与 `d15f70a2` 一致。

`MaxMap/nw` 里烤死的「小地图」艺术字：整块 nw 的暗色像素（max(rgb)<110）
共 292 个，分布在 x0..59 / y0..71（含边框描线）；文字本体目视在 x6..50 / y5..18，
即**正好占满白色标题条**。

## 3 由分区推出的三处修正

| 元素 | 改前 | 改后 | 依据 |
| --- | --- | --- | --- |
| 地图标记 `MapHelper/mark/<mapMark>` | `(8, 8)` | `(MARK_X=8, MARK_Y=24)` | 标记全量 38×38（`list MapHelper.img/mark` 每个子节点都是 38x38）。改前落在 **y8..45**，压在白色标题条和「小地图」艺术字上；信息带 y20..65 高 46，38 居中 → `20+(46-38)/2 = 24` |
| `region_text`（地图名，第一行） | `(46, 28)` | `(46, 22)` | 墨迹 = draw_y+6 → 28..44，落在信息带 y20..65 内且与标记同行起 |
| `town_text`（街道名，第二行） | `(46, 44)` | `(46, 39)` | 墨迹 45..61，仍在带内；改前 draw_y=44 → 墨迹 50..66，**第二行下沿顶到 y66 的深蓝分隔线**，正是「文字错位」 |
| NORMAL `town_text` | `(8, 18)` | 不改 | NORMAL 无信息带，街道名本就压在画布上沿；`d15f70a2` 目视一致，且该图配准 SAD=8.5 不足以支撑再抠 2~3px |

文字 x=46 的来源：标记占 x8..45（`MARK_X + 38`），文字紧随其后 —— 与
`9d28d3c5` 中两行字右移避开标记方框的观感一致。

## 4 「大地图」钮语义：这颗钮是打开世界地图

`UIWindow.img/MiniMap/BtMap/normal/0` 导出来是 36×12 的橙底白字，
**内容就是「大地图」三个字**（`E:/project/_r19_btmap_5x.png`，本目录
`btmap-5x.png` 留档）。结合 §1（本包 MiniMap 只有这一颗钮、没有任何缩放钮），
原生语义只可能是「打开大地图」。

改前 `button_pressed(BT_MAP)` 做的是 `MIN→NORMAL→MAX` 循环（旧注释写的是
「用户实测：点击应在小/大地图间切换」）。改为：

```cpp
// UIMiniMap.cpp:445 起
if (auto wm = UI::get().get_element<UIWorldMap>()) { if (!wm->is_active()) wm->makeactive(); }
else UI::get().emplace<UIWorldMap>();
```

⚠ **2026-11-05 翻案**：`BT_MAP` 语义（打开世界地图）保持正确；但三态切换**不再**
靠 `send_key` 调试分支 —— 那个「任意键循环切态」是非原生行为，已删除。现在
MIN/NORMAL/MAX 由顶带 `Basic.img/BtMin`（−）/`BtMax`（+）真件逐级切换，
`button_pressed` 写回 `Setting<MiniMapType>` 并 `Configuration::get().save()` 落盘，
去城镇强制 MAX 后换图/重启都保持同一全局态。实机验证见
[docs/features/minimap-mode-switch/README.md](../../../minimap-mode-switch/README.md)。
下面 §6 第 5 项「user_type 只由调试键改、需确认是否保留」同步作废：已确认为
全局持久化项、实机坐实。

## 5 顺带核对通过（未改）

- `map_draw_origin_y = n_top`（NORMAL 画布从 y29 起）、MAX 由 `+ MAX_ADJ` 落到 y72 ✓
  与分区里「72 顶带结束、内容区开始」的 y70..71 亮边吻合。
- `body_w = max(106, mark_text_width - 12, map_w)`，其中 `mark_text_width = 48 + 文字宽 + 10`
  → 最小内容宽 = 文字宽 + 46，与「标记 38 宽 + 左缩 8 + 文字间隙」自洽 ✓
- 画布 1:1 绘制 + 标记坐标 `/scale`（`scale = 2^mag`）是 v83 的正解，
  `width/height` 元数据与 bitmap 实际像素双重上限（`kMaxCanvasEdge=2048`）保留。

## 6 待真机确认（并入 CHECKLIST W11）

1. MAX 态：蘑菇标记是否落在浅蓝带内、不再压「小地图」三字；两行字是否
   一行贴带上一行贴带下、都不碰 y66 的深蓝线。
2. NORMAL 态：街道名与画布上沿的叠压关系是否与 `d15f70a2` 一致。
3. **点「大地图」钮 → 应打开世界地图**（§4 行为改动，需用户拍板）。
4. 换一张 `info/mapMark` 为空的地图，确认标记缺失时不崩、不留黑块。
5. ⚠ `user_type`（Setting<MiniMapType>）**已定案保留并落地**（2026-11-05）：
   它是全局单一 `minimap.type`，`button_pressed` 写回 + `Configuration::get().save()`
   落盘到 `Settings`，跨图/重启保持同一态，实机坐实 —— 不再是「只由调试键改」。

## 7 已知不足 / 阻塞

- 本包只有两张极小的小地图截图，**文字基线未能用像素配准钉死**，
  现在的 22 / 39 是「切片分区 + 字量模型」的推导值，误差估计 ±2px。
- ⚠ **2026-11-05 翻案**：`refcrops/minimap_5x.png`（640×720）里的 `−` / `+`
  两颗钮**正是本包全包现成真件** `Basic.img/BtMin`（蓝 −）/`BtMax`（灰 +），
  之前以「`MiniMap` 子树只有 BtMap」为由把它排除出证据链是**错误外推**（把子树
  结论当全包结论）。该帧已回归为三态版式与按钮的还原目标，实机抓帧坐实，
  见 [docs/features/minimap-mode-switch/README.md](../../../minimap-mode-switch/README.md)。
- 无 `ida-pro-mcp`，`BtMap` 的处理函数无法从二进制侧确认（§4 属资源语义推断）。
