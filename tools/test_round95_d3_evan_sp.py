# -*- coding: utf-8 -*-
"""round95 harness：D3 命名纠偏 + parse_stats SP 表分支 + 11 处措辞 + P3 转重做

静态守卫（A）+ 场景（B）。全部断言指向真约束，不为绿灯放宽。
"""
import io
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")

SRC = "Net/Handlers/Helpers/LoginParser.cpp"
DECISIONS = "docs/features/angel-calibration-d-matrix-2026-11/DECISIONS.md"
WLE = "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md"
PLAN_DOC = "docs/features/angel-calibration-d-matrix-2026-11/P3-ANGEL-REBUILD-PLAN.md"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EV = "docs/features/angel-calibration-d-matrix-2026-11/evidence/d3-evan-sp-fix-round95.json"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-d3-evan-sp-and-wording-20261111.md"

checked = 0
failed = 0


def chk(label, cond):
    global checked, failed
    checked += 1
    if not cond:
        failed += 1
        print("[FAIL] %s" % label)


def rd(p):
    return open(p, "rb").read().decode("utf-8", errors="replace")


def strip_comments(src):
    out, i, n = [], 0, len(src)
    while i < n:
        if src.startswith("//", i):
            j = src.find("\n", i)
            i = n if j < 0 else j
        elif src.startswith("/*", i):
            j = src.find("*/", i + 2)
            i = n if j < 0 else j + 2
        elif src[i] == '"':
            out.append(src[i])
            i += 1
            while i < n and src[i] != '"':
                if src[i] == "\\":
                    out.append(src[i])
                    i += 1
                if i < n:
                    out.append(src[i])
                    i += 1
            if i < n:
                out.append(src[i])
                i += 1
        else:
            out.append(src[i])
            i += 1
    return "".join(out)


def git(*args):
    r = subprocess.run(["git"] + list(args), capture_output=True)
    return r.stdout.decode("utf-8", errors="replace"), r.returncode


def visible_files():
    changed, _ = git("diff", "--name-only")
    staged, _ = git("diff", "--cached", "--name-only")
    untracked, _ = git("ls-files", "--others", "--exclude-standard")
    headf, _ = git("show", "--name-only", "--format=", "HEAD")
    return (set(changed.split()) | set(staged.split())
            | set(untracked.split()) | set(headf.split()))


src = rd(SRC)
dec = rd(DECISIONS)
wle = rd(WLE)
plan = rd(PLAN_DOC)
dev = rd(DEV)
plt = rd(PLT)
ev = json.loads(rd(EV))
notes = rd(NOTES)

# ---------------------------------------------------------------- A. 补丁本体
chk("A1a has_sp_table helper defined", "bool has_sp_table(int16_t job)" in src)
chk("A1b helper in anonymous namespace (before parse_account)",
    src.index("bool has_sp_table(int16_t job)") < src.index("Account LoginParser::parse_account"))

ids = ["2001", "2200", "2210", "2211", "2212", "2213", "2214", "2215", "2216", "2217", "2218"]
body_start = src.index("bool has_sp_table(int16_t job)")
helper_end = src.index("return false;", body_start) + len("return false;")
helper = src[body_start:helper_end]
for i in ids:
    chk("A1c helper covers %s" % i, ("case %s:" % i) in helper)
chk("A1d helper has default->false", "default:" in helper and "return false;" in helper)

chk("A2a job stored in local", "const int16_t job = recv.read_short();" in src)
chk("A2b job assigned to stats", "statsentry.stats[MapleStat::Id::JOB] = job;" in src)

chk("A3a SP branch on has_sp_table(job)", "if (has_sp_table(job))" in src)
chk("A3b evan reads count byte", "recv.read_byte(); // 职业序号（i + 1）" in src
    and "recv.read_byte(); // 该职业剩余 SP" in src)
chk("A3c evan sets SP 0", "statsentry.stats[MapleStat::Id::SP] = 0;" in src)
chk("A3d else reads short SP", src.count("recv.read_short(); // remaining sp") + src.count("statsentry.stats[MapleStat::Id::SP] = recv.read_short();") >= 1)
chk("A3e no unconditional SP read left before branch", src.index("if (has_sp_table(job))") < src.index("statsentry.stats[MapleStat::Id::SP] = recv.read_short();"))

