# 天气系统完整版 · 真机验收清单（2026-11-19）

> 用途：用户在本端（`E:\MXD\BeiDou-Client-xy\MapleStory.exe`，SHA `d17df6db6452fa71`）逐项核对。
> **截图必须本端**——不得用参考端/历史插件端截图（证据错配 = 事实造假）。
> 运行后进户外图（射手村 100000000 / 埃利尼亚 101000000 / 佩里昂 102000000 均可，路灯点位见下）。

## 0. 前置确认（1 分钟）
| 项 | 期望 | 不符合就停 |
| --- | --- | --- |
| exe SHA | 实时 `sha256sum E:/MXD/BeiDou-Client-xy/MapleStory.exe` = `d17df6db6452fa71` 前缀 | 被登录器回滚则需重新部署，旧结论失效 |
| 0512.img SHA | `c621e574906af3a0` 前缀 | 被回滚则 5 个天气粒子节点丢失 |
| 天气 img | `Data/Map/Back/{MapleMoonSky,WeatherFog,WeatherRainbow,WeatherSnow}.img`、`Map/Obj/MapleMoonLamp*.img`、`Effect/Weather*.img` 均在 | 缺则对应层不显示 |
| 服务端 | 已启动且跑过 V1.9.9 + V1.9.9 timing migration；网页 `/game/weather` 可打开 | 否则无 0x373D 包，客户端渲染 stock |

## 1. 时钟与频率（对应问题①"切换太快"）
- 默认 `day_length_minutes=240`（现实 4 小时一整天）、`roll_interval_minutes=15`、`broadcast_interval_seconds=60`（`WeatherService.java:153/424`、`WeatherTask.java:23`）。
- 进游戏观察 5 分钟：天色应缓慢变化，**不应**出现"几分钟内昼夜来回跳"。
- 打开 `/game/weather` → 频率 tab：把 day_length 改成 1440（24h）保存 → 客户端下个广播包（≤60s）后昼夜变化速度应明显变慢（day length 与 roll 热更即时；**broadcast 周期改后需重启服务端**才生效，这是设计，接口已注明）。
- 复位：改回 240。

## 2. 昼夜 tint + 灰白蒙版（对应最初的 bug）
- 白天（游戏内 7:00–17:00）：`WeatherFx.cpp:124` night≤0.03 直接 return，**画面不应有任何全屏蒙版**；若仍灰白发白 → 看垫片日志（下节），把 `night`/`tint` 值回传。
- 黄昏/夜晚：应有深蓝夜色渐暗（上限 alpha 0.55，`WeatherFx.cpp:126`），**不是灰白**。
- 日志：`E:\MXD\BeiDou-Client-xy\weather_fx_trace.log`（垫片每 5s 或分钟变化写一行），回传最后 10 行。字段：`minute / night / tint=r,g,b / sky / flags / has`。

## 3. 背景层（批次1，对应问题②"没有粒子和背景"）
- 户外图夜晚：应见**月 + 星**（nightDesert back/3 + ani）、**夜空渐变**（MapleMoonSky）；天亮后渐隐。
- 雨天（sky=RAIN 或 STORM）：应见**雨丝**在前景（ancientForest back/28-31，bundle 同名 stock 资源）。
- 雪天（sky=SNOW）：应见**两层雪**（远在地图后、近在玩家前）。
- 雾（sky=BLIZZARD/STORM）：应有横向漂移雾带（WeatherFog 4 层）。
- 沙暴（sky=SANDSTORM）：整片沙尘 backdrop（nightDesert back/19 两行相接）。
- 彩虹：雨停后 `rainbowSecsLeft>0` 期间渐显（WeatherRainbow）。
- 层 fade 用 `weather_profiles.inc` 的 fade_step（约 2 秒到位）。

## 4. 闪电（批次2）
- 仅 STORM 且 rain≥0.70 时：`WeatherFx.cpp` lightning 用 `skyToken + skyElapsedMs` 同步脉冲，全屏白蓝闪（上限 0.55）。观察 1–2 分钟应见偶发闪光。
- 若无 STORM 天气：用 web Override tab 强制天空=storm（profile id 4）观察。

## 5. 地面层（批次3）
- 雪/叶/花天：地面积雪/落叶/花瓣（Effect/WeatherAccum.img），约 90 秒长满、25 秒融化。
- 雨天：地面水坑（WeatherPuddle，三级宽度）+ 雨滴溅射（WeatherSplash）。
- **已知近似**：地面 y 用玩家脚下位置近似（`WeatherGround.cpp:44-51` TODO），台阶/坡道会浮空或下陷——这是预期内的真机校准点，看到局部偏移不算 bug，回传具体地图+坐标即可。
- 打滑（weathermove）本版**未接入**（只留接口桩，避免破坏移动手感）——不期望有打滑效果。

## 6. 路灯（批次4）
- 仅在 5 张城镇图有手工点位（bundle lamps.cpp:581-661）：射手村 100000000（7 处 lamp6）、埃利尼亚 101000000（12 处 lamp22）、佩里昂 102000000（7 处 lamp12）、魔法密林、废弃都市 103000000（5 处 lamp8）、Lith Harbour 104000000（9 处 lamp0）。
- 夜晚：这些点位应亮起**暖色光晕**（`WeatherLamps.cpp` 光晕圆，随 night_level）。白天不亮。
- **已知近似**：本版画光晕圆近似，未逐灯画 sprite（74 种 emitter 精确对齐留 TODO）。看到"灯的位置发光但看不到灯柱 sprite"是本版预期。

## 7. 环境音（批次3，如实标注）
- **本版不发声**：原生 `Audio::Sound` 只有一次性 play、无 loop API，bundle 的 rain/wind 持续 loop 无法等价实现（接口桩，已如实标注）。不期望听到雨/风环境音。

## 8. Web 管理（批次5）
- 打开 `/game/weather`（admin 账号），5 个 tab：区域 / 季节权重 / 地图归属 / Override / 频率。
- Override tab：设置天空=night、时间=night、清除，观察客户端 ≤60s 内变化；剩余时长倒计时应走。
- 改任一配置 → 客户端下个广播包生效（broadcast 周期本身除外，需重启）。

## 反馈格式（一次给全，不用来回补）
```
1) exe SHA：____
2) 时钟频率：正常/异常（异常描述）
3) 灰白蒙版：有/无；有→垫日志最后10行：____
4) 背景层（月/星/雨/雪/雾/沙暴/彩虹）：各自 有/无
5) 闪电：有/无（在 storm 下观察）
6) 地面（积雪/水坑/溅射）：各自 有/无
7) 路灯：城镇夜图 亮/不亮
8) Web 5 tab：各自 可用/异常
9) 截图（本端）：<路径>
```
