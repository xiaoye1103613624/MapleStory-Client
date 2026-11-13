# SET_FIELD skillbook Stack underflow 修复（2026-11-13）

## 根因

1. **masterlevel 帧宽**：`parse_skillbook` 旧判定 `((skill_id % 100000) / 10000 == 2)` 与 Angel `is_skill_need_master_level@0x4e8f04` / s9 `Skill.isFourthJob`（`job%10==2` + Evan 特例）不一致 → 多读或少读 4B → 后续段错位/下溢。
2. **前导 `writeByte(0)`**：s9 `addSkillInfo` 在 size 前写 0；CASH 无终结符。若库存段多吞 1B 吃掉该标记，硬读前导字节会把 size 低位当标记（历史 `remain=354` 失败 / `355` 偶发成功）。

对照：`BeiDou-Server_s9/.../PacketCreator.java` `addInventoryInfo` / `addSkillInfo`；Angel IDA `0x4e8f04`。

## 改动

| 文件 | 内容 |
|---|---|
| `Net/InPacket.h/.cpp` | 新增 `position()` / `seek()` 供回滚试帧 |
| `Net/Handlers/Helpers/CharacterParser.cpp` | masterlevel 对齐 Angel/s9；先试「消费标记」再试「裸 size」，size 可行性门禁；成功写 `skillbook-framed-*` |

## 验证

- 构建 staging → live：`5164032` / `19:14:49`（`COPY_OK`）
- `--tiers 0,1,5 --seconds 55` → **3/3 pass**；`skillbook-framed-marker remain=104` + `fade done enter_game=1` ×3
- 日志：`evidence/tier-matrix-after-skillbook.log`、`setfield_trace.log`
