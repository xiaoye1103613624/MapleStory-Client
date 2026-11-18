# 天气系统实现状态（2026-11-18 开工轮）

> 状态：**双端核心闭环代码已落地、编译级验证通过、已原子提交；WZ 资源入库与真机未闭环（外部阻塞）**。
> 本文件是"候选完成 + 证据链 + 剩余风险"，不是最终验收。

## 提交记录（2026-11-18）
- 服务端 `BeiDou-Server_s9` @ `feature/integration_client`：`:sparkles: feat(weather)` — 30 文件 / 3169 行（含 V1.9.9 migration、GM 写库、数据卫生修复）。提交哈希以 `git log --grep=feat(weather)` 实时查询为准（本文不硬编码，避免 amend 后失配）。
- 客户端 `MapleStory-Client` @ `feature/dev_0.0.1`：`:sparkles: feat(weather)` — 16 文件 / 976 行。哈希同上，实时查询。
- 两提交均 hunk 级隔离并行 WIP：服务端 Character.java(inventory_slot WIP)、客户端 HUD 侧边栏/UIRaceSelect/Locale 等均未带入。提交后工作区残留仅为上述并行 WIP，天气代码零残留。
- 真机验收需以这两个 feat(weather) 提交对应的代码为准。

## 已完成（带证据）

### 服务端（BeiDou-Server_s9，分支 feature/integration_client）
- bundle 核心类落地（改包名 org.gms）：WeatherService/Packets/Profile/Palette/Region/MapVisibility、LampCatalog/Packets、WeatherTask、WeatherConfigManager。
- 可选层（WeatherCombat/WeatherDebuff/NocturnalMobService）未落地，grep 确认零编译期引用。
- DB：V1.9.9__create_weather_tables.sql（4 表）+ Entity/Mapper 4 组。
- web：WeatherController（`/weather/v1/*`，14 接口）+ WeatherConfigService + WeatherConfigManager（volatile 快照 + reload + 广播）。
- GM 命令：WeatherCommand(gm2)/LampCommand(gm3)/WeatherNightCommand(gm3, `!wnight`)。
- required patch：SendOpcode(0x373D/0x373F)、World、MapFactory、MapleMap、CommandsExecutor、ServerManager 启动 reload。
- **验证**：`mvn -pl gms-server compile` → BUILD SUCCESS / EXIT 0（1185 源文件，javac target 21）。target/classes 下 Weather*.class 齐全（实测）。
- D3 硬约束：minuteOfDay 纯函数体逐字未动，DB 只做配置覆盖层。

### 客户端（MapleStory-Client，分支 feature/dev_0.0.1）
- 新增 `Weather/` 目录：WeatherTypes.h（Sky/PaletteId/Flags，行序=wire）、WeatherConfig.h/.cpp（profiles/tint ramps 静态表逐字移植 bundle + 运行时覆盖接口 + static_assert 锁行序）、WeatherState.h/.cpp（单例状态 + 宽容解码落地 + 本地时钟）、WeatherFx.h/.cpp（昼夜曲线逐字移植 bundle weather.cpp:363-431 + 全屏夜色叠加）。
- Net：PacketSwitch 加 0x373D/0x373F enum + emplace（走 extended_handlers 稀疏表）+ 名字表；CommonHandlers 加 WeatherSyncHandler（26B 宽容解码，legacy 4B 占位保留）/ LampPreviewHandler（14B）。
- Stage：update 驱动 Fx::update（固定 timestep）、draw 末尾 Fx::draw（UI 之前，只压场景）、clear 重置天气。
- vcxproj/vcxproj.filters 加 4 编译单元（hunk 级，未动 HUD 并行 WIP 行）。
- **验证**：WeatherConfig/WeatherState 用 cl.exe /std:c++17 隔离编译 `CL_EXIT=0`，obj 产出（4858B/2503B）；WeatherFx.cpp 在 MSBuild ClCompile 进度行出现且错误列表零 WeatherFx → **天气 4 cpp 编译零错误**。
- 全工程未能链接：HUD 并行 WIP 的 `SideIconConfig::FILENAME` LNK2005 双重定义 + 一批文件需 C++17（HudLayout.h/SideIconConfig.h 等），与天气无关。

## 未闭环（外部阻塞，如实标注）

1. **WZ 资源入库（P6）未做**：orange-wz MCP 服务在 10012 监听（curl HTTP 400 = SSE 端点正常响应），但本会话未注册 `mcp__orange_wz__*` 工具前缀，`ops_discover_tools` 也无法激活。按 AGENTS.md §1/§3.1，WZ/IMG 一律走 orange-wz，禁止裸拷——故 7 个核心 img + 0512 节点合并**尚未执行**。需在工具可用的会话执行转 key 流水线（两个 IMPORTED 包 `WeatherSnow.img`/`MapleMoonLamp2.img` 必须 `batch_convert_key(verify:true)` + 短启 probe）。
2. **整工程链接未过**：HUD 并行 WIP 阻塞（`FILENAME` LNK2005 + C++17 配置），非天气代码。天气 4 cpp 编译级通过已单独取证。
3. **真机未验收**：无 GUI 操作权；昼夜/雨雪观感、web 改状态即时生效、服务端发包须用户按原路径截图复核（且截图必须是本端，不能拿参考端）。按用户硬要求如实声明。
4. **服务端未启动/DDL 未执行**：Flyway 表能否在真实 MySQL 建成功需启动验证；未 package、未启动。

