# R27 oracle 基准表（原生帧 ↔ 客户帧逐格比对）

这一目录是**验收标准的物理载体**。以前只查「元素在位」，结果一整轮 11 组偏位缺陷全部放过；
从现在起，判据是：**同一个格子在原生帧上的坐标 = oracle，在我们客户帧上的坐标 = 实测，
Δ 超过 tol（默认 2px）就判未修复**。

## 组成

| 文件 | 作用 |
| --- | --- |
| `../tools/layout.py` | 量具本体：锚定 + **锚点审计（`bias`）** + 探针 + 对账 + 自测（用法见其 docstring） |
| `tmpl/<Window>_<node>.png` | 从 `.nxcache/UI.nx` 导出的位图，做锚点和 `tmpl` 探针用（`nx_inspect.py image` 导，禁止手画）。目录里存的是**导出集**，不等于都在用：例如 `KeyConfig_backgrnd`(629x373)、`SkillMacroEx_backgrnd`(181x303) 比对应原生帧还高，`anchor_in` 判「模板比帧大」直接不可用；`Equip_BtCashshop/BtDetail/BtPet1_normal` 在原生帧上 agree 只有 .216/.000/.322（那帧根本没画这几个按钮），当探针必 MISSING。留着是给后续裁切/复核当原料，别误当已建成的探针 |
| `<Window>.json` | 该窗口的探针清单 + 已量出的 oracle 数值 |
| `frames/native_<窗体>.png` | **oracle 帧本体**：原生客户端截图的 ASCII 命名副本，`mark` / `check` 都从这里读。桌面截图换机器/改名就断，spec 里的 `oracle_frame` 一律是仓库相对路径（旧 spec 曾写 `C:\Users\...\正确客户端截图\角色能力.png`，换机器即失效） |
| `Stat_anchor.png` | `layout.py anchor --png` 的输出样本：绿框 = 量出来的 backgrnd 左上角，给人眼复核用 |
| `origin-rule.md` | **B1a 裁定**：位图 origin/锚点只能有一套语义，含 KeyConfig 的量尺证据；§6 另记一处同根因（底栏快捷格，缺 oracle 故本轮不改） |

## 一条硬规矩：oracle 只能被「量」出来

`oracle` 字段一律由 `layout.py mark <spec> <原生帧>` 写回，**任何人不得手写或照代码抄**。
手写的 oracle 等于把客户端现有的错误常数供认为「正确」，整轮对账就失去意义。

配套一条：**原生帧先拷进 `frames/` 再 `mark`**（文件名 ASCII，中文名在 Git Bash 里会被转码，
`cp` 之后 md5 对一次原图）。`mark` 现在把落在仓库内的帧存成仓库相对路径，
所以「从干净 clone 复现 oracle」是跑得通的 —— 7 张 spec 各自 `check` 自己的 oracle 帧
全部 **不合格 0**（2026-11-01 复跑，见下表）。

再配套一条：**每张 spec 建好后先跑 `layout.py bias <spec>` 审锚点**，确认记录锚点是 meanSAD
最优格才允许写 oracle。锚点没审就对账 = 拿一把可能整体偏 1px 的尺子量所有格子，
量出来的「缺陷」是门禁的，不是客户端的 —— 这轮就在 Item 上真的偏了 1px，见下节。

## 锚点自己也是量出来的：紧裁帧的 Δ=1 系统性伪影（2026-11-01 抓到并修掉）

`oracle` 全是**相对锚点**的坐标，所以锚点偏 1px ⇒ **整张 spec 每一格齐刷刷偏 1px**，
而且是三个不同窗口同时偏 —— 看着像「客户端有一处公共件低了一格」，实际是门禁在骗人。

成因：`anchor_in` 硬性要求模板整张落在帧内。用户给的原生截图是**紧裁**的，
`背包栏（折叠）.png` 高 289 == `Item/backgrnd` 图高 289 ⇒ 竖直方向只有 dy=0 这一个合法解，
**锚点的 y 是被尺寸约束强制出来的，不是内容对上的结果**。

`layout.py bias <spec>` 就是专门审它的：允许模板出界，只比重叠的实心像素，
在记录锚点周围扫 ±2 的 meanSAD / agree 网格，最优格不是记录格就报「锚点应为 (x,y)」并给出 `mark --anchor` 那行命令。

