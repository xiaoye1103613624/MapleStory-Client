# W11 / R14-7 世界地图「蓝色点位不对、路线也是错的」——锚点空间定版

**结论**：不是点位数据缺、也不是贴图缺，是**坐标空间读错了一层**。
`WorldMap*.img` 里 `MapList/*/spot`、`MapList/*/path`、`MapLink/*/link/linkImg`
全部以 **BaseImg 的 origin（= 底图中心，640×470 时为 (320, 235)）** 为原点存储，
而我们的代码只加了 `base_position`（画布左上角），于是点位/路线/跳转图整体偏
**(-320, -235)** —— 大多直接落到窗口外或左上角，用户看到的「蓝点位置不对」
和「点了点位后路线是错的」就是这一个根因的两种表现。

- 前置缺陷（同批修掉）：`tools/nx_inspect.py` 把 PKG4 的 type-4（vector）节点当成
  「`data` 槽低 16 位 = x、高 16 位 = y」解码，所以历史上所有 origin/spot 都打印成
  `(x, 0)` 或 `(x, -1)`，且 y 的符号跟着 x 走。这份 evidence 里凡是引用过
  `origin=(320, 0)` 一类数值的旧笔记都作废，以本文件为准。
- 真解码：vector 占节点记录的 **8 字节 = 两个 int32**，x 在记录偏移 +12（`data` 槽），
  y 在 +16（`width`+`height` 槽）。节点记录 20 B：
  `name u32 | first_child u32 | child_count u16 | type u16 | data u32 | width u16 | height u16`，
  第 i 个节点在 `64 + i*20`。
  验证方式：`_r18_vec.py` 直接 hexdump 记录字节 ——
  `WorldMap.img/BaseImg/0/origin` 的 `raw20 = ... 40 01 00 00 eb 00 00 00` →
  `0x140=320`、`0xeb=235`。修完后 `(320, 235)` 一次到位。

## 1 底图 origin 全量核对（28 张 BaseImg）

全部 `WorldMap*.img/BaseImg/0` 的度量（`tools/nx_inspect.py`，修 vector 解码后重新 dump）：

| 文件 | 尺寸 | origin |
| --- | --- | --- |
| 除下列之外的 24 张（`WorldMap.img`、000/010/011/020/021/030/031/032/040/050/051/060/070/080/090/100/140/141/142/143/211/220/230 …） | 640×470 | **(320, 235)** |
| `WorldMap012.img` | 640×469 | (320, 236) |
| `WorldMap013.img` | 640×470 | (320, 236) |
| `WorldMap014.img` | 640×470 | (321, 236) |

即 origin ≈ 底图几何中心（`ceil`），任何一张都成立；**不能把 origin 当常量硬编码**，
必须 `base_img.get_origin()` 现取。全量原始输出保留在
`E:/project/_r18_bases.txt`（临时工作文件，关键行已抄进本表）。

## 2 数据侧证据：spot 值域只可能在「中心原点」空间

`WorldMap.img/MapList`（36 条，全部 `type=3`）的 spot：

```
x ∈ [-279, 239]   y ∈ [-185, 175]
例：0 → (-178, -169)   16 → (216, 125)   20 → (-268, 121)   9 → (-16, -86)
```

- 若 spot 是「相对画布左上角」，负 x/负 y 就意味着点位画在窗外——而原生这些点位
  （维多利亚岛、奥斯、埃洛亨…）明明在窗内。**只有相对中心**才自洽：
  `-279 + 320 = 41`、`239 + 320 = 559`，两者都落在 `[0, 640)`；y 同理。
- 反证：改之前 `-178 + base_position.x(7) = -171` → 窗口左侧之外，正是用户截图里
  「点位挤成一团/看不见、只剩几条」。

`mapImage`（点位贴图，`MapHelper.img/worldMap/mapImage`）：

| index | 尺寸 | origin |
| --- | --- | --- |
| 0 | 20×20 | (9, 9) |
| 1 | 14×14 | (6, 6) |
| 2 | 20×20 | (9, 9) |
| 3 | 13×13 | (6, 6) |

origin ≈ 自身中心 → 印证 spot 是**锚点**（贴图以 origin 点落在 spot 上），
而不是贴图左上角。13×13 的 type-3 点，锚点就是那颗点画的中心。

## 3 路线（path）与跳转图（linkImg）同空间

`MapList/*/path` 是「bitmap 值 + `origin`/`z` 子节点」的属性型节点（例：
`0/path` = 112×125，origin (279, 180)，z=2）。判据取「贴图必须整块落在 640×470 画布内」：

