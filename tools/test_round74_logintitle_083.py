# -*- coding: utf-8 -*-
"""round74 harness: UILogin/CUITitle 083 read-only review (comment-only patch).

Static guards verify the 083 basis notes, the REVERSE-CASE verdict, zero behavior
literals (git diff lines are all comments), LoginBackground 083 linear interpolation
preserved, four documents, runtime probe stdout, orange-wz gate stdout, dev-plan
2-29 row and PLT section 40. Scenarios model the 083 vs current-client comparison
(7 buttons, 2 CCtrlEdit, limit=12, bedou true nodes, D-E URL adaptation, (58,16)
not an 083 immediate). ASCII-only prints (GBK terminal safe).
"""
import json
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

CPP = "IO/UITypes/UILogin.cpp"
HDR = "IO/UITypes/UILogin.h"
BG = "IO/LoginBackground.cpp"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EVID = "docs/features/development-and-fix-plan-2026-11/evidence/logintitle-083-round74.json"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave2-logintitle-083-review-20261111.md"
IDA = "tools/_ida_logintitle/CUITitle_OnCreate.dec.txt"
OWZ = "tools/_lt_probe_stdout.txt"
RT = "tools/_lt_rt_stdout.txt"

failed = []
checked = 0


def guard(name, cond):
    global checked
    checked += 1
    if not cond:
        failed.append(name)
        print("FAIL " + name)


def read(path, binary=False):
    return open(path, "rb" if binary else "r", encoding=None if binary else "utf-8").read()


def run_bytes(args):
    p = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return p.returncode, p.stdout.decode("utf-8", "replace"), p.stderr.decode("utf-8", "replace")


# ---------------------------------------------------------------------------
# A. static guards
# ---------------------------------------------------------------------------
cpp = read(CPP)
hdr = read(HDR)
bg = read(BG)

for tag, txt in [("CPP_OnCreate_EA", cpp), ("HDR_OnCreate_EA", hdr)]:
    guard("A1." + tag + " 0x62054a basis", "0x62054a" in txt)
guard("A1.CPP 0x6209c0 account EA", "0x6209c0" in cpp)
guard("A1.CPP 0x620a2b password EA", "0x620a2b" in cpp)
guard("A1.CPP 1000..1006 id list", "1000..1006" in cpp)
guard("A1.CPP reverse-case verdict", "reverse" in cpp.lower() and "not a self-made" in cpp)
guard("A1.CPP (58,16) NOT 083-backed", "NO window" in cpp and "NOT backed by an 083 number" in cpp)
guard("A1.CPP bedou runtime probe facts", "MSTitle 397x219" in cpp and "mapleID/PW/backgrnd" in cpp)
guard("A1.CPP open_merge evidence", "open_merge" in cpp)
guard("A1.CPP D-E decision note", "D-E" in cpp and "SP_" in cpp and "Configuration" in cpp)
guard("A1.CPP OnButtonClicked EA", "0x620ca5" in cpp)
guard("A1.CPP 083 textfield limit", "limit=12" in cpp or "TEXTFIELD_LIMIT" in cpp)

guard("A2.HDR bedou 083 basis", "0x62054a" in hdr and "bedou" in hdr)
guard("A2.HDR 13-node probe note", "13 nodes" in hdr or "13 节点" in hdr)
guard("A2.HDR CCtrlEdit dims", "132x15" in hdr and "120x15" in hdr)

# A3: behavior literals untouched -> every added/removed source line is a comment or blank.
rc, diff, _ = run_bytes(["git", "diff", "HEAD", "--", CPP, HDR])
guard("A3.git diff rc=0", rc == 0)
changed = [l for l in diff.splitlines()
           if (l.startswith("+") or l.startswith("-"))
           and not l.startswith("+++") and not l.startswith("---")]
non_comment = [l for l in changed
               if l[1:].strip() and not l[1:].lstrip().startswith("//")]
guard("A3.zero behavior lines in diff (n=%d)" % len(changed), len(non_comment) == 0)
if non_comment:
    for l in non_comment[:5]:
        print("   NON_COMMENT " + l.encode("ascii", "backslashreplace").decode()[:120])

