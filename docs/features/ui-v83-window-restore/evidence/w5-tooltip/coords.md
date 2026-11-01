# W5 悬浮提示（RC3/RC4）· Q1 定案与节点/几何对照

> 取证方式：`python tools/nx_inspect.py list|image "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx" ...`（只读）；
> 复用既有转储 [_w5_tooltip_root.txt](../_w5_tooltip_root.txt)、[_w5_chatballoon.txt](../_w5_chatballoon.txt)、[_w5_worldnotice.txt](../_w5_worldnotice.txt)、[_w5_partyhp.txt](../_w5_partyhp.txt)、[_w5_iconbase.txt](../_w5_iconbase.txt)；
> 本轮新增角件导出与像素统计：[pngs/](pngs/)、[pngs/pixel_report.txt](pngs/pixel_report.txt)。

## 1. Q1 定案：tooltip 框体 = 程序描边（半透明黑底 + 1px 灰边），非贴图

**结论**：选 (c)。包内不存在任何可用作 v83 tooltip 框体的九切片；原生框体按程序描边近似 ——
`Color(0,0,0,0.9)` 底 + `Color(0.5,0.5,0.5)` 1px 四边框，实现为共享件
`ms::TooltipBackdrop`（定义在 `IO/Components/TextTooltip.h`，5 个 tooltip 组件共用）。

### 1.1 排除 (a)「包内九切片借用」的像素证据

候选族逐一 `nx_inspect image` 导出角件/中心件并统计主色（见 pixel_report.txt）：

| 族 | 角件尺寸 | clr (ARGB int) | 中心件 c | 角件主色（实测） | 判定 |
| --- | --- | --- | --- | --- | --- |
| `ChatBalloon.img/0`（及 `dead`、`pet/0`） | nw/ne/sw/se 6x6，w 6x14，n 12x6 | 4278190080 (0xFF000000) | 12x14 **不透明浅灰 (238,238,238,255)** | nw = 白(255,255,255,255)x14 + 黑描边(0,0,0,255)x7 + 透明x12 | 圆角白底气泡（怪物/宠物说话泡），**浅底粗黑边**，与深底细边相反 → 弃 |
| `Login.img/WorldNotice/Balloon` | nw/ne/sw/se **9x9**，e/w 9x1，n/s 1x9，c 1x1 | 4278190080 | c = **(0,0,0,0) 全透明** | nw = (203,221,255,204)x44 浅蓝半透明 | 浅蓝公告泡、内部透明（靠引擎铺 clr）→ 风格不符，且 W4 已占用它做公告框 → 弃 |
| `Login.img/WorldNotice/BalloonForLoginProcess` | nw/ne 22x40，sw/se 22x24 | 4278190080 | c 12x9 | nw = (255,255,221,255) 奶白不透明 | 奶白大公告板 → 弃 |
| `UIWindow.img/UserList/Party/PartyHP` | nw/sw 5x7，ne/se **144x7**，e 144x1 | 无 clr | c 1x1 透明 | nw = (255,255,255,85)+(136,136,136,187) | 血条量规件（横向超长边件），非通用框 → 弃 |
| `UIWindow.img/ToolTip` 本体 | — | — | — | **无 c/e/ne/n/nw/w/sw/s/se 任何子件**（_w5_tooltip_root.txt 全量转储） | 无原生框体件 |

### 1.2 排除「贴图路径藏在别处」的字符串证据

- `E:\project\_ida_ui_out\allstrings.txt`（BeiDou.exe 字面量转储）中 `ToolTip` **仅 2 处命中**：
  `UI/UIWindow.img/ToolTip/Equip/GrowthDisabled` @ `sub_8E49B5:0x8E67A7`、
  `.../GrowthEnabled` @ `sub_8E49B5:0x8E6707`。
- 无任何含 `Frame`/`Frame2`/`ItemIcon`/`balloon` 的资源路径字面量 → 原生 tooltip 构造函数
  （即 `sub_8E49B5` 所在簇）没有从贴图根加载框体。
