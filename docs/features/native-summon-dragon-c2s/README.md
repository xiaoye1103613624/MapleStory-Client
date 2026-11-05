# W2 — 召唤兽 / 战神龙上行

## 对账

| 包 | opcode | Handler | 消费点 |
| --- | --- | --- | --- |
| MOVE_SUMMON | 175 | MoveSummonHandler | `MapFieldMarkers::report_owned_c2s` 跟随位移 |
| SUMMON_ATTACK | 176 | SummonDamageHandler | 近怪 1s CD 上报 |
| DAMAGE_SUMMON | 177 | DamageSummonHandler | UIAdmin SoftKey `dmgsummon oid dmg mid` |
| BEHOLDER | 178 | BeholderHandler | UIAdmin SoftKey `beholder oid skillId` |
| MOVE_DRAGON | 181 | MoveDragonHandler | 本角色龙跟随上报 |

## 文件
- `Net/Packets/SummonDragonPackets.h`
- `Gameplay/MapleMap/MapFieldMarkers.*`
- `Gameplay/Stage.cpp`
- `IO/UITypes/UIAdmin.cpp`
