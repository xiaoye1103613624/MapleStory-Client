# 天气系统 V2 P0–P4 实施交付（2026-11-23）

> **2026-11-23 22:32 追加修正（用户验收追问后）**：
> - 满月加回：`WeatherBackLayers.cpp` 恢复 `nightDesert back/3` 满月（浅夜 0.6≤night<0.85 四季通用），
>   残月（冷季 night≥0.85）/魔法月（暖季 night≥0.85）收窄到深夜。切换口径=用户拍板"浅夜满月→深夜残月/魔法月"。
> - 闪电：`WeatherFx.cpp:190` 触发从 STORM-only+rain≥0.70 改为 RAIN 或 STORM+rain≥0.20；
>   `lightning_pulse` 静默率 20%→10%、可见时长 0.9s→1.5s（用户拍板"任意有雨天都可能闪电"）。
> - 最终产物 SHA `7dfed5ec8b17e7f8befa4ab5a8caf51f6aa572947cf5fefa50e5598ce760a14c`，已部署 live，T0/T10 一致、差异 0 字节，备份 `.bak-before-moonlight-20261123`。
> - 太阳出现时机：服务端时段档 band=2（正午 09:00–17:00）才显示（`kSkyDefs` cond_band=2）。
> - 闪电真机尚未验收；服务端需重启才能让 flags 时段档/0x3742 生效。

> 前置：README.md（设计）、PLAN.md（计划+决策）。本文档记录实际实现、构建、部署、验收清单。
> 用户 6 决策：按季节切换天体 / 参数化 / 公历近似+MM月DD日HH:mm / 全做 / Web重写 / 接受0x3742。
> 用户追加授权：一次性推进、保留现有工作树、服务端改完由用户手动重启。

## 1. 实际改动清单（行号级）

### P0 协议地基（双端）
- 服务端 `SendOpcode.java:375` 加 `WEATHER_ELEMENT(0x3742)`。
- 服务端 `WeatherElementPackets.java`（新）：version+gameDate(MMdd)+byte[16] elements 打包/sendTo/broadcast。
- 服务端 `WeatherService.java`：flag 新常量 `FLAG_BAND_MASK=0x18`/`FLAG_SEASON_COLD=0x20`、`timeBand()`/`seasonCold()`/`v2FlagBits()`/`sendElementSync()`；`TIME_DAWN 360→300`、`TIME_DUSK 1080→960`（Q2 时段亮度档塌陷修复）。
- 服务端 `WeatherPackets.java:76` flags 加 `v2FlagBits()`。
- 服务端 `WeatherConfigManager.java:191` 去掉无条件双清，keep=null 轴保持现状（Q1 修复）。
- 客户端 `WeatherTypes.h`：Flags 新位 + `Element` 枚举(16) + `time_band()`/`season_cold()`。
- 客户端 `WeatherState.h/.cpp`：game_date_/elements_[16]/element_version_ + `apply_elements()`（版本回退忽略）。
- 客户端 `CommonHandlers.h/.cpp` + `PacketSwitch.cpp`：`WeatherElementHandler` 收 0x3742（三处注册）。
- 客户端 `UIBuffList.h/.cpp`：HUD `%02d月%02d日 %02d:%02d`（gameDate 来自 0x3742，无则退化 hh:mm；日期/分钟都进缓存键）。

### P1 规则层（服务端）
- `SolarTerm.java`（新）：24 节气公历近似锚点 + `current()` + `season()`。
- `WeatherService.java`：`seasonAllows(profile,season)` 防冲突（夏禁雪/暴雪、春禁暴雪、冬禁花叶），注入 `pick()` 权重循环；`sendElementSync()` 在 `MapleMap.java:2646` 进图补发。

### P2 客户端渲染
- `WeatherBackLayers.h/.cpp`：Def/Layer 加 cond_cold/cond_band/night_min/night_max；K_CELESTIAL 天体层按服务端 flags 时段/冷热季 + night_level 选：白天太阳(`blackheavensky_sunset back/25`)、黄昏暖光柱(`blackmagician_base2 ani/0/0`, night<0.6)、冷季残月(`blackheavensky_night back/29`, night>=0.6)、暖季魔法月(`blackmagician_base3 back/34`)；K_FOG 叠加 EL_FOG 强度。

