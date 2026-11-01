# R27 oracle 基准表（原生帧 ↔ 客户帧逐格比对）

这一目录是**验收标准的物理载体**。以前只查「元素在位」，结果一整轮 11 组偏位缺陷全部放过；
从现在起，判据是：**同一个格子在原生帧上的坐标 = oracle，在我们客户帧上的坐标 = 实测，
Δ 超过 tol（默认 2px）就判未修复**。

## 组成

| 文件 | 作用 |
| --- | --- |
| `../tools/layout.py` | 量具本体：锚定 + 探针 + 对账 + 自测（用法见其 docstring） |
| `tmpl/<Window>_<node>.png` | 从 `.nxcache/UI.nx` 导出的位图，做锚点和 `tmpl` 探针用（`nx_inspect.py image` 导，禁止手画）。目录里存的是**导出集**，不等于都在用：例如 `KeyConfig_backgrnd`(629x373)、`SkillMacroEx_backgrnd`(181x303) 比对应原生帧还高，`anchor_in` 判「模板比帧大」直接不可用；`Equip_BtCashshop/BtDetail/BtPet1_normal` 在原生帧上 agree 只有 .216/.000/.322（那帧根本没画这几个按钮），当探针必 MISSING。留着是给后续裁切/复核当原料，别误当已建成的探针 |
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

5 张 spec、29 个探针，`check` 对各自的原生帧全部 **不合格 0**（工具判据：Δ>tol 即 FAIL）。

| 窗口 | 原生帧 | 关键 oracle |
| --- | --- | --- |
| `Stat.json` 角色能力 | `角色能力.png` 428x349 | anchor=(1,2) agree .734；`basicStat` 面板 (8,195) agree 1.000；数值文字 bbox (13,236) 149x43（框内）；**`value_lines` 行首 236/252/268，行距恒 16、墨高恒 11** |
| `Item.json` 背包折叠 | `背包栏（折叠）.png` 179x289 | anchor=(3,0)；**网格分隔行 = 49,83,117,151,185,219（行距恒 34）** |
| `KeyConfig.json` 键盘设置 | `键盘设置.png` 629x369 | anchor=BtClose@(606,5) agree 1.000；**键帽上沿 59/92/125/158/191（行距恒 33、键帽高 32）**；4 颗 `icon` 键帽 + OK 按钮 tmpl 逐格命中现有坐标表 ⇒ 见 [origin-rule.md](origin-rule.md) |
| `SkillMacro.json` 宏指令栏 | `技能展开宏指令.png` 379x291 | 锚故意钉在**左边技能窗** `Skill/backgrnd`@(0,0)（agree .568），这样「宏面板没贴在技能栏上」才是被检对象；量得面板左边界 x=184 而技能窗宽 175 ⇒ **原生贴合标准答案 = 两窗间隙 9px**；宏格行 y=86,131,176（行距恒 45）、三格 `Macroicon/<i>` tmpl 分别落在 (319,56)/(319,101)/(319,146) 且 agree 均 1.000 |
| `Equip.json` 角色装备 | `角色装备.png` 348x305 | anchor=`Equip/backgrnd`@(0,0) agree .884；**装备格网格顶边 32 / 底边 265 ⇒ 233 = 7×33+2，行距恒 33、共 7 行**；粉色宠物格 (72,234) 31x31 与 (6,234) 64x31 各钉一次底行的 x 起点/格宽/格高；宠物面板两条尺互证：`pet` tmpl @(171,123) agree .631 与白 bbox @(181,124)（= art 内框 (10,1) + (171,123)）。**Equip 网格线是 255/238 抖动，没有可用的周期分隔色**（四种灰蓝在 min_run≥20 下全部 0 行），尺只能取自面板白外框 |

