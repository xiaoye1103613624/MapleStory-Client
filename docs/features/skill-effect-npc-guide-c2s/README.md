# SKILL_EFFECT / NPC_ACTION / CLICK_GUIDE 上行接线

> 日期：2026-11-05 ｜ 状态：代码已闭环；真机 → pending-live-tests

## 对账（s9）

| 包 | opcode | Handler | body |
| --- | --- | --- | --- |
| SKILL_EFFECT | 93 / 0x5D | SkillEffectHandler | int skill + byte lv + byte flags + byte speed + byte direction；白名单广播 `PacketCreator.skillEffect` |
| NPC_ACTION | 197 / 0xC5 | NPCAnimationHandler | length==6 对话：`int oid + byte + byte` 原样回广播 |
| CLICK_GUIDE | 162 / 0xA2 | ClickGuideHandler | 空包 → 骑士团开 MIMO(1101008)，其它开 LILIN(1202000) |

## 接线

- `SkillEffectPacket::is_effect_skill` 与 handler switch 18 项白名单逐项对齐；`Combat::apply_move` 攻击/非攻击路径起手前各发一次（无 KeyDown 条时的最小闭环）。
- `TalkToNPCPacket` 构造内顺带 `NpcActionTalkPacket(oid)`（`-1,0`），全部 SoftKey/点击入口继承。
- `MapFieldMarkers::try_click_guide`：SPAWN_GUIDE 场标 oid=`0x70000001` 命中 → `ClickGuidePacket`。

## 显式预留

- 持续技 KeyDown 按住/松开（取消特效）需独立状态机；本批只发起手一包。
- NPC_ACTION 移动分支（length>6）未接。

## tip / 字体 / NPC 对话框（2026-11-05 约束）

本批**只补出站协议**，未改 tip、ChatBalloon、Tooltip、`UINpcTalk` 字号/字色/α/框体/按钮。

| 项 | 现状 | 下一项 |
| --- | --- | --- |
| tip / 字体 | 未触碰；客户端默认宋体/思源宋体路线保持 | 若改 tip 须 IDA/IMG/像素取证，禁臆造、禁回 SimHei |
| NPC 对话交互 | 点选 `#L` 发 selection 已通；`TalkToNPC`+`NPC_ACTION` 已接 | — |
| NPC 对话「下项」A11b | 列表可点；右下「下项」钮未激活（open-issues A11b） | IDA 定语义后再接，禁半成品钮 |
| NPC 对话框样式 | UtilDlgEx 既有；名牌 A11M 白 / 正文 A12M DARKGREY | 全量像素对齐原端（框体/α/按钮/立绘）另立切片 |
