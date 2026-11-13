# W08 map — 执行记录

## 对照

- 已有：[map-angel-calibration-2026-11](../../map-angel-calibration-2026-11/README.md)
- s9：`RecvOpcode.FIELD_DAMAGE_MOB(0xBF)` / `MOB_DAMAGE_MOB_FRIENDLY(0xC0)` / `MONSTER_BOMB(0xC1)` / `MOB_DAMAGE_MOB(0xC2)` / `LEFT_KNOCKBACK(0xD4)` 均有 enum + `PacketProcessor` 注册

## 本波

- Mob 死亡兜底（工作区续作）已入账；本波无新增 map UI 大改

## 为何不能做（场地伤害 c2s 族）— 确认

| opcode | s9 handler | 不能做原因 |
|---|---|---|
| FIELD_DAMAGE_MOB | `FieldDamageMobHandler`（真实现：oid+dmg，要求 `map.getEnvironment()` 非空） | 客户端障碍只渲染、无 environment 玩法面；补枚举无触发点 = 死包（wave4-battle-packets 已判「依赖未建」） |
| MOB_DAMAGE_MOB / FRIENDLY | 真实现（怪物互伤） | 触发在怪 AI / 受控怪出招；联机事件图未建 |
| MONSTER_BOMB | 真实现（仅 HIGH/LOW_DARKSTAR） | 需道场内容面 |
| LEFT_KNOCKBACK | 真实现（回 leftKnockBack+enableActions） | Angel 滚球 `COutPacket(212)` 与命名冲突；无脚本触发面，wave4 明确不做 |

**结论**：不能仅靠客户端加 OutPacket 销案；破 s9 语义或造假触发会污染联机。维持阻塞。

## 验收

- 无本轮代码；联机未跑
