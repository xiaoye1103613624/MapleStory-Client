# v83 客户端 UI 版式还原 · 总计划（全量一次规划，分批交付）

> 状态：**待你确认后开工**
> 日期：2026-10-31
> 适用：`E:\project\MapleStory-Client` → 部署 `E:\MXD\BeiDou-Client-xy\`，BeiDou gms083，1280×720，`data.format=img`
> 证据来源：你给的 `当前客户端截图` / `正确客户端截图` + `Data/.nxcache/UI.nx` 节点实测 + `E:\project\_ida_beidou\BeiDou.exe` 字符串锚点 + 代码 file:line
> 前置文档：[ui-layout-v83-unify](../ui-layout-v83-unify/README.md)（登录链+底栏第一轮）、[login-flow-ui-ida-align](../login-flow-ui-ida-align/README.md)（IDA 锚点式改法）、[beidou-v17-parity r15](../beidou-v17-parity/notes/r15-baseline-ui-gap-vs-screenshots.md)（上一轮基础 UI 差距审计）

## 0. 已确认决策（本轮你拍板）

| 项 | 决策 |
| --- | --- |
| 证据来源 | **节点树为主，按窗口补 IDA**（版式取 `UI.nx` 权威 origin/size；命中区/分发/显隐才定点反编译） |
| 改造策略 | **混合**：先修公共件（tooltip 根、滚动条、空节点门闩），再逐窗校准 |
| 范围 | **全量一次规划完，再分批做**（本文件即全量） |
| 验收 | **我驱动线上客户端逐窗截图对比**（沿用本会话已给的授权；完事恢复鼠标/窗口） |

## 1. 一句话结论

**不是缺资源。** 两端 `Data/UI/*.img` SHA256 全同（r00 已证），且原生 `BeiDou.exe` 里字符串 `UIToolTip` **命中 0 次** —— 说明当前端引用的那批资产根根本不属于 v83。问题集中在三类：

1. **走错资产代次**（HeavenClient 偏新路径：`UIToolTip.img` / `UIWindow2/4/6.img` / `StatusBar3.img` / `chat/ingame` / `VScr9`）；
2. **公共件在 v83 包上静默失效**（滚动条、九宫格、`usable()` 空节点判定）；
3. **按钮坐标默认 `(0,0)`**（`try_maple_btn` / `TwoSpriteButton` 不传坐标 → 全叠在窗口原点，表现为"按钮缺失/跑到左上角/图标缺失"）。

## 2. 根因清单 RC1–RC15

等级：**A** = 我本轮亲自复核过代码与资产；**B** = 子代理取证、file:line 明确但未二次复核；**C** = 需 IDA 补证才能定案。

| # | 症状（你报的） | 根因 | 证据 | 级 | 修复动作 | 影响面 |
| --- | --- | --- | --- | --- | --- | --- |
| RC1 | 背包/技能/任务**都没有滚动条** | `Slider` 由枚举值拼 `Basic.img/VScr<type>`，四处都传 `DEFAULT_SILVER`＝**9** → `VScr9`；包内只有 `VScr`…`VScr8` | [Slider.h:44-59](../../../IO/Components/Slider.h#L44) 枚举逐项算值；[Slider.cpp:53-58](../../../IO/Components/Slider.cpp#L53)；调用点 [UIItemInventory.cpp:273](../../../IO/UITypes/UIItemInventory.cpp#L273)、[UISkillBook.cpp:275,287](../../../IO/UITypes/UISkillBook.cpp#L275)、[UIQuestLog.cpp:105,112](../../../IO/UITypes/UIQuestLog.cpp#L105) | A | 换成本包存在的 type（观感对齐后定 5 或 8），并在 `Slider` 构造里对不存在的根做钳制+日志 | 全部滚动条 |
| RC2 | 能力值 +/- 钮、装备左上两钮、技能 5 个 tab **叠在一起/位置不对** | `try_maple_btn(..., Point pos = {})` 与 `TwoSpriteButton` 默认坐标 `(0,0)`，调用点大量不传 | [UIStatsInfo.cpp:54](../../../IO/UITypes/UIStatsInfo.cpp#L54),[:143-157](../../../IO/UITypes/UIStatsInfo.cpp#L143)；[UIEquipInventory.cpp:181-182](../../../IO/UITypes/UIEquipInventory.cpp#L181)；[UISkillBook.cpp:234-240](../../../IO/UITypes/UISkillBook.cpp#L234)；[TwoSpriteButton.cpp:14](../../../IO/Components/TwoSpriteButton.cpp#L14) | A | 逐窗补坐标（按节点尺寸算），并让"该建却没坐标"成为显式失败 | 能力值/装备/技能/背包/任务 |
| RC3 | 悬浮道具**无背景色** | 5 个 tooltip 组件指向不存在的 `UI["UIToolTip.img"]["Item"]` → `MapleFrame` 拿到 9 个空纹理，什么都不画 | [ItemTooltip.cpp:33](../../../IO/Components/ItemTooltip.cpp#L33)、[EquipTooltip.cpp:32](../../../IO/Components/EquipTooltip.cpp#L32)、[SkillTooltip.cpp:30](../../../IO/Components/SkillTooltip.cpp#L30)、[TextTooltip.cpp:30](../../../IO/Components/TextTooltip.cpp#L30)、[MapTooltip.cpp:32](../../../IO/Components/MapTooltip.cpp#L32)；`nx_inspect`：`UIWindow.img/ToolTip` 只有 `Equip/cash/mesos`，**无 c/e/ne/n/nw/w/sw/s/se**；BeiDou.exe `UIToolTip` 0 命中 | A | 框体真身需 IDA 定（见 §6-Q1）；先把根改到 `UIWindow.img/ToolTip` | 全部 tooltip |
| RC4 | 悬浮内容"不知道对不对" | 需求/属性图标实际在 `UIWindow.img/ToolTip/Equip/{Can,Cannot,Property,Growth*,Speed,WeaponCategory,ItemCategory}`，代码找 `UIToolTip.img/Item/Equip/Can` | 原生 exe 字符串 `UI/UIWindow.img/ToolTip/Equip/GrowthDisabled`、`.../GrowthEnabled`；`nx_inspect` 已确认这些子节点存在 | A | 改路径 | 装备/道具 tooltip |
| RC5 | 部分窗体"有框无底"、分类无背景 | `UiAssetResolve::usable(n)` 只是 `bool(n)`，**present-but-empty 节点判不出来**，门闩形同放行 | [UiAssetResolve.cpp:77-80](../../../Util/UiAssetResolve.cpp#L77)（头文件 `.h:38` 声称会查子件/位图，实现没做） | A | 加 `child_count>0 且有位图` 判定；`window()` 去掉对不存在根的静默回退，改记日志 | 所有窗体门闩 |
| RC6 | 等级字体不对、数值颜色不对、名字/血蓝经验偏下 | 底栏数值用 `Text(A11B, YELLOW)` 绘制；`StatusBar.img/number`（5×7 位图字库）**建了 Charset 却没画**；`Basic.img/LevelNo`/`ShowLevel` 全仓 0 引用；LV 徽章是程序画的 `ColorBox` | [UIStatusBar.cpp:1830-1834](../../../IO/UITypes/UIStatusBar.cpp#L1830),[:1894-1900](../../../IO/UITypes/UIStatusBar.cpp#L1894),[:2107-2126](../../../IO/UITypes/UIStatusBar.cpp#L2107) | B | 改画位图字库；行 y 按 `backgrnd[817x71]` 重算 | 底栏/能力值数字 |
| RC7 | **商城点了没反应** | 只发 `EnterCashShopPacket`（出站 opcode **40**，空 body）；可用的商城请求走 `CASHSHOP_OPERATION=229`，HUD 换态靠 recv `SET_CASH_SHOP=127` → `UI::change_state(CASHSHOP)` | [UIStatusBar.cpp:625-629](../../../IO/UITypes/UIStatusBar.cpp#L625)；[GameplayPackets.h:47](../../../Net/Packets/GameplayPackets.h#L47)；[OutPacket.h:61](../../../Net/OutPacket.h#L61),[:172](../../../Net/OutPacket.h#L172)；[CashShopHandlers.cpp:642](../../../Net/Handlers/CashShopHandlers.cpp#L642) | B | 换包（**跨到 s9 协议线**，需先对服务端 opcode 表取证） | 底栏 |
| RC8 | "界面"跑到左下角，且**一同打开键盘设定** | 同一 handler 里连开两窗；`UIShortCut` 位置写死 `(8, vh-120)`；其 draw 只画 `backgrnd` + 一行黄字，`UIWindow.img/ShortCut` 的 11 个 `Bt*` **一个都没实例化** | [UIStatusBar.cpp:714-727](../../../IO/UITypes/UIStatusBar.cpp#L714)；[UIMiscShells.cpp:3301-3304](../../../IO/UITypes/UIMiscShells.cpp#L3301),[:2571-2575](../../../IO/UITypes/UIMiscShells.cpp#L2571) | B | 拆派发；用真素材重建快捷面板 | 底栏 |
| RC9 | "帮助"原生是打开脚本 | `UIHelp` 用 `Login.img/Notice`；`Npt_GuideLine.img`（纯中文文本 `NptGuide/Text00..48`）**全仓 0 引用** | [UIStatusBar.cpp:708-713](../../../IO/UITypes/UIStatusBar.cpp#L708)；[UIHelp.cpp:24](../../../IO/UITypes/UIHelp.cpp#L24)；原生 exe 有字符串 `UI/StatusBar.img/BtNPT` | B | 接 `Npt_GuideLine` 文本 + `BtNPT` | 底栏 |
| RC10 | 能力详情展开后**收不回**；详细钮不在右下角 | 收缩钮读不存在的 `main/BtDetailClose`、`detail/BtHpUp` → 永不构造 → `set_detail(false)` 不可达；`BtApDown` 包内也没有；`BtDetail` 建了但无坐标 | [UIStatsInfo.cpp:155-162](../../../IO/UITypes/UIStatsInfo.cpp#L155),[:345](../../../IO/UITypes/UIStatsInfo.cpp#L345),[:478](../../../IO/UITypes/UIStatsInfo.cpp#L478) | A（缺件）/C（原生用哪个节点） | IDA 补证后接 | 能力值 |
| RC11 | "扩展道具栏"在窗外包着、展开后内容外溢 | `set_full` 只改尺寸不重排；展开态金币标签 y=**359** > 框高 289；`BtFull` 与 `BtSmall` 同坐标 `(109,6)` | [UIItemInventory.cpp:1588-1620](../../../IO/UITypes/UIItemInventory.cpp#L1588),[:210-218](../../../IO/UITypes/UIItemInventory.cpp#L210),[:300-309](../../../IO/UITypes/UIItemInventory.cpp#L300) | B | 展开态重排表 | 背包 |
| RC12 | 任务窗无关闭钮、"寻找NPC位置"错位、点击不切换、分类无背景 | `close_pos=(514,4)`、`marknpc_pos=(460,371)` 在 **:100-101** 赋值，而按钮在 **:80/:84** 已构造 → 双双落 `(0,0)`；无 `send_cursor`，只有双击选中；分类只取 `Tab[0..2]`，`icon2..icon9`（8 组分类图）未用；还拿 `ColorBox` 冒充高亮/书页/项目符 | [UIQuestLog.cpp:72-105](../../../IO/UITypes/UIQuestLog.cpp#L72),[:413-425](../../../IO/UITypes/UIQuestLog.cpp#L413),[:317-327](../../../IO/UITypes/UIQuestLog.cpp#L317),[:59-66](../../../IO/UITypes/UIQuestLog.cpp#L59) | B | 调赋值顺序 + 补单击 + 用 `icon*`/`backgrnd2..5` 重建 | 任务 |
| RC13 | 小地图空框、橙色"大地图"钮乱位；W 大地图开在左上角且版式不对 | 小地图请求 `BtMin/BtMax/BtSmall/BtBig/BtNpc`（包内**只有 `BtMap`**）且 y=**-6**；canvas 被 2048px 守卫跳过；头像从不请求；`PosMINIMAP` 默认 `(0,0)`；`UIWorldMap` 把 `Border["0"]`（**60×33**）当整窗框用，默认位置 `(100,35)` | [UIMiniMap.cpp:84-90](../../../IO/UITypes/UIMiniMap.cpp#L84),[:170-215](../../../IO/UITypes/UIMiniMap.cpp#L170),[:637-706](../../../IO/UITypes/UIMiniMap.cpp#L637)；[UIWorldMap.cpp:52-92](../../../IO/UITypes/UIWorldMap.cpp#L52)；[Configuration.h:661](../../../Util/Configuration.h#L661),[:682](../../../Util/Configuration.h#L682) | B | 按 `MinMap`/`MaxMap` 九切片重建框；`BtMap` 唯一钮归位；大地图用 `Border{0..7}` 拼边 | 小地图/大地图 |
| RC14 | 快捷格只一行、没有键帽提示 | 只用 `base/quickSlot`/`quickSlot08`；`quickSlot26`/`quickSlot32`/`base/box`/`iconMemo` **0 引用**；`key` 字层仅窄屏且只画在空格上 | [UIStatusBar.cpp:1862](../../../IO/UITypes/UIStatusBar.cpp#L1862),[:1903-1904](../../../IO/UITypes/UIStatusBar.cpp#L1903),[:2135-2161](../../../IO/UITypes/UIStatusBar.cpp#L2135) | B | 换 26/32 格面板 + 补键帽层（`key`+`number`+`FontMemo`） | 快捷格 |
| RC15 | 聊天白区缺"目标对象按钮 + 展开缩放"，逻辑显示都不对 | 日志面板是程序画的 `ColorBox`；classic 只有 `BtWhisper` 一个钮；`BtMax/BtMin/BtChat/...` **只在 modern 分支**从 `chat/ingame`（本包没有）创建；展开/缩放靠手搓 12×12 命中区翻转 `classic_log_open`，**没接任何按钮** | [UIChatBar.cpp:119-128](../../../IO/UITypes/UIChatBar.cpp#L119),[:139-148](../../../IO/UITypes/UIChatBar.cpp#L139),[:205-216](../../../IO/UITypes/UIChatBar.cpp#L205),[:620-640](../../../IO/UITypes/UIChatBar.cpp#L620),[:1266-1280](../../../IO/UITypes/UIChatBar.cpp#L1266)；原生 exe 有字符串 `UI/StatusBar.img/base/chatTarget` | B | 用 `base/chatTarget` + `Basic.img/BtMax/BtMin` 重建控件行；日志框体改用可用九切片 | 聊天栏 |

### 2.1 顺带确认的资产事实（后续批次直接引用，勿再猜）

- 存在的完整九切片只有 5 族：`ChatBalloon.img/pet/*`、`ChatBalloon.img/dead`、`Login.img/WorldNotice/Balloon`（9×9 角，带 `clr`）、`/BalloonForLoginProcess`、`UIWindow.img/MiniMap/MinMap`、`/MaxMap`、`UIWindow.img/UserList/Party/PartyHP`。→ **tooltip 框体在包内没有原生对应物**，见 §6-Q1。
- `Basic.img/VScr…VScr8` 存在（无 VScr0/VScr9）；子件命名 `disabled->{base,next,prev}`、`enabled->{base,next0,next1,prev0,prev1,thumb0,thumb1}` —— **`Slider.cpp:61-75` 已经对齐**，RC1 只是根名越代。
- `UIWindow.img/BtUIClose`、`BtUIClose2` 是共享关闭钮（任务窗就该用它）。
- `NameTag.img/<id> -> {c,e,w,clr}` 是**三切片 + ARGB**（id 3…254，另有 `medal{}`/`pet{}` 分组）→ 名牌背景的正确做法，也是 #29 未拍到的 NAMETAG 受益面。
- `CashShop.img` 有 23 个子根（含 `Base`）→ 商城素材齐，RC7 纯接线/协议问题。
- `UIWindow.img/WorldMap` 只有 `Border{0..7}` + `title` → 大地图版式必须靠 `Map.nx/WorldMap/*.img` 的区域素材。

## 3. 批次分解（W0–W13，按依赖排序）

每批交付 = 代码 + 构建 + 线上抓帧对照 + evidence 表。**一批一个原子 commit**（等你逐批要求时提交）。

### W0 公共件（先做，多窗同时受益）
- **W0.1** RC1 滚动条根名钳制：`Slider` 构造时若 `Basic.img/VScr<type>` 不存在 → 回落到存在的根并打一条 WARN（可观测，不静默）。背包/技能/任务/详情四处滚动条同时复活。
- **W0.2** RC5 `usable()` 真判定（子件数 + 位图），并给 `window()/window4()` 的跨代回退加一次性日志，便于后续批次看清"哪些窗其实没素材"。
- **W0.3** RC2 机制化：`try_maple_btn` / `TwoSpriteButton` 无坐标时**不创建并 WARN**（禁止再静默叠 `(0,0)`）。⚠ 本条会让现有"恰好能用"的叠位钮消失，**必须与 W6–W10 的逐窗补坐标同批构建、同批验收**，中间不允许单独发布。
- 验收：滚动条可见可拖；`_resource_probe` 日志能列出空壳根；无新增崩溃。

### W1 底栏身份块（RC6）
LV 徽章/等级数字改画 `Basic.img/LevelNo`+`ShowLevel`；HP/MP/EXP 数值改画 `StatusBar.img/number` 位图字库（含 `[ ]` `%` `/`）；职业名/角色名/三条 gauge 的 y 按 `base/backgrnd[817x71]` 重算。
验收判据：`HP [50/50]` 数字基线与正确截图偏差 ≤2px；字模宽高 = 5×7。

### W2 底栏四按钮行为（RC7/RC8/RC9）
- 帮助 → `Npt_GuideLine.img/NptGuide/Text00..48`（中文文本窗，不再用 `Login.img/Notice`）；
- 界面 → 只开 `UIShortCut`，用 `UIWindow.img/ShortCut` 的 11 个 `Bt*` 真素材重建，位置改窗口锚定而非 `(8,vh-120)`；
- 商城 → **先对服务端 opcode 表取证**（40 vs 229），属跨线改动，单独评估后再动（默认：本批只把"点了没反应"变成"给出明确未支持提示"，不擅自换协议）。
验收：四钮各自行为与正确截图一致；不再同帧开两窗。

### W3 快捷格（RC14）
`base/quickSlot32`（571×80，2 行）+ 键帽层（`StatusBar.img/key`+`number`+`FontMemo`）+ 中文标签沿用 T12b 度量换行。
验收：2 行 26/32 格；每格左上角键名可见。

### W4 聊天栏（RC15）
`base/chatTarget` 目标选择钮行 + `Basic.img/BtMax/BtMin` 接 `toggle_view`；日志面板框体选型（候选：`Login.img/WorldNotice/Balloon` 九切片）—— 与 §6-Q1 同一决策，若 Q1 定案则复用。
验收：白区有目标钮行与展开/缩放；展开/收缩可点。

### W5 悬浮提示（RC3/RC4）
根改 `UIWindow.img/ToolTip`；`Equip/{Can,Cannot,Property,Speed,WeaponCategory,ItemCategory}` 与 `cash`/`mesos` 接上；框体依赖 §6-Q1。
**顺带闭环 #29**：tooltip 的长中文换行 + 9 切片拉伸首次可拍。
验收：道具/技能/装备三类 tooltip 有底、有边框、换行正确（对照 T12b 度量预测）。

### W6 能力值（RC2/RC6/RC10）
`backgrnd[208x347]` 为基准重排 8 行身份 + 4 行属性；`basicStat[159x121]` 作窗内蓝头（不再"只在有 AP 且上方有空间时"才画）；`BtDetail[46x19]` 落右下角；详情面板锚点改 `backgrnd` 宽；`Disabled{STR/DEX/INT/LUK}` 用于不可加点态；收缩钮待 §6-Q2。
验收：与 `正确客户端截图/打开了S-E-I-K页面.png` 左窗逐边对齐；详情可开可收。

### W7 装备（RC2 + §6-Q3）
`BtDetail`/`BtCashshop` 补坐标；新增 `BtPet1/2/3`、`BtPetEquipShow/Hide`、`pet[177x181]`、`FullBackgrnd[208x291]`；"二级密码"文案来源先定案（§6-Q3）再改。
验收：左上两钮语义正确；宠物装备区可展开。

### W8 背包（RC2/RC11 + W0.1）
tab 条底衬（`Tab{0-4}` 27×16 与 `New/Tab0|Tab1` 34×18 的取舍按截图定）；`BtFull`/`BtSmall` 坐标分离；`set_full` 后重排 tab/工具/金币；滚动条随 W0.1 复活后校几何。
验收：分类有底；扩展栏按钮在窗内；滚动条可拖 6→更多行。

### W9 技能（RC2）
5 个 tab 补坐标（`Tab/enabled|disabled{0-4}`）；行底 `skill0/skill1[141x35]` + `line[141x1]`；顶部职业横幅 `recommend[105x35]`（缺失图标即此项）；`tip0..2[193x102]`；`UIWindow.img/Macro` 接宏指令钮；滚动条几何。
验收：tab 可切；行底对齐；缺图标补齐。

### W10 任务（RC12 + §6-Q4）
赋值顺序修正（先算 `close_pos`/`marknpc_pos` 再建钮）；关闭钮用 `UIWindow.img/BtUIClose`；分类 `icon2..icon9`；单击选中（补 `send_cursor`）；`BtMarkNpc`/`BtDetail`/`BtGiveup`/`BtOK`/`BtNo`/`Gauge{frame,gauge,spot}`（原生字符串已证）；"任务整理"来源待 §6-Q4。
验收：可关、可切、可查详情、进度条显示。

### W11 小地图 / 大地图（RC13）
小地图：`MinMap`/`MaxMap` 九切片拼框、唯一 `BtMap` 归位、头像（`Character` 脸饰）+ 地图名（`String/Map`）+ canvas 2048 守卫复核、`PosMINIMAP` 默认改左上安全位。大地图：`Border{0..7}` 拼边 + `Map.nx/WorldMap/*` 区域素材 + 默认位置。
验收：两块都与正确截图同形；W 打开位置合理。

### W12 名牌（`NameTag.img` 三切片）
`NameTag.img/<id>{c,e,w,clr}` 正确用法替换现有 `NAMETAG` 近似实现；与 T12b 度量联动（框宽 = 文本量宽 + 左右帽）。**闭环 #29 的 NAMETAG 受益面**。
验收：中文 NPC/怪物名牌框宽 = 字形宽 + 2 帽 + 余量 ≤2px。

### W13 回归与归档
登录链 4 屏不退化复验；全窗截图对照表；`README.md`（最终实现逻辑）；删临时调试日志与 `tools/_*.py` 临时脚本；原子提交清单。

## 4. 验证门禁（每批都跑）

1. **构建**：`MSBuild MapleStory.sln /p:Configuration=Release /p:Platform=x64`；`OutDir` 即运行目录 `E:\MXD\BeiDou-Client-xy\`。
2. **部署身份**：比对 COFF TimeDateStamp + SHA256 前 16 位（Release 无客户端日志，只能靠这个认版本）。
3. **回退链**：开工前给当前 exe 建 `bak_pre-ui-v83-restore-<date>`（现有 `bak_pre-t12b-20261031` = `e1399451…`、`bak_pre-t11-20261031` 保留不动）。
4. **实机**：我驱动线上客户端（本会话已授权）→ 逐窗 `PrintWindow` 抓帧（DPI-aware，client 1280×720；抓帧含标题栏，**frame→client = (0, −38)**，此结论本轮实测）→ 与 `正确客户端截图` 并排 → 量化判据：**关键边/中心差 ≤2px**。
5. **禁改**：全程只用 `tools/nx_inspect.py` 只读资源；不改任何 `.img`/`.nxcache`；不写库。

## 5. 产物

```text
docs/features/ui-v83-window-restore/
  PLAN.md                       # 本文件
  README.md                     # 完成后：最终实现逻辑
  notes/asset-node-inventory.md # UI.nx 权威节点清单（W0 起手生成）
  notes/native-ui-asset-strings.txt # BeiDou.exe 资源路径字符串锚点
  evidence/wNN-<窗口名>/        # 每批：改前/改后抓帧 + 对照表 + 像素判据
  patches/                      # 逐批 diff 摘要
```

## 6. 待 IDA 补证（**开工前置，不阻塞本计划评审**）

| # | 问题 | 为什么节点树答不了 | 取证方式 |
| --- | --- | --- | --- |
| Q1 | v83 提示框/聊天日志框的**框体真身**是哪个节点 | 包内 5 族九切片里没有 tooltip 风格的那族；原生 exe 又 0 引用 `UIToolTip` | 定点反编译原生 tooltip 构造函数（`search` 字符串 `ToolTip` 的 2 处命中 + `MapleFrame` 式 9 键取用点） |
| Q2 | 能力值**收缩钮 / 减点钮**原生用哪个节点 | `BtApDown`、`BtDetailClose` 在本包不存在 | `CUIStat` ctor + `CreateCtrl` |
| Q3 | 装备左上两钮为何弹**"二级密码"** | 代码里找不到该文案，`UIWindow.img/Equip` 与 String 转储里也没有 | 追原生 `CUIEquip` 两钮的 tooltip 绑定；或先实机复现抓包 |
| Q4 | 任务窗**"任务整理"**控件来源 | 当前 `UIQuestLog` 的 Buttons 枚举里根本没有该钮 | `CUIQuest`/`CUIQuestLog` 控件表 |

**工具状态**：本会话 **IDA MCP 未接入**（`mcp_list` 无 ida）。Q1–Q4 需要时我会先问你是让我按 [toolchain](file:///C:/Users/11036/.agents/skills/xiaoye-MapleStory-dev/toolchain.md) 起 headless idalib，还是你手动导出。

## 7. 风险与边界

- **W0.3 是全局风险点**：让叠位钮显式失败 → 与 W6–W10 绑定交付，不允许单独发布。
- **RC7 跨协议线**：默认不换 opcode，只做"明确提示未支持"；要换需单独取证并归入 s9 计划的 T14。
- 不改服务端、不写库、不动 `.img`；`W1–W12` 全部是客户端 C++ 与既有资产取用。
- 你已授权"改已联调链路，逐条给 s9 `path:line` 证据" —— 本计划每条改动都会在 evidence 里留 `path:line` + 节点路径。
- 背包/装备类验收需要角色**持有道具**：当前 Admin 号背包是空的。到 W7/W8 时我会先问你是否允许用游戏内 GM 命令自然发放（不手写 SQL）。

## 8. 与其它在途任务的关系

| 任务 | 关系 |
| --- | --- |
| #29 T12b 剩余受益面抓帧 | **被 RC3 阻塞**，并入 W5（tooltip 换行/拉伸）与 W12（名牌框）；不单独收尾 |
| #18 T7 / #23 S1..S11 / #24 T14（s9 协议线） | 不受影响；RC7 若升级为协议改动则挂到 #24 |
| T13 中文输入（IME） | 独立；NPC 对话乱码属该线，不在本计划内 |
