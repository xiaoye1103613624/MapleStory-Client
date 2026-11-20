# 天气现实化强度与细节设计（2026-11-20）

> 依据：`E:\MXD\扩展改动\天气系统\weather-time-systems`（README 349 行 / IMPLEMENTATION.md 1014 行 / 源码）逐条取证。
> 原则：参考文档**没有**小/中/大三级，用户要求"按现实生活丰富实现"属新增设计；
> 时间比率（4h/天等）保留可配不动，其余按现实优化。

## 一、取证结论（行号级）

### 1. 强度：原包是"连续 alpha + 不同现象"，没有三级枚举

| 项 参考 | 行号 |
|---|---|
| `weather_profiles.inc` 只有 9 个天空；`cloud`/`rain` 是连续 0..1 alpha | `weather_profiles.inc:2-50` |
| 行序即 wire format，插行会静默错位 | `weather_profiles.inc:5-8` |
| 暴雪 `native=5120997` item `count=280` vs 默认 100；rain=0 | `:22-24`、`:54-56` |
| 雪用双 sheet（远小慢暗/近大快亮），暴雪不画 sheet 只堆粒子+4 雾带 | `README:60-63`、`IMPLEMENTATION.md:921-930` |
| 雨的强度靠水坑**生长**而非增多：`PUDDLE_MAX=26`、`PUDDLE_PLACE_PER_S=3` | `IMPLEMENTATION.md:808` |
| 粒子速度在 build 时定死（VM_FOREVER），天空/进图时才重掷；风不在持续时间内涨落 | `README Known limits:295` |
| 淡入淡出 `FX_FADE_STEP=0.015`（约 2s），storm 的 profile.fade=0.008（更慢） | `weatherfx.cpp:18`、`weather_profiles.inc:48` |

### 2. 昼夜时间/速度

| 项 | 值 | 行号 |
|---|---|---|
| 一天 | 4 现实小时（10 秒/游戏分钟），墙钟纯函数无漂移 | `WeatherService.java:154`、`README:84` |
| 天空重掷 | 15 分钟（`ROLL_INTERVAL_MS`） | `WeatherService.java:157` |
| 广播 | 60 秒/世界 + 玩家进图补一包 | `:160`、`WeatherPackets.java:88-93` |
| GM override | 1 小时有限时长（防 GM 冻结午夜后离线留世） | `IMPLEMENTATION.md:800` |
| 黄昏/黎明 | 05:00–07:00 / 17:00–19:00，线性 `nightLevel` | `WeatherService.java:259-279` |
| 客户端包间自推进 | `State::tick` 本地推进 minute / skyElapsedMs | `WeatherState.cpp:40-58` |

### 3. 路灯细节（文档完整，当前实现缺）

| 项 | 参考值/语义 | 行号 |
|---|---|---|
| 光敏开关迟滞 | ON=0.35 / OFF=0.28，**OFF 必须 >0**（归零后 apply 停跑，=0 会白天常亮） | `lamps.cpp:415-430` |
| 预热/冷却/错峰 | warmup 1200ms、cooldown 250ms、stagger 2600ms/灯（逐盏亮，不齐刷） | `:430-434` |
| 暮色渐变 | DIM_FROM=0.14 / DIM_MAX=0.34，带下限（大白天不漏光） | `:436-450` |
| 灯芯混合 | **必须 LB_NORMAL**：实测加色出不了暖色（白玻璃夜染后蓝领先 66，加暖光三通道一起涨到 clip，只有白）；core 0.85 → (255,229,180) | `:388-410` |
| 光晕 | LB_ADD，`GLOW_ALPHA_MAX=95`（留余量给叠加，三灯才 clip），画在 core 之前（physically bloom 在玻璃与观者之间） | `:389`、`:386` |
| 光晕聚集 | 半径 120px 内视为一簇互相压暗 | `:472` |
| 74 sprite | 26+6+42，`kLamps` 顺序=wire，加灯同步 LampCatalog.java 四数组 | `IMPLEMENTATION.md:818` |
| 闪电 | 27 区域各有 window/earliest/pulse/cluster 与亮度下限 | `weatherfx.cpp:900+` |

