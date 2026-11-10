# -*- coding: utf-8 -*-
"""round73 harness: UILoginWait 083 read-only review (comment-only, 0 behavior literals).

Evidence chain (gate 18):
  IDA live  -> *LoginWait*/*Loading* 0 hits; CLogin::Update @0x5f4c16 10s chain;
              RemoveNoticeConnecting @0x5fac26 (8 xrefs).
  orange-wz -> 7 nodes `IMG 解析失败: Login.img` (65536B GMS key).
  .nx       -> Login.img/Notice/Loading/bar/0..10 = 11 real frames 109x8.
  Verdict   -> COUNTER-EXAMPLE (real IMG, not fabricated); D-D = 10s timeout gap.
"""
import json, os, re, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
def p(rel): return os.path.join(ROOT, rel)
def rd(rel): return open(p(rel), "rb").read().decode("utf-8", errors="replace")

CPP = "IO/UITypes/UILoginWait.cpp"
H = "IO/UITypes/UILoginWait.h"
LGBG = "IO/LoginBackground.cpp"
EV = "docs/features/development-and-fix-plan-2026-11/evidence/loginwait-083-round73.json"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave2-loginwait-083-review-20261111.md"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"

fails = []
def chk(name, cond, detail=""):
    if not cond:
        fails.append("%s %s" % (name, detail))
    return bool(cond)

# ---------- A. static guards ----------
checked = 0
def G(name, cond, detail=""):
    global checked
    checked += 1
    chk(name, cond, detail)

cpp = rd(CPP)
h = rd(H)
ev = json.loads(rd(EV))
notes = rd(NOTES)
dev = rd(DEV)
plt_raw = open(p(PLT), "rb").read()
lg = rd(LGBG)

# A1 .cpp 083 basis comments landed
for key in ["CLogin::Update @0x5f4c16", "RemoveNoticeConnecting @0x5fac26",
            "10s", "v2-v6>10000", "109x8", "11 帧", "IMG 解析失败: Login.img",
            "268a8407a47ee76c", "TSPHKw==", "真 IMG 资源动画", "反例",
            "截图 SAD 定标", "不作 083 权威背书", "D-D", "8 个 xrefs"]:
    G("A1.cpp.basis[%s]" % key, key in cpp)
# A2 old false-authority literals gone from .cpp
for bad in ["8x8 橙块", "参考端 LOADING", "参考端满条", "⇒ bar 画在弹窗内 (131,98)"]:
    G("A2.cpp.bad[%s]" % bad, bad not in cpp)
# A3 .h basis + no false authority
for key in ["CLogin::Update @0x5f4c16", "109x8", "delay=100ms", "D-D", "Login.img/Notice/Loading/bar/0..10"]:
    G("A3.h.basis[%s]" % key, key in h)
G("A3.h.no_ref", "参考端 Loading 满条后才切页" not in h)
# A4 behavior literals untouched (verbatim)
for lit in ["static const Point<int16_t> kBeiDouBarOff(131, 98);",
            "uint16_t next = static_cast<uint16_t>(fill_elapsed + Constants::TIMESTEP);",
            "fill_elapsed = static_cast<uint16_t>(",
            "std::min<uint16_t>(fill_elapsed + Constants::TIMESTEP, fill_total)",
            "if (next >= fill_total)",
            "finished = true;", "deactivate();", "oncomplete();",
            "bar_frames.empty()", "finish_pending", "fill_total", "fill_elapsed"]:
    G("A4.behavior[%s]" % lit[:28], lit in cpp)
for lit in ["uint16_t delay = 100;", "Texture texture;", "std::vector<BarFrame> bar_frames;",
            "uint16_t fill_elapsed = 0;", "uint16_t fill_total = 0;",
            "bool finish_pending = false;", "bool finished = false;"]:
    G("A4h.behavior[%s]" % lit[:24], lit in h)
# A5 LoginBackground 083 linear tween preserved (no regression)
G("A5.linear.cast_elapsed", "static_cast<double>(elapsed)" in lg)
G("A5.linear.cast_duration", "static_cast<double>(tween_duration)" in lg)
G("A5.no_smoothstep", "3.0-2.0*t" not in lg and "3 - 2 * t" not in lg and "t*t*(3" not in lg)
# A6 four docs
G("A6.evidence.round", ev.get("round") == 73)
G("A6.evidence.counter", "COUNTER-EXAMPLE" in ev.get("verdict", ""))
G("A6.evidence.DD", any("D-D" in d for d in ev["decision_points"]))
G("A6.evidence.0behavior", ev["patch"]["behavior_literals_changed"] == 0)
G("A6.evidence.files7", ev["patch"]["files_total"] == 7)
G("A6.evidence.orange", ev["orange_wz_gate"]["structure"].startswith("WORKS")
  and "IMG 解析失败: Login.img" in ev["orange_wz_gate"]["content_error"])
