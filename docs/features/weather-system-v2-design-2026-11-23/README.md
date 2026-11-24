# 天气系统 V2 完整设计方案（季节/节气/时段/气象现象 + Web）— 2026-11-23

> 状态：**方案设计（未实现）**，等用户拍板决策点。
> 依据：orange-wz 节点级实测（`tools/_celestial_*.json`）、s9 服务端/Web 全链路只读盘点、客户端 `Weather/` 架构盘点。
> 合规：仅个人学习/技术研究、单机离线，严禁商用；素材为版权资源，遵守第三方许可。

---

## 0. 用户诉求（原样归纳）
1. 拓展天气系统：24 节气、四季、早中晚凌晨、全套气象现象切换；天气与季节呼应（禁止夏天下雪）。
2. 气象现象清单：降水（雨/毛毛雨/阵雨/雪/阵雪/雨夹雪/冰雹/霰/冰粒/米雪）、地面凝结（露/霜/雾凇/雨凇）、视程障碍（雾/轻雾/霾/扬沙/浮尘/沙尘暴/吹雪/雪暴/烟幕）、雷电（雷暴/闪电/极光）、其他（大风/飑/龙卷/尘卷风/冰针/积雪/结冰）。
3. 给定素材节点：残月、3个月亮、太阳、暖色光；截图文件树中还有云朵/月亮/闪电/光线/太阳等。
4. Web 管理，允许重构。

---

## 1. 素材实测结论（orange-wz，live 10012，逐节点解码目视）

| 用户给的 | 节点 | 实测 | 画面（真 PNG 解码） | 可用性 |
|---|---|---|---|---|
| 残月 | `blackheavensky_night.img/back/29` | CANVAS 219×216 origin(109,108) ARGB4444 | 蓝白残月+光晕 ✓ | ✅ 直接画，可作"残月/月末"天体 |
| 月亮×3 | `blackmagician_base3.img/back/34/35/36` | CANVAS 860×532 / 748×472 / 424×320 | **蓝色魔法光球/星体+猫星座**（非写实月牙） | ⚠️ 可作"魔法月相"变体，与"月亮"语义有出入 |
| 太阳 | `blackheavensky_sunset.img/back/25` | CANVAS 332×332 origin(166,166) ARGB8888 | 金黄发光圆盘+亮心 ✓ | ✅ 直接画，作"夕阳/太阳" |
| 暖光 | `blackmagician_base2.img/back/ani/0/0` | CANVAS 506×608 origin(226,272) ARGB4444 带 delay | 暖黄斜向光束/光柱 ✓（注意是 Back 目录那份，Obj 同名文件无此节点） | ✅ 作"黄昏光线/神光" |

节点命名全为数字索引（back/29、ani/0/0），无 moon/sun 语义名——需在代码里用"节点清单表"按语义注册。证据：`tools/_celestial_final.json`、`tools/_celestial_png/*.png`（6 张解码图+manifest）。

**素材缺口**（本次没验到、需补找）：极光、冰雹/霰/冰粒/米雪的独立粒子图、龙卷/尘卷风、霜/雾凇/雨凇的地表图。live 现有 `WeatherParticles.img`（snowFall/blizzardSnow/leafMix/blossomMix/sandGrit）+ `WeatherAccum/Puddle/Splash`（积雪/水坑/溅射）可复用一部分。

---

## 2. 现状架构与硬约束（行号级）

### 2.1 客户端（`E:\project\MapleStory-Client`）
- `Weather/WeatherTypes.h:28-42`：`Sky : uint8_t` 9 值（0..8，`COUNT=9`），wire 单字节。
- `Weather/WeatherTypes.h:46-73`：`PaletteId` 27 区域调色板。
- `Weather/WeatherTypes.h:78`：`Flags` 只用 3 bit（snap/frozen/baresky），**5 bit 空闲**。
- `Weather/WeatherFx.cpp:68-81`：`night_from_minute()` smoothstep 曲线（DAWN 4-6 / DUSK 15-17）。
- `Weather/WeatherBackLayers.cpp:32-48`：`kSkyDefs` 已扩到 10 层（渐变/月/星×2/彩虹/云×3/沙暴×2），`K_*` 驱动 K_NIGHT/K_DUST/K_RAINBOW/K_CLOUD/K_SNOW/K_RAIN/K_FOG。
- `Weather/WeatherFx.cpp:373`：`Config::intensity(sky)` 三档粒子密度（0轻/1中/2强，来自 0x3741）。
- 渲染批次：`GraphicsGL` quads / quads_mul / quads_add / glow_quads 四路；`push_tint` 分层压暗（`Stage.cpp:244-251`）。

