# 公共件：文字落位度量模型（r15 建立 → r27-B1b 定版）

**为什么单独一篇**：W1–W13 里反复出现「数值/标题位置偏了几像素」，根因不是坐标常量本身，
而是**配准错**（截图 ↔ 贴图没对齐就量墨迹）+ **把字形顶当成绘制点**。
本篇给出可复用的两步法，后续所有文字落位都按它解，不再逐窗猜。

> **r27-B1b 已把第 2 步的「查表」环节整体作废**：下面 r15 那张 `bitmap_top` 表用的是
> PIL 口径，与客户端真正用的 FreeType 口径差 2~3px。新定版见下一节，
> 老表只保留「同一列里不同字形的墨迹顶不是一个值」这个结论。

## r27-B1b 定版：度量归渲染器，窗口只记「墨迹顶」

### 1）`bitmap_top` 有两个口径，只有 FreeType 那个是客户端用的

渲染器 `GraphicsGL.cpp` 的 `char_y = y + ay - ch.bt`，`ch.bt` 来自
`FT_Load_Char(..., FT_LOAD_RENDER)` 之后的 `face->glyph->bitmap_top`。
r15/r27 之前各处注释里的 11/12 是用 **PIL** 量同一个 ttf 得到的，两者不等。
用 freetype-py 按渲染器完全相同的调用序列实测
（`fonts/SimHei/simhei.ttf`，脚本 `E:/project/_r27_font_probe.py`
+ `E:/project/_r27_word_bt.py`，输出 `_r27_font_probe.txt` / `_r27_word_bt.txt`）：

| size | ASCII maxrows | `linespace()` = int(maxrows×1.35+1) | `'0'` | `'A'` | `'/'` | `'('` | CJK 众数 | CJK 最大 |
|---|---|---|---|---|---|---|---|---|
| 11 (A11M) | 12 | **17** | 8 | 8 | 9 | 9 | 9 | **10** |
| 12 (A12M) | 13 | **18** | 9 | 9 | 9 | 10 | 10 | 10 |
| 13 | 14 | 19 | 10 | 10 | 10 | 11 | 11 | 11 |
| 14 | 15 | 21 | 10 | 10 | 11 | 11 | 12 | 12 |
| 15 | 15 | 21 | 11 | 11 | 12 | 12 | 13 | 13 |
| 18 | 19 | 26 | 13 | 13 | 14 | 15 | 15 | 15 |

⇒ r15 记的「`/` 11、CJK 11」在 11px 下实际是 **9~10**，凡按它反解的行都低 2~3px。

### 2）CJK 在 11px 分两档 ⇒ 「按行类别选常量」必错 1px

170 个常用字抽样（size 11）：`bitmap_top` = 9 的 126 个、= 10 的 42 个。
词级更明显：`新手` = 新**10** + 手9、`等级` = 等**10** + 级9、`人气度` 含 10，
而 `战士`/`能力`/`经验值` 全是 9。一行的墨迹顶 = 基线 − **本行最高字形**的 `bitmap_top`，
所以「CJK 行」不是一个值。曾经做过 `InkKind{SHORT,TALL}` 两档常量，实测挡不住：

- 选 9 ⇒ 「新手」这类含 bt=10 的行高 1px（能力窗职业行）；
- 选 10 ⇒ 「50 / 50」这类 ASCII 升部只有 9 的行低 1px。

⇒ 结论：**换算必须按当前字符串精算**，任何「类别常量」都是错的抽象。

### 3）唯一真源 API（窗口侧只允许记「量到的墨迹顶」）

```cpp
// Graphics/Text.h —— 实例方法：自己知道自己的 font 和 text
static int16_t Text::linespace(Font font);   // 只管换行推进，不是行距常量
int16_t Text::ink_offset() const;            // = linespace - max(bitmap_top over 本串)
int16_t Text::y_for_ink_top(int16_t ink_top) const;   // 墨迹顶 → Text::draw 的 y
```

- 后端：`GraphicsGL::get_ink_offset(Font, const std::string&)`，逐码点走
  `GraphicsGL::glyph_bt()` —— ASCII 直接读 `chars[]`，CJK 先查已上传的 `wide` 缓存
  （保证「量到的 == 画出来的」），未命中时 `FT_LOAD_RENDER` 现场量一次并存进
  `Font::bt_probe`（**不写图集**，所以 `lock()` 期间照样可用）。
- 空串退回 `Font::bt_short`（= `'0'` 的 `bitmap_top`，`addfont` 现场探测）。
- `OutlinedText::y_for_ink_top()` 转发 `inner`。
- 名牌底框（`Background::NAMETAG`）盒顶改**逐行**取本行最大 `bitmap_top`
  （原来是 `- linespace + 6` 的写死假定）。

