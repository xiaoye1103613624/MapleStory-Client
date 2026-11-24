# 天气时段 / 夜色不压 UI / 游戏时间 HUD / 背包页码居中 — 最终实现逻辑

日期：2026-11-23　分支：feature/dev_0.0.1
状态：客户端已构建（BUILD_EXITCODE=0）；**真机未验收**；**服务端未重启（W4 未生效）**。

## 决策（用户 2026-11-23 拍板）
1. 服务端 = BeiDou-Server_s9，已有 weather DB 配置 → W2 仅核对，不改。
2. 时间 HUD = 游戏内时间（weather::State::minute_of_day），无 0x373D 时隐藏。
3. 夜色时段：黄昏 15:00→17:00 渐暗，黎明 04:00→06:00 渐亮，smoothstep 曲线，最黑强度不变。
4. 夜色不压 UI：一次性全局修好（场景/UI 分段 flush），不逐窗打补丁。

## 一、W0 — 夜色只压场景（全局 bug 根治）
### 根因（行号证据）
- `MapleStory.cpp:230-237`：一帧顺序 begin → Stage::draw → UI::draw(1.0) → end。
- `Window.cpp:1009` 只 `clearscene()`；`Window.cpp:1014` 帧末唯一一次 `GraphicsGL::flush()`；`Window.cpp:1030` 才 SwapWindow。
- `Stage.cpp:270` `Fx::draw()` → `WeatherFx.cpp:208` `drawscreenmultiply()` → `GraphicsGL.cpp:1990` 把全屏 quad 塞进 `quads_mul`，**只入队不绘制**。
- 真正绘制在帧末 `flush()` 的 `GraphicsGL.cpp:2080-2112`，混合 `GL_DST_COLOR/GL_ZERO`（:2084）＝ `dst *= src.rgb`，作用于**当时 framebuffer 上所有像素**，此时 UI 已画完 → 所有窗口/文字/buff 一起被压暗。
- `Stage.cpp:266` 注释「只压场景不压 UI」与代码相悖 = 本次 bug 的文案证据。

### 修复
- `GraphicsGL.h`：新增 public `flush_scene()`；把帧批次绘制抽成 private `draw_batches()`；新增 `bool scene_flushed_`。
- `GraphicsGL.cpp:2024` `draw_batches()`：主 quad → 乘色 → 加色 → 光晕（原 flush 的四个批次段原样抽出，含各自 blend 切换与 clear）。
- `GraphicsGL.cpp:2191` `flush_scene()`：一帧唯一一次 `glClear` 移到这里 → `draw_batches()` → 清主 `quads` → `scene_flushed_=true`。
- `GraphicsGL.cpp:2205` `flush(opacity)`：无场景帧兜底 clear → fade 黑幕 quad → `draw_batches()` → pop → `scene_flushed_=false`。
- `Gameplay/Stage.cpp:279` 之后（draw 末尾）调用 `GraphicsGL::get().flush_scene()`；新增 `#include "../Graphics/GraphicsGL.h"`。
- 效果：夜色/灯光只作用于场景 framebuffer；`UI::draw` 之后在帧末 flush 提交，不再被乘色。角色/NPC 属于场景层，仍随场景压暗（符合原生逐层 tint 语义）；用户所关心的窗口/弹窗/扩展 UI/buff 保持原亮度。

## 二、W1 — 背包页码居中
- `IO/Components/Textfield.h:54` 已有 `set_text_shift()`；新增 `center_text_vertically(box_height)`：按字面墨迹高 `height()-ink_offset()` 居中，`y_for_ink_top()` 换算绘制点（Text.h:146-164）。
- `IO/UITypes/UIItemInventory.cpp:350` 后调用 `page_input.center_text_vertically(page_input_dim.y())`（框 24×16）。
- 不替换控件、不改翻页逻辑（`:1415` change_text 与 `set_text_shift` 正交，翻页后仍居中）。

## 三、W2 — 天气/时间全局配置（核对，不改）
- s9 已具备：`V1.9.9__create_weather_tables.sql:48-61` `weather_override`（`time_minute` 冻结分钟 0..1439，我已实时核对）、`V1.9.10__weather_timing_config.sql:14,24,27` `day_length_minutes`（默认240）。
- `WeatherConfigManager`（volatile 不可变快照 + reload 热更 + `WeatherService.setConfigProvider`）；gms-ui `views/game/weather/` 多标签页（override/timing/region/...）。
- 结论：无需新增/移植；用户通过现有管理页即可「设计什么天气、什么时间」。

## 四、W3 — 右上 buff 下方游戏时间 hh:mm
- `IO/UITypes/UIBuffList.h`：新增 `Text timeclock_label / timeclock_outline`（A11M，RIGHT 对齐，黄字黑描边）+ `int16_t timeclock_shown_minute=-1`。
- `IO/UITypes/UIBuffList.cpp`：include `../../Weather/WeatherState.h`；构造函数初始化两个 Text；
  - `update()`：取 `State::minute_of_day()`，`has_state()==false` 时清空文本（隐藏）；仅分钟变化时 `change_text("%02d:%02d")`，避免每帧重排。
  - `draw()`：在 buff 图标带下方 `position + (0, +7)` 画 8 方向黑描边 + 黄字（复用 BuffIcon:83-95 口径）；非交互，不影响 `is_in_range/rightclick`。