### 2.2 服务端（`E:\project\BeiDou-Server_s9\gms-server`）
- `WeatherProfile.java:40-64`：9 天空 id（byte）；`:97-103` byId 越界回落 CLEAR。
- `WeatherRegion.java:57-283`：27 区域；每区 `double[9] bias`；`:361-376` 静态断言 bias 宽==9。
- `WeatherService.java:829-848`：`Season` 4 值，按公历月硬编码；`:861-867` `int[4][9] SEASON_WEIGHTS`；`:879-894` 静态断言。
- 7 张表：`weather_region / weather_season_weight / weather_map_region / weather_override / weather_timing_config / weather_intensity_config / weather_element_rule`（DDL `V1.9.9/9.10/9.11/V1.11.19/V1.11.21`）。无 `weather_config` 总表。
- `weather_element_rule`：`sky_id 0..8 + 9=夜晚伪天空`（`WeatherElementRuleDO.java:9,16`），key=(sky<<16)|(element<<8)|scope。
- 协议：`WeatherPackets.java:59-87` 0x373D 定长 28B（minute/sky/flags/tint/palette/...）；`WeatherIntensityPackets.java:25-33` 0x3741 定长 14B（byte[9] 强度）。
- Web：`gms-ui/src/views/game/weather/` 7 tab；`constants.ts:13-23` 9 profile、`:56-61` 4 season、`:64-69` 4 QUICK_TIMES；`api/weather.ts:126-295`。

### 2.3 关键硬约束
1. **wire 锁死**：0x373D 定长 28B、位置敏感（tint 在 palette 前、legacy skyElapsedSec 占位）；0x3741 硬编码 byte[9]（`WeatherIntensityPackets.java:28`）。
2. **9 值深度耦合**：天空类型被 bias 宽度、SEASON_WEIGHTS 行宽、intensity switch、0x3741 长度、前端常量 5 处硬编码——每加一种天气要成组改。
3. **静态断言护栏**：扩枚举必须成组改，否则类初始化抛异常（安全失败）。
4. **双端曲线同步**：`nightLevel()` 与客户端 `night_from_minute()` 必须逐字一致。
5. **flags 有 5 bit 空闲**：不改包长即可承载 时段档位（早/中/晚/凌晨 2bit）、节气提示（预留）、季节（2bit）——需客户端解码配合。

---

## 3. V2 架构设计（核心：把"天气类型"与"表现参数"解耦）

### 3.1 三层模型
```
[时间层]  真实/模拟时钟 → 季节(4) × 节气(24) × 时段(早/中/晚/凌晨/夜)
[规则层]  DB 规则表：季节×时段×区域 → 天气类型概率 + 强度 + 温度/湿度参数
[表现层]  客户端：sky 类型 + intensity 档 + flags(时段/季节) → 渲染（天体/粒子/地面/光照）
```

### 3.2 气象现象 → 现有/新增映射（关键：多数现象=参数组合，不是新 sky id）

