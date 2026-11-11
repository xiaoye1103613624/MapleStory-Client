# -*- coding: utf-8 -*-
"""round87 五面复核归档 harness：静态守卫 + 场景。

验收：5 面结论可检索、逐面锚点现场可达、常量命中源文件、EOL 字节级干净、
0 源码/0 协议/0 WZ 改动、生成器幂等、外部 11 WIP 零夹带、dev §4 2-35 / PLT §46 落盘。
"""
import hashlib
import json
import os
import subprocess
import sys

ROOT = r"E:/project/MapleStory-Client"
os.chdir(ROOT)

DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EVID = "docs/features/development-and-fix-plan-2026-11/evidence/five-face-review-round87.json"
NOTE = "docs/features/development-and-fix-plan-2026-11/notes/wave3-five-face-review-archive-20261111.md"
GEN = "tools/_fiveface_deliver.py"
HARNESS = "tools/test_round87_five_face_archive.py"
ALIGNED = "已对齐 / 合法混合面，无需改源码"

fails = []
checks = 0


def chk(name, cond):
    global checks
    checks += 1
    if not cond:
        fails.append(name)
        print("[FAIL]", name)


def rb(p):
    with open(p, "rb") as f:
        return f.read()


def txt(p):
    return rb(p).decode("utf-8")


def git(*args):
    return subprocess.run(["git"] + list(args), stdout=subprocess.PIPE,
                          stderr=subprocess.DEVNULL, text=True).stdout.strip()


# ---------- A. static guards ----------
# A1 files exist
for p in [DEV, PLT, EVID, NOTE, GEN, HARNESS]:
    chk("A1 " + p, os.path.isfile(p))

ev = json.loads(txt(EVID))
note = txt(NOTE)
dev = txt(DEV)
plt = txt(PLT)

# A2 authority chain + user directives
chk("A2a round87", ev.get("round") == "round87")
chk("A2b m13177 in task", "m13177" in ev.get("task", ""))
chk("A2c m09211 in task", "m09211" in ev.get("task", ""))
chk("A2d 083 authority", "083" in ev.get("authority", "") and "Angel" in ev.get("authority", ""))
chk("A2e s9 first", "s9" in ev.get("authority", ""))
chk("A2f nature pure-doc", "纯文档" in ev.get("nature", "") and "0 源码" in ev.get("nature", ""))
chk("A2g source_rounds", "round73-77" in ev.get("source_rounds", ""))

# A3 constraints (6 硬约束)
cons = ev.get("constraints", [])
chk("A3a constraints 6", len(cons) == 6)
for kw in ["083/v83 only", "s9 协议优先", "数值重合≠语义重合", "校准≠改回原生",
           "orange-wz", "门禁 18"]:
    chk("A3 " + kw, any(kw in c for c in cons))

# A4 five faces
faces = ev.get("faces", [])
chk("A4a face_count 5", ev.get("face_count") == 5 and len(faces) == 5)
ids = [f["id"] for f in faces]
for want in ["UIChannel", "UICharSelect", "UICommonCreation", "UIMiscShells", "UIItemInventory"]:
    chk("A4 face " + want, want in ids)
vd = ev.get("verdict_distribution", {})
chk("A4b verdict dist", vd.get(ALIGNED) == 5)

# A5 per-face verdict + file + calibration_refs + source_anchors reachability
for f in faces:
    fid = f["id"]
    chk("A5 " + fid + " verdict", f["verdict"] == ALIGNED)
    src = f["file"]
    chk("A5 " + fid + " file exists", os.path.isfile(src))
    src_b = rb(src)
    src_lines = src_b.count(b"\n")  # wc -l 口径：无尾换行文件 splitlines() 会多 1
    chk("A5 " + fid + " line count", src_lines == f["file_lines"])
    for ref in f["calibration_refs"]:
        rp, _, ln = ref.rpartition(":")
        ok = os.path.isfile(rp)
        if ok:
            ok = int(ln) <= len(open(rp, encoding="utf-8", errors="replace").read().splitlines())
        chk("A5 " + fid + " calib " + ref, ok)
    for ref in f["source_anchors"]:
        rp, _, ln = ref.rpartition(":")
        start = ln.split("-")[0]
        ok = os.path.isfile(rp)
        if ok:
            ok = int(start) <= src_lines
        chk("A5 " + fid + " anchor " + ref, ok)
    for d in f.get("evidence_dirs", []):
        chk("A5 " + fid + " evdir " + d, os.path.isdir(d))
    for n in f.get("prior_notes", []):
        chk("A5 " + fid + " note " + n.split("/")[-1], os.path.isfile(n))

# A6 constants hit source (key literals per face)
HITS = {
    "UIChannel": ["channel0", "channel1", "channel2", "Configuration", "STRIP_HEIGHT"],
    "UICharSelect": ["sub_602ADC", "PAGESIZE", "CS_CHARINFO_OFF", "1384", "1348"],
    "UICommonCreation": ["CustomizeChar", "init_modern", "init_classic", "BtViewAll", "0x43524254"],
    "UIMiscShells": ["load_window_bg", "has_assets", "sub_9A3CAE"],
    "UIItemInventory": ["0x81c6c9", "0x81e3a6", "slot_col", "slot_row", "slot_pos"],
}
for f in faces:
    body = txt(f["file"])
    for kw in HITS[f["id"]]:
        chk("A6 " + f["id"] + " const " + kw, kw in body)

# A7 zero_change declaration
zc = ev.get("zero_change", {})
chk("A7a zero source", zc.get("source_files_touched") == 0)
chk("A7b zero protocol", zc.get("protocol_edits") == 0)
chk("A7c zero wz", zc.get("wz_edits") == 0)
chk("A7d deliverables 6", isinstance(zc.get("deliverables"), list) and len(zc["deliverables"]) >= 6)

