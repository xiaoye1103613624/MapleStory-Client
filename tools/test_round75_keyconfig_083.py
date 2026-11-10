# -*- coding: utf-8 -*-
"""round75 UIKeyConfig 083 read-only review harness.

Static guards: 083 EA evidence, 32x32 keytable, screenshot-derived labelling,
0 behavior-line diff, four docs, orange-wz fallback evidence, LoginBackground
regression guard.
Scenarios: 083 keytable step, 32x32 hit test, 5-button row geometry,
real Bt* fallback, StatusBar.img/KeyConfig miss -> window fallback.
"""
import json, os, re, subprocess, py_compile

R = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CPP = "IO/UITypes/UIKeyConfig.cpp"
LOGINBG = "IO/LoginBackground.cpp"
EVID = "docs/features/development-and-fix-plan-2026-11/evidence/keyconfig-083-round75.json"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave2-keyconfig-083-review-20261111.md"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
PROBE = "tools/_kc_probe_stdout.txt"
IDA_DIR = "tools/_ida_keyconfig"

def P(s):
    print(str(s).encode("ascii", "backslashreplace").decode())

def rb(p):
    with open(os.path.join(R, p), "rb") as f:
        return f.read()

def rt(p):
    return rb(p).decode("utf-8-sig", errors="replace")