| 现象 | 实现方式 | 是否新增 sky id |
|---|---|---|
| 雨/阵雨/毛毛雨 | sky=RAIN + intensity(1/2/0) + 新"降水型"flags 位 | 否（复用 RAIN） |
| 雪/阵雪/米雪 | sky=SNOW + intensity + 粒子尺寸参数 | 否 |
| 雨夹雪/霰/冰粒 | sky=SNOW+RAIN 混合（新 combo 值或双粒子层） | 视方案 |
| 冰雹 | 新粒子组（复用 WeatherParticles.img 加 hail canvas） | 否（参数化） |
| 雾/轻雾 | sky=OVERCAST + 雾 alpha 档（WeatherFog back/0..3 已有） | 否 |
| 霾/烟幕/浮尘/扬沙 | sky=SANDSTORM + 强度 + 色调（复用 back/19 + tint） | 否 |
| 沙尘暴 | sky=SANDSTORM（已有） | 否 |
| 吹雪/雪暴 | sky=BLIZZARD + 大风 flags（粒子横向速度倍率） | 否 |
| 雷暴/闪电 | sky=STORM + 已有 lightning（thunder 帧+夜色释放） | 否 |
| 极光 | **新** sky 或 flag + 新粒子/背景层（素材需补） | 视方案 |
| 大风/飑/龙卷/尘卷风 | 风场参数（粒子 drift 倍率）+ 龙卷新素材（需补） | 否/视素材 |
| 露/霜/雾凇/雨凇/积雪/结冰 | `WeatherAccum` 地面层参数化（新 element 类型，复用 weather_element_rule） | 否 |

**核心决策**：优先用"现有 sky + intensity + flags 参数 + 地面 element"表达绝大多数现象，**避免 sky id 从 9 暴涨到 30+**（那样要改 5 处硬编码 + 两端表）。只有极光/龙卷等确实需要独立背景的才加 id。

### 3.3 季节呼应 + 防冲突（规则层）
- 新增 `WeatherClimateRuleDO`（新表）：字段 `season(4) × solar_term(24 或 0=不限) × time_band(早/中/晚/凌晨/任意) × region → allowed_profiles(CSV) + forbidden(CSV) + temp/humidity`。
- 服务端 `pick(region)`（`WeatherService.java:902-959`）改为：先按 season×solar_term×time_band 过滤 allowed，再按权重 roll。
- 防冲突硬规则（在规则层校验）：
  - SUMMER 及其相邻节气（立夏..大暑）禁止 SNOW/BLIZZARD/霰/冰粒/霜/雾凇/结冰（除非 override 强制）。
  - WINTER（立冬..大寒）禁止 BLOSSOM/LEAVES/雷暴高概率。
  - SPRING 偏 BLOSSOM/RAIN/薄雾；AUTUMN 偏 LEAVES/OVERCAST/霜。
  - 沙尘暴仅 SPRING 北方区域（PERION/MAGATIA 等 bias>0）。
- Web 提供"冲突矩阵"可视化 + 保存时校验拦截。

### 3.4 24 节气表示
- 服务端新增 `SolarTerm` 枚举（24 值）+ `currentSolarTerm()`：按公历日期近似（立春 2/4、雨水 2/19 … 大寒 1/20），误差 ≤1 天；或接农历库（需依赖）。
- wire **不改**：节气由服务端按日期算，客户端只收 sky/intensity/flags；仅在 Web 状态页显示当前节气。
- 规则表 `solar_term` 字段 0=不限 / 1..24。

### 3.5 时段表示（早/中/晚/凌晨）
- 服务端按 `minuteOfDay` 派生 time_band（凌晨 0-5、早 5-9、中 9-17、晚 17-20、夜 20-24）。
- wire **用 flags 空闲位**：`FLAG_BAND_MASK=0x18`（2bit，4 档：凌晨/早/中/晚夜），客户端 flags 解析（`WeatherState.cpp`）读它，供渲染选天体（太阳/夕阳/月亮）。
- 时段强度仍由 minuteOfDay 的 smoothstep 驱动（现有曲线）。

### 3.6 天体渲染（用户素材落地）
- 客户端 `BackLayers` 加"天体层"：`K_CELESTIAL` 驱动，按 band 选：
  - 白天（中）：`blackheavensky_sunset.back/25` 太阳（或自绘白天太阳）。
  - 黄昏（晚）：同太阳 + `blackmagician_base2.ani/0/0` 暖光柱。
  - 夜（夜/凌晨）：`blackheavensky_night.back/29` 残月 或 `blackmagician_base3.back/34/35/36` 魔法月（可配置）。
- 层级：天体在注入夜空层之后、云之前（bundle g_aNight 语义）。
- 节点清单表按语义注册（因节点名无语义）：新增 `WeatherCelestial.h` 常量表。

