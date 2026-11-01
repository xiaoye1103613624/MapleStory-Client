# W4 聊天栏（RC15）· 控件坐标与依据

> 取证：`python tools/nx_inspect.py list/image "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx" <depth> <path>`（只读）
> 参考端截帧：`C:\Users\11036\Desktop\正确客户端截图\打开了S-E-I-K页面.png`（1280×720，1:1）
> 本端几何：1280×720；classic 输入行 `position = (8, vh-71-INPUT_ROW_H) = (8, 625)`、`dimension = (560, 24)`
> 度量脚本产物（本目录）：`meas_row.txt` `meas_box.txt` `meas_btn.txt` `meas_whisper.txt`
> `montage_buttons.png` `ref_left_zoom.png` `ref_right_zoom.png` `ref_row_1.png`
> 优先级按 batch-conventions §3：1=烤进背景 / 2=角锚算术 / 3=截图实测

## 1. 参考端实测（截图，屏幕绝对坐标）

| 元素 | 实测 | 说明 |
| --- | --- | --- |
| 输入行白带 | 上边线 y=653、下边线 y=681（内区 654..680，27px） | 整行白底横贯全屏；本端行高仍用既有 `INPUT_ROW_H=24` |
| `base/chatTarget` 框 | x 0..80（81 宽）、y 656..676（20 高） | 与节点 81×20 完全吻合 → 行内相对 (0, +2) |
| 目标文字「对所有人」 | 左沿 x≈6、右沿 x≈56 | 左对齐、紧跟箭头右侧，**不是居中**（旧实现居中） |
| `BtWhisper` ▶ 箭头 | 黑色箭头仅 x≈1..3、y≈669..672 露出 | 箭头本体在 12×19 贴图的 (3..7, 6..10)（见 `meas_whisper.txt`）→ 钮原点 ≈ 行内 x -3..0，压在框左沿 |
| 展开/缩放钮 | 12×12 于 x 536..547、y 661..672 | 逐像素 ASCII 比对：与 `Basic.img/BtMin`（横杠在第 7..8 行）**完全一致**；`BtMax` 是十字、`BtMax2`/`BtMin2` 也都是十字形 → 排除 |

## 1'. R16 定版：底栏整体几何（本节推翻 §1/§5 里「输入行绝对 y 越界」的判断）

> 取证脚本 `E:\project\_r17_band.py`，输出 `E:\project\_r17_band.txt`；
> 素材导出 `E:\project\_r17_{chatTarget,chat,backgrnd,bar,grad,BtMax,BtMin}.png`
> （`nx_inspect.py image <UI.nx> <node> <out>`，`image` 子命令**没有 depth 参数**）。
> 配准底图换成 `正确客户端截图/底部信息栏、按钮、快捷键窗.png`（**1282×142**，通栏），
> 之前用的 `底部信息栏和功能按钮.png`（816×142）是半宽裁图，只能定 y、定不了 x。

整图 alpha 掩膜 SAD 配准（搜索范围已按 §1 的先验收窄，全图暴力扫会误配）：

| 件 | WxH | 最佳落点（裁图坐标） | SAD | 结论 |
| --- | --- | --- | --- | --- |
| `base/backgrnd` | 817×71 | (0, 70) | **5.30** | 底栏顶 = 裁图 y70；左沿 = 裁图 x0 |
| `base/chatTarget` | 81×20 | (0, 78) | **5.01** | 与 backgrnd **同左沿**，顶 = 底栏顶 + 8 |
| `base/chat` | 566×5 | (0, 102) | **4.56** | 顶 = 底栏顶 + 32（白带下方那条，不是输入框） |
| `gauge/graduation` | 340×31 | (221, 107) | **0.00** | 量规 = 底栏顶 + 37、x221（本端 GAUGE_X=218，Δ3） |
| `base/backgrnd2` | 570×71 | (0, 70) | 17.58 | **不是干净配准** → backgrnd2 与 backgrnd 同原点叠加绘制，其上被白带/快捷格覆盖，不能用来定 x |
| `Basic/BtMax` / `BtMin` | 12×12 | — | 35.9 / 27.4 | 在 x480..600 内无匹配 → 参考端那颗蓝方块不是这两张贴图的本色（按钮带底衬）；位置沿用 §1 的实测 (536,661) |