`value_lines` 就是 B1b 的标尺：[Graphics/GraphicsGL.h:247](../../../../../Graphics/GraphicsGL.h#L247)
的 `linespace() = int(height * 1.35 + 1)`（A11M maxrows 12 ⇒ **17**），比原生行距多 1px/行，
且首行墨顶 = `draw_y + linespace − bitmap_top` = `draw_y + 6` —— 「数值偏下、不在框内」两项合成一个根因。
`KeyConfig` 那四格 0 误差则把「按键图片全都偏上」钉死成子层没抹 origin（`KeyConfig/icon/<n>` origin (0,32)）
导致的整层 32px 平移，不是坐标表写错。

## 自测：门禁自己也要有证据

```
python layout.py selftest ../evidence/r27-oracle/Stat.json      --probe panel_text_bbox   --probe-kind blob  --shifts 1,14,60
python layout.py selftest ../evidence/r27-oracle/Stat.json      --probe value_lines       --probe-kind lines --shifts 1,16,40
python layout.py selftest ../evidence/r27-oracle/Item.json      --probe slot_rows         --probe-kind band  --shifts 1,14,34
python layout.py selftest ../evidence/r27-oracle/Item.json      --probe meso_digits       --probe-kind blob  --shifts 1,14,60
python layout.py selftest ../evidence/r27-oracle/KeyConfig.json --probe keycap_rows_main  --probe-kind band  --shifts 1,14,34
python layout.py selftest ../evidence/r27-oracle/KeyConfig.json --probe keycap_row2_bbox  --probe-kind blob  --shifts 1,16,32
python layout.py selftest ../evidence/r27-oracle/KeyConfig.json --probe keycap_icon23     --probe-kind tmpl  --shifts 1,14,32
python layout.py selftest ../evidence/r27-oracle/SkillMacro.json --probe panel_border_rows --probe-kind band --shifts 1,14,45
python layout.py selftest ../evidence/r27-oracle/SkillMacro.json --probe macroicon_0       --probe-kind tmpl --shifts 1,14,32
python layout.py selftest ../evidence/r27-oracle/Equip.json     --probe grid_top_edge     --probe-kind band  --shifts 1,14,32
python layout.py selftest ../evidence/r27-oracle/Equip.json     --probe pet_slot_pink_bbox --probe-kind blob --shifts 1,14,32
python layout.py selftest ../evidence/r27-oracle/Equip.json     --probe pet_panel_tmpl    --probe-kind tmpl  --shifts 1,14,32
```

把 oracle 帧里的一块内容整体下移 N 像素，再看 `check` 报出的 Δ 是否等于 N。
**四类探针 × 5 张 spec 共 12 条注入全部逐像素复现**（注入档按各自尺子取，如 `lines` 用行距 16、
`band` 用 34/45、键帽用 32 —— 32 正是要修的那一档缺陷的尺寸）；报 MISSING 也算检出，
那是内容被推出探针区。注入 14px 却报 ok 的话，后面 B2..B7 全部作废，所以这一步不是可选的。

自测本身修掉过三个「会骗人的假 ok / 直接崩」，都记在这儿免得下次再犯：

1. `lines`/`band` 搬的是**整个探针区**而不是 oracle 的 bbox：这两类 oracle 里 `w=0`、
   `h` 是行数不是像素高度，照 bbox 搬等于没搬（改之前注入 16px 实测报 Δ=0）。
2. `tmpl` 同理：oracle 只有一个点，`w=h=0` ⇒ 搬 0 像素，注入 14px 也报 Δ=0；
   现在搬的是**模板自身那一块**（尺寸从 png 读，不写死）。
3. 擦除色必须离探针色足够远（`fill_far`）：`Equip/grid_top_edge` 量的是纯白，
   拿区域中位数（也是白）去擦，擦出来的实色块被当成新内容，注入 1/14/32 三档全报 Δ=-12
   —— 那 12 是纯色块顶到探针区上沿的距离，跟注入量毫无关系。
4. 注入可能顺手把**锚点图**也搬烂（band 要清空整片区域，agree 从 .884 掉到 .77 < 阈值 .80），
   原先 `measure_all` 返回 None、selftest 直接 `AttributeError` 崩掉；
   现在 selftest 传 `force_anchor`（spec 里已记录的锚点）继续量探针，并明打印一句降级说明。
   真实帧的锚定失败仍然是硬失败，不受这条影响。