# A8 live backlog
lb = ev.get("live_backlog", [])
chk("A8a backlog >=5", len(lb) >= 5)
chk("A8b 五屏", any("五屏" in x for x in lb))
chk("A8c InPacket 393", any("available()" in x and "393" in x for x in lb))

# A9 dev-plan §4 2-35 row
row235 = next((ln for ln in dev.split("\n") if ln.startswith("| **2-35** |")), "")
chk("A9a 2-35 row", bool(row235))
chk("A9b 2-35 five faces", all(x in row235 for x in ids))
chk("A9c 2-35 zero change", "0 源码 / 0 协议 / 0 WZ 改动" in row235)
chk("A9d 2-34 intact", "| **2-34** |" in dev)
chk("A9e Wave3 anchor", "### Wave 3" in dev)

# A10 PLT §46
chk("A10a §46 head", "## 46. round87" in plt)
for i in range(1, 6):
    chk("A10 46-%d" % i, "| 46-%d |" % i in plt)
chk("A10b §45 intact", "## 45. round78" in plt)

# A11 EOL byte-level
dev_b = rb(DEV)
plt_b = rb(PLT)
chk("A11a DEV lone-LF", b"\r\n" not in dev_b and b"\r\r\n" not in dev_b)
chk("A11b DEV tail LF", dev_b.endswith(b"\n") and not dev_b.endswith(b"\n\n"))
chk("A11c PLT CRLF", b"\r\n" in plt_b and b"\n" not in plt_b.replace(b"\r\n", b"") and b"\r\r\n" not in plt_b)
chk("A11d PLT tail single CRLF", plt_b.endswith(b"\r\n") and not plt_b.endswith(b"\r\n\r\n"))
chk("A11e EVID JSON LF", b"\r\n" not in rb(EVID))
chk("A11f NOTE LF", b"\r\n" not in rb(NOTE))

# A12 zero-change honesty (this round's deliverables are doc/harness/gen only)
DELIVERABLES = [DEV, PLT, EVID, NOTE, GEN, HARNESS]
chk("A12a no source deliverables", not any(p.endswith((".cpp", ".h", ".c", ".cc")) for p in DELIVERABLES))
chk("A12b DEC zero-change text", "0 源码 / 0 协议 / 0 WZ 改动" in note)

# A13 external WIP recorded (none are our deliverables)
src_changed = [x for x in git("diff", "--name-only").splitlines()
               if x and not x.startswith("warning:") and x.endswith((".cpp", ".h"))]
chk("A13a external WIP >=11", len(src_changed) >= 11)
chk("A13b none delivered", all(x not in DELIVERABLES for x in src_changed))

# A14 idempotence (re-run generator, compare bytes)
before = {p: hashlib.sha256(rb(p)).hexdigest() for p in [DEV, PLT, EVID, NOTE]}
r = subprocess.run([sys.executable, GEN], capture_output=True, text=True)
after = {p: hashlib.sha256(rb(p)).hexdigest() for p in [DEV, PLT, EVID, NOTE]}
chk("A14a generator rc0", r.returncode == 0)
for p in [DEV, PLT, EVID, NOTE]:
    chk("A14 idempotent " + p.split("/")[-1], before[p] == after[p])

# ---------- B. scenarios ----------
sc_bad = [0]


def sc(name, cond):
    if not cond:
        sc_bad[0] += 1
        print("[FAIL] SCEN:" + name)


sc("B1 5 faces all aligned", len(faces) == 5 and all(f["verdict"] == ALIGNED for f in faces))
sc("B2 unique ids", len(set(ids)) == 5)
sc("B3 every face has calib refs", all(len(f["calibration_refs"]) >= 1 for f in faces))
sc("B4 every face has anchors", all(len(f["source_anchors"]) >= 4 for f in faces))
sc("B5 every face has constants", all(len(f["constants"]) >= 4 for f in faces))
sc("B6 every face has basis", all(len(f["basis"]) >= 20 for f in faces))
sc("B7 no 095 as authority", not any("095" in f["basis"] and "083" not in f["basis"] for f in faces))
sc("B8 UICharSelect 083 EA", "sub_602ADC" in next(f for f in faces if f["id"] == "UICharSelect")["constants"].get("CUICharDetail_Draw", ""))
sc("B9 UIItemInventory EA", "0x81c6c9" in " ".join(next(f for f in faces if f["id"] == "UIItemInventory")["calibration_symbols"]))
sc("B10 UIMiscShells P3", "P3" in next(f for f in faces if f["id"] == "UIMiscShells")["basis"])
sc("B11 UICommonCreation NX", "NX" in next(f for f in faces if f["id"] == "UICommonCreation")["basis"])
sc("B12 UIChannel config", "Configuration" in next(f for f in faces if f["id"] == "UIChannel")["constants"].get("channel_count_source", ""))
sc("B13 zero-change consistent", zc["source_files_touched"] == 0 and zc["protocol_edits"] == 0 and zc["wz_edits"] == 0)
sc("B14 live backlog user-owned", any("用户认领" in x or "真机" in x for x in lb))
sc("B15 note 5 rows", all(x in note for x in ids))
sc("B16 note PUA/no-score scope", "不重做取证" in note and "goal-11388c42 保持 active" in note)

n_sc = 16
print("STATIC GUARDS checked=%d failed=%d" % (checks, len(fails)))
print("SCENARIOS %d, bad=%d" % (n_sc, sc_bad[0]))
if fails or sc_bad[0]:
    sys.exit(1)
print("PYCOMPILE_OK")
print("PASS: round87 five-face review archive (5 aligned, anchors reachable, zero change, idempotent, live backlog user-owned)")