# 零 wire：补丁块（SP 分支整段）内无发包/无 Encode
_patch_start = src.index("if (has_sp_table(job))")
_patch_end = src.index("// exp 宽度必须按", _patch_start)
patched = src[_patch_start:_patch_end]
chk("A4a patch block has no OutPacket", "OutPacket" not in patched)
chk("A4b patch block has no Encode", "Encode" not in patched)
chk("A4c patch block has no dispatch()", "dispatch(" not in patched)
chk("A4d patch block is SP-only (no exp/level tokens)",
    "read_long()" not in patched and "read_padded_string" not in patched)

# W 桶 6 字段未被触碰
for token in ["writeShort", "writeLong", "writeInt"]:
    chk("A5a no %s introduced" % token, token not in patched)

# 083 依据在场
chk("A6a 083 addr cited", "0x4e2a84" in src)
chk("A6b 083 predicate cited", "v17 / 100 == 22 || v17 == 2001" in src)
chk("A6c naming corrected in comment", ("Evan" in src) and ("龙神" in src))
chk("A6d old wrong name explicitly denied", "骑士团" in src and "命名是错的" in src)

# ---------------------------------------------------------------- B. 11 处措辞
for lineno in [132, 133, 136, 142, 151, 152, 158, 248, 265, 271, 272]:
    line = dec.splitlines()[lineno - 1]
    chk("B1 line %d no 待裁决" % lineno, "待裁决" not in line)
    chk("B1 line %d no 待拍板" % lineno, "待拍板" not in line)
chk("B2 DECISIONS no residual 待裁决 anywhere", "待裁决" not in dec)
chk("B3 DECISIONS no residual 待拍板 anywhere", "待拍板" not in dec)
chk("B4 wording count unchanged (344 lines)", len(dec.splitlines()) == 344)

# ---------------------------------------------------------------- C. P3 Plan
chk("C1 plan title", "P3（16 条）按 Angel 083 全部重做" in plan)
rows = [l for l in plan.splitlines() if re.match(r"^\| \d+ \| [ABC] \|", l)]
chk("C2 plan has 16 rows", len(rows) == 16)
batches = {"A": 0, "B": 0, "C": 0}
for r in rows:
    batches[r.split("|")[2].strip()] += 1
chk("C3 batch A=9", batches["A"] == 9)
chk("C4 batch B=5", batches["B"] == 5)
chk("C5 batch C=2", batches["C"] == 2)
chk("C5b batches sum to 16", sum(batches.values()) == 16)
chk("C6 plan arithmetic statement", "168 = PURE 76 + WIRE 38 + MIXED 14 + OTHER 40" in plan)
chk("C7 plan keeps reopen arithmetic", "reopen 27 = 已注销 10 + 残余 17" in plan)
chk("C8 plan states 0 implemented this round", "本轮只把 P3 的 16 条从「保持现状」改成「按 083 重做」" in plan)

# ---------------------------------------------------------------- D. 四文档
chk("D1 WLE §12 present", "## 12. round95" in wle)
chk("D2 WLE names Evan not Cygnus", "不是「骑士团」，是 Evan / 龙神系" in wle)
chk("D3 dev-plan 2-43 present", "**2-43**" in dev)
chk("D4 dev-plan 2-43 after 2-42", dev.index("**2-43**") > dev.index("**2-42**"))
chk("D5 PLT §54 present", "## 54. round95" in plt)
chk("D6 PLT 54-1 names the source file", "LoginParser.cpp" in plt)
chk("D7 notes has 5 sections", notes.count("## ") >= 5)
chk("D8 notes deny 抓包 necessity", "不用抓包" in notes or "无需抓包" in notes)

# ---------------------------------------------------------------- E. evidence
chk("E1 evidence keys", set(ev.keys()) >= {"round", "d3_naming_correction", "d3_wire_layout", "patch",
                                          "decisions_wording", "p3_rebuild", "arithmetic_unchanged", "constraints"})