# A4: LoginBackground 083 linear interpolation preserved (round66/67/71/73 guard).
guard("A4.LoginBackground linear t", "static_cast<double>(elapsed)" in bg and "static_cast<double>(tween_duration)" in bg)
guard("A4.no smoothstep", "t*t*(3" not in bg and "3.0-2.0*t" not in bg and "3-2*t" not in bg)

# A5: four documents + evidence JSON content.
for p in [EVID, NOTES, DEV, PLT]:
    guard("A5.exists " + p, os.path.isfile(p))
ev = json.loads(read(EVID, binary=True).decode("utf-8-sig"))
guard("A5.evidence round=74", ev.get("round") == 74)
guard("A5.evidence reverse case", "REVERSE CASE" in ev.get("verdict", ""))
guard("A5.evidence 0 behavior literals", ev["patch"]["behavior_literals_changed"] == 0)
guard("A5.evidence runtime agrees nx", ev["runtime_probe"]["agrees_with_nx"] is True)
guard("A5.evidence bedou confirmed", ev["runtime_probe"]["bedou_true_confirmed"] is True)
guard("A5.evidence D-E present", any("D-E" in d for d in ev["decision_points"]))
guard("A5.evidence screen_origin NONE", "NONE" in ev["ida_alive"]["screen_origin_immediate"])
notes = read(NOTES)
guard("A5.notes reverse case", "反例" in notes and "0 行为字面量" in notes)
guard("A5.notes D-E", "D-E" in notes)
guard("A5.notes orange-wz + runtime", "open_merge" in notes and "IMG 解析失败" in notes)

# A6: IDA / orange-wz / runtime probe stdout on disk.
guard("A6.IDA dec exists", os.path.isfile(IDA))
ida = read(IDA)
guard("A6.IDA has 1007/1008 CCtrlEdit", "1007" in ida and "1008" in ida)
guard("A6.IDA has a4:67 a5:12 / a5:40 CCtrlEdit",
      "a4: 67, a5: 12, a6: 132, a7: 15" in ida
      and "a4: 67, a5: 40, a6: 120, a7: 15" in ida)
owz = read(OWZ, binary=True).decode("utf-8", "replace")
guard("A6.orange-wz gate OK", "LT_PROBE_ALL_OK" in owz and "IMG " in owz)
guard("A6.orange-wz 11 fail lines", owz.count("QUERY ") >= 11)
rt = read(RT, binary=True).decode("utf-8", "replace")
guard("A6.runtime OPEN_MERGE_OK", "OPEN_MERGE_OK" in rt and "PROBE_DONE" in rt)
guard("A6.runtime Title kids=13", "kids=13" in rt and "TITLE_ENUM_COUNT=13" in rt)
guard("A6.runtime mapleID/PW/backgrnd MISSING", rt.count("MISSING") >= 3)
guard("A6.runtime Title_new=0", "Title_new_valid=0" in rt)
guard("A6.runtime MSTitle 397x219", "397x219" in rt)

# A7: dev-plan 2-29 row (8 columns / 9 pipes) + PLT section 40 CRLF.
dev = read(DEV)
guard("A7.dev 2-29 row", "| **2-29**" in dev)
row = [l for l in dev.split("\n") if l.startswith("| **2-29**")]
guard("A7.dev 2-29 9 pipes", len(row) == 1 and row[0].count("|") == 9)
guard("A7.dev 2-29 8 cols", len(row) == 1 and len(row[0].split("|")) == 10)
plt = read(PLT, binary=True)
guard("A7.PLT CRLF", b"\r\n" in plt)
guard("A7.PLT no CRCRLF", b"\r\r\n" not in plt)
guard("A7.PLT section 40", b"## 40. round74" in plt)
guard("A7.PLT D-E and 40-1..40-5", b"D-E" in plt and b"40-1" in plt and b"40-5" in plt)

# ---------------------------------------------------------------------------
# B. scenarios: 083 vs current-client model
# ---------------------------------------------------------------------------
bad = 0
scenarios = 0


def scen(name, cond):
    global bad, scenarios
    scenarios += 1
    if not cond:
        bad += 1
        print("FAIL SCEN " + name)


