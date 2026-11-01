#!/usr/bin/env python3
"""J7 出站攻击包对拍：把服务端 parseDamage 的读法和客户端 AttackPacket 的写法
都翻译成 Python，逐字节走一遍，证明两边不脱节。

依据（只读，不改服务端）：
  AbstractDealDamageHandler.java:681-1047  parseDamage
  Net/Packets/AttackAndSkillPackets.h      AttackPacket

用法：python tools/attack_layout_check.py
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

    def skip(self, n):
        if self.pos + n > len(self.data):
            raise IndexError("EOF(skip) at %d, need %d of %d" % (self.pos, n, len(self.data)))
        self.pos += n

    def left(self):
        return len(self.data) - self.pos


def parse_damage(buf, ranged=False):
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
                dmgs = [p.read_int() for _ in range(bullets)]
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
        dmgs = [p.read_int() for _ in range(ret["num_damage"])]
        p.skip(4)                                    # :1037-1039 条件恒真
        damage.append((oid, dmgs))
    ret["damage"] = damage

    if skill == POISON_BOMB:                         # :1042-1045
        p.skip(4)
        ret["position"] = (p.read_short(), p.read_short())

    return ret


def build_packet(attack):
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
                i32(d)
            pad(4)
        i32(0)      # 服务端多读的那一块：oid(int) + 金币堆数(byte)
        byte(0)
    else:
        for oid, dmgs in lines:
            i32(oid)
            pad(14)
            for d in dmgs:
                i32(d)
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


def check(name, attack, builder):
    buf = builder(attack)
    try:
        info = parse_damage(buf, ranged=attack["ranged"])
    except IndexError as e:
        return ["服务端读不完：%s" % e]

    problems = []
    want_lines = attack["damage"][:min(attack["mobcount"], 15)]
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
    builder = build_packet_legacy if "--legacy" in sys.argv else build_packet
    label = "修复前" if builder is build_packet_legacy else "修复后"
    fails = 0
    for name, attack in CASES:
        problems = check(name, attack, builder)
        size = len(builder(attack))
        if problems:
            print("FAIL %-24s %s" % (name, "; ".join(problems)))
            fails += 1
        else:
            print("ok   %-24s %3d 字节，服务端整包读满" % (name, size))

    print("\n[%s] %d/%d 用例异常" % (label, fails, len(CASES)))
    print("ALL LAYOUTS ALIGN" if fails == 0 else "%d CASE(S) BROKEN" % fails)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
