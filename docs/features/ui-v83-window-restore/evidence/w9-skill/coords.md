# W9 — 技能窗 (UISkillBook) 版式取证

Root cause: **RC2**（`try_maple_btn`/`TwoSpriteButton` 缺坐标 → W0.3 后不创建 → tab/宏钮/SP 钮缺失 + 行底/文字偏移）。
Pack: `E:/MXD/BeiDou-Client-xy`。资源根: `UI.nx` → `UIWindow.img/Skill/*`（`UiAssetResolve::window("Skill")`）。

坐标系（batch-conventions §2）：
- `try_maple_btn(...,position,...)` 的 `position` = **贴图左上角**，内部再叠 `normal/0` 的 origin。
- `MapleButton(src,point)` / `TwoSpriteButton(n,s,pos)` 的 `point` 是 **anchor**（`draw = pos - origin`，见 `DrawArgument::get_rectangle` `rlt = pos - center - origin`）。本批所有 tab / BtSpUp / VScr 件 origin=(0,0)，故 **anchor == 左上角**，可直给左上角。
- `recommend[0]` origin=(16,0)，是唯一非零 anchor：绘制传 `左上角 + origin`。

**依据 basis**：①=参考截图实测（`正确客户端截图/打开了S-E-I-K页面.png`，技能窗在右下）；②=`nx_inspect` 节点 W×H+origin（可信，PKG4 向量已内联解码）；③=v83 惯例/烘焙美术推理。

## 窗口原点标定（basis ①）
用 backgrnd(175×289) 对截图做 alpha-mask 模板匹配 → backgrnd 左上角落在屏幕 **(619,327)**。
backgrnd 由 `sprites.emplace_back(ui_backgrnd, Point(1,0))` 画在 `position+(1,0)`、origin(0,0) → **窗口 position(0,0) = 屏幕 (618,327)**。下文所有坐标均为窗口相对值（=传给 draw/按钮的左上角）。

---

## 节点清单 (W×H + origin，全部 basis ②)

| 节点 | 路径 | W×H | origin | 角色 |
|---|---|---|---|---|
| backgrnd | UIWindow.img/Skill/backgrnd | 175×289 | (0,0) | 整窗皮肤（标题"技能"/红下划线/深色书封条/技能点行/滚动槽 均烘焙在内）|
| Tab/enabled\|disabled 0 | .../Skill/Tab/{enabled,disabled}/0 | 10×11 | (0,0) | 新手段图标（圆点 ○）|
| Tab/… 1 | .../{enabled,disabled}/1 | 7×12 | (0,0) | 1 转段图标（\|）|
| Tab/… 2 | .../{enabled,disabled}/2 | 12×12 | (0,0) | 2 转段图标（\|\|）|
| Tab/… 3 | .../{enabled,disabled}/3 | 16×12 | (0,0) | 3 转段图标（\|\|\|）|
| Tab/… 4 | .../{enabled,disabled}/4 | 15×12 | (0,0) | 4 转段图标（\|\|\|\|）|
| skill0 | .../Skill/skill0 | 141×35 | (0,0) | 技能行底（不可加/普通态）|
| skill1 | .../Skill/skill1 | 141×35 | (0,0) | 技能行底（可加态）|
| line | .../Skill/line | 141×1 | (0,0) | 行间分隔线 |
| recommend[0] | .../Skill/recommend/0 | 105×35 | **(16,0)** | 顶部职业横幅（＝用户报「缺失的新手技能图标」）|
| BtSpUp | .../Skill/BtSpUp/{normal,mouseOver,pressed,disabled}/0 | 12×12 | (0,0) | 行内 SP 加点「+」钮（4 态）|
| tip0/tip1/tip2 | .../Skill/tip{0,1,2} | 193×102 | (0,0) | 技能说明底衬（**本窗未用**，见下）|
| CoolTime{0-15} | .../Skill/CoolTime/{0..15} | 32×32 | — | 冷却遮罩（**本窗未用**，见下）|
| BtMacro | **Basic.img**/BtMacro/{normal,…}/0 | 48×18 | (0,0) | 「宏指令」钮（不在 Skill 根下）|
| VScr prev/next/thumb/base | **Basic.img**/VScr/enabled/* | 12×12 | (0,0) | 滚动条（DEFAULT_SILVER=9 越代 → W0.1 钳到 `VScr`，宽 12）|

---

## 布局表 — classic 单列（frame W=175, H=289）

| 控件 | 节点 | 左上角 (x,y) | 推导 | 依据 |
|---|---|---|---|---|
| 技能行起点 `CLASSIC_SKILL_OFFSET` | skill0/skill1 | **(7, 99)** | 行框右缘实测 x=148，宽 141 → 左 7；行框顶实测 y=99（框 y-run 99-133/139-173/179-213/219-253，pitch 40）。旧值 (10,87) 高 12px → 即用户报「贴图/文字偏移」根因 | ① |
| 行分隔线 `LINE_OFFSET` | line | 行起点 +(0,37) | 行高 35，线在下一行前 2px；宽 141 与行对齐 | ①/③ |
| 行内图标/名/等级 | `SKILL_META_OFFSET` | 行起点 +(2,2) | 图标 32×32 左上实测 (9,101)=行(7,99)+(2,2) ✓ | ① |
| SP 加点钮 `CLASSIC_SPUP_OFFSET` | BtSpUp | 行起点 +(125,20) → 首行 **(132,119)** | 「+」框实测左上 (132,119)，行距 40 → 119/159/199/239；x=132-7=125，y=119-99=20。旧值 +(130,4)→(140,91) 偏右下 8/28px | ① |
| tab0（新手） | Tab/*/0 | **(16, 29)** | 圆点图标模板匹配 score 2.4（最强证据），左上 (16,29) | ① |
| tab1 | Tab/*/1 | (51, 28) | 槽中心 21+33·1=54，图标宽 7 → 54-3=51；中心 y=34，高 12 → 28 | ①/③ |
| tab2 | Tab/*/2 | (81, 28) | 中心 87，宽 12 → 81 | ③ |
| tab3 | Tab/*/3 | (112, 28) | 中心 120，宽 16 → 112 | ③ |
| tab4 | Tab/*/4 | (146, 28) | 中心 153，宽 15 → 146 | ③ |
| recommend 横幅 | recommend/0 | 左上 (5, 20)；绘制 anchor = 左上+(16,0) = **(21,20)** | 见下「recommend 说明」 | ②/③ |
| 宏指令钮 | Basic.img/BtMacro | **(121, 265)** | 模板匹配 score **0.0**（像素级完全命中），48×18 | ① |
| 滚动条 x | VScr | **156** | prev/next 钮实测左上 x=156（宽 12 → 右缘 168，落在 175 框内滚动槽）| ① |
| 滚动条 顶/底 | VScr | **100 / 242** | prev(上箭头) 实测 (156,100)、next(下箭头) (156,242)；`Slider` 用 `Range(100,242)`。旧值 Range(99,258) 底越到技能点行 | ① |
| 可滚动行数 | — | CLASSIC_ROWS=4 | 列表区 y99-253（154px）/ 行距 40 → 4 行；`offset+4+shift<=skillcount` 门限 | ① |

