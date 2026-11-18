# 天气系统原生客户端 + 服务端 web 管理移植 · 完整开发计划（2026-11-18）

> 状态：Plan 已成型，待用户确认后进入实现（skill 门禁阶段 2）。
> 决策依据：见 Mnemon 文档 `天气系统原生客户端 + 服务端 web 管理移植 · 决策拍板（2026-11-18）`（id e19cdd37）。
> 客户端源码：`E:\project\MapleStory-Client`（当前分支 `feature/dev_0.0.1`，含并行 WIP）；
> 服务端源码：`E:\project\BeiDou-Server_s9\gms-server\`（工作区外）。

## 0. 目标与决策摘要

**目标**：把 bundle 的"服务端权威状态 + 单 opcode 广播 + 客户端渲染"架构，移植到当前原生源码客户端（去掉 DLL 注入与地址 hook），并把内存态升级为 DB 持久化 + web 热更新。

**已锁决策**：
- D1 = 核心闭环：时钟 / 天空 / tint / 路灯 / 协议 / web 管理；**预留**：sway、地面堆积、音效、splash、夜战、debuff、夜怪（接口留桩，`CurrentSky()` 可返回 `SKY_CLEAR` 独立构建；PNG 的椅子 re-anchor 补丁暂不纳入）。
- D2 = server-wide 天空（N world 共享），per-world 改法见 `WeatherService.java:49-50`，本版不做。
- D3 = 裁剪版持久化：区域 / 季节权重 / override 入库；时钟保持 `System.currentTimeMillis()` 纯函数（`WeatherService.java:83-95`），不改 DB 计数器。
- D4 = 表现配置化：profiles / palettes 运行时加载 + 版本号下发；静态 `.inc` 表作为兜底。

**协议锁定**：`LP_WeatherSync 0x373D` 28 字节定长（`WeatherPackets.java:36-50,82-109`），`LP_LampPreview 0x373F` 16 字节定长（`LampPackets.java:35-48`）。双端实测当前无占用（客户端 `Net/PacketSwitch.cpp:415-419`、服务端 `SendOpcode.java:358-359,367-368` 均只到 `0x3730`）。**legacy skyElapsedSec 4 字节占位必须保留**，删掉后续字段全偏移（`WeatherPackets.java:370-378`）。

---

## 1. 任务分解（有序，含依赖）

### P0 — 接入前复核（阻塞后续，只读）
| # | 动作 | 验收 |
| --- | --- | --- |
| P0.1 | 双端 `grep -rn "0x373D\|0x373F\|WEATHER_SYNC\|LAMP_PREVIEW"` 复查 opcode（bundle patch 的"0x37xx 全空"前提已过时） | 0 匹配才继续；有占用则换号并同步双端 |
| P0.2 | 确认 `BeiDou-Server_s9` 目标分支（套装改动所在分支），`git status` 记录并行 WIP | 分支确认，避免覆盖套装 hunk |
| P0.3 | 确认 orange-wz 端点 `127.0.0.1:10012/mcp` 可用；不可用则跑 `E:\project\orange-wz\start-mcp.bat` | MCP health 1/1 |
| P0.4 | 落 `notes/client-path.md`（用户客户端路径记忆） | 文件存在 |

### P1 — 服务端数据层 + 缓存（依赖 P0，DB 骨架）
照抄套装系统范式（`SetItemDO/Mapper/Service/Manager/ConfigController` + `GameConfig.java:21-29`）。
| # | 动作 | 文件（服务端） | 验收 |
| --- | --- | --- | --- |
| P1.1 | Flyway 建表：`weather_region`、`weather_season_weight`、`weather_map_region`、`weather_override` | `src/main/resources/db/migration/V1.9.9__create_weather_tables.sql` | SQL 含表注释、幂等 |
| P1.2 | Entity + Mapper（4 组） | `org.gms.dao.entity.Weather*DO` / `org.gms.dao.mapper.Weather*Mapper` | 照 SetItemDO 的 `@Table`/BaseMapper |
| P1.3 | `WeatherConfigManager`：volatile 快照 + `synchronized reload()` 全量读表 → 不可变 map 原子替换 + `reconfigure(snapshot)` 灌入 WeatherService + 广播 | `org.gms.server.weather.WeatherConfigManager` | 照 `SetItemManager.java:32-106` |
| P1.4 | `WeatherConfigService`：CRUD + reload（`@Transactional` 写后调 reload） | `org.gms.service.WeatherConfigService` | 照 `SetItemService.java:22-71` |
| P1.5 | 改造 `WeatherService`：`skyByRegion` / `SEASON_WEIGHTS` 由读常量改为读快照；**保留全部 static 访问签名**（`minuteOfDay/skyForMap/tintForMap/paletteForMap` 被发包大量调用）；`minuteOfDay()` 纯函数体不动 | `server/weather/WeatherService.java` | 签名零变；wall-clock 逻辑零改 |
| P1.6 | 改造 `WeatherRegion.forMap()` / `WeatherMapVisibility` 的映射读快照 | `WeatherRegion.java:387-430` / `WeatherMapVisibility.java:29-36` | 配置可热更 |
| P1.7 | 启动加载：`ServerManager.run()` 挂 `WeatherConfigManager.reload()` | `org.gms.manager.ServerManager.java` | 启动日志打印 loaded regions/weights |
| P1.0 | **前置（P0 复核新增）**：s9 目标分支 `feature/integration_client` 无 `org/gms/server/weather/`、无 `SendOpcode` 天气常量——bundle 服务端类从未落地本树。先按 bundle `server/` 原样落地核心类（WeatherService/Packets/Task/Profile/Region/Palette/MapVisibility + lamp + 3 命令 + SendOpcode 2 常量 + 4 required patch），再在其上叠 P1.1–P1.7 的 DB/web 改造 | 服务端 `org/gms/server/weather/`、`lamp/`、`net/server/task/`、`client/command/...` + 4 patch | bundle 服务端类编译通过，opcode 已加；补丁锚点按 s9 实际代码校正（bundle patch 基于 stock Cosmic，s9 有差异） |

### P2 — 服务端 web 控制面（依赖 P1）
| # | 动作 | 文件 | 验收 |
| --- | --- | --- | --- |
| P2.1 | `WeatherController`：`/weather/{v}/region/list\|save`、`/seasonWeight/...`、`/override/set\|clear`、`/reload`、`/state`（state 暴露 GM override 剩余时长——`OVERRIDE_HOLD_MS=1h`） | `org.gms.controller.WeatherController` | 照 `ConfigController.java:22-64` 信封/Swagger |
| P2.2 | GM 命令改为调 Manager API（保留 gm2/gm3 作 web 不可用救场） | `WeatherCommand:60-146` / `NightCommand:38-43` / `LampCommand:60-97` | 命令与 web 共用同一入口 |
| P2.3 | 写库/override 后 `WeatherPackets.broadcast(world,false)` 在线生效 | 复用 `WeatherPackets:146-162` 的 region 缓存 | 不重启即可见 |

### P3 — 客户端网络层（依赖 P0；可与 P1/P2 并行）
| # | 动作 | 文件（客户端） | 验收 |
| --- | --- | --- | --- |
| P3.1 | 加 enum `WEATHER_SYNC=0x373D` / `LAMP_PREVIEW=0x373F` + `emplace<...>()`（走 `extended_handlers` 稀疏表 `PacketSwitch.h:58-60,87-99`） | `Net/PacketSwitch.cpp:406-419` | 编译过，无重注册告警 |
| P3.2 | `WeatherSyncHandler` / `LampPreviewHandler`，照 `CommonHandlers.cpp:68-166` 的 `SetItemFinalDamageHandler` 范式 | `Net/Handlers/WeatherHandlers.h/.cpp` | 逐字段解析对齐 wire |
| P3.3 | 状态单例 `WeatherState`（照 `CombatPanelStats`）；落地 0x373D 全字段；**保留宽容解码**（`CanRead` 语义 → `recv.available()`，legacy `_Sec` 4 字节占位） | `Weather/WeatherState.h/.cpp` | 19B 老包与 28B 新包都能解 |
| P3.4 | opcode 名字表补 `case 0x373D/0x373F` | `Net/PacketSwitch.cpp:1000,1170` 区 | 日志可读 |

### P4 — 客户端表现层配置化（D4，依赖 P3）
| # | 动作 | 文件 | 验收 |
| --- | --- | --- | --- |
| P4.1 | `WeatherTypes.h`：Sky/Palette/Flags 枚举（来自 `weather.h:38-83`） | `Weather/WeatherTypes.h` | id 与 wire 一致 |
| P4.2 | profiles/palettes 运行时加载器 + 静态表兜底；保留 `static_assert` 行序契约（`weather.cpp:68-72,85-86`） | `Weather/WeatherProfiles.*` / `WeatherPalettes.*` | 配置缺失回退静态表 |
| P4.3 | `weather_config.json`（无 UTF-8 BOM）+ 版本号下发；服务端只发 id，客户端查表 | `Weather/weather_config.json` | web 改表现无需重编 |
| P4.4 | skytable/nosky 建议服务端与客户端共用一份生成源（web 统一管理，客户端启动拉取） | `Weather/WeatherSkyTable.*` | 两端判定一致 |

### P5 — 客户端渲染层核心闭环（依赖 P4）
| # | 动作 | 文件 | 验收 |
| --- | --- | --- | --- |
| P5.1 | 天空注入 + night tint：用 `GraphicsGL::drawscreenfill`（`GraphicsGL.cpp:1924`，对应 DLL 夜色 multiply）+ `MapBackgrounds::drawbackgrounds/foregrounds`（`MapBackgrounds.cpp:198,209`） | `Weather/WeatherFx.*` + `Gameplay/MapleMap/MapBackgrounds.cpp` | 夜色/天空可见 |
| P5.2 | 户外判定：`Background`（`MapBackgrounds.h:36-78`）新增 `bS/no` 读取，或离线生成 fieldId→outdoor 表 | `Gameplay/MapleMap/MapBackgrounds.h/.cpp` | 与服务端 `WeatherMapVisibility` 判定一致 |
| P5.3 | 路灯：`Map/Obj` lamp img + `weather_lamptable*.inc` 坐标转 C++ 数组，接入 `Stage::load` | `Weather/WeatherLamps.*` | 天黑灯亮 |
| P5.4 | 接入 `Stage::load/update/draw`（`Stage.cpp:62,182,213`）、主循环 `MapleStory::update`（`MapleStory.cpp:207-225`）；源码直调，**无 Detours/地址** | 上述 4 文件 | 直接函数调用，零 hook |
| P5.5 | 线程模型：DLL 的 atomics/dirty 三件套（`weather.cpp:108-146`）删掉，单线程直接读写 | `Weather/WeatherState.*` | 无接收线程竞态 |
| P5.6 | 预留桩：sway/地面/音效/splash 接口留空实现（`CurrentSky()` 返回 `SKY_CLEAR` 可独立构建） | `Weather/WeatherReserved.h` | 编译过，行为关闭 |

### P6 — WZ 资源入库（依赖 P0.3；与 P5 并行准备，渲染前就位）
**强制 orange-wz MCP，禁裸拷**（AGENTS.md §1/§3.1）。
| # | 动作 | 验收 |
| --- | --- | --- |
| P6.1 | 核心闭环所需 img 清单：`WeatherParticles`、`MapleMoonSky`、`WeatherFog`、`WeatherSnow`、`WeatherRainbow`、`MapleMoonLamp`、`MapleMoonLamp2`（音效/splash/积水本版不入库） | 清单落 `resources/` |
| P6.2 | 流水线：`load_files(exclusive:false)` → 校验节点 → 两个 IMPORTED 包（`WeatherSnow`/`MapleMoonLamp2`）`batch_convert_key(目标key,verify:true)` → `save_as` 到异路径 → `unload_all` → 原子替换进 live | verify:true 且本端 probe 通过 |
| P6.3 | `0512.img` 追加 5 个 art pointer 节点 `05120995`–`05120999`（orange-wz 节点合并，非整包覆盖）；`String/Cash.img` 若需 debuff 文案则加 `5120994`（本版 debuff 预留，可不加） | 节点查询可见、类型正确 |
| P6.4 | 短启验收：`--resource-probe` 目标 img 出现在 UI 根且进程无 `0xc0000374` | 无堆损坏 |

### P7 — 验证门禁（所有实现完成后）
| # | 命令/动作 | 通过标准 |
| --- | --- | --- |
| P7.1 | 服务端 `mvn -pl gms-server compile` + `package` | exit 0，jar 生成 |
| P7.2 | 客户端 MSBuild Rebuild（非增量） | `error C=0` / `error LNK=0`；obj mtime > cpp mtime（防增量假通过） |
| P7.3 | 双端 wire 蓝军核对：逐字段对照 `WeatherPackets.java:82-109` ↔ `WeatherHandlers` 解析 | 字段序/长度一一对应，legacy 4B 占位在 |
| P7.4 | 资源 probe（P6.4） | 通过 |
| P7.5 | 真机：切 exe → 重启服务端（Flyway 跑 V1.9.9）→ 进户外图看昼夜/雨雪/tint → web 改 override 不重启即时生效 → GM 命令救场 → 回选角无图层泄漏 | **本端截图由用户提供（外部依赖）；agent 无 GUI 权，如实声明未验收** |

### P8 — 产物 + 文档 + 提交（依赖 P7）
| # | 动作 | 验收 |
| --- | --- | --- |
| P8.1 | `docs/features/weather-system-2026-11/README.md` 最终实现文档（按 feature-doc-template） | 逻辑/配置/协议/资源/验证齐全 |
| P8.2 | 归档 `notes/`（client-path、协议表、户外判定规则）、`evidence/`（opcode grep、wire 核对、MCP 节点摘要）、`patches/`（双端 diff）、`builds/`（jar/exe SHA）、`resources/`（img 清单） | 可复核、脱敏（无 key/账号明文） |
| P8.3 | 原子提交：服务端 / 客户端分开；共享文件（`PacketSwitch.cpp`、`Stage.cpp`、`ServerManager.java`）混并行 WIP → hunk 级 `git apply --cached` 只提本功能（照套装手法） | 并行 WIP 完好留工作区 |
| P8.4 | 清理临时调试日志，保留上线级节点日志 + 完整注释 | 无 `[DBG-*]` 残留 |
| P8.5 | 部署具名副本（如 `MapleStory_weather.exe`），**不覆盖主 exe**；实时核对 SHA | 产物/副本 SHA 一致 |

---

## 2. 涉及文件/模块/资源预估

- **服务端（工作区外）**：新增 `WeatherConfigManager/Service/Controller` + 4 组 Entity/Mapper + Flyway；改 `WeatherService/Region/MapVisibility`、`ServerManager`、3 个 GM 命令类、`SendOpcode.java`（加 2 常量）。
- **客户端（工作区内）**：新增 `Weather/` 目录（Types/State/Fx/Lamps/Reserved/配置）；改 `Net/PacketSwitch.cpp`、`Net/Handlers/`、`Gameplay/Stage.cpp`、`Gameplay/MapleMap/MapBackgrounds.*`、`Graphics/GraphicsGL.cpp`（drawscreenfill 复用，多半无需改）。
- **资源（orange-wz）**：7 个核心 img + `0512.img`/`String` 节点合并；2 个 IMPORTED 包强制转 key。

## 3. MCP / 同步步骤
- WZ/IMG/XML：全程 orange-wz MCP（`load_files → 校验 → batch_convert_key(verify) → save_as 异路径 → unload → probe → 原子替换`），禁 `Copy-Item`/裸拷。
- REA/IDA：本版**不需要**——移植目标是原生源码，逻辑来自 bundle 源码 + 行号证据，不动二进制、不改私有偏移。
- 插件同步：本版不产 DLL，无 ijl15 同步步骤。

## 4. 验证门禁汇总
服务端编译/打包 + 客户端 Rebuild（防增量）+ wire 逐字段蓝军 + 资源 probe + 真机（用户截图，外部依赖）。

## 5. 风险与回滚
- **协议行序**：web 校验 id 只 append 不重排（`WeatherProfile:36-38`、`LampCatalog:23-28`）；回滚 = 撤销 web 配置行，代码不动。
- **户外判定双份一致**：客户端 `bS/no` 读取与服务端 `WeatherMapVisibility` 必须一致；回滚 = 静态 fieldId 表兜底。
- **资源跨端**：两个 IMPORTED 包转 key 失败即停；回滚 = 保留 live 备份 `.bak`，probe 失败原子还原。
- **并行 WIP**：客户端 `feature/dev_0.0.1` 有套装等并行改动，提交前 hunk 级划界；回滚 = 按原子 commit 粒度 revert。
- **DB DDL**：Flyway migration 由服务端启动执行（非 agent 直接操作库）；验证阶段若需查库内数据，先按门禁 17 选 dbx / 自备 MCP / 暂不碰库。

## 6. 预留项（D1=A 明确不做）
sway / 地面堆积 / 音效 / splash / 夜战 / WeatherDebuff / 夜怪 / hair-sway 椅子 re-anchor 补丁 —— 仅留接口桩，后续按扩展点逐个加，不塞进本版。
