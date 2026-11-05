# 公会 Boss HUD + 地图物件态机（287 / 288）

## 状态（2026-11-05）

| 项 | 状态 |
| --- | --- |
| `UIGuildBoss` 顶栏 HUD | ✅ |
| HealerMove(287) / Pulley(288) 接线 | ✅ 不再写 QuestAlarm tip |
| 地图物件动画态机 `MapGuildBoss` | ✅ healer RelMove 插值 + pulley 状态门控字节 |
| 真机 | ⏳ pending §13（需 990000900 + 脚本发包） |

## 证据

| 源 | 结论 |
| --- | --- |
| s9 `GuildPackets.GuildBoss_*` | short y / byte state；opcode 287/288 |
| IDA `sub_558B0F` | 同号分发到 `sub_558C0C` / `sub_558C3B` |
| IDA `sub_5587B3` | 读 `healer`/`yMin`/`pulley`，建层 |
| NX `990000900.img` | 莎莲娜王室；Obj `guild.img/syarenian/boss/{0,1}` |

详见 [evidence/ida-healer-pulley.md](evidence/ida-healer-pulley.md)。

## 实现要点

- 进图若地图根有 `healer`/`pulley` 节点 → `MapGuildBoss` 加载动画；否则包仍更新 HUD，物件侧 debug 忽略。
- 287：地图 healer Y 按时长插值（参考 rise/fall）+ HUD。
- 288：写入 pulley state（原生仅门控；绘制仍播循环帧）+ HUD。
- 交互键发包（原生 `sub_558B45` / c2s）本轮未接——无服务端脚本消费证据，显式预留。

## 范围说明

本轮闭合**收包驱动的地图物件 + HUD**。Pulley 交互上行、healMin/Max 服务端治疗逻辑不在客户端范围。