白色输入带的**逐行**实测（>200 亮度连续段）：带 = 裁图 y74..99（内区）、y100..101 一条深色分隔线；
横向 y74/75/98/99 都是 x3..820 通栏白 ⇒ **带是 `base/backgrnd` 自身烘焙的一条**，
宽 817（= backgrnd 宽），顶 = 底栏顶 + 4，内高 26。带内 x5..82 被 chatTarget 蓝框占住，
x83..576 才是输入区，x577 之后盖的是快捷格图标。

⇒ 换算成屏幕绝对值（底栏顶 = vh-71 = 649）：带 **653..678**、chatTarget **(0,657)**、
`base/chat` **(0,681)**。这与 §1 里对 1280×720 全图量到的「白带上边线 653、chatTarget y656..676」
逐条吻合，Δ≤1。

**根因**：旧 `position = (8, vh-71-INPUT_ROW_H)` = (8, 625) 把整行放在底栏**上方**，
而原生是把行放在底栏**内部**那条带上 —— 差 28px。所以「对所有人」和展开钮都浮在白带上沿之外。
§5 原文把它记成「属底栏整体几何、本批越界未动」，本节把它收编并改掉。

改后的定版常量（`IO/UITypes/UIChatBar.h`，全部是**行内相对**，绝对 y 由 `vh-71+4` 推）：
`INPUT_BAND_TOP=4`、`INPUT_ROW_H=24→26`、`MAX_ROW_W=560`、`position=(0, vh-71+4)`、
`TARGET_BOX_Y=4`（chatTarget 与 BtWhisper 同顶）、`TARGET_TEXT_Y=2`、
`LOG_BT_X=536`/`LOG_BT_Y=8`（定宽底栏 ⇒ 用固定左偏移，废弃旧的 `LOG_BT_RIGHT_PAD=20` 右锚算术）、
`UNDERLINE_Y=INPUT_ROW_H+2`、`min_view_y = position.y - user_view_y`（历史框底边贴白带顶）。
目标文字 y 由 §2 的 `-4` 改 `-4→2`：旧值是按「Textfield 字形比 bounds 顶低 10px」的经验式凑的，
本批改用定版模型 `ink_top = draw_y + linespace(17) − SimHei bitmap_top(11) = draw_y + 6`
反推（框内区 657..676、11px 字形居中 ⇒ 墨迹顶 661 ⇒ draw 655 ⇒ 行内 2）。

## 2. 控件 → 节点 → 坐标

| 控件 | 节点路径 | WxH + origin | 行内相对左上角 | 屏幕绝对（本端） | 依据 |
| --- | --- | --- | --- | --- | --- |
| 目标框底 | `StatusBar.img/base/chatTarget` | 81×20，位图无 origin | `(0, 2)` | (8, 627) | 3（参考端框体 x0..80 / y656..676 即行内 (0,2)）；沿用既有画法 |
| 目标文字 | `Text A11M` | — | `(TARGET_TEXT_X=14, -4)` | (22, 621) | 3（参考端左沿 x≈6，但本端箭头占 0..12，故 14；y=-4 是真机字形带校准，沿用） |
| 目标切换钮 | `StatusBar.img/BtWhisper` | 12×19，origin (0,0) | `(0, 2)` = `((24-19)/2)` | (8, 627) | 2+3（与 chatTarget 同顶；参考端钮原点行内 x≈-3..0，本端取 0 保证不出屏、不丢命中区） |
| 目标框命中（选择） | 同上（贴图，非按钮） | 81×20 | `(12, 2) + (69, 20)` | (20, 627)..(89, 647) | 2（箭头右侧到框右沿；`send_cursor` 手动矩形，已避开箭头钮 0..12） |
| 展开钮（收起态可见） | `Basic.img/BtMax` | 12×12，origin (0,0) | `(528, 6)` = `(560-12-20, (24-12)/2)` | (536, 631) | 3（`LOG_BT_RIGHT_PAD=20` 使屏幕 x=536 = 参考端实测 536，Δ0；y 行内 6 vs 参考 8，Δ2，因参考白带 27px 而本端行高 24） |
| 缩放/收起钮（展开态可见） | `Basic.img/BtMin` | 12×12，origin (0,0) | 同上 `(528, 6)` | (536, 631) | 3（与 BtMax 同位互斥，`set_classic_log()` 翻 active） |
| 输入下划线 | `StatusBar.img/base/chat` | 566×5 | `(0, 19)` | (8, 644) | 沿用（它是细线，**不是输入框**，不拉伸、不当背景用） |
| 历史日志框体 | 无贴图（程序描边） | — | `(0, -user_view_y)` 即 (0,-80)，`560×80` | (8, 545) | 跨批定案：复用 W5 的 Q1 判据共享件 `ms::TooltipBackdrop`（半透明黑底 + 1px 灰边，`IO/Components/TextTooltip.h`），不再裸画 ColorBox、不用九切片 |

