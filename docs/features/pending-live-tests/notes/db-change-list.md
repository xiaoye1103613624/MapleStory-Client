# 待确认的库内写入清单（2026-11-05）

> **状态：全部「未执行」。** 本轮只做了只读查询；GM 命令注入受阻后未改用 DML，避免越过「写操作逐条确认」门禁。
> 通道：用户自备并已启动的 dbx MCP（`C:\Users\11036\.dbx\bin\dbx-mcp.exe`，连接 `75c9875d-d7ff-4bfa-955c-6e09077530c6` / `beidou`）。
> 本会话以 stdio 直连该 MCP 完成 `tools/list` 与 `dbx_execute_query` 只读查询；`.codebuddy/mcp.json` 的合并操作因审批超时未执行，也未重试或绕过。
> 回滚基线：`notes/character-snapshot-before.json`。

## 只读基线（已取得）

| 项 | 值 |
| --- | --- |
| `characters` Admin | id=1, accountid=1, world=0, level=1, job=0, ap=0, gm=6, guildid=0, hp=50/50, mp=5/5 |
| `inventoryitems` | 8 行，仅装备 + `2040002` + `4161001`；**无**回城卷/技能书/召唤袋 |
| `keymap` | 42 行（key 2..92），无 SCROLL_LOCK(89) 槽绑定 |
| `guilds` / `bbs_threads` | 均为 0 行 |
| `game_config` | `use_server_auto_pot=false`，`enable_pic=false`，`enable_pin=false`，`deterred_player_command=false` |
| `accounts` | admin / admn / xiaoye，`loggedin=0` |

## 待确认语句（逐条，均未执行）

| # | 目标 | 语句（草案，执行前需按实际列名/ID 复核） | 关联待测项 | 状态 |
| --- | --- | --- | --- | --- |
| 1 | 角色等级/职业/AP | `UPDATE characters SET level=120, job=412, ap=500 WHERE id=1;`（`sp` 为逗号分隔串，须照快照格式重写） | 一键 AP `0x58`、手雷 `0x6D`、技能书 `82` | ⏸ 未执行 |
| 2 | 补道具 | `INSERT INTO inventoryitems (characterid,itemid,quantity,inventorytype,position,...) VALUES (1, <回城卷/技能书/召唤袋 itemid>, 1, 2, ...);`（itemid 需从 WZ/Item 常量确认） | `85 / 82 / 75` | ⏸ 未执行 |
| 3 | 补技能 | `INSERT INTO skills (characterid,skillid,skilllevel,masterlevel) VALUES (1, <技能id>, <等级>, <上限>);` | 手雷、Buff、能量、疾病 | ⏸ 未执行 |
| 4 | 第二角色 | `INSERT INTO characters (...) VALUES (...);`（需与 `accounts` 关联、填满非空字段） | WORLD_MAP_PLAYERS 多人、`PICK_ALL_CHAR` 多角色 | ⏸ 未执行 |
| 5 | 公会 | `INSERT INTO guilds (...) VALUES (...);` + `UPDATE characters SET guildid=... WHERE id=1;` | GuildBBS 完整窗、Guild Boss HUD、公会摘要 | ⏸ 未执行 |
| 6 | 公会 BBS | `INSERT INTO bbs_threads (...) VALUES (...);` | `UIGuildBBS` 列表/读帖 | ⏸ 未执行 |
| 7 | 自动药水开关 | `UPDATE game_config SET config_value='true' WHERE config_code='use_server_auto_pot';` | `SET_HPMPALERT(4096)` 端到端回灌 | ⏸ 未执行 |

## 约束

- 上表每条执行前须由用户明确确认；执行后逐条记录到本文件（时间、影响行数）。
- #7 **优先走管理端** `ConfigController` 的 `/config/{latest}/updateConfig`（8686），而非裸 SQL；`GameConfig` 可能有缓存。
- 改 `game_config` 后若需重启 s9，由用户执行（本机 Java 21 环境不具备，AI 不自行启停服务端）。
- 任何语句都不得触碰并行会话正在使用的表/行；执行前重读 `character-snapshot-before.json` 对照。
- 回滚：按快照逐字段恢复，或重新导入快照中的行。
