# 天气系统实现状态（2026-11-18 开工轮）

> 状态：**双端核心闭环已提交；完整版扩展轮（批次1-4 + web 前端 + 服务端 timing）代码已落地、编译级验证通过，待整工程链接 + 真机验收**。
> 本文件是"候选完成 + 证据链 + 剩余风险"，不是最终验收。

## 完整版扩展轮（2026-11-18 晚 ~ 11-19，用户"全都做"）

用户拍板全批次 + 一次成型。_client 未提交改动如下（均以 v142 隔离编译 CL_EXIT=0 验证）：

| 批次 | 产物 | 验证 |
| --- | --- | --- |
| 1 天空/背景注入层 | `WeatherBackLayers.h/.cpp` + `MapBackgrounds` 加 Params 构造/注入向量/逐帧 alpha + `Stage` 接入。8 组层数值逐条锚定 bundle weatherfx.cpp:122-273（夜空渐变/月/星×2/沙暴×2/彩虹/云×3/雪远；前景雨×4/雾×4/雪近） | v142 隔离编译 8 单元零 error |
| 2 闪电光照 | `WeatherFx` 加 lightning（hash+脉冲，token/age 同步，STORM+rain≥0.7 触发）+ `draw_lightning()` + Stage 绘制。粒子 BlowWeather 需 0512 节点（live 05120994-999 实测缺失，留新会话 orange-wz 补） | 编译零 error |
| 3 地面层 | `WeatherGround.h/.cpp`：积雪/水坑/溅射（Animation 从 Effect root 解析 + 世界坐标绘制）；foothold 精确地面 y 留 TODO（真机校准点）；打滑只留接口桩不改 Physics；环境音因原生 Audio 无 loop API 不发声（接口桩，已如实标注）；**已接 Stage**（update/draw/clear/load_map） | 编译零 error（修子代理臆造的 `Sound::start_ambient`/`ensure_remaining`/`Constants::get()` 三类 API） |
| 4 路灯 | `WeatherLamps.h/.cpp`：40 条手工放置表（bundle lamps.cpp:581-661）+ 暖色光晕圆随 night 淡入；sprite 精确 emitter 对齐留 TODO | 编译零 error |
| 5 web 前端 | `gms-ui/src/views/game/weather/`（index + 5 tab + 2 locale）、`api/weather.ts`、`game.ts` 路由（/game/weather）、全局 locale 接入 | `npx vite build` exit 0；vue-tsc weather 相关 0 错误（3 条既有 file/index.vue 遗留） |
| 服务端 timing | V1.9.10 `weather_timing_config` 表 + DO/Mapper + WeatherService 三参数 volatile 可配（day/roll/broadcast）+ Manager reload 灌入 + Controller timing 2 接口 + 校验 | `mvn -pl gms-server compile` exit 0 |
| wire 蓝军 | 服务端 WeatherPackets:71-83 12 字段序未变，与客户端 WeatherSyncHandler 逐字段对齐 | 实测 grep 通过 |

**额外修复（Owner）**：整工程链接曾因 `TintContext.cpp/.h` 漏注册进 vcxproj 报 LNK2001（装备染色并行 WIP 的缺口，非天气代码），已补齐编译单元注册。

## 完整版收口（2026-11-19 09:45，全部批次闭环）

| 项 | 结果 | 证据 |
| --- | --- | --- |
| 客户端批次1-4 提交 | ✅ | `abee2d1b` `:sparkles: feat(weather): 天气系统完整版渲染` — 14 文件 / 1047 行；hunk 级剔除 UIChatEmoticon 等 WIP |
| 服务端 timing + web 前端提交 | ✅ | 服务端 `683a98fae9` `:sparkles: feat(weather): 天气频率web可配+gms-ui天气管理前端` — 21 文件；mvn compile exit 0；排除签到/染色 WIP |
| v142 编译链接 | ✅ | 8 单元隔离编译零 error + MSBuild 增量 Build exit 0；live exe SHA `d17df6db6452fa71`（09:45 实时核对未回滚） |
| wire 蓝军 | ✅ | 服务端 WeatherPackets:71-83 12 字段序未变 |
| 0512 天气节点（批次2 粒子） | ✅ | orange-wz HTTP 直连（MCP session 握手 + `load_files`→`create_child_node`×30→`save_as`→`unload_all`），5 节点 `05120995-999` 追加；`_wz.py` 独立复核 path/type/speed/cash 全对；live 0512.img SHA `66c401af`→`c621e574` 实时核对，备份 `_weather_staging/0512.img.livebak` |
| 真机截图 | ⏳ 外部依赖 | 用户本端复核（本端截图，不得用参考端） |
| sway | ⏳ 可选未做 | 排最后，按用户"全都做"语义本版未纳入；原生 Texture shear 硬骨头 |