`Item` 审出来的两条独立证据（互相印证，不靠单把尺子）：

| 证据 | 结果 |
| --- | --- |
| `bias` meanSAD 网格 | dy=-1 → **9.77 / agree 0.913**；dy=0（记录值）→ 23.37 / 0.792；dx=±1 → 20.6 ⇒ 最优 (3,-1) 且分得很开 |
| 拿美术 PNG 自己的像素当尺 | 金币白框上边框行：`Item_backgrnd.png` 自己=**252**、真机帧窗口相对=**252**、原生帧=**251** ⇒ 原生帧的 y0 是美术的第 **1** 行，裁掉了第 0 行 |

改锚 `mark --anchor 3 -1` 后：oracle 从 (131,268)/行 49 变 (131,269)/行 50，
**真机帧逐格 Δ=0**。也就是说 B1b 改的金币常量与格子行距本来就对，那 1px 全是门禁伪影；
如果不先审锚点，B3–B7 会拿着这把偏 1px 的尺子去「修」七个本来没错的窗口。

顺带修掉两个由负锚点暴露的工具 bug（都在 `layout.py`）：

1. `check` / `selftest` 之前**重新自动锚定**，等于在 oracle 帧上把 `mark` 的校正抹掉
   （改锚后 `check` 对自己的 oracle 帧反而报 Δ=1）。现在 `recorded_anchor_if_oracle()`
   判「这张帧就是 spec 的 oracle 帧」⇒ 直接用记录的锚点；真机帧窗口位置未知，照旧自动锚。
2. 锚点带负分量后，`selftest` 的注入框 `src[-1:288]` 是**空切片** ⇒ median 出 NaN、
   `fill_far` 直接 `ValueError` 崩。现在注入框一律先钳进帧内，钳空则明确报错而不是崩。

其余 6 张 spec 的 `bias` 全过（记录锚点即最优格）：Stat 26.61、SkillMacro 34.39、
Equip 11.60、ChatBar 20.56 都在 dy=0 取最小，KeyConfig / EquipTooltip 记录格 meanSAD=**0.00**
（小按钮当锚，逐像素全等）。**只有 Item 这一张是紧裁等高的受害者。**

## 形态（mode）：同一扇窗的多种开法不能互相判死

有的探针只在某一种窗口形态下存在：能力窗的「基本属性点」浮层只在**详情态**画，
宏面板只在**技能窗 + 宏指令同开**时画。这类探针给 `"active_in": ["<形态>"]`，
`check ... --mode <形态>` 只判该形态下应当存在的格子，其余打 `SKIP(不在 X 形态)`
且**不计入不合格**；`mark` / `selftest` 不看 mode（`mode=None` 一律全量照量，
因为 oracle 取自内容齐全的那张原生帧）。

判据是「这一格在这一形态下有没有东西可量」，不是「像不像」—— 少一个 `--mode`
就会把「本轮没开那个面板」误报成版式缺陷，反之全塞进 default 就永远量不到宏面板。

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

7 张 spec、44 个探针，`check` 对各自入库的 oracle 帧全部 **不合格 0**（工具判据：Δ>tol 即 FAIL），
且**每张都过了 `bias` 锚点审计**（见上一节 —— Item 那张原本偏 1px，已校正后重写 oracle）。
其中 **Stat / Item / SkillMacro 三张已经用真机客户帧跑过 `check`**（B1b+B2，2026-11-01）：
Item 三格 **Δ=0**（锚点校正前恒报 Δ=1，那是尺子的问题），Stat/SkillMacro 逐格 Δ ≤ 1px，
明细见 [../w0-common/text-metrics.md](../w0-common/text-metrics.md) §4。