- 时间与夜色同源（都来自 `minute_of_day`），显示与画面明暗严格同步。

## 五、W4 — 夜色时段三端同步
| 端 | 文件:行 | 新值 |
| --- | --- | --- |
| 客户端曲线 | `Weather/WeatherFx.cpp:41-44` | DAWN 4:00-6:00 / DUSK 15:00-17:00 |
| 客户端天空层 | `Weather/WeatherBackLayers.cpp:62-63` | 同上（night_level 与 Fx 统一） |
| 服务端权威 | `BeiDou-Server_s9/.../WeatherService.java:290-293` | 同上 |

- `night_from_minute`（Fx:68-81）/`nightLevel()`（服务端 :267-280）函数体不变；golden hour 自动用新时段；最黑强度由 tint ramp + boost 决定，未改。
- **服务端需重启才生效**（用户选择自己重启）；未重启前服务端仍按旧 5-7/17-19 口径下发 minute→两端算出的 night level 会与客户端不一致（客户端已改）→ 验收前务必重启。

## 追加：2026-11-23 时间放大 4 倍 + Web 管理核验
### Web 全局天气/时间管理 —— 已做且在运行（非新增）
- 前端 9 文件：`BeiDou-Server_s9/gms-ui/src/views/game/weather/`（`index/overrideTab/timingTab/regionTab/seasonWeightTab/mapRegionTab/intensityTab/constants` + locale）。
- `overrideTab.vue:36-39`「设置」按钮、`:70-74` 时间列；`api/weather.ts:87-94` `WeatherOverrideSetParams{profileId,timeMinute,bareSky,holdMs}`；`:4` 对接 `WeatherController /weather/v1/**`。
- 实际在跑：前端 8787（vite PID 49496/48304）、后端 8686（java PID 33412）。访问 `http://127.0.0.1:8787` → 游戏/日常 → 天气页。
- 结论：无需再做；前提仍是改过的时段常量要重启服务端才生效。

### 时间 HUD 放大 4 倍
- 根因：当前 A11M=11px；枚举最大 A18M=18px（`Text.h:33-48`），换字体达不到 44px；`DrawArgument(pos,4,4)` 缩放对文本无效——`GraphicsGL::drawtext` 用 `args.getpos()` 直接算字形 quad（`:1912-1915`），不读 xscale/yscale。
- 方案：新增真实 44px 字体 `Text::Font::A44M`（`Text.h`，11×4）；`GraphicsGL.cpp:275` `addfont(...,A44M,0,44)`；`UIBuffList` 两个 Text 改用 A44M、绘制点 y=`position.y()+2`、描边 1px→2px（44px 字需要）。
- 安全性：`fonts[NUM_FONTS]` 数组与 CJK 分区循环按枚举自动扩（`GraphicsGL.h:406`、`GraphicsGL.cpp:281-285`）；bold/normal switch 有 default 兜底（`:1090/:1181`）；数字+冒号是 ASCII 不占 CJK 区。
- 构建：BUILD_EXITCODE=0、0 error；产物 `build-time44-2026-11/MapleStory.exe` SHA `A326CBC9302E5CB550BD8E7C575028F32886C7F468BD40C8CDA2C21B0C74BE94`。
- 部署：无 live 占用进程（无需杀进程）；备份 `MapleStory.exe.bak-before-time44-20261123`（SHA `4FB49A0F…A0B8`）；覆盖后 live SHA 立即+8s 均为 `A326CBC9…BE94`，无回滚。

## 构建 / 部署证据
- MSBuild v142 / Release x64 / /m:1 /t:Rebuild；`BUILD_EXITCODE=0`，日志 0 error（仅既有 warning C4005/C4100 等）。
- 产物：`build-weather-time-hud-2026-11/MapleStory.exe`，SHA256 `4FB49A0F094ED280D77B3AE26E56BA4A94700526405EF78E9CAE41468802A0B8`，5477376 B，2026-11-23 08:42:38。
- 部署前 live SHA `C2383D21C52259C3369E399E80BE8674376FEB4061D8DA58E8225854C9FFFC23`；占用进程 PID 22668（用户授权结束后覆盖）。
- 部署与防回滚复核：见本目录 `DEPLOY.md`（部署后补）。

## 诚实边界
- **真机未验收**：客户端部署后需用户本端截图（背包/弹窗夜间对比、右上 hh:mm、15/16/17 三段渐变）；未拿参考端截图当本端证据。
- **服务端未重启**：W4 在服务端侧未生效，需用户重启。
- Hindsight initiative 捕获本轮 401（无 API token）阻塞，计划落在本目录 `PLAN.md`。
- 未提交 git（用户未要求）；工作树含其它会话既有改动，本功能改动为其中子集。