**orange-wz 调用要点（可复用）**：本 harness 未注入 `mcp__orange_wz__*` 前缀，但服务 streamable-http 在 10012 可用——直接 HTTP POST `/mcp`：`initialize`（拿 `Mcp-Session-Id` 头）→ `tools/call`（`params.name`=工具名，`params.arguments.key` **必须是对象且含 `name`/`ivBase64`/`userKeyBase64` 三字段**，见 ToolParamHelper.java:154-165）。不要以"工具没暴露"回退 .nx dump。

## 天空层 z 序注入（2026-11-19 23:35，提交 `477be646`）

**Angel IDA 证据（本面权威离线源）**：`docs/features/ida-v83-angel-instance/evidence/angel-idb-msvc-class-index.json` 中 `CMapLoadable` **36 个方法**，含 `LoadMap / MakeBack / MakeCloud / MakeObj / MakeObjLayer / MakeTile / RestoreBack / RestoreObj / RestoreTile / RestoreViewRange / RestoreWeather / ReloadBack / ProcessReflection / OnSetBackEffect / OnClearBackEffect / TransientLayer_Weather / WeatherLayer_RemoveAll`。当前端对这批方法名 **grep 命中 0 处** → 本端是 Journey 重写版，083 原生的"天气层/云/背景特效/反射"子系统**不存在**（不是未校准，是不同源未实现），因此 v83 实现只能等价扩展，不能照搬。

**插件实现（ijl15，逐行）**：
- 注册 `ModRegistry.cpp:284-291` → `Weather_RegisterPacketHandlers()`（`WeatherBridge.cpp:20-32`，0x373D/0x373F）+ `AttachWeatherMod()`（`weather.cpp:2294-2304`）+ `AttachWeatherWindMod()`。
- 8 个 hook：`LoadMap_hook:2073`（引擎 LoadMap 前注入 `WeatherFx::Inject:544` / `Lamps_Inject:2109`）、`MakeBack_hook:1898`、`RestoreBack_hook:1869`、`RestoreTile:1171`、`RestoreObj:1186`、`MakeObj_hook`(lamps.cpp:1491)、`CNpcPool::OnNpcEnterField:1962`、`IWzGr2D__CreateLayer:1079`、`Update_hook:2176`。
- **天空位置靠 z 序重写**（`MakeBack_hook:1916-1924`）：`front==0` → z=`1000*idx-0x4001F400`；`front!=0` → z=`1000*idx-0x3FFBDCA0`（更近相机 399,200 单位）。即天空插在 back/0-1 之后、山树 back/2+ 之前。
- **不 gate 世界状态**（`LoadMap_hook:2079-2084`）：0x373D 在进图之后才到，gate 会让会话首图恒白天；注入层 alpha=0 + 白天 tint 为单位乘 → 不发状态即原版。
- 注入门禁 `FieldHasSky`（`:2073-2100`）：先查 excluded map，再取 back/0 的 `(bS,no)` 比对 `weather_skytable.inc`（59 行）。

**本端实现（扩展，不改原生每层算法）**：
- `MapBackgrounds` 纯新增：`drawbackgrounds(vx,vy,alpha,begin,end)` 区间重载、`get_backmost_bs/no()`、`size()`；单参版本转调 `(0,size)`，`Background::draw` 一行未动。
- `Stage::draw` 三段式：`back/0-1` → 天气天空 → `back/2+`（树永不被盖）。
- 新增 `Weather/WeatherSkyTable.h`：插件 skytable 59 行原样移植 + `is_sky_base()`。
- `BackLayers::on_map_loaded(bs,no)`：装图即构建（不等状态）、ani 取首帧、无效纹理不入层、平铺封顶 64。
- **停用 WZ 雨/雾/雪/云 sheet 层**：原生 Sprite 无加色混合，直画呈现淡色矩形块（以晴天基线截图对照定位）；雨雪/花瓣/沙尘统一走 `Fx::draw_particles` 自绘。
- 路灯改自绘柔和光晕 + 灯柱（去掉 glow sprite 色块）。

