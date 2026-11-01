# R27 oracle 基准表（原生帧 ↔ 客户帧逐格比对）

这一目录是**验收标准的物理载体**。以前只查「元素在位」，结果一整轮 11 组偏位缺陷全部放过；
从现在起，判据是：**同一个格子在原生帧上的坐标 = oracle，在我们客户帧上的坐标 = 实测，
Δ 超过 tol（默认 2px）就判未修复**。

## 组成

| 文件 | 作用 |
| --- | --- |
| `../tools/layout.py` | 量具本体：锚定 + 探针 + 对账 + 自测（用法见其 docstring） |
| `tmpl/<Window>_<node>.png` | 从 `.nxcache/UI.nx` 导出的位图，做锚点和 `tmpl` 探针用（`nx_inspect.py image` 导，禁止手画）。目录里存的是**导出集**，不等于都在用：例如 `KeyConfig_backgrnd`(629x373)、`SkillMacroEx_backgrnd`(181x303) 比对应原生帧还高，`anchor_in` 判「模板比帧大」直接不可用，留着是给后续裁切/复核当原料，别误当已建成的探针 |
| `<Window>.json` | 该窗口的探针清单 + 已量出的 oracle 数值 |
| `Stat_anchor.png` | `layout.py anchor --png` 的输出样本：绿框 = 量出来的 backgrnd 左上角，给人眼复核用 |
| `origin-rule.md` | **B1a 裁定**：位图 origin/锚点只能有一套语义，含 KeyConfig 的量尺证据；§6 另记一处同根因（底栏快捷格，缺 oracle 故本轮不改） |

## 一条硬规矩：oracle 只能被「量」出来

`oracle` 字段一律由 `layout.py mark <spec> <原生帧>` 写回，**任何人不得手写或照代码抄**。
手写的 oracle 等于把客户端现有的错误常数供认为「正确」，整轮对账就失去意义。

## 坐标系

全部是**窗口相对坐标**：先用窗口自己的 `backgrnd` 位图在整帧里做模板匹配定出左上角（anchor），
探针区域再相对它。所以原生帧与客户帧分辨率不同、窗口在屏幕上摆的位置不同，都不影响对账。

`backgrnd` 比整帧还大时（实测 `KeyConfig/backgrnd` 629x373 vs 截图 629x369）`anchor_in` 直接返回
None，改拿该窗口一颗**小按钮位图**当锚（`BtClose/normal/0` 12x12，agree=1.000），
此时窗口相对坐标允许出现负 x —— 参照点是按钮，不是底图左上角，读数值前先换算
（换算过程与残差见 [origin-rule.md](origin-rule.md) §3、§5）。

锚定成功与否看 **agree（吻合像素比）**，不看 rmse：底衬之上必然还要画按钮/数字/名条，
实测原生 `Stat/backgrnd` 有 21% 像素被属性值覆盖（agree=0.734）却是正确锚点；
按 rmse 设阈值会把好帧误判成「不能用」。反过来 agree 异常低正是 G3 那一类缺陷（同点重复画底）。

## 探针类型

| kind | 量什么 | 用来抓哪类缺陷 |
| --- | --- | --- |
| `tmpl` | 一张 nx 位图在帧里的左上角 | G1 origin/锚点错（图标、面板、按钮画歪） |
| `band` | 某颜色「水平连续段 ≥ min_run」的**整列行号** | 网格尺：行距漂移 / 少一行 / 整体偏上下 |
| `lines` | 把有墨的行聚成**文本行块**，返回每块行首 + 行距 + 墨高 | G2 基线/行距：行距虚高、首行沉出框外 |
| `blob` | 某颜色在区域内的 bbox（带 `row_min`/`col_min` 抗杂散点） | 文字落位：数值有没有落在框内 |

