# r002 · 三层差异矩阵（原生端 → s9 → Fyms085 基线）

> 依据：`notes/r001-native-damage-evidence.md`（IDA 反编译）、`E:/MXD/扩展改动/伤害上限/IMPLEMENTATION.md`、
> `E:/MXD/自定义功能实现/长伤害显示/最终实现方案.md`、s9 源码、`[mcp:codegraph]` 与 `[subagent:code-explorer]` 调用面清单。
> 标记：**已还原** / **部分** / **缺失** / **原生端无此能力**。

## 1. 数值层

| 项 | 原生端（BeiDou.exe / ijl15） | s9 服务端 | Fyms085 基线 / 当前客户端 | 判定 |
|---|---|---|---|---|
| 物理基础值 | `sub_78DF87`：`PAD` 钳到 1999、命中率、随机 `[0.7,1.3]`、技能倍率、怪物防御、暴击 | `Character.calculateMaxBaseDamage()`：`ceil(((weaponMultiplier*主属+副属)/100)*watk)`，仅用于反外挂校验 | `CharStats::close_totalstats`：`(主属+副属)*(damagepercent+WATK/100)`，武器倍率在 `get_multiplier()` | **部分**：武器倍率同构，但缺 PAD 1999 钳制/随机区间/技能倍率细节 |
| 魔法基础值 | `sub_791617`：`MAD` 钳到 1999、独立公式、属性克制 `1-elem*0.01` | `calcDmgMax` 中 `matk` 分支 | `Skill::apply_stats` 只累加 `stats.matk`，`Mob::calculate_damage` 对 `DMG_MAGIC` 仍用武器 min/max | **缺失** |
| 技能倍率 | WZ level 节点驱动，物理/魔法分支独立 | `effect.getDamage()/100` | `stats.damage = WZ damage/100`，直接乘 min/max | **部分** |
| 无视防御 | 原生公式含 `ignoredef` 分支 | 校验侧不含（客户端自算） | `SkillData::Stats.ignoredef = 0` 恒零，`Attack.ignoredef` 未参与计算 | **缺失** |
| 暴击 | 原生有 `mincrit/maxcrit` 与暴击伤害倍率区间 | `maxWithCrit = hitDmgMax*2`（校验）；广播用负数编码暴击位 | 固定 `×1.5`，`critical` 仅来自技能节点（恒零） | **缺失** |
| 等级差 / 命中 | 原生 `sub_78DEA6` + 等级差修正 | 不校验 | `calculate_hitchance` / `calculate_mindamage|maxdamage` 简化式 | **部分** |
| 玩家受伤 | 原生 mobatk 公式 | 服务端 `TakeDamageHandler` 计算 | `CharStats::calculate_damage` 带 `// TODO` 占位 | **缺失** |
| 单段上限 | **199999.0**（`dbl_AFE8A0`，物理 `0x79058F` / 魔法 `0x791F94`），超出替换 ST(0) 后 `__ftol` | `SNIPE 195000+rand(5000)`、`BAMBOO_RAIN 82569000`、`maxWithCrit*1.5/*5` DAMAGE_HACK | `DAMAGECAP = 999999.0`（`Mob.cpp:808`） | **不一致**：三套不同上限 |
| 破功值（伤害上限同步） | 参考端 `0x3730` 下发到标题桥；玩法值仍钳 `INT_MAX` | s9 **无** 字段 / 无 opcode / 无下发 | 客户端 `0x3730` handler 仅写窗口标题 | **缺失**（s9 侧） |
| 怪物 HP 承载 | 原生客户端不持有权威 HP | `Monster.hp = AtomicInteger`、`MonsterStats.hp int`、Boss 血条 `writeInt` | 客户端仅 `int32` 血条显示 | **缺失**：>21 亿必回绕 |

## 2. 协议层

| 项 | 原生端 / 插件 | s9 | 当前客户端 | 判定 |
|---|---|---|---|---|
| C2S 每伤害行 | 4B int（`anDamage[]`）；Kaentake/ijl15 插件改造为 `int + int64(lo/hi)` 12B（**ijl15 侧休眠**） | `p.readInt()` 单行 | `AttackPacket::write_int` 单行（r806 13/13） | **缺失**（未 long 化） |
| S2C 每伤害行 | 4B int，暴击用负数反写 | `addAttackBody` `writeInt(eachd)` | `AttackHandlers` 读 int，`damage<0` 判暴击后 `&0x7FFFFFFF` | **缺失** |
| 暴击标志 | 原生 line int 上带符号位 | `-Integer.MAX_VALUE + damage - 1`（作用于 apply 列表） | 入站按负数判 | **需重定义**：long 化后符号位只留在 vanillaLine |
| 梅索爆炸 4211006 | 专用多段版式 | `parseDamage:715-750` 内联解析（`skip(12)` + `readInt`） | `AttackAndSkillPackets.h:84-107` 专用分支 | **已还原**（r806），long 化须单列例外 |
| 召唤兽攻击 | 独立 int | `summonAttack` `writeInt` | 无对应客户端实现 | **原生端无此能力**（本轮不改） |
| `writeLong/readLong` | — | `OutPacket:14` / `InPacket:10` 已存在（8B 小端） | `OutPacket` 需补 | **可用**：12B = `writeInt + writeLong` |

