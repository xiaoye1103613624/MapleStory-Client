#!/usr/bin/env python3
"""伤害公式 / 上限边界对拍（客户端 ↔ s9 ↔ 原生端）。

依据：
  Gameplay/Combat/Damage.h                      clamp_damage / vanilla_line / legacy_line
  Gameplay/MapleMap/Mob.cpp                     next_damage（唯一 clamp 点）
  Character/CharStats.cpp                       close_totalstats
  s9 Character.calculateMaxBaseDamage()        ceil(((weaponMult*主属+副属)/100)*watk)
  s9 AbstractDealDamageHandler.parseDamage()   旧 hitDmgMax / maxWithCrit / SNIPE / 竹林
  s9 DamageLinesConfig                         cap / clampDamage / vanillaLine
  IDA：BeiDou.exe dbl_AFE8A0 = 199999.0（原生单段硬上限）

用法：
  python tools/damage_formula_check.py
  python tools/damage_formula_check.py --out <归档文本>
"""
import math
import struct
import sys

INT32_MAX = 0x7FFFFFFF
EXACT_INT_CAP = 9007199254740992          # 2^53
NATIVE_CAP = 199999                       # 原生 BeiDou.exe 硬封顶
SNIPE_MAX = 199999                        # 195000 + rand(5000)
BAMBOO_RAIN_MAX = 82569000                # 道场 Boss 30% HP


def client_base_damage(weapon_mult, primary, secondary, watk):
    """客户端 CharStats::close_totalstats（未开 v2 时）。"""
    return (primary * weapon_mult + secondary) * (watk / 100.0)


def s9_calculate_max_base_damage(weapon_mult, primary, secondary, watk):
    """s9 Character.calculateMaxBaseDamage：ceil(((wm*主属+副属)/100)*watk)。"""
    return math.ceil((weapon_mult * primary + secondary) / 100.0 * watk)


def clamp_damage(raw, cap=EXACT_INT_CAP, limit_break=-1):
    """Damage.h::effective_cap + clamp_damage 的 Python 版。"""
    eff = min(cap if cap > 0 else EXACT_INT_CAP, EXACT_INT_CAP)
    if limit_break > 0:
        eff = min(eff, limit_break)
    if math.isnan(raw) or raw <= 0:
        return 0
    if raw >= EXACT_INT_CAP or math.isinf(raw):
        return eff
    return min(int(raw), eff)


def vanilla_line(value, critical):
    safe = max(0, min(value, EXACT_INT_CAP))
    base = INT32_MAX if safe > INT32_MAX else safe
    if critical:
        base |= 0x80000000
        if base >= 1 << 31:
            base -= 1 << 32
    return base


def encode_12b(value, critical):
    return struct.pack("<iq", vanilla_line(value, critical), value)


def decode_12b(buf):
    vanilla, real = struct.unpack("<iq", buf)
    critical = vanilla < 0
    if real <= 0 or real > EXACT_INT_CAP:
        real = vanilla & 0x7FFFFFFF
    return real, critical


def legacy_line(value):
    return max(0, min(value, INT32_MAX))


CASES = [
    # (名称, 原始伤害, 配置 cap, 破功值, 期望 clamp 结果, 期望 vanilla)
    ("普通伤害不截断", 432, EXACT_INT_CAP, -1, 432, 432),
    ("原 999999 封顶处", 999999, EXACT_INT_CAP, -1, 999999, 999999),
    ("原生 199999 处已放开", NATIVE_CAP + 1, EXACT_INT_CAP, -1, NATIVE_CAP + 1, NATIVE_CAP + 1),
    ("SNIPE 旧上限之上", SNIPE_MAX + 5000, EXACT_INT_CAP, -1, SNIPE_MAX + 5000, SNIPE_MAX + 5000),
    ("竹林 82569000", BAMBOO_RAIN_MAX, EXACT_INT_CAP, -1, BAMBOO_RAIN_MAX, BAMBOO_RAIN_MAX),
    ("INT_MAX 边界", INT32_MAX, EXACT_INT_CAP, -1, INT32_MAX, INT32_MAX),
    ("INT_MAX+1 不再回绕", INT32_MAX + 1, EXACT_INT_CAP, -1, INT32_MAX + 1, INT32_MAX),
    ("超 21 亿", 5000000000, EXACT_INT_CAP, -1, 5000000000, INT32_MAX),
    ("2^53 精确上限", float(EXACT_INT_CAP), EXACT_INT_CAP, -1, EXACT_INT_CAP, INT32_MAX),
    ("超过 2^53 饱和", float(EXACT_INT_CAP) * 2, EXACT_INT_CAP, -1, EXACT_INT_CAP, INT32_MAX),
    ("配置 cap 生效", 10 ** 12, 10 ** 9, -1, 10 ** 9, 10 ** 9),
    ("破功值 199999 生效", 10 ** 12, EXACT_INT_CAP, 199999, 199999, 199999),
    ("破功值 long 生效", 10 ** 12, EXACT_INT_CAP, 10 ** 11, 10 ** 11, INT32_MAX),
    ("负数/零归零", -12345, EXACT_INT_CAP, -1, 0, 0),
]


