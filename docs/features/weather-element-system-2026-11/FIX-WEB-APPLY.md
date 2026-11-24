# 修复：天气 web「改不了时间/天气」+ 405（2026-11-23 15:21）

## 一、根因（逐条行号级）
1. **405「当前请求方法不支持」**：不是前端 bug、不是服务离线。
   - 运行进程 PID 2444（8686 持有者）启动于 **14:51:15**，classpath = `target\classes`（IDE 启动）。
   - 旧版 `WeatherController`（git HEAD）无 `/weather/v1/apply` 端点，只有 intensity/region/.../state。
   - 前端 `quickTab.vue` 的"保存数据"→ `applyWeather`(`api/weather.ts:122` POST `/weather/v1/apply`)打到不存在的 POST 路径 → Spring 返回 405（`exception_zh_CN.properties:36 REQUEST_METHOD_SUPPORT`）。
   - 第二条 toast「游戏服务可能未在线」是 catch-all 文案，非事实；状态条有实时数据即证明服务在线。
2. **设置能力被上一版删掉**：我曾把 `WeatherAdminService.apply()` 改成无条件重置（清 override），导致即使端点存在也无法"设成某时刻/天气"。

## 二、修复（一次性做全）
后端（已重新编译，class 时间 15:21）：
- `WeatherService.java`：新增 `setTimeFlow(minute, untilMs)`/`setTimeFreeze(minute, untilMs)`/`isTimeFlow()`；`minuteOfDay()`(:185) 在 flow 模式返回 `目标分钟 + 已流逝游戏分钟`（按 DAY_LENGTH_MS 换算），不设 FLAG_FROZEN → 客户端自绘继续走；`clearTimeOverride` 复位 flow。`setTime(minute)` 保留为 freeze 兼容既有调用。
- `WeatherService.FLAG_FROZEN` 广播条件仍是 `isTimeOverridden()`；flow 模式 timeOverride 已置位，但仍会带 FROZEN 标志——注意：这里有个语义细节见文末"诚实边界"。
- `WeatherConfigManager.java`：ActiveOverride 加 `timeMode`；reload(:147) 读 DO 的 timeMode；applyOverride(:189) 按 mode=1→setTimeFlow / mode=0→setTimeFreeze；并在施加前 clearTime/ClearSky 两轴残留（单行权威语义）。
- `WeatherConfigService.java`：`setOverride` 加 6 参数重载（含 timeMode），旧 5 参数重载保留（兼容 3 个 GM 调用）。
- `WeatherOverrideDO.java`：加 `timeMode` 字段。
- **新 migration `V1.11.20__weather_override_time_mode.sql`**：ALTER TABLE 加 time_mode 列（默认 0）。V1.9.9 已还原原样（避免 Flyway checksum 失败）。
- `WeatherAdminService.apply()` 恢复设置：语义时段(day/dusk/night/dawn)=flow、指定时刻(clock)与 midnight=freeze、写 DB override（holdMs=1h 到期自愈），auto/release=重置。
- `WeatherController` 的 `POST /weather/{v}/apply` 端点不变（class 已含）。

前端：功能链路本就正确（保存→apply、重置→auto），无需改；仅更新两条 locale 提示说明"保存=设置（约1h自愈）/重置=清空"。

## 三、构建验证
- 后端 `mvn -q -DskipTests package` → EXIT=0；javap 确认新 class 含 `apply`/`setTimeFlow`/`setTimeFreeze`，编译时间 15:21。
- 前端 `vue-tsc` 天气模块 0 错（仅既有 file/index.vue 3 错）；`vite build` EXIT=0。
- 客户端无需重编（本次未改 C++）。

## 四、待用户操作（唯一阻塞）
**重启服务端**（IDE 停 PID 2444 再启动），让 JVM 加载 15:21 的新 class 并执行 V1.11.20 migration。重启后：
1. GET `/weather/v1/quick/status` 应正常（无需 token 时 401 属正常，前端带 JWT）。
2. 快速切换选"正午+下雨"点保存 → 应写入 DB、黄字提示、游戏内时间跳到 12:00 并继续走、天空下雨约 1h 后自愈。
3. 点"重置为自然" → 时间回墙钟、天气自然 roll。
4. 405 应消失。

## 五、FLAG_FROZEN 语义漏洞（已一次性修掉，未留给用户踩）
- 初版 flow 模式复用了 `isTimeOverridden()` 判断 → 仍发 FROZEN → 客户端 `WeatherState.cpp:57` 停摆，与"时间逐渐增加"矛盾。
- 修复：`WeatherService.isTimeFrozen()`（仅 freeze 为真，flow 为假）；`WeatherPackets.java:64` 改用 `isTimeFrozen()` 决定 FLAG_FROZEN。客户端逐字核对（`CommonHandlers.cpp:328-363` 解 flags → `WeatherState.cpp:22-23` 直接置 minute、:57 仅 FROZEN 停摆）：flow 包带目标 minute + SNAP、不带 FROZEN → 立即跳到目标后继续 tick。**客户端无需改动**。
- 最终打包 EXIT=0，`WeatherService.class`/`WeatherPackets.class` 时间 15:25:49。

## 六、诚实边界
- 真机未验收；**服务端未重启前一切改动未生效**（运行 JVM 仍是 14:51 的旧 class，405 仍在）。
- V1.11.20 migration 由 Flyway 在重启时执行（ALTER 加列），未手动连库。
- 未改客户端 C++ / 未重新部署 client exe（本次纯服务端+前端）。
- 未提交 git（用户未要求）。
