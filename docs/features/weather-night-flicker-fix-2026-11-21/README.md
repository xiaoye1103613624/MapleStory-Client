# 天气系统夜间逻辑根治（2026-11-21）

## 0. 背景
用户本端两段视频（`XiaoYe_2026-11-21_11-58-41.mp4` / `12-03-28.mp4`）反馈：夜间月亮/路灯**一直闪烁**，且**月亮显示在平台/石墙前面**；并质疑"夜间效果就是加一层蒙版？原始文档没有具体逻辑？光晕都没有"。另附 1920×1080 截图反馈底部蓝色空背景带。

本次按"点到面、一次性"原则，读穿 bundle 原始实现（`E:/project/BeiDou-ijl15/ezorsia/weather/weatherfx.cpp` + `weather.cpp` + `lamps.cpp`）、本端全部 `Weather/*`、IDA 取证、视频逐帧量化后，一次性修复确定性缺陷。

## 1. 闪烁根因（确定性 BUG，已修）
- **真因**：天气层绘制把不透明度写成 `l.current * alpha`，而 `alpha` 是 `MapleStory.cpp:285` 的 `accumulator/timestep`——**每帧 0..1 抖动的插值系数**（UI 侧 `MapleStory.cpp:236` 早已改 `UI::draw(1.0f)`，天气层漏改）。
  - `WeatherBackLayers.cpp:256`（sky cover）、`:303`（平铺月亮/天空）、`WeatherLamps.cpp:249`（路灯 `ga=lit*stagger*alpha`）。
  - 视频逐帧量化：整体亮度 108.9↔121.0 逐帧跳；天空区几乎不变 → 是全屏天气 alpha 抖，不是夜色曲线。
  - 文档 `IMPLEMENTATION-STATUS.md:84` 自写"alpha 不乘 UI 插值 alpha"，代码相悖＝回归。前几轮"删呼吸/固定坐标"都没碰这几行。
- **修复**：三处全部去掉 `* alpha`，天气层不透明度只由 `l.current`（step_toward 平滑的天气/时钟强度）驱动；位置已由 viewx/viewy 插值，不需要第二个 alpha。
- **曲线统一**：`WeatherBackLayers::night_level` 原为线性，`WeatherFx::night_from_minute` 为 smoothstep，黄昏/黎明错拍 → 统一为 smoothstep（与服务端 WeatherService 同步）。

## 2. 层级根因（已修）
- bundle `weatherfx.cpp:644-652`：月/星是 `front=0`（back 天空层），经 `weather.cpp` 的 z-promotion 让地图山树/平台盖在其前（正确深度）。
- 本端原把月亮放 `kFrontDefs`（front=1）→ `draw_front` 在角色/平台之后画 → "月亮显示在平台前面"。
- **修复**：月亮（nightDesert back/3）从 `kFrontDefs` 移到 `kSkyDefs`；`Stage::draw` 的 `draw_sky` 在地图背景 back/2+ 与 tilesobjs 之前，层级与原生一致。

## 3. 夜间逻辑（用户质疑"只是蒙版"，已改为原生 multiply）
- bundle 原生夜色 = **逐层乘色**（`weather.cpp:908-920 Argb` / `:939-941 SceneryColor`）：把场景 RGB 按夜色调色板逐像素压暗，月/星走 **alpha-only 曲线**（`Weather_NightSkyColor:1055-1060`，RGB 恒白）、材质自带 glow；还有 golden hour、路灯 LB_NORMAL 暖核/LB_ADD 光晕/光敏迟滞/错峰、地面多模块。
- 本端原用 `WeatherFx.cpp:200 drawscreenfill(tint, alpha)` 全屏盖半透明暗色＝"洗灰蒙版"，与原生语义不同。
- **修复**：
  - `GraphicsGL` 新增 multiply 批次 `quads_mul`：`drawrectangle_mul` / `drawscreenmultiply`，flush 用 `glBlendFunc(GL_DST_COLOR, GL_ZERO)` 实现 `dst *= src.rgb`。
  - `WeatherFx::draw()` 改为 `drawscreenmultiply(night_mul_*, strength)`，`factor = lerp(1, nightTint, nightLevel)`，天亮不改画面、深夜原生压暗；`night_mul_*` 含 golden hour 色温（update 里算好）。
