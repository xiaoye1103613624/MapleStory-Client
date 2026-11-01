# r806 · J7 出站攻击包：版式对拍与 5 处位移修复

对应任务 #74（J7 出站封包：charge / sustained skip / 每目标计数 byte）。
唯一权威依据是服务端的**顺序读**实现：
`E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\net\server\channel\handlers\AbstractDealDamageHandler.java:681-1047`
（`parseDamage`），三个 handler 各自入口：
`CloseRangeDamageHandler.java:65` → `parseDamage(p, chr, false, false)`、
`RangedAttackHandler` → `(p, chr, true, false)`、`MagicDamageHandler.java:53` → `(p, chr, false, true)`。
客户端侧只有 `Net/Packets/AttackAndSkillPackets.h` 的 `AttackPacket` 一个写点。

> 这类包的性质：`parseDamage` 全程定长顺序读。前面任何一处多读/少读 N 字节，
> `display / direction / stance / speed` 和**整段伤害行**都会整体位移 N，
> 服务端拿到的是垃圾 oid 与垃圾伤害；`applyAttack` 的 `getMonsterByOid(垃圾)` 返回 null，
> 结果就是「本地看到挥了、怪掉不了血」，而且 `AutobanFactory` 还可能记 DAMAGE_HACK。

## 已核对为正确的部分（不改）

- 近战/魔法前缀：`byte 占位 + byte numAttackedAndDamage + int skill`，
  `skip(8)`，`display / direction(=客户端 toleft) / stance` ✓
- 远程中段：`byte + speed + byte + rangedirection + skip(7)` 共 11 字节 ✓
  （客户端 `skip(1) + write_byte(speed) + skip(1) + write_byte(toleft) + skip(7)`）
- 近战/魔法中段：`byte + speed + skip(4)` 共 6 字节 ✓
- `Mob::calculate_damage` 用 `attack.hitcount` 生成伤害向量，和包头低 4 位天然一致 ✓
- `stance` 字节的朝向语义（服务端 `Math.abs(stance) % 2 == 1` 判左）与
  `user.get_look().get_stance()`（STAND1/WALK1 偶=右，STAND2/WALK2 奇=左）✓

## 修复 1：charge 字段按**技能 id**判定，不按「有没有充能值」

服务端 `:705-709` 只在技能命中这张固定表时才 `p.readInt()`：

| 技能 | id |
| --- | --- |
| Evan.ICE_BREATH | 22121000 |
| Evan.FIRE_BREATH | 22151001 |
| FPArchMage.BIG_BANG | 2121001 |
| ILArchMage.BIG_BANG | 2221001 |
| Bishop.BIG_BANG | 2321001 |
| Gunslinger.GRENADE | 5201002 |
| Brawler.CORKSCREW_BLOW | 5101004 |
| ThunderBreaker.CORKSCREW_BLOW | 15101003 |
| NightWalker.POISON_BOMB | 14111006 |

旧代码是 `if (attack.charge > 0) write_int(attack.charge);`，而
`AttackResult::charge` **全工程没有任何赋值点**（grep 只命中声明），恒为 0 →
这 9 个技能一个字节都不发，服务端却照读 4 字节 → 后面全体前移 4 字节。
现在改成 `has_charge_field(skill)`（表照抄上表）。
充能**数值**仍然发 0：服务端只把它用于 `PacketCreator.magicAttack` 的广播显示
（`MagicDamageHandler.java:68`），不参与伤害计算，所以先发 0 不改变伤害，只补齐版式。

## 修复 2：远程持续/贯穿技补上第 4 段 4 字节

`:757-759` 在固定 `skip(7)` 之后还会对这 4 个技能 `skip(4)`：
Bowmaster.HURRICANE `3121004`、WindArcher.HURRICANE `13111002`、
Marksman.PIERCING_ARROW `3221001`、Corsair.RAPID_FIRE `5221004`。
旧代码留的是 `// TODO: skip(4); If hurricane, piercing arrow or rapidfire.`——没实现，
所以这三个技能发包时少 4 字节，**下一目标的 oid 从错位处读起**。现在由
`has_sustained_field(skill)` 补 `skip(4)`。

## 修复 3：每个目标尾部的 4 字节改成无条件

`:1037-1039`：

```java
if (ret.skill != Corsair.RAPID_FIRE || ret.skill != Aran.HIDDEN_FULL_DOUBLE
 || ret.skill != Aran.HIDDEN_FULL_TRIPLE || ret.skill != Aran.HIDDEN_OVER_DOUBLE
 || ret.skill != Aran.HIDDEN_OVER_TRIPLE) {
    p.skip(4);
}
```

这是 `a != x || a != y || ...` 的恒真式（and 写成 or），服务端**每个目标后面都会 skip(4)**。
旧客户端写成 `if (attack.skill != 5221004) skip(4);`，等于速射（RAPID_FIRE 5221004）
发包时少 4 字节。现在无条件 `skip(4)`，并把恒真式的证据记在注释里——
不能因为「服务端有这个条件」就去对齐条件，要对齐它的**实际行为**。

## 修复 4：暴怒 MESO_EXPLOSION(4211006) 走专用版式

