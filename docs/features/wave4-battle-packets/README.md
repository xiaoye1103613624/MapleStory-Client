# Wave 4 · B 类战斗/环境报文（2026-11-04）

> 构建：全量 Rebuild EXITCODE=0（exe 20:43:51 部署）；矩阵 c2s MATCHED 109 → **111**、never-constructed 0。
> 结论先行：台账列的 9 条 B 类里 **7 条是假缺口/依赖未建，2 条真接**（再次验证"先读 handler 再写码"的规矩——直接按 gap 表接线会产出 7 个没人收的包）。

## 真接（2 条）

| # | 上行 | s9 证据 | 消费点 | 备注 |
| --- | --- | --- | --- | --- |
| 4.1 | GRENADE_EFFECT(109) `int x,y,keyDown,skillId` | GrenadeEffectHandler：白名单 POISON_BOMB/GRENADE、校验技能等级>0 后回 THROW_GRENADE(204) 广播 | `Combat.cpp` AttackPacket 派发后（`origin`/`result.skill` 同块可用） | s2c 204 渲染早已接（WaveAShell:1727 spawn_summon）——本条补齐"自己扔的雷别人能看见"闭环；keyDown 固定 1（按下即投，不参与服务端判定分支） |
| 4.2 | AUTO_AGGRO(189) `int oid` | AutoAggroHandler→Monster.aggroAutoAggroUpdate:2090（无控制者→aggroSwitchController 抢控制；控制者=自己→setControllerHasAggro） | `Stage` 触怪碰撞块，`create_attack` **之外**（无 bodyAttack 的怪服务端同样该转战）；每 oid 1s 节流（碰撞每帧跑），`Stage::clear()` 清空 | 节流值 NO-NATIVE-EXACT（近似"进入接触"事件语义，注释已标） |

## 假缺口 / 依赖未建判词（从 gap 台账划掉或改性）

| gap 条目 | s9 复核结论 | 处置 |
| --- | --- | --- |
| `TOUCH_MONSTER_ATTACK(47)` | 仅 BODY_PRESSURE/energyBar=15000 时结算，本端无该技能数据 | 不做 |
| `MOB_BANISH_PLAYER(56)` | handler 读 int mobid 找**带 banish 信息的怪**换图；本端 spawn 解析无 banish 字段面，banish 由 Boss 脚本驱动 | 依赖未建（随公会/Boss 内容批） |
| `FIELD_DAMAGE_MOB(191)` | s9 要求 `map.getEnvironment()` 非空（障碍击怪），本端障碍只渲染无玩法面 | 依赖未建 |
| `MOB_DAMAGE_MOB_FRIENDLY(192)`/`MOB_DAMAGE_MOB(194)` | 怪物互伤事件（Dojo/召唤流），触发方是怪物 AI 状态；受控怪出招已走控制器上报（波2-A） | 联机事件图批再说 |
| `MONSTER_BOMB(193)` | s9 只认 HIGH/LOW_DARKSTAR（竹林兄弟机制），需道场内容 | 依赖未建 |
| `LEFT_KNOCKBACK(212)` | s9 收到即回 leftKnockBack+enableActions——给道场/滚球**脚本**用的请求击退，非玩家操作 | 不做（无脚本触发面） |
| `ARAN_COMBO_COUNTER(163)` | **假缺口实锤**：本端 `Combat.cpp:186` 每次战神命中已发；gap 扫描器"包类未实例化"漏判（无参构造+直接调用行不匹配类名正则） | 销案 |
| `BEHOLDER(178)` | s9 只对 AURA_OF_BEHOLDER 结算；本端暗骑 aura 充能面未接 | 随技能补全批 |
| `PARTY_SEARCH_REGISTER(220)` 复确认 | s9 空 stub，Start 代理已在（UIPartySearch:246 注释坐实） | 维持已消化 |

## 触碰文件

`Net/OutPacket.h`（+GRENADE_EFFECT=109、+AUTO_AGGRO=189）、`Net/Packets/AttackAndSkillPackets.h`（+GrenadeEffectPacket/AutoAggroPacket）、`Gameplay/Combat/Combat.cpp`（投掷上报）、`Gameplay/Stage.{h,cpp}`（仇恨上报+节流表+clear 清空）。Stage.h 加成员 → 已走全量 Rebuild（stale-obj 教训）。

## 真机验收点

1. 海盗 4 转手雷（5201002）/夜行毒弹（14111006）释放 → 他人视角应见雷标（THROW_GRENADE 广播回程已接）；`packet_dump` 可见 0x6D。
2. 撞任意怪（不攻击）→ 该怪应转身攻击玩家（服务端 aggroSwitchController）；`packet_dump` 可见 0xBD，同怪 1s 内不重复。
3. 战神命中连击计数不变（回归 163 既有路径）。
