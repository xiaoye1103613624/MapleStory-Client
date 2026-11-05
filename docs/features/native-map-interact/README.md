# W4 — 地图特殊交互

## c2s

| 包 | opcode | 接线 |
| --- | --- | --- |
| CHANGE_MAP_SPECIAL | 100 | 脚本门 `warpinfo.scripted` |
| USE_INNER_PORTAL | 101 | 同图内门 |
| TOUCHING_REACTOR | 206 | `Stage::check_touch_reactors` |
| USE_TREASUER_CHEST | 115 | ETC 4280000/1 双击 |
| USE_ITEM_REWARD | 112 | USE 202* SoftKey 确认 |

## s2c

| 包 | opcode | 接线 |
| --- | --- | --- |
| GUILD_BOSS_HEALER_MOVE | 287 | tip |
| GUILD_BOSS_PULLEY_STATE_CHANGE | 288 | tip |
