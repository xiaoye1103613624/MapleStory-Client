# 战斗统计（Battle Analysis）192 移植 — 服务端侧交付记录（B2）

日期：2026-11-25　作用域：`E:\project\BeiDou-Server_s9`（gms-server）　用途：单机版/技术研究，严禁商业用途。

## 1. 结论摘要

- 完成协议、Handler、Character 运行时状态、经验/金币/击杀归因、组队聚合下发、生命周期清理。
- **956 无冲突**：`Opcode.getValue()` 返回 `int`（`net/opcodes/Opcode.java:4`），全库检索 `956|0x3BC` 只命中 `ExpTable.java:25` 的经验表数值，无 opcode 占用；客户端 `Net/PacketSwitch.cpp`/`Net/OutPacket.h` 由主代理确认也无冲突。
- **956 下行严格 1B 许可**（对齐 IDA `OnServerOnCalcRequestResult@0x628900` 原生语义），组队数据走新增独立下行 `0x3BD`，不污染 `DPT_TRACKER(0x3714)` / `CUSTOM_PACKET(0x3713)`，现有伤害榜链路零删改。
- 构建门禁：`mvn -q -DskipTests package` 通过（exit 0，两次：初次实现 + 广播修正后）。
- **未做真机验证**：未连客户端实机发包、未验证面板渲染与数值口径，按用户要求如实声明。

## 2. 改动文件清单（绝对路径 + 行号）

| 文件 | 行号 | 改动 |
|---|---|---|
| `E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\net\opcodes\RecvOpcode.java` | 26–28 | 新增 `BATTLE_STATS_REQUEST(0x3BC)` 上行（含取证注释） |
| `E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\net\opcodes\SendOpcode.java` | 359–362 | 新增 `BATTLE_STATS_RESULT(0x3BC)` 下行 + `BATTLE_STATS_PARTY(0x3BD)` 下行 |
| `E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\net\PacketProcessor.java` | 141–142 | `registerCommonHandlers()` 注册 `RecvOpcode.BATTLE_STATS_REQUEST → BattleStatsHandler` |
| `E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\net\server\handlers\BattleStatsHandler.java` | 1–72（新文件） | 上行解析 3×byte + 条件 2B 时长、三态处理、1B 许可回包、异常包 drain |
| `E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\util\PacketCreator.java` | 480–519 | `battleStatsResult` / `battleStatsPlayerUpdate` / `battleStatsReset` 三个发包方法 |
| `E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\client\Character.java` | 565–575 | 新增 battleStats* 运行时状态字段（与 dpt* 完全隔离） |
| 同上 | 1842–1844 | `changeMapInternal()` 插入切图清理 `battleStatsOnLeave("change-map")` |
| 同上 | 2160–2167 / 2208–2215 | 掉落金币拾取（含组队平分）两处插入 `battleStatsRecordMeso` |
| 同上 | 5626–5782 | 新增 battleStats 开关/清理/归因/时长/组队广播方法族 |
| 同上 | 5852–5856 | `dptOnDamage` 内附加 `battleStatsDamage += dmg` + 广播（dpt 原逻辑与发包不变） |
| 同上 | 10187–10189 | `logOff()` 插入下线清理 `battleStatsOnLeave("logout")` |
| `E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\server\life\Monster.java` | 778–782 | `giveExpToCharacter` 在 `gainExp` 之后记录**实得经验**（personal+party）并广播 |
| 同上 | 812–820 | `killBy` 在死亡结算处给击杀者（伤害最高者回退 killer）记击杀数 |
| `E:\project\BeiDou-Server_s9\gms-server\src\main\resources\i18n\log_zh_CN.properties` | 301–311 | BattleStats 日志键（10 条） |
| `E:\project\BeiDou-Server_s9\gms-server\src\main\resources\i18n\log_en_US.properties` | 290–300 | 对应英文日志键（10 条） |

## 3. 协议字段表

### 3.1 上行 C→S `0x3BC`（RecvOpcode.BATTLE_STATS_REQUEST）

