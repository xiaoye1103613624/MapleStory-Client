# CANCEL_DEBUFF（减益到期通知）

> 日期：2026-11-05

## 取证

| 源 | 结论 |
| --- | --- |
| port-095 | `CWvsContext::CheckTemporaryStatDuration` → `CANCEL_DEBUFF` |
| s9 `CancelDebuffHandler` | **整段注释**（类注释 `TIP: BAD STUFF LOL!`）——启用会无条件清全疾病，属危险逻辑 |
| 真解除通道 | `USE_ITEM` 药剂 dispel（AllCure 等），本端发 72 已通 |

## 实现（协议对齐，非玩法解除）

- `CancelDebuffPacket` 空包 opcode 99
- `Player::update`：STUN/POISON/SEAL/DARKNESS/WEAKEN 本地 duration 扣减到期 → 清本地 + 发空包
- **不改服务端**（保持空 handler）

## 待测

见 [pending-live-tests §11](../pending-live-tests/README.md)。
