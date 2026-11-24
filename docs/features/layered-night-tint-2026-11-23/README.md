# 夜色作用范围精确复刻 — 分层 tint（2026-11-23）

> **2026-11-23 追加决策变更（最终口径）**：用户拍板改为"**只压暗地图自带的天空 back 层（back/0-1），其它层一律不压暗**"。
> 因此本文件 §2 描述的"tiles/objs 全压、NPC 0.45"已被**取代**；最终实现见文末 §7。
> final live SHA `56b278f889e155cb40edeab394a2f8617fd313e7a71af7c46c1f2fabdab206fa`。

> 分支：`feature/dev_0.0.1`
> 用户给原生目标参考截图（47774cd4…），观察：前景角色/NPC/平台与白天一样亮，只有天空/远景变暗。
> 结论：观察**基本正确**，并据此把本端"全屏 multiply 压暗一切"改为原生分层 tint。

## 1. 原生事实（bundle 一手，行号级）

`ApplySceneryNow`（`E:/project/BeiDou-ijl15/ezorsia/weather/weather.cpp:2043-2052`）：
- `g_vTiles` / `g_vObjs` / `g_vBacks` → `Argb(1.0, EffectiveNight())` **全强度夜色乘色**（平台/物件/背景/地图自有天空都压暗）
- `g_vNpc` → `Argb(1.0, EffectiveNight()*NPC_TINT_SCALE)`，`NPC_TINT_SCALE=0.45`（weather.cpp:93）→ **NPC 只压 45%**
- **玩家角色本体不在捕获层**：本地玩家坐标只被地面模块读取（weather.cpp:943-1000 `Weather_ReadPlayerPos`），角色不进 `g_vTiles/Objs/Npc` → **完全不压暗**
- `Argb`（weather.cpp:908-920）：每通道 `255 - fNight*(255-tint)`，打包进 layer.color 的 RGB（alpha 单独），是**乘色不是叠蒙版**
- 开头注释 weather.cpp:29-33：明确放弃了"注入 desert backdrop 只压前景"的旧做法，改为"地图自己的天空也 tint"——所以天空/远景压暗是原生设计

**对用户观察的精确回应**：
- "前景和白天一样亮" → 角色/玩家确实不压；NPC 只压 45%；平台/物件（tiles/objs）原生其实**会压**，只是 tint 调色板偏亮 + 路灯光池抬亮，所以看着接近白天。
- "夜晚只是把后面背景增加夜景颜色" → 基本正确：天空/背景全压，角色不压。本端之前的全屏 multiply 把角色也压黑，是**方向性错误**。

## 2. 本轮实现（精确复刻分层 tint）

| # | 文件 | 改动 |
|---|---|---|
| 1 | `Graphics/GraphicsGL.h/.cpp` | 新增 `push_tint(Color)/pop_tint()` + `tint_stack`；在 `draw(bitmap,...)` 与 `draw_raw_ex(...)` 提交 quad 时把 sprite 颜色乘当前 tint（RGB 乘、alpha 保留）。**只作用于场景 sprite，不影响 drawrectangle/draw_glow/粒子/UI** |
| 2 | `Weather/WeatherFx.h/.cpp` | 新增 `scene_tint()`（factor=lerp(1,night_mul,level)）与 `npc_tint()`（强度×0.45）；`Fx::draw()` 的全屏 `drawscreenmultiply` 移除（保留空实现兼容） |
| 3 | `Gameplay/Stage.cpp` | `Stage::draw` 按原生分层：地图 backgrounds + tilesobjs + reactors 全压 scene_tint；npcs + mobs 压 npc_tint(0.45)；chars/player/drops/field_markers 不压；注入的夜空/月层不压（原生 alpha-only）。**tint 在每层循环内 push/pop，保持 Layer::IDs 的 z 顺序不变**（避免层间遮挡错乱） |

