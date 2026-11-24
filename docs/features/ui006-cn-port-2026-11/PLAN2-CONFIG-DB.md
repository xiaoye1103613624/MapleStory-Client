# 魔女森林 / 突破石 配置 DB 化 + Web 管理 + reload 热更 + 时间轮换 — 一次性完整 Plan（2026-11-21）

> 状态：**待用户确认计划**。确认后按本计划一次成型实现，不分多轮。
> 前置：功能本身已落地（见 `docs/features/ui006-cn-port-2026-11/README.md`），本计划把它从 Java 硬编码升级为可运维配置。

## 0. 已确认决策（用户 2026-11-21 拍板）

- **D1 配置形态**：DB 表 + Web 管理 + reload 热更（真正的可维护）。
- **D2 表结构**：三张表 —— 奖项表 + 档位表 + 全局配置单行。
- **D3 时间轮换**：奖项按**生效时段**切换（每条奖项带生效时间窗，服务端按当前时间选生效条目抽奖）。
- **D4 Web 需求**：管理页含 reload 按钮 + 时间轮换配置。
- **D5 表空语义**：**严格以 DB 为准，表空就没奖励/不可用**（不回退代码默认）——用户明确选择，放弃项目惯例的代码降级。
- **D6 定时**：按用户 web 自定义要求，加定时 reload（让时段切换自动生效，无需手动点 reload）。

## 1. 现成模板链路（照抄，证据见 recon 报告）

migration 建表/种子 → DO(@Table/@Id/@Data@Builder) → Mapper(空接口 extends BaseMapper) →
Manager(volatile 不可变快照 + synchronized reload + 广播) → Service(@AllArgsConstructor + @Transactional + reload) →
Controller(@RestController + SubmitBody + ResultBody + **POST /v1/reload**) →
gms-ui(api/*.ts + views/game/*/index.vue 含 reload 按钮) →
启动接线 ServerManager.run() → 定时 TimerManager.register()。

## 2. 三张表设计（migration V1.11.18 起）

### 2.1 `xy_witch_reward`（奖项表）
| 列 | 类型 | 说明 |
|---|---|---|
| id | BIGINT AUTO_INCREMENT PK | |
| pool_type | INT | 0=普通50池, 1=高级100池 |
| item_id | INT | 奖励道具 id |
| qty | INT | 数量 |
| weight | INT | 权重 |
| grade | INT | 评级 0..4（对应 WZ Grade） |
| effective_from | TIME NULL | 生效起始（每日 HH:mm），NULL=全天 |
| effective_to | TIME NULL | 生效结束（跨午夜支持），NULL=全天 |
| enabled | INT DEFAULT 1 | |
| sort_order / remark / create_time / update_time | | 惯例字段 |

### 2.2 `xy_breakthrough_tier`（档位表）
| 列 | 类型 | 说明 |
|---|---|---|
| id | BIGINT AUTO_INCREMENT PK | |
| item_id | INT UNIQUE | 消耗道具 id |
| success_rate | INT | 0..100 |
| delta | BIGINT | 成功增量 |
| effective_from / effective_to / enabled / sort_order / remark / time | | 同上 |

### 2.3 `xy_witch_activity_config`（全局配置单行，抄 weather_timing_config）
| 列 | 类型 | 说明 |
|---|---|---|
| id | BIGINT AUTO_INCREMENT PK | |
| cost_50 / cost_100 | INT | 两档单次点券消耗 |
| ten_draw_count | INT | 十连次数（默认10） |
| miracle_from / miracle_to | TIME | 奇迹时间窗口 |
| cycle_minutes | INT | 时段轮换 reload 周期（分钟，定时用） |
| enabled / remark / create_time / update_time | | |

种子：`FROM DUAL WHERE NOT EXISTS`，写入当前硬编码值（50/100/10、miracle 50:00-59:59、cycle 5）。

## 3. 分层实现清单（行号级照抄点见 recon）

- **migration**：`V1.11.18__xy_witch_breakthrough_config.sql`（3 表 + 种子，抄 `V1.9.10` 单行配置 + `V1.11.16` 三表）。
- **DO**：`WitchRewardDO`、`BreakthroughTierDO`、`WitchActivityConfigDO`（抄 `SetItemDO.java:15-36`、`WeatherTimingConfigDO.java:16-38`）。
- **Mapper**：3 个空接口 extends BaseMapper（抄 `SetItemMapper.java:1-7`，靠 `@MapperScan` 注册）。
- **Manager**：`WitchConfigManager`（抄 `SetItemManager.java:32-106`）：
  - 3 个 `volatile` 不可变快照（reward/tier/config）；
  - `synchronized reload()` 全量读 3 表 → 按 effective_from/to 过滤出"当前生效"子集 → unmodifiableMap 原子替换；
  - **D5 严格语义**：DB 空 → 快照为空 map，不降级；`draw()` 返回 null、`find()` 返回 null → handler 回失败/无奖励包，不扣券；
  - getter 供游戏线程无锁读。