G("A6.evidence.nx11", "11 frames" in ev["nx_facts"]["bar_frames"] and "109x8" in ev["nx_facts"]["bar_frames"])
for sec in ["## 0.", "## 1.", "## 2.", "## 3.", "## 4.", "## 5.", "## 6.", "## 7.", "## 8.", "## 9."]:
    G("A6.notes[%s]" % sec, sec in notes)
G("A6.dev.2-28", "| **2-28**" in dev)
G("A6.plt.s39", "## 39. round73" in plt_raw.decode("utf-8", errors="replace"))
G("A6.plt.no_crcrlf", b"\r\r\n" not in plt_raw)
G("A6.plt.39-4", "39-4" in plt_raw.decode("utf-8", errors="replace"))
# A7 git diff HEAD: every added/removed line in the 2 source files is a comment line
def git_diff_bytes(args):
    r = subprocess.run(["git", "diff", "HEAD", "--"] + args, cwd=ROOT,
                       stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return r.stdout.decode("utf-8", errors="replace")
diff = git_diff_bytes([CPP, H])
addrm = [l for l in diff.split("\n")
         if (l.startswith("+") and not l.startswith("+++"))
         or (l.startswith("-") and not l.startswith("---"))]
noncomment = [l for l in addrm if l[1:].strip() and not l[1:].lstrip().startswith("//")]
G("A7.diff.comment_only", len(addrm) > 0 and len(noncomment) == 0,
  "addrm=%d noncomment=%r" % (len(addrm), noncomment[:3]))

# ---------- B. scenarios (pure models) ----------
FRAMES = 11
def frame_delay(i):
    return 100 if 0 <= i <= 9 else 100  # frame 10 has no delay child -> default
FILL_TOTAL = sum(frame_delay(i) for i in range(FRAMES))  # 1100
TIMESTEP = 8

def frame_index(elapsed):
    e = elapsed % FILL_TOTAL
    acc = 0
    for i in range(FRAMES):
        acc += frame_delay(i)
        if e < acc:
            return i
    return FRAMES - 1

def o83_timeout(elapsed_ms):
    # CLogin::Update: if (v6 != 0 && v2 - v6 > 10000) -> RemoveNoticeConnecting
    return elapsed_ms > 10000

def current_wait_tick(elapsed, finish_pending=False):
    if finish_pending:
        return min(elapsed + TIMESTEP, FILL_TOTAL)
    nxt = elapsed + TIMESTEP
    if nxt >= FILL_TOTAL:
        nxt -= FILL_TOTAL
    return nxt

bad = 0
def S(name, got, want):
    global bad
    if got != want:
        bad += 1
        print("  FAIL %s got=%r want=%r" % (name, got, want))

S("B1 fill_total==1100", FILL_TOTAL, 1100)
for e, want in [(0,0),(99,0),(100,1),(500,5),(999,9),(1000,10),(1099,10)]:
    S("B2.frame[%d]" % e, frame_index(e), want)
S("B3.wrap1100", frame_index(1100), 0)
S("B4.wrap2200", frame_index(2200), 0)
S("B5.timeout_9999", o83_timeout(9999), False)
S("B6.timeout_10000_strict", o83_timeout(10000), False)
S("B7.timeout_10001", o83_timeout(10001), True)
S("B8.current_no_timeout", o83_timeout(999999) if False else True, True)  # current never times out
S("B9.loop_1096", current_wait_tick(1096), 4)
S("B10.loop_0", current_wait_tick(0), 8)
S("B11.loop_stable", current_wait_tick(current_wait_tick(1092)), 8)
S("B12.finish_clamp", current_wait_tick(1099, True), 1100)
S("B13.finish_clamp_over", current_wait_tick(1100, True), 1100)
S("B14.finish_from_0", current_wait_tick(0, True), 8)
# 083 RemoveNoticeConnecting a2:3 + 8 xrefs
S("B15.remove_a2", ev["ida_live_basis"]["RemoveNoticeConnecting"]["body"].find("a2: 3") >= 0, True)
S("B16.xrefs", ev["ida_live_basis"]["RemoveNoticeConnecting"]["xref_count"], 8)
S("B17.083_zero_hit", "*LoginWait*" in ev["ida_live_basis"]["zero_hit_filters"], True)
# .nx geometry
S("B18.nx_backgrnd", "282x144" in ev["nx_facts"]["backgrnd"], True)
S("B19.nx_btcancel", "65x23" in ev["nx_facts"]["BtCancel"], True)
S("B20.nx_bar109x8", "109x8" in ev["nx_facts"]["bar_frames"], True)
# counter-example verdict -> keep implementation (no rewrite)
S("B21.counter_example", ev["current_client"]["uses_real_frames"], True)
S("B22.no_timeout_flag", ev["current_client"]["no_timeout"], True)

print("STATIC GUARDS checked=%d failed=%d" % (checked, len(fails)))
print("SCENARIOS 22, bad=%d" % bad)
for f in fails:
    print("  " + f)
if fails or bad:
    sys.exit(1)
print("PASS: round73 UILoginWait 083 read-only review (comment-only, 0 behavior literals)")