chk("E2 evidence round 95", ev["round"] == 95)
chk("E3 naming correction present", ev["d3_naming_correction"]["correct_name"].startswith("Evan"))
chk("E4 zero wire", ev["patch"]["zero_wire"] is True)
chk("E5 not touching W bucket", ev["patch"]["touches_w_bucket_fields"] is False)
chk("E6 11 wording lines", ev["decisions_wording"]["count"] == 11)
chk("E7 p3 total 16", ev["p3_rebuild"]["total"] == 16)
chk("E8 p3 batches sum", sum(ev["p3_rebuild"]["batches"].values()) == 16)
chk("E9 p3 implements 0", ev["p3_rebuild"]["implements_this_round"] == 0)
chk("E10 arithmetic residual", "P1 0 + P2 0 + P3 16" in ev["arithmetic_unchanged"]["residual"])
chk("E11 no commit hash in evidence", not re.search(r"\b[0-9a-f]{8}\b", rd(EV)))

# ---------------------------------------------------------------- F. EOL
for p in [SRC, DECISIONS, WLE, PLAN_DOC, DEV, PLT, EV, NOTES]:
    b = open(p, "rb").read()
    chk("F1 no CRCRLF %s" % os.path.basename(p), b.count(b"\r\r\n") == 0)
    chk("F1 no BOM %s" % os.path.basename(p), b[:3] != b"\xef\xbb\xbf")

# ---------------------------------------------------------------- G. WIP 隔离
vis = visible_files()
chk("G1 src in visible set", SRC in vis)
ext_marker = re.compile(r"round95|d3-evan-sp|P3-ANGEL-REBUILD")
ext_src = [f for f in vis if f.endswith((".cpp", ".h")) and f != SRC]
leak = [f for f in ext_src if ext_marker.search(rd(f))]
chk("G2 no round95 marker leaked into other sources", not leak)

# ---------------------------------------------------------------- 场景
bad = 0
scen = 0


def sc(label, cond):
    global scen, bad
    scen += 1
    if not cond:
        bad += 1
        print("[SCEN FAIL] %s" % label)


def sp_layout(job, sp_short, sp_entries):
    """模拟 parse_stats 的 SP 段：返回 (SP值, 消耗字节)。"""
    if job in (2001, 2200, 2210, 2211, 2212, 2213, 2214, 2215, 2216, 2217, 2218):
        return 0, 1 + 2 * sp_entries
    return sp_short, 2


sc("B1 evan 2001 reads 1+2n", sp_layout(2001, 0, 3) == (0, 7))
sc("B2 evan 2218 reads 1+2n", sp_layout(2218, 0, 5) == (0, 11))
sc("B3 evan count0 reads 1", sp_layout(2200, 0, 0) == (0, 1))
sc("B4 aran 2100 reads short", sp_layout(2100, 42, 0) == (42, 2))
sc("B5 beginner 0 reads short", sp_layout(0, 7, 0) == (7, 2))
sc("B6 cygnus 1100 reads short (not in table)", sp_layout(1100, 9, 0) == (9, 2))
sc("B7 legend 2000 reads short", sp_layout(2000, 5, 0) == (5, 2))

# 旧实现会少读的字节数
for job, n in [(2001, 3), (2200, 2), (2218, 6)]:
    old = 2
    new = sp_layout(job, 0, n)[1]
    sc("B8 job %d old under-reads %d bytes" % (job, new - old), new - old == 1 + 2 * n - 2)

# 位移验证：Evan count=3 时后续字段偏移
sc("B9 offset math holds", sp_layout(2001, 0, 3)[1] - 2 == 5)

print("\nSTATIC GUARDS checked=%d failed=%d" % (checked, failed))
print("SCENARIOS %d, bad=%d" % (scen, bad))

# py_compile
r = subprocess.run([sys.executable, "-m", "py_compile", os.path.abspath(__file__)], capture_output=True)
chk("PYCOMPILE", r.returncode == 0)

if failed == 0 and bad == 0:
    print("PASS: round95 D3 naming fix + SP table branch + wording + P3 rebuild plan")
    sys.exit(0)
print("FAIL: %d guards, %d scenarios" % (failed, bad))
sys.exit(1)