复算脚本 `E:/project/_r27_ink_check.py`（输出 `_r27_ink_check.txt`），与 C++ 同口径。

### 4）已迁站点（B1b 定版：常量 = 原生帧量到的墨迹顶，Δ = 真机帧门禁实测）

**这一版和 r15 那一版的区别**：r15 的「墨迹顶」是拿**我们自己客户端的截图**反推的
（截图里数到几就写几），等于把自己的错误常数供成标准，越修越偏；B1b 起每个常量都必须
① 出自 `evidence/r27-oracle/frames/` 里的**原生帧**、② 由 `layout.py mark` 写进 oracle、
③ 再用**真机帧** `layout.py check` 量出 Δ 才算完。下表最后一列就是 ③ 的原始读数。

| 站点 | 字体 / 对齐 | 常量（窗口相对） | oracle（原生帧） | 真机实测 | Δ | 旧值 → 结论 |
|---|---|---|---|---|---|---|
| `UIStatsInfo` 数值列 7 行 | A11M LEFT | 行墨迹顶 36 / 53 / 82 / 118 / 136 / 154 / 172 | `Stat.json/value_ink_tops` 同上，pitch=[17,29,36,18,18,18] | rows=[36,54,83,119,137,155,172] | 行首 Δmax=1 | r15 只量到「等级/人气度」两行，其余 5 行当时打 `INK_TOP_NONE` 退回原 y；现在 7 行全部有 oracle |
| 〃 数值列水平 | A11M | 列左 x=61 | `value_col_bbox` (61,36) 36x145 | (61,36) 41x144 | Δ=0 | 61 正确（w9 那批 x=67 的读法是错的） |
| 〃 下部 AP 三行 | A11M | 行墨迹顶 236 / 252 / 268 | `value_lines` 同上，ink=11 | 完全一致 | Δ=0 | — |
| `UIStatusBar` 职业名 | A12M | 墨迹顶 40 | `frames/native_bottom_bar.png` 逐行核对 | — | — | **未改**：40 经原生帧确认正确，r15 的怀疑不成立 |
| 〃 角色名 | A12M | 墨迹顶 56 | 同上 | — | — | **未改**：同上 |
| `UIItemInventory` 金币 | A11M RIGHT | 墨迹顶 **269**、右缘 **x137** | `Item.json/meso_digit` (131,269) 6x9（锚点经 `bias` 校正为 (3,-1) 后与真机逐格 Δ=0；旧 oracle (131,268) 是紧裁帧锚点伪影，见 [../r27-oracle/README.md](../r27-oracle/README.md) 锚点一节） | (131,269) 5x8 | Δ=0 | 旧 (141, **284**) ⇒ 低 16px，数字整条掉到白框外，就是用户报的「金币值位置偏下了，没有在框内」 |
| `UISkillBook` SP | A12M RIGHT | 墨迹顶 **268**、右缘 **x109** | `SkillMacro.json/sp_digit` (103,268) 6x9 ⇒ 右缘 109 | (102,269) 5x8 ⇒ 右缘 107 | Δ=1（墨宽/墨高各差 1，非锚点：该 spec 锚点 (0,0) 已过 `bias`） | 旧 (92, **274**) ⇒ 低 6px、左 17px。**残留 2px 右缘缺口**（109 vs 107）归 B4/#84：A12M 的 RIGHT 里 x 到底是墨迹右缘还是 advance 右缘，见 [../../notes/open-issues.md](../../notes/open-issues.md) B15 的复测配方 |
| `UIChatBar` 目标文字 | A11M LEFT | 墨迹顶 8 | `frames/native_chatbar_fold.png` 逐行核对 | — | — | **未改**：8 经原生帧确认正确 |

**跨窗互证的「白框尺子」**（这两个常量不是各调一次凑出来的，是同一把尺）：
`Item/backgrnd` 金币白框上边框 = 窗口 y266，原生数字顶 = 269（+3）；
`Skill/backgrnd` SP 白框上边框 = y265，原生数字顶 = 268（+3）。
两张独立原生帧给出同一个 +3，且折叠/展开两种背包形态扣掉各自窗口偏移后都回到 269。

**残差怎么读**：Δ=1 与「实测高 8 / 原生高 9」是 SimHei 的字形高度比原生位图字库矮 1px，
属字体层不是落位层（tol=2 内判 ok）。要压到 0 得换字库，那是 #80 之后的事。

**门禁跑法**（本轮三张真机帧全绿）：

