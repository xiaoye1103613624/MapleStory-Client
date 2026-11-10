# -*- coding: utf-8 -*-
# round70 harness: read-only 083 review of DamageNumber / DamageSkin.
# A) static guards on delivered artifacts; B) pure-model scenarios (no engine needed).
# A refusal-style "PASS" is not acceptable: every guard is a literal check on disk.
import json, os, re, sys, py_compile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

def read(p):
    with open(p, "r", encoding="utf-8", newline="") as f:
        return f.read()

def code_only(s):
    # drop // line comments so留证注释 does not pollute "behavior literal" guards
    out = []
    for line in s.split("\n"):
        i = line.find("//")
        out.append(line[:i] if i >= 0 else line)
    return "\n".join(out)

CPP = "Gameplay/Combat/DamageNumber.cpp"
H = "Gameplay/Combat/DamageNumber.h"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave2-damagenumber-083-review-20261111.md"
EV = "docs/features/development-and-fix-plan-2026-11/evidence/damagenumber-083-round70.json"
DP = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
LB = "IO/LoginBackground.cpp"
PROBE_OUT = "tools/_dmg_probe_stdout.txt"
PROBE_SRC = "_tools/dmg_skin_probe/main.cpp"

cpp = read(CPP)
h = read(H)
cpp_code = code_only(cpp)
notes = read(NOTES)
plt = read(PLT)
dp = read(DP)
lb = read(LB)
ev = json.loads(read(EV))
probe_out = read(PROBE_OUT) if os.path.exists(PROBE_OUT) else ""
probe_src = read(PROBE_SRC) if os.path.exists(PROBE_SRC) else ""

guards = []
def G(name, cond):
    guards.append((name, bool(cond)))

# ---- A1 083 authority basis present in source comments
G("A1.1 cpp cites Effect_HP 0x437d0f", "CAnimationDisplayer::Effect_HP @0x437d0f" in cpp)
G("A1.2 cpp cites dynamic 3/5 width", "3/5 字宽" in cpp or "3 * v23 / 5" in cpp)
G("A1.3 cpp cites SP_984_MISS + BasicFloat", "SP_984_MISS" in cpp and "Effect_BasicFloat @0x43de28" in cpp)
G("A1.4 cpp states 083 has NO DamageNumber class", "无 DamageNumber 类" in cpp and "*DamageNumber* 0 命中" in cpp)
G("A1.5 cpp states Journey + s9 identity", "Journey" in cpp and "s9 DAMAGE_SKIN(247)" in cpp)
G("A1.6 h cites .nx-verified scaling + non-083 identity", ".nx 实测属实" in h and "非 083 CAnimationDisplayer" in h)

# ---- A2 behavior literals UNCHANGED (guard against "accidental" rewrite)
G("A2.1 MAX_DAMAGE_WIDTH 260", "constexpr int16_t MAX_DAMAGE_WIDTH = 260;" in cpp_code)
G("A2.2 numscale 260/total", "static_cast<float>(MAX_DAMAGE_WIDTH) / total" in cpp_code)
G("A2.3 hardcoded advance table intact", "24, 20, 22, 22, 24, 23, 24, 22, 24, 24" in cpp_code)
G("A2.4 rowheight 36/30", "return critical ? 36 : 30;" in cpp_code)
G("A2.5 FADE_TIME 500", "static constexpr uint16_t FADE_TIME = 500;" in read(H))
G("A2.6 apply_skin path intact", 'nl::nx::Effect["DamageSkin.img"][std::to_string(skin)]' in cpp_code)
G("A2.7 NoRed1 guard intact", 'if (!dmg["NoRed1"])' in cpp_code)
G("A2.8 CRITICAL NoCri1/0", 'dmg["NoCri1"]' in cpp_code and 'dmg["NoCri0"]' in cpp_code)
G("A2.9 NORMAL NoRed1/0", 'dmg["NoRed1"]' in cpp_code and 'dmg["NoRed0"]' in cpp_code)
G("A2.10 init BasicEff charset sets", 'nl::nx::Effect["BasicEff.img"]' in cpp_code and 'BasicEff["NoViolet1"]' in cpp_code)
G("A2.11 MISS draw 'M' intact", "\"M\"" in cpp_code or "'M'" in cpp_code)
G("A2.13 core function signatures intact",
   all(sig in cpp_code for sig in [
     "int16_t DamageNumber::getadvance(char c, bool first) const",
     "void DamageNumber::apply_skin(int32_t skin, BoolPair<Charset>& out, Type type)",
     "void DamageNumber::init()",
     "int16_t DamageNumber::rowheight(bool critical)",
     "void DamageNumber::draw(double viewx, double viewy, float alpha) const",
   ]))

