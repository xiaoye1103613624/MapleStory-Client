# CREATE_CHAR job 扩展（本端 + s9 成对）

| CREATE_CHAR `job` | 种族 | 出生 Job | Creator |
| --- | --- | --- | --- |
| 0 | 骑士团 | 1000 | `NoblesseCreator` |
| 1 | 冒险家 | 0 | `BeginnerCreator` |
| 2 | 战神 | 2000 | `LegendCreator` |
| **3** | **龙神** | **2001** | `EvanCreator` |
| **4** | **双刀** | **0（双刀教程图）** | `DualBladeCreator` |

- 客户端：`UIRaceSelect` classic 五选；`UICommonCreation` `evan→3` / `dualBlade→4`
- 服务端开关：`enable_evan` / `enable_dual_blade`（迁移 `V1.11.8`，默认 true）
- 说明：与官方纯 083 三职业创角协议不兼容，属本端扩展
