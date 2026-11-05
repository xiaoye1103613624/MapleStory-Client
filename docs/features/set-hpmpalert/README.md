# SET_HPMPALERT（HP/MP 报警阈值存档）

> 日期：2026-11-05 ｜ 状态：代码已闭环（Wave 1-1.1）；本票仅销案归档

## 结论

parity-gap「不发 0x1000」条目**已过时**。上行 + 回灌均已接线：

| 方向 | opcode | 实现 |
| --- | --- | --- |
| c2s | `SET_HPMPALERT=4096` | `UIOptionMenu` 确定 → `SetHpMpAlertPacket(hp_step, mp_step)` |
| s2c | 扩展 `UPDATE_HPMPAALERT` | `UpdateHpMpAlertHandler` → `ui.sysopt.*_warn` |

量纲：本地 0..100% → step=`(pct+2)/5` 钳 0..19；s9 `HpMpAlertService` step/20=百分比。

## 服务端

`SetHpMpAlertHandler` 已注册；端到端回灌依赖 `use_server_auto_pot=true`（碰库，待用户授权）。

## 待测

见 [pending-live-tests §7](../pending-live-tests/README.md)。
