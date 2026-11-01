# W10 `UIQuestLog` 版式还原坐标表（v83 / gms083 / 1280x720 / data.format=img）

取证口径：
- 节点与 `WxH`/`origin`：`python tools/nx_inspect.py list "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx" <depth> UIWindow.img/Quest`
  → 原始转储 `_nx_quest_list.txt`（原始读数未归档）
- 位图识字（含 Q4 判据）：本目录导出的 PNG，拼合图 `w10-buttons.png`（6 颗 `Bt*`）、`w10-strips.png`（`basic/prob/reward/select/summary/notice*/notice{0-2}`）、
  `w10-icons2.png`（`icon0..icon9` 首帧 + `BtHide/BtHide2` + `notice/0`）、`w10-close-candidates.png`（4 颗候选关闭钮）
- 参考截图 1:1 裁切：`pngs/ref-quest-full.png`（**548x396 = 整窗**，左板 x0..244、右板 x243..547）；
  度量转储 `_measure_ref.txt`（原始读数未归档）、[_measure_geom.txt](_measure_geom.txt)、[_measure_geom2.txt](_measure_geom2.txt)
- 依据列编号 = batch-conventions §3：①烤进底板不重画 ②角锚算术 ③截图实测

## 1. 窗体骨架

| 控件 | 节点路径 | WxH + origin | 收缩态左上 | 展开态左上 | 依据 |
| --- | --- | --- | --- | --- | --- |
| 左板底 | `UIWindow.img/Quest/backgrnd` | 245x396 + (0,0) | (0,0) | (0,0) | ①「任务」标题、列表区、底带、y41..43 红线全烤在图里（`backgrnd.png` 已核） |
| 右板底 | `UIWindow.img/Quest/backgrnd2` | 305x396 + (0,0) | 不绘制 | (243,0) | ③接缝：左板右边框 x241、右板左边框 x243 → `bg_right_pos = left_width-2`；整窗 245+305-2=548 与截图同宽 |
| 整窗 `dimension` | — | — | 245x396 | 548x396 | ②按 `collapsed` 反算，`set_collapsed()` 里同步 |

## 2. 标题条按钮（角锚 k=7）

| 控件 | 节点路径 | WxH + origin | 收缩态左上 | 展开态左上 | 依据 |
| --- | --- | --- | --- | --- | --- |
| 关闭 | `Basic.img/BtClose`（回退 `UIWindow.img/BtUIClose` → `Basic.img/BtClose2`） | 12x12 + (0,0) | (226,5) | (226,5) | ②`x = 245-12-7`；③实测蓝芯 (68,204,238) x227..236 y6..15 |
| 展开/收缩 | `Basic.img/BtHide` | 13x12 + (0,0) | (207,5) | (528,5) | ②展开 `x = 548-13-7`；③实测黄绿芯 (170,204,34) x529..539 y6..15。收缩态挂关闭钮左侧 `close_x-6-13` |

⚠ 与任务书 A 条的偏差（已定案，勿再改回）：`UIWindow.img/BtUIClose` 是 **32x15 的「X CLOSE」梯形对话框件**（见 `w10-close-candidates.png`），
参考截图里这颗是 **12x12 蓝底白叉** = `Basic.img/BtClose`，两者不同形；`BtUIClose` 只作缺件回退。

## 3. 分类条（三格，字图真素材 + 自绘底衬）

`Quest/Tab/*` 只有 0/1/2 三档，且**都是透明底字图**（`w10-tabs.png`：`disabled` = 白字、`enabled` = 粉字），
`backgrnd` 里没有粉/灰格（`backgrnd.png` 已核）→ 底衬必须自绘，这是原生行为，不是「ColorBox 冒充素材」。

| 控件 | 节点路径 | WxH + origin | 收缩态左上 | 展开态左上 | 依据 |
| --- | --- | --- | --- | --- | --- |
| 格 1 黑框 | 自绘 ColorBox | 65x19 | (2,22) | (2,22) | ③实测黑列 x2/66、黑行 y22（`_probe_title.txt`） |
| 格 2 黑框 | 自绘 ColorBox | 66x19 | (66,22) | (66,22) | ③实测黑列 x66/131 |
| 格 3 黑框 | 自绘 ColorBox | 66x19 | (131,22) | (131,22) | ③实测黑列 x131/196 |
| 格 1 内衬 | 粉 `0xEE6688` / 灰 `0xBBBBBB` | 61x16 | (4,24) | (4,24) | ③粉底 bbox x4..64 y24..39；灰芯 (187,187,187) |
| 格 2 内衬 | 同上 | 62x16 | (68,24) | (68,24) | 同上 |
| 格 3 内衬 | 同上 | 62x16 | (133,24) | (133,24) | 同上 |
| 字图 1「可以开始」 | `Tab/enabled/0`(粉,未选) + `Tab/disabled/0`(白,选中) | 50x14/50x15 | (9,24) | (9,24) | ②格内水平居中 `tab_x+(tab_w-w)/2`；③字图内芯 y2..12 → 绝对 26..36，实测文本行 y26..36 吻合 |
| 字图 2「正在进行」 | `Tab/enabled/1` + `Tab/disabled/1` | 49x14/50x15 | (74,24) | (74,24) | 同上 |
| 字图 3「完成」 | `Tab/enabled/2` + `Tab/disabled/2` | 27x14/27x15 | (150,24) | (150,24) | 同上 |

