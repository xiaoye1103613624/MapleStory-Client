# W6 能力值窗（UIStatsInfo）坐标定案 — UIWindow.img/Stat

批次：W6（RC2 版式错位 / RC6 字体颜色 / RC10+§6-Q2 详情收不回）。
仅改 `IO/UITypes/UIStatsInfo.cpp/.h`。经典分支判定：`UiAssetResolve::window4("Stat")`
且 `Stat["main"]` 不可用 → classic（本包恒为 classic）。

取证命令（只读）：
`python tools/nx_inspect.py list "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx" <depth> <path>`
模板匹配脚本 `E:/project/_w6_measure.py`（numpy SAD，仅比对不透明像素），结果
`_w6_match1.txt / _w6_bthide_match.txt / _w6_close_match.txt / _w6_textbands.txt`；
导出图 `E:/project/_w6_pngs/`；参考截图
`C:/Users/11036/Desktop/正确客户端截图/打开了S-E-I-K页面.png`（下称 s1，窗口绝对原点 (175,191)）
与 `角色能力.png`（详情展开态 s2，349x428，窗口绝对原点 (1,2)）。

依据等级：**①** art 逐像素行带实测；**②** 角锚算术（容器宽/高 − 控件宽/高 − 边距 k）；
**③** 截图模板匹配（SAD=像素平均绝对差，0 即全等）。凡 try_maple_btn 的 position 均为
**贴图左上角**（批注约定：内部自动加 origin，本批所有件 origin=(0,0) 除 basicStat=(78,0)）。

## 主窗与控件表（窗口相对左上角）

> **r15 更正（仅影响下方「数值文字表」的 x 与两行 y）**：本篇的数值绘制点是从
> `角色能力.png` 按「标题行暗度对齐」配准后量出来的，该配准被整块贴图 SAD 复核推翻
> （真偏移 0，不是 +6/−1）。定案见 `evidence/w0-common/text-metrics.md`：
> **左对齐数值列 x = 67（非 61）**；纯数字行比含 `/`、含 CJK 行低 3px，故
> **等级 y = 78、人气度 y = 167**，其余 34/52/98/116/134/152 与 243/261/279/297 不变。
> 控件/按钮/贴图坐标不受影响（那些本来就是 origin 算术 + 整块匹配）。

| 控件 | 节点 | WxH + origin | 窗相对左上角 | 依据 |
|---|---|---|---|---|
| 主窗背景 | Stat/backgrnd | 208x347 (0,0) | (0,0)（Sprite 直绘） | ③ s2 SAD=56.4 峰位 (1,2)−(1,2)=(0,0)；标签/绿格烘焙在图内不重绘（优先级1） |
| 关闭钮 | Basic.img/BtClose2（BtClose3 本包不存在，回退链生效） | 14x14 (0,0) | (191,5) | ② 208−14−3 / 5；③ s1 搜索峰恰在 rel (191,5)（SAD=220：截图贴皮与 BtClose2 非同图，仅证位不证图） |
| HP 加号 | Stat/BtApUp/HP | 12x12 (0,0) | (186,115) | ① 绿格右缘 x=200 留 2px；HP 行带顶 113，16px 带嵌 12px 钮 +2 |
| MP 加号 | Stat/BtApUp/MP | 12x12 (0,0) | (186,133) | ① 同上，行带 131 |
| STR 加号 | Stat/BtApUp/STR | 12x12 (0,0) | (186,245) | ① 行带 243 |
| DEX 加号 | Stat/BtApUp/DEX | 12x12 (0,0) | (186,263) | ① 行带 261 |
| INT 加号 | Stat/BtApUp/INT | 12x12 (0,0) | (186,281) | ① 行带 279 |
| LUK 加号 | Stat/BtApUp/LUK | 12x12 (0,0) | (186,299) | ① 行带 297 |
| 能力详情钮 | Stat/BtDetail | 46x19 (0,0) | (153,324) | ② x=208−46−9、y=347−19−4（kx=9/ky=4）；③ s1 匹配 (328,515)−(175,191)=(153,324)，SAD=0.25 |
| 详情收起钮 | **Basic.img/BtHide**（近似复刻，可后续 IDA 复核） | 13x12 (0,0) | (405,326) | ② 面板(208,144)+[218−13−8, 203−12−9]，kx=8/ky=9；③ s2 SAD=**0.0** 全等，见下文 Q2 |
| 基本属性点板 | Stat/basicStat | 159x121 **origin(78,0)** | (8,195)（绘制点=左上角+origin） | ③ s1 SAD=**0.0** 峰 (183,386)−(175,191)=(8,195)；**改为恒绘**（旧逻辑"仅有 AP 且上方有空间"是错的） |
| 详情面板 | Stat/backgrnd2 | 218x203 (0,0) | (208,144)（贴右缘、底对齐 347−203=144） | ③ s2 峰 (209,146)−(1,2)=(208,144)，SAD=46（数值遮挡）；② 角锚同值 |

## 数值行偏移（statoffsets，classic 实测）

行带=数值像素带顶；A11M linespace=15，ASCII 字形顶=pos.y+5 → 带顶−5≈旧写法核对后直取带顶值
（与烘焙标签行同基准）。pitch 全部 18。**旧 DEX/INT/LUK 用 271/297/325，比实测低 10/18/28px
=「数值位置偏低」根因。**

