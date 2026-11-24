# 7 项 UI 问题一次性修复 · 交付记录（2026-11-21）

## 用户诉求
7 张截图列 7 项 UI 问题（染色/伤害统计/签到/图4签到来源/任务列表/全局ESC拖拽/小地图），按点到面全链路取证后用户拍板 4 个方向：D1 完整切中文染色包、D2 用需求包示例奖励、D3 伤害统计自适应高度、D4 新做 Angel IDA 字段级还原。

## 逐项交付（行号级证据见 PLAN.md）
1. **染色（D1）**：从巴西服包（ColorPrism.img/0x372E-F）完整切到中文包（`UIWindow.img/ColoringPrism/` 300×398、0x11D/0x184）。服务端新增 COLORING_PRISM_ACTION(0x11D)/COLORING_PRISM(0x184)+Handler+`server/coloring/`4类+SQL脚本（dye-zh）；客户端 9 文件（OutPacket/PacketSwitch/ColorPrismPackets/WaveAShellHandlers/UIColorPrism/locale×2/UIElement注释/opcode矩阵，dye-client）：真按钮+拖拽+ESC+i18n+拖入校验，降级=复用 draw_preview+TintContext（不新做CAvatar/改色引擎）、无拖入动画。资源经 batch_convert_key 转 GMS + copy_paste 子树注入 live UIWindow.img（根 115→116，probe 通过）。SQL 只落脚本未执行 DDL。
2. **伤害统计（D3+图标+居中）**：skill_icon() 按 skill_id 取 SkillData 图标+缓存；10 处文字走 `Text::y_for_ink_top` 真源；自适应高度 current_height 按行数、无滚动条；+send_key（rank-fix）。
3. **签到（D2）**：服务端 DailyCheckinRewards 28 天真实表（药水/白卷/金喇叭/Gachapon/百万mesos 等）；客户端收包存 icon_ids/tooltips→每格画主 icon+hover tooltip；关闭钮(490,6)命中区从 dragarea 摘出+send_key+close()。
4. **图4 DAILY GIFT**：结论=UIDailyGift 灰框假数据（无协议/无 IDA），非高版本非完整移植；angel 校准明确"仅 UI 假态❌"——属告知项，不改。
5. **任务（D4）**：真 IDA 一手（QuestInfo 16字段/CUIQuestInfo 42字段/CT_INFO 17字段）+ orange-wz 128B key autoParse 校正 IMG origin；行文字/图标居中；去自造裸 id 改可读 NPC/任务名；i18n；r07 文档（quest-angel）。未闭环：CUIQuestInfoDetail 3208B 匿名结构未强解、勋章独立窗版式无真机截图。
6. **全局 ESC/拖拽**：UI.cpp:429-462 白名单补 DAILYCHECKIN/DAMAGERANK/COLORPRISM；三窗 send_key。
7. **小地图**：绿点/文字按 y_for_ink_top 居中；NPC 单击改 mouse-up 确认+命中区摘出；换图显式 refresh_npclist()+签名兜底；MapDrops::reanchor_spawns 对称 npc/mob（minimap-fix）。

顺带修：并行会话遗留 `UIWorldMap.h:212` SearchRow 声明前置于定义的编译错误（非本次功能）。

## 构建/部署（三证 + 铁律）
全量 Rebuild `BUILD_EXITCODE=0`、链接行、error=0、产物 5203968B。
按铁律部署 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，实时核对 live SHA = 产物 SHA = **`CD94BB01D0B6392C21C72D2682846900F904D539DC235B7559B8B0934F32993D`**（17:53:47）。回滚点 `MapleStory.exe.bak-before-7fix-20261121`。

## 诚实边界
1. **真机未验收**：构建过≠显示对/发包对，7 项需用户用当前 live 逐项复核，截图须本端。
2. **染色协议服务端需用户重启服务端 + 执行 SQL**（`resources/db/extensions/coloring-prism-standalone.sql`）——AI 未执行 DDL；客户端 opcode 与服务端已对齐但真机染色链路未验收。
3. 签到奖励是需求包示例值（非用户自定义真实表），可改 DailyCheckinRewards.java。
4. 染色降级：预览复用自身 draw_preview、他人仅 TintContext 染色（无独立头像动画）、无拖入动画、确定提示用 UIOk（非原生 Notice 弹窗）。
5. 任务窗字段级未闭环 2 项（见上）。

## 证据
- 研判/逐行号：`docs/features/ui-7issues-assessment-2026-11/PLAN.md`
- 任务 IDA 一手：`docs/features/quest-angel-calibration-2026-11/notes/r07-field-level-2026-11.md`
- 协议账目：`docs/features/s9-protocol-align/protocol/opcode-matrix.json`（GATE OK）