| 窗口 | 原生帧 | 关键 oracle |
| --- | --- | --- |
| `Stat.json` 角色能力 | `frames/native_stat.png` 428x349 | anchor=(1,2) agree .734；`basicStat` 面板 (8,195) agree 1.000；数值文字 bbox (13,236) 149x43（框内）；**`value_lines` 行首 236/252/268，行距恒 16、墨高恒 11**；`value_ink_tops` 上半区 7 行墨顶 36/53/82/118/136/154/172（B1b 定版尺，真机 Δ≤1）；`backgrnd2_detail_panel` 带 `active_in: ["detail"]` |
| `Item.json` 背包折叠 | `frames/native_bag_fold.png` 179x289 | **锚点=(3,-1)，不是自动量出的 (3,0)** —— 见下节；校正后**网格分隔行 = 50,84,118,152,186,220（行距恒 34）**，与真机帧逐格 Δ=0；`meso_digits` 金币带 (8,266) 167x14；**`meso_digit` 单个「0」纯墨迹 (131,269) 6x9 —— 夹在白框上下边框（y266/281）之间，金币常量 269/137 就是它定的** |
| `KeyConfig.json` 键盘设置 | `frames/native_keyconfig.png` 629x369 | anchor=BtClose@(606,5) agree 1.000；**键帽上沿 59/92/125/158/191（行距恒 33、键帽高 32）**；4 颗 `icon` 键帽 + OK 按钮 tmpl 逐格命中现有坐标表 ⇒ 见 [origin-rule.md](origin-rule.md) |
| `SkillMacro.json` 宏指令栏 | `frames/native_skill_macro.png` 379x291 | 锚故意钉在**左边技能窗** `Skill/backgrnd`@(0,0)（agree .568），这样「宏面板没贴在技能栏上」才是被检对象；量得面板左边界 x=184 而技能窗宽 175 ⇒ **原生贴合标准答案 = 两窗间隙 9px**；宏格行 y=86,131,176（行距恒 45）、三格 `Macroicon/<i>` tmpl 分别落在 (319,56)/(319,101)/(319,146) 且 agree 均 1.000。**面板那 6 格全带 `active_in: ["macro"]`**（技能窗单开时它们不存在）；`sp_digit` 不带 = 技能窗本体，量到 SP 白框里的「0」纯墨迹 (103,268) 6x9 —— 技能点常量 268/109 由它定 |
| `Equip.json` 角色装备 | `frames/native_equip.png` 348x305 | anchor=`Equip/backgrnd`@(0,0) agree .884；**装备格网格顶边 32 / 底边 265 ⇒ 233 = 7×33+2，行距恒 33、共 7 行**；粉色宠物格 (72,234) 31x31 与 (6,234) 64x31 各钉一次底行的 x 起点/格宽/格高；宠物面板两条尺互证：`pet` tmpl @(171,123) agree .631 与白 bbox @(181,124)（= art 内框 (10,1) + (171,123)）。**Equip 网格线是 255/238 抖动，没有可用的周期分隔色**（四种灰蓝在 min_run≥20 下全部 0 行），尺只能取自面板白外框 |
| `ChatBar.json` 聊天栏展开 | `frames/native_chatbar_open.png` 569x103 | 底图 `base/backgrnd`(817x71) 与 `backgrnd2`(570x71) 都比这张裁图宽 ⇒ `anchor_in` 判模板过大，按降级规则改锚 `base/chatTarget`(81x20) @(1,79) agree .897，**参照点是那颗蓝框左上角，消息区在它上方 ⇒ 相对 y 为负是合法的**。`msg_stack_lines` 量到两条消息墨块 47..58 / 60..71 ⇒ **消息堆叠行距 13、墨高恒 12**；同一对行在 `打开了S-E-I-K页面.png`(1280x720) 的绝对 y=625/638 复量还是 13。聊天日志区没有底衬，字压在地图上，这条尺天生比带面板的窗不稳，复核先看 `_debug` 图 |
| `EquipTooltip.json` 装备悬浮提示 | `背包中鼠标悬浮物品上显示的物品信息内容.png` 268x310（裁 y≥90 → `frames/EquipTooltip_native_crop.png`） | tooltip 各位图全是纯白 ⇒ 全帧模板匹配 SSD=0 命中 90 处（背包标题白条），**必须先无损裁掉上方无关区域**才能锚；锚 `ToolTip/Equip/Can/reqLEV` @(124,27) agree 1.000。`body_stat_lines` 四行 128/143/158/173 ⇒ **属性块行距恒 15**；`req_band_str`/`req_band_pop` 两条 tmpl 互证需求块**位图原点行距恒 12**（零抖动），而 `req_label_lines` 墨顶 11/12/13 抖 ⇒ 那是底图块不是文字行 |