取证：192 IDA `CBattleRecordMan::RequestOnCalc@0x62ee00`（PLAN.md §0 / 00-analysis.md §3）。

| 顺序 | 类型 | 字段 | 语义 |
|---|---|---|---|
| 1 | byte | `bOn` | 1=开始/进行中，0=结束 |
| 2 | byte | `bIsNew` | 1=重新起一段统计（exp/meso/kill/damage 归零） |
| 3 | byte | `bClear` | 1=仅清运行时，回许可 0 |
| 4（仅 bOn=1） | uint16 | `durationSec` | 客户端本地计时秒数，服务端只记录不采信为数值口径 |

三态处理（Handler:39–65）：
- `bClear=1` → `battleStatsClear("packet-clear")`，回 0。
- `bOn=1` → 若 `bIsNew=1` 或尚未开始则归零并用当前时间作 startMs；仅 `isLoggedInWorld() && map!=null` 回 1，否则回 0；回 1 时立即下发一次全量队员快照（0 基线）。
- `bOn=0` → 记 endMs/endField，下发最终快照，回 0。
- 包体 <3B 记 WARN 并丢弃；多余字节 WARN + drain，避免包错位（GACHAEXP 4B 教训）。

### 3.2 下行 S→C `0x3BC`（SendOpcode.BATTLE_STATS_RESULT）

**恒 1B**：`m_bServerOnCalc`（1=许可，0=拒绝/关闭）。取证：192 IDA `OnServerOnCalcRequestResult@0x628900`。禁止附加 subtype 或额外字段。

### 3.3 下行 S→C `0x3BD`（SendOpcode.BATTLE_STATS_PARTY，B2 新增）

不占用 `DPT_TRACKER(0x3714)`，不改现有伤害榜子类型。

| 顺序 | 类型 | 字段 | 说明 |
|---|---|---|---|
| 1 | byte | `subtype` | 0=重置通知（无后续字段）；1=队员快照 |
| 2 | int32 | `charId` | 角色 ID |
| 3 | int32 | `jobId` | 职业 ID |
| 4 | string | `name` | `PacketCreator.writeString`（客户端 read_string 对应） |
| 5 | int32 | `partyId` | 队伍 ID，0=无队伍 |
| 6 | int64 | `exp` | 实得战斗经验累计 |
| 7 | int64 | `meso` | 拾取金币累计 |
| 8 | int64 | `kill` | 击杀数累计 |
| 9 | int64 | `damage` | 累计伤害（复用 dptOnDamage 事件，但走本包） |
| 10 | uint16 | `durationSec` | 接收者自己的统计时长（秒） |

广播语义（Character.java:5738–5782）：每次事件对每个 viewer 下发**全量活跃队员行**（含自己；同图 + 同队且 battleStatsActive；无队时仅自己）。客户端策略：subtype=1 按 charId upsert（可把一批内首行当 clear 信号更严格）；subtype=0 清空该接收者表。触发时点：开启 / 结束 / 每次伤害累计 / 每次怪物经验分配。注意：subtype=0 目前只发给触发清理的角色本人，未广播给全队（队员退出时其他人需靠下一次全量快照自然淘汰，或后续按需补发）。

## 4. 每个字段的数据来源与口径

