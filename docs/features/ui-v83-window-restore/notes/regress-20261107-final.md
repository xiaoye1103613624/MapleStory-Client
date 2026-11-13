# 2026-11-07 回归修复终账：14 项「之前好的功能又坏了」

## 结论总览（按项）

| # | 用户报 | 根因 | 处置 | 验证 |
|---|---|---|---|---|
| 1/2 | 属性/详情数值没了 | **主因：R28-B5 换思源宋体后装机 fonts/ 没同步** ⇒ FT_New_Face 失败 ⇒ 全部 FreeType 文字空白（位图/烘焙字正常，正是「数值消失」签名）。次因：混尺寸增量编译（见下） | 补 `fonts/SourceHanSerifSC` 到 `E:\MXD\BeiDou-Client-xy\fonts\`；整工程 Rebuild | r5b/reg_03_stats.png：名字/职业/等级/HP/MP/经验/人气全出数 |
| 3 | 底部名称职业没了 | 同上（A12M OutlinedText） | 同上 | reg_01_game.png：新手/xiaoye 出字 |
| 4 | 金币不显示 | 同上（mesolabel A11M） | 同上 | reg_04_inventory.png：9,260,000 |
| 5 | 装备排版/背景色 | 字体缺失连带 tooltip 文字空；版式本体此前 R27 已配准 | 字体修复后回归正常；对比图已存 evidence | 待用户细验（悬浮对比属主观项） |
| 6 | 双击不可穿戴无提示 | EQUIP 双击被 095 魔方劫持 | 恢复原生双击=穿戴（can_wear_equip 自带 UIOk）；魔方收回到 CASH 243* 双击→SoftKey 槽位（cube.prompt_slot i18n 已补） | 代码级；真机双击手测待用户 |
| 7 | 快捷键提示部分没 | 字体缺失（键帽绑定名走 FreeType） | 同上 | zoom_hotkeys.png：Shift/Ins/Hm/Pup + 全绑定文字 |
| 8 | NPC 名称不显示 | 同上（namelabel A13B） | 同上 | r5b 帧：娜娜/罗杰名字出字 |
| 9 | 光圈自动传送 | find_warp_at 把 REGULAR 放自动触发，与原生相反 | PortalScope::TOUCH_ONLY（仅 TOUCH/SCRIPTED_TOUCH 碰触即发）+ 按上键 ALL | 代码级；真机走门手测待用户 |
| 10 | 自由市场 MapleTV 无画面 | **两层**：a) 服务端 NPCTalkHandler 按英文名 endsWith("Maple TV") 路由，zh-CN 名不匹配 ⇒ 9250025.js 缺失 ⇒ "is not coded"；b) 客户端 Npc 此前不读 MapleTV.img（并行会话已补立绘双层），SendTV 文字未灌屏 | a) 补 scripts-zh-CN/npc/9250025.js + scripts/npc 同步（cm.dispose，与原生 TV 纯显示语义一致；脚本按文件懒加载无需重启）；b) Npc::set_tv_message + MapNpcs::broadcast_tv_message + SendTV/RemoveTV 接线（TVchat 底板盖台标 + 5 行文字） | r7_tv_room.png/r7_tv_room_b.png：上屏台标+下屏广告渲染、两帧广告轮播变化 |
| 11 | NPC/玩家点不了 | 光标 CANCLICK 正常（手型），TalkToNPC 链路通；此前「点不了」感知的另一半是 #10 is not coded 无反馈 + 字体缺失无名字 | #10 已修；点击链真机已证（手型光标命中） | 玩家名点击开 CharInfo 属 UICharInfo 窗，待用户手测 |
| 12 | 技能窗只剩图标；组合栏一行；关了还留着 | 图标/等级=字体；一行=rows=total>0?total:1 空角色只画一行；关不掉=K 走 pre_add→toggle_active 绕过 close() | 字体修复；rows 恒 MACRO_COUNT=3 行可见；UISkillBook::toggle_active 关闭分支收 UISkillMacroEx + 复位 dimension | r6_01（三行空槽版式与原生同构）/r6_02（关窗后组合栏消失） |
| 13 | 小地图无地区信息、M 不循环、透明窗 | 地区名=字体；M 循环分支被删 | 字体；UIMiniMap::send_key 补 MIN→NORMAL→MAX→关→MIN（含持久化），UIStateGame 兜底重建 | r2_02_m2.png：「彩虹岛：番茄田」；r2_01/03/04 循环态 |
| 14 | 聊天类型只有背景色；偶发白底 | 类型文字=FreeType；白底=quads 空帧清屏色（47fd9045 已改黑底） | 字体修复 | r2_06_chat_focus.png：「对所有人」出字 |

## 混尺寸 ABI 插曲（一并修掉）

IO/UIElement.h 12:21 加 SPECIALUI ⇒ NUM_TYPES 变 ⇒ UIState*::elements 实例尺寸变；
23:20 的 exe 是 146/286 旧 obj 混链（头文件注释本就要求整工程 Rebuild）。
已 `/t:Rebuild` 全量重编并部署。此后所有验证帧均出自新 exe。

## 顺带修复（并行会话遗留编译损坏）

IO/UITypes/UINpcTalk.h 两处 `\1` 损坏行 + BTN_DY 常量被连带删除：
按 HEAD 原值补回 TEXT_SLACK=18 / rowmax / BTN_DY=49（保留并行会话对
BTN_DY_NEXT/TEXT_BOTTOM_PAD 的重校改动）。

## 部署与还原

- 主部署 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` = 13:27 最新构建（TV 文字层 + 全部修复）。
- `MapleStoryTest.exe` 同构建，留作自动化测试通道（与用户实例文件锁互不干扰）。
- 两个部署目录 fonts/ 均已含 SourceHanSerifSC。
- xy-config.yaml 已还原（admin/auto:false）；备份 .bak.pass5b/6/7 留存。
- ms-alt-b53 试验副本已清理还原。

## 聊天窗（用户 turn1 原始问题）终验

`r8_01_messenger.png`（C 键 = 代码默认 MAPLECHAT）：标题条「聊天窗」、页签
「聊天窗」、四枚名牌（灰名牌出 xiaoye 名 + 本机立绘）、双白框、双输入槽、
ENTER 橙徽标、右上 X 全部正常 —— 与 `native-refs-r14/b81b3b97…png` 逐带同构。
最初「聊天框 UI 不匹配」的观感 = 字体缺失把「聊天窗」标题/名牌名/页签文字
抹掉只剩底板；字体修复后消失。

## 待用户手测清单（自动化不可达）

1. #6 双击不可穿戴装备 → UIOk 提示文案观感；243* 魔方道具重洗链路。
2. #9 走门不自动传、按↑传；弹跳门（SPRING）仍碰触即发。
3. #11 点玩家开 CharInfo、点 NPC 出对话。
4. #5 装备悬浮对比 tooltip 细节观感（背景色/排版）。
5. #10 发一条真喇叭（5070005 道具）→ TV 上屏 5 行文字（本轮只验了台标+广告层，文字层需 SEND_TV 在场）。
