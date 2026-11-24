# Animation/EffectLayer 高级特效族 · 阶段0 证据与方案（2026-11-24）

> 用户要求：「评估并实现 EffectLayer/Animation 高级特效族改造，一定要对照那三个客户端校准或参考」。本文件 = skill 阶段0/0.5 证据解析 + 分析归纳 + 方案；**未改源码**。下一步需用户确认方案后再实现。

## 0. 先纠正一个错误前提（诚实声明）
上一轮 ROUND2-README 把「std::set 去重破坏 zigzag（0,1,2,1,0）」列为 P2 功能落差，并以此作为本改造的主要理由之一。**三客户端校准证伪了这一点**：
- **MinMS** 自己也用 `std::set<int16_t> frameids`（`E:/project/MinMS/src/client/Graphics/Animation.cpp:228`）按 id 升序唯一装载，zigzag 靠独立 `framestep=±1` 方向标志在 `[0..last]` 回放（`:256/:387-414`）——WZ 只存 0,1,2 三帧，set 里根本没有重复帧可丢。
- **Angel 083 IDA**（端口13337 `` `Angel.i64``）：`find_regex(zigzag|reverse|bounce|loop)` 0 命中；原生是「单调帧推进 + 循环锚点帧回绕 + 重复计数」（`CMob::ProcessAction@0x666205`），**没有 Ping-Pong 原语**。
- **资源层 D3 实证**：live `14121004/keydown` 帧 children = `0..9` 无重复、`0001009/screen`=`0..12` 无重复、pet `00002000/stand1`=`0,1,2` 无重复；192 `1412.img.xml` 与 xy 逐字段一致（尺寸/origin/delay/_hash 全同）。「0,1,2,1,0 往返」**不在资源节点里**，是播放端解释。
- 结论：**当前端 set 去重在此客户端实际资源上不丢帧，zigzag 逻辑与 MinMS 同源近似，不是真 bug**。Angel 子代理「set 破坏 zigzag」是从代码结构推断、未经 WZ 证据，已被资源实证推翻。此项从偏差清单移除。

## 1. 真正经三端确认的差距（方案范围）
权威=Angel 083；移植蓝本=MinMS；交叉=095/192 资源。

| 能力 | Angel 083 原生语义 | MinMS 蓝本 | 当前端 | 判定 |
|---|---|---|---|---|
| keep_order 保序装载 | 帧容器线性遍历、逐帧 delay 累加，无按 id 去重（ProcessAction/LoadMobAction） | `Animation(src,start,force_zigzag,settle_static,keep_order)` keep_order=true 绕过 set（`Animation.cpp:212-225`） | 只有 set 路径，无 keep_order | **真实缺口**：若某 WZ 用 uol/重复帧名表达回摆（095 XML 大量 `<uol name="2" value="1"/>`），set 会折叠。live 当前样本无重复，但接口缺失属能力差距，可低成本补齐 |
| oneshot/settle（停末帧） | `ShowSkillPrepare@0x93640b`/`ShowSkillSpecialEffect@0x936528` 传 `0x7FFFFFFF` 无限循环，直到外部停 | `update(timestep,loop=false)` + 0xFFFF 冻结哨兵、`settle_static`（`Animation.cpp:321-456`） | 仅 `update()/update(timestep)`，末帧 wrap 回 0 | **真实缺口**：一次性技能特效会循环/回站姿，与原生「播出即保持」不符 |
| freeze_at/thaw_and_reverse/delay_scale | 083 无 Papulatus 卷曲具名证据（0命中）；hold 由帧 delay 表达 | Papulatus 卷曲 `freeze_at`@Mob.cpp:1230 / `thaw_and_reverse`@:1278 / `set_delay_scale`@:1280 | 全缺 | **条件性**：MinMS 有完整实现，但 083 无一手证据、live 未定位卷曲节点 → 可补 API 供未来用，不声称原生复刻 |
| total_delay / start_frame / set_zigzag / get_frame_index | 083 `GetActionDelay@0x664da5` = 逐帧 delay 之和 | 全套 public API（`Animation.h:82-130`） | 全缺 | **真实缺口（接口层）**：驱动 hold 时长、技能特效时长所需 |
| EffectLayer delay_ms | 原生特效无独立 delay 参数，时序来自帧自身 delay | `add(...,delay_ms)` 先等再播（`EffectLayer.h:43-47`） | `add` 无 delay | **真实缺口**：I/L 箭雨等需延时的多发特效无法表达 |
| EffectLayer fall/particle/offset | 083 无 particle 系统/CTemporaryEffect/AddEffect；最近似 `Effect_Tremble@0x43da05`（相机偏移+衰减+归零） | `FallEffect`/`OffsetEffect` 类、`add_fall`/`add_particle`/`add_offset`（`EffectLayer.h:125-251`） | 全缺 | **现代化增强，非原生复刻**：MinMS 有完整实现可移植；但必须标注「非 083 原生语义」，不能包装成还原 |
| getdelayuntil 索引 bug | 同源 bug（MinMS `:675`） | `frames[frame_id]` 非 `frames[i]` | 同 bug（`Animation.cpp:406`） | **共享 bug，当前零调用点** → 顺手修（低风险） |

## 2. 三客户端一致性
- `14121004/keydown`：**xy ≡ 192 逐字段一致**（10 帧尺寸/origin/delay=120/_hash 全同）；095 为较早变体（无 14121004），节点类别同构。
- `special`=帧动画 + `x/y/fall/start/interval/count/duration` 复合参数；`tile`=子帧 + `effectDistance` + alpha 对；`screen`=纯帧动画；`zigzag`=INT flag（非帧序列）；`finish`=三端都存在；`receiver`=三端皆无（不是版本差异）。
- 权威口径：095/192 仅交叉，不改 v83/Angel 裁决。IV 三套（xy GMS `TSPHKw==` / 192 零 `AAAAAA==` / 095 CMS `uX1j6Q==`）已由 ev-res 独立复证 AGENTS.md §3.1。

## 3. 推荐方案（一次性实现，纯客户端、不动服务端/资源）
按「真实缺口优先、原生语义标注、低风险顺手」分三组：

**A. Animation 接口补全（对齐 MinMS，低风险，无行为回归）**
- 新增构造 `Animation(node,start_frame,force_zigzag=false,settle_static=false,keep_order=false)`；keep_order 时按 WZ 子节点原始顺序 emplace 不做 set 去重（保 uol/重复帧）。
- 新增 `update(timestep,loop)`（loop=false 时末帧/0端冻结 + 0xFFFF 哨兵）、`settle_static`/`settled` 状态、`delay_scale`/scaled_left 慢放。
- 新增 public：`total_delay/get_frame_index/get_frame_count/is_zigzag/set_zigzag/freeze_at/thaw_and_reverse/get_delay_left/get_framestep/narrowest_frame_index`。`freeze_at/thaw` 供未来 Papulatus 类用，**标注为 MinMS 增强而非 083 原生**。
- 顺手修 `getdelayuntil` 索引 bug。
- 现有 `update()/update(timestep)` 改为委托 `update(ts,true)`，保证现有调用零行为变化。

**B. EffectLayer 能力补齐（对齐 MinMS，标注现代化增强）**
- 新增 `Effect.wait_ms`（delay_ms 先等再播）；`add(...,z,speed,delay_ms=0)`。
- 新增 `FallEffect`（start_offset/fall_delta/delay/duration/follow/mirror/angle）+ `add_fall`/`add_particle`（mirror 复用 FallEffect）。
- 新增 `OffsetEffect` + `add_offset`。
- `update(Point movement={})` 传 movement，`!follow` 世界锚定 `start_offset -= movement`；`drawbelow` 画 z<0 的 falls，`drawabove` 画 z>=0 falls + 全部 offsets。
- 保留当前端独有的 `count/effect_cap` 密度上限、`texture_override` 构造、`freeze_opacity_if_static`，零回归。

**C. 接线（把已修能力实际用起来，需证据支撑才动）**
- `SkillUseEffect`/`Skill` 里的 prepare/screen/special/tile 播放改用 `delay_ms`/settle/keep_order——但仅对 live WZ 确有对应节点的技能（14121004 等），不改无源技能。
- 不主动接 fall/particle 到具体技能（需先有 REA/WZ 证据证明某技能用 special 位移参数；本轮证据表明 special 资源带 x/y/fall 参数，可后续单项立项）。

## 4. 风险与边界
- Angel 083 无 particle/freeze 一手证据 → B 组和 freeze_at 属「照 MinMS 现代化增强」，文档必须如实标注，绝不宣称原生还原。
- 当前端 `count/effect_cap` 画质密度上限可能与新增 fall/offset 冲突，实现时新增特效也计入 count。
- 全部静态 + IDA + 资源交叉证据，**真机未验**；动画观感（尤其 settle/keep_order/delay）须用户在 s9+MySQL 环境点验。

## 5. 待用户拍板（阶段1）
- **D1 范围**：A+B 一次性做（接口+EffectLayer 能力），C 组接线只做「已证有源节点」的最小接线；fall/particle 到具体技能的逐技能接线另案。是否同意？
- **D2 标注**：接受「freeze_at/thaw/fall/particle 标注为 MinMS 现代化增强、非 083 原生复刻」的表述？
- **D3 验证**：真机由你点验，截图须先确认来源端；编译后如实声明未真机。
- **D4 提交**：单独原子 commit，隔离工作树既有 444 项改动。

用户确认后进入一次性实现；在此之前不改源码。
