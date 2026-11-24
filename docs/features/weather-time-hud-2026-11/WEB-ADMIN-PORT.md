# 天气 Web 管理：上一代 gms-ui 体验补齐到 s9（2026-11-23）

用户诉求："web 管理之前插件服务端有维护这个功能，参考下" → 用户裁定"全补"。
调查结论：所谓"插件服务端 web 管理"实为**上一代 `BeiDou-Server` 自带的 gms-ui 天气页**（S10 同款）；`BeiDou-ijl15` 与 `E:\MXD\扩展改动\天气系统` 只有 C++ 插件 + GM 命令，无 web。已把上一代 6 个亮点全部移植到 s9，不动 s9 原有 6-tab 深度配置。

## 参考实现（行号证据）
- 上一代 Service：`BeiDou-Server/gms-server/.../WeatherAdminService.java`
  - `:25-34` 时段选项（day/dusk/night/dawn/midnight/clock/release）
  - `:75-154` apply 双轴语义；`:160-175` broadcastAll；`:178-195` notifyWorld→yellowMessage
  - `:38-72` status（wallClock/nightLevel/equivalentCommand）
- 上一代前端：`BeiDou-Server/gms-ui/src/views/game/weather/index.vue`
  - `:6-11` 两条 alert；`:42-46` 等价指令；`:52-59` auto 按钮；`:111/:176-215` 预览
  - locale `zh-CN.ts:2-29` 原文案

## 关键架构约束（为什么不能照搬旧版内存实现）
s9 是 DB 权威：`WeatherConfigManager.reload()`（`:100-177`）每次热更都重新施加 DB override，`applyOverride`（`:179-195`）在无 DB 行时 clearTime/clearSky。
→ 旧版"内存 setTime 即时切换"会在任何配置页保存后被 reload 冲掉；本移植保持与旧版一致的**临时 override 语义**，需要持久请用现有「临时覆盖」页写 DB。文档中已如实说明。

## 后端改动（s9 gms-server）
1. **新增** `org/gms/service/WeatherAdminService.java`（~250 行）：移植 status/apply/broadcastAll/notifyWorld；s9 无 DTO，沿用自由 `Map`（与现有 `overrideSet` 一致）。
2. `controller/WeatherController.java`：注入 `WeatherAdminService`；新增
   - `POST /weather/{v}/apply`（自由 Map body，语义同 `!weather`）
   - `GET /weather/{v}/quick/status`
3. `service/WeatherConfigService.java`：`state()` 增 `wallClock/nightLevel/equivalentCommand`；加 `formatMinute` helper + 注入 `WeatherAdminService`（`@AllArgsConstructor` 自动装配）。
4. i18n：`message_zh_CN.properties` / `message_en_US.properties` 末尾加 `WeatherAdmin.notify`（黄字提示，{0}=时刻 {1}=天空）。

## 前端改动（s9 gms-ui）
1. `api/weather.ts`：`WeatherStateResult` 增 3 字段；新增 `WeatherApplyParams / WeatherQuickStatus` 类型 + `applyWeather / getWeatherQuickStatus`（transformRequest 防二次包装）。
2. **新增** `views/game/weather/quickTab.vue`：双轴单选（时段 8 项 / 天空 11 项）+ 实时"将执行"预览（逐条移植旧版 `previewCommand`）+ 一键 auto + snap 勾选 + 状态条（wallClock/nightLevel/等价指令）。
3. `views/game/weather/index.vue`：状态条加 wallClock/nightLevel/等价指令 tag；新增首个 `quick` tab；import + visited。
4. locale `zh-CN.ts` / `en-US.ts`：两条引导 alert 原文案 + 全部新 key。

## 六项补齐对照
| 旧页亮点 | 落地位置 |
|---|---|
| 等价命令展示 | `WeatherAdminService.status()` + `state()` + index.vue 状态条 |
| 实时"将执行"预览 | `quickTab.vue previewCommand` |
| 一键 auto | `POST /apply {auto:true}` + quickTab 按钮 |
| 语义快捷 + midnight/分别 release | `WeatherAdminService.apply()` switch + quickTab 双轴 |
| 全服黄字提示 | `notifyWorld` + i18n `WeatherAdmin.notify` |
| wallClock/nightLevel + 引导文案 | `state()` + 两条 alert |

## 构建验证（实测）
- 后端 `mvn -q -DskipTests package` → `MVN_EXITCODE=0`（首次 `ResultBody.success(Map,Map)` 无重载编译错误已修正为单参）。
- 前端 `npx vue-tsc --noEmit --skipLibCheck` → 天气模块 0 错误；仅剩 3 个既有 `file/index.vue` 类型错误（本功能未触碰，MEMORY 已记载）。
- 前端 `npx vite build --config ./config/vite.config.prod.ts` → `VITE_EXITCODE=0`。

## 诚实边界
- **服务端未重启**：新接口/日志/黄字提示需重启才生效（用户明确自己重启）；未重启前 8787 页面 quick tab 调用新接口会 404。
- 前端 dev server（8787）源码即 live，刷新即见新 tab；后端 8686 需重启后才响应新端点。
- 即时切换为临时语义，任何配置页保存触发 reload 后回到 DB 权威状态（s9 架构约束，非 bug）。
- 黄字提示真机未验收；dbx/数据库未碰（本次纯代码，未连库未写数据）。
- 未提交 git（用户未要求）。