状态映射（RC12 修正点）：`TwoSpriteButton(常态=enabled 粉字, 选中态=disabled 白字)`。
旧代码把两参写反 → 选中格变成「粉底 + 粉字」，视觉上就是用户报的「没有分类背景色」。

## 4. 左板列表区（收缩/展开同一套，都在左板内）

| 控件 | 节点路径 | WxH + origin | 左上（两态相同） | 依据 |
| --- | --- | --- | --- | --- |
| 命中区 `list_area` | — | — | (9,48)-(226,366) | ③实测首行带 y48 起、底带边线 y367；右缘 x224..241 是滚动条槽 |
| 行距 `row_height` | — | 23 | — | ③分组头 48..66 + 次行 71..89 → 步距 23，可视 13 行 |
| 分组头底 | 自绘 ColorBox | 224x19 | (9,row_top) | ③实测浅蓝 (153,187,204) x9..232 y48..66 |
| 分组头标记 | `Quest/icon4` | 13x14 + (0,0) | (11,row_top+2) | ③实测蓝像素 x13..22 y52..60（截图那本蓝书，**不是** icon2 箭头） |
| 分组头文字 | Text A11B 白 | — | (29,row_top+4) | ③ |
| 选中行底 | 自绘 ColorBox | 204x19 | (27,row_top) | ③实测深蓝 (51,102,153) x27..230 y71..89 |
| 行项目符（可接） | `Quest/icon0` | 13x14 | (12,row_top+3) | ③实测金像素 x15..18 y76.. |
| 行项目符（进行中） | `Quest/icon2/0` | 15x13 | (12,row_top+3) | 动画序列首帧（icon2 共 8 帧） |
| 行项目符（已完成） | `Quest/icon1` | 13x14 | (12,row_top+3) | 灰「!」；本客户端无「限时/重复/双人」态数据，icon5..icon9 不接（见 §7） |
| 行文字 | Text A11M | — | (29,row_top+3) | ③实测黑字左缘 x29、首字顶 y97（行首 94） |
| 空列表提示 | `Quest/notice0/1/2` | 124x35 / 124x34 / 102x33 | 列表区水平居中，y=96 | 位图识字 = 「没有可以开始/正在进行/被完成的任务」，原生无实测位，取列表上部居中 |
| 列表滚动条 | `Basic.img/VScr` | 宽 12..15 | x=226，y48..366 | ③实测槽 x224..241；Quest 根内无滚动条 |

## 5. 右板详情区（仅展开态绘制）

| 控件 | 节点路径 | WxH + origin | 展开态左上 | 依据 |
| --- | --- | --- | --- | --- |
| 蓝色横幅 + 绿点 + 立绘框 | 烤在 `backgrnd2` | — | — | ①实测横幅 x251..545 y15..129、绿点 x23..26（板内）→ **不再自绘项目符** |
| 任务名 | Text A11B 白 | — | (277,40) | ③实测白字 bbox x278..380 y41..52 |
| 正文 `detail_area` | — | — | (261,124)-(524,340) | ③实测黑字左缘 x261；下界给进度条让位 |
| 正文滚动条 | `Basic.img/VScr` | — | x=530，y124..340 | ②右板内缘 546 反算 |
| 进度条底 | `Quest/Gauge/frame` | 163x14 + **(81,0)** | (313,345) | ②`panel_x+(305-163)/2`、`BAND_TOP-14-8`；绘制点 = 左上 + origin(81,0) |
| 进度条填充 | `Quest/Gauge/gauge` | 1x11 + (0,0) | (315,346) 起，宽 = `(163-4)*ratio/100` | `DrawArgument(pos, stretch)` 的 stretch 是**绝对宽高**（见 DrawArgument.h:122） |
| 进度条端点 | `Quest/Gauge/spot` | 1x11 + (0,0) | 填充右端 | 同上 |
| 进度文字 | Text A11M 黑 | — | (482,346) | ②底板右缘 +6 |

