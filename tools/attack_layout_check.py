#!/usr/bin/env python3
"""J7 出站攻击包对拍（双模式）：把服务端 parseDamage 的读法和客户端 AttackPacket
的写法都翻译成 Python，逐字节走一遍，证明两边不脱节。

模式（必须与 s9 gms.service.damage.long-lines / 客户端 damage.long_lines 一致）：
  legacy  每条伤害行 4 字节 int32                      （默认）
  long    每条伤害行 12 字节 = int32 vanillaLine + int64 真实值
          vanillaLine 溢出写 INT_MAX 哨兵、暴击置 0x80000000 位

依据：
  AbstractDealDamageHandler.java  parseDamage / readDamageLine
  PacketCreator.addAttackBody     S2C 广播
  Net/Packets/AttackAndSkillPackets.h  AttackPacket
  Gameplay/Combat/Damage.h        clamp_damage / vanilla_line

用法：
  python tools/attack_layout_check.py             # legacy 13/13
  python tools/attack_layout_check.py --long     # 12B/行（含超 INT_MAX 与 2^53 边界）
  python tools/attack_layout_check.py --legacy   # 修复前反向对照，应当出现 FAIL
"""
import struct
import sys

# ---- 服务端常量（org/gms/constants/skills/*.java）----
EVAN_ICE_BREATH = 22121000
EVAN_FIRE_BREATH = 22151001
FP_BIG_BANG = 2121001
IL_BIG_BANG = 2221001
BISHOP_BIG_BANG = 2321001
GRENADE = 5201002
CORKSCREW_BLOW = 5101004
TB_CORKSCREW_BLOW = 15101003
POISON_BOMB = 14111006
CHARGE_SKILLS = {EVAN_ICE_BREATH, EVAN_FIRE_BREATH, FP_BIG_BANG, IL_BIG_BANG,
                 BISHOP_BIG_BANG, GRENADE, CORKSCREW_BLOW, TB_CORKSCREW_BLOW, POISON_BOMB}

BOW_HURRICANE = 3121004
WIND_HURRICANE = 13111002
PIERCING_ARROW = 3221001
RAPID_FIRE = 5221004
SUSTAINED_SKILLS = {BOW_HURRICANE, WIND_HURRICANE, PIERCING_ARROW, RAPID_FIRE}

MESO_EXPLOSION = 4211006

INT32_MAX = 0x7FFFFFFF
EXACT_INT_CAP = 9007199254740992  # 2^53，double 精确整数上限


def line_value(line):
    """伤害行可以是 int，也可以是 (值, 是否暴击)。"""
    return line[0] if isinstance(line, tuple) else line


def line_critical(line):
    return line[1] if isinstance(line, tuple) else False


def vanilla_line(value, critical):
    """Gameplay/Combat/Damage.h::vanilla_line 的 Python 版。"""
    safe = max(0, min(value, EXACT_INT_CAP))
    base = INT32_MAX if safe > INT32_MAX else safe
    if critical:
        base |= 0x80000000
        if base >= 1 << 31:
            base -= 1 << 32
    return base


def clamp_legacy(value):
    return max(0, min(value, INT32_MAX))


class Reader:
    """模拟 s9 InPacket：读操作推进游标，越界即抛（等价于服务端读到脏数据/异常）。"""

    def __init__(self, data):
        self.data = data
        self.pos = 0

    def _take(self, n, fmt):
        if self.pos + n > len(self.data):
            raise IndexError("EOF at %d, need %d of %d" % (self.pos, n, len(self.data)))
        v = struct.unpack_from(fmt, self.data, self.pos)
        self.pos += n
        return v

    def read_byte(self):
        return self._take(1, "<b")[0]

    def read_short(self):
        return self._take(2, "<h")[0]

    def read_int(self):
        return self._take(4, "<i")[0]

    def read_long(self):
        return self._take(8, "<q")[0]

    def skip(self, n):
        if self.pos + n > len(self.data):
            raise IndexError("EOF(skip) at %d, need %d of %d" % (self.pos, n, len(self.data)))
        self.pos += n

    def left(self):
        return len(self.data) - self.pos


def read_damage_line(p, long_lines):
    """服务端 readDamageLine 的 Python 版；返回真实伤害（非负）。"""
    vanilla = p.read_int()
    if long_lines:
        real = p.read_long()
        if real <= 0 or real > EXACT_INT_CAP:
            real = vanilla & 0x7FFFFFFF
        return max(0, min(real, EXACT_INT_CAP))
    if vanilla < 0:
        return vanilla & 0x7FFFFFFF
    return max(0, vanilla)


