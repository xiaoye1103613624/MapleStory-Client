# 双刀原生转职链（430→434）

## 创角

- RaceSelect `DUALBLADE` → CREATE_CHAR job=4 → `DualBladeCreator` → Job.BEGINNER(0) @ `MapId.DUALBLADE_TUTORIAL_START` (103050000)

## NPC / 等级

| 阶段 | Job | 等级 | NPC | 脚本 |
| --- | --- | --- | --- | --- |
| 首转 | 0→430 | ≥10 | 1056000 Lady Syl | `scripts-zh-CN/npc/1056000.js` |
| 二转 | 430→431 | ≥20 | 1056000 | 同上 |
| 2.5 | 431→432 | ≥30 | 1056000 | 同上 |
| 三转 | 432→433 | ≥70 | 2020011 | `2020011.js` 双刀旁路 |
| 四转 | 433→434 | ≥120 | 2081400 | `2081400.js` 双刀旁路 |

## 传送门

自 BeiDou-Server 移植：`portal/dual_*.js`、`Dual_moveGate.js`

## Quest WZ

自 192 合入 s9 `Quest.wz` Check/Act/Say/QuestInfo：236x / 262x–264x / 1447 / 1457 等（见 staging `quest_merge`）
