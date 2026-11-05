# 改动路径索引（客户端 ↔ s9）

## 出站（伤害产生 → 服务端扣血）

```
Character/CharStats.cpp::close_totalstats       面板 min/max（武器倍率×主属+副属）×WATK/100
Character/Player.cpp::prepare_attack           v2: MAD 1999 钳制、INT/LUK 魔法基础值、暴击区间、BOSS 增伤
Gameplay/Combat/Skill.cpp::apply_stats         v2: 魔法技能改走 magicdamage、ignoredef、criticalDamage
Gameplay/Combat/RegularAttack.cpp              普通攻击（继承 Attack 字段）
Gameplay/Combat/Combat.cpp::apply_move         技能/普通攻击派发
Gameplay/MapleMap/MapMobs.cpp::send_attack     [r806] 候选怪钳到 15
Gameplay/MapleMap/Mob.cpp::calculate_damage    v2: 无视防御、BOSS 增伤、暴击区间
Gameplay/MapleMap/Mob.cpp::next_damage         【唯一 clamp 点】2^53 ∩ 配置 cap ∩ 0x3730 破功值
Gameplay/Combat/Attack.h                      Attack / AttackResult / DamageLine(int64,bool)
Net/Packets/AttackAndSkillPackets.h           legacy 4B 或 long 12B/行（damage.long_lines）
  -> s9 AbstractDealDamageHandler.readDamageLine()
  -> s9 parseDamage()  (Map<Integer,List<Long>>，符号编码暴击)
  -> s9 applyAttack()  (long totDamage / totDamageToOneMonster)
  -> s9 Monster.aggroMonsterDamage(long) / MapleMap.damageMonster(long)
  -> s9 Monster.damage(long) / applyAndGetHpDamage(long) / AtomicLong hp
```

## 入站（广播 → 飘字）

```
s9 PacketCreator.addAttackBody()              写 int32 vanillaLine + int64 real（或 legacy 负数编码）
  -> 客户端 Net/Handlers/AttackHandlers.cpp  long 模式读 int+long，暴击取 vanilla 符号位
  -> Gameplay/Combat/Combat.cpp::push_attack / extract_effects / place_numbers
  -> Gameplay/Combat/DamageNumber.cpp         int64 数字，最长 16 位按 260px 等比缩放
```

## 上限同步

```
s9 SendOpcode.LIMIT_BREAK_SYNC(0x3730)
  -> s9 PacketCreator.limitBreakSync(long) 由 PlayerLoggedinHandler 上线时下发
  -> 客户端 Net/Handlers/CommonHandlers.cpp::LimitBreakSyncHandler
       ① IO/Window.cpp::set_limit_break（标题，原行为保留）
       ② damage::set_server_limit_break（新）→ Mob::next_damage 的 effective_cap
```

## 配置开关

| 端 | 位置 | 键 | 默认 |
| --- | --- | --- | --- |
| 客户端 | `xy-config.yaml` / `Configuration.cpp` | `damage.formula_v2` | false |
| 客户端 | 同上 | `damage.long_lines` | false |
| 客户端 | 同上 | `damage.cap` | 9007199254740992 |
| s9 | `application.yml` / `DamageLinesConfig` | `gms.service.damage.long-lines` | false |
| s9 | 同上 | `gms.service.damage.cap` | 9007199254740992 |
| s9 | JVM 属性（可选） | `gms.damage.longLines` | false |

## 关键兼容点

- `Damage.h::legacy_line()`：legacy 4B/行写包前压回 INT_MAX。
- `DamageLinesConfig.clampToInt()`：仍为 int 的旧 API（血条包、脚本监听器、HP 操作、aggro 表）统一收窄。
- 梅索爆炸 4211006 的 `skip(12)` / `skip(4)` / 末尾金币堆块保持 r806 版式，只把逐行读取换成 12B。
- `summonAttack` 与 `damageMonster` HP 弹字未改（插件文档 §15 leftovers）。
- `0x3730` 沿用客户端既有值，s9 新增同值枚举；未重分配 opcode。