def parse_damage(buf, ranged=False, long_lines=False):
    """服务端 parseDamage 的读法，只保留与字节布局有关的部分。"""
    p = Reader(buf)
    ret = dict(reader=p)
    p.read_byte()                                    # :686 占位
    nad = p.read_byte()                              # :687 Java byte，符号扩展成 int
    # Java 的 `numAttackedAndDamage >>> 4` 是无符号右移，必须按 32 位补码算。
    ret["num_attacked"] = ((nad & 0xFFFFFFFF) >> 4) & 0xF
    ret["num_damage"] = nad & 0xF
    ret["skill"] = p.read_int()                      # :691
    skill = ret["skill"]

    ret["charge"] = p.read_int() if skill in CHARGE_SKILLS else None   # :705-709

    p.skip(8)                                        # :711
    ret["display"] = p.read_byte()                   # :712
    ret["direction"] = p.read_byte()                 # :713
    ret["stance"] = p.read_byte()                    # :714

    if skill == MESO_EXPLOSION:                      # :715-750 专用分支
        if nad == 0:
            p.skip(10)
            bullets = p.read_byte()
            for _ in range(bullets):
                p.read_int()
                p.skip(1)
            ret["damage"] = []
            return ret
        p.skip(6)
        mobs = []
        for i in range(ret["num_attacked"] + 1):
            oid = p.read_int()
            if i < ret["num_attacked"]:
                p.skip(12)
                bullets = p.read_byte()
                dmgs = [read_damage_line(p, long_lines) for _ in range(bullets)]
                p.skip(4)
                mobs.append((oid, dmgs))
            else:
                bullets = p.read_byte()
                ret["stacks"] = bullets
                for _ in range(bullets):
                    p.read_int()
                    p.skip(1)
        ret["damage"] = mobs
        return ret

    if ranged:                                       # :751-759
        p.read_byte()
        ret["speed"] = p.read_byte()
        p.read_byte()
        ret["rangedirection"] = p.read_byte()
        p.skip(7)
        if skill in SUSTAINED_SKILLS:
            p.skip(4)
    else:                                            # :760-764
        p.read_byte()
        ret["speed"] = p.read_byte()
        p.skip(4)

    damage = []
    for _ in range(ret["num_attacked"]):             # :906-1041
        oid = p.read_int()
        p.skip(14)
        dmgs = [read_damage_line(p, long_lines) for _ in range(ret["num_damage"])]
        p.skip(4)                                    # :1037-1039 条件恒真
        damage.append((oid, dmgs))
    ret["damage"] = damage

    if skill == POISON_BOMB:                         # :1042-1045
        p.skip(4)
        ret["position"] = (p.read_short(), p.read_short())

    return ret


def build_packet(attack, long_lines=False):
    """客户端 AttackPacket 的写法，与 AttackAndSkillPackets.h 一一对应。"""
    out = bytearray()
    w = out.extend

    def pad(n):
        w(b"\x00" * n)

    def byte(v):
        # 与 OutPacket::write_byte(int8_t) 一致：超界按 8 位截断回绕。
        w(struct.pack("<b", ((v + 128) % 256) - 128))

    def short(v):
        w(struct.pack("<h", v))

    def i32(v):
        w(struct.pack("<i", v))

    def i64(v):
        w(struct.pack("<q", v))

    def damage_line(line):
        value = line_value(line)
        critical = line_critical(line)
        if long_lines:
            i32(vanilla_line(value, critical))
            i64(value)
        else:
            i32(clamp_legacy(value))

    charge = attack.get("charge", 0)
    # Combat::apply_move 把候选怪钳到 15（协议高 4 位上限），所以发出去的怪物块不会超过 15 个。
    lines = attack["damage"][:min(attack["mobcount"], 15)]
    mobcount = len(lines)
    hitcount = attack["hitcount"]

    pad(1)
    byte((mobcount << 4) | (hitcount & 0xF))
    i32(attack["skill"])
    if attack["skill"] in CHARGE_SKILLS:
        i32(charge)
    pad(8)
    byte(attack.get("display", 0))
    byte(1 if attack.get("toleft") else 0)
    byte(attack.get("stance", 0))
    pad(1)
    byte(attack.get("speed", 0))

    if attack["ranged"]:
        pad(1)
        byte(1 if attack.get("toleft") else 0)
        pad(7)
        if attack["skill"] in SUSTAINED_SKILLS:
            pad(4)
    else:
        pad(4)

    if attack["skill"] == MESO_EXPLOSION:
        for oid, dmgs in lines:
            i32(oid)
            pad(12)
            byte(len(dmgs))
            for d in dmgs:
                damage_line(d)
            pad(4)
        i32(0)      # 服务端多读的那一块：oid(int) + 金币堆数(byte)
        byte(0)
    else:
        for oid, dmgs in lines:
            i32(oid)
            pad(14)
            for d in dmgs:
                damage_line(d)
            pad(4)
        if attack["skill"] == POISON_BOMB:
            pad(4)
            short(attack["origin"][0])
            short(attack["origin"][1])
    return bytes(out)


