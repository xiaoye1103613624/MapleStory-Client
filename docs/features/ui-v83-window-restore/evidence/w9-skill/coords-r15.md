# W9-SKILL r15 复测 — 经典技能窗（175×289）顶部三区标定

参考截图：`evidence/native-refs-r14/3fe4ade2-588d-43a6-be72-9bf32cadba98.png`（756×444，
左=技能窗列表区，右=技能组合栏）。
方法：`E:/project/tmpl_np.py` / `E:/project/tmpl_all.py`（numpy alpha-masked SAD 模板匹配）
+ `E:/project/_r15_ink.py`（暗像素墨迹带）。贴图一律 `tools/nx_inspect.py image` 导出。

## 窗口原点标定

| 锚点 | 截图绝对坐标 | 窗口相对 | 说明 |
|---|---|---|---|
| 行底 `Skill/skill1`（141×35） | (315, 101) | (7, 99) | 与 `CLASSIC_SKILL_OFFSET=(7,99)` 一致 ⇒ **窗口原点 = (308, 2)** |
| 行距 | 101→141→181→221 | 40 | `ROW_HEIGHT=40` ✓ |
| 行右缘 | 315+141 = 456 | 148 | 列表区宽 141 ✓ |

## 顶部 tab 条与书名区

| 件 | 节点 | 尺寸/origin | 窗口相对左上 | 依据 |
|---|---|---|---|---|
| 选中页横幅 | `UIWindow.img/Skill/recommend/0` | 105×35，origin (16,0) | **(4, 22)** | 橙色描边左缘 x4、右缘 x108（宽 105 ✓）、上描边 y22、下描边 y56（高 35 ✓）→ `RECOMMEND_TOPLEFT` |
| 书页图标 | `Skill/000.img/info/icon` | 26×30，origin (-4,-1) | **(13, 25)** | 黑色轮廓左列 323-309=14、上列 27-2=25，轮廓在位图内缩 1px → 位图左上 (13,25) → `BOOK_TOPLEFT`（绘制点再加 `get_origin()`） |
| 书名 | `String/Skill.img/000/bookName` = 冒险的技术 | — | **(5, 70)** | 截图−贴图差分得墨迹 x6..59、y76..85；A11M 字形顶 = 绘制点 + 6 → 绘制点 (5,70) → `BOOK_TEXT_TOPLEFT` |

页码点图标 `Skill/Tab/{enabled,disabled}{0..4}` 仍作可点按钮叠在横幅上
（槽心 x = 21 + 33·i，y = 34）。

**删除的旧实现**：`tabplates/tabplates_dis` 借用了 `UIWindow.img/Item/New/Tab1|Tab0`
（背包分类页签，34×19 粉底白点）——与原生完全不同，是「新手阶段的书籍和标题位置偏了」的一半根因。

## 行内元素（见 w0-icon/coords.md 依据 4）

| 元素 | 行相对 | 我们的常量 | 结论 |
|---|---|---|---|
| 行图标 | +(2,2) | `SKILL_META_OFFSET=(2,2)` + 无 shift | ✓（shift 已删） |
| 技能名墨迹 | y +5..+14、x +41..+93 | `draw(args+(38,-5))` → 基线 +13 | 差 ≤2px，不改 |
| 等级数字墨迹 | y +24..+31、x +40..+44 | `draw(args+(38,13))` → 基线 +31 | ✓ |
| SP「+」钮 | y +20..+31、x +125..+136 | `CLASSIC_SPUP_OFFSET=(125,20)` | ✓ |

## 遗留

- ~~技能点数值行（`splabel`）待下调~~ → **已解**：SP 白框边框 = 窗口 x 61..111、y 265..281，
  数字「0」墨迹 = x 86..91、y 274..282；按 `w0-common/text-metrics.md` 的 A12M 度量
  （墨迹顶 = draw_y + 9、RIGHT 单字符墨迹左 = draw_x − 6）解出绘制点 **(92, 265)**，
  旧值 (111, 258) 把数字甩到框右上角外。
- 绘制顺序：`UIElement::draw_buttons` 在我们的 `draw()` **之后**画按钮，
  所以 tab0 的页码点圆点会压在书页图标上。原生是横幅→点→书页→书名。
  需把书页/书名移到按钮之后（或把 tab 点降级为 sprite），待 R14-2 收尾。