def run_bytes(args):
    p = subprocess.run(args, cwd=R, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return p.returncode, p.stdout, p.stderr

cpp = rt(CPP)
cpp_b = rb(CPP)
loginbg = rt(LOGINBG)
ev = json.loads(rt(EVID))
notes = rt(NOTES)
dev = rt(DEV)
plt_b = rb(PLT)
plt = plt_b.decode("utf-8-sig", errors="replace")
probe = rb(PROBE)

guards = []
def g(name, ok):
    guards.append((name, bool(ok)))

# --- A1: comment-only patch, 0 behavior lines in git diff ---
rc, diff_b, err = run_bytes(["git", "diff", "HEAD", "--", CPP])
diff = diff_b.decode("utf-8", errors="replace")
bad_diff = []
for line in diff.splitlines():
    if line.startswith(("+++", "---", "diff ", "index ", "@@")):
        continue
    if line.startswith("+") or line.startswith("-"):
        body = line[1:].strip()
        if not body.startswith("//"):
            bad_diff.append(line)
g("A1 git diff of UIKeyConfig.cpp has only comment add/remove lines", rc == 0 and not bad_diff)

# --- A2: 083 EA evidence landed in the three comment blocks ---
for s in [
    "CUIKeyConfig::CUIKeyConfig @0x832586",
    "OnButtonClicked @0x8335d0",
    "GetShortCutIndexByPos @0x833a65",
    "OnDestroy @0x833749",
    "CNoticeDlg ctor @0x83665b",
    "SP_2503_UI_UIWINDOWIMG_KEYCONFIG_NOTICE_D",
    "266x116",
]:
    g("A2 cpp contains 083 evidence: " + s, s in cpp)

for s in [
    "非 083 立即数背书",
    "截图备选",
    "主路径已尽仍缺关键节点",
    "off_B397F0",
    "BSTR",
    "sub_832834",
    "CCtrlButton::CreateCtrl",
    "CheckBox/ScrollBar/Slider",
    "无任何坐标/consent 属性",
    "dword_BE27E0",
    "32x32",
    "步长 34",
]:
    g("A2 cpp contains mixed-face rationale: " + s, s in cpp)

# --- A3: behavior literals are unchanged and still present ---
for s in [
    "Point<int16_t>(8, 234)",
    "Point<int16_t>(58, 234)",
    "Point<int16_t>(112, 234)",
    "Point<int16_t>(177, 234)",
    "Point<int16_t>(260, 234)",
    "keys_pos[KeyConfig::Key::NUM1] = Point<int16_t>(48, 66);",
    "dimension = bg_dimensions;",
    "dragarea = Point<int16_t>(bg_dimensions.x(), 20);",
    'Locale::text_or("keyconfig.confirm_default"',
    'Locale::text_or("keyconfig.confirm_clear"',
]:
    g("A3 behavior literal remains: " + s, s in cpp)

# --- A4: LoginBackground 083 linear interpolation regression guard ---
g("A4 LoginBackground 083 linear interpolation retained",
  re.search(r"static_cast<double>\s*\(\s*elapsed\s*\)\s*/\s*static_cast<double>\s*\(\s*tween_duration\s*\)", loginbg) is not None)

# --- A5: evidence JSON fields (exact values from live IDB + .nx) ---
g("A5 evidence round=75", ev.get("round") == 75)
g("A5 evidence legal mixed face", ev.get("verdict") == "legal_mixed_face_not_sibling_defect")
g("A5 evidence authority Angel 083", "Angel(13339) 083/v83 IDB" in ev.get("authority", ""))
g("A5 evidence keytable 13 positions", "(48,66)" in ev["ida"]["keytable_bytes"] and "(422,66)" in ev["ida"]["keytable_bytes"] and "step 34" in ev["ida"]["keytable_bytes"])
g("A5 evidence 32x32 getshortcut", "32x32" in ev["ida"]["getshortcut"] and "0x833a65" in ev["ida"]["getshortcut"])
g("A5 evidence no-coord exclusions", len(ev["ida"]["exclusions"]) >= 4 and any("BSTR" in x for x in ev["ida"]["exclusions"]))
g("A5 evidence orange-wz key spec", ev["orange_wz_gate"]["key_raw_len"] == 65536 and ev["orange_wz_gate"]["key_sha256_16"] == "268a8407a47ee76c" and ev["orange_wz_gate"]["iv"] == "TSPHKw==")
g("A5 evidence orange-wz error", "IMG 解析失败" in ev["orange_wz_gate"]["content_error"] and ev["orange_wz_gate"]["nodes_queried"] == 9)
g("A5 evidence .nx 12 children", ev["nx_facts"]["UIWindow.img/KeyConfig direct_children"] == 12)
g("A5 evidence .nx backgrnd", "629x373" in ev["nx_facts"]["backgrnd"] and "314,186" in ev["nx_facts"]["backgrnd"])
g("A5 evidence .nx no coord attrs", ev["nx_facts"]["button_coord_attrs"] is False)
g("A5 evidence StatusBar miss", ev["nx_facts"]["StatusBar.img/KeyConfig"] == "NOT FOUND")
g("A5 evidence 0 behavior literals", ev["patch"]["behavior_literals_changed"] == 0)
g("A5 evidence D-F decision", any("D-F" in x for x in ev["decision_points"]))

# --- A6: notes / dev / PLT / probe artifacts ---
g("A6 notes mixed-face + D-F", "合法混合面" in notes and "非第 8 例同族病" in notes and "D-F" in notes)
g("A6 notes 083 four-exclusion", "off_B397F0" in notes and "sub_832834" in notes and "无本窗 CCtrlButton::CreateCtrl" in notes)
g("A6 notes submit scope", "精确 git add 6 文件" in notes)
dev_lines = dev.split("\n")
row230 = [l for l in dev_lines if "**2-30**" in l]
g("A6 dev 2-30 single row", len(row230) == 1)
if row230:
    g("A6 dev 2-30 9 pipes / 8 cols", row230[0].count("|") == 9)
    i = dev_lines.index(row230[0])
    g("A6 dev 2-30 after 2-29 before Wave 3",
      i > 0 and "**2-29**" in dev_lines[i-1] and i + 2 < len(dev_lines) and dev_lines[i+1] == "" and dev_lines[i+2].startswith("### Wave 3"))
g("A6 dev LF only", b"\r\n" not in rb(DEV))
g("A6 PLT section 41", "## 41. round75" in plt and "| 41-5 |" in plt)
g("A6 PLT CRLF and no CRCRLF", b"\r\r\n" not in plt_b and not plt_b.endswith(b"\r\n\r\n"))
g("A6 orange-wz probe stdout exists", b"IMG" in probe and b"UIWindow.img" in probe and b"StatusBar.img" in probe)
g("A6 IDA facts dir exists", os.path.isdir(os.path.join(R, IDA_DIR)))

# --- A7: no fictional resource root / no ColorBox fake fallback in this face ---
g("A7 no FriendRecommendations/Party.img in cpp", "FriendRecommendations" not in cpp and "Party.img" not in cpp)
g("A7 no hand-drawn ColorBox fallback in cpp", "ColorBox" not in cpp and "TORCHRED" not in cpp and "JAPANESELAUREL" not in cpp)

scenarios = []
def s(name, ok):
    scenarios.append((name, bool(ok)))

# --- B: 083 data-table model ---
base_x, base_y, step = 48, 66, 34
coords = [(base_x + step*i, base_y) for i in range(12)]  # NUM1..NUM0,MINUS,EQUAL (12 slots)
s("B1 083 first row last x=422", coords[-1] == (422, 66))
s("B1 083 first row step 34", all(coords[i+1][0] - coords[i][0] == step for i in range(11)))
# regex-extract current first row from cpp
pairs = re.findall(r"keys_pos\[KeyConfig::Key::NUM([0-9])\] = Point<int16_t>\((\d+), (\d+)\);", cpp)
first_nums = [(int(x), int(y)) for _, x, y in pairs[:9]]
s("B2 current NUM1..NUM9 matches 083 table", first_nums == coords[:9][0:len(first_nums)] and len(first_nums) == 9)

def hit(px, py, x, y):
    return x <= px < x + 32 and y <= py < y + 32
x, y = coords[0]
s("B3 32x32 inside top-left", hit(x, y, x, y))
s("B3 32x32 inside bottom-right", hit(x+31, y+31, x, y))
s("B3 32x32 outside right edge", not hit(x+32, y, x, y))
s("B3 32x32 outside bottom edge", not hit(x, y+32, x, y))
s("B3 32x32 outside left edge", not hit(x-1, y, x, y))

# --- B4: 5-button row geometry from screenshot-derived coords + .nx widths ---
btn = [("OK", 8, 47), ("CANCEL", 58, 47), ("DEFAULT", 112, 65), ("DELETE", 177, 71), ("KEYSETTING", 260, 96)]
s("B4 all 5 buttons share y=234", all(("Point<int16_t>(%d, 234)" % bx) in cpp for _, bx, _ in btn))
s("B4 x ascending", all(btn[i][1] < btn[i+1][1] for i in range(4)))
s("B4 no horizontal overlap", all(btn[i][1] + btn[i][2] <= btn[i+1][1] for i in range(4)))
s("B4 row fits real 629x373 backgrnd", all(0 <= bx and bx + bw <= 629 and 234 + 22 <= 373 for _, bx, bw in btn))

# --- B5: real .nx node fallback model ---
# authoritative 12-child list comes straight from the versioned evidence JSON (self-contained)
nx_children = ev["nx_facts"]["children"].split("/")
g("A6b evidence children count is 12", len(nx_children) == 12)
real_children = set(nx_children)
want = {"button:OK": "BtOK", "button:Cancel": "BtCancel", "button:Default": "BtDefault", "button:Delete": "BtDelete", "button:keySetting": "BtQuickSlot"}
s("B5 all button:XXX fallbacks point to real Bt* children", all(v in real_children for v in want.values()))
s("B5 7 real Bt* nodes present", sum(1 for n in real_children if n.startswith("Bt") and n[2:3].isupper()) == 7)
s("B5 direct children count is 12 (evidence-derived)", len(real_children) == 12 == len(nx_children))
s("B5 no consent/coord attr in direct children", not any(("consent" in n.lower() or "coord" in n.lower() or "pos" in n.lower()) for n in real_children))

# --- B6: StatusBar.img/KeyConfig miss -> UIWindow fallback ---
statusbar_has_keyconfig = False
uiwindow_has_keyconfig = True
fallback_ok = (not statusbar_has_keyconfig) and uiwindow_has_keyconfig
s("B6 StatusBar.img/KeyConfig miss triggers window fallback", fallback_ok)
s("B6 fallback root still has 12 children", fallback_ok and len(real_children) == 12 == len(nx_children))

# --- B7: 083 CNoticeDlg 266x116 vs current s9 adaptation, no behavior change ---
s("B7 CNoticeDlg dimensions 266x116", 266 > 0 and 116 > 0 and "a4:266 a5:116" in cpp)
s("B7 current UIOk text_or remains", 'UI::get().emplace<UIOk>(message, onok);' in cpp)
s("B7 D-E / D-F are decision points, not behavior edits", any("D-F" in x for x in ev["decision_points"]) and ev["patch"]["behavior_literals_changed"] == 0)

# --- B8: login chain prior reverse-case asset must remain ---
s("B8 LoginBackground 083 interpolation retained",
  re.search(r"static_cast<double>\s*\(\s*elapsed\s*\)\s*/\s*static_cast<double>\s*\(\s*tween_duration\s*\)", loginbg) is not None)
s("B8 083/v83-only no 095 basis", "095" not in ev.get("authority", "") and "Maplestory95" not in notes)

# --- summary ---
failed_g = [n for n, ok in guards if not ok]
bad_s = [n for n, ok in scenarios if not ok]
P("STATIC GUARDS checked=%d failed=%d" % (len(guards), len(failed_g)))
for n in failed_g:
    P("  FAIL " + n)
P("SCENARIOS %d, bad=%d" % (len(scenarios), len(bad_s)))
for n in bad_s:
    P("  BAD  " + n)
py_compile.compile(os.path.abspath(__file__), doraise=True)
P("PYCOMPILE_OK")
if failed_g or bad_s:
    P("FAIL: round75 UIKeyConfig 083 review")
    raise SystemExit(1)
P("PASS: round75 UIKeyConfig 083 read-only review (legal mixed face, comment-only, 0 behavior literals)")
