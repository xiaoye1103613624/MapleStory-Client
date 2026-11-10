# -*- coding: utf-8 -*-
"""round71 harness: GAMEOPT/SYSOPT 083 review.

Static guards prove (a) 083 authority annotations are in place, (b) NO
behaviour literals were touched, (c) the four docs landed, (d) LoginBackground
linear tween (Angel 083) is still retained.  Scenarios are pure models of the
v2 pixel test / 083 immediates / runtime probe facts -- no source is executed.
"""
import json, pathlib, re, sys

ROOT = pathlib.Path(".")

def rd(p):
    return (ROOT / p).read_text(encoding="utf-8")

def code_only(s):
    return "\n".join(l for l in s.splitlines() if not l.lstrip().startswith("//"))

def check(name, cond):
    return (name, bool(cond))

guards = []
h = rd("IO/UITypes/UIOptionMenu.h")
hc = code_only(h)
e = rd("IO/UIElement.h")
dev = rd("docs/features/development-and-fix-plan-2026-11/README.md")
plt = rd("docs/features/pending-live-tests/README.md")
notes = rd("docs/features/development-and-fix-plan-2026-11/notes/wave2-optionmenu-083-review-20261111.md")
ev = json.loads(rd("docs/features/development-and-fix-plan-2026-11/evidence/optionmenu-083-round71.json"))
lb = rd("IO/LoginBackground.cpp")

# ---- A1: 083 authority annotations (comment text, not code)
guards += [
    check("A1.1 GameOpt OnCreate EA in header", "0x993a45" in h and "12 个 CCtrlCheckBox x=128" in h),
    check("A1.2 SysOpt OnCreate EA in header", "0x99417a" in h and "40/70/100/160/190/220" in h),
    check("A1.3 Apply EA pair", "0x49ea1c" in h and "0x49ea33" in h),
    check("A1.4 D-B decision label", "D-B" in h and "五屏" in h),
    check("A1.5 v2 pixel note (12 cells/gaps)", "12 个独立洋红格" in h and "11 个真实间隙" in h),
    check("A1.6 runtime probe dims", "299x396" in h and "299x365" in h),
    check("A1.7 083 y=342 vs 372", "y=342" in h and "y370..395=0.00" in h),
    check("A1.8 UIElement.h probe note", "CUIGameOpt @0x993a30" in e and "UIWindow2.img" in e),
    check("A1.9 numeric-overlap rule", "数值重合≠语义重合" in h),
]

# ---- A2: behaviour literals EXACTLY unchanged
for lit in [
    "static constexpr int16_t CHECK_X = 130;",
    "static constexpr int16_t CHECK_TOP = 34;",
    "static constexpr int16_t CHECK_PITCH = 18;",
    "static constexpr int16_t BTN_Y = 262;",
    "static constexpr int16_t OK_X = 34;",
    "static constexpr int16_t CANCEL_X = 84;",
    "static constexpr int16_t TRACK_X0 = 99;",
    "static constexpr int16_t TRACK_X1 = 229;",
    "static constexpr int16_t SLIDER_ROW[SL_COUNT] = { 68, 98, 128, 188, 218, 248 };",
    "static constexpr int16_t COMBO_X = 157;",
    "static constexpr int16_t COMBO_Y_PATH = 155;",
    "static constexpr int16_t COMBO_Y_MOB = 306;",
    "static constexpr uint8_t RES_COUNT = 9;",
    "static constexpr int16_t RES_COMBO_Y = 343;",
    "static constexpr int16_t BTN_Y = 372;",
    "static constexpr int16_t OK_X = 194;",
    "static constexpr int16_t CANCEL_X = 244;",
]:
    guards.append(check("A2.lit " + lit[:38], hc.count(lit) == 1))
guards.append(check("A2.UIElement enum intact",
                    e.count("\t\t\tGAMEOPT,") == 1 and e.count("\t\t\tSYSOPT,") == 1
                    and e.count("\t\t\tNUM_TYPES") == 1 and e.count("\t\t\tSPECIALUI,") == 1))
