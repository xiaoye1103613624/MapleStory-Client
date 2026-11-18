# 真机视频五问逐条结论（2026-11-17 23:26:14 视频）

> 证据：`evidence/hp-dragonroar-sequence-20261117_2326.png`（联络表，HP 从 1081 多次施放掉到 110，中间出现 4023/4032/7152 等异常高值）
> 纪律：每条给行号级结论；真机发包未验收如实声明；不拿"应该没问题"糊弄。

## 用户5问逐条结论

### Q1 龙咆哮血量判断是否正常？——❌ 不正常，我上轮客户端门禁拦不住持续自损

- 上一轮我加的门禁 `Character/Player.cpp:466-494`：仅在 `Player::can_use`（起手瞬间，`:132 Combat::use_move`）判断 `HP > maxHP×x/100`。
- 但龙咆哮是**持续技能**（WZ `level/time=4`，4秒持续，effect 18帧），服务端在持续期间由 `StatEffect.java:772 hpR=-x/100` + `:1439 hpchange += maxHP×hpR` **每 tick 权威自损**，客户端门禁只在起手判断一次，管不住后续扣血。
- 结果：起手时 HP 够（>59%或按等级x），但4秒内持续自损把血扣到 0——真机现象坐实我上轮方案不完整。
- 服务端有保护 `Character.java:8478 cannotApplyHp` + `:8481 return false`：非GM 且 `nextHp<=0` 时**拒绝本次自损**（技能不生效），所以理论上不该到0；真机到0说明自损走的不是这条受保护路径，或 HP 已被其它来源压低。

### Q2 为什么自动加血（没用药水）？——服务端 autopot 或地图回血，两条路径

- **服务端 autopot**：`Character.java:8494-8510`，`applyHpMpChange` 中 `hpchange<0`（龙咆哮自损即负数）且 `use_server_auto_pot/use_compulsory_auto_pot` 开启时，自动触发 keybind 91 的药水 `PetAutopotProcessor.runAutopotAction`。这是"自损触发自动吃药"的原生机制。当前运行时配置未显式读到该开关（yml 无键 → `getServerBoolean` 默认 false），但**需以服务端实际加载的外部配置为准**。
- **地图自动回血**：客户端 `Gameplay/Stage.cpp:336 try_heal_over_time()` 每帧调用，`:394-424` 按 `mapinfo.get_recovery()`（默认 1.0，`MapInfo.cpp:53-57`）每 tick 回 `maxHP×1%×recovery`，钳到 `abHeal=77×rec×1.5`。**这是客户端视觉回血**，不走服务端，可能与服务端权威 HP 反复打架 → 视频里 110↔1110 跳变、以及异常高值。
- 异常高值（4023/7152 > maxHP 1081）：`setHp` 有 clamp（`AbstractCharacterObject.java:227-233` 限 0..localMaxHp），服务端不可能发超过 maxHP 的值；客户端 `PlayerHandlers.cpp:176` read_int 与服务端 `PacketCreator.java:1081 writeInt` 对齐。故高值更可能是**客户端本地 try_heal_over_time 与服务端包交替覆盖 + 显示刷新竞态**，非包错位。

### Q3 使用龙咆哮血量到0？——会，因持续自损无下限保护到1

- 服务端 `applyHpMpChange:8478` 保护在 `cannotApplyHp`（非GM nextHp<=0 拒施），保护成功时 HP 停在 >0。**但若 HP 已被压到临界，保护触发后本次技能无效，HP 不再降**——真机仍到0，需抓服务端日志确认自损实际走的路径。
- 用户期望（Q4）：HP 最少为1，0=死亡。当前 `setHp` 下限 clamp 到 **0** 而非 1（`AbstractCharacterObject.java:229-230`），这是"血能到0"的直接代码原因。

### Q4 血量应最少为1、0应死亡、技能不该让血到0？——部分未实现

- **下限为0**：`setHp:229-230` `if(thp<0) thp=0` —— 应改为 1 或触发死亡。
- **0=死亡**：服务端 `hpChangeAction:8397-8415` 判定 `hp<=0 && oldHp>hp → playerDead()`（`:8410`），逻辑通；经 `registerCharacterStatUpdate` 每200ms 执行（`MapleMap:876,4586`）。`playerDead():7081-7146` 处理掉经验/清buff/下椅。
- **但**：`playerDead()` **没有下发"角色死亡"状态包给客户端**。客户端 opcode 表只有 `MONSTER_CARNIVAL_DIED=294`（斗技场，`PacketSwitch.cpp:316,669`），无普通角色死亡 handler。所以服务端判死了、客户端角色仍站立、无死亡表现——这就是"0不死"的观感真因。

### Q5 死亡逻辑和特效实现了没？——服务端业务逻辑有，客户端状态/特效缺失

- 服务端死亡业务：`Character.java:7081-7146 playerDead()` 有（掉经验/安全护符/清buff/取消变形/下椅）。
- 客户端死亡状态：`Char.h:53 DIED=18`、`Char.cpp:73`、`Player.cpp:593/610` 有 DIED 状态引用，但**没有任何 s2c 包把角色切到 DIED**（无 opcode、无 handler）。
- 死亡特效：`Data/Effect/` 无 die/dead 节点（BasicEff/CharacterEff/... 均无），客户端无死亡动画资源接线。
- 复活流程：服务端 `Character.java:7203 cancelAllBuffs` 附近有复活相关注释，但需真机验证复活包链路。

## 根因归类（一次性列全）

| # | 缺口 | 位置 | 性质 |
| --- | --- | --- | --- |
| A | 客户端门禁拦不住持续自损 | `Player.cpp:466-494` 只在起手判断 | 我上轮方案不完整 |
| B | HP 下限 clamp 到 0 而非 1/死亡 | `AbstractCharacterObject.java:229-230` | 服务端 |
| C | 无角色死亡状态包/handler | 客户端无 opcode + 服务端 playerDead 不下发 | 服务端+客户端 |
| D | 无死亡特效资源/接线 | `Data/Effect/` 无死亡节点 | 客户端资源 |
| E | 客户端 try_heal_over_time 与服务端权威 HP 竞态 | `Stage.cpp:336,394-424` | 客户端 |
| F | autopot 被自损触发 | `Character.java:8494-8510` | 服务端配置 |

## 下一步（需一次性改全，禁止挤牙膏）

真机证明上轮纯客户端门禁不足。需按 A-F 全链路评估：B（HP下限1）、C（死亡状态包+handler）是核心；A 要改为服务端权威或客户端持续判断；E 要评估是否关闭客户端本地回血以免与服务端打架。涉及服务端+客户端协议，按 skill 门禁需先出 Plan 并经用户确认范围（改玩法行为）。
