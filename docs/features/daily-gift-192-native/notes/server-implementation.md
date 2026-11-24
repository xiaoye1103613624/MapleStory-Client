# 192 原生 28 天每日礼物 — s9 服务端实现说明

> 实现时间：2026-11-23（Asia/Shanghai）。仓库：`E:/project/BeiDou-Server_s9`（仅改此仓库）。
> 协议依据：`docs/features/daily-gift-192-native/notes/protocol-evidence.md` §2/§3/§4。
> 本实现是**真实功能闭环**：服务端权威校验 + 发奖 + 独立表持久化 + 内存降级，非 stub。
> 严格复用本端每日签到范式（DailyCheckinStore/Rewards/Handler + PacketCreator + opcode 段），
> 但**未触碰任何 DailyCheckin\* 文件**（slot2 独立）。

---

## 1. 私有协议（严格按任务书，非照搬 192 的 710/1188）

| 方向 | opcode | 结构 |
| --- | --- | --- |
| C→S | `RecvOpcode.DAILY_GIFT_ACTION = 0x3721` | `byte action`(0=OPEN,1=CLAIM) + 若 CLAIM 则 `byte day`(1..28) |
| S→C | `SendOpcode.DAILY_GIFT_SNAPSHOT = 0x3722` | `byte respType`(0=配置快照,1=领奖结果) |

`respType=0` 字段序（逐字节，客户端只读展示）：

```
byte  currentDay      // 可领日(1..28)或已领最后日(24h 冷却中)
int   claimedMask     // 28 位已领掩码(bit0=第1天)
byte  levelMin        // =33（192 DAILYGIFTDATA.nLevelMin，协议证据 §4.1）
byte  questDone       // 杀怪300 quest 是否完成 0/1（本端 quest 4512）
int   killCount       // 真实 quest 进度 0..300（非假计数）
byte  premiumLeft     // 月内 premium 剩余次数，固定 1
int[28] rewardItemIds // 每天奖励 itemId（v83 真道具）
int[28] rewardCounts  // 每天数量
```

`respType=1` 字段序：`byte resultCode, byte day, int itemId, int count`。
resultCode：`0成功 / 1等级不足 / 2杀怪不足 / 3已领过 / 4非当前日 / 5月限已尽`。

发包位置：`PacketCreator.dailyGiftSnapshot(...)` / `dailyGiftResult(...)`。
**发奖成功后**：先发 `respType=0` 快照刷新（currentDay 回到最后已领日、mask 置位），
再单独发 `respType=1` 成功结果（resultCode=0）。

---

## 2. 服务端权威逻辑

### 2.1 落库（独立表，不碰 characters）

`gms-server/src/main/resources/db/extensions/daily_gift_state.sql`（CREATE TABLE IF NOT EXISTS，
运维手动执行，AI 未执行 DDL）：

| 列 | 类型 | 语义 |
| --- | --- | --- |
| characterid | INT PK | 角色 ID（仅关联，不改 characters 表） |
| giftDay | INT | 本周期最后领取日 0..28 |
| giftClaimed | INT | 28 位已领 mask |
| giftLastClaim | BIGINT | 上次领奖 epoch millis（24h 门） |
| giftMonthKey | INT | 上次领奖月份 YYYYMM（跨月重置月限） |
| premiumLeft | SMALLINT | 本月剩余 premium 次数（固定 1） |

### 2.2 DailyGiftStore（仿 DailyCheckinStore）

- per-character `State` + `ConcurrentHashMap` 常驻内存；`get/load/save` 首次 SQL 失败即
  `tableUsable=false` 全内存降级，缺表不阻塞角色加载。
- `claimableNow()`：24h 内返回 0（冷却）；距上次 >48h 或已领满 28 天则重置周期；
  跨月（monthKey 变化）重置 premiumLeft=1。
- `applyClaim(d)`：推进 day、置位 mask、刷新 lastClaimMs、premiumLeft-1。

### 2.3 DailyGiftRewards（28 天真表 + 门控常量）

- 28 天奖励参考 DailyCheckinRewards 的 v83 真道具（红/蓝/橙/万能/属性药水、白/黑/混沌/
  超级混沌卷、金/银喇叭、gachapon 券、潜力卷等），每天 `itemId + count + mesos`；
  `grantDay(Client,day)` 统一发 mesos + 道具（`InventoryManipulator.addById`）。
- `MIN_LEVEL = 33`（192 nLevelMin，协议证据 §4.1 题述固定 33）。
- **杀怪门用本端真实 quest 机制（未用假 killCount）**：
  - `KILL_QUEST_ID = 4512`，来自 `gms-server/wz/Quest.wz/Check.img.xml:50708-50727`：
    quest 4512 "The great secret reveals" 的 completeReq 为单条 `MobRequirement`
    （mob 9420512, count=300），无职业限制，lvmin=30、前置 4511 完成。
  - `questDone(player) = player.getQuestStatus(4512) == 2`（COMPLETED，协议证据 §3.1
    `GetQuestState(questId)==2` 语义）；进度由本端 `Character.raiseQuestMobCount` 在杀怪时累计。
  - `killCount` 从 `QuestStatus.getProgress(9420512)` 读取真实进度并封顶 300；未开始时 0。
  - 若后续换用自建 300 杀怪 quest，只改 `KILL_QUEST_ID/KILL_MOB_ID` 常量并同步本说明即可。