### tab 槽位模型（basis ①/③）
新手截图仅 tab0 可见（其余 4 段未解锁，`change_job` 里 `set_active(i<=level)`）。以 tab0 实测中心 x=21 为锚，条带（backgrnd 白区 x5-170，165px）5 等分 → pitch 33，槽中心 21/54/87/120/153；图标按各自宽度居中，竖直中心 y=34。代码在构造期读 `Texture(enabled[tabid]).get_dimensions()` 计算，不硬编码。

### recommend 说明（basis ②/③，**未决**）
`recommend[0]` 是 105×35 的桃色渐变横幅（含拇指向上「推荐」标）。**新手参考帧里它不可见**（模板匹配全窗最高残差 33.7，无命中）——推断原生仅在「有推荐技能」的高转职态或选中态才绘制，新手态被白色 tab 条 + 粉色选中页取代。本批按任务要求「补上」，取烘焙 tab 条区左上 (5,20) 为锚（origin 16,0 → anchor 21,20）。**风险**：新手态会多出一块桃色横幅，与参考帧白/粉条不完全一致；建议 W13 线上抓帧或 IDA `CUISkill` 复核 recommend 的显隐条件与真锚点。

---

## 烘焙件不复画（batch-conventions §3.1）
`Skill/backgrnd` 已烤好：标题「技能」文字、tab 条白底、红下划线、深色书封条（含 tan 描边）、底部「技能点」标签与输入框、滚动槽。→ **禁止**再用 `Text` 重画标题；书封条上仅叠 `bookicon`(JobData 图标, +(11,55)) 与 `booktext`(职业名, +(28,68))，二者非烘焙内容。

## 回退链保留（inventory 已确认）
`main = Skill["main"]`（本包不存在）→ 回退 `main = Skill`；`ui_backgrnd/skill0/skill1/line/Tab/BtSpUp` 的 `main[...] ? : Skill[...]` 回退链全部落到 `Skill[...]`，正确，保留。

## 本包没有 / 未接线（显式列出）
- `Skill/main`、`Skill/skillBlank`、`Skill/skillPoint`、`Skill/macro`、`Skill/BtHyper/BtGuildSkill/BtRide/BtMacro`：**均不存在** → 相关 `try_maple_btn` 不创建（W0.3 语义）。故 SP 编辑面板（`skillPoint` 块，原 184-188 行坐标）与窗内宏面板（`macro` 块，原 211 行坐标）在本包为死代码，其旧坐标无需按新语义重算（永不构造）。
- `Skill/tip0..2`（193×102）：技能说明底衬，**本窗未引用**；技能说明走 `SkillTooltip` 组件（W5/RC3 线），不在本批接线。
- `Skill/CoolTime{0-15}`（32×32）：冷却遮罩，**本窗未引用**（冷却由图标组件层处理），无路径可修。
- `UIWindow.img/Macro`：是宏**编辑窗**根（backgrnd 289×164 + num1/num2 + admin），**无按钮件**；「宏指令」按钮贴图实为 `Basic.img/BtMacro`。按钮点击后打开的独立窗（UIMacro/UISkillMacroEx）才用 `UIWindow.img/Macro` 根 —— 未改其封包/开窗语义（禁止项）。
