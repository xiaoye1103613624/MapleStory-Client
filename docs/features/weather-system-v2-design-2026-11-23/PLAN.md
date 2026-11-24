# 天气系统 V2 —— 最终方案与实施 Plan（2026-11-23 定稿）

> 前置：README.md（架构设计）。本文档是**用户 6 项决策后的最终定稿 + 可执行 Plan**。
> 状态：**待用户确认 Plan 后进入实现**。本轮未改任何代码/DB/Web。

---

## 一、用户 6 项决策（已拍板）

| # | 决策 | 落地 |
|---|---|---|
| 1 天体 | 按季节切换 | 春=魔法光球(base3/34-36)、夏=太阳(sunset/25)、秋=残月+暖光、冬=残月(night/29)；Web 可覆盖 |
| 2 粒度 | 参数化 | 绝大多数现象=现有 sky+intensity+flags+element，不扩 sky id 到 30+ |
| 3 节气 | 公历近似 + **时间显示 MM月DD日 HH:mm** | 服务端算节气/日期；0x3742 下发 gameDate；HUD 显示 |
| 4 范围 | **全做**（含极光/龙卷，缺素材的继续找） | 见素材清单；个别真缺的用程序化/参数化兜底并标注 |
| 5 Web | 有更好设计就重写 | 采用"规则矩阵+实时预览"重构（见 §4），保留旧数据迁移 |
| 6 协议 | 接受 0x3742 新包 | 双端同步 |

---

## 二、素材实测清单（orange-wz + 解码目视，证据 tools/_celestial_* / _wxassets_*）

### 2.1 天体（用户指定，全部可独立 canvas）
- 残月 `blackheavensky_night.img/back/29` CANVAS 219×216 origin(109,108) ARGB4444 — 蓝白残月+光晕 ✓
- 太阳 `blackheavensky_sunset.img/back/25` CANVAS 332×332 origin(166,166) ARGB8888 — 金黄圆盘+亮心 ✓
- 魔法月×3 `blackmagician_base3.img/back/34/35/36` CANVAS 860×532/748×472/424×320 — 蓝色魔法光球+星座（非写实月，作"魔法月相"）
- 暖光柱 `blackmagician_base2.img/back/ani/0/0` CANVAS 506×608 origin(226,272) 带 delay — 暖黄斜光束 ✓

### 2.2 用户新增指定
- **极光候选** `bm3_1.img`：back/4(650×200)、/6(720×234)、/11(535×214)、/12(744×182) 是**淡青色波纹光带**（可作极光光幕，需染色放大）；其余 back 多为黑魔法师深色建筑机械片（非极光）。
- **龙卷/云** `bm1_3.img`：back/3-8 是**黑色横向云团**；ani/5(292×1376)、ani/6(1392×264) 等是**旋转黑色气柱/波浪**（作龙卷/尘卷/烟幕主体，需染色）。canvas 总数 223。

### 2.3 全现象候选（子代理已采，effect_stats.json）
- 雾/轻雾/烟幕：`foresttraining.img back/0,1,3,6,9,12,14,16,19,22`（青/绿云雾山景）、`WeatherFog.img back/0-3`（1024×460 等）✓
- 彩虹：`WeatherRainbow.img back/0` 900×340 ✓
- 雪/暴雪/花瓣/叶/沙粒：`WeatherParticles.img` 的 snow(5×9)/blizz/leaf/blossom/sandGrit ✓
- 积雪/水坑/溅射：`WeatherAccum.img`/`WeatherPuddle.img`/`WeatherSplash.img` ✓
- 雷电：`Direction3.img/effect/skill/thunder/0/0..14` 15帧 delay=70（技能特效借用）✓
- **仍缺真素材**：冰雹/霰/冰粒/米雪独立粒子（用 snow 染色+尺寸参数兜底）、霜/雾凇/雨凇地表（用 WeatherAccum 白色调兜底）、极光专用绿紫光幕（bm3_1 青光带染色兜底）、冰针（snow 细针参数）、龙卷专用（bm1_3 气柱染色兜底）。**全部可交付，缺的部分用参数化兜底并在 Web 标注"素材近似"**。

---

## 三、协议与数据（最终）

### 3.1 0x373D（保持 28B，启用 flags 空闲 5bit）
```
flags bit0=snap bit1=frozen bit2=baresky
      bit3-4 = timeBand(0凌晨/1早/2中/3晚夜)
      bit5   = seasonCold(0暖季/1冷季)
      bit6-7 = 预留
```
客户端 `WeatherState` 增 `time_band()`/`season_cold()` 访问器，`CommonHandlers.cpp:259-309` 解析 flags 新位。

### 3.2 0x3742 WEATHER_ELEMENT（新包）
```
short version | short gameDate(月*100+日, 如 1123=11月23日)
byte[N] elements (雾/风/霜/冰/极光/龙卷... 强度 0..2)
```
服务端 `WeatherElementPackets` 打包；客户端 `WeatherElementHandler` → `WeatherConfig::apply_elements`。
HUD 时间：`UIBuffList.cpp:284` 当前 `%02d:%02d`，改为读 gameDate + minute → `"%02d月%02d日 %02d:%02d"`。