`value_lines` 那 16 **不是**引擎行距，是能力窗下半区（力量/敏捷/智力/运气）自己的行带；
上半区基础信息四行（HP/MP/EXP/人气度）实测墨顶 122/140/158/176 ⇒ 行距 18，
聊天消息堆叠 13、tooltip 属性块 15、NPC 对话与任务描述折行 18 —— **原生根本没有一套
按字号统一的行距**，行距是每窗自己的常量。所以 `linespace()` 只当「折行步进」用
（A12M 实测 18 与现公式吻合），窗口要更密的行就用 `Text` 构造函数的 `line_adj` 收窄。
真正被统一到一处的是**墨迹顶换算**：[Graphics/Text.h](../../../../../Graphics/Text.h) 的
`Text::y_for_ink_top(ink_top)`（`OutlinedText` 转发），它按**当前字符串**逐字形取最大
`bitmap_top`（`GraphicsGL::glyph_bt`，与绘制路径同一个 `FT_LOAD_RENDER` 口径）。
中途做过 `InkKind{SHORT,TALL}` 两档常量的方案，实测挡不住：SimHei 11px 的 CJK 本身分
9/10 两档（170 字抽样 126:42），「新手」顶 10 而「50 / 50」顶 9 ⇒ 选类别必错 1px，
所以定版是按串精算。r15 手抄的「CJK 11」是 PIL 口径，FreeType 实测 9~10，
能力窗/底栏名条/金币/聊天目标字因此一致偏低 2~3px
（定版模型与逐站点差值见 [../w0-common/text-metrics.md](../w0-common/text-metrics.md)）。
`KeyConfig` 那四格 0 误差则把「按键图片全都偏上」钉死成子层没抹 origin（`KeyConfig/icon/<n>` origin (0,32)）
导致的整层 32px 平移，不是坐标表写错。

## 自测：门禁自己也要有证据

```
python layout.py selftest ../evidence/r27-oracle/Stat.json      --probe panel_text_bbox   --probe-kind blob  --shifts 1,14,60
python layout.py selftest ../evidence/r27-oracle/Stat.json      --probe value_lines       --probe-kind lines --shifts 1,16,40
python layout.py selftest ../evidence/r27-oracle/Item.json      --probe slot_rows         --probe-kind band  --shifts 1,14,34
python layout.py selftest ../evidence/r27-oracle/Item.json      --probe meso_digits       --probe-kind blob  --shifts 1,14,60
python layout.py selftest ../evidence/r27-oracle/Item.json      --probe meso_digit        --probe-kind blob  --shifts 1,14,60
python layout.py selftest ../evidence/r27-oracle/SkillMacro.json --probe sp_digit         --probe-kind blob  --shifts 1,14,60
python layout.py selftest ../evidence/r27-oracle/KeyConfig.json --probe keycap_rows_main  --probe-kind band  --shifts 1,14,34
python layout.py selftest ../evidence/r27-oracle/KeyConfig.json --probe keycap_row2_bbox  --probe-kind blob  --shifts 1,16,32
python layout.py selftest ../evidence/r27-oracle/KeyConfig.json --probe keycap_icon23     --probe-kind tmpl  --shifts 1,14,32
python layout.py selftest ../evidence/r27-oracle/SkillMacro.json --probe panel_border_rows --probe-kind band --shifts 1,14,45
python layout.py selftest ../evidence/r27-oracle/SkillMacro.json --probe macroicon_0       --probe-kind tmpl --shifts 1,14,32
python layout.py selftest ../evidence/r27-oracle/Equip.json     --probe grid_top_edge     --probe-kind band  --shifts 1,14,32
python layout.py selftest ../evidence/r27-oracle/Equip.json     --probe pet_slot_pink_bbox --probe-kind blob --shifts 1,14,32
python layout.py selftest ../evidence/r27-oracle/Equip.json     --probe pet_panel_tmpl    --probe-kind tmpl  --shifts 1,14,32
python layout.py selftest ../evidence/r27-oracle/ChatBar.json   --probe msg_stack_lines   --probe-kind lines --shifts 1,13,32
python layout.py selftest ../evidence/r27-oracle/EquipTooltip.json --probe body_stat_lines   --probe-kind lines --shifts 1,15,32
python layout.py selftest ../evidence/r27-oracle/EquipTooltip.json --probe req_label_lines   --probe-kind lines --shifts 1,12,32
```