## 二、现实化设计（新增，参考文档未定义）

### 2.1 强度模型：每类天气 3 档，用"多维度联动"而非只改一个数

现实感来自**多个量同时变**：粒子密度、速度、大小、alpha、风偏、云量、雾/致盲、地面堆积速率、音效强度。定义 `Intensity ∈ {0:轻, 1:中, 2:强}`，作用在客户端 `Config` 之外的纯表现层 `WeatherIntensity`（不动 0x373D 与 12 字段）。

| 天气 | 轻 (0) | 中 (1) | 强 (2) | 现实例子 |
|---|---|---|---|---|
| 雨 | 少量细丝、慢、无积水、云薄 | 稳定雨丝 + 积水生长 | 密集斜雨 + 快 + 雾 + 水坑饱和 + 闪电概率↑ | 毛毛雨/中雨/暴雨 |
| 雪 | 稀疏小片、慢飘、轻积 | 稳定片、中等斜 | 密片 + 高斜 + 快积 + 雾 | 小雪/中雪/大雪(暴雪沿用独立 sky) |
| 落叶 | 几片、慢 | 稳定 | 密、风偏大 | 秋风 |
| 樱花 | 稀疏 | 稳定 | 密、飘旋快 | 樱花吹雪 |
| 沙暴 | 扬尘少 | 扬尘 | 蔽日 + 高偏 | 扬沙/沙尘暴 |
| 晴/阴 | 无强度维度 | | | |

**实现映射（逐维）**：在 `WeatherFx::draw_particles` 中把 `count/speed/scale/alpha/drift` 定义为 `f(Intensity)`，而不是现在的单值；强度默认取**服务端 sky 的自然档**（rain 中、storm 强、snow 中、blizzard 强），web 若配置则覆盖。

### 2.2 昼夜切换：现实的不是"线性 2 小时"，而是分段曲线

参考是 05–07 / 17–19 线性。现实里黄昏/黎明有"缓入—陡降—缓出"，且天空色温随太阳角度变化。设计：
- 保留 `nightLevel()` 基准（协议/服务端共用，不动）
- 客户端**表现曲线**改成 smoothstep：`t^2(3-2t)`，让暮色中段过渡快、两头慢；
- 加**色温偏移**：黎明偏冷蓝紫→暖橙，黄昏偏暖橙→冷蓝（用现有 `TintRamp` 的 dusk/night 之间插值 + 一个 goldenFactor 已有）
- 日出/日落时刻各加 15 分钟"蓝色时刻"（blue hour），夜色未散但天空偏靛蓝

### 2.3 路灯补齐（全部按参考）

1. 光敏：`level_` 改成三态（OFF/DIM/ON）带 ON=0.35/OFF=0.28 迟滞；白天熄灭但 DIM 段 0.14–0.34 给微光。
2. 逐灯 warmup（1200ms）+ stagger（按灯序号 ×2600ms 取相位）+ cooldown 250ms；用服务端 `skyToken` 或本地灯序确定性错峰，保证多客户端一致。
3. 灯芯 LB_NORMAL 暖色核 `(255,229,180)`，光晕 LB_ADD 上限 95，聚集半径 120 互压。
4. 光池：保留现有地面光池，但叠在 `drawrectangle_add`，随 level 与光晕同相位。

### 2.4 配置闭环（不破坏协议）

- **协议零改动**：强度不进 0x373D（12 字段锁定）；用客户端 `Config` 新增 `intensity(Sky)` 数组 + `apply_intensities` 运行期接口。
- **web 可配**：管理端新增"强度"tab → 写入服务端一张小表 `weather_intensity_config` → 通过**新增独立 opcode（0x3741）**只在配置变更/进图时下发 9 个强度字节。服务端不重启生效。
- 若不许新增 opcode，则退化为"客户端本地默认 + GM 命令 `!weather intensity` 临时调"——待用户拍板。

## 三、影响面与风险