CASES = [
    ("普通近战 1怪1击", dict(ranged=False, mobcount=1, hitcount=1, skill=0,
                         damage=[(7, [432])])),
    ("近战 3怪6击", dict(ranged=False, mobcount=3, hitcount=6, skill=11000001,
                      damage=[(7, [1] * 6), (8, [2] * 6), (9, [3] * 6)])),
    ("BIG_BANG 充能技能", dict(ranged=False, mobcount=2, hitcount=8, skill=BISHOP_BIG_BANG,
                           charge=120, damage=[(7, [1] * 8), (8, [2] * 8)])),
    ("GRENADE 充能远程", dict(ranged=True, mobcount=1, hitcount=4, skill=GRENADE,
                          charge=70, damage=[(7, [5] * 4)])),
    ("POISON_BOMB 充能+落点", dict(ranged=False, mobcount=2, hitcount=6, skill=POISON_BOMB,
                              charge=255, origin=(10125, -240),
                              damage=[(7, [1] * 6), (8, [2] * 6)])),
    ("普通远程 2怪4击", dict(ranged=True, mobcount=2, hitcount=4, skill=31000002,
                        damage=[(7, [1] * 4), (8, [2] * 4)])),
    ("飓风 持续技 3怪12击", dict(ranged=True, mobcount=3, hitcount=12, skill=BOW_HURRICANE,
                            damage=[(7, [1] * 12), (8, [2] * 12), (9, [3] * 12)])),
    ("穿透箭 持续技 1怪1击", dict(ranged=True, mobcount=1, hitcount=1, skill=PIERCING_ARROW,
                            damage=[(7, [900])])),
    ("战神 2怪5击", dict(ranged=False, mobcount=2, hitcount=5, skill=21110002,
                     damage=[(7, [1] * 5), (8, [2] * 5)])),
    ("暴怒 2怪3击+0金币堆", dict(ranged=False, mobcount=2, hitcount=3, skill=MESO_EXPLOSION,
                            damage=[(7, [1] * 3), (8, [2] * 3)])),
    ("暴怒 未命中怪+0金币堆", dict(ranged=False, mobcount=0, hitcount=6, skill=MESO_EXPLOSION,
                             damage=[])),
    # Skill.nx 实测：4211006 的 attackCount 在 lv16-30 是 16/18/20，只有它超过 15。
    ("暴怒 lv20 段数20 2怪", dict(ranged=False, mobcount=2, hitcount=20, skill=MESO_EXPLOSION,
                            damage=[(7, [1] * 20), (8, [2] * 20)])),
    # Skill.nx 实测：竹林 0001009/10001009/20001009 的 mobCount = 30，超过高 4 位上限，
    # 客户端按协议钳到 15 只。
    ("竹林 30怪（钳到15）", dict(ranged=False, mobcount=30, hitcount=1, skill=10001009,
                           damage=[(10 + i, [i]) for i in range(30)])),
]


# 12 字节/行专用用例：超过 INT_MAX 的真实伤害、2^53 边界、暴击符号位、
# 以及梅索爆炸/竹林等专用版式在长行下是否仍被服务端整包读满。
LONG_CASES = [
    ("long 普通近战 1怪1击", dict(ranged=False, mobcount=1, hitcount=1, skill=0,
                              damage=[(7, [432])])),
    ("long 超 INT_MAX 单段", dict(ranged=False, mobcount=1, hitcount=2, skill=0,
                              damage=[(7, [2147483648, 5000000000])])),
    ("long 2^53 精度上限", dict(ranged=False, mobcount=1, hitcount=2, skill=0,
                            damage=[(7, [EXACT_INT_CAP, EXACT_INT_CAP - 1])])),
    ("long 暴击符号位", dict(ranged=False, mobcount=2, hitcount=2, skill=0,
                        damage=[(7, [(123456, True), (9876543210, True)]),
                                (8, [(999, False), (2147483647, True)])])),
    ("long 远程 3怪6击", dict(ranged=True, mobcount=3, hitcount=6, skill=31000002,
                          damage=[(7, [2 ** 33] * 6), (8, [1] * 6), (9, [EXACT_INT_CAP] * 6)])),
    ("long 充能+落点", dict(ranged=False, mobcount=2, hitcount=6, skill=POISON_BOMB,
                        charge=255, origin=(10125, -240),
                        damage=[(7, [3 ** 30] * 6), (8, [(5 ** 20, True)] * 6)])),
    ("long 暴怒 2怪3击", dict(ranged=False, mobcount=2, hitcount=3, skill=MESO_EXPLOSION,
                          damage=[(7, [2 ** 40] * 3), (8, [(7, True)] * 3)])),
    ("long 暴怒 lv20 段数20", dict(ranged=False, mobcount=2, hitcount=20, skill=MESO_EXPLOSION,
                               damage=[(7, [2 ** 35] * 20), (8, [1] * 20)])),
    ("long 竹林 30怪钳到15", dict(ranged=False, mobcount=30, hitcount=1, skill=10001009,
                             damage=[(10 + i, [i * 10 ** 12]) for i in range(30)])),
]