## 3. 与上一版的区别
- 上一版：`drawscreenmultiply` 全屏压暗 → 角色/NPC/前景全变暗（与截图原生相反）。
- 本版：分层乘色 → 角色保持白天亮度、NPC 半压、背景/平台全压（与截图原生一致）。

## 4. 构建与部署（按铁律）
- 脚本 `tools/_layertint_build.bat`，隔离 OutDir `build-tint/`+全新 IntDir，全量 Rebuild。
- `MSBUILD_EXIT=0`、error 0；产物 5513728 B，SHA `5e2f5fd4b31afb51508bfe82684c2e1f2614cbebb8f4b6fa88aaaebd2394f300`。
- 备份 `MapleStory.exe.bak-before-layertint-20261123` → 覆盖 live → T0/T10 SHA 一致、与产物差异 0 字节（无回滚）。

## 5. 路灯层级（用户曾选 A 精确复刻）— 本轮未做，如实说明

原生灯柱经 `MakeObj_hook`（lamps.cpp:1491）当 Obj 按 foothold 的 layer/z 插入 tilesobjs，故底座被地砖/草皮遮挡、光晕穿透。
本端 `kManualLamps` 手工表（WeatherLamps.cpp:61）**只有 x/y/lamp_id，没有 layer 字段**；`MapTilesObjs` 的 objs 是 `std::multimap<uint8_t,Obj>`（MapTilesObjs.h:42）按 z 排序，但没有外部 Obj 插入接口。

在缺少每层灯的 layer/z 数据的情况下强行按 y 猜 layer，会导致灯在不同地图被错误遮挡或整体消失，且我无法真机验证——按用户"高风险不擅改"红线，本轮**不动层级**。要精确复刻需二选一：
- (a) 为 `kManualLamps` 每条补 layer/z 数据（需从地图 WZ obj 或原生视频取，工作量大）；
- (b) 接受观感折中（灯柱在 tilesobjs 之后统一画，仍不被真正遮挡，但至少不在最末尾盖住角色）。

## 6. 真机验收清单（未验收，红线如实声明）
- [ ] 夜里角色/玩家保持白天亮度，不再被压黑（对照参考截图）。
- [ ] NPC/怪物比背景亮（约 45% 夜色），不是全黑。
- [ ] 平台/石墙/树/天空按夜色变暗（tiles/objs/backs），但不过度发黑。
- [ ] 月亮/夜空注入层不被二次压暗。
- [ ] 黄昏/白天过渡平滑；UI/血条/技能栏完全不受夜色影响。
- [ ] 路灯光晕、角色光环、月光外晕仍正常（光晕不受 tint 影响）。

## 7. 最终口径（2026-11-23 用户拍板，取代 §2）

用户明确要求：**只压暗地图自带的天空 back 层，其它层不要压暗**。

实现（`Gameplay/Stage.cpp`）：
- `Stage.cpp:247-249`：仅 `drawbackgrounds(0, sky_end=2)`（天空 back/0-1）push/pop `scene_tint`。
- `Stage.cpp:250` 注入夜空/月、`Stage.cpp:253` 山树 back/2+：`**不压**`。
- layers 循环（`Stage.cpp:255-274`）：tilesobjs / reactors / npcs / mobs / chars / player / drops / field_markers **一律不压**（移除上一版所有 push_tint）。
- `WeatherFx::npc_tint()` 及其声明已删除（死代码清理），`scene_tint()` 保留供天空层使用。

验证：`MSBUILD_EXIT=0`、error 0，产物 SHA `56b278f889e155cb40edeab394a2f8617fd313e7a71af7c46c1f2fabdab206fa`；已覆盖 live，T0/T10 一致、差异 0 字节。

真机验收（未验收）：①夜里只有天空变暗、地面/角色/平台/NPC 全部保持白天亮度；②月亮/夜空注入层与路灯光晕正常；③白天/黄昏过渡平滑；④UI 不受影响。
