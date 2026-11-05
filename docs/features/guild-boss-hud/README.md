# 公会 Boss HUD（287 / 288）

## 状态（2026-11-05）

| 项 | 状态 |
| --- | --- |
| `UIGuildBoss` 顶栏 HUD | ✅ |
| HealerMove(287) / Pulley(288) 接线 | ✅ 不再写 QuestAlarm tip |
| 地图物件动画态机 | ⏳ 预留（IDA `sub_5587B3` 读 `healer`/`pulley`/`yMin`） |
| 真机 | ⏳ pending §13（需公会 PQ 图脚本发包） |

## 证据

| 源 | 结论 |
| --- | --- |
| s9 `GuildPackets.GuildBoss_HealerMove/PulleyStateChange` | short y / byte state |
| IDA `sub_5587B3` | 读地图属性 `healer`+`yMin`、`pulley` 并调用 `sub_558455`/`sub_558612` |
| 本仓既有 tip handler | 仅 QuestAlarm 黄字，无 HUD |

## 范围说明

本轮交付**同步 HUD**（可见性 + 数值），不伪造物件位移动画。完整原生物件驱动需公会 PQ 图 + 物件层续作。
