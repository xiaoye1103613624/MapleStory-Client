# A 档天气背景 + 闪电 + 夜色范围调整（2026-11-23）

> 分支：`feature/dev_0.0.1`
> 用户本轮合并需求：①山树 back/2+ 也压暗 ②A档星空/云/彩虹/沙暴 ③闪电 ④删玩家/NPC光晕、保留路灯+月亮 ⑤光源归灯泡、路灯上移 ⑥光晕强度随时间淡→亮→暗。
> 素材：全部 live 现成，orange-wz MCP 节点级实测（128B key/TSPHKw==/exclusive:false/逐会话 unload）。

## 1. 改动清单（行号级）

| # | 文件 | 改动 |
|---|---|---|
| 1 | `Gameplay/Stage.cpp:244-251` | 地图背景 back/0-1 与 back/2+ **两段都纳入 tint**（此前只有 back/0-1）；注入夜空/月/云/星/彩虹仍不压。tiles/objs/NPC/角色仍不压 |
| 2 | `Weather/WeatherBackLayers.cpp` | `Def` 加 `cover` 字段；`kSkyDefs` 按 bundle weatherfx.cpp:644-655 补齐：星空 ani/0,1(K_NIGHT)、彩虹 WeatherRainbow back/0(K_RAINBOW)、云 nightDesert back/8×3(K_CLOUD, rx=-2/-3/-4 滚动)、沙暴 back/19×2行(K_DUST)；`build()` 用 `d.cover` 取代 `!front`（否则 800px 云被拉伸变形） |
| 3 | `Weather/WeatherFx.cpp/.h` | `draw_lights()` 删玩家光环，只留月光；强度 ∝ night_level 淡→亮→暗。新增 `thunder_frames()`（Direction3.img/effect/skill/thunder/0/0..14，delay=70ms，effect_img 同机制）；`draw_lightning()` 叠加 thunder 逐帧（放大2倍画天空中上部）+ 保留白蓝全屏脉冲（bundle 夜色释放） |
| 4 | `Weather/WeatherLamps.cpp/.h` | 新增 `night_` 成员（update 存 night_level，clear 清零）与 `kLampLiftY=-20`（路灯整体上移）；光晕强度 `glow_a = lit*stagger*night_` 随时间淡→亮→暗；光源在 emitter（灯泡）位置不变 |

## 2. 素材实测（orange-wz，证据 tools/_weather_probe*_out.json / _thunder_detail_out.json）
- 云 `nightDesert back/8` CANVAS 800×183 ✅；`clouds.img back/4..10` ✅；`WeatherFog back/0..3` ✅
- 星 `nightDesert ani/0`(8帧 381×312)、`ani/1`(9帧 368×278) → 本端取首帧，无 twinkle
- 彩虹 `WeatherRainbow back/0` CANVAS 900×340 ✅
- 沙暴 `nightDesert back/19` CANVAS 800×350 ✅
- 闪电 `Direction3.img/effect/skill/thunder/0/0..14` 15帧 CANVAS（delay=70，技能特效借用）
- 太阳：live 无天气太阳（地图背景自带，未做）
- 高版本客户端素材不如 live 全，未搬；网络资源仅线索，禁止商用

## 3. 构建与部署（按铁律）
- 脚本 `tools/_atier_build.bat`，隔离 OutDir `build-atier/`+全新 IntDir，全量 Rebuild。
- `MSBUILD_EXIT=0`、error 0；产物 5516288 B，SHA `2d980cb20e02d8c29d16c0ad785d48cd90ef55cce43bb8ec2de7cbbe69b234d2`。
- 首次覆盖被运行中的客户端（PID 35052）锁定 Permission denied → 用户自行关闭客户端后重试成功。
- 备份 `MapleStory.exe.bak-before-atier-20261123`；覆盖后 T0/T10 SHA 一致、与产物差异 0 字节（无回滚）。

## 4. 真机验收清单（未验收，红线如实声明）
- [ ] 夜里天空 back/0-1 与山树 back/2+ 都变暗；tiles/objs/NPC/角色保持原亮度。
- [ ] 云在天空滚动（近快远慢），月亮在云后不被完全遮；星在夜空闪烁位（首帧静态）。
- [ ] 雨后出现彩虹；沙暴天气天空被沙尘替换。
- [ ] 暴风雨出现 thunder 贴图闪电 + 夜色释放白蓝脉冲。
- [ ] 玩家/NPC 无光晕；月亮有柔和外晕；路灯光源在灯泡上、整体上移，强度随夜色淡→亮→暗。
- [ ] 白天全部天气素材不出现；换图/商城进出无闪白天、路灯不重新暖灯。
- [ ] 帧率无明显下降；UI 不被压暗。