## 用户拍板与落实（2026-11-18，3 项全部闭环）

- **Q1 `!night` 命名冲突 → 已定**：接受 `!wnight`（WeatherNightCommand），**不替换** s9 原生 `!night` 夜间特效命令。`!weather night/midnight` 亦可达冻结效果。代码已落地、编译过。
- **Q2 GM override 持久化 → 已定并落实**：GM `!weather`/`!wnight` 改走 `WeatherConfigService.setOverride/clearOverride` 写库（实测 `WeatherCommand.java:77/95-100`、`WeatherNightCommand.java:25/27-28`），与 web `/weather/v1/override/set|clear` 等价，重启后仍生效。lamp 保持不写库（纯放置工具，服务端无权威状态；日后要"灯具布局表"属另一功能）。**额外修一处数据卫生**：`WeatherConfigService.setOverride` 写新行前先将旧 `enabled=1` 置 0（`Service.java:127-137`），避免连设堆积多条生效记录、消除"最晚到期≠最新设置"语义歧义；`mvn -pl gms-server compile` exit 0。
- **Q3 orange-wz 全局注册 → 已查明**：`custom-orange_wz` **本就全局注册**（`~/.dsh/storages/mcp_connector.json:32-47`，`injectionMode:always`/`enabled:true`），无需重复注册。工具前缀是**会话启动快照**——本会话无该前缀且无 `mcp_connector_reconnect`（`ops_discover_tools` 实测无法激活），今早会话有全套 8 个工具。**正解：新开会话自动带上全局 orange_wz 工具**，照 `notes/orange-wz-resource-runbook.md` 直接执行 P6，无需每次重新分析/指定。已记入 Mnemon 项目文档（id 5228183c）。

**P6 WZ 资源仍未入库**（本会话无工具，非借口；新会话按 runbook 执行）。

## P7 wire 蓝军核对（逐字段实测，2026-11-18）

拉服务端真实发包序列（`BeiDou-Server_s9/.../WeatherPackets.java:71-83`）逐行对客户端解析（`CommonHandlers.cpp` WeatherSyncHandler）：

| 序 | 服务端 write | 客户端 read | 结论 |
| --- | --- | --- | --- |
| 1 | writeShort minuteOfDay :71 | read_short | ✅ |
| 2 | writeInt msPerGameMinute :72 | read_int | ✅ |
| 3 | writeByte sky :73 | read_byte | ✅ |
| 4 | writeByte flags :74 | read_byte | ✅ |
| 5 | writeInt skyElapsedSec :75（legacy 占位） | read_int（占位不消费） | ✅ |
| 6-8 | writeByte tint R/G/B :77-79 | read_byte ×3 | ✅ |
| 9 | writeShort rainbowSecsLeft :80 | read_short | ✅ |
| 10 | writeByte palette :81 | read_byte | ✅ |
| 11 | writeInt skyElapsedMs :82 | read_int | ✅ |
| 12 | writeInt skyToken :83 | read_int | ✅ |

12 字段全对齐，legacy 4B 占位保留（删掉后面全偏移）。0x373F lamp preview 14B（opcode 之后）亦对齐 `LampPackets.java:35-48`。

## 已知设计近似（非 bug，如实披露）

- 原生 Sprite 无统一 multiply-tint 通道，D1 核心闭环用 `GraphicsGL::drawscreenfill` 全屏半透明夜色叠加（上限 alpha 0.55）近似 bundle 的逐层 multiply；观感接近但不是逐像素乘法，UI 不被压暗。严格还原需给 GraphicsGL 加 tint uniform，列为后续增强。
- 户外判定 D1 先用"有 world state 才 tint"保守策略，未做 bundle 的 backmost (bS,no) 白名单（264 手工排除 + 57 sky bases），避免误压室内。完整判定 + skytable 双端一致留后续。
- 天空注入（云/月/雨雪 sheet）、地面堆积、路灯 sprite、音效、sway 均属 D1 预留项，接口未启用。

## 产物与证据文件
- PLAN.md（8 阶段完整计划）、notes/p0-preflight.md（接入前复核）、本文件。
- 编译日志：_weather_compile.log / _weather_compile2-4.log、_weather_build/obj/Weather*.obj（隔离编译硬证据）。
- 服务端编译证据见 weather-server-impl 子代理交付（BUILD SUCCESS / exit 0）。
