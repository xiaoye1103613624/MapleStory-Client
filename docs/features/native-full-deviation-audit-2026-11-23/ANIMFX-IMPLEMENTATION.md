# Animation/EffectLayer 高级特效族 · 实施记录（2026-11-24）

> 用户授权「可以，一定要对照那三个客户端校准或参考一下」；三端校准见 ANIMFX-PHASE0.md。
> 构建：最终 `tools/build_smoke.bat` → EXITCODE=0 / GATE OK。真机：**未验收**。
> 重要前提纠正：三端校准证伪「std::set 去重破坏 zigzag」——set 去重在 live 资源上不丢帧、zigzag 靠 framestep 标志（与 MinMS 同源），此项未作为 bug 修；本轮实现的是经证据确认的**API 能力差距**与共享 bug。

## 实现项（逐条 file:line）

### A. Animation API 补全（对齐 MinMS Animation.cpp:194-259/311-456）
| 项 | 证据 | 改动 |
|---|---|---|
| 完整构造 | `Graphics/Animation.h:78`、`Animation.cpp:179-231` | 新增 `Animation(node,start_frame,force_zigzag,settle_static,keep_order)`；keep_order 按 WZ 原始顺序装载绕过 set 去重（保 uol/重复帧）；原 `Animation(node)` 委托之（零行为变化） |
| oneshot/settle | `Animation.cpp:341-456` | `update(timestep,loop)`：loop=false 末帧/0端冻结 + 0xFFFF 哨兵；settle_static 播一次停末帧；`update()/update(ts)` 委托 loop=true（现有调用零变化） |
| delay_scale 慢放 | `Animation.cpp:355-373` | `set_delay_scale`/scaled_left 逐帧慢放（MinMS Papulatus 展开用，标注非原生复刻） |
| freeze/thaw API | `Animation.h:100-105`、`Animation.cpp:533-566` | `freeze_at`/`thaw_and_reverse`/`total_delay`/`get_frame_index`/`get_frame_count`/`is_zigzag`/`set_zigzag`/`get_delay_left`/`get_framestep` |
| Frame 终值 | `Animation.h:43-44`、`Animation.cpp:141-150` | 新增 `end_opacity()/end_scale()`（oneshot 冻结用） |
| 共享 bug 修正 | `Animation.cpp:488-500` | `getdelayuntil` 从 `frames[frame_id]` 改为 `frames[i]`（MinMS:675 同源 bug，当前端此前零调用点） |

### B. EffectLayer 能力补齐（对齐 MinMS EffectLayer.h:43-251，标注现代化增强）
| 项 | 证据 | 改动 |
|---|---|---|
| Effect.wait_ms | `EffectLayer.h:61-86` | `add(...,delay_ms=0)` 先等再播，draw/update 处理 wait |
| FallEffect | `EffectLayer.h:90-139` | 新增类：start_offset/fall_delta/delay/duration/follow/mirror/angle；draw 按 t 线性位移 + flip 镜像；update(movement) 中 !follow 世界锚定 |
| OffsetEffect | `EffectLayer.h:141-176` | 新增类：固定 offset + 延时启动 |
| 四个 add | `EffectLayer.cpp:108-130` | `add_fall`/`add_particle`(mirror FallEffect)/`add_offset`，计入 effect_cap |
| update(movement) | `EffectLayer.h:37`、`EffectLayer.cpp:81-105` | 默认参数 `{}` 保持所有现有 `update()` 调用兼容；falls/offsets 逐帧更新与回收 |
| z 分流 draw | `EffectLayer.cpp:41-77` | drawbelow 画 z<0 的 falls；drawabove 画 z>=0 falls + 全部 offsets |
| Char 接线 | `Char.h:200`、`Char.cpp:36/124-128`、`Char.cpp:~236 三 helper` | Char::update 计算 movement=当前位置-上帧位置传 effects.update；新增 show_falling_effect/show_particle_effect/show_offset_effect 三个入口（均受 SysOptSkillEffects 开关） |

### C. 未做的接线（有意，见 PHASE0 方案）
- 未把 fall/particle 自动接到具体技能：需逐技能 REA/WZ 证据确认 special 的 x/y/fall/interval 语义，属单项另案；本轮只暴露能力入口，不发明时序。
- 未改 WZ/IMG、未改服务端、未动 opcode。

## 保留的当前端独有能力（零回归）
`Animation(node,texture_override)` 构造、`freeze_opacity_if_static`、`EffectLayer::count()/effect_cap() 画质密度上限`——全部保留，新增特效也计入 count。

## 验证
- 构建 EXITCODE=0 / GATE OK（Release/x64）。
- 静态：所有新 API 存在（grep 行号见上）；现有 `update()` 调用因默认参数零改动；`getdelayuntil` 索引修正。
- 三端校准证据：ANIMFX-PHASE0.md（MinMS 蓝图 / Angel IDA 地址 / 095-192-xy 资源逐字段一致 / D3 帧无重复实证）。
- 真机（待用户）：新 API 本身无即时可见变化（能力层），需后续逐技能接线或专门探针才能观察 settle/fall/offset 观感；由用户 s9+MySQL 环境点验，截图须先确认来源端。

## 风险与声明
- freeze_at/thaw/delay_scale/fall/particle/offset 是 **MinMS 现代化增强**，Angel 083 无对应原生原语（PHASE0 §1 已证），不宣称原生还原。
- 全部为静态源码 + IDA + 资源交叉证据，真机未验。
- 提交隔离工作树既有 445 项改动。
