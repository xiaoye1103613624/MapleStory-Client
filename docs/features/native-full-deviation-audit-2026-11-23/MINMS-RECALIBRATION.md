# MinMS 单向对照项 · 三端 IDA 再校准结果（2026-11-24）

> 用户指令：参照 MinMS 源码发现的问题，再对照 Angel v83 / 095 / 192 校验后全部修复；客户端 IDA 直接发现的酌情修。
> 5 个取证代理（cal-num / cal-skill / cal-move / cal-sweep / cal-p0）全部实连 13337=Angel v83 `Angel.exe`、13338=095 `Maplestory95.exe`、13339=192 `MapleStoryT.exe`，server_health ok + hexrays_ready=true。全部静态 IDA 反编译，**真机未验**。

## 1. 三端证据推翻的 MinMS 假设（当前端本来就对，**不改**）
| 项 | MinMS 说法 | 三端 IDA 实证 | 结论 |
|---|---|---|---|
| 物理常量 0.3/0.055/0.15 | "官方值" | 三端全 WZ 数据驱动（095 CWvsPhysicalSpace2D CONSTANTS 19 字段从 WZ get_double）；反编译字面量 0.3/0.5 是游泳速度界/梯形积分，无 0.055；192 已换 Box2D | MinMS 自研调校，**保持当前端 Journey 原值** |
| 暴击倍率 1.5~1.75 (bonus 0.5/0.75) | "官方值" | 083 PDamage@0x78df87 常量 20.0/÷100 → 1.2~1.5；095 PDamage@0x730130 同；192 PvM@0x100d550 同结构 | 当前端 1.2/1.5 **正确**，MinMS 偏离，**不改** |
| 冲锋磁吸 magnet_pull + 地形步进 | "原生" | 083 CombatStep@0x96d95e / 095 @0x93ee00 / 192 @0x1967ba0 均单次位移脉冲，无 get_y_below 步进、无拉怪；TryDoingMonsterMagnet 是独立技能 | MinMS 自研增强，当前端 apply_rush 属另一套设计，**不改** |
| 输入 send_key 沿语义 / can_attack 门控 | "MinMS 特殊处理" | 083 OnKey@0x94c856 keyup→OnKeyDownSkillEnd（攻击在 keyup）、keydown→UseFuncKeyMapped（道具 keydown-only）；位移技豁免攻击硬直=原生一致 | 当前端功能安全，仅时序近似；无真机不改 |

## 2. 三端证实、本轮已修（代码行号见 IMPLEMENTATION）
| 项 | 三端证据 | 当前端修复 |
|---|---|---|
| 二段跳数值 | 083 BoundJump@0x96be38 / 095 @0x93fdf0 = h±(40q+350)、v=-(20q+250) px/s；192 演进 +450/+350 | `Player.cpp:apply_flash_jump` 改 px/s 公式 × TIMESTEP/1000（原 MinMS px/tick 斜率不符） |
| 瞬移落点 | 083 TryRegisterTeleport@0x957b74 / 095 @0x913690 / 192 @0x1901190 = 单步±nRange + 上下 foothold 取近 + CanGoThrough + 120ms 节流，无 1/4 递进、无 400 上限 | `Player.cpp:apply_teleport` 去 1/4 递进与 400 硬上限，改单步落点+下方 foothold 判定；CanGoThrough/倒立取反因本端无原语显式降级 |
| 元素克制 | 083 CalcDamage 无倍率（跨版本增强）；095 get_damage_adjusted_by_elemAttr@0x7231E0 / 192 @0xFCC020：digit1 免疫=0、digit2 ×0.5 抗性、digit3 ×1.5 弱点(夹下限)，仅 skill>0&&lv>0，字母索引 P/I/F/L/S/H/D/U(/G) | `Mob.cpp:1042+` 补 skill>0 守卫、修注释标签（原 case2/3 标反）、case3 夹下限、保留 s9 兜底 1 点免疫（偏离已标注） |

## 3. 三端证实能力存在、但本轮不修（阻塞/风险，非遗漏）
| 项 | 三端证据 | 不修理由 |
|---|---|---|
| KeyDown/蓄力状态机 | 083 is_keydown_skill@0x4fb08f、OnKeyDownSkillEnd@0x95bedf、DrawKeyDownBar@0x95c3e3；095/192 同族齐全 | s9 无 083 的 opcode 92（s9 92=CANCEL_BUFF），但有 attack.charge 字段(AbstractDealDamageHandler:770)+SkillEffectHandler；需独立实现「本地蓄力条+松手发带 charge 攻击包」，属较大功能+真机手感，不混入本次逻辑校准批次 |
| NPC 头顶任务可交态 | 083 CNpc::SetQuestList@0x6d1779 / 095 @0x671980 / 192 @0xdc08b0 均有 state2 可交（CheckCompleteDemand / m_aPreCompleteQuest） | 本端 QuestIcon 节点布局与三端不同（UIWindow.img/QuestIcon 0-9 vs 三端 UIWindow2.img/QuestIcon 0-2），且客户端无 Check.img 可交判定引擎；s9 有 Quest.canComplete() 但需服务端下发状态。强行改编号=制造新 bug，按"不发明能力"维持 |

## 4. 声明
全部静态 IDA + 源码证据，未真机发包。截图未采信（来源端未确认）。本会话 4 个既有 commit 未重写；提交仅含本轮三端校准改动。
