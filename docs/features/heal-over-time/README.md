# HEAL_OVER_TIME（自然回血/回蓝上报）

> 日期：2026-11-05

## 取证

| 源 | 结论 |
| --- | --- |
| s9 `HealOvertimeHandler` | `skip(8); short healHP; short healMP`；HP 上限 `77*map.recovery*1.5`；MP 要求 `0<mp<1000` |
| port-095 | v95 `CWvsContext::TryRecovery` → opcode 映射 HOT |
| BeiDou IDA | 道具节点 `HPrecovery`/`MPrecovery`/`recoveryInterval`(默认 10000) |

## 实现

- `MapInfo` 解析 `info/recovery`（缺省 1.0）
- `Stage::try_heal_over_time`：每 10s 按 `max*1%*recovery` 计算，钳 abHeal；发 `HealOvertimePacket`
- 乐观本地加减暂不改（等 STAT_CHANGED 回灌），避免与椅子服务端回复双加

## 待测

见 [pending-live-tests §9](../pending-live-tests/README.md)。