- 客户端：`WeatherFx.cpp`（粒子强度）、`WeatherLamps.cpp/.h`（光敏/错峰/暖核）、`WeatherConfig.h/.cpp`（强度表）、`WeatherState.cpp`（曲线，可选）、Stage（调用不变）。
- 服务端：新表 + Controller/Service + 新 opcode（需 opcode 注册避免撞号）。
- 管理端：新 tab + api。
- 风险：新 opcode 必须确认 0x3741 未被占；强度默认值必须与现网观感一致（不配时 stock）。

## 四、实现状态（2026-11-20 11:15）

用户拍板：①新增独立 opcode 0x3741 下发强度表 ②默认按参考 sky 语义映射。已一次成型实现：

| 层 | 提交 | 内容 |
|---|---|---|
| 服务端 | `80e7881e0b` | `WeatherIntensityConfigDO`/Mapper/`V1.9.11` migration、0x3741 opcode+`WeatherIntensityPackets`、`WeatherIntensityService`（@PostConstruct 装载/DB 保存/reset/广播/进图补发）、Controller `intensity/get\|save\|reset`、`MapleMap` 进图补发。`mvn -pl gms-server compile EXIT=0` |
| 管理端 | `d6bd8b5e7c` | `intensityTab.vue`（8 类天气三档单选）、api 三函数、index tab、zh-CN/en-US 19 键。`type:check` 仅剩 file/index.vue 既有 3 错 |
| 客户端 | `ee058099` | 0x3741 收包（12 字段零改）、强度默认表、粒子 5 维联动分级、雨丝长度/斜度随档、路灯 photo 迟滞/暮色/warmup/stagger/暖色灯芯 LB_NORMAL+光晕 LB_ADD。隔离编译 5 单元零 error + 整工程 BUILD=0 |

**服务端重启与真机验收（2026-11-20 11:49–12:12）**：
- 用户 11:49 重启服务端：首启 11:28 失败（`@AllArgsConstructor` 把 `int[]/int` 当 bean 注入，日志 "No qualifying bean of type 'int'"，提交 `889bb55b69` 修复为手写构造器）；11:49:31 `Started ServerApplication`，日志 `[WeatherIntensity] reload v=1 intensities=[1,1,1,1,2,2,1,1,2]`。
- 端口 8484/7575/8686 全监听、`/weather/v1/intensity/get` 返回 20000、Flyway 建表 `weather_intensity_config`（config id=1）。
- **0x3741 端到端探针铁证**（`_wi.log`，验收后已删）：`v=6 rows=...,rain=2 applied=2`（save 强档）→ `v=7 applied=2`（进图补发）→ `v=8 rain=1 applied=1`（reset 回默认）。即：web 保存 → 服务端广播 → 客户端收包 → `Config::apply_intensities` 落地 → `Config::intensity(RAIN)` 返回新档，**全链路成立**。
- 真 bug 修复：前端 `WeatherIntensityRecord`/tab 用下划线字段 → `intensity/save` 40000（Jackson 强类型 DO 需驼峰）；实测下划线=40000/驼峰=20000，改驼峰后修复（服务端提交 `226c419f9b`）。
- 三档雨夜截图 `V_{light,medium,heavy}_rain_night_{a,b}.png` 已留存；但我用的"帧差移动像素"口径（7556/7420/7989）被原生云层/角色动画淹没，**不能作为粒子强度递增的证据**——探针才是证据，视觉量化口径待改进（如固定相机、隐藏 NPC 的纯背景帧）。