### 2.4 DailyGiftHandler（权威发奖闭环）

1. OPEN → 直接发配置快照（respType=0）。
2. CLAIM → 按序校验：`level >= 33` → `questDone(4512)` → `premiumLeft > 0` →
   `claimable >= 1` → `day ∈ [1,28]` → `day == claimable` → `mask 未置位`。
   任一失败：发 respType=1 对应 resultCode + 快照，**不发奖**。
3. 全过 → `grantDay` 发奖 → `applyClaim` → `save` → 快照刷新 + resultCode=0 结果包。
4. 入口/拒绝/成功/异常均有 INFO/WARN 节点日志（`[DailyGift]` 前缀），无 DBG 残留。

### 2.5 GM 命令

`@dailygift`（`DailyGiftCommand`，仿 CheckinCommand）打开窗口，下发与 OPEN 同构快照；
等级不足提示，冷却中提示剩余时分。注册于 `CommandsExecutor.registerLv0Commands()`。

---

## 3. 改动文件清单（相对 E:/project/BeiDou-Server_s9）

### 新增

| 文件 | 行数 | 关键行 |
| --- | --- | --- |
| `gms-server/src/main/resources/db/extensions/daily_gift_state.sql` | 15 | 6-15 建表（全列中文 COMMENT） |
| `gms-server/src/main/java/org/gms/server/DailyGiftStore.java` | 155 | 29-75 State/claimableNow/applyClaim；95-155 load/save/降级 |
| `gms-server/src/main/java/org/gms/server/DailyGiftRewards.java` | 122 | 18-27 常量(33/4512/9420512)；30-60 奖励表；64-115 查询/questDone/killCount/grantDay |
| `gms-server/src/main/java/org/gms/net/server/handlers/DailyGiftHandler.java` | 109 | 32-106 handlePacket/validateClaim/sendSnapshot |
| `gms-server/src/main/java/org/gms/client/command/commands/gm0/DailyGiftCommand.java` | 41 | 全文（@dailygift） |

### 修改

| 文件 | 关键行 | 改动 |
| --- | --- | --- |
| `gms-server/src/main/java/org/gms/net/opcodes/RecvOpcode.java` | 215 | `DAILY_GIFT_ACTION(0x3721)` |
| `gms-server/src/main/java/org/gms/net/opcodes/SendOpcode.java` | 380 | `DAILY_GIFT_SNAPSHOT(0x3722)` |
| `gms-server/src/main/java/org/gms/util/PacketCreator.java` | 358-391 | `dailyGiftSnapshot` / `dailyGiftResult` 两方法 |
| `gms-server/src/main/java/org/gms/net/PacketProcessor.java` | 144 | `registerHandler(DAILY_GIFT_ACTION, DailyGiftHandler)` |
| `gms-server/src/main/java/org/gms/client/command/CommandsExecutor.java` | 202 | `addCommand("dailygift", DailyGiftCommand.class)` |

**未改**：`DailyCheckinStore.java` / `DailyCheckinRewards.java` / `DailyCheckinHandler.java` /
`CheckinCommand.java` / `0x3717`/`0x3718` 注册与发包 / `Character.java` / `characters` 表。

---

## 4. 验证

- 基线：`mvn -o -pl gms-server compile` 改动前 EXIT=0（BUILD SUCCESS，无基线错误）。
- 改动后：`mvn -o -pl gms-server compile` → **BUILD SUCCESS / EXIT=0 / error=0**。
- DBG 残留检索：新增 5 个文件无 `System.out` / `[DBG-…]` / 调试级日志；仅上线级 INFO/WARN。
- 未做真机联调（客户端 UI 尚未接 0x3722 解析）；协议字段序按需求包逐字节实现，
  可与客户端 `DailyGiftHandler`/`UIDailyGift` 对接时逐字段核对。

## 5. 运维事项

1. 手动执行 `daily_gift_state.sql`（幂等 CREATE IF NOT EXISTS）；未执行时功能以纯内存运行，
   重启后状态丢失，建表后即持久化。
2. 杀怪门依赖 quest 4512 存在（本端 WZ `Quest.wz/Check.img.xml` 已确认）；若服务端 quest
   数据未加载或运维换 quest，改 `DailyGiftRewards.KILL_QUEST_ID/KILL_MOB_ID`。
3. 服务端重启加载新 opcode/handler；客户端需另接 0x3721/0x3722 收发才能展示真窗。
