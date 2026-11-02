# W0–W13 统一验证清单（一起过一遍）

> 用法：按顺序走，每条给「操作 → 预期 → 对照 → 判据」。判据不满足就先别改代码，把实际截图存到
> `evidence/w13-verify/<条目号>.png`，我们再按 evidence 表定位是**算错**还是**素材判断错**。
> 本轮改动全部是客户端显示层 + 既有封包的取用，**没有**改服务端、没改 opcode、没写库、没动 `.img`。

---

## 0. 前置（一次性）

| # | 事项 | 说明 |
| --- | --- | --- |
| 0.1 | 构建通过 | Release x64 已 0 error。构建链（靠 COFF 戳 + SHA256 区分，尺寸会变）：<br>· `6AE5ED29 / 0756a986a9f39d93`（21:29，4,358,144）—— **已启动并成功进图**，门禁「能不能跑」已过<br>· `6AE5F2CA / c5b296658d34190f`（21:53，4,358,144）—— 含 `PosMINIMAP (5, 24)`，同样进图验证过<br>· `6AE5F86C / d76626eb0b300ab8`（22:23，4,361,216）—— **临时带 `[DBG-UICFG]` 诊断**，用来定案 A5，已废弃<br>· `6AE5FAD4 / fb1283d24c1ccd44`（22:27，4,360,704）—— 诊断 + 解析器修复，定案用<br>· **`6AE5FE0D / 67145c64a74c67b3`（22:46，4,357,632）= 当前线上件**：解析器修复 + 公告条修复，诊断已全删<br>⚠ 读戳必须用 **`pe+8`**：`pe+4` 是 Machine+NumberOfSections，误读会得到 `427620`（=1970-01-05）这种假值——本轮就这么错过一次 |
| 0.1b | 部署 | `build_check\MapleStory.exe` + `.pdb` 覆盖到 `E:\MXD\BeiDou-Client-xy\`（覆盖时客户端未在跑，无占用）。只覆盖 exe/pdb，不动 `Data/`。回退件另存 `MapleStory.exe.bak_pre-minimappos-20261031` |
| 0.2 | 回退点 | `MapleStory.exe.bak_pre-minimappos-20261031`（= `0756a986a9f39d93`）；更早还有 `bak_pre-uialpha-*`、`bak_pre-ui-v83-restore-*`、`bak_pre-t12b-*`、`bak_pre-t11-*` 不动 |
| 0.2b | ~~小地图发暗 = 公告条压顶，不是不透明度~~（**已修并复验，见 0.2c/0.2d**） | 原记「A1 帧插值系数当不透明度」是**误判**：修完 `UI::draw(1.0f)` 后收缩条仍暗且 5 帧不抖。真因是 `ScrollingNotice` 的全宽 23px 黑底压住了默认 y=5 的小地图。修法：`Configuration.h:688` 默认坐标 `(5, 5)`→`(5, 24)`。详见 [notes/open-issues.md §A4](notes/open-issues.md) |
| 0.2c | **【本轮最大】`Settings` 里所有三级键以前从来没生效过** | `Configuration::parse_config_file()` 拼层级键时「正向遍历 + 前插」，把三级以上键**整个倒过来**（`ui.position.minimap` → `position.ui.minimap`），于是 39 项设置（31 个窗口坐标 + `audio.*.volume` + `font.path.*` + `ui.autopot.*` + `boot.logo.frame_ms`）**改文件永远没用**，只认代码默认值；拖动过的窗口位置也是「存进去了、下次读不出来」。已修（`Configuration.cpp:415`）。<br>**回归核查已过**：线上 `Settings` 的 39 项里 32 项值 == 代码默认、7 项文件里没这个键 ⇒ 修复后**不会有任何窗口移动/音量变化**。<br>⚠ **仓库根目录 `Settings` 是脏的**（`ui.position.map=(100,35)`、`ui.position.minimap=(0,0)` 是早年死数据），**从仓库目录启动**才会踩到；线上目录不受影响。要不要我顺手清掉这两行，等你点头 |
| 0.2d | 公告条不再常驻 | `ScrollingNotice` 的 `active` 以前永远不会置回 false ⇒ 登录一条「Welcome to Scania!」让顶部 0..22 永久变半透明黑带，把落进来的 UI 全压暗。已改成文字滚完即消失（参考端同一位置是天空本色，没有这条带）。<br>**这一条要你眼睛确认**：进图后**前 ~45 秒**内应该还能看到黄字在 23px 黑底上滚，滚完黑带消失 |

| 0.3 | 空壳资源探针 | 运行目录会生成 `_ui_asset_diag.log`（同一条只写一次）。**先清掉旧文件**，进游戏走一遍后看它：里面每一条 = 代码引用了本包不存在的根/件。它是判「该不该有图」的第一手依据 |
| 0.4 | 参考图 | `C:\Users\11036\Desktop\正确客户端截图\`（**58 张**，2026-11-02 复核；早前记的「16 张」是数错，见 §I.3）；问题端 `当前客户端截图\`（8 张） |
| 0.5 | 角色态前置 | ①背包需**持有道具**（空背包拍不出道具 tooltip / 排序 / 金币行）②需**有未加点与已学技能**（技能窗行底、+SP 钮）③需**已接至少 1 个任务**（任务窗分类与进度条）。当前 Admin 号背包是空的 → 到 B12/B13 时问你是否用游戏内 GM 命令自然发放（不手写 SQL） |
| 0.6 | **参考端二进制的边界**（2026-11-02 定案） | `E:\MXD\BeiDou-ClientV17.7\BeiDou.exe` 目录里是 `Gr2D_DX8.dll / Canvas.dll / NameSpace.dll / ResMan.dll / Shape2D.dll / WzFlashRenderer.dll / Sound_DX8.dll / CIL.dll / PCOM.dll / ZLZ.dll / List.wz / TempPackage.wz` ⇒ **它是关闭源码的原版 MapleStory 引擎，不是我们这套（cJ 血统）代码的编译产物**。所以 IDA 里**找不到与我们源文件 1:1 对应的函数**，「反编译参考端来还原逻辑」只对**数据/常量**成立（槽位表、opcode、字符串），对**函数级重构**不成立。素材侧（`.nxcache/*.nx`）不受此限，仍是第一手依据。<br>另记：该 exe 的字面量是 **UTF-16LE**，`find_bytes` 要按 UTF-16 传字节才有命中（`backgrnd2` → `0xb405ba`）；`find_regex` 只搜 IDA 已识别的字符串表，两者结论不一致时**以 `find_bytes` 为准**，别把它当成「搜索范围不全」。 |

---

## A. 不退化（先确认没把已经好的东西弄坏）

| # | 操作 | 预期 | 判据 |
| --- | --- | --- | --- |
| A1 | 登录 → 选世界 → 选角 → 进游戏 | 四屏版式与 `登录界面.png / 选择频道页.png / 选择角色页.png / 进入游戏页.png` 一致（上一轮 ui-layout-v83-unify 的成果） | 无新增错位；不崩；能正常进图 |
| A2 | 进图后站立 30s，打一只怪 | 无崩溃、无刷屏日志；`_ui_asset_diag.log` 行数不随时间增长（只写一次） | 帧率无明显下降 |
| A3 | 走路时看怪物/NPC 头顶名牌 | 中文名牌**仍正常**（W12 改了 `NameTag` 三切片，但游戏内名牌走的是 `GraphicsGL` 的 `Text::Background::NAMETAG`，本轮**没**接 id → 预期就是「和改之前一样」） | 无花屏/无空白框 |

---

## B. 底栏 HUD（W1 身份块 / W3 快捷格 / W4 聊天栏 / W2 四按钮）

对照 `进入游戏页.png`、`底部信息栏、按钮、快捷键窗.png`、`底部信息栏和功能按钮.png`、`底部快捷按钮窗.png`、`当前客户端截图\底部信息栏、聊天、功能按键.png`。

| # | 看什么 | 预期（本轮改成的样子） | 判据 | 出处 |
| --- | --- | --- | --- | --- |
| B1 | HP/MP/EXP 数值字形 | 从 `Text(A11B, YELLOW)` 换成 `StatusBar.img/number` **位图**（数字 5×7、括号 2×9、`/` `%` 7×7，白色），文本形态 `HP [50/50]`、`MP [50/50]`、`EXP [0.00%]` | 数字基线与参考偏差 **≤2px**；字模宽高 = 5×7；不再发黄 | [coords.md](evidence/w1w3-hud/coords.md) §1.1 |
| B2 | 三条量规位置 | `gauge/bar`/`graduation` 按**同行三联图集** 1:1 画，原点 rel `(218,38)`，段偏移 `0/109/222` | HP/MP/EXP 三段界与暗框对齐；红按钮不再被当成段界 | 同上 §1 |
| B3 | 职业名 / 角色名 | 两行落在 `backgrnd2` 下带：职业名 rel `(87,40)`、角色名 rel `(87,56)`（旧值 21/39 压在聊天输入行 = 「职业没显示」真因） | 两行都在深色面板带内、白字可读 | 同上 §1 |
| B4 | LV 数字 | `Basic.img/LevelNo` 橙盒位图（11×13），在 LV 盒（x0..84）内居中；「LV.」字样**不重影**（已烤在底板） | 单字中心 rel x≈55；无双层描边 | 同上 §1 |
| B5 | 闪光覆盖层 | `hpFlash/mpFlash` 109×18 落在各自分段可视框（rel y=+13），不再画到名字亮板上 | 掉血瞬间闪烁位置正确 | 同上 §1 |
| B6 | 快捷格面板 | 宽屏 26 格 → `base/quickSlot26` 466×80 **两行**；面板贴右缘 x = VWIDTH−466（1280 屏 = 814） | 两行 13 列；格几何 28/35/34、首格 (9,9) | 同上 §2.1 |
| B7 | 键帽层 | 每格左上角有键名：`Shift/Ins/Hm/Pup/Ctrl/Del/End` 用 `key/0..6` 贴纸；`1..5` 用 `FontMemo` 位图数字；字母与 `Pdn` 用描边文字回退（**本包无字母位图字体**） | 键名可见且不压图标；中文技能名在格下半带可读 | 同上 §2.3 |
| B8 | 快捷格收起/展开 | `QuickSlot`(上箭头) 与 `QuickSlotD`(下箭头) 是**同位单钮**，图标随状态切换（不是两颗并存） | 点一次收起、图标变下箭头 | 同上 §2.2 |
| B9 | 聊天栏「目标对象」 | 白区左侧有 `base/chatTarget` 81×20 目标条 + `BtWhisper` 12×19，文字左对齐 x=14，12..81 可点切换目标 | 有目标条、能点；`_ui_asset_diag.log` 里出现 `chat/ingame`（证明走的是可用回退而非静默透明） | [coords.md](evidence/w4-chatbar/coords.md) |
| B10 | 聊天栏展开/缩放 | 用真件 `Basic.img/BtMax`/`BtMin`（互斥出现，行内 rel (528,6) → 屏 x=536），手搓 12×12 命中区已删 | 两个态都能点、图标不重叠 | 同上 |
| B11 | **聊天栏点击无反应（功能级根因）** | classic 模式 `is_in_range` 曾把命中区整体上移 100px → 光标事件从不派发给聊天栏。本轮修在聊天栏内部，**预期是能点输入框了** | 点击白区能聚焦输入；回车开关输入框不退化 | 同上（W4「额外根因」） |
| B12 | 「帮助」钮 | 打开**脚本窗**：`Npt_GuideLine.img/NptGuide/Text00..Text48` 49 条中文，滚动条 `Basic.img/VScr`，关闭 `BtUIClose`/`BtClose2`；不再用 `Login.img/Notice` | 中文不乱码、能滚到底、能关 | [coords.md](evidence/w2-hud-buttons/coords.md) T-1 |
| B13 | 「界面」钮 | **只**打开快捷面板；面板用 `UIWindow.img/ShortCut` 的 11 颗真 `Bt*`（`x=6, y=20+i*20`，底板 93×245），位置改为锚底栏 `BtShort`（1280×720 → (722,404)）；**不再一同弹键盘设定** | 单窗、11 图标齐全、点每颗开对应窗口 | 同上 T-2 |
| B14 | 「商城」钮 | 聊天区给出明确提示「商城暂未支持。」（YELLOW），**没有**换 opcode（40 vs 229 属 s9 线，另案） | 不再是「点了没反应」 | 同上 T-3 |
| B15 | 「键盘设定」钮（`KeySet`）与窗内 5 颗钮 | 5 颗（确定/取消/取消修改/全部消除/变更快捷键）此前没坐标被 W0 哨兵**整批跳过** → 现在按 `键盘设置.png` 实测落在同一行 y=234：`8/58/112/177/260` | 5 颗都出现且行对齐；关闭钮在 `(W−18,3)` | 同上 T-4 |
| B16 | 「目录」钮（`BtMenu`） | ✅ **本轮已实装**（原记「未实装」作废）：底板 `UIWindow.img/GameMenu/backgrnd` 93×140 + 5 颗 81×25 真钮 `x=6, y=23+i*20`。落点：频道变更→`UIChannel`、游戏设置/系统设置→`UIOptionMenu`、游戏结束→`UIQuit(stats)`、**场景变更→黄字「场景变更暂未支持。」（显式预留）**。英文调试提示只在缺素材回退壳上出现 | 5 颗都可见可点；除「场景变更」外各开对窗；Esc 能关；**注意 `UIOptionMenu` 因本包 `GameOpt/SysOpt` 只有 `backgrnd` 而仍是空壳（§C3 of open-issues），不算本条失败** | [notes/open-issues.md §A2](notes/open-issues.md) |

---

## C. 悬浮提示（W5）

对照 `当前客户端截图\鼠标悬浮道具上显示的信息.png`。

| # | 看什么 | 预期 | 判据 |
| --- | --- | --- | --- |
| C1 | 道具/装备/技能/文本/地图 5 类 tooltip 的**底** | 框体真身定案为**程序绘制** `TooltipBackdrop`（黑 0.9α + 1px 灰边 (0.5,0.5,0.5)）；5 处 `UI["UIToolTip.img"]["Item"]` 引用全部清零（`UIToolTip` 在本包不存在、原生 exe 内 0 命中；包内仅 5 族九切片被像素判否） | 有底、有边、不再透明 |
| C2 | 需求/属性图标 | 根改到 `UIWindow.img/ToolTip/Equip/{Can,Cannot,Property,GrowthDisabled,GrowthEnabled,Speed,WeaponCategory,ItemCategory}` + `cash`/`mesos` | 属性图标显示；`Growth*` 路径与原生 exe 字面量一致 |
| C3 | 装备 tooltip 宽度 | 固定框宽 264 | 与参考一致 |
| C4 | 长中文换行（**闭环 #29**） | 复用 T12b 度量：预测宽 vs 实际渲染宽 ≤2px | 换行处不截字、不出框 |

## D. 角色类窗口（W6 能力值 / W7 装备 / W8 背包 / W9 技能）

| # | 窗口 | 看什么 | 预期 | 出处 |
| --- | --- | --- | --- | --- |
| D1 | 能力值（对照 `角色能力.png`、`打开了S-E-I-K页面.png`） | 8 行身份 + 4 行属性重排；`basicStat[159x121]` 蓝头**总是**画在 (8,195)；13 行数值重测（旧 DEX/INT/LUK 分别低 10/18/28px） | 与参考逐边对齐 ≤2px | [coords.md](evidence/w6-stats/coords.md) |
| D2 | 能力值 | `+` 钮 `BtApUp`×6 在 (186,115…299)；`BtDetail[46x19]` 落右下角（角锚 kx=9/ky=4 → (153,324)） | 位置对；点击可展开 | 同上 |
| D3 | 能力值「收不回」（定案） | 收缩钮真身 = `Basic.img/BtHide`（13×12，SAD 模板匹配命中），在窗相对 (405,326)，**仅展开态可点**；`BtDetail` 本身也做开/关双保险。旧代码读不存在的 `main/BtDetailClose` → `set_detail(false)` 永不可达 | 展开→收起可反复 | 同上 |
| D4 | 能力值 | ⚠ 预留：`BtAuto` 仍未给坐标（本包无对应原生位）；`Disabled{STR,DEX,INT,LUK}` 用于不可加点态 | 不可加点时显示禁用贴片 | 同上 |
| D5 | 装备（对照 `角色装备.png`） | 「左上两钮弹二级密码」定案为**误读**：那是烤图的 `Equip/BtDetail`「详细信息」46×19 与 `Equip/BtCashshop`「Shop」32×11（全仓/全客户端/exe 内「二级密码」0 字节命中）。两颗**默认隐藏**（原生匹配 mae 55.8/61.1） | 不再莫名弹窗 | [coords.md](evidence/w7-equip/coords.md) |
| D6 | 装备 | 宠物区：`pet[177x181]` 面板 + `BtPet1/2/3` 三页签 + `BtPetEquipShow[67x18]`/`BtPetEquipHide[17x16]`，按右下锚算位；展开改 `dimension` | 可展开/收起 | 同上 |
| D7 | 装备 ⚠ | 预留：宠物槽**穿戴逻辑**未接（仅版式与开关）；modern 分支 6 颗（`btEffect/btSalon/BtConsumeSetting/BtException/BtShop/BtSlot`，节点在 UIEquipInventory.cpp:288-293）仍无坐标 → 被 W0 哨兵跳过并记日志，需确认是否属非 classic 代次 | 不算回归失败 | 同上 |
| D8 | 背包（对照 `背包栏（折叠）.png`、`背包栏展开.png`） | 分类 tab 底衬 = `Item/New/Tab0`(灰) + `New/Tab1`(粉=选中)，文字叠 `Item/Tab/enabled\|disabled`；`x=5+i*34, y=24`，**只有 0–4**（旧代码取到 5 → SETUP/ETC 错位的成因） | 5 个分类都有底 | [coords.md](evidence/w8-item/coords.md) |
| D9 | 背包 | `BtFull`/`BtSmall` 合并到同一个槽 `(W−51,4)`（此前两颗叠在 (109,6)），`BtGather`/`BtSort` 在 `(W−31,4)`，都在框内；`set_full` 按 `dimension.x()` 重排 CLOSE/GATHER/SORT/FULL/SMALL/CASHSHOP | 展开态按钮在窗内、金币标签不溢出（(141,275) < 289） | 同上 |
| D10 | 背包 | 滚动条复活（W0 根名钳制 `VScr9`→`VScr`）：x=157、Range(46,250)，行高按 slotmax | 可拖、行数随展开变化 | 同上 + W0 |
| D11 | 技能（对照 `ikse.png`） | 5 个 tab 补坐标（槽0 中心 21、间距 33，由新手圆点实测 (16,29) 反推）；行底 `skill0/skill1[141x35]` + `line[141x1]`；`CLASSIC_SKILL_OFFSET` (10,87)→**(7,99)**（旧值造成贴图/文字 12px 错位）；`+SP` → (132,119)；宏指令钮 = `Basic.img/BtMacro` (121,265)；滚动条 x156、top100/bottom242、4 行 | tab 可切、行底对齐、贴图与文字不错位 | [coords.md](evidence/w9-skill/coords.md) |
| D12 | 技能 ⚠ 待定 | 「缺新手技能图标」：顶部职业横幅 `Skill/recommend[0]`（105×35，origin 16,0）已接在 (5,20)，但**新手参考帧里看不到它** → 疑为高转职/选中态才显示。实机拍一张「有 recommend 的原生端」才能定案（或补 IDA） | 先确认是否真缺 | 同上「未决」 |

## E. 任务 / 地图（W10 任务窗、W11 小地图与大地图）

| # | 看什么 | 预期 | 出处 |
| --- | --- | --- | --- |
| E1 | 任务窗关闭钮（对照 `任务栏.png` vs `当前客户端截图\任务栏界面.png`） | 有钮。定案：那颗是 12×12 蓝底白叉 = `Basic.img/BtClose`（`BtUIClose` 32×15「X CLOSE」是对话框件，仅作后备） | [coords.md](evidence/w10-quest/coords.md) |
| E2 | 任务窗「寻找NPC的位置」/ 展开钮 | **坐标先量后建**（`measure` → `relayout_slots` → `try_maple_btn`），不再落 (0,0)；`BtMarkNpc` 回到底带 (458,370) | 同上 |
| E3 | 任务分类背景色 | 用真 `Quest/Tab{enabled,disabled}` + `icon2..icon9`；Tab 贴图状态倒置已纠正（选中＝白字那态）；组标题子弹改真 `icon4` | 同上 |
| E4 | 任务窗「点击不切换」 | 补 `send_cursor` + `row_index_at`，**单击即选中**，双击语义保留 | 同上 |
| E5 | 任务进度条 | `Gauge{frame[163x14], gauge[1x11], spot[1x11]}`（原生 exe 字符串已证） | 同上 |
| E6 | 「任务整理」定案 | 本包**无此控件**（导出 131 张 `Quest/*` 位图逐张看字，最接近的是 `Quest/BtAlert`＝「任务讯息」）→ `set_active(false)` 预留。PLAN §6-Q4 关闭 | 同上 §D |
| E7 | 任务窗 ⚠ 预留 | `BtOK/BtNo/BtAlert/BtMarkNpc` 无现成协议 → 按钮在、动作未接（**未发明封包**）；仅「放弃」复用既有 `QuestActionPacket(3, id)`。Tab 粉底/灰底、黑边框、选中行横条本包无位图 → **保留 ColorBox**（附实测色值），不是漏改 | 同上 |
| E8 | 小地图（对照 `当前客户端截图\小地图.png`） | 框体由 `MinMap`/`MaxMap` **九切片正确拼**（角件不拉伸、边件单轴拉伸、中心 `c` 铺内容区、按 origin 补偿，手法同 W12 三切片）；`Min` 三切片做收缩条；顶带补**地图名**（`String/Map`） | [coords.md](evidence/w11-map/coords.md) |
| E9 | 小地图按钮 | 越代件 `BtMin/BtMax/BtSmall/BtBig/BtNpc` 停止实例化（本包只有 `BtMap{d,mo,n,p}[36x12]`）；`BtMap` 归位到顶带右框内（旧 y=−6 会甩出框）；simpleMode 死取用 `MiniMap/Window` 回落并记 diag | 同上 |
| E10 | 小地图缩放样式 | `MaxMap` 顶带 72 → `MAX_ADJ` 40→43；展开/收缩靠 `BtMap` 或既有 toggle | 同上 |
| E11 | 大地图（对照 `当前客户端截图\打开W大地图.png`、`切换大地图.png`） | 不再拿 `Border["0"]`(60×33) 当整窗底板，改用 `Border{0..7}` **拼九切片窗框**；底图落内容区（`base_position=(7,33)` + 补偿 origin(320,0)）；默认位置 `PosMAP (100,35)` → **(313,64)**（654×521 在 1280×720 居中偏上），`PosMINIMAP (0,0)` → `(5,5)` | 同上 |
| E12 | 大地图 ⚠ 预留 | `ListNpc` NPC 列表、`WorldMap/WorldMapSearch` 搜索栏、`BtSearch/BtAutoFly_1/BtNaviRegister`、`MiniMapSimpleMode` 本包缺 → 显式预留（非静默透明）；头像用既有 MapHelper 点位机制（无独立件） | 同上 §D |

## F. 名牌（W12，`NameTag.img`）

| # | 看什么 | 预期 |
| --- | --- | --- |
| F1 | `NameTag.img/<id>{c,e,w,clr}` 用法 | 三切片 + **逐片 origin 补偿**（`w.origin.x == width`）；`clr` 按 `data_type()` 分派读（int/float/string；id 178 存字符串 `-4096`，`get_integer()` 对非数字串会抛 `node.cpp:169`）；`e` 平铺 |
| F2 | 框宽算式 | 左帽 + T12b `OutlinedText::width()` + 右帽，**0px 偏差**；缺素材 → 只画文字 + 一条 diag |
| F3 | 事实提醒 | 本包只有 id 3…254 中的 **92 个**；`medal`(130，独立 id 空间含 0/1)、`pet`(289) 各自独立；同一 id 三片**高度可不同**（id 40 = 32/34/19）；`clr` 是**文字色**（深底白字/浅底黑字），`Text` 只吃 `Color::Name` → 白字模 × `DrawArgument` 乘法着色（参 `GraphicsGL.cpp:1345-1353`） |
| F4 | ⚠ 未接 | 游戏内名牌仍走 `GraphicsGL.cpp:1316-1333` 的 `Text::Background::NAMETAG` 近似实现；接真 id 要改 `Npc/Mob/Char/UIStateGame`（**未授权**）→ 故 A3 的判据就是「与改前一致」 |

---

## G. 明确「不算失败」的已知边界

| 项 | 状态 |
| --- | --- |
| RC7 商城协议（出站 40 vs `CASHSHOP_OPERATION=229`，HUD 换态靠 recv `SET_CASH_SHOP=127`） | **不换**（跨 s9 协议线，挂 #24 T14）。本轮只把「点了没反应」变「明确提示未支持」 |
| NPC 对话乱码 | **不属本线** → 属 T13（IME/文本处理）。取证已转记：`UINpcTalk.cpp:467-519` 残留收尾 `#`、`:157-165` 逐字节打字机 |
| 「目录」`GameMenu` 5 钮 | **已实装**（底板 + 5 颗 81×25 真钮；频道/游戏设置/系统设置/游戏结束均有落点，仅「场景变更」显式预留黄字提示）→ 见 [notes/open-issues.md §A2](notes/open-issues.md) |
| 小地图 −/＋ 缩放钮 | 本包无 `BtMin/BtMax/BtSmall/BtBig/BtNpc` 与 `Window/Max` 子树，**素材代次差异**，无法还原（§C2） |
| 宠物槽穿戴、任务 `BtOK/BtNo/BtAlert/BtMarkNpc` 动作 | 版式就绪、动作预留（不发明协议） |
| 大地图 NPC 列表 / 搜索栏 / 简单模式 | 本包无件，显式预留 |
| 游戏内名牌接真 id | 需跨 4 个文件授权 |
| `BtAuto`（能力值自动加点）、modern 装备分支 6 钮 | 无原生依据，留哨兵 + diag |

---

## H. 判据不满足时怎么定位

1. 先看 `_ui_asset_diag.log` 有没有对应 tag → 有 = 引用了本包不存在的根（素材判断问题）。
2. 再翻对应 `evidence/wNN-*/coords.md` 的「依据」列：①烤图不重画 ②角锚算术 ③截图实测。
   - ③ 得出的偏 → 大概率是参考图裁切原点/`frame→client = (0, −38)` 换算问题；
   - ② 得出的偏 → 是 k 常数问题；
   - 出现双影/文字重影 → 违反 ①（背景已烤的字又被 `Text` 画了一遍）。
3. `nx_inspect` 的 `origin` 本轮已修（PKG4 向量内联 `struct.unpack("<hh", ...)`），**可信**；节点清单见 [asset-node-inventory.md](notes/asset-node-inventory.md)（其中 §0.2「origin 不可信」一条已被该修复作废）。

---

## I. 真机验证轮结果（构建 `6AE608CB`，23:27–23:50，我驱动客户端自证）

### I.1 我已自证通过 ⇒ **不用再帮我看**
| 项 | 判据 | 证据 |
| --- | --- | --- |
| A7 `UIStatusBar` 空指针崩溃（`[` 在商城之后必崩） | 已修且**线上复验**：`` ` ``→`[`→ 存活；随后整轮扫描全程未崩 | [open-issues §A7](notes/open-issues.md) |
| B16「目录」= UIGameMenu 5 真钮 | 实拍确认 5 颗齐全可点 | §A2 |
| D11 技能窗（`k`） | tab / 职业横幅 / 技能行实拍在位 | `_v_*` |
| D5/D6 装备窗（`e`） | 窗体、槽位网格、`魔物扩展区` 钮实拍在位 | `_v_A1.png` |
| D8/D9 背包 tab 文案、`扩充道具栏` 文案 | **逐字与参考一致**（装备/消耗/设置/其它/特殊） | `_v_ztabs.png` `_v_zbtn.png` vs `_ref_inv.png` |
| D8 背包空格粉色 ✕ 底纹 | **与参考一致，不是缺素材** | §D5 |
| `i`/`e` 重复堆窗 | **证伪**：`UI::emplace` 复用并 toggle | §D7 |
| B1/B2/B3 底栏三段量规、职业名/角色名 | 底栏实拍与参考版式一致 | `_v_A0.png` |

### I.2 本轮**新增**缺陷 ⇒ 需要你拍板或一起看
| 项 | 现象 | 分类 | 出处 |
| --- | --- | --- | --- |
| **I-1 键位 id 表错位** | 7 个死键（`5 6 h g l o f`）+ 3 个语义错位（`` ` ``不开商城、`b` 发 `EnterCashShopPacket`、`;` 开成 Boss 列表）；`case MONSTERCOLLECTION` 永不可达 | **需抉择**（动协议枚举，甲/乙/丙三案） | [open-issues §F](notes/open-issues.md) |
| **I-2 tooltip 五条** | 底不透明缺失（透出金币条）、属性两列重叠、数值渲染成 `000`、稀有度行是黑块、**英文调试串泄漏**（`Type: BOTTOM`/`Remaining Enhancements`/`Hammers Applied`） | 我能自己修（W5 判为未完成） | §B8 |
| I-3 背包窗不响应 `Esc` | 装备窗响应、背包窗不响应；v83 背包本就无 ✕ ⇒ 只能靠再按一次 `i` | 我能自己修 | §B9 |
| I-4 ~~小地图地图名跨场景不刷新~~ | ❌ **追加轮撤回**：角色整轮从没真的换过图，出生图在 v83 中文表里就叫「番茄田」，画面里的路牌/电话亭是出生图美术。代码路径是通的（`UIMiniMap.cpp:161-165` 比 mapid、`:242-247` 换图时重算文本/按钮/画布） | 无需修 | §B10 |
| I-5 关窗后第一个键被吞 | `UIStateGame.cpp:219-224` 清 `focused` 后直接 `return` | 需确认（逐窗失焦语义） | §F 末 |
| I-6 C4 背包中间那颗红 `+` 缺失 | 参考 3 颗、线上 2 颗；`BT_SORT`/`BT_GATHER` 叠同一点 | 我能自证（`nx_inspect` 数 `Item/Bt*`） | §C4 |
| I-7 `_ui_asset_diag.log` 每次启动都写 | 缺件清单探针 | **需抉择**（留 / 加开关 / 删） | §B11 |

### I.4 追加轮（00:00–00:20，构建 `14A40006` / md5 `13eba5ac7782ece2`，**尚未部署**）
上一轮列出的「我能自己修」的，这一轮**全部落地**了；同时抓出一个影响所有线上结论的工具级坑。

| 项 | 处置 | 证据 / 位置 |
| --- | --- | --- |
| I-2 之 B8.2 需求两列重叠 | ✅ 已修：DEX/INT 行 y `71→77`，三行统一 15px | [IO/Components/EquipTooltip.cpp:57-64](file:///E:/project/MapleStory-Client/IO/Components/EquipTooltip.cpp) |
| I-2 之 B8.3b 补零下溢崩溃 | ✅ 已修：`reqstr.size() < 3` 守卫（≥4 位需求值会抛 `length_error`） | `EquipTooltip.cpp:381` |
| I-2 之 B8.4 职业名行「纯黑块」 | ✅ **已修，且归因翻案**：不是缺素材（`nx_inspect` 证明 `ToolTip/Equip/Can\|Cannot` 六颗职业名位图全在），是 `for (jbit : okjobs) … .draw(job_position)` **不推进 x**，6 张半透明位图叠画在同一点。主窗 + 对比预览窗两处都改成 `width()+2` 横排 | `EquipTooltip.cpp:649-658`、`:747-756` |
| I-2 之 B8.1 金币条透出 | ⏸ **素材侧定案**：`UIWindow.img/ToolTip` 下**只有 `Equip`**，原生 `backdrop` 位图本包确实没有 ⇒ 只能调程序描边的 α（`TextTooltip.h:37` 的 `0.9f`）。**需你拍板**：0.9 透 / 1.0 实 / 先给一张原生 tooltip 参考帧 | §B8.1 |
| I-2 之 B8.5 英文串 | ⏸ 不改：换中文需要 v83 原生文案依据（`String.img`/IDA），**禁止凭记忆编** | §B8.5 |
| I-3 + I-5（同一条根因） | ✅ 已修：`UIStateGame::send_key` 清脏焦点后**继续**下发，不再 `return` 吞键 ⇒ 「第一下 Esc 没反应、要按第二下」应当消失。**背包窗本来就写了 Esc 处理**（`UIItemInventory.cpp:939-946`），我原先「背包没写 Esc」的判断是错的 | `IO/UIStateGame.cpp:211-226`、§B9 |
| I-6 背包中间那颗红 `+` | ✅ 已修：classic 包没有 `AutoBuild/FullAutoBuild` ⇒ `_SM` 那对按钮根本不创建，展开态却只点亮 `_SM`，中间那颗就消失了；现在 `_SM` 缺席时回退到公用的一对。另把 `sort_enabled` 初值 `true→false`（原生开窗默认显示 `BtGather`，不是 `BtSort`） | [IO/UITypes/UIItemInventory.cpp:112](file:///E:/project/MapleStory-Client/IO/UITypes/UIItemInventory.cpp)、`set_sort()` |
| **工具坑：驱动没抢到前台** | ⚠ 本轮复测 `i`/`e` 全部「没反应」，实测 `GetForegroundWindow()` 是**原生「北斗」客户端**（`MapleStoryClass`，你正在里面截图，屏上还弹着「保存图像」），我的测试客户端已被最小化。旧 `_drive.py` 不看 `SetForegroundWindow` 返回值，照样打印 `tapped i x1` ⇒ **所有「按键没反应」的结论都必须重测**。已改为 attach+裸 ALT 解锁+校验 `fg==hwnd`，抢不到就 ABORT/退出码 3；另加 `_postkey.py`（`PostMessage` 直投 SDL 队列，后台可用） | §B5 第四条 |
| 撤回一条旧结论 | `g` 是死键的**线上探针**作废（键位错位的结论本身不受影响，它靠 `4f 01` 封包解码 + `grep case` 得出） | §F |
| 顺带记一笔（**未改**） | `UIStateGame.cpp:296` 编译告警 C4701「`tab` 可能未初始化」：外层 `switch` 已保证 `action ∈ {FRIENDS,PARTY,BOSSPARTY}`，三个 `case` 全覆盖 ⇒ 实际不会未初始化，属告警噪声。要不要加个初值消警告，等你点头再动 | `IO/UIStateGame.cpp:277` |
| I-4 小地图地图名「不刷新」 | ❌ **撤回**：见上表 I-4 行 —— 换图重算的代码路径本来是通的（`UIMiniMap.cpp:161-165` + `:242-247`），我误把出生图美术当成了别的图 | §B10 |

**部署状态（2026-11-01 19:5x 更新）**：`build_check\MapleStory.exe` = md5 `6f0a8bc10be012bd…` / 4,422,144 B（18:53:30 构建，EXITCODE=0，
协议门禁 GATE OK）。**已部署**：锁在 19:5x 自然释放（旧实例已退出，未结束任何进程），覆盖后目标目录 md5 与 `build_check` 一致，
运行中的就是这一版。R27-B2 的复验判据已达成 —— `check ItemFull.json live-postfix-expanded.png` **9/9 不合格 0**、
`full_grid_rows` 从改前 12 条双行回到 6 行、6 颗 disabled 由 Δ=(+1,+1) 回到 Δ=(0,0)，折叠态 4/4、能力窗 8/8 同样全绿；
读数与三张真机帧见 [evidence/r27-b2-bagfull/](evidence/r27-b2-bagfull/README.md)。
顺带把这次部署解锁的两条「要我驱动才能收」验掉：
**B9/I-5 ✅**（开背包 → **一下** `Esc` 就关，连做两次可复现：agree 0.890→0.058→0.890→0.058）；
**C4 ✅**（展开态顶部红条/加号在位：真机窗口内红像素 1887 vs 原生 1920，`x 290..350 / y 20..60` 两处裁片逐块同形）。

**部署状态（2026-11-02 10:4x 更新 —— B2 已在新件上重跑，台账①那一行销掉）**：线上件被另一条会话连换两版
（`d728fc99…` 4,462,080 B @07:46 → **`d780ce77…` 4,462,592 B @10:36**，含未提交的 `UIMapSearchPanel`/`UIWorldMap`/`UIMapInfoPanel`）。
我这次是自己启动客户端、自己进图、自己重跑三张 spec：**展开态 9/9、折叠态 4/4、能力窗 8/8（跳过 1）不合格 0**，
非零项仍只有 `*_ink` 的 `x+w −1`（SimHei 右 bearing，已定案 `delta_tol=1`）。读数与三张帧见
[evidence/r27-b2-bagfull/rerun-d780ce77.txt](evidence/r27-b2-bagfull/rerun-d780ce77.txt)。
两条新记的工具坑：
① **首次按 `i` 后 1.2 s 抓帧，背包窗根本不在帧里**（agree=0.062，被 0.60 闸拦下），补按一次 + settle 3.0 s 才正常
　　⇒ `live/open_window.py` 的 1.2 s 对「首次开窗要加载素材」偏短；这类帧必须先过 agree 闸再谈版式。
② **「我验的是哪一版」会自己漂**：部署件在我启动客户端前 3 分钟刚被换过，身份只能靠
　　*进程启动时刻 vs exe mtime* 对齐，不能靠上一次登记的 md5。

### I.3 仍然只剩你能做的（追加轮已合并成一张单子）
**要你拍板的（3 条，都是决策不是体力活）**
1. **§F 键位 id 错位**：甲（在 KeymapHandler 边界做 v83→本表翻译，推荐）/ 乙（`KeyAction.h` 整体改号 + 存档迁移）/ 丙（只补 7 个死键）。
2. **B8.1 tooltip 底**：`0.9α` 保持透 / 改 `1.0` 实心 / ~~你先给一张原生 tooltip 参考帧再定~~
   **参考帧已到位**（桌面 `角色装备栏显示物品信息.png` 等 4 张，见 §I.3）⇒ 这条不再需要取证，
   可按帧量 α 后直接定；原生 `ToolTip/backdrop` 位图本包确认缺失这一点不变。
3. **B11 `_ui_asset_diag.log`**：留 / 加配置开关默认静默（推荐）/ 删。

**要你在原生「北斗」端帮我拍的参考帧（我这边取不到）**
- ~~**tooltip**：悬停一件装备…桌面 16 张里一张 tooltip 都没有~~ —— **本条作废（2026-11-02 数错）**。
  桌面目录现有 **58 张**，其中 4 张正是 tooltip/悬浮信息，B8.1、B8.3、B8.5、R15-4 **不再卡取证**，
  已降级为「我这边对着帧量」的活：
  `角色装备栏显示物品信息.png` 359x404（装备 tooltip，B3b 主参照）、
  `背包中鼠标悬浮物品上显示的物品信息内容.png` 268x310、
  `背包栏鼠标悬浮按钮上显示按钮功能提示信息.png` 175x290（按钮功能提示）、
  `角色头上提示信息.png` 150x148（他人信息气泡）。
- **真的换一次图**：走传送门或 GM 传一张别的图，拍小地图顶带 ⇒ 验「换图后地图名是否重算」（B10 撤回后剩下的那条真问题）。
- **公告滚动中途**（A6）：我这边滚太快只拍到滚完的空带。
- **技能窗有 recommend 的原生端**（D12）：验「新手技能图标」。

**要我驱动线上客户端才能收的（部署完一次性做完，你只需看着）**
- ✅ B9/I-5（19:5x 已验）：`i` 开背包 → **一下** `Esc` 就关，两轮可复现（agree 0.890→0.058→0.890→0.058）。
- ✅ C4（19:5x 已验）：展开态顶部红条/加号在位，与原生同形同位（红像素 1887 vs 1920）。
- B8.2/B8.4：tooltip 需求三行不再叠、职业名横排。
- B3：「界面」钮（UIShortCut）双开回归（W2 四按钮路径，`emplace` 复用那条证伪**不覆盖**它）。
- B2：小地图 MIN/NORMAL/MAX 三态对 `进入游戏页.png`（本图 `has_map=false`，只能验顶带）。
- **A1 段落点**：A5 修好后，凡 `Settings` 里有历史拖动值的窗体会突然开始听存档 ⇒ 逐窗前先确认该键 == 代码默认值。

---

## J. R23/R24/R25 轮（2026-11-01 05:48 增量构建 → **登录即崩，已废**；改全量 `-t:Rebuild`）

> **构建身份已换，别再看 `6AE65CF2`**：`MapleStory.pdb`/`.exe` 的 COFF `TimeDateStamp` 恒为 `0x0`
> （确定性构建），所以身份只能用 **md5 + size + mtime**。
> - `05:48` 那份（md5 `0688b7c146e2eccb9fbd0df198acf2bc`，4,340,736 B）**已部署但 100% 登录崩溃**，作废。
> - 根因与取证：[evidence/r26-login-crash/README.md](evidence/r26-login-crash/README.md)
>   —— 给 `Foothold`/`PhysicsObject` 加成员改了类布局，增量构建留下 178 个旧 `.obj` ⇒ ODR 错乱。
> - 全量重建后的新 md5 见本文件末尾 §J.5（部署前回填）。

这一轮对应你最新点的三组现象。代码全部落地、`error C`/`error LNK` 计数 0。
取证与机制说明：[evidence/r23-map-motion/README.md](evidence/r23-map-motion/README.md)、
[evidence/r24-ui-geometry/README.md](evidence/r24-ui-geometry/README.md)、
[evidence/r25-npctalk-geometry/README.md](evidence/r25-npctalk-geometry/README.md)。

### J.1 履带：站上去要跟着走（R23-A）

| # | 验什么 | 判据 | 谁能验 |
| --- | --- | --- | --- |
| J1-1 | 番茄田（`mapid=1`/`2`，15+13 条 `force` 落脚点）站上传送带 | 角色被水平推着走，**不需要按键**；反向走能挣脱 | 我驱动 |
| J1-2 | 速度量级 | 约 0.8 px/tick（比走路略慢），不是"弹射"也不是"蜗牛" | 我驱动（抓帧量位移） |
| J1-3 | 离开履带后不残留惯性 | 走出/跳下 `force` 落脚点那一帧起 `x` 不再被推 | 我驱动 |
| J1-4 | 回归：普通平台站立/行走/跳跃不受影响 | 摩擦滑行（`GROUNDSLIP`）手感同修前 | 我驱动 |
| J1-5 | 同类受益面抽查一张非番茄田的 | `Map9/922240000`（123 条）或 `Map2/220020200`（7 条） | 需你传图或给我 mapid |

> **未接**：`forbidFallDown`（322/400 抽样）仍是零实现 ⇒ 见 open-issues **C12**，不算失败。

### J.2 地图上的 NPC / 怪物（R23-B）

| # | 验什么 | 判据 | 谁能验 |
| --- | --- | --- | --- |
| J2-1 | 番茄田那颗「莎丽」 | 站在**她自己的平台上**，不再掉到路下面 | 我驱动 |
| J2-2 | 「有的 NPC 不见了」 | 修前被丢到 `borders` 外的 NPC 回来；**素材侧已排除**：地图摆的 827 个怪 id 全有 `Mob.nx`、1280 个 NPC id 只缺 1 个 | 我驱动 |
| J2-3 | 脚本挪位 / 控制器重下发同一 oid | NPC 跟到新坐标（修前只 `makeactive()`，新坐标整包丢掉） | 需触发动作的图 |
| J2-4 | 8 位 npcid 的对话不再打崩 | `extend_id` 取代 `insert(0, 7-len, '0')` 下溢 | 需一颗 8 位 id 的 NPC |
| J2-5 | 怪物不受影响（回归） | 怪物包本来就是真 y，落位应与修前一致 | 我驱动 |

### J.3 NPC 对话窗（R23-B + R25）

对桌面 `正确客户端截图\NPC对话.png`、`NPC对话2.png` 与你给的 528×261 参考帧：

| # | 验什么 | 判据 |
| --- | --- | --- |
| J3-1 | 正文起笔/折行 | `TEXT_X=166`、折行宽 **310**（右缘 x476）；**正文不再压到「确定」**。<br>⚠ 早前写的 (122, 35)/387 是把网格叠画图看走眼编的，已作废（见 r25 README §5） |
| J3-2 | 立绘朝向 | 希娜面向正文（朝右），不是背对 |
| J3-3 | 长文本翻页 | 每翻一页正好一屏（壳高 − 18px 余量），不跳字 |
| J3-4 | 命中区 | 窗口下方空白带不再吞点击（`dimension` = t+body+s） |
| J3-5 | 底带按钮槽位 | 「结束对话」左 **x12**；「确定/下项」右 **x474**；成对间距 **65**；纵向一律 `正文高 + 49` |
| J3-6 | 接受/拒绝顺序 | 主操作在**左**(401)、次操作在**右**(466) —— 原生就是这个顺序，不是"确定在右" |
| J3-7 | **`TEXT_Y` 待你眼睛定** | 三帧墨迹顶 39/58/72 不等，疑原生按正文高度**垂直居中**；现取 48。请拍一张对一下 |
| J3-8 | **「下项」槽位冲突待你眼睛定** | `NPC对话2.png` 量到 x450，参考帧量到 x474（差 24px）。现锚 474，需同场景真机帧裁决 |
| J3-9 | `SENDSIMPLE` 无动作钮 | **点了没反应 = 预期**（显式预留，见 open-issues **A11b**）；列表本身可点、可发 selection |

### J.4 任务窗（R23-C）—— 一条 `dimension` 笔误同时造成三条"点了没反应"

| # | 验什么 | 判据 | 谁能验 |
| --- | --- | --- | --- |
| J4-1 | 展开态命中宽 | 307 → **548**（= `backgrnd` 245 + `backgrnd2` 305） | 我驱动 |
| J4-2 | 「详情」钮 | 现在**点得到**，正文在「简述 ↔ 任务编号/等级/区域/NPC/前置」间切 | 我驱动 |
| J4-3 | 右板收缩钮 | 现在**点得到**，展开↔245 宽收缩态切换 | 我驱动 |
| J4-4 | 正文溢出 | 按 `detail_area.width()` 折行 + 上下缘裁剪 ⇒ 不再铺出窗口 | 我驱动 |
| J4-5 | 列表底色重叠 | 选中行 199@x25、分组行 219@x7，右缘收进 226，**不从滚动条底下透出** | 我驱动 |
| J4-6 | 按区域展开/收起 | 三个分类页都有分组标题行，**点标题行**即收起/展开该区域 | 我驱动 |
| J4-7 | 任务 NPC 立绘 | 蓝横幅右格（窗口 x433..540、y26..118）出现该任务的 NPC | 我驱动 |
| J4-8 | 「寻找NPC位置」 | **仍无反应 = 预期**（显式预留，见 open-issues **C13**） | 你确认口径 |

### J.5 部署与验收顺序（本轮实际走过的）

1. 备份：`E:/MXD/BeiDou-Client-xy/MapleStory.exe.bak_r24-20261101`（md5 `51d3fe17…`，上一份能跑的）。
2. 退出：`_quit.py` 走 `WM_CLOSE` → 悬停确认红色高亮 → 点「确定」，**全程没杀过进程**。
3. 覆盖 `MapleStory.exe` + `.pdb`，回读 md5/size 校验。
4. **第一关只验「能否登录进图」**，判据不能再用 `_boot.py` 的底部亮度
   （NEXON splash 白底会假阳性，见 open-issues **A13**）；改用 `_drive.py where` 有窗口
   + 抓帧能看到底栏血蓝条/小地图顶带。
5. 第一关过了，再跑 J.1 → J.4。

**新构建身份（全量 `-t:Rebuild` 后回填）**：md5 `9636f77b4030ab4d9a85379cb0694911`，size `4,419,584 B`，mtime `2026-11-01 14:16:01`。

> **R26 已闭环（2026-11-01 14:17 真机自证）**：全量重建后 `x64/obj-Release` 里 **0/274** 个 `.obj` 早于
> `Gameplay/Combat/Attack.h`（增量构建时是 **200/274**）；`_boot.py wait` 10s 到 HUD，抓帧确认不是 splash 假阳性
> —— 已进图（彩虹岛：蘑菇森林），底栏血蓝/经验条、小地图顶带、聊天页签全部正常，怪物带血条正常刷新。
> 修前那份 `0688b7c1…` 是**登录进图 100% 崩**，现在同一台机同一份存档一次进图。⇒ §J.1–§J.4 的验收前提成立。

---

## K. 玩法轮 J1–J8（怪物加载 / 技能数据 / 出站封包 / buff 掩码 / 战神连击 / 三家族创建）

构建身份：md5 `9636f77b…`（全量 `-t:Rebuild`，2026-11-01 14:16）。
取证与机制：[r805](../beidou-v17-parity/notes/r805-buff-mask-aran-combo.md)、
[r806](../beidou-v17-parity/notes/r806-out-attack-packet-layout.md)、
[J 段门禁复跑记录](../beidou-v17-parity/notes/r807-j8-rebuild-gate.md)。

### K.1 我已自证通过 ⇒ 不用再帮我看

| # | 项 | 自证方式 |
| --- | --- | --- |
| K1-1 | 登录进图不崩（R26 脏 obj） | 真机 `_boot.py wait` → 抓帧确认在图内（§J.5 末） |
| K1-2 | 怪物能出现、带血条、会动（J1） | 同一帧里 5 颗 蓝蜗牛/香菇怪 全带 `100/100` 名牌 |
| K1-3 | 出站攻击封包字节布局（J7） | `python tools/attack_layout_check.py` ⇒ **0/13 异常**；`--legacy` ⇒ **9/13 异常**（反向对照） |
| K1-4 | 协议门禁 | `tools\build_smoke.bat` ⇒ `GATE OK`（s9 `44d0d5d9ca`）+ `EXITCODE=0`，0 error |

### K.2 只剩你能验的（需要特定职业 / 特定技能 / 队友）

| # | 验什么 | 判据 | 前置 |
| --- | --- | --- | --- |
| K2-1 | 战神（Aran）打怪涨连击 | 命中怪物才 +1；停手 3s 归零；每满 10 层给一次 buff；`21100004/21110004/21120006` 终结技后清零 | 需一个 2100–2112 的号（⇒ K2-6） |
| K2-2 | 组队 buff 全显示 | 队友上 加速/祝福/神圣之翼 等，我方 buff 栏**逐个**出现（修前只显示第一个） | 需第二个号或队友 |
| K2-3 | 魔法盾 / 魔法抵抗不再串号 | 上这两个 buff 后 buff 栏不再把别的 buff 顶掉（掩码位表已按服务端 `StatUpgrade` 逐位校准） | 法师 4 转号 |
| K2-4 | Hyper Body 的 HP%/MP% | 面板 HP 上限与 MP 上限**都**按同一百分比涨（修前两者取值互换） | 需该技能 |
| K2-5 | 四条 J7 伤害路径真机 | `BIG_BANG`/`GRENADE`/`POISON_BOMB`/`CORKSCREW_BLOW` 有伤害；飓风/穿透箭/速射**多目标**都有伤害；竹林一次最多报 15 怪 | 对应职业号 |
| K2-6 | 三家族创建（J3） | 选角页 → 创建 → RaceSelect 左/中/右（骑士团/冒险家/战神）→ 捏脸 → 起名 → 能进图 | 服务端闸门**已证放开**（r808），只剩客户端流程 |

### K.3 明确的边界（不是 bug，别按失败算）

1. **暴怒（`4211006`）只掉血不炸金币堆**：伤害块已按服务端专用分支写对，但本客户端没有
   「哪个金币堆是我的掉落」的所有权集合，伪造 oid 会让服务端 `:210` 的 ITEM 校验直接吞掉整包伤害。
   ⇒ 需要单独一条功能线（掉落物归属），本轮显式不做。
2. **一次攻击最多报 15 颗怪**：`numAttackedAndDamage` 高 4 位的格式上限；本包 `mobCount` 最大是 30
   （竹林 `0001009/10001009/20001009`），多出的目标服务端本来就读不到，现按 15 截断而不是写成 0。
3. **战神连击计数窗（`UIComboCounter`）没有原生美术**：`UIWindow.img` 里查无 `Combo`/`Number` 节点，
   计数逻辑与收发包已就绪，显示层是空壳 ⇒ 需要出图（见 ui-photoshop 流程）。
4. **`PYRAMID_PQ = 0x20000000000` 是个名字写错的既有常量**：位号与服务端 BERSERK 位一致（所以行为对），
   只是符号名误导；改名会牵动多处，本轮不动。
5. **J2-③ 主动收窄**：技能暴击/攻击位那一组里，`critiDamage` 一类节点在本包 `Skill.nx` 中实测不存在，
   已按证据删掉对应读取，不是漏做。

### K.4 收尾：提交拆分与推送台账

- 见 [r810-commit-split-ledger.md](../beidou-v17-parity/notes/r810-commit-split-ledger.md)：
  **29** 个提交，`169ea09..ef3b353`，已推 `origin/feature/dev_0.0.1`；推送前屏蔽了约 120 MB 联调副产物，
  并把 `port-095/evidence/world_props.txt` 里的数据库账号+口令脱敏后才入库。
- **⚠ 但同批台账自己漏了一次**（该台账 §6）：`97954a2` 抄录扫描命中行时把 095 参考端的**数据库口令明文**带进了
  公开仓库，已推送、现已在工作树掩码；账号名未泄露。**待你拍板**：换本机口令 / 重写历史强推 / 两者都做。
- 三条限制记在这儿备查：拆分粒度是**文件**而非 hunk（一个文件叠了多条轮次就无法再分）；
  `UIElement` / `PacketSwitch` / `vcxproj` 这类共享注册件只落一次；
  **中间提交不保证各自可构建**，只有最后一次源码提交 `358193d`（其后仅两笔文档提交）等于本轮真机验过的那份 `9636f77b4030ab4d9a85379cb0694911`。
- 一条新查出的收尾欠账：`Graphics/GraphicsGL.cpp:33` 的 `dbgatlas()` 仍会在 Windows Release 落 `atlas_dbg.log`
  （无开关、限 200 条），属登录页竖条纹轮遗留，随 #44 关闭时删除。
- 本机临时件已按「是否仍被引用」清过一遍（**1529 个 / 168.9 MB 删除，删后反查引用断链 0**）：
  判据脚本 [`tools/scratch_sweep.py`](tools/scratch_sweep.py)，清单与副作用见
  [`evidence/scratch-sweep-20261101/README.md`](evidence/scratch-sweep-20261101/README.md)。
  12 个 `_` 目录也已按同一判据清完：**删 9 个（85.3 MB），留 3 个 + 1 个活跃目录**——
  `_ida_beidou` 174 MB 是 IDA 数据库（删则要重跑整轮自动分析）、`_ms_probe` 30 MB 是 opcode 文档生成脚本的工作目录、
  `_w6_pngs` 仍被 `evidence/w6-stats/coords.md` 指向、`_audit` 是清理当时另一个会话正在写的取证目录（未碰）。
  删前补查了两次判据漏洞：语料白名单原本没有 `.vcxproj/.props/.targets`（SDL3 的实际取处在 `includes/SDL3`，
  `_sdl3dl` 只是下载缓存，确认可删），以及 `_nat` 的「40 条引用」其实是 `_native-samples` 子串撞名。

