# 魔女森林 / 突破石 配置 DB 化交付说明（2026-11-21）

> 把原本 Java 硬编码的抽奖内容/消耗/档位升级为「DB 表 + Web 管理 + reload 热更 + 时段轮换」。
> 完整计划见 [PLAN2-CONFIG-DB.md](PLAN2-CONFIG-DB.md)。

## 一、用户拍板决策

- 配置形态：DB 表 + Web 管理 + reload 热更。
- 表结构：三张表（奖项 + 档位 + 全局配置单行）。
- 时间轮换：奖项/档位按生效时段切换（每条带 effective_from/to）。
- Web：含 reload 按钮 + 时间轮换配置（cycle_minutes）。
- 表空语义：**严格 DB 为准**（表空=无奖励/不可用，不回退代码默认）。

## 二、已交付（行号级）

### DB 层
- migration `V1.11.18__xy_witch_breakthrough_config.sql`：3 表（xy_witch_reward / xy_breakthrough_tier / xy_witch_activity_config）+ WHERE NOT EXISTS 幂等种子。
- DO：`WitchRewardDO` / `BreakthroughTierDO` / `WitchActivityConfigDO`（@Table/@Id/@Data@Builder；config 的 cost_50/cost_100 用 @Column 显式映射）。
- Mapper：3 个空接口 extends BaseMapper（靠 @MapperScan 注册）。

### 缓存层
- `org.gms.server.witch.WitchConfigManager`：3 个 volatile 不可变快照；synchronized reload() 全量读 3 表 + LocalTime 时段过滤（含跨午夜）；严格 DB 空语义；`cost()/draw()/find()/tiers()/miracleNow()/secondsToNextCycle()/rotationIntervalMillis()` 供 handler 读。
- `WitchRotationTask`：TimerManager.register 按 cycle_minutes 定期 reload，时段切换自动生效。

### Service / Controller
- `WitchConfigService`：3 类 CRUD（@Transactional upsert/delete）+ reload。
- `WitchConfigController`：`/witchConfig/v1/{reward|tier|config}/{list|save|delete|get}` + `POST /reload`（反馈 activeRewards/activeTiers/hasConfig/cycleMinutes；reload 后重建定时器）。

### handler 改造（4 处读取点 → Manager）
- `WitchForestHandler:44` cost、`:59` draw、`:84-87` 倒计时/奇迹时间 → 全部读 Manager；无配置/无奖项不扣券回空结果。
- `BreakthroughStoneHandler:36` find、`:65` tiers → 读 Manager；无生效档位 USE 回失败不消耗道具。
- 已删除旧硬编码类 `WitchForestRewards.java` / `BreakthroughStoneConfig.java`。

### 启动 / 定时接线
- `ServerManager.run()`（Server.init() 之后）加 `WitchConfigManager.reload()` + `WitchRotationTask.register()`。

### Web（gms-ui，vite dev 8787 源码即 live）
- `src/api/witchConfig.ts`：各 list/save/delete + `reloadWitchConfig()`。
- `src/views/game/witchConfig/index.vue`：3 tab（奖项/档位/全局配置）+ 顶部 reload 按钮 + a-table CRUD + a-modal 编辑；生效时段用 a-time-picker。
- locale zh-CN/en-US + 路由 `game.ts` + 全局 locale 聚合。

## 三、验证（实测）

- 服务端 `mvn -q -DskipTests package`：EXIT 0。
- 前端 `npx vue-tsc --noEmit`：本功能 0 错误；`npx vite build`：EXIT 0，witchConfig 已打包。
  - 注意：`npm run build` 会因**既有** `src/views/game/file/index.vue` 的 3 个类型错误（javascriptDefaults/ScriptTarget，本功能未触碰）失败；本功能自身类型干净。
- 服务端重启（IDE target/classes 方式，PID 26712）+ Flyway V1.11.18 建表成功。
- 运行日志实证：`[WitchConfig] reloaded: rewards=8 tiers=3 config=present`、`rotation timer registered, interval=300000ms`。
- 两端监听：后端 8686（PID 26712）、前端 dev 8787（PID 49496）。
- 期间修复：`WitchActivityConfigDO.cost50/cost100` 加 `@Column("cost_50"/"cost_100")`（MyBatis-Flex 不会把 cost50 自动转 cost_50，导致首次启动 SQL 报 Unknown column）；修复后重启加载成功。

## 四、部署说明

- 服务端：当前 live 为 IDE `target/classes` 启动（非 jar），已重新编译并重启，Flyway 已执行 V1.11.18。
  - jar 产物 `gms-server/target/BeiDou.jar`（SHA `E99A02D2F147FDB04996A2046A36A64FE89FD302E411F754E3051117DA777E4F`）为可选择的生产部署形态，当前未用它启动。
- 前端：vite dev server（8787）读源码，新页面/路由/i18n 已自动生效，刷新即见；未改静态托管形态。

## 五、诚实边界

- **真机未验收**：抽奖/突破实际点击链路（客户端↔服务端 0x3719/0x371A、0x371B/0x371C）未做进游戏点测（客户端进程权限高于代理无法结束）；本结论来自源码行号 + 启动日志。
- API 需鉴权，未绕过鉴权直接调 `/witchConfig/v1/reload`；配置生效以服务端启动日志为准。
- 严格 DB 语义下，若运维清空配置表/删全局配置行，抽奖将无奖励、突破石不可用（用户明确选择此语义）。
- 时区：时段窗口按服务端本地时区（Asia/Shanghai）解释。
- Web 周期 `cycle_minutes` 改动后 reload 会重建定时器；时段切换本身由定时 reload 触发。