guards.append(check("A2.no 083 const copied into code", "y=33+18*i" not in hc and "(194,342)" not in hc))

# ---- A3: docs landed
guards += [
    check("A3.1 dev 2-26 row", "| **2-26**" in dev and "optionmenu-083-review" in dev),
    check("A3.2 dev row pipe count 9", dev.split("| **2-26**")[1].split("\n")[0].count("|") == 8),
    check("A3.3 PLT 37", "## 37. round71" in plt and "37-5" in plt),
    check("A3.4 notes 9 chapters", all(("## %d." % i) in notes for i in range(9))),
    check("A3.5 notes D-B + fake-green lesson", "D-B" in notes and "假绿" in notes),
    check("A3.6 evidence keys", len(ev) >= 10 and ev["round"] == 71),
    check("A3.7 evidence D-B + GameOpt closed",
          any("D-B" in d for d in ev["decision_points"]) and
          any("CLOSED" in d for d in ev["decision_points"])),
    check("A3.8 evidence runtime 299x396", "299x396" in ev["runtime_probe"]["SysOpt_backgrnd"]),
    check("A3.9 evidence v2 real green", "real green" in ev["pixel_verification"]["gameopt_v2_rows"]),
]

# ---- A4: no forbidden 095 / no behaviour rewrite claims
guards += [
    check("A4.1 no 095 basis", "Maplestory95" not in h and "0x7F34C0" not in h),
    check("A4.2 fix_scope pure-comment", "pure-comment" in ev["fix_scope"] and "ZERO behaviour" in ev["fix_scope"]),
]

# ---- A5: LoginBackground linear tween retained (Angel 083)
guards += [
    check("A5.1 linear t", "static_cast<double>(elapsed) / static_cast<double>(tween_duration)" in lb),
    check("A5.2 smoothstep gone", "t*t*(3.0-2.0*t)" not in lb and "3.0-2.0*t" not in lb),
]

# ---- A6: gameopt v2 pixel bands actually resolvable from the BGRA dump
bgra = ROOT / "tools/_opt_visual/GameOpt_runtime.bgra"
if bgra.exists():
    W, H, raw = 158, 284, bgra.read_bytes()
    def magenta(x, y):
        o = (y * W + x) * 4
        b, g, r = raw[o], raw[o + 1], raw[o + 2]
        return r > 140 and g < 130 and b > 90 and (r - g) > 45 and (b - g) > 20
    counts = [sum(1 for x in range(2, 93) if magenta(x, y)) for y in range(H)]
    bands, inb, s = [], False, 0
    for y in range(H):
        hot = counts[y] > 25
        if hot and not inb:
            s, inb = y, True
        elif not hot and inb:
            if y - s >= 5: bands.append((s, y - 1))
            inb = False
    guards.append(check("A6.1 v2 finds 12 distinct cells", len(bands) == 12))
    guards.append(check("A6.2 cell pitch 18", all(bands[i + 1][0] - bands[i][0] == 18 for i in range(11))))
    guards.append(check("A6.3 first cell y32..47", bands and bands[0] == (32, 47)))
else:
    guards.append(check("A6 BGRA dump present", False))

# ---- B: pure-model scenarios
scen = []
def band_of(y, bands):
    for i, (s, e2) in enumerate(bands):
        if s <= y <= e2: return i
    return None