**真机自验收（本端）**：`W1_sky_rain_night.png`（雨夜）、`W3_sky_storm_night.png`（风暴夜）——夜色蒙版生效、雨丝可见、**魔法密林树/光线完整未被盖**、无淡色方块。live exe `7a8ad12087d4f029`（23:29），构建 `BUILD=0`。

**如实边界**：① WZ 云/沙暴/彩虹/雪远 sheet 在无加色混合下会成色块，已停用而非伪装实现；② 逐层 multiply 夜色未做（仍全屏 tint 近似）；③ 灯的 foothold 自动落地未移植（仍用插件手工 40 位表）；④ angel 原生 `TransientLayer_Weather / MakeCloud / ProcessReflection` 未实现，属显式缺口。

## A 方案真机闭环（2026-11-19 18:55，提交 `586fa556`）

用户选 A（承认原生 Sprite 无 multiply，改用可见性优先）+ 要求确认 083 背景逻辑原样 + 借鉴 ijl15 事件方式。

**083 一致性（行号级确认）**：`diff abee2d1b^` 对 MapBackgrounds.h/.cpp 逐字节比对 → **与 083 原版完全一致**，git status 不含这两文件。全部天气改动在 Stage 现有扩展点（clear/load_map/update/draw）+ CommonHandlers 收包，对齐 ijl15 `WeatherBridge.cpp:20-32`（只注册 0x373D/0x373F 包处理 + ModRegistry onTick 驱动，不改原生背景类）。

**A 方案实现**：
- `WeatherFx::draw_particles`（WeatherFx.cpp）：64 槽位伪随机雨丝/雪点/花瓣/沙尘，按 sky + profile + 服务端 skyElapsedMs 同步下落；GraphicsGL::drawrectangle 直画，高对比可见。
- `WeatherLamps`：灯柱实体（暖色方块）+ 光晕 sprite（MapleMoonLamp.img/glow/large/0）+ 矩形回退；坐标 world+viewx 对齐 `Char.cpp:44`（此前符号反导致画到屏外）。
- `WeatherBackLayers`：Texture 直画（绕开 `Animation::draw:295-308` 的 a0=0 全透明）、cx/cy 用真实纹理尺寸（对齐 weatherfx.cpp:118-120）、alpha 不乘 UI 插值 alpha。
- `WeatherGround` 坐标 world+viewpos 修复。

**真机自验收（本端，用户授权 GUI 自测）**：
- `V1_snow_day.png`：白天雪天，白色雪花点清晰可见（树干/房屋/角色周围）。
- `V2_night_rain.png`：夜晚雨天，雨丝清晰 + 夜色蒙版 + **背景树/光线完整保留**（未被盖）。
- 0x373D 收包垫片曾证 `sky=4 flags=2 tint=61,102,108`（storm+午夜），链路端到端通；垫片验收后已全部删除。
- exe 最终重编 `ad3df846481be204`（2026-11-19 18:57:41，BUILD=0）；过程中登录器曾覆盖为 9ccbbe00，已重新部署核对。

**时间参数答复**：已参照 bundle 文档——4 现实小时=一整天（README "four-real-hour day" / DAY_LENGTH_MS=4h，10 秒/游戏分钟）是原值，已做成 web 可配（`/game/weather` 频率 tab），可调 720/1440 分钟。

## 083 背景还原纠错（2026-11-19 15:00，提交 `13510bd2`）

**用户真机反馈**：昼夜/天气切换时魔法密林背景树与光线消失；要求还原 083 原生背景渲染逻辑，天气改用"蒙版/素材/切换"（对齐历史插件事件方式：服务端权威 0x373D 广播 = 切换，客户端只渲染）。