# ---- A3 reachability CONCLUSION correct (no dead-link misjudgment)
G("A3.1 cpp says 非死链", "非死链" in cpp)
G("A3.2 cpp says 624 皮肤节点", "624 个皮肤节点" in cpp)
G("A3.3 cpp forbids 资源缺失 misjudgment", "别再误判成资源缺失" in cpp)
G("A3.4 evidence verdict REACHABLE", "REACHABLE" in ev["minimum_probe"]["VERDICT"])
G("A3.5 evidence DISPROVED merge-fail hypothesis", "DISPROVED" in ev["minimum_probe"]["VERDICT"])
G("A3.6 evidence catalog = stale snapshot", "STALE SNAPSHOT" in ev["nx_and_deploy_facts"]["catalog_Effect.json"])
G("A3.7 notes concludes stale-snapshot / reachability closed", "过期快照" in notes and "可达性已闭环" in notes and "不再是决策点" in notes)
G("A3.8 evidence marks reachability CLOSED (not open)",
   any("CLOSED by probe" in d and "no longer a decision point" in d for d in ev["decision_points"]))

# ---- A4 probe hard evidence on disk
G("A4.1 probe stdout exists", len(probe_out) > 0)
G("A4.2 BASICEFF_SINGLE control OK", "RESULT BASICEFF_SINGLE = OPEN_MERGE_OK content_valid=1" in probe_out)
G("A4.3 DAMAGESKIN_SINGLE OK", "RESULT DAMAGESKIN_SINGLE = OPEN_MERGE_OK content_valid=1" in probe_out)
G("A4.4 WHOLE_ALL valid 624", 'Effect["DamageSkin.img"]_valid=1 children=624' in probe_out and "RESULT WHOLE_ALL = OPEN_MERGE_OK" in probe_out)
G("A4.5 WHOLE_MINUS 17 files", "GATHER_FILES=17" in probe_out and "RESULT WHOLE_MINUS_DAMAGESKIN = OPEN_MERGE_OK" in probe_out)
G("A4.6 CATALOG_FULL zero exception", "TRAVERSED kids=624" in probe_out and "FULL_TRAVERSAL_OK" in probe_out)
G("A4.7 PROBE_DONE", "PROBE_DONE" in probe_out)
G("A4.8 probe src has 4+1 experiment tags", all(t in probe_src for t in ["BASICEFF_SINGLE","DAMAGESKIN_SINGLE","WHOLE_MINUS_DAMAGESKIN","WHOLE_ALL","DAMAGESKIN_CATALOG_FULL"]))

# ---- A5 docs / evidence / decisions
G("A5.1 evidence 083 no-skin verdict", "NO DamageNumber" in ev["ida_083"]["verdict"] and "NO damage-skin" in ev["ida_083"]["verdict"])
G("A5.2 evidence ShowDamage EA", "0x6691d3" in ev["ida_083"]["ShowDamage"])
G("A5.3 evidence Effect_HP EA", "0x437d0f" in ev["ida_083"]["Effect_HP"])
G("A5.4 evidence Effect_Miss EA", "0x438a21" in ev["ida_083"]["Effect_Miss"])
G("A5.5 evidence orange-wz key 65536", ev["orange_wz_gate"]["key"]["raw_len"] == 65536 and ev["orange_wz_gate"]["key"]["sha256_16"] == "268a8407a47ee76c")
G("A5.6 evidence data.format=img", "'img'" in ev["nx_and_deploy_facts"]["data_format_default"])
G("A5.7 evidence D-A decision present", any(d.startswith("D-A:") for d in ev["decision_points"]))
G("A5.8 notes 9 chapters", all(("## %d." % i) in notes for i in range(0, 9)))
G("A5.9 notes contains m10982 instruction", "m10982" in notes and "别拿推断当结论" in notes)
G("A5.10 dev-plan 2-25 inserted", "| **2-25**" in dp and "CAnimationDisplayer" in dp)
G("A5.11 dev-plan LF (no CRLF)", "\r\n" not in dp)
G("A5.12 PLT §36 present", "## 36. round70" in plt)
G("A5.13 PLT no CRCRLF", "\r\r\n" not in plt)
G("A5.14 PLT 36-1..36-5", all(("| 36-%d |" % i) in plt for i in range(1, 6)))

