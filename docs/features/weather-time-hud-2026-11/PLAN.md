# 实施计划 — 天气时段 / 夜色不压 UI / 游戏时间 HUD / 背包页码居中

日期：2026-11-23　分支：feature/dev_0.0.1
状态：用户已拍板全部 4 项，按一次性 Plan 实施。

## 决策（用户拍板）
1. 服务端 = BeiDou-Server_s9，已有 weather DB 配置 → W2 只核对不改。
2. 时间 HUD = 游戏内时间（State::minute_of_day），无 0x373D 时隐藏。
3. 夜色时段：15:00→17:00 渐暗，04:00→06:00 渐亮，smoothstep，最黑保持现值。
4. 夜色不压 UI：一次性全局修好 → 场景/UI 分段 flush。

## W0 — 夜色只压场景（GraphicsGL 分段 flush）
- `GraphicsGL.h`：新增 `void flush_scene();`（只绘制并清空 quads/quads_mul/quads_add/glow_quads，不含 fade 黑幕）。
- `GraphicsGL.cpp`：把 `flush()` 的四个批次段抽成 `draw_batches()` 私有 helper；`flush_scene()` = `draw_batches()`；`flush()` = 黑幕 quad → `draw_batches()` → pop。
- `Gameplay/Stage.cpp:279` 之后调用 `GraphicsGL::get().flush_scene()`；清除该处批次残留，UI 在 `MapleStory.cpp:236` 之后绘制，不再被乘色。
- 注意：`clearscene()`(:2231) 只清 `quads`（无锁时），`quads_mul/add/glow` 由 flush_scene 清；两套批次互不干扰。

## W1 — 背包页码居中
- `IO/UITypes/UIItemInventory.cpp:350` 后调用 `page_input.set_text_shift(Point<int16_t>(0, y_for_ink_top 补偿))`，用 `page_input_frame` 同款 A11M 字模度量；数字墨迹顶落在 (24-ink_h)/2 内。

## W3 — 游戏时间 HUD
- 在 `UIBuffList` 内新增 `Text timeclock`（A11M，黄字黑描边，复用 BuffIcon 描边逻辑）；
- `draw()` 在 buff 图标行下方 y≈(layout_margin.y()+32+8) 绘制；
- 数据源 `weather::State::get().minute_of_day()`，无 `has_state()` 不画；格式 hh:mm（两位补零）。
- 右键命中逻辑不受影响（时间文本非交互）。

## W4 — 夜色时段三端同步
- `Weather/WeatherFx.cpp:41-44`：DUSK_START=15*60, DUSK_END=17*60, DAWN_START=4*60, DAWN_END=6*60。
- `Weather/WeatherBackLayers.cpp:62-63`：同上。
- `E:/project/BeiDou-Server_s9/.../WeatherService.java:290-293`：同上；`nightLevel()` 纯函数体不改（:267-280）。
- golden hour 色温仍按新时段自动生效（`golden_factor(minute, DAWN_START, DAWN_END)` 用同一常量）。
- 服务端改动需重启才生效（文档中标注）。

## 验证
- 构建：MSBuild v142 / Release x64 / m:1（禁并发，禁中途 kill mspdbsrv）。
- 产物：SHA 记录；部署 live 前核对当前 live SHA，部署后实时复核防回滚。
- 真机：夜间开背包/弹窗截图（本端客户区 PrintWindow），UI 亮度=白天，场景仍暗；15:00/16:00/17:00 三段差分确认渐变；右上 hh:mm 与明暗同步。
- 服务端：JUnit/构建 + 重启 + status API 核对 night 时段。
- 文档：`docs/features/weather-time-hud-2026-11-22/README.md` + evidence/。

## 诚实边界
- Hindsight initiative 记录本轮 401（无 API token）阻塞 → 计划落本文件。
- 真机截图必须本端；未真机验收的部分如实标注。
