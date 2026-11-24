# 天气系统逻辑遗漏全审 + 三项时序缺陷修复（2026-11-23）

> 分支：`feature/dev_0.0.1`
> 用户反馈三问：①路灯进图几秒后才出现 ②路灯层级在地砖前（应在后）③切图先闪白天再变夜晚；并要求"检查天气系统还有什么逻辑遗漏"
> 全程只读取证 + 行号级结论；低风险缺陷一次性修复，高风险层级项交用户拍板。

## 1. 全链路审查发现的遗漏（对照 bundle `E:/project/BeiDou-ijl15/ezorsia/weather/` + 文档）

### 本轮已修（确认缺陷）
| # | 缺口 | 根因/行号 | 修复 |
|---|---|---|---|
| 1 | 路灯进图几秒才亮 | 时间基准 `+16ms/帧` 但 TIMESTEP=8ms（WeatherLamps.cpp:224）→ warmup 实 600ms；错峰 `delay=seq*2600` 线性，第12盏等31s（:337）；无进图 snap；暮色线性曲线（:230-231） | 时间改 8ms、warmup 150帧/cool 31帧、错峰 `(seq*977)%2600`（bundle lamps.cpp:2008）、smoothstep 暮色、SNAP 直接到位（bundle StepSwitch）、升降速分离 |
| 2 | 切图闪白天 | `Stage::clear()` 清天气成 720/night=0（Stage.cpp:155-159），200ms fade 后才 load，0x373D 后到 → 中间白窗 | `Stage::clear(bool keep_weather)`，换图(SetFieldHandlers:251)/退商城(UICashShop:634)传 true 保留天气；登录/退出仍清空 |
| 3 | 夜色色相瞬跳 | `night_mul_*` 用 target 直算无平滑（WeatherFx.cpp:168-170），draw 只用平滑 night_level 作强度 → 包到达色相瞬跳；tint_* 平滑后是死变量 | 先平滑 tint_*，再用平滑值算 night_mul_*；强度仍 night_level 平滑 |
| 4 | 0x3741 截断包越界 | `available()<10` 但 wire=11B（CommonHandlers.cpp:314）→ 10B 包第11次 read_byte 越界 | 改 `<11` |

### 确认缺失但本轮未做（工作量大/高风险/原版本就无，列出供后续）
- 云×3 + 彩虹 arc + 沙暴 back/19 背景（`kSkyDefs` 只有渐变+月；K_CLOUD/RAINBOW/DUST 是空驱动）——bundle weatherfx.cpp:66-70/171-200。补云可一并缓解底部蓝带（flicker README §4）。
- 星空×2（bundle `FX_NIGHT_COUNT=3`：月+两星空，weatherfx.cpp:74/145-149）——BackLayers::build 已支持 ani+type3，加 2 条 Def 成本低但失 twinkle。
- 白天 boost 失效（Fx 只吃 clock night_from_minute，bundle `EffectiveNight=max(clock,boost)`）——正午 storm/overcast 也该压暗。**改这个要同时改路灯 night 源，影响面大，且与 11-21 multiply 改造需重新对齐，本轮不动**。
- 环境音映射错（本端 SNOW→wind，bundle snow=无声/overcast=wind）、无 rain 迟滞淡入；`prof.sound` 字段现成未用（WeatherConfig.h:30）。
- 路灯 nWarm 暖/中性区分、uGlowRgb 逐灯染色、GLOW_CROWD_R=120 互压、真逐层 relight（现为全屏 multiply + glow 观感等效）。
- 雪地脚印、水坑变体×3、ACCUM_MARGIN、snow_slip 打滑（高风险共享 Physics，文档也列为可选）。
- 摇曳植物 sway（weathersway.cpp 1364 行，复杂）。
- 天气配置 `apply_profiles/apply_tint_ramps` 零调用、0x373D 下发的 tint R/G/B 渲染端未读（需先确认服务端是否只发 palette id，资料不足）。

### 文档过时（代码已超越文档，建议同步文档）
`IMPLEMENTATION-STATUS.md:30/82-83/175`、`ACCEPTANCE-CHECKLIST.md:42`、`flicker README:54`、`WeatherGround.h:8-16` 仍写"地面用玩家脚下近似/环境音无声/路灯非逐灯 sprite"——实际都已实现（WeatherGround.cpp:355-435 foothold、:281-325 发声、WeatherLamps.cpp emitter 多光源）。

## 2. 用户问题②「路灯层级在地砖前，应在后」— 需用户拍板方案（高风险未擅改）

现状：路灯是 `Stage::draw` 末尾全屏 overlay（`Stage.cpp:268`），在 tilesobjs/角色之后画 → 必然压在地砖前。
原生：经 `MakeObj_hook`（lamps.cpp:1491）把灯当 Obj 按 foothold z 插入 tilesobjs，灯柱底被地砖/草皮遮挡，只有光晕穿透。
本端 `MapTilesObjs::objs` 是 `std::multimap<uint8_t,Obj>`（MapTilesObjs.h:42，按 z 排序），但没有"插入外部 Obj"接口。

两个方案（价值/风险判断交用户）：
- **A. 精确复刻**：给 MapTilesObjs 加外部灯柱源，按灯的世界 y 对应的 layer + z 插入 objs 多映射，光晕仍单独最后画。最贴原生，但改资源加载链路，有灯消失/闪烁风险，须真机反复验。
- **B. 观感折中**：把灯柱 sprite 绘制从末尾移到 layers 循环里 tilesobjs 之后（至少不在最后），光晕仍最后。改动小、无崩溃风险，但灯柱底仍不会被地砖真正遮挡（只是不再盖住角色/怪物）。

## 3. 构建与部署（按铁律）
- 脚本 `tools/_wxsweep_build.bat`，隔离 OutDir `build-wxsweep/`+全新 IntDir，全量 Rebuild。
- 构建期间遭另一会话并发 MSBuild/cl 抢 PDB（CL.exe -1 / .r/.t 假错），等待后重试 `MSBUILD_EXIT=0`、error 0。
- 产物 5475840 B，SHA `c2383d21c52259c3369e399e80be8674376feb4061d8da58e8225854c9fffc23`（比上轮大 10KB：含并发会话的 UISideIconBar.cpp 等未跟踪文件改动）。
- 备份 `MapleStory.exe.bak-before-wxsweep-20261123` → 覆盖 live → T0/T10 SHA 一致，与产物差异 0 字节（无回滚）。

## 4. 真机验收清单（未验收，红线如实声明）
- [ ] 换图/退商城不再"闪一下白天"，夜色连续过渡。
- [ ] 进图路灯立即可见（或 ≤2.6s 全亮），不再逐盏等十几秒。
- [ ] 0x373D 到达/天气切换时夜色无瞬跳色相。
- [ ] 白天正常、黄昏平滑；0x3741 强度切换不崩包。
- [ ] 云/星空/星空等未做项不影响现有表现。
