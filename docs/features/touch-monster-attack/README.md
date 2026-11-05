# TOUCH_MONSTER_ATTACK（触怪攻击）

> 日期：2026-11-05

## 取证

| 源 | 结论 |
| --- | --- |
| s9 `TouchMonsterDamageHandler` | 仅当 `energyBar==15000 \|\| BODY_PRESSURE` 时 `applyAttack(parseDamage(...))` |
| 与 TAKE_DAMAGE 区别 | TAKE_DAMAGE=怪碰人；TOUCH_MONSTER=人碰怪（身体压力/能量满） |
| body | 与 CLOSE_ATTACK 同形（`AbstractDealDamageHandler.parseDamage`） |

## 实现

- `OutPacket::TOUCH_MONSTER_ATTACK=47`
- `AttackPacket(result, Opcode::TOUCH_MONSTER_ATTACK)` 重载
- `Stage` 碰撞：条件满足时 `prepare_attack` + `send_attack` + 发 47；同 oid 500ms 节流

## 局限

无 BODY_PRESSURE / 能量满时服务端丢弃——属预期。本端无该技能数据时无法端到端验伤，步骤见待测清单。

## 待测

见 [pending-live-tests §10](../pending-live-tests/README.md)。