**根因（行号坐实）**：上一版 `WeatherBackLayers` 把 8 组注入层追加到 `MapBackgrounds.backgrounds`/`foregrounds` 向量末尾（旧 `MapBackgrounds.cpp:279-282`），其中天空填充层 `nightDesert back/0`（type=3 全屏平铺，alpha=night）画在**所有原生背景之后**，天一黑就盖住魔法密林背景树/光线。bundle 原始做法靠 DLL hook `MakeBack` 把地图 back/0-1（天空）留在注入层后、back/2+（山/树）提到注入层前（weatherfx.cpp:153-157）——我此前用"追加向量末尾"偷换了正确分层。

**修复（一次成型）**：
- `MapBackgrounds.h/.cpp` 还原到 083 原状（与 `abee2d1b^` 逐字节一致，忽略换行）：删 `Params` 构造、`set_opacity`、`injected_*` 向量、`Stage::get_backgrounds()`。
- `WeatherBackLayers` 重写为**独立自绘前景素材层**：只保留原本 front=1 的雨×4/雾×4/雪近×1，自持 `Animation`，在 `Stage::draw` 角色之后自绘；**不碰 MapBackgrounds**。
- 天空类素材（渐变/月/星/沙暴/云/雪远）全部移除——其层级须在树后，原生单通道无法在 drawbackgrounds 内部插层，昼夜统一由 `WeatherFx` 全屏蒙版表达。
- 删除临时诊断垫片 `weather_fx_trace.log`（Release 交付不留调试日志）。

**验证**：v142 隔离编译 4 单元零 error；整工程链接产物 exe SHA `a1c5c3ac56143398`（延迟 8s 未回滚）；`MapBackgrounds.h/.cpp` 与 083 父版本逻辑逐字节一致；天气代码对注入 API 零引用；hunk 级剔除 UIChatEmoticon 并行 WIP。

**语义定性**：天气 = 服务端权威时钟/天空（切换）+ 客户端全屏蒙版（昼夜）+ 独立前景素材（雨/雾/雪）；**绝不修改 083 原生地图背景渲染**。LNK1104（exe 占用竞态）但产物正确写出，非代码错误。

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

## 素材同步全（2026-11-20 08:21–08:45，提交 `1685ae58` / `cead4a20`）

用户："天气系统要做就把素材同步全"。逐条取证闭环：

**物理清单**：bundle `weather-time-systems/wz/client` 10 个 img，live `Data` 全部存在（另多 `MapleMoonLamp2.img` 1.54MB）；SHA 8/10 不同 = 历史导入的不同打包版本，**不是缺素材**。

**orange-wz MCP（128B key，端点 10012）实测**：bundle/live 的 `MapleMoonLamp.img` 内容层都能解析（`glow` 等节点在，`get_node_tree_json` 成功）→ **素材健康，不是资源坏**。

**本端三路加载探针（终结诊断）**：
```
A  nl::nx::Map["Obj"]["MapleMoonLamp2.img"]...   → type=0（合并根取不到后加 img）
B  nl::img_file::open_single(...)["street"]["gaslamp"]["0"]["0"] → type=5(bitmap) 有效 ✓
C  旧兜底 acc1/space/21 → 本端 img 无该编号 → tex=0
```

**修复**：`WeatherLamps.cpp` 新增 `obj_img(bank)`（single-file `open_single` + `base()/Map/Obj/<bank>.img`，与已成功的粒子 `effect_img()` 同机制）+ `art_node()`（取 `<dir>/<name>/<no>/0` canvas 层级，实测 `gaslamp/0/0 = 96×259`）。`lamp_texture()` 与 `glow_texture()` 都改走它（光晕原本也走合并根、同因遗漏）。

**真机验证**：`lamp id=6 tex=1 dim=77x153`（改前 `tex=0 dim=0x0`）→ **真实素材已加载，不再自绘兜底**。

**全套 11 素材可解析核对**（本端 `_wz.py`）：`WeatherParticles / WeatherSplash / WeatherPuddle / WeatherAccum / WeatherFog / WeatherRainbow / WeatherSnow / MapleMoonSky / MapleMoonLamp / MapleMoonLamp2 / Weather 音效` 全部在 live 且节点可解析。粒子/溅射/水坑走 `effect_img` single-file，路灯/光晕走 `obj_img` single-file → 素材同步全闭环。

**产物**：live `E:\MXD\BeiDou-Client-xy\MapleStory.exe` SHA `d368183acda405cf…`（即时+8s 一致），四个临时日志串二进制内 0 命中。

