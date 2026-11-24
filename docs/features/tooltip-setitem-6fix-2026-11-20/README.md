# 装备/套装 tooltip 6 问题二次修复（2026-11-20 晚）

> 个人学习/技术研究，单机离线。逐条行号级结论；截图来源已确认为本端 5fix build（含「（未鉴定）」字样）；真机未验收如实声明。

## 0. 背景

用户对本轮 5fix 部署（22:28）给出 4 张实拍 + 1 张高版本参考图，追加 6 项问题。用户确认截图来自 `MapleStory_tooltip_5fix_20261120.exe`（SHA 707ce08a…）。一次性全改。

## 1. 逐条根因与修复（行号为改后）

| # | 用户报的问题 | 根因（证据） | 修复位置 |
| --- | --- | --- | --- |
| 1 | 没有装备对比，括号里是强化提升？ | 括号是 `shown - defstat` 的卷轴/强化提升（`EquipTooltip.cpp:1208-1222`）；对比窗仅在「背包悬停 && 同部位已穿」时出现（`:485`），差值另由 `:1224-1244` 生成 | **非 bug，为设计如此**；未改逻辑，仅文档说明 |
| 2 | 「唯一道具/不可交换」「所有者」上下间距不一样 | `trade_line` 无冒号走 CENTER（`:180`），`owner_line` 有冒号走「冒号压中线」（`:186-202`），两行视觉中心不同 | 新增 `BodyLine::build_centered()`（`EquipTooltip.h:110`、`cpp:169`），trade/owner 统一整串居中（`:1132/:1143`、预览 `:718/:728`） |
| 3 | 套装名没居中还溢出、档位标题没颜色 | `title` 已是 CENTER 却画在 `pos.x+PAD_X`（`SetTooltip.cpp` draw），CENTER 原点即中心 → 以 x=10 为中心左溢出；档位色函数旧版仅 reached 上色，未达全灰 | 标题改画在 `pos.x+FRAME_WIDTH/2`（`:519`）；`tier_header_color()` 改 GAUGEGREEN 恒亮绿（`:175`，对齐用户参考图全绿档位标题） |
| 4 | 套装列表只剩「未鉴定」 | String/Eqp.img 与服务端 SetItemDO 都没有成员名字段/节点（orange-wz + server 源码实证）；旧「部位（未鉴定）」兜底不佳 | 按用户参考图命名法，从服务端权威套装名派生：`setName` 去「套装」+ 部位 →「冒险岛宝石帽子/上衣/鞋…」（`derived_member_name()` `:140`，调用 `:369`）；服务端文案优先（`:300-305`） |
| 5 | 卷轴显示成装备 tooltip | `UIStateGame.cpp:1038` 旧 `EquipData::get(itemid)` 经 Cache::get 对任何有效道具为 true → 2041043 卷轴走 EquipTooltip。原生仅装备号段有 EquipTooltip | 改 `itemid/1000000==1`（`UIStateGame.cpp:1043`），卷轴/消耗/etc/cash 走 ItemTooltip（名称居中+图标+说明） |
| 6 | 新增：套装列表加装备图标、多选武器兼容 | 本端 `SetTooltip::Line` 仅 Text 无图标；live Character 里各成员图标齐全（01052357.img 等） | `SetTooltip.h:32-36` Line 加 Texture/icon/multi_weapon；cpp `:410-412` 取图标、`:410` 多选武器标记；draw `:528-547` 缩放绘制图标+「X」标记，名称右移 ICON_TEXT_X 水平对齐 |

## 2. 关键证据

- 截图来源：4 张实拍含「（未鉴定）」= 5fix build 独有 → 本端。
- orange-wz（128B key）实读 live `String/Eqp.img`：仅 1002357 有 name，1052357/1072521/1082314 等无；live Character/ 各成员图标 imgs 齐全（Cap/01002357, Longcoat/01052357, Shoes/01072521…）。
- 服务端 s9 `SetItemDO.java:25-34` 只有 `setName/itemIds`，无成员名字段；`PacketCreator.java:7827-7852` 0x176 结构无成员名 → 故放弃「0x176 带成员名」，改 setName 派生（已在回复中如实告知用户）。
- 构建曾与并行会话 crashfix 全量 Rebuild 抢 obj/ 锁中断一次；待其完成后隔离 IntDir 重编成功。

## 3. 构建与部署

- 脚本 `tools/_tooltip_6fix_build.bat`（Release x64，v142，隔离 IntDir `build-tooltip6fix-int/`）。
- 构建：`tooltip6fix_build.log` → `MSBUILD_EXIT=0`，`error C` = 0。
- 产物 `build-tooltip6fix-out/MapleStory.exe`，5,105,664 B，**SHA256 `8D33452682FD9A0C41D4F39E5E6A20B6F975E4746852CCD67862B5E853E8321C`**（23:28）。
- 部署（用户 22:39 明确「一次性全改」+ 既有部署授权）：live 唯一新名 `MapleStory_tooltip_6fix_20261120.exe`，**live 实测 SHA = 8D334526…E8321C**（与 build 一致，实时核对，防回滚）。locale 未改（本轮无新键），live 两份 ini SHA 与 build 一致。
- 未触碰并行会话的 `MapleStory_crashfix_20261120.exe`（SHA 41931f21…）及其它 variant exe。

## 4. 真机验收清单（待用户，截图须本端）

1. 卷轴/消耗品 tooltip：只显示居中标题 + 图标 + 说明（ItemTooltip），无需求/属性/对比。
2. 「唯一道具，不可交易」与「所有者：Admin」水平中心一致、上下等距。
3. 套装面板：标题水平居中不溢出；所有「N套装效果」标题亮绿。
4. 套装成员：显示「冒险岛宝石帽子/上衣/鞋…」等派生名，无「未鉴定」；名前有装备图标，水平对齐。
5. 多选武器行：显示「X」标记 + 组名。

## 5. 如实声明

- 已部署到 live 新命名 exe，但未改登录器启动目标、未做真机截图验收。
- 括号数值是强化提升、不是 bug；对比窗不出现是「同部位未穿装备/配置关闭」的正常行为。
