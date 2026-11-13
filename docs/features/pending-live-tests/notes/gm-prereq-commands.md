# GM 前置补齐命令清单与注入限制（2026-11-05）

## 服务端事实

- 命令前缀：`!` = GM / `@` = 用户命令（`gms-server/src/main/java/org/gms/client/command/CommandsExecutor.java:53-54`）。
- 权限门：`client.getPlayer().isGM()` 且 `gmLevel >= command.getRank()`（同文件 `:65-69, :127`）。
- 本会话角色 `Admin`：`characters.gm = 6`，满足全部 rank ≤ 6 的命令。
- 命令名在 `CommandsExecutor` 的 `addCommand(...)` 中注册，例如 `addCommand("level", 2, LevelCommand.class)`（`:260`）。
- 参数按空格切分：`message.substring(1).split(" ", 2)`。

## 已确认的命令清单（供手工补验或后续授权执行）

| 命令 | rank | 参数 | 用途 / 对应待测项 |
| --- | --- | --- | --- |
| `!level` | 2 | 等级数字 | `LevelCommand` setLevel(n-1)；补齐等级 |
| `!ap` | 2 | AP 数字 | 补齐可分配属性点 → 一键 AP `0x58` |
| `!job` | 2 | jobid | 切换职业 → 手雷/毒弹技能前置 |
| `!item` | 2 | itemid [数量] | 补回城卷/技能书/召唤袋 → `85/82/75` |
| `!maxskill` | 2 | — | 全技能满 → 手雷 `0x6D`、Buff 源 |
| `!maxhpmp` | 3 | — | 提高 HP/MP 上限 |
| `!hpmp` | 3 | 数值 | 调当前 HP/MP → `HEAL_OVER_TIME(89)` 掉血前置 |
| `!maxenergy` | 3 | — | 能量条满 15000 → `TOUCH_MONSTER_ATTACK(47)` |
| `!buffme` | 1 | — | 给自己技能 buff → `CANCEL_BUFF(92)` |
| `!debuff` | 3 | 疾病名（大写） | 施加疾病 → `CANCEL_DEBUFF(99)` |
| `!gmshop` | — | — | GM 商店（取特定道具） |
| `!setgmlevel` | 6 | 等级 | 调整 GM 等级（无需，Admin 已 gm=6） |

## 自动注入失败记录（真实抓包证据）

聊天包 opcode = `0x31`（`RecvOpcode.GENERAL_CHAT`）。四种注入方式均**未能**把 `!level 30` 作为 GM 命令发出：

| 方式 | 抓包结果 | 结论 |
| --- | --- | --- |
| `SendKeys "{!}level 30{ENTER}"` | `31 00 07 00 6c 65 76 65 6c 33 30 01` = 普通聊天 `level30` | `{!}` 无效键名，空格丢失，服务端按普通聊天广播回 `a2 00 ...` |
| `SendKeys "!level 30{ENTER}"` / `"+1level 30{ENTER}"` | 同样 `level30` 或无聊天包 | `!` 与空格未进输入框 |
| 剪贴板 `Set-Clipboard` + `Ctrl+V` | 发出 `2c 00 ...`（动作包），无 `31 00` | 客户端聊天框不吃 Ctrl+V |
| Unicode `SendInput` 逐字符（含前置 Enter 开框） | 无 `31 00` | `SDL_EVENT_TEXT_INPUT` 路径未把该串送进聊天框 |

客户端侧原因线索：`IO/UI.cpp:264-269` 聊天框聚焦时走 `Keyboard::get_text_mapping(...)` + `text_input_active()`；`!`（Shift+1）与空格在该映射下未落到 `Textfield`，空格还触发了游戏动作包。这属于**自动化注入/键位文本映射限制**，不等于 GM 命令本身失效；用真键盘手输 `!level 30` 应可用。

## 补验路径（责任方）

1. **用户手验**：在游戏内聊天框手输上表命令补齐前置，再按 `docs/features/pending-live-tests/README.md` 各项步骤点验（最快）。
2. **受控 DML**：授权执行 `notes/db-change-list.md` 的逐条语句后，重启客户端重登加载数据，再自动驱动验收。
3. **代码修复**（未授权，不建议本轮做）：修 `Keyboard::get_text_mapping` 对 `!`/空格的文本映射，使 GM 命令可被自动注入。

## 快照

`notes/character-snapshot-before.json`：改动前的 `characters`(1) / `inventoryitems`(8) / `keymap`(42) / `skills`(1) / `queststatus`(4) / `game_config`(200) 只读导出，用于回滚对照。