`:715-750` 给 4211006 单开一条读法，和普通攻击的每目标块**长度不同**：

| 普通攻击每目标 | 暴怒每目标 |
| --- | --- |
| `int oid` + `skip(14)` + numDamage×`int` + `skip(4)` | `int oid` + `skip(12)` + **`byte 段数`** + 段数×`int` + `skip(4)` |

而且循环是 `for (i = 0; i < numAttacked + 1; i++)`——**多一块**，最后一块是
「被引爆的金币堆」列表：`int oid` + `byte 堆数` + 堆数×(`int dropoid` + `skip(1)`)。

现在 `AttackPacket` 为 4211006 单开分支：按上表写每目标块（段数用真实的伤害行数），
末尾固定写 `int 0 + byte 0`（0 个金币堆）。

**为什么金币堆写 0（这是本版的功能边界，不是遗漏）**：金币堆的 drop oid 由服务端持有，
本客户端只按 `SPAWN_DROP` 渲染掉落物，没有「哪些金币堆属于我、还在技能范围内」的可选集合，
硬造 oid 会让服务端第一个校验循环（`:210` 起，要求 allDamage 的 key 必须是 ITEM）
直接 `return`，反而把伤害全废掉。所以：
**暴怒现在能正常结算对范围内怪物的伤害，但不会引爆地上的金币**。
要完整引爆需要另开一条线（掉落物归属 + 选堆），已列入待确认项。

## 修复 5：numAttackedAndDamage 两个 4 位段的越界

包头字节是 `(怪物数 << 4) | 段数`，两段各只有 0-15。用 `tools/nx_inspect.py` 的图遍历
对部署包 `E:/MXD/BeiDou-Client-xy/Data/.nxcache/Skill.nx` 全量扫了一遍：

- `attackCount > 15` 的**只有** `421.img/skill/4211006`（lv16-30 = 16/18/20）；
  其余命名空间最大 8（512.img）、6（1511.img/420.img）、4……
- `mobCount > 15` 的只有竹林三兄弟 `000.img/0001009`、`1000.img/10001009`、
  `2000.img/20001009`，值都是 **30**。

于是两处越界都是真实可达的：

1. 段数 20 的 bit4 会灌进高 4 位（`mobcount=2` 时 `(2<<4)|20 = 0x34` → 服务端读成 3 只怪）；
   暴怒走专用分支，低 4 位本来就被服务端忽略，所以包头**低段做 `& 0xF` 掩码**即可。
2. 怪物数 30 → `(30 << 4) & 0xFF = 0xE0`… 实际写出的字节是 `(30<<4)|hitcount` 截断成
   `0xE1`，Java `(-31) >>> 4 & 0xF` = **14**，既不是 30 也不是 15，服务端只读 14 个块，
   而客户端写了 30 个 → 尾部多 16 个块（对拍脚本里就是这个 FAIL）。
   协议格式的上限就是 15，所以在 `Combat::apply_move` 里把候选怪**钳到 15**
   （`find_closest` 的 objcount 参数），让「声明的怪物数」和「写出的块数」天然相等。

## 对拍脚本（新增产物）

`tools/attack_layout_check.py`：把 `parseDamage` 的读法和 `AttackPacket` 的写法都翻译成
Python，逐字节喂一遍，校验「服务端整包读满 + 每个 oid/伤害/charge/落点都对得上」。

```
python tools/attack_layout_check.py            # 修复后：13/13 ok，退出码 0
python tools/attack_layout_check.py --legacy   # 修复前：9/13 FAIL（反向对照）
```

`--legacy` 里保留的就是上面 5 处的旧写法，用来证明这些不是「为改而改」：
BIG_BANG / GRENADE / POISON_BOMB / 飓风 / 穿透箭 / 暴怒(3 个用例) / 竹林 全部读不完或读错位。

脚本内也照 Java 语义处理了两个坑：`readByte()` 赋给 int 是**符号扩展**，
`>>>` 必须按 32 位补码算；`write_byte` 越界按 8 位截断回绕。

## 构建

`cmd //c "tools\\build_smoke.bat"` → `GATE OK: opcode matrix matches both sources (s9 44d0d5d9ca)`
/ `GATE OK` / `EXITCODE=0`（`AttackAndSkillPackets.h` 行号变了，先跑
`python tools/gen_opcode_matrix.py` 再构建）。

## 需要真机确认（并入 #75 的统一验证清单）

1. 主教/火毒/冰雷 `BIG_BANG`、夜行 `POISON_BOMB`、重炮 `GRENADE`、拳手 `CORKSCREW_BLOW`
   命中怪物后**能正常掉血**（修复前这 9 个技能的包全部错位）。
2. 弩弓/游侠 `飓风`、神射手 `穿透箭`、海盗 `速射` 打 2 只以上的怪时，
   **每只怪都掉血**（修复前只有第一只有机会结算）。
3. -bandit `暴怒`：对怪能掉血，但**不会引爆地上金币**（本版边界，见修复 4）。
4. 道场 `竹林`：一次攻击最多结算 15 只怪（协议上限），不再出现「一只不掉」。