`band` 比的是**整张行列表**，不只是首行：行距漂和少一行是两个独立缺陷，只看首行会漏。
`band` 量不出行距（它返回每一行有墨的行号），所以文本行距必须用 `lines`——
G2 的判据形式就是「行首序列」，`cmp_probe` 对 `rows` 逐元素比，错一行即 Δ=99。

**量不稳就不写 oracle。** 反例：`Stat` 详情面板的描边与文字同为深色，逐行墨量分块
在 min_pix 4 时碎成 6 块墨高 3~6、在 min_pix 6+gap 2 时把相邻两行并成 pitch 36 ——
这种探针写进去只会制造假 FAIL，已删除并在 spec 的 `note` 里记下原因。

## 已建

| 窗口 | 原生帧 | 关键 oracle |
| --- | --- | --- |
| `Stat.json` 角色能力 | `角色能力.png` 428x349 | anchor=(1,2) agree .734；`basicStat` 面板 (8,195) agree 1.000；数值文字 bbox (13,236) 149x43（框内）；**`value_lines` 行首 236/252/268，行距恒 16、墨高恒 11** |
| `Item.json` 背包折叠 | `背包栏（折叠）.png` 179x289 | anchor=(3,0)；**网格分隔行 = 49,83,117,151,185,219（行距恒 34）** |
| `KeyConfig.json` 键盘设置 | `键盘设置.png` 629x369 | anchor=BtClose@(606,5) agree 1.000；**键帽上沿 59/92/125/158/191（行距恒 33、键帽高 32）**；4 颗 `icon` 键帽 + OK 按钮 tmpl 逐格命中现有坐标表 ⇒ 见 [origin-rule.md](origin-rule.md) |
| `SkillMacro.json` 宏指令栏 | `技能展开宏指令.png` 379x291 | 锚故意钉在**左边技能窗** `Skill/backgrnd`@(0,0)（agree .568），这样「宏面板没贴在技能栏上」才是被检对象；量得面板左边界 x=184 而技能窗宽 175 ⇒ **原生贴合标准答案 = 两窗间隙 9px**；宏格行 y=86,131,176（行距恒 45）、三格 `Macroicon/<i>` tmpl 分别落在 (319,56)/(319,101)/(319,146) 且 agree 均 1.000 |

`value_lines` 就是 B1b 的标尺：[Graphics/GraphicsGL.h:247](../../../../../Graphics/GraphicsGL.h#L247)
的 `linespace() = int(height * 1.35 + 1)`（A11M maxrows 12 ⇒ **17**），比原生行距多 1px/行，
且首行墨顶 = `draw_y + linespace − bitmap_top` = `draw_y + 6` —— 「数值偏下、不在框内」两项合成一个根因。
`KeyConfig` 那四格 0 误差则把「按键图片全都偏上」钉死成子层没抹 origin（`KeyConfig/icon/<n>` origin (0,32)）
导致的整层 32px 平移，不是坐标表写错。

## 自测：门禁自己也要有证据

```
python layout.py selftest ../evidence/r27-oracle/Stat.json --probe panel_text_bbox --probe-kind blob  --shifts 1,14,60
python layout.py selftest ../evidence/r27-oracle/Stat.json --probe value_lines     --probe-kind lines  --shifts 1,16,40
python layout.py selftest ../evidence/r27-oracle/Item.json --probe slot_rows       --probe-kind band   --shifts 1,14,34
```

把 oracle 帧里的一块内容整体下移 N 像素，再看 `check` 报出的 Δ 是否等于 N。
三类探针各自的注入档（含按行距取档，如 `lines` 用 16、`band` 用 34）全部逐像素复现 ——
注入 14px 却报 ok 的话，后面 B2..B7 全部作废，所以这一步不是可选的。

`lines`/`band` 的自测搬的是**整个探针区**而不是 oracle 的 bbox：这两类 oracle 里 `w=0`、
`h` 是行数不是像素高度，照 bbox 搬等于没搬（改之前注入 16px 实测报 Δ=0，是个会骗人的假 ok）。