```bash
python docs/features/ui-v83-window-restore/tools/layout.py check \
    docs/features/ui-v83-window-restore/evidence/r27-oracle/Item.json <客户帧>
#   -> 不合格 0 / 3
python ... layout.py check .../Stat.json <客户帧> --mode default        # 0 / 8（跳过 1）
python ... layout.py check .../SkillMacro.json <客户帧> --mode default  # 0 / 1（跳过 6）
```

### 5）行距（基线推进）不是全局常量

原生 4 张帧各自量到的基线间距：**聊天气泡叠行 13**（`ChatBar.json/msg_stack_lines`）、
**装备 tooltip 属性块 15**（`EquipTooltip.json/body_stat_lines`）、
**能力窗下部属性带 16**（`Stat` 行带）、**真换行的流式文本 18**
（NPC 对话、任务段落 —— A12M 的 `linespace()`=18 恰好吻合，故不改）。
⇒ `linespace()` 只负责换行推进；逐窗密度是构造参数 `line_adj` 的活，属 B2–B7。
A11M 没有任何原生换行帧背书，所以它的 17 不动（动了就是编数据）。

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

> **r27-B1b：这一步的换算已经不再手算**。公式仍然成立，但 `bitmap_top` 一律由
> `Text::y_for_ink_top()` 现场取，禁止把下面的数字抄进窗口常量（下面这张表已作废，
> 只留作 r15 当时为什么错的证据）。

渲染器 = SimHei（`Configuration.h:518` `font.path.normal = fonts/SimHei/simhei.ttf`），
`addfont(FONT_NORMAL_STR, A11M, 0, 11)` / `A12M, 0, 12`（`GraphicsGL.cpp:237-239`）。

```
linespace = int(maxrows * 1.35 + 1)        // GraphicsGL.h:247，maxrows = ASCII 32..127 最大位图行数
基线      = draw_y + linespace             // LayoutBuilder::add_line 的 line_y = ay
墨迹顶    = draw_y + linespace - bitmap_top// GraphicsGL.cpp:1386  char_y = y + ay - ch.bt
墨迹左    = draw_x + line_x + bitmap_left  // line_x：LEFT 0 / CENTER -adv/2 / RIGHT -adv
```

SimHei 逐字形 `bitmap_top`（**PIL 同字体同像素实测，脚本 `E:/project/_r15_font2.py`，
输出 `_r15_font2.txt`** —— 与渲染器不同口径，r27 已改判，见上面定版表）：

| 字形类别 | A11M（size 11，linespace **17**） | A12M（size 12，linespace **18**） |
|---|---|---|
| 数字 `0-9` | top 8 → 墨迹顶 = **draw_y + 9**，高 9 | top 9 → **+9**，高 10 |
| `/` | ~~top 11~~ → **+6**，高 12 | ~~top 12~~ → **+6** |
| CJK（`技`/`名`…） | ~~top 11~~ → **+6**，高 12 | ~~top 12~~ → **+6** |
| `-` | top 4 → **+13** | top 4 → **+14** |
| 推进量 | 数字 6、CJK 11 | 数字 6、CJK 12 |

**关键推论（仍然有效）：同一列里「纯数字」和「含 `/` 或 CJK」的墨迹顶不是同一个值**
（r27 实测差 1~2px，不是 r15 说的 3px）。所以不能对一列数值套同一个 y 偏移 ——
这正是 R14-1 第一版「统一 +3」把等级/人气度画低的原因。

## 应用实例

> ⚠ 下面两张表的「解出 draw_y」是 r15 按 PIL 口径算的，r27-B1b 定版表已给出更正值
> （多数行 −2/−3px）。保留原样是为了留下「当时怎么错的」证据，**不要照抄**。

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
4. **只把量到的墨迹顶写进窗口常量**，绘制点交给 `Text::y_for_ink_top(ink_top)`（构造 `Text` 时就用该行的最终字符串，
   或像 `UIStatsInfo`/`UIStatusBar` 那样在 draw 里边 `change_text` 边换算）。**不要**再手算 `linespace - bitmap_top`。
5. 横向仍按 `墨迹左 = draw_x + line_x + bitmap_left` 手解（r27 只统一了纵向）。
6. 想要独立复核 C++ 的换算，跑 `E:/project/_r27_ink_check.py`（同 `FT_LOAD_RENDER` 口径）；
   字体度量总表用 `E:/project/_r27_font_probe.py` / `E:/project/_r27_word_bt.py` 重生成。
7. 多行/换行文字的 `y` 反解取的是**整段最大** `bitmap_top`，只对单行精确；多行段落请量首行。