- **文档核查结论**：用户"原始文档没有具体逻辑"基本属实——项目文档只写了"全屏 tint 近似"（PLAN P5.1 / STATUS:173）、昼夜 smoothstep 与路灯细节（realism-design），**月/星/云/蒙版各层 alpha 曲线与 multiply 混合模式只在 bundle 源码里，没抄进项目文档**。本次把关键逻辑落到实现与本文档。

## 4. 底部蓝带（未完成，需用户拍板）
- 视频/像素取证：蓝带 = RGB(60,99,161) = 顶端天空色 = 天空基座 `back/0`（TILED 铺满）经夜色 tint；**不是**兜底 fill（兜底 RGB(107,168,219) 出现 0 次）。
- 根因（IDA + 源码，行号级）：NORMAL(type=0) 背景层不随视口延展（`MapBackgrounds.cpp` draw 的 htile/vtile 对 type0 恒 1）；真 v83 原生**无 1080p 游戏视口**（docs/multi-resolution:37 分辨率是插件新增），高视口下 back/2 山体底边到草带之间露出 sky base。不是相机 clamp 失效，也不是 letterbox 缺失（原生 letterbox 是黑色、按地图 Info 配置）。
- **未改代码**，因为 IDA 指明首选修法是"资源/平铺补齐"（改 WZ/IMG 给山体下补植被使其覆盖到 1080p），需资源编辑 + 视觉验收；次选"相机可视范围约束"有玩家偏位副作用；两者属价值/范围判断，交用户拍板：
  - A. 资源补齐（最贴原生，需 orange-wz 改 live back 层 + 逐档验收）；
  - B. 相机 clamp 约束（不改资源，但靠近地图底部时角色可能偏离屏幕中心）；
  - C. 接受为已知问题（真 v83 本无此视口）。

## 5. 构建与部署
- 脚本 `tools/_adaptive_build.bat`，Release x64 v142，隔离 IntDir。
- 构建 `MSBUILD_EXIT=0`，`error`=0。
- 产物 SHA256 `BBBB06481841CDE3EA7F3B6C6581641ABA5540F30A9DBD4F4C38B020382FDCE8`。
- 按铁律覆盖 live 正式 exe `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，**live 实测 SHA = 产物 SHA**，8s 后防回滚复核一致；locale 键仍在（2/2）。
- 备份 `MapleStory.exe.bak-before-weatherfix-20261121`。
- 期间并行会话多次改同批文件并部署过（live before SHA 曾为 `1cca3684...`），本机构建基于当前工作树全部改动，部署 SHA 与本机构建一致。

## 6. 真机验收（待用户，截图须本端）
- [ ] 夜间月亮/路灯亮度稳定，不再逐帧明暗跳（建议再录 4s 视频抽帧对拍）；
- [ ] 月亮被平台/石墙正确遮挡，不再画在前景；
- [ ] 夜色为逐像素压暗（不发灰发白），golden hour 黄昏偏暖；
- [ ] 底部蓝带按用户拍板方案处理。

## 7. 如实声明
- 已构建、已部署 live 正式 exe 并实时核对 SHA（含防回滚复核）。底部蓝带**未修**，明确列为待拍板项。
- 真机未验收；IDA 用的 IDB 是现代客户端（非 v83），结构性结论可用、地址不可直接映射 v83，已在报告注明。
- 仍缺失（bundle 有、本端未补）：星×2、云×3、彩虹 arc、路灯真 sprite/光池/NPC 分层 relight、地面物理/音效。均属"完整还原"范畴，未在本次"闪烁/层级/multiply"修复内，未宣称闭环。
