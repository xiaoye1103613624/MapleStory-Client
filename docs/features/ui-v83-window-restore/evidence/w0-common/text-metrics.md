# 公共件：文字落位度量模型（r15 定版）

**为什么单独一篇**：W1–W13 里反复出现「数值/标题位置偏了几像素」，根因不是坐标常量本身，
而是**配准错**（截图 ↔ 贴图没对齐就量墨迹）+ **把字形顶当成绘制点**。
本篇给出可复用的两步法，后续所有文字落位都按它解，不再逐窗猜。

## 第 1 步：把截图配准到窗口（必须先做）

窗口贴图节点的 `origin` 决定贴图像素 (0,0) 落在窗口哪个位置：
`Graphics/DrawArgument.h:134` ⇒ **左上 = 绘制点 − origin**，而 `UIElement` 的 `sprites`
一律画在 `position`，所以 **贴图像素坐标 = 窗口坐标 − origin**。

- `Stat/backgrnd` origin 实测 `(0,0)`（`nx_inspect list UI.nx 2 UIWindow.img/Stat`）⇒ 贴图像素 == 窗口坐标。
- 校验手段：整块贴图对截图做 alpha-masked SAD（`E:/project/tmpl_np.py`），取全局最小。
  9f1b69f5 vs `Stat/backgrnd`(裁 208×300) ⇒ **(0,0) 1.13/像素，次优 (0,1) 3.14** ⇒ 配准成立。

> 反例（本篇推翻的旧结论）：`w6-stats/coords.md` 用「角色能力」标题行做暗度对齐，
> 得出 `art_y = crop_y + 6`、`art_x = crop_x - 1`，于是把数值列定在 x=61。
> 整块 SAD 显示真实偏移是 0 —— **x 少 6、y 少 1**，且标题行本身有 1px 抗锯齿边，
> 单行特征做配准不可靠。**只用整块贴图 SAD 配准，不要用单行/单特征对齐。**

## 第 2 步：由墨迹反解绘制点

渲染器 = SimHei（`Configuration.h:518` `font.path.normal = fonts/SimHei/simhei.ttf`），
`addfont(FONT_NORMAL_STR, A11M, 0, 11)` / `A12M, 0, 12`（`GraphicsGL.cpp:237-239`）。

```
linespace = int(maxrows * 1.35 + 1)        // GraphicsGL.h:247，maxrows = ASCII 32..127 最大位图行数
基线      = draw_y + linespace             // LayoutBuilder::add_line 的 line_y = ay
墨迹顶    = draw_y + linespace - bitmap_top// GraphicsGL.cpp:1386  char_y = y + ay - ch.bt
墨迹左    = draw_x + line_x + bitmap_left  // line_x：LEFT 0 / CENTER -adv/2 / RIGHT -adv
```

SimHei 逐字形 `bitmap_top`（PIL 同字体同像素实测，脚本 `E:/project/_r15_font2.py`，
输出 `_r15_font2.txt`）：

| 字形类别 | A11M（size 11，linespace **17**） | A12M（size 12，linespace **18**） |
|---|---|---|
| 数字 `0-9` | top 8 → 墨迹顶 = **draw_y + 9**，高 9 | top 9 → **+9**，高 10 |
| `/` | top 11 → **+6**，高 12 | top 12 → **+6** |
| CJK（`技`/`名`…） | top 11 → **+6**，高 12 | top 12 → **+6** |
| `-` | top 4 → **+13** | top 4 → **+14** |
| 推进量 | 数字 6、CJK 11 | 数字 6、CJK 12 |

**关键推论：同一列里「纯数字」和「含 `/` 或 CJK」的墨迹顶差 3px。**
所以不能对一列数值套同一个 y 偏移 —— 这正是 R14-1 第一版「统一 +3」把等级/人气度画低的原因。

## 应用实例

### 能力值窗（`UIStatsInfo.cpp` classic）

| 行 | 值 | 墨迹顶实测(=窗口) | 类别 | 解出 draw_y | 常量 |
|---|---|---|---|---|---|
| 名字 | `Admin` | 40 | 含升部字母 top 11 | 34 | 34 ✓ |
| 职业 | `新手` | 58 | CJK | 52 | 52 ✓ |
| 等级 | `1` | 87 | 数字 | 78 | 78（旧 80） |
| 家族 | `-` | 不可测（与底纹虚线混） | — | — | 98 保持 |
| HP | `50 / 50` | 122 | 含 `/` | 116 | 116 ✓ |
| MP | `5 / 4357` | 140 | 含 `/` | 134 | 134 ✓ |
| 经验值 | `0 (0%)` | 158 | 含 `(` top 11 | 152 | 152 ✓ |
| 人气度 | `0` | 176 | 数字 | 167 | 167（旧 170） |
| AP / 力量 / 敏捷 / 智力 / 运气 | — | 被「基本属性点」浮层盖住（浮层 x8..170 y172..330） | — | — | 不猜，保持 |

x：所有数值行墨迹左缘 = **67**（`Admin` 67..95、`50 / 50` 67..103、`1` 67..72，
与 SimHei 11px 推进量逐项吻合）⇒ 左对齐常量 61 → **67**（AP 是右对齐，另算）。

### 技能窗 SP 数值（`UISkillBook.cpp` classic）

窗口原点 (308, 2)（`Skill/skill1` 行底 SAD 配准，见 `w9-skill/coords-r15.md`）。
SP 白框边框 = 窗口 x 61..111、y 265..281；数字「0」墨迹 = x 86..91、y 274..282。
A12M 数字：墨迹顶 = draw_y + 9 ⇒ **draw_y = 265**；RIGHT 对齐单字符墨迹左 = draw_x − 6 ⇒ **draw_x = 92**。
旧值 (111, 258) ⇒ 数字甩到框右上角外（右 19px、上 7px）。

## 复用清单（下次量文字位置照抄）

1. `nx_inspect list` 取该贴图节点 origin；origin≠(0,0) 时先换算。
2. `tools/nx_inspect.py image` 导出贴图 → `E:/project/tmpl_np.py 截图 贴图 前缀` 做整块 SAD 配准（贴图比截图高就先裁）。
3. 用 `_r15_stat.py` 式的「逐行暗像素 run」取墨迹包围盒，**别用亮度阈值找白框**（整片背景都亮时会失效）。
4. 判类别（纯数字 / 含 `/` / 含 CJK）→ 查上表 → 解 draw 点。
5. 度量脚本改字体大小时记得 linespace 会变（11→17、12→18、13→19）。