| 字段 | 数据来源（行号） | 口径说明 |
|---|---|---|
| `battleStatsStartMs/EndMs` | `System.currentTimeMillis()`，Handler.bOn/bOn=0 时写入（Character.java:5650/5677） | 服务端时间；客户端本地计时为兜底 |
| `battleStatsStartField/EndField` | `Character.getMapId()`（:5657/:5678） | 原生 `m_nStartFieldID/m_nEndFieldID` |
| `battleStatsExp` | `Monster.giveExpToCharacter` 在 `attacker.gainExp(...)` 之后调用 `battleStatsRecordExp(_personalExp, _partyExp)`（Monster.java:778–781） | **实得经验**：`_personalExp/_partyExp` 已含状态倍率、等级差、MVP、家族加成、EXP buff（`Monster.java:743–776`）。排除任务/道具/扭蛋经验（走 `Character.gainGachaExp:3041–3059` 等其它路径，不调用本记录点） |
| `battleStatsMeso` | `Character.pickupItem` 中 `mapitem.getMeso()>0` 的两处 gainMeso 之后（Character.java:2160–2167 / 2208–2215） | 仅怪物掉落金币拾取（含组队平分）。排除交易(`Trade.java`)、任务(`MesoAction`)、商店/签到/雇佣商店等其它 gainMeso 路径 |
| `battleStatsKill` | `Monster.killBy` 服务端死亡结算（`MapleMap.killMonster:1491` 调用），取 `getHighestDamagerId()` 对应角色，回退 killer（Monster.java:812–820） | 不依赖客户端包带 cid；与掉落归属 looter 选择一致 |
| `battleStatsDamage` | `dptOnDamage` 事件入口（5 处调用点：AbstractDealDamageHandler/SummonDamageHandler/MobDamageMobHandler/TakeDamageHandler/Monster 持续伤害），Character.java:5852–5856 | 复用事件但不改 dpt 字段/发包；无技能明细（技能明细由客户端 DamageRank 本地渲染） |
| `durationSec` | `battleStatsDurationSec()`（Character.java:5716–5725） | 进行中 now-start，结束态 end-start；uint16 秒封顶 |

并发：经验归因在 `gainExpInternal`（synchronized）**返回之后**调用，只读 `_personalExp/_partyExp` 局部值并累加到角色自己的 `battleStatsExp`，不持服务端锁、不反向进入 gainExp，无新增锁顺序，不引入死锁。状态按 Character 实例隔离；伤害/经验热点路径的广播发包为单包逐 viewer，不上线级逐 tick 刷屏（INFO 日志在开/关/清/归因关键节点，广播汇总一条）。

## 5. 生命周期

- 切图：`changeMapInternal`（Character.java:1844）→ `battleStatsOnLeave("change-map")` → 清运行时 + 给该角色发 subtype=0。
- 换频道/断线：`Client.disconnect` → `player.logOff()` → `logOff()`（Character.java:10189）→ `battleStatsOnLeave("logout")`。
- 上行 bClear：仅清本角色（Handler:52）。
- dpt 字段/发包/伤害榜主逻辑完全未改。

## 6. 验证门禁

- `cd E:/project/BeiDou-Server_s9 && mvn -q -DskipTests package`：**通过，exit 0**（两次）。无 Maven 路径问题。
- 未跑真机发包/面板联调（需客户端 build + 登录 + Start/打怪/End 全流程）；按用户要求声明为**未验证项**。
- 未写数据库：全程未执行任何 INSERT/UPDATE/DDL/DML。

## 7. 未验证项 / 风险

1. 真机未验：956 许可回包与面板开关联动、0x3BD 逐队员快照的客户端解析与翻页渲染均未实机验证。
2. 伤害技能明细：服务端只下发总伤，逐技能明细依赖客户端 DamageRank 单例（B2 本地路径），未做服务端技能维度下发（PLAN.md T2.7 的「按技能」不在本端实现）。
3. subtype=0 未广播全队：队员退出/切图时其它队员表中旧行不会立即删除，需客户端在下次 subtype=1 全量快照时按「消失即淘汰」处理；若要即时清，服务端可在 `battleStatsOnLeave` 给同队 viewer 补发全量快照（待主代理确认是否需要）。
4. 时长口径：服务端 durationSec 与客户端本地计时可能存在秒级差异；客户端本地计时作兜底，服务端值以 0x3BD 为准。
5. 上线级 INFO 日志在经验/金币/击杀每次事件都会打，刷怪密集时日志量较大；如需降噪可改为 DEBUG 并仅保留开关/广播汇总 INFO（当前按用户「关键节点 INFO」要求保留）。