def build_packet_legacy(attack):
    """修复前的写法（保留在这里做反向对照：`--legacy` 应当看到 FAIL）。

    两处偏差：charge 用「值是否为 0」而不是「技能是否在表里」判断（真实客户端 charge 恒 0），
    持续技不补 4 字节，每个目标尾部的 4 字节按 skill 有条件跳过，暴怒没有专用版式。
    """
    out = bytearray()
    w = out.extend

    def pad(n):
        w(b"\x00" * n)

    def byte(v):
        # 与 OutPacket::write_byte(int8_t) 一致：超界按 8 位截断回绕。
        w(struct.pack("<b", ((v + 128) % 256) - 128))

    def i32(v):
        w(struct.pack("<i", v))

    charge = 0
    pad(1)
    byte((attack["mobcount"] << 4) | attack["hitcount"])
    i32(attack["skill"])
    if charge > 0:
        i32(charge)
    pad(8)
    byte(0)
    byte(1 if attack.get("toleft") else 0)
    byte(attack.get("stance", 0))
    pad(1)
    byte(attack.get("speed", 0))

    if attack["ranged"]:
        pad(1)
        byte(1 if attack.get("toleft") else 0)
        pad(7)
    else:
        pad(4)

    for oid, dmgs in attack["damage"]:
        i32(oid)
        pad(14)
        for d in dmgs:
            i32(d)
        if attack["skill"] != RAPID_FIRE:
            pad(4)
    return bytes(out)


def check(name, attack, builder, long_lines=False):
    if builder is build_packet:
        buf = builder(attack, long_lines)
    else:
        buf = builder(attack)
    try:
        info = parse_damage(buf, ranged=attack["ranged"], long_lines=long_lines)
    except IndexError as e:
        return ["服务端读不完：%s" % e]

    problems = []
    raw_lines = attack["damage"][:min(attack["mobcount"], 15)]
    if long_lines:
        want_lines = [(oid, [min(line_value(d), EXACT_INT_CAP) for d in dmgs])
                      for oid, dmgs in raw_lines]
    elif builder is build_packet:
        want_lines = [(oid, [clamp_legacy(line_value(d)) for d in dmgs])
                      for oid, dmgs in raw_lines]
    else:
        want_lines = raw_lines
    want_mobcount = len(want_lines)
    if info["reader"].left() != 0:
        problems.append("尾部剩 %d 字节" % info["reader"].left())
    if info["skill"] != attack["skill"]:
        problems.append("skill=%d" % info["skill"])
    if info["num_attacked"] != want_mobcount:
        problems.append("numAttacked=%d（应为 %d）" % (info["num_attacked"], want_mobcount))
    if attack["skill"] != MESO_EXPLOSION and info["num_damage"] != (attack["hitcount"] & 0xF):
        problems.append("numDamage=%d" % info["num_damage"])
    if info["damage"] != want_lines:
        problems.append("伤害行不匹配：%s" % (info["damage"],))
    if attack["skill"] in CHARGE_SKILLS and info["charge"] != attack.get("charge", 0):
        problems.append("charge=%s" % info["charge"])
    if "position" in info and tuple(info["position"]) != tuple(attack.get("origin", (0, 0))):
        problems.append("position=%s" % (info["position"],))
    if "stacks" in info and info["stacks"] != 0:
        problems.append("金币堆数=%s" % info["stacks"])
    return problems


def main():
    legacy_reverse = "--legacy" in sys.argv
    long_lines = "--long" in sys.argv
    builder = build_packet_legacy if legacy_reverse else build_packet
    cases = CASES if legacy_reverse else (LONG_CASES if long_lines else CASES)
    label = "修复前(legacy)" if legacy_reverse else ("12字节/行" if long_lines else "legacy 4字节/行")
    fails = 0

    for name, attack in cases:
        problems = check(name, attack, builder, long_lines=long_lines and not legacy_reverse)
        buf = builder(attack, long_lines) if builder is build_packet else builder(attack)
        size = len(buf)
        if problems:
            print("FAIL %-24s %s" % (name, "; ".join(problems)))
            fails += 1
        else:
            print("ok   %-24s %3d 字节，服务端整包读满" % (name, size))

    print("\n[%s] %d/%d 用例异常" % (label, fails, len(cases)))
    if legacy_reverse:
        print("EXPECTED FAILURES (legacy reverse control)")
    else:
        print("ALL LAYOUTS ALIGN" if fails == 0 else "%d CASE(S) BROKEN" % fails)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