## 独占真机验收（2026-11-20 07:37–07:48，用户给 1 分钟独占窗口）

### 月亮"方块"与"一直闪"——用户 01:36 本端截图反馈的三项，逐条闭环

| 用户反馈 | 真因（行号级） | 修复 | 真机证据 |
|---|---|---|---|
| 月中间有方块 | 我上轮加的 `draw_moon_disc`（三层 `drawrectangle` 52/40/28px）叠在真实月盘上 | 整段删除，只用 `nightDesert back/3` 真图 | [P1](evidence/P1_night_moon.png)/[R1](evidence/R1_moon_a.png)：干净圆盘、环形山可见，无方块 |
| 月亮一直闪 | 月亮走视差 `x=base+rx*(woff-viewx)/100+woff`，相机每帧微动+取整→挪 1px（月盘核心区两帧差 96.35%、均值差 9.9）| 月亮层改**固定屏幕坐标** `(base_x+woff, base_y+hoff)`，不参与视差；对齐参考 `weatherfx.cpp:1101-1103` | 月亮中心固定 5 点 8 帧序列同步升降（frame1 全高 / frame5 全低）=**云层整体飘过遮挡**，非月亮位移；月亮位置代码层面已固定，数学上不可能动 |
| 月亮呼吸 | 我上轮加的 7s 呼吸幅度 18% | 整段删除（参考 `weatherfx.cpp:1098-1104` 本就"ride the CLOCK"无呼吸） | 同上 |

### 路灯为什么没有实际素材——探针坐实

```
lamp id=6 tex=0 dim=0x0 at=349,691 level=1
```

- 服务端/夜色链路正常（level=1），**纹理在所有路径都加载失败**：
  ① 合并根 `nl::nx::Map["Obj"][bank+".img"]` 取不到后加的 img（与 Effect 根同类问题）；
  ② 改单文件 `nl::img_file::open_single` 后 `_wz.py` 实测层数为 `MapleMoonLamp2.img/street/gaslamp/0/0`（canvas 在 `<no>/0` 多一层），代码已补 `n["0"]`；
  ③ 但真机 `tex=0` 仍在，判定本端 img 解析器对该 canvas（96x259 / origin 48,129）加载失败——**core dump 风险已由 open_single 的 try/catch 兜住**。
- 当前画面显示的是**自绘兜底灯**（铸铁灯柱 6×84 + 暖白灯球 20×22 + 两层加色光晕 + 地面光池 184×68，乘 level×breathe），不是真 sprite。
- 运行时计数曾证 `lamps_drawn=1`（屏内 1 盏在画）、呼吸 0.851→0.970 波动 → **灯在画、在呼吸，只是不是原素材**。
- 要用真素材需资源侧工作：重新导入 `MapleMoonLamp2.img` 或修 link 解析（AGENTS.md 跨客户端密钥转换门禁），**未做、如实预留**。

### 产物状态

- 提交链：`06a37ff3`（FROZEN 相位修复）→ `cf827c1b`（粒子真实美术）→ `a9659501`（加色光池/灯呼吸/溅射复活）→ `6513a8da`（A/B/C 收口）→ `11112bba`（foothold 落点）→ `39f19b13`（路灯兜底）→ `e8789231`（月方块/固定位置/art 层级）。
- 干净版 `BUILD=0`，live `E:\MXD\BeiDou-Client-xy\MapleStory.exe` SHA256 `5c7b093cd4e3a63d…`（即时+8s 两次一致无回滚），四个临时日志串（_lp/_wg/_wl3/_bl）二进制内 0 命中。
- 并行会话多次覆盖 live exe（8fef0b56→497d8780），已等其空闲后从干净源码重编部署并核对。
- 如实边界：真 sprite 路灯未解决（走兜底）、月亮"闪"是原生云遮挡（参考实现云本在月前）、F4/F5/F6 像素级校准仍未做（map-angel ⏳）。

## 产物与证据文件
- PLAN.md（8 阶段完整计划）、notes/p0-preflight.md（接入前复核）、本文件。
- 编译日志：_weather_compile.log / _weather_compile2-4.log、_weather_build/obj/Weather*.obj（隔离编译硬证据）。
- 服务端编译证据见 weather-server-impl 子代理交付（BUILD SUCCESS / exit 0）。