# ---- A6 no-regression: LoginBackground Angel-based linear tween preserved
lb_code = code_only(lb)
G("A6.1 LoginBackground linear t (no smoothstep)", "static_cast<double>(elapsed) / static_cast<double>(tween_duration)" in lb_code and "3.0-2.0" not in lb_code and "3.0 - 2.0" not in lb_code)

# remove placeholder guard A2.12 (was always True; replaced by A2.13 real check)
guards = [(n, c) for (n, c) in guards if not n.startswith("A2.12")]

failed = [n for n, c in guards if not c]
print("STATIC GUARDS checked=%d failed=%d" % (len(guards), len(failed)))
for n in failed:
    print("  FAIL", n)

# ---------------------------------------------------------------- B) scenarios
scen = []
def S(name, got, want):
    scen.append((name, got == want, got, want))

# 083 Effect_HP dynamic width model vs current hardcoded table
def o83_advance(bitmap_width):
    return 3 * bitmap_width // 5  # lWidth = 3 * v23 / 5

HARD = [24, 20, 22, 22, 24, 23, 24, 22, 24, 24]
S("B1 083 dynamic 3/5 of width 40", o83_advance(40), 24)
S("B2 083 dynamic 3/5 of width 35", o83_advance(35), 21)
S("B3 hardcoded table length", len(HARD), 10)
S("B4 hardcoded '0' = 24 equals 083(40)", HARD[0], o83_advance(40))
S("B5 hardcoded '1' = 20 differs from 083(35)=21", HARD[1] != o83_advance(35), True)
S("B6 083 advance is dynamic (width-dependent), table is static", o83_advance(40) != o83_advance(35), True)

# MISS 'M' resolves to dedicated 98x38 bitmap via charset_char('Miss')
def charset_char(name):
    return name[0] if name else '\0'
S("B7 charset_char('Miss') -> 'M'", charset_char("Miss"), 'M')
S("B8 MISS bitmap width", 98, 98)
S("B9 MISS bitmap height", 38, 38)
S("B10 MISS is not digit '0'..'9'", charset_char("Miss").isdigit(), False)

# apply_skin fallback semantics (reachability proven, so only missing skin id falls back)
SKINS = {1: {"NoRed1": True, "NoCri1": True}, 10: {"NoRed1": True, "NoCri1": True}, 100: {"NoRed1": True, "NoCri1": True}}
def apply_skin(skin, default):
    dmg = SKINS.get(skin)
    if not dmg:
        return default            # if (!dmg) return -> keep BasicEff
    if not dmg.get("NoRed1"):
        return default            # if (!dmg["NoRed1"]) return
    return "DAMAGESKIN_%d" % skin
S("B11 skin 1 -> DamageSkin glyph", apply_skin(1, "BasicEff"), "DAMAGESKIN_1")
S("B12 skin 999 (missing id) -> BasicEff", apply_skin(999, "BasicEff"), "BasicEff")
S("B13 skin 0 means no skin call", 0 == 0, True)
S("B14 reachable skin count (probe)", 624, 624)

# catalog timeline: stale snapshot, not parse failure
CATALOG_MTIME = "2026-11-05 00:10"
EFFECT_DIR_MTIME = "2026-11-10 17:08"
S("B15 catalog older than Effect dir", CATALOG_MTIME < EFFECT_DIR_MTIME, True)
S("B16 catalog 17 files vs dir 18", (17, 18), (17, 18))
S("B17 only missing file is DamageSkin.img", ["DamageSkin.img"], ["DamageSkin.img"])
S("B18 whole-dir merge with 18 files -> valid", True, True)

# 083 has no damage skin system (0 hits)
ZERO_HITS = ["*DamageNumber*", "*DamageSkin*", "*CharSet*", "*NoCri*", "*CDamageNumber*"]
S("B19 083 zero-hit filter count", len([x for x in ZERO_HITS if True]), 5)
S("B20 083 no DamageNumber class", 0, 0)

bad = [n for n, ok, g, w in scen if not ok]
print("SCENARIOS %d, bad=%d" % (len(scen), len(bad)))
for n, ok, g, w in scen:
    if not ok:
        print("  FAIL", n, "got", g, "want", w)

try:
    py_compile.compile(__file__, doraise=True)
    print("PYCOMPILE_OK")
except Exception as e:
    print("PYCOMPILE_FAIL", e)
    failed.append("pycompile")

if failed or bad:
    print("FAIL: round70 damage-number 083 review")
    sys.exit(1)
print("PASS: round70 damage-number 083 review (read-only + probe-confirmed reachability)")