def main():
    out_path = None
    if "--out" in sys.argv:
        out_path = sys.argv[sys.argv.index("--out") + 1]

    lines = []
    fails = 0

    def emit(text):
        print(text)
        lines.append(text)

    emit("伤害公式 / 上限边界对拍（客户端 vs s9 vs 原生端）")
    emit("=" * 72)

    # 1. 基础公式同构性（客户端 close_totalstats vs s9 calculateMaxBaseDamage）
    emit("\n[1] 物理基础值：客户端 (主属*倍率+副属)*WATK/100  vs  s9 ceil(...*watk)")
    for wm, primary, secondary, watk in [(4.0, 300, 60, 120), (4.8, 500, 80, 300),
                                         (3.6, 120, 40, 80)]:
        a = client_base_damage(wm, primary, secondary, watk)
        b = s9_calculate_max_base_damage(wm, primary, secondary, watk)
        ok = abs(a - b) <= 1.0
        emit("  %s wm=%.1f 主属=%d 副属=%d WATK=%d -> client=%.1f s9=%d" %
             ("ok  " if ok else "FAIL", wm, primary, secondary, watk, a, b))
        if not ok:
            fails += 1

    # 2. clamp 与 12 字节契约
    emit("\n[2] clamp / vanillaLine / 12 字节往返")
    for name, raw, cap, lb, want, want_vanilla in CASES:
        got = clamp_damage(raw, cap, lb)
        got_vanilla = vanilla_line(got, False)
        ok = got == want and got_vanilla == want_vanilla
        emit("  %s %-24s raw=%-22s cap=%-18s lb=%-14s -> %d (vanilla=%d)" %
             ("ok  " if ok else "FAIL", name, raw, cap, lb, got, got_vanilla))
        if not ok:
            fails += 1

        # 12 字节编解码必须无损；暴击位只影响 vanilla，不影响真实值。
        for critical in (False, True):
            real, crit = decode_12b(encode_12b(got, critical))
            if real != got or crit != critical:
                emit("      FAIL 往返: real=%d crit=%s（期望 %d/%s）" %
                     (real, crit, got, critical))
                fails += 1

    # 3. 暴击符号位不落在真实值字段
    emit("\n[3] 暴击位契约")
    for value in (0, 1, 432, INT32_MAX, INT32_MAX + 1, 10 ** 12, EXACT_INT_CAP):
        raw = encode_12b(value, True)
        v, real = struct.unpack("<iq", raw)
        ok = v < 0 and real == value
        emit("  %s value=%-18d vanilla=%-12d real=%d" % ("ok  " if ok else "FAIL", value, v, real))
        if not ok:
            fails += 1

    # 4. legacy 4 字节/行仍压回 int32，避免溢出被误判为暴击
    emit("\n[4] legacy 4 字节/行")
    for value, want in [(0, 0), (432, 432), (INT32_MAX, INT32_MAX),
                        (INT32_MAX + 1, INT32_MAX), (10 ** 12, INT32_MAX),
                        (EXACT_INT_CAP, INT32_MAX)]:
        got = legacy_line(value)
        ok = got == want and got >= 0
        emit("  %s value=%-18d -> %d" % ("ok  " if ok else "FAIL", value, got))
        if not ok:
            fails += 1

    emit("\n%s：%d 项异常" % ("ALL DAMAGE CHECKS ALIGN" if fails == 0 else "FAILED", fails))

    if out_path:
        with open(out_path, "w", encoding="utf-8") as f:
            f.write("\n".join(lines) + "\n")

    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