## 3. 表现层

| 项 | 原生端 / 插件 | 当前客户端 | 判定 |
|---|---|---|---|
| 默认数字图集 | `BasicEff.img` `NoRed0/1`、`NoCri0/1`、`NoViolet0/1`、`NoBlue0/1`（运行时确认存在） | `DamageNumber.cpp:192-202` 已引用 | **已还原** |
| 超长数字 | ijl15 用单位缩写（K/M/B/中文单位），完整封包链路休眠 | `int32_t damage` + `std::to_string`，最长 19 位无宽度保护 | **缺失** |
| 伤害皮肤 | ijl15 `DamageSkin`/`DamageRank`、`UIWindow.img/DamageSkin`、`BasicEff/damageSkin`（**本部署运行时数据无该节点**） | 0 实现，`UICharInfo.cpp:444-455` 只发 `UseCashItemBasicPacket` | **资源缺失 → 显式预留** |
| 伤害排行 / DPT | ijl15 `DamageRank::RegisterPacketHandler`；s9 `DPT_TRACKER(0x3714)` 有枚举 | 客户端无 handler | **证据不足 → 预留** |
| `UIStatsInfo` 词缀显示 | 服务端 `org/gms/combat` 有体系（本轮不纳入） | `DAMAGE_BONUS`/`FINAL_DAMAGE`/`CRITICAL_DAMAGE` 固定 `"0%"` | **本轮范围外**（用户明确不含） |

## 4. 改动面（本轮执行清单）

**客户端**（`e:/project/MapleStory-Client`）：`Configuration.{h,cpp}`、`xy-config.example.yaml`、`Character/CharStats.{h,cpp}`、`Character/Player.cpp`、`Character/Char.cpp`、`Gameplay/Combat/Attack.h`、`Skill.{h,cpp}`、`RegularAttack.cpp`、`DamageNumber.{h,cpp}`、`Combat.{h,cpp}`、`Gameplay/MapleMap/Mob.{h,cpp}`、`MapMobs.{h,cpp}`、`Data/SkillData.{h,cpp}`、`Util/Randomizer.h`、`Net/Packets/AttackAndSkillPackets.h`、`Net/OutPacket.{h,cpp}`、`Net/InPacket.{h,cpp}`、`Net/Handlers/AttackHandlers.{h,cpp}`、`Net/Handlers/CommonHandlers.cpp`、`IO/Window.{h,cpp}`。

**s9 服务端**（`e:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/`）：`net/server/channel/handlers/AbstractDealDamageHandler.java`、`util/PacketCreator.java`、`net/server/channel/handlers/CloseRangeDamageHandler.java`、`RangedAttackHandler.java`、`MagicDamageHandler.java`、`server/life/Monster.java`、`server/life/MonsterStats.java`、`server/maps/MapleMap.java`、`client/Character.java`、`net/opcodes/SendOpcode.java`、`net/server/channel/handlers/PlayerLoggedinHandler.java`、`net/server/coordinator/world/MonsterAggroCoordinator.java`、`scripting/event/EventInstanceManager.java`、配置开关。

## 5. 关键风险（来自 code-explorer 调用面清单）

1. `AttackInfo.allDamage` 改 `List<Long>` 后，`Collections.fill(onedList, 1)` 必须改 `1L`；`CloseRangeDamageHandler:150 .get(0)` 赋 int 会直接编译失败。
2. `Integer.MAX_VALUE` 暴击补偿（`:341/:354/:1022`）在 long 下语义失效，须重定义。
3. 12B/行不能简单用 `writeLong` 替换（8B）；`parseDamage` 的 `skip(12)`（梅索爆炸）与 `skip(14)`（普通目标）需逐处核对。
4. `Monster` HP 从 `AtomicInteger` 到 `AtomicLong` 会牵动 `MonsterStats.hp`、`LifeFactory:386`、`OverrideMonsterStats`、`Boss 血条 writeInt`、脚本 API（JS 数值转换）。
5. s9 游戏开关走 DB `game_config`（`GameConfig.getServerBoolean`），不是 `application.yml`；**本轮不做 DDL/DML**，开关采用不落库方式。
6. 旧端连新端会封包失步；客户端与 s9 必须同版本发布，开关成对。