`try_maple_btn`（W0 公共件）按**贴图左上角**解释坐标，且这批节点 origin 均为 (0,0) → 传参即屏幕相对行的左上角，无需再加 origin。

## 3. 根因与行为改动

1. **点了没反应的真根因**：`is_in_range()` 用基类 `get_position()`，而它在 classic 下会按 `top_y+center_y+user_view_y = 100` 把命中矩形整体上移 → 输入行（625..649）根本不在命中区，`UIStateGame::get_front()`（`IO/UIStateGame.cpp:821`）就不会把光标派发给聊天栏。本批在 classic 下改用 `position..position+dimension` + 历史框矩形，并删掉原先按 `get_position()` 算的错误 log 矩形。
2. 展开/缩放原先是 `send_cursor` 里手搓的 12×12 矩形 + `iconRed/iconBlue` 贴图，**没有挂任何按钮** → 换成 `Basic.img/BtMax|BtMin` 真按钮（同位互斥），手动矩形撤掉以免抢在按钮之前吞点击；`button_pressed` 的 BtMax/BtMin 在 classic 下翻 `classic_log_open`（经 `set_classic_log()`），modern 路径原样 `toggle_view(bool,bool)`。
3. 公开无参 `toggle_view()` 在 classic 下语义改为翻历史框（原先只翻 inert 的 `view_max`）。
4. 目标选择：`BtWhisper` 从「行右端」移到 `chatTarget` 框左沿（参考端位置），并保留「点框体本身 = 循环切换」；目标文字由居中改左对齐。封包语义未动（`cycle_chat_target()` 仍只切标签，实际发送走 `/w`、`MultiChatPacket` 既有分支）。

## 4. 走错代次 / 放弃与预留

| 件 | 状态 |
| --- | --- |
| `StatusBar.img/chat/ingame`（含 `view/btMax`、`view/btMin`、`input/button:chat…`） | inventory 标 ✗（本包不存在）。modern 分支保留（只有 `StatusBar3.img` 在场才会走），但补一条 `UiAssetResolve::diag_once("chatbar.modern.chat_ingame")` 留痕；classic 分支不再引用它 |
| `BtChat` / `BtItemLink` / `BtChatEmoticon` / `BtHelp` / `BtOutChat` | **classic 预留不建**：本包无对应素材根。现有 `toggle_input/toggle_view/update_view` 已有 `classic_mode` 早退保护，不会解引用空指针 |
| `base/iconRed` / `base/iconBlue` | 节点存在（12×12 ✓），但被 `Basic.img/BtMax|BtMin` 真按钮取代 → 成员与取用一并删除 |
| `Basic.img/BtMax2` / `BtMin2` | 存在（12×12），但两者都是十字形（`meas_btn.txt`），无法表达「收起」语义 → 放弃，选 `BtMax`(+) / `BtMin`(-) |
| 白底整行输入框 | 参考端输入行是白底横条；本包无对应件（`base/chat` 是 566×5 细线，判据明确禁止当输入框用）→ 保持现状，不新造 |
| 聊天历史框九切片 | 包内 5 族九切片经 W5 像素实测均为浅底/透明/gauge，已全部否决 → 用 `TooltipBackdrop` 程序描边 |

## 5. 未决 / 越界项（需主线处理）

- `IO/UIStateGame.cpp:413` 注释「classic HUD 没有 BtMax/BtMin」本批后已过时（该文件不在本批授权范围，未改）；其 `TOGGLECHAT` 分支 `toggle_input(true)` 行为不受本批影响。
- ~~classic 输入行的**绝对 y**（本端 625 vs 参考端 653，Δ28）属底栏整体几何（W1/`base/backgrnd[817x71]` 锚定）范畴，本批只做行内相对版式，未动 `position`。~~ **R16 已解决**，见 §1'。
- `GAUGE_X=218` vs 参考端实测 221（Δ3）、`GAUGE_Y=38` vs 37（Δ1）—— 属 W1/`UIStatusBar` 范畴，本批未动，留作后续微调。
- 目标切换目前仅改标签，不改变发送通道（`/w`、`/p`、`/g` 命令仍按原文本解析）——要按目标直接改路由属协议/语义改动，需单独决策。
- 未构建、未部署（按 conventions §4）。