**收口（2026-11-20 12:26–12:57）**：
- **web 管理同步**：用户确认用 Vite dev server（8787）。已 `npm run dev` 后台启动，`http://localhost:8787/` HTTP 200、`intensityTab.vue` 模块 200、天气页 5 处 intensity 引用 → 强度 tab 在 web 管理可达（HMR 自动含新代码）。
- **昼夜曲线 smoothstep**：服务端 `966d633eba` + 客户端 `03885519`，`nightLevel()`/`night_from_minute()` 逐字同步改为 `t²(3-2t)`（两端导数 0、中段陡），两端编译 EXIT=0。蓝调时刻（blue hour）未单独加，已由 smoothstep 中段陡峭+现有 goldenFactor 覆盖主要观感。
- **GM 命令** `!weather intensity 0|1|2`（服务端 `966d633eba`）：索引循环消费值参数，改当前天空强度档并写库+广播，与 web 保存等价；report usage 加提示。
- **三档视觉量化尝试（如实）**：`_intensity_credible.py` 固定雨/午夜、每档 3 帧取中值，因夜色蒙版把雨丝也压暗（亮像素阈值全 0）、白天场景石墙/云主导高亮（43683/43828/45970，噪声大），改用无阈值像素差：light→medium 10.61/8421、medium→heavy 12.94/10040、light→heavy 10.94/9838 —— 中→强差异最大（符合强档增强），但受原生云/角色动画干扰，**不构成严格单调证据**。功能正确性以 0x3741 探针（applied 随档 2→1）为准，不伪造视觉量化结论。

**未闭环（如实）**：
- 三档粒子**视觉**严格单调差异未拿到可信量化（原生云/角色动画干扰；探针证功能通）。
- 路灯错峰/暖色灯芯画面未单独验收。
- 蓝调时刻（独立 blue hour 色温层）未做——由 smoothstep + goldenFactor 近似。
**路灯闪烁修复（2026-11-20 14:14，用户反馈"灯光哪有一直闪烁的"）**：
- 行号级真因：`WeatherLamps.cpp:242-244` stagger 做成 `(tick+phase)%1300` 的 **1.3s 周期 45% 幅度循环**（主因）；`:219-224` + `WeatherLamps.h:46` breathe 5s 周期 15% 幅度（次因）。
- 参考实证：`lamps.cpp:430 LAMP_STAGGER_MS=2600` 是"点亮那一刻"的一次性错峰（"a street lighting up one lamp at a time reads as a town waking up"），点亮后应稳定；`lamps.cpp:212-224` 明确指出路灯自身 brightness ramp 是故障，参考用 `LampKeepFrame/LampStaticFrame` 停掉 art 自带呼吸。
- 修复（提交 `df2083ad`）：删 breathe 全部成员/相位/乘数；stagger 改为 `lit_since_ms - lamp_seq*2600` 的一次性 0→1（1200ms warmup），到位后恒 1。`grep` 确认源码零 `%1300/%5000/sin` 周期逻辑；隔离编译+整工程 BUILD=0；live SHA `0ec62b437f1c335c`。
- 月亮：源码早已无 `moon_breathe/draw_moon_disc` 残留；用户看到的明暗是地图原生云层飘过遮挡（5 点同步升降实测、参考 `weatherfx.cpp:1101-1103` 云本在月前），非天气层缺陷。

**服务端重启确认**：
- 服务端已于 13:43 重启，`966d633eba`（smoothstep + GM 命令）**已生效**：`Started ServerApplication in 5.615s`、`[WeatherIntensity] reload v=1`。web `intensity/get` 20000、reset 回基线 `[1,1,1,1,2,2,1,1,2]` 已确认。
- **GM `!weather intensity` 命令真机未验收**：自动化脚本的聊天输入框坐标在当前 HUD 布局下失效（点 y=601/y=684、Enter 均打不开输入行，打字触发了快捷键），命令未发出（web version 未变、服务端日志无 saved）。命令代码本身已编译 EXIT=0，且与已端到端验证的 web 保存共用同一 `WeatherIntensityService.save`；GUI 输入失败是测试方法学问题，非产品缺陷。待手工在游戏里敲一次确认。

## 五、验收（待服务端重启后）

- 真机同场景 `!weather rain/snow` 三档截图，量化粒子数/速度差（帧差法）
- 路灯错峰：夜里连续 3s 抓帧，统计亮灯数递增
- 昼夜曲线：黄昏 17:00→19:00 每 10 分钟截图，夜色 alpha 应呈缓-陡-缓
- 构建 BUILD=0、协议 12 字段不变、live SHA 实时核对
