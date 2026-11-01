# r808 · J4：三家族服务端开关 —— 已生效，无需重启，无需再写库

对应任务 #71。日期：2026-11-01。

## 1. 结论

**骑士团 / 冒险家 / 战神三个家族在服务端侧全部已放开，且当前运行中的进程已经加载了放开后的值。**
⇒ 不需要重启世界服，也不需要再执行任何 DML。

## 2. 判据链（全部可复核）

### 2.1 开关在哪

`gms-server/.../net/server/handlers/login/CreateCharHandler.java:100-116`：
创建角色时按**家族序号**（不是职业 id）分三支，各查一个布尔开关，关掉就回 `-3`：

| `job`（家族序号） | 家族 | 开关 key | 创建器 |
| --- | --- | --- | --- |
| 0 | 骑士团 Knights of Cygnus | `enable_knights_of_cygnus` | `NoblesseCreator` |
| 1 | 冒险家 Adventurer | `enable_adventurers` | `BeginnerCreator` |
| 2 | 战神 Aran | `enable_the_lord_of_war` | `LegendCreator` |

`status == -3` 时先发一条 `serverNotice(1, ...disableJob)` 再 `getLoginFailed(1)` 主动断开，
所以踩到闸门的观感是「弹一句禁用职业提示然后被踢回登录」。

### 2.2 开关存在库里，不在配置文件里

全仓 grep 这三个 key，只有两处命中：`CreateCharHandler.java` 与
`gms-server/src/main/resources/db/migration/V1.7.0__create_game_config.sql:202-204`。
⇒ 值在 **`game_config` 表**（Flyway 建表 + 种子），`application.yaml` 里没有。

种子值（迁移文件写的）：

```sql
('server','Game Mechanics','java.lang.Boolean','enable_adventurers','true', ...),
('server','Game Mechanics','java.lang.Boolean','enable_knights_of_cygnus','false', ...),
('server','Game Mechanics','java.lang.Boolean','enable_the_lord_of_war','false', ...);
```

### 2.3 线上库实测（只读 SELECT）

本机会话里有两套 schema：`beidou` 与 `beidou_s9`。先确认跑的是哪一套 ——
`beidou.characters` 里 `id=1 / name=Admin / job=0 / level=1 / map=10000`，
与真机抓帧的底栏身份块 `LV.1 新手 Admin` 逐字对上（`beidou_s9` 是 `Admin job=212 lv=200`，不是它）。
⇒ **运行中的服务端用 `beidou`**。

```
SELECT config_code, config_value, update_time FROM game_config
 WHERE config_code IN ('enable_adventurers','enable_knights_of_cygnus','enable_the_lord_of_war');
→ enable_adventurers      true   NULL
  enable_knights_of_cygnus true   NULL
  enable_the_lord_of_war   true   NULL
```

三行都是 `true`，但 `update_time` 全 `NULL` ⇒ **不是走管理端改的**：
`ConfigService.updateConfig()`（:127-133）会 `updateTime(new Date())` 并顺手
`GameConfig.update(...)` 热更新内存。`NULL` 说明这次改动是**直接 SQL**，绕过了热更新通道。

### 2.4 绕过热更新为什么仍然生效：比时间戳

`GameConfig.java:27` 在 bean 构造期 `configService.loadGameConfigs()` 一次性把全表读进
`config.properties`；全仓没有 `@Scheduled` / reload 钩子（`GameConfig.add/update/remove`
的调用方只有 `ConfigService` 与三条 GM 命令）。⇒ **内存值 = 进程启动那一刻的库值**。

| 事件 | 时刻 | 取证 |
| --- | --- | --- |
| `beidou.game_config` 最后一次数据变更 | **2026-11-01 09:33:16** | `information_schema.tables.update_time` |
| 服务端进程启动完成 | **2026-11-01 09:36:47** | `logs/out.log:69` `Started ServerApplication in 4.88 seconds` |

改库早于启动 3 分 31 秒 ⇒ 启动时读进去的就是 `true` ⇒ **当前进程内存里三家族全开**。

## 3. 因此本轮不做什么

1. **不再 UPDATE**：值已经是 `true`，重复写只会白改 `update_time`，对运行中的进程毫无影响。
2. **不重启世界服**：没有需要重启才能生效的待生效改动。重启会顺带踢掉真机客户端会话，纯负收益。
3. 若将来又用**裸 SQL** 改这类开关，必须记得 `GameConfig` 没有 reload 钩子 ——
   要么走管理端（热更新 + 落库），要么改完重启；否则会出现「库里 true、游戏里仍说禁用职业」的假象。

## 4. 留给真机的一步（J3 客户端侧）

服务端闸门既然开着，「能不能创建骑士团/战神」就纯粹是客户端捏人流程的事了：
选角页 → 创建角色 → RaceSelect 左/中/右（骑士团/冒险家/战神，坐标见
`docs/features/create-char-classic-dual-path/README.md §8`，BtSelect (530,422)）→ 捏脸/捏发 → 起名 → 确认。
三族各建一个，能进图即 J3 闭环。已并入 CHECKLIST §K.2 K2-6。