| path | 尺寸 | origin | 以 base_anchor 画的可见左上角 | 是否入画 |
| --- | --- | --- | --- | --- |
| `MapList/0/path` | 112×125 | (279, 180) | (7+41, 33+55) = (48, 88) | ✓ |
| `MapList/9/path` | 462×316 | (214, 132) | (113, 136) → 右下角 (575, 452) | ✓ |
| `MapList/6/path` | 57×279 | (25, 95) | (302, 173) → 下边缘 (302, 452) | ✓ |
| `MapList/20/path` | 178×70 | (279, -110) | (48, 178) | ✓ |

`MapLink/*/link/linkImg` 用同一判据逐条验过，**每张都整块入画**；
按改前的 `position + base_position` 画则每张都跑到窗外（因为 `Texture::draw(p)`
的可见左上角 = `p − origin`，linkImg 的 origin 有几百像素）。

## 4 改动（`IO/UITypes/UIWorldMap.{h,cpp}`）

新增成员 `Point<int16_t> base_anchor;`（[UIWorldMap.h:136](file:///e:/project/MapleStory-Client/IO/UITypes/UIWorldMap.h#L136)），
在 `update_world()` 里跟着底图现算：

```cpp
base_anchor = base_position + base_img.get_origin();   // UIWorldMap.cpp:528
```

然后把**所有非底图**的绘制/命中从 `base_position` 换成 `base_anchor`：

| 位置 | 改前 | 改后 |
| --- | --- | --- |
| `draw()` linkImages | `draw(position + base_position)` | `draw(position + base_anchor)`（cpp:183） |
| `draw()` path_img | `draw(position + base_position)` | `draw(position + base_anchor)`（cpp:192） |
| `draw()` spot_pos | `spot.first + position + base_position` | `… + base_anchor`（cpp:197） |
| `draw()` 选中光晕 | `spot_pos - (4,4)` | `spot_pos - marker.get_origin() - (4,4)`（cpp:208） |
| `draw()` npcPos/curPos | `spot.first + position + base_position` | `… + base_anchor`（cpp:228-229） |
| `send_cursor()` 点位判定 | `path.first + position + base_position - 10` | `… + base_anchor - marker.get_origin() - (4,4)`，尺寸 `dims + (8,8)`（cpp:473-475） |
| `update_world()` AreaButton | `base_position - link_image.get_origin()` | `base_anchor - link_image.get_origin()`（cpp:552） |

两点顺带收口：

1. `base_img.draw()` 之前已经加了 `+ base_img.get_origin()`（把可见左上角钉回
   `base_position`），这行是对的，注释里写死的 `(320,0)` 是 nx_inspect 旧 bug 带出来的
   错值，改成 `(320, 235)`。
2. 判定框里那个 magic `- 10` 是在错误的锚点空间上硬凑的补偿，一并去掉，改成
   **「判定区 = 选中光晕区 = marker 可见框外扩 4」**，两者自洽，不再各算一套。

`base_position` 保持原义（画布左上角 = `content_topleft` = (7, 33)），只服务底图本身。
构造函数里把 `base_anchor` 先初始化成 `content_topleft`，`update_world()` 一定在
任何绘制发生前跑过（`update()`/`toggle_active()` 都会调），所以不会出现「用默认值画一帧」。

## 5 部署后必须点验的项（并进 CHECKLIST W11）

1. 打开 W 大地图：**点位要散布在底图对应城市上**，且当前地图的蓝点（`curPos`）
   要压在所处位置，不再挤在窗内左上角。
2. 鼠标悬浮任一点位 → 该点位的路线 `path` 图要**连着该点和它的邻接点**画出，
   而不是从窗口左上角射出来。
3. 悬浮 `MapLink` 区域（如大陆之间的航线/船线）→ 跳转图出现在**它自己的位置**，
   且判定点与贴图重合（点得到才亮，不会出现「鼠标在 A 处、亮的是 B」）。
4. 点击点位 → 黄色选中光晕要**正好框住那颗点位贴图**（原来光晕会偏移半个点位）。
5. 换到 `WorldMap014.img`（x origin=321）一类非标准底图再看一次，确认没有把
   (320,235) 当常量写死的残留。

## 6 仍未闭环 / 阻塞

- **原生点位判定框尺寸**无法从 IMG 直接读到（`MapList` 没有 hit 尺寸属性），
  本文件用「外扩 4 对齐光晕」是**可读性/自洽优先的工程选择**，不是 IDA 证据。
  若真机觉得难点中，需要 IDA 校准（本会话无 `ida-pro-mcp`）→ 记为待 IDA 项。
- `WorldMap` 根下 28 个子项里除 `WorldMap.img` 外是否还有非 `WorldMapNNN.img`
  命名的底图（如 `WorldMapSpecial`）未逐一张清点；`update_world()` 走的是
  `"WorldMap0" + prefix` 精确名 + 兜底 `WorldMap.img`，异常命名会静默兜底。
- 本文件推翻了 `docs/features/ui-v83-window-restore/evidence/` 里所有引用
  `(x, 0)` 形态 origin 的历史结论，逐篇回扫在 #43 归档轮处理。