## 6. 底带按钮行（内容区下方，y367..391，钮高 17 → 左上 y=370）

| 控件 | 节点路径 | WxH + origin | 收缩态左上 | 展开态左上 | 依据 |
| --- | --- | --- | --- | --- | --- |
| 寻找NPC位置 | `Quest/BtMarkNpc/n\|s\|p\|d\|keyFocused` | 81x17 | (155,370) | **(458,370)** | ②`x = 548-81-9`；③实测 cyan 芯 x461..538 y372..385（底带角锚 k=9，与标题条 k=7 不同口径）。旧值 (460,371) 且构造时未赋值 → 落 (0,0) 压在列表上，即用户报的「位置不对」 |
| 详细 | `Quest/BtDetail` | 57x17 | (92,370) | (395,370) | ②同一行左移 `w+6`；有既有逻辑（`detail_mode` 切换简述/详细），故保留可点 |
| 放弃 | `Quest/BtGiveup` | 57x17 | (29,370) | (332,370) | ②同上；仅「正在进行」页激活，发**既有** `QuestActionPacket(3, qid)`（与 UIMiscShells 的快捷键分支同一协议） |
| 接受 | `Quest/BtOK` | 57x17 | 预留不显示 | 预留不显示 | 见 §7 |
| 拒绝 | `Quest/BtNo` | 57x17 | 预留不显示 | 预留不显示 | 见 §7 |
| 任务讯息 | `Quest/BtAlert` | 56x16 | 预留不显示 | 预留不显示 | 见 §7（就是用户报的「任务整理」） |

## 7. 本包没有 / 已放弃 / 已预留

1. **Q4「任务整理」定案：本包不存在该控件。** 位图识字覆盖 `Quest` 根下全部带字件：
   `BtAlert`=「任务讯息」、`BtDetail`=「详细 ▶」、`BtGiveup`=「放弃」、`BtMarkNpc`=「寻找NPC位置」、`BtNo`=「拒绝」、`BtOK`=「接受」；
   标题条 `basic`=「无条件获得」、`prob`=「几率获得」、`reward`=「奖励!!」、`select`=「选择获得」、`summary`=「任务简述」；
   `notice0/1/2`=「没有可以开始/正在进行/被完成的任务」、`notice{0,1,2}`=「彩虹岛/金银岛/神秘岛」（红星标 + 区域名）。
   → 全仓中文串 0 命中 + 全位图 0 命中，唯一形近的是 **「任务讯息」(BtAlert)**：旧代码把它排在底带最左，即用户看到的「任务整理被渲染到下面」。
   现已 `set_active(false)` 预留，底带只剩实测过的「寻找NPC位置」。
2. **窗内关闭件**：`Quest` 根下没有 → 用 `Basic.img/BtClose`（实测同形），`BtUIClose`/`BtClose2` 仅作缺件回退。
3. **滚动条**：`Quest` 根下没有 → `Basic.img/VScr`（W0 已做根名钳制）。
4. **行为预留（不发明协议）**：`BtOK`(接受)、`BtNo`(拒绝)、`BtAlert`(任务讯息)、`BtMarkNpc`(寻找NPC位置，需地图高亮/广播，包内无对应出站包)。
   钮都已按上面的槽位建好，只缺行为；接包前保持 `set_active(false)`（寻找NPC位置例外：显示但不发包）。
5. **未用素材**：`backgrnd3[296x289]`/`backgrnd5[266x117]`（另两种代次/尺寸的外壳，本窗实测用 `backgrnd`+`backgrnd2`）、
   `backgrnd4[305x396]`（蓝横幅下多一块粉色区，属「奖励/条件」明细态，本包无对应数据）、`Gauge2{frame 79x8, gauge 1x4}`、
   `TimeQuest{AlarmClock,TimeBar}`（限时任务，无数据）、`icon3/5/6/7/8/9`（限时/新任务/重复/双人等动画态，无数据）、
   `notice{0-2}`（区域名红标横幅，参考截图里分组头用的是蓝书 + 白字，不用它）、标题条 `basic/prob/reward/select/summary`（属接受/奖励页）。
6. **与任务书 B 条的偏差**：分类条的**粉/灰底与黑描边、行选中深蓝底、分组头浅蓝底**在本包里都没有位图（已用 `backgrnd.png`/`w10-tabs.png` 反证），
   原生即自绘 → 这几处保留 ColorBox（颜色/尺寸全部按截图实测取整），只把**字图与项目符**换成真素材；`icon2/0` 冒充分组头书标的写法已删。