| 行 | 偏移 | 行 | 偏移 |
|---|---|---|---|
| 名字 | (61,31) | 经验值 | (61,149) |
| 职业 | (61,49)（高带26，允许换行） | 人气度 | (61,167) |
| 等级 | (61,77) | AP | (84,212) 右对齐，白框实测 (62..86,215..227) |
| 家族 | (61,95) | 力量 | (61,243) |
| HP | (61,113) | 敏捷 | (61,261) |
| MP | (61,131) | 智力 | (61,279) |
|  |  | 运气 | (61,297) |

不可加点态（AP=0）：`Stat/Disabled{STR,DEX,INT,LUK}` 48x16 (0,0) 盖烘焙绿格 (7, 243/261/279/297)。

详情板 9 行（板相对，左对齐 x=76）：攻击力 min~max (76,6)（max 段宽=min 宽实时拼接）；
物理防御/魔法力/魔法防御/命中率/回避率/手技/移动速度/跳跃力按 (76, 24+i*18) i=0..7，
行序对应烘焙标签 攻击力→跳跃力（s2 逐行文字带实测，pitch 18）。
`is_in_range` 展开态加宽 218。

## RC6 字体/颜色定案

- 颜色：s1 数值像素为纯黑 (0,0,0)；旧值 `EMPEROR`(87 灰) 是「颜色不对」根因 → classic 用
  `Color::Name::BLACK`（现代分支保持 EMPEROR）。
- 字体：native 数值含名字/`(基础+加成)`/伤害区间等**比例文本**，非位图数字 →
  维持 A11M 普通字体（native CJK 字形 11px 与 A11M 行高吻合），**不用** StatusBar.img
  位图字库（Charset 属 HUD 专用件）。此为任务第5条允许的"普通字体则只校 y 基线+颜色"路线。

## §6-Q2 定案：详情收起钮（第一结论）

**包内确无 `Stat/BtDetailClose`、`Stat/BtApDown`**（`UIWindow.img/Stat` 全量子节点仅：
BtApUp、BtAuto、BtAuto1、BtAuto2、BtDetail、Disabled、Job、backgrnd、backgrnd2、basicStat；
Basic.img 按钮根全量核对见 `_w6_basic_tree.txt/_w6_btn_origins.txt`）。旧代码
BT_DETAILCLOSE/BT_DETAIL_DETAILCLOSE 引用的节点不存在 → `set_detail(false)` 永不可达
=「展开后收不回」根因。

s2 详情板右下角收起件与候选逐一匹配（搜索窗 x380..419, y300..337）：

| 候选 | 尺寸 | 最优 SAD | 最优位（绝对） |
|---|---|---|---|
| **Basic.img/BtHide** | 13x12 | **0.0** | (406,328) → 窗相对 (405,326) |
| Basic.img/BtHide2 | 12x12 | 249.54 | (395,328) |
| Basic.img/BtHide3 | 12x12 | 247.79 | (395,328) |
| Basic.img/BtMin | 12x12 | 243.59 | (395,328) |

BtHide（黄绿底白色左箭头，语义 hide）**像素全等**且远优于其余候选 → 接此件：
`try_maple_btn(..., UI["Basic.img"]["BtHide"], (405,326), "Stat/BtHide")`，仅展开态激活
（`set_detail` 控制）；同时 BtDetail 在 classic 下常显、点击即开/关切换（双保险）。
标注：**近似复刻，可后续 IDA 复核**（native 真身或为 UIN 皮肤下另一节点，但资源全等度已是
本包最优）。

## BtAuto 落点（2026-11-05 补证闭合）

新手参考帧（s1 / native_stat）AP 区被 `basicStat` 盖住，SAD 扫 `BtAuto/1/2` 峰 SAD≥53
均为假峰 → 截图路径不可用。改走 **IDA（Maplestory95.i64）+ UIWindow2 dump**：

| 证据 | 结论 |
|---|---|
| `CUIStat::OnCreate` @ `0x867b90` | `CLayoutMan::AddButton(..., nOffsetX=0, nOffsetY=0)` 对 `UI/UIWindow2.img/Stat/main/BtAuto`（id `0x7D7`） |
| `CCtrlOriginButton::GetRect` | `OffsetRect(-get_cx, -get_cy)` → Create(0,0) 时 **topleft = −origin** |
| `UIWindow2_img.json` `Stat/main/BtAuto/normal/0` | 67×34，origin **(−94, −198)** ⇒ 现代 topleft **(94, 198)** |
| 同行 ApUp y | 现代 STR 244 / 经典 245（Δ≈0）⇒ 经典 **y=198** |
| 本包 `Stat/BtAuto` | **100×35**（非现代 67）→ 若按 ApUp Δx=+39 得 (133,198) 右缘越界；按窗宽 208 右缘 2px 钳 **x=106** |

定案：`try_maple_btn(..., Point(106, 198), "Stat/BtAuto")`。`BtAuto1/2` 仍不接（本端仅一颗 `BT_AUTO`，发包逻辑已在 Wave3）。

## 放弃 / 预留项

- `BtAuto1` / `BtAuto2`：包内有件，原生为 Auto 族备选态；本端单钮 `BT_AUTO` 已接 `BtAuto`，不另建。
- `BtApDown`（减点钮）：包内不存在 → v83 无减点入口，接受缺失。
- `Stat/main`、`detail/*`、`cover0/cover1`、`BtHyperStat*`、`detail/BtAbility`、
  `Stat/BtClose3`：本包不存在；原调用点全部清理（含旧坐标 position 版与无 position 版，
  一个不留），现代分支代码仅在未来包出现时可达。
- 经验百分比：`(N%)` 与 UIStatusBar::getexppercent 同式，属同批修正，坐标不新增。