把 oracle 帧里的一块内容整体下移 N 像素，再看 `check` 报出的 Δ 是否等于 N。
**四类探针 × 7 张 spec 共 17 条注入全部逐像素复现**（注入档按各自尺子取，如 `lines` 用该窗
自己量到的行距 12/13/15/16、`band` 用 34/45、键帽用 32 —— 32 正是要修的那一档缺陷的尺寸）；
报 MISSING 也算检出，那是内容被推出探针区。注入 14px 却报 ok 的话，后面 B2..B7 全部作废，
所以这一步不是可选的。

锚点审计（上一节）之后这条自测**变严了一档**，值得记下来：17 条注入报出的 Δ 现在**精确等于注入量**
（`shift=1 → Δ=1`、`shift=34 → Δ=34`）。改锚之前，Item 的 `shift=1` 实测报 Δ=0 ——
那 1px 不是注入没生效，是测量锚点与 mark 锚点不一致把偏移吃掉了；
`selftest` 现在用 `anchor_override` 直接采用记录锚点，不再重新自动锚定，才算得准。

唯一不能自测的探针：`EquipTooltip/name_line` —— 把它下移 15px 等于把**纯白**文字复制进锚点搜索区，
那颗同样纯白的 `reqLEV` 模板会抢走锚点（agree .969），量到的 Δ 是锚点漂移不是注入量。
这条尺只能靠 `check` 对帧，改客户帧时先看锚点 agree 再信它的 Δ。

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

## 第九张 spec：MonsterBook（2026-11-02 建）与两张卡住的窗

`MonsterBook.json` 5 探针、`check` 对自己的 oracle 帧不合格 0：锚 `backgrnd`(475x349)@(34,8)
agree .767，`BtClose` 与 `RightTab0_selected` 两把尺 agree **1.000 / rmse 0.00**，
`infoPage` .916、`cardSlot` .724，`label_lines` 五行墨顶 `[112,165,196,217,253]`。
`cover`(31x42 origin(2,40)) 在本帧 agree **0.000**（那格红卡预览不是 cover 节点）、
`RightTab1..3` 的 normal 位图 agree 仅 .343~.371（原生这帧只有 tab0 是 selected，且标签含烘焙中文）
—— 三者一律**不写 oracle**，png 留在 `tmpl/` 当后续复核原料。

### 目录（GameMenu）：现有帧量不出尺，等留边截图

`目录按钮点击后的列表.png` 只有 **82x182**，比 `GameMenu/backgrnd`(93x140) 还窄 ⇒
`anchor_in` 判「模板过大」直接拒。更关键的一条是**美术对不上**：`GameMenu/backgrnd` 的
不透明像素众数色是深灰 `(17,17,17)` + 白，而帧上面板实测是蓝 `(0,153,238)/(0,102,170)`
—— 原生这帧的底色来自 81x25 的按钮位图本身，不是 backgrnd。可按钮位图**不含烘焙的中/英文文字**
（帧上每颗都是「频道变更 / CHANNEL CHANGE」两行），拿它当模板匹到帧上 agree 只有 **.712**，
低于任何可用阈值。裁 backgrnd 左侧 60 列再试 agree .065~.195、rmse ~253，同样废。
结论：**不换帧就没有尺**，已请用户补一张四周留边的完整目录截图；在那之前 B6 的「目录列表位置不对」
只能按代码语义推，不进门禁。

### 游戏设置 / 系统设置：原生帧缺失

桌面 57 张原生帧里没有这两张窗（`GameOpt` 只有 `backgrnd 158x284`、`SysOpt` 只有 `backgrnd 299x365`
的资产取证来自 nx，不是截图）。B7 的「二合一 + 无勾选框」可以照资产与 `Basic.img/CheckBox` 修，
但**版式对不对没有 oracle**，属「只能用户取证」。