- **Service**：`WitchConfigService`（抄 `SetItemService.java:22-72`）：3 类 CRUD（@Transactional upsert/delete）+ `reload()`。无运行时状态字段，可用 @AllArgsConstructor。
- **Controller**：`WitchConfigController`（抄 `SetItemController.java:19-61`）：`/witchConfig/v1/{reward|tier|config}/{list|save|delete}` + `POST /witchConfig/v1/reload`。
- **改造现有读取点**（handler 仅 4 处）：
  - `WitchForestHandler.java:44` `cost()` → 读 config 快照；`:59` `draw()` → 读 reward 快照（传当前生效子集）；
  - `BreakthroughStoneHandler.java:36` `find()`、`:65` `TIERS` → 读 tier 快照；
  - `WitchForestRewards.java` / `BreakthroughStoneConfig.java` 保留为"数据表结构定义（record Entry/Tier）+ 被 Manager 调用的过滤/抽取算法"，删掉硬编码数组。
- **启动接线**：`ServerManager.java:45-52` 加 `WitchConfigManager.reload()`（放 Server.init() 之后）。
- **定时轮换**：`WitchConfigManager` 提供 `reload()`；在 `World.java` 或 ServerManager 用 `TimerManager.register(()->WitchConfigManager.reload(), interval, interval)` 按 `cycle_minutes` 定期重读 → 时段切换自动生效；抄 `World.java:271-280` + `WeatherTask.java`。注意：DB 里改 cycle 后需 reload 才会重建定时器（web reload 按钮触发），文档说明。

## 4. Web 管理页（gms-ui）

- `src/api/witchConfig.ts`（抄 `weather.ts:98-175`）：各 list/save/delete + `reloadWitchConfig()` = POST `/witchConfig/v1/reload`。
- `src/views/game/witchConfig/index.vue`（抄 `weather/index.vue:29-31,104-113`）：顶部 **reload 按钮**（status=warning + loading）+ 状态刷新；3 个 Tab：
  - 奖项 tab（抄 `regionTab.vue:157-256` CRUD）：pool_type/item_id/qty/weight/grade/生效时段；
  - 档位 tab：item_id/success_rate/delta/生效时段；
  - 全局配置 tab（单行）：cost_50/cost_100/ten_draw_count/miracle 窗口/cycle_minutes。
- 路由 + 菜单：按 gms-ui 现有路由/菜单注册方式加入（照 weather 的路由项）；i18n 加 zh-CN/en-US 文案。
- 前端构建：确认 dist 部署形态（问用户或沿用现有，本计划不改部署方式）。

## 5. 验证门禁

- 服务端 `mvn -q -DskipTests package` 通过；新 DO/Mapper/Manager/Service/Controller 全部编入 jar。
- migration flyway 启动不报错（3 表建成、种子 1 行 config）。
- DB 有数据时：web 改 cost/权重 → 保存 → reload 生效（日志验证）；改生效时段 → 到点自动切换（定时 reload 日志）。
- **D5 边界验证**：清空奖项表 → 抽奖返回"暂无奖励"且不扣券（不崩、不降级）；清空档位表 → 突破石返回不可用。
- opcode 不变，`gen_opcode_matrix.py --check` GATE OK。
- 部署：服务端 jar 落 live（按 RuntimePaths/jar 位置），按部署铁律实时核对 SHA。
- 前端 dist：构建后说明部署位置；不改客户端 exe（本功能纯服务端配置，客户端协议未变）。

## 6. 回滚

- migration 用 `DROP TABLE IF EXISTS` 反向脚本（保留为 `V1.11.18__down.sql` 说明，不自动执行）。
- 代码：单功能原子 commit（用户授权时）。
- 严格 DB 语义下，回滚配置 = 恢复种子数据；极端情况恢复硬编码临时分支。

## 7. 工作量与风险

- 约 3 后端新类族 + 1 Controller + 1 migration + 1 前端页/api/路由/i18n + 2 处接线改造。
- 风险：D5 严格语义下若运营忘记建种子，抽奖会"无奖励"——由 migration 种子 + web reload 状态提示缓解；此时段轮换的时区按服务端本地时区（Asia/Shanghai）解释，文档写明。
- 真机验收：仍需用户进游戏点测（本代理无法结束高权限客户端进程），如实声明。