### P3 Web
- 服务端 `WeatherController.java`：`GET /weather/v1/v2/status`（季节/节气/时段/天体/现象强度/gameDate 总览）。
- Web `api/weather.ts`：`WeatherV2Status` + `getWeatherV2Status()`。
- Web `climateTab.vue`（新）：气候总览 tab（只读展示）；`index.vue` 注册 "气候总览 V2" tab。

### P4 素材收口
- `WeatherBackLayers.cpp`：K_AURORA（极光 `bm3_1 back/6` 青光带）、K_TORNADO（龙卷 `bm1_3 ani/5` 气柱）由 0x3742 的 EL_AURORA/EL_TORNADO 强度驱动。

## 2. 构建与验证
- 服务端 `mvn -o compile -DskipTests`：P0/P1/P3 三次均 EXIT=0、0 error。
- Web `vite build --config config/vite.config.prod.ts`：成功（climateTab 打包入 bundle）。注：`vue-tsc type:check` 报 3 处错误均在**与本改动无关的** `src/views/game/file/index.vue`（monaco 过时类型），本改动文件零类型错误。
- 客户端全量 Rebuild（`tools/_v2p0_build.bat`）：P0/P2/P4 三次均 `MSBUILD_EXIT=0`、0 error；最终产物 SHA `5736a67fe71329833b14d699fd83568c874231018d6131d539e0a13c1d3e9502`（5556736→最终体积见 build-v2p0）。
- 客户端部署：备份 `MapleStory.exe.bak-before-v2-20261123` → 覆盖 live → T0/T10 SHA 一致、差异 0 字节（无回滚）。

## 3. 待用户操作（阻塞项，如实声明）
1. **服务端需用户手动重启**（端口 7575 java PID 8128 未动）：P0/P1/P3 的服务端改动（协议/规则/Web接口）未生效直到重启。重启后 0x373D flags 新位与 0x3742 才开始下发。
2. **抓包对拍**（P0 门禁）：重启后抓 0x373D（flags bit3-4 时段、bit5 冷热季）与 0x3742（version/gameDate/byte[16]）字节，确认双端对齐。本轮未做（服务端未重启）。
3. **真机逐项验收**（P2/P4 门禁）：见下清单，用户确认前不称"完全完成"。

## 4. 真机验收清单（未验收）
- [ ] HUD 显示 `MM月DD日 HH:mm`（如 11月23日 19:30），与 Web 气候总览日期一致。
- [ ] Web "气候总览 V2" tab 显示当前季节/节气/时段/夜色强度/天空，刷新按钮正常。
- [ ] 白天显示太阳、黄昏暖光柱、冷季夜残月、暖季夜魔法月；时段/季节切换时天体平滑切换。
- [ ] 0x3742 下发的雾/极光/龙卷强度生效（Web 改强度→广播→真机可见）。
- [ ] Q1：夜里选"不改时段+暴雨"不再变白天（时间保持夜）。
- [ ] Q2：黎明/正午/黄昏/黑夜 亮度各不相同（TIME 常量入 ramp 生效）。
- [ ] 防冲突：夏季 roll 不出雪/暴雪（可多次刷新天气观察或查日志）。
- [ ] 极光/龙卷兜底观感可接受（bm3_1 青光带 / bm1_3 气柱染色）。
- [ ] 帧率无明显下降；旧客户端收到 0x3742 不崩（PacketSwitch 未注册包静默忽略——已注册，旧 exe 才需验证）。
- [ ] 服务端重启后配置不丢失（DB 现有 7 表未改结构）。

## 5. 诚实边界
- **仅 P0/P1/P2/P3/P4 的"核心可用版"落地**：Web 采用"气候总览只读 tab"，**未做完整"24节气×4时段编辑矩阵 + 3抽屉 CRUD"**（需后端新气候规则表 + 新 REST，属 PLAN §5 的进一步增强；本轮按"一次性但分阶段"先交付可运行骨架，缺素材现象用参数化兜底）。
- 元素强度当前 P1 默认全 0（0x3742 发全 0），**Web 改强度→实际下发** 的完整链路需 P4 后续把元素强度写入 DB 并在服务端 sendElementSync 读回——本轮未接 DB（诚实声明）。
- 一切真机表现未验收；服务端未重启，端到端未验证。
- 全程未做 DDL/DML；未改服务端运行进程。