### 3.3 DB 新增/扩展
- 新表 `weather_climate_rule`：`season(4) × solar_term(0-24) × time_band(0-3) × region → allowed_csv, forbidden_csv`
- 新表 `weather_element_config`：element 类型 × 强度默认
- 扩展 `weather_override`：加 `game_date_override`（可选）
- 现有 7 表保留；Q1 keep 缺陷（`WeatherConfigManager.applyOverride:191`）、Q2 常量错位（`WeatherService.java:284/286`）在本计划 P0 顺手修。

---

## 四、Web 重构（决策5：有更好设计→重写）

新设计：**"气候日历" 单页 + 3 个抽屉**
- 主视图：24 节气 × 4 时段 的年度矩阵热力图（格内=当前概率最高的天气，点击编辑）。
- 抽屉A 天体配置：早/中/晚/夜 × 四季 → 用哪个天体节点；实时预览小窗。
- 抽屉B 现象强度：全现象（降水/凝结/视障/雷电/其他）强度滑条 + "当前生效"指示。
- 抽屉C 即时控制：保留原 quickTab 的双轴切换（修 keep 语义）+ 自定义日期/时刻输入（填 gameDate+minute）。
- 冲突矩阵可视化：保存时校验"夏天下雪"等，红色拦截。
- 旧 7 tab 数据通过迁移脚本导入新表，不丢配置。

---

## 五、分阶段执行计划（一次性交付，分阶段验证）

| 阶段 | 内容 | 涉及文件（预估） | 验证门禁 |
|---|---|---|---|
| **P0 协议地基** | flags 新位 + 0x3742 包 + Q1/Q2 修复 | 客户端 `WeatherTypes.h`/`WeatherState.*`/`CommonHandlers.cpp`/`PacketSwitch.cpp`；服务端 `WeatherPackets.java`/`WeatherElementPackets.java`(新)/`WeatherService.java`/`WeatherConfigManager.java`/`SendOpcode.java` | 双端编译0错；抓包对拍 0x373D/0x3742 字节；Q1 keep 不再清时段、Q2 时段亮度各不同 |
| **P1 规则层** | 新表+节气算法+时段派生+防冲突+pick 改造 | 服务端 DAO/DO/migration/`WeatherService.pick`/新 `SolarTerm.java`/`WeatherClimateService` | DB 迁移可回滚；单测覆盖"夏季禁雪"等冲突；季节切换模拟通过 |
| **P2 客户端渲染** | 天体层(季节切换)+现象参数化(降水三档/雾/风/霜/极光/龙卷)+地面 element+HUD 日期 | `WeatherBackLayers.*`/`WeatherFx.*`/`WeatherLamps.*`/`WeatherGround.*`/`UIBuffList.cpp`/`WeatherConfig.*`/新 `WeatherCelestial.h` | 全量构建；真机逐现象截图（30+，用户验收前不称完成）；帧率无明显下降 |
| **P3 Web 重构** | 气候日历单页+3抽屉+冲突矩阵+数据迁移 | `gms-ui/src/views/game/weather/*`(重写)/`api/weather.*`(新接口)/i18n | Web 构建；端到端：Web 改→服务端 reload→客户端抓包→真机生效 |
| **P4 素材收口** | 缺素材的现象用兜底方案落地 + 清单标注 | 资源清单文档（无需改 WZ，复用现有节点染色/参数） | 真机目视确认兜底可接受 |

**总验证门禁**：每阶段 单测+构建；P0/P3 抓包对拍；P2/P4 真机逐项验收（用户确认前不称完成）；最终 live 产物 SHA 实时核对（防登录器回滚）。

---

## 六、风险与回滚
- **双端同步风险**：P0/P3 改协议必须同批部署服务端+客户端，否则旧客户端收到 0x3742 会 UnknownOpcode → 需客户端 `PacketSwitch` 对未注册包静默忽略（实现时加）。
- **规则层复杂度**：24节气×4时段×27区域矩阵大，用 CSV+稀疏默认，避免全量枚举。
- **素材近似**：极光/龙卷/冰雹等为染色兜底，观感非原生，Web 标注"素材近似"，用户可后续替换真素材。
- **回滚**：DB 加 migration 可 down；客户端/服务端保留 `.bak-before-v2`；Web 旧代码 tag 保留。

---

## 七、待用户最终确认（Plan 确认后才动手）
1. 上表 P0-P4 的分阶段与顺序是否认可？
2. Web 重写采用"气候日历单页+3抽屉"设计是否认可（vs 保留7 tab 增量）？
3. 缺素材现象用"参数化兜底+Web标注近似"是否接受（vs 等真素材）？
4. HUD 日期来源用 0x3742 下发的 gameDate（与游戏季节同步）是否认可（vs 客户端本地系统日期）？

确认后我即从 P0 开始，一次性按 Plan 推进到 P4，中途每阶段交付验证记录。
