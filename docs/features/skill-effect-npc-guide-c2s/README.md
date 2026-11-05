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