- 用户已核实：`UIToolTip` 在 exe 中 0 命中（反证现引用代次错误，RC3 成立）。

**✅ 已直证（原为推断）**：[equip-native-compare.md](equip-native-compare.md) §2 用原生 tooltip 截图
逐像素反算出底 α=0.90、边框 (128,128,128)、分隔线白 α0.4，三个值与 `TooltipBackdrop` /
`separator` 的实现完全吻合 ⇒ 「原生为程序描边」不再依赖合成推断。
**后续 IDA 复核线索**（只读产物已在 `E:\project\_ida_ui_out\`，勿新起 IDA）：
- `sub_8E49B5`（tooltip 图标装载 ctor，两处 Growth 字面量在 0x8E6707/0x8E67A7）及其调用方；
- 在其中检索 Gr2D 矩形/线段绘制调用（`drawrectangle` 式）即可坐实描边颜色与 alpha。

### 1.3 (b) MapleFrame 拼法核对

`IO/Components/MapleFrame.cpp:28-36` 取用键名 `c/e/ne/n/nw/w/sw/s/se`，与 inventory §其它 的
九切片命名一致 —— 拼法本身无误，问题仅在包内无深底族，故不再走 MapleFrame。

## 2. 新旧节点路径对照表

| 组件 | 旧路径（全部失效，UIToolTip.img 不存在） | 新路径 / 处置 | 包内状态 |
| --- | --- | --- | --- |
| 5 个 tooltip 框体 | `UI/UIToolTip.img/Item/Frame2`（MapleFrame 9 键）、`.../Frame/{top,line,dotline,bottom,cover}` | **程序描边 `TooltipBackdrop`**（Q1） | — |
| ItemTooltip 图标衬 | `.../Item/ItemIcon/{base,cover,new,old}` | **删除**（原生 tooltip 图标无衬框，直接 2x 绘制） | 包内无对应 |
| EquipTooltip 需求图标 | `.../Item/Equip/Can|Cannot/{reqLEV,reqPOP,reqSTR,reqDEX,reqINT,reqLUK}` | `UI/UIWindow.img/ToolTip/Equip/Can|Cannot/{同名}` | ✓ 47x11 |
| EquipTooltip 数字字库 | `.../Equip/Can|Cannot`（Charset 0-9） | `UIWindow.img/ToolTip/Equip/Can|Cannot`（子件 `0..9` 5x7/3x7） | ✓ |
| EquipTooltip 等级数字 | `.../Equip/YellowNumber` | 改用 `Can`（绿）；包内无黄色字库 | ⚠ 近似 |
| EquipTooltip 职业图标 | `.../Equip/Job/{normal,enable,disable}/{0-5}` | `Can|Cannot/{beginner,warrior,magician,bowman,thief,pirate}`（38x13 职业名图）；`Job/normal` 底衬删除 | ✓ 替代 |
| EquipTooltip 分隔线 | `.../Frame/dotline` | `ColorLine(264, WHITE, 0.4, false)` 程序线 | — |
| EquipTooltip 潜能角标 | `.../ItemIcon/{0-4}` | **删除**（包内无；potflag 文本行保留） | 包内无对应 |
| EquipTooltip ATT 数值 | `.../Equip/Summary/{incline,decline}` | **删除绘制**（包内无；"ATT INCREASE" 文本保留） | 包内无对应 |
| MapTooltip 生活图标 | （已用 `UiAssetResolve::window("ToolTip")/WorldMap/{Mob,Npc}`） | 保持；`UIWindow.img/ToolTip` 下**无 WorldMap 子根** → 空纹理不绘 | ✗ 预留 |
| Skill/TextTooltip 框体 | `.../Item/Frame2` + `Frame2/cover` | 程序描边 `TooltipBackdrop` | — |

**存在但本批未接的件**（当前显示层用 `Text` 已表达同义信息，留作后续批次）：
`ToolTip/Equip/{Property 0-18, Speed 0-6, WeaponCategory 30-47, ItemCategory 1-21, GrowthEnabled, GrowthDisabled, Dot 0-2}`、
`ToolTip/{cash[23x5], mesos[30x5]}`、`UIWindow.img/IconBase/0[32x32]`。

## 3. 框体几何（pos = 组件收敛后的内容原点；均为「左上角 + WxH」）

| 组件 | 框体 rect（相对 pos） | 依据 |
| --- | --- | --- |
| ItemTooltip | `(-6,-6) + (fillwidth+28, fillheight+adj_t+adj_d+18)` | ②角锚算术：内容右界 = desc x90+宽185 = 275 ≤ fillwidth+22=286；下界 = 图标底 82+name.h+adj_t ≤ 框底 |
| EquipTooltip | `(-6,-6) + (264+12, height+32)`（对比窗同宽，横向间距 `FRAME_WIDTH=264`） | ②：内容右界 = reqset 177+54+6 位数字 ≈ 261 → 框宽 264；下界 = 原 bot 底线 height+26 |
| SkillTooltip | `(-6,-6) + (width+30, height+20)` | ②：leveldesc 右界 25+290=315 = width+23 ≤ width+24（框右）；行分隔线在 height 内 |
| TextTooltip | `(-8,-8) + (fillwidth+16, fillheight+16)` | ②：文本 (0,1)/(-1,-2) 起绘，四周留 ≥6px |
| MapTooltip·worldmap | `(0,-2) + (fillwidth+20, fillheight+24)` | ②：框中心 = 标题中心 = fillwidth/2+10（原 frame 中心 halfwidth+14 等价） |
| MapTooltip·minimap 标题态 | `(0,-4) + (fillwidth+16, title.h+12)` | ②：标题中心 fillwidth/2+8 |
| MapTooltip·minimap 详情态 | `(-2,-4) + (fillwidth+20, fillheight+18)` | ②：兼容两种 halfwidth 中心（fillwidth/2+8..+10） |

屏幕越界回收（adj_x/adj_y）公式沿用原实现，未改动。

## 4. 文本颜色/字号校准

**⚠ 本节原结论已被推翻**：原文写「全部 17 张逐一查看，无一张含悬浮 tooltip」。
用户后续补入的 [e75687e9-46b7-4309-9792-c23fee32e8be.png](../native-refs-r14/e75687e9-46b7-4309-9792-c23fee32e8be.png)
（319×362，悬停「白棉T恤 (M)」）**就是一张装备 tooltip 原生图**，已按它做完逐像素对拍，
结论与改动见 [equip-native-compare.md](equip-native-compare.md)。

该图同时**直接坐实**了 §1 的 Q1 定案（原先只是合成推断）：底 = 纯黑 α0.90
（覆盖 238 得 24、覆盖 255 得 26）、四边框 = (128,128,128) 不透明 1px、
分隔线 = 白 α0.4（覆盖黑底得 102、覆盖 24 底得 116）。

内容层方面：名称 A12B、属性行 A11M、描述 A12M 的**颜色与字号与原生一致，本批不改**；
唯一实质偏离是职业名画法（§4 of equip-native-compare.md，已回退为原生叠绘）。
换行/宽度沿用 T12b `Text` CJK 度量，未重实现。

## 5. 本批代码改动清单

- `IO/Components/TextTooltip.h`：新增共享件 `TooltipBackdrop`；成员 frame/cover → backdrop。
- `IO/Components/TextTooltip.cpp`、`ItemTooltip.{h,cpp}`、`SkillTooltip.{h,cpp}`、
  `MapTooltip.{h,cpp}`、`EquipTooltip.{h,cpp}`：去除全部 `UIToolTip.img` 引用；
  EquipTooltip 内容图标改 `UIWindow.img/ToolTip/Equip` 真路径（§2 表）。