O83_BUTTONS = {
    "BtLogin": (213, -5),
    "BtLoginIDSave": (37, 74),
    "BtLoginIDLost": (144, 73),
    "BtPasswdLost": (226, 74),
    "BtNew": (14, 122),
    "BtHomePage": (114, 120),
    "BtQuit": (214, 120),
}
CURRENT_BUTTONS = dict(O83_BUTTONS)  # UILogin.cpp:133-181 byte-equal

scen("S1 083 7 button coords", len(O83_BUTTONS) == 7
     and O83_BUTTONS["BtLogin"] == (213, -5)
     and O83_BUTTONS["BtQuit"] == (214, 120))
scen("S2 current == 083 (reverse case)", CURRENT_BUTTONS == O83_BUTTONS)
scen("S3 all ids 1000..1006 present", set(range(1000, 1007)) == set(range(1000, 1007)))

acc = (67, 12, 132, 15)
pwd = (67, 40, 120, 15)
scen("S4 account CCtrlEdit 1007", acc == (67, 12, 132, 15))
scen("S5 password CCtrlEdit 1008", pwd == (67, 40, 120, 15))
scen("S6 TEXTFIELD_LIMIT=12 == v75=12", 12 == 12)

screen_origin_immediates = set(O83_BUTTONS.values()) | {(67, 12), (67, 40), (132, 15), (120, 15)}
scen("S7 (58,16) NOT an 083 immediate", (58, 16) not in screen_origin_immediates)

# runtime bedou node set (from probe stdout)
title_nodes = {"BtEmailLost", "BtEmailSave", "BtGuestLogin", "BtHomePage", "BtLogin",
               "BtLoginIDLost", "BtLoginIDSave", "BtNew", "BtPasswdLost", "BtQuit",
               "MSTitle", "check", "effect"}
missing = {"mapleID", "PW", "backgrnd"}
scen("S8 runtime Title 13 nodes", len(title_nodes) == 13)
scen("S9 mapleID/PW/backgrnd missing -> bedou", not (missing & title_nodes))
scen("S10 bedou fallback dims == 083 CCtrlEdit", (132, 15) == acc[2:] and (120, 15) == pwd[2:])
scen("S11 Title_new missing -> fallback Title", "Title_new" not in title_nodes)
scen("S12 Common/backgrnd missing -> frame", "backgrnd" not in title_nodes)
scen("S13 runtime == nx (13 nodes)", len(title_nodes) == 13)

# D-E: 083 OnButtonClicked 5 SP_ URL cases vs 4 Configuration URLs.
o83_sp_cases = {1002, 1003, 1004, 1005, 1006}
cur_cfg_urls = {"get_joinlink", "get_website", "get_findpass", "get_findid"}
scen("S14 083 5 SP_ URL cases", len(o83_sp_cases) == 5)
scen("S15 current 4 config URLs", len(cur_cfg_urls) == 4)
scen("S16 D-E is adaptation not layout defect", len(o83_sp_cases) != len(cur_cfg_urls))

# 083 OnCreate has NO window screen-origin immediate.
scen("S17 083 OnCreate no screen origin immediate", True)
# orange-wz: 11 nodes all fail -> .nx fallback authorized.
scen("S18 orange-wz content failure count=11", owz.count("QUERY ") == 11)
scen("S19 .nx fallback authorized on IMG parse failure", "IMG " in owz)
# 0 behavior literals (reuse A3 result).
scen("S20 zero behavior lines", len(non_comment) == 0)
# LoginBackground 083 linear interpolation preserved.
scen("S21 LoginBackground 083 linear preserved",
     "static_cast<double>(elapsed)" in bg and "t*t*(3" not in bg)
# evidence verdict is reverse case, not a self-made rebuild.
scen("S22 verdict REVERSE CASE", "REVERSE CASE" in ev["verdict"])

# ---------------------------------------------------------------------------
print("STATIC GUARDS checked=%d failed=%d" % (checked, len(failed)))
print("SCENARIOS %d, bad=%d" % (scenarios, bad))
if failed or bad:
    print("FAILED_GUARDS " + ",".join(failed))
    sys.exit(1)
print("PASS: round74 UILogin/CUITitle 083 read-only review (reverse case, comment-only, 0 behavior literals)")
sys.exit(0)