### 3.7 协议扩展（最小改动）
- 0x373D **保持 28B 不动**，仅启用 flags 空闲 5bit：
  - bit3-4：时段档（0凌晨/1早/2中/3晚夜）
  - bit5：季节提示（0暖季/1冷季，2bit 不够可只 1bit 冷热）
  - bit6-7：预留
- 0x3741 **保持 14B 不动**（byte[9] 强度不变）；新的现象强度（雾/风/地面）走**新 opcode** `0x3742 WEATHER_ELEMENT`（version + byte[N] element 强度），与 0x3741 分离，避免动 12 字段锁定包。
- 客户端：`PacketSwitch` 注册 0x3742 → `WeatherElementHandler` → `WeatherConfig::apply_elements`。

### 3.8 Web 重构（允许重构）
- 保留现有 7 tab 的数据 CRUD；新增 3 tab：
  - **气候规则 tab**：season×solar_term×time_band 矩阵，配 allowed/forbidden profile + 冲突矩阵可视化。
  - **现象强度 tab**：各气象现象（降水/雾/风/地面/雷电）强度滑条 → 0x3742。
  - **天体配置 tab**：早中晚夜各用哪个天体节点（残月/满月/太阳/暖光）+ 位置/缩放。
- quickTab 修 Q1/Q2（keep 语义 + TIME 常量入 ramp）。
- constants.ts 的 9 profile 保持（不扩 id），新增 `SOLAR_TERMS[24]`、`TIME_BANDS[4]`、`ELEMENTS[N]`。

---

## 4. 分阶段实施计划（一次性交付但分阶段验证）

| 阶段 | 内容 | 交付物 | 风险 |
|---|---|---|---|
| P0 协议与枚举 | flags 时段/季节位、0x3742 element 包（客户端+服务端） | 双端编译通过、抓包对拍 | 中（需两端同步） |
| P1 规则层 | 新 climate rule 表 + SolarTerm + time_band + 防冲突校验 + pick 改造 | DB 迁移 + 服务端单测 | 中 |
| P2 客户端渲染 | 天体层、现象参数化粒子、地面 element、intensity 扩展 | 客户端构建 + 真机逐现象验收 | 高（现象多） |
| P3 Web | 3 新 tab + 冲突矩阵 + quickTab 修复 | Web 构建 + 端到端 | 中 |
| P4 素材补齐 | 极光/冰雹/龙卷/霜等缺素材，orange-wz 入库 | 资源清单 | 视素材可得性 |

每阶段验证门禁：单测/构建/抓包对拍/真机截图（用户验收前不称完成）。

---

## 5. 决策点（需用户拍板，阻塞实现）

1. **天体画风**：月亮用 ①写实残月 `blackheavensky_night.back/29` / ②魔法光球 `blackmagician_base3.back/34-36` / ③两者按季节切换？太阳用 `blackheavensky_sunset.back/25`（金黄圆盘）可否？
2. **现象粒度**：用"现有 sky+参数"表达绝大多数（推荐，改动可控）vs 每种现象独立 sky id（30+，改动巨大）？
3. **节气来源**：公历日期近似（零依赖，误差≤1天）vs 接农历库（精确但加依赖）？
4. **气象全集范围**：本轮是否全做（30+ 现象，含需补素材的极光/冰雹/龙卷）vs 先做核心（雨/雪/雾/沙尘/雷暴/积雪+四季天体），缺素材现象预留接口？
5. **是否重构 Web**：增量加 3 tab（推荐，稳）vs 整体重写管理端？
6. **0x3742 新包**是否接受（新增 opcode 需服务端+客户端同步）？

---

## 6. 诚实边界
- 本方案基于源码与节点只读实测；**未实现任何代码**，未改任何文件。
- 素材画面描述来自 orange-wz 返回 PNG 解码目视（live 节点渲染，非参考端截图），节点结构 100% 来自 MCP 原文。
- "两端逐字同步"为源码注释声明的契约，实际 DLL/客户端一致性需实现时抓包核对。
- 所有版权素材仅个人学习使用，禁止商用。
