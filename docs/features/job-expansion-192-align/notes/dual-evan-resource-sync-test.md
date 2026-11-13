# 双刀 / 龙神资源同步与创角测试（2026-11-12）

## 已同步

### 客户端 `BeiDou-Client-xy`
- Map：`103050*`、`1000301*`、`900010*`、`910350*`（orange-wz GMS 128B：`get_node_tree_json` → `create_img` → `copy_paste` → `save_as`，禁止裸拷）
- Npc：`1056000`、`1013000`、`1013100–1013104`、`1057001`、`1057003`
- Weapon：katara `01342*`、短刀 `01332*`、法杖/短杖 `01372*`/`01382*`（含创角常用）
- Dragon：S9 目录现有龙形态 img 已入库；`Character.nx` 仍保留 Wave2 的 0005×12

### 服务端 `BeiDou-Server_s9`
- Map/Npc XML（同上 ID）
- `String.wz/Eqp.img.xml`：1342/1332/1372/1382 名条
- 教程 NPC 脚本 `scripts-zh-CN/npc/1013100–1013104`（缺源则 stub）

## 怎么选（创角）

1. 用**本仓库新编译**客户端（含 RaceSelect 改动）。
2. 进入创角职业选择后应走 **classic 五卡**：`骑士团 | 冒险家 | 战神 | 龙神 | 双刀`（卡面下方有中文标签）。
3. 点 **龙神** → `UIEvanCreation`（CREATE_CHAR job=3）；点 **双刀** → `UIDualBladeCreation`（job=4）。
4. 若仍看到旧「书本/RaceSelect_new」且没有龙神/双刀：确认跑的是新 exe；`Login.img/RaceSelect` 存在时会强制 classic。现代 UI 兜底已解开 EVAN/DUALBLADE 禁用。

## 怎么测

| 步骤 | 期望 |
| --- | --- |
| 创角选龙神，进新手村 | 地图 `1000301xx` / `900010xxx` 可进；NPC `1013000`/`101310x` 可对话 |
| 创角选双刀，进教程图 | 地图 `103050xxx` / `910350xxx`；NPC `1056000` 可对话 |
| 装备外观 | 双刀短刀/katara、龙神杖在背包/穿戴不丢图；Eqp 名非空白 |
| 回归 | 骑士团/冒险家/战神创角与进图无回归 |

日志：`_create_char.log` 应出现 `[RaceSelect] mode=classic` 与 `enter creation class=`（龙神/双刀对应枚举值）。