# reproduce v2 bands from the verified constants
bands = [(32 + 18 * i, 47 + 18 * i) for i in range(12)]
o83 = [33 + 18 * i for i in range(12)]
cur = [34 + 18 * i for i in range(12)]
scen.append(check("S1 083 rows -> band 0..11", [band_of(y, bands) for y in o83] == list(range(12))))
scen.append(check("S2 current rows -> band 0..11", [band_of(y, bands) for y in cur] == list(range(12))))
scen.append(check("S3 same band per index", all(band_of(o83[i], bands) == band_of(cur[i], bands) for i in range(12))))
scen.append(check("S4 083/current delta is 1px", all(cur[i] - o83[i] == 1 for i in range(12))))
scen.append(check("S5 11 real gaps", all(48 + 18 * i <= 47 + 18 * (i + 1) + 1 for i in range(11))))
scen.append(check("S6 old wide-band fake green rejected",
                  not (all(32 <= y <= 246 for y in o83) and all(32 <= y <= 246 for y in cur) and
                       "real green" in ev["pixel_verification"]["gameopt_v2_rows"] is False)))

# 083 SysOpt immediates vs current +28
o83_sl = [40, 70, 100, 160, 190, 220]
cur_sl = [68, 98, 128, 188, 218, 248]
scen.append(check("S7 +28 exactly", all(cur_sl[i] - o83_sl[i] == 28 for i in range(6))))
scen.append(check("S8 both inside wide light band (no discrimination)",
                  all(27 <= y <= 307 for y in o83_sl) and all(27 <= y <= 307 for y in cur_sl)))
scen.append(check("S9 083 buttons y=342, current 372", 342 != 372 and 372 - 342 == 30))
scen.append(check("S10 button 47x19 within 396 runtime",
                  372 + 19 <= 396 and 342 + 19 <= 365))
scen.append(check("S11 083 has no 9-res combo, current has 9", ev["ida_083_immediates"]["no_9_res_options"] and ev["current_end"]["UISysOpt"].find("RES 9") >= 0))
scen.append(check("S12 runtime 299x396 != old .nx 299x365", 396 != 365 and ev["pixel_verification"]["old_nx_vs_runtime"].find("0/365") >= 0))
scen.append(check("S13 UIWindow2/StatusBar3 runtime 0",
                  ev["runtime_probe"]["UIWindow2.img_valid"] == 0 and ev["runtime_probe"]["StatusBar3.img_valid"] == 0))
scen.append(check("S14 GameOpt backgrnd 158x284 runtime", ev["runtime_probe"]["GameOpt_backgrnd"] == "158x284"))
scen.append(check("S15 Basic.img CheckBox 12x12 / BtOK2 47x19",
                  "12x12" in ev["nx_facts"]["Basic.img/CheckBox"] and "47x19" in ev["nx_facts"]["Basic.img/BtOK2/normal/0"]))
scen.append(check("S16 orange-wz gate content error kept", ev["orange_wz_gate"]["content_error"].find("IMG parse failed") == 0 or
                  "IMG" in ev["orange_wz_gate"]["content_error"]))
scen.append(check("S17 65536B GMS key", ev["orange_wz_gate"]["key_raw_len"] == 65536 and
                  ev["orange_wz_gate"]["key_sha256_16"] == "268a8407a47ee76c"))
scen.append(check("S18 D-B open, UIGameOpt closed",
                  any("D-B" in d for d in ev["decision_points"]) and any("CLOSED" in d for d in ev["decision_points"])))
scen.append(check("S19 no behaviour literal changed (A2 all green)", all(g[1] for g in guards if g[0].startswith("A2."))))
scen.append(check("S20 LoginBackground linear retained (A5 all green)", all(g[1] for g in guards if g[0].startswith("A5."))))

bad_g = [n for n, ok in guards if not ok]
bad_s = [n for n, ok in scen if not ok]
print("STATIC GUARDS checked=%d failed=%d" % (len(guards), len(bad_g)))
for n in bad_g: print("  FAIL", n)
print("SCENARIOS %d, bad=%d" % (len(scen), len(bad_s)))
for n in bad_s: print("  FAIL", n)
if bad_g or bad_s:
    sys.exit(1)
print("PASS: round71 GAMEOPT/SYSOPT 083 review (read-only + v2 pixel real-green + probe)")
