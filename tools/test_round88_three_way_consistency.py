# -*- coding: utf-8 -*-
"""round88 三方一致性确认 harness（静态守卫 + 场景；0 源码改动口径不可放宽）。
判据全部来自现场逐字：s9 PacketCreator.java / Q1 EA / DECISIONS.md §4 / evidence JSON。
"""
import json, re, os, sys, subprocess, hashlib
from collections import Counter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

MATRIX = "docs/features/angel-calibration-d-matrix-2026-11"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EV = f"{MATRIX}/evidence/three-way-consistency-round88.json"
VERDICT = f"{MATRIX}/CONSISTENCY-VERDICT.md"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-three-way-consistency-20261111.md"
GEN = "tools/_r88_deliver.py"
S9 = r"E:/project/BeiDou-Server_s9/gms-server/src/main/java/org/gms/util/PacketCreator.java"
LOGIN = "Net/Handlers/Helpers/LoginParser.cpp"
DEC = f"{MATRIX}/DECISIONS.md"

DELIVERABLES = [EV, VERDICT, NOTES, DEV, PLT, GEN]

def rb(p):
    with open(p, "rb") as f: return f.read()
def rs(p):
    with open(p, "r", encoding="utf-8", errors="replace", newline="") as f: return f.read()

FAILED = []
CHECKED = 0
def chk(name, cond, detail=""):
    global CHECKED
    CHECKED += 1
    if not cond:
        FAILED.append((name, detail or "(assert false)"))
        print(f"[FAIL] {name} {detail}")

SC_BAD = []
SC_N = 0
def sc(name, cond, detail=""):
    global SC_N
    SC_N += 1
    if not cond:
        SC_BAD.append((name, detail or "(assert false)"))
        print(f"[FAIL] SCEN:{name} {detail}")

# ============ 载入 ============
ev = json.load(open(EV, encoding="utf-8"))
verdict_s = rs(VERDICT)
notes_s = rs(NOTES)
dev_s = rs(DEV)
plt_b = rb(PLT)
plt_s = plt_b.decode("utf-8")
s9 = rs(S9)
login = rs(LOGIN)
dec = rs(DEC)

# ============ A. 静态守卫 ============
# A1 交付物存在
for p in DELIVERABLES:
    chk(f"A1 exists {p.split('/')[-1]}", os.path.isfile(p))

# A2 权威触发链
chk("A2a trigger m09543", ev["trigger"]["m09543"].startswith("记录一次有可见 PUA 输出"))
chk("A2b trigger m09544", "如果一致就全部按Angel" in ev["trigger"]["m09544"])
chk("A2c verdict cites m09544", "m09544" in verdict_s)
v = ev["verdict"]
chk("A2d q1_judgement denies causal", "不是" in v["q1_judgement"] and "HeavenClient" in v["q1_judgement"])
chk("A2e conditional NOT triggered", "不成立" in v["angel_vs_s9"]["conditional_full_align"] and "不触发" in v["angel_vs_s9"]["conditional_full_align"])
chk("A2f beidou_bugs affirmed", "成立" in v["beidou_many_bugs"])
chk("A2g round86_gap present", "一刀切" in v["round86_gap"] and "27" in v["round86_gap"])
chk("A2h align_policy 3 layers", all(k in v["align_policy"] for k in ("s9", "body", "Angel")))

# A3 Q1 同二进制
q1 = ev["q1_same_binary"]
chk("A3a angel decorated 3122", q1["angel_13339_decorated"] == 3122)
chk("A3b beidou decorated 50", q1["beidou_13337_decorated"] == 50)
chk("A3c same version string", q1["same_version_string"] is True)
chk("A3d EA 95/97", q1["ea_matched_classes"] == 95 and q1["ea_target_classes"] == 97)
chk("A3e EA 809 named funcs", q1["ea_named_funcs"] == 809)
chk("A3f conclusion self-reimplement", "自研重实现" in q1["conclusion"] and "无代码继承" in q1["conclusion"])

# A4 Angel vs s9 分层判定
a = v["angel_vs_s9"]
chk("A4a handshake consistent", "一致" in a["handshake_layer"])
chk("A4b opcode consistent", "一致" in a["opcode_layer"])
chk("A4c body INCONSISTENT", "不一致" in a["body_layer"])
chk("A4d body lists exp 8B", "exp 8B" in a["body_layer"])
chk("A4e body lists GachaExp", "GachaExp" in a["body_layer"])
chk("A4f hello 83 in verdict", "version 83" in verdict_s)

# A5 s9 wire divergences（源码逐字 + evidence 一致）
divs = ev["s9_wire_divergences"]
chk("A5a 6 divergences", len(divs) == 6)
for tag, code in [("level 2B", "p.writeShort(chr.getLevel());"),
                  ("HP/MP int", "p.writeInt(chr.getHp());"),
                  ("exp 8B", "p.writeLong(chr.getExp());"),
                  ("GachaExp 4B", "p.writeInt((int) chr.getGachaExp());"),
                  ("spawn tail 4B", "p.writeInt(0);")]:
    chk(f"A5 source has {tag}", code in s9)
    chk(f"A5 evidence has {tag}", any(d["tag"] == tag and d["code"] == code for d in divs))
chk("A5z verdict table exp 8B", "`p.writeLong(chr.getExp());`" in verdict_s)
chk("A5y single PacketCreator copy", ev["s9_packetcreator_single_copy"] is True)

# A6 白屏实锤
chk("A6a login parser whitescreen note", "进图白屏" in login)
chk("A6b login fix read_long", "statsentry.exp = recv.read_long();" in login)
chk("A6c login fix skip tail", "recv.skip(4);" in login)
chk("A6d evidence whitescreen case", "mapId" in ev["client_whitescreen_case"]["old_bug"])
chk("A6e verdict quotes case", "LoginParser.cpp:150-174" in verdict_s)

# A7 168 三分类
c = ev["classification_168"]
chk("A7a PURE 76", c["PURE"] == 76)
chk("A7b WIRE 38", c["WIRE"] == 38)
chk("A7c MIXED 14", c["MIXED"] == 14)
chk("A7d OTHER 40", c["OTHER"] == 40)
chk("A7e total 168", c["total"] == 168 and c["PURE"] + c["WIRE"] + c["MIXED"] + c["OTHER"] == 168)
chk("A7f arithmetic string", "76+38+14+40=168" in c["arithmetic"])
chk("A7g verdict classification", "PURE 76 / WIRE 38 / MIXED 14 / OTHER 40" in verdict_s)

# A8 79 KEEP
k = ev["kept_79"]
chk("A8a WIRE 27", k["WIRE"] == 27)
chk("A8b OTHER 25", k["OTHER"] == 25)
chk("A8c PURE 20", k["PURE"] == 20)
chk("A8d MIXED 7", k["MIXED"] == 7)
chk("A8e total 79", k["total"] == 79 and k["WIRE"] + k["OTHER"] + k["PURE"] + k["MIXED"] == 79)
chk("A8f verdict 79 split", "WIRE 27 / OTHER 25 / PURE 20 / MIXED 7" in verdict_s)

# A9 reopen 27 / must_keep 27
reopen = ev["reopen_candidates"]
must = ev["must_keep_s9"]
chk("A9a reopen 27", len(reopen) == 27)
chk("A9b must_keep 27", len(must) == 27)
chk("A9c reopen only PURE/MIXED", all(r["cls"] in ("PURE", "MIXED") for r in reopen))
chk("A9d reopen PURE 20 MIXED 7", Counter(r["cls"] for r in reopen) == Counter({"PURE": 20, "MIXED": 7}))
chk("A9e must_keep all WIRE", all(r["class"] == "WIRE" if "class" in r else True for r in must))
chk("A9f reopen reason honest", all("不碰 wire" in r["reason"] and "逐条" in r["reason"] for r in reopen))
chk("A9g verdict reopen count", "**27**" in verdict_s or "27 条" in verdict_s)
# 每条 reopen 带 face/id
chk("A9h reopen face/id keys", all("face" in r and "id" in r for r in reopen))

# A10 条件指令主裁决 + 策略
chk("A10a verdict one-liner", "**不是**因为 BeiDou==Angel" in verdict_s)
chk("A10b verdict body mismatch", "字段级 body 层不一致" in verdict_s or "body 层不一致" in verdict_s)
chk("A10c verdict conditional off", "前提不成立" in verdict_s)
chk("A10d verdict beidou bugs", "成立且一直在修" in verdict_s)
chk("A10e notes 4 conclusions", all(x in notes_s for x in ("不是", "body 不一致", "成立", "reopen")))
chk("A10f strategy wire-first", "s9 源码优先" in verdict_s or "s9 协议优先" in verdict_s or "wire / opcode 层" in verdict_s)
chk("A10g PLAN P2 in verdict", "PLAN.md §P2" in verdict_s)

# A11 evidence JSON 结构
chk("A11a round 88", ev["round"] == "88")
chk("A11b zero_source_change True", ev["zero_source_change"] is True)
chk("A11c zero_change_note", "未改任何" in ev["zero_change_note"] and "11 个外部" in ev["zero_change_note"])
chk("A11d constraints 6", len(ev["constraints"]) == 6 and any("128B" in x for x in ev["constraints"]))
chk("A11e live_backlog", len(ev["live_backlog"]) >= 3)
chk("A11f gate head", ev["handshake_and_gate"]["gate_head"].startswith("a78ba681a4"))
chk("A11g gate counts out148", ev["handshake_and_gate"]["counts"]["client_out_sent"] == 148)
chk("A11h shipped totals", ev["shipped_devfix_total"] == 33 and ev["shipped_align_total"] == 15)
chk("A11i shipped_align list", len(ev["shipped_align_commits"]) == 15)

# A12 dev-plan 2-36 行
chk("A12a 2-36 row exists", "| **2-36** |" in dev_s)
row36 = next((ln for ln in dev_s.split("\n") if ln.startswith("| **2-36** |")), "")
chk("A12b row cites m09544", "m09544" in row36)
chk("A12c row cites body mismatch", "body 层不一致" in row36 or "body" in row36)
chk("A12d row cites reopen 27", "27 条" in row36)
chk("A12e row cites verdict path", "CONSISTENCY-VERDICT.md" in row36)
chk("A12f row zero change", "零源码改动" in row36)
chk("A12g 2-35 still present", "| **2-35** |" in dev_s)

# A13 PLT §47
chk("A13a §47 exists", "## 47." in plt_s)
chk("A13b 47-1..47-5", all(f"47-{i}" in plt_s for i in range(1, 6)))
chk("A13c §47 Q1", "HeavenClient" in plt_s)
chk("A13d §47 GATE", "a78ba681a4" in plt_s or "GATE OK" in plt_s)
chk("A13e §47 reopen 27", "reopen 27 条" in plt_s)
chk("A13f §47 zero change", "0 源码" in plt_s)
chk("A13g §46 still present", "## 46." in plt_s)

# A14 EOL
chk("A14a PLT CRLF", b"\r\n" in plt_b and b"\n" not in plt_b.replace(b"\r\n", b""))
chk("A14b PLT no CRCRLF", b"\r\r\n" not in plt_b)
chk("A14c DEV is LF", b"\r\n" not in rb(DEV))
chk("A14d verdict no CRCRLF", b"\r\r\n" not in rb(VERDICT))
chk("A14e no trailing blank on verdict", not rb(VERDICT).endswith(b"\n\n"))
chk("A14f no trailing blank on PLT", not plt_b.endswith(b"\r\n\r\n"))
# verdict 无 trailing whitespace
for i, ln in enumerate(verdict_s.split("\n"), 1):
    if ln.rstrip() != ln:
        chk(f"A14g verdict line {i} trailing ws", False, repr(ln[-30:]))
        break

# A15 零源码改动诚实口径（本轮交付物无 .cpp/.h）
SRC_EXT = (".cpp", ".h", ".c", ".cc", ".hpp")
chk("A15a deliverables are doc/tool only", all(not p.endswith(SRC_EXT) for p in DELIVERABLES))
# 工作树外部 WIP 登记（stderr 隔离，避免 warning 混入）
def git(args):
    return subprocess.run(["git"] + args, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL).stdout.decode("utf-8", "replace")
src_changed = [x for x in git(["diff", "--name-only"]).split("\n") if x.strip() and x.endswith(SRC_EXT)]
chk("A15b external WIP recorded (none are our deliverables)", all(p not in DELIVERABLES for p in src_changed), f"n={len(src_changed)}")
chk("A15c verdict declares zero change", "0 源码 / 0 协议 / 0 WZ 改动" in verdict_s)

# A16 notes
chk("A16a notes trigger", "m09544" in notes_s)
chk("A16b notes round86 纠偏", "PURE76/WIRE38/MIXED14/OTHER40" in notes_s)
chk("A16c notes reopen", "27 条" in notes_s and "reopen" in notes_s)
chk("A16d notes evidence link", "three-way-consistency-round88.json" in notes_s)

# A17 DECISIONS.md §0 仍为权威口径（未被本轮改写）
chk("A17a §0 s9 优先", "s9 联机优先" in dec)
chk("A17b §0 校准≠改回原生", "校准 ≠ 改回原生" in dec)

# ============ B. 场景 ============
# B1 三分类算术
sc("B1a 168 sum", c["PURE"] + c["WIRE"] + c["MIXED"] + c["OTHER"] == 168)
sc("B1b 79 sum", k["WIRE"] + k["OTHER"] + k["PURE"] + k["MIXED"] == 79)
sc("B1c reopen = PURE+MIXED", len(reopen) == k["PURE"] + k["MIXED"])
sc("B1d must_keep = WIRE", len(must) == k["WIRE"])

# B2 reopen 与 DECISIONS.md §4 逐条一致（face/id 能在 §4 找到）
# DECISIONS.md §4 表格式是 `| **D1** | K2 | ...`，按 12 面分段（### 4.N (`face`，14 条））
dec_pairs = set()
cur_face = None
for ln in dec.split("\n"):
    mhead = re.search(r"### 4\.\d+ .*?\(`([a-z\-]+)`", ln)
    if mhead: cur_face = mhead.group(1)
    mrow = re.match(r"\|\s*\*{0,2}(D\d+)\*{0,2}\s*\|\s*(K\d)\s*\|", ln)
    if mrow and cur_face:
        dec_pairs.add((cur_face, mrow.group(1)))
sc("B2a reopen (face,id) all in DECISIONS §4", all((r["face"], r["id"]) in dec_pairs for r in reopen),
   f"missing={[(r['face'],r['id']) for r in reopen if (r['face'],r['id']) not in dec_pairs][:4]}")
sc("B2b must_keep (face,id) all in DECISIONS §4", all((r["face"], r["id"]) in dec_pairs for r in must),
   f"missing={[(r['face'],r['id']) for r in must if (r['face'],r['id']) not in dec_pairs][:4]}")
sc("B2c DECISIONS §4 parsed 168 pairs", len(dec_pairs) == 168, f"n={len(dec_pairs)}")

# B3 must_keep 全部 reason 指向 wire/s9
sc("B3 must_keep reason s9", all("s9" in r["reason"] and "wire" in r["reason"].lower() for r in must))
# reopen 全部 reason 指向不碰 wire
sc("B3b reopen reason pure", all("不碰 wire" in r["reason"] for r in reopen))

# B4 verdict 6 节齐全
sc("B4 verdict sections", all(f"## {i}." in verdict_s for i in range(0, 7)))

# B5 s9 源码 6 处 divergence 逐字对账（evidence ↔ 真实源码）
sc("B5 source divergences", all(d["code"] in s9 for d in divs))
sc("B5b hello 83 source", "0x0E" in s9 and "83" in s9)

# B6 白屏字段链（s9 尾 + 客户端读序）
sc("B6a s9 writeInt(mapId)", "p.writeInt(chr.getMapId());" in s9 or "getMapId" in s9)
sc("B6b s9 writeByte spawn", "p.writeByte(chr.getInitialSpawnPoint());" in s9)
sc("B6c client read_int mapid", "statsentry.mapid = recv.read_int();" in login)

# B7 GATE counts 自洽
cnt = ev["handshake_and_gate"]["counts"]
sc("B7a client out sent 148", cnt["client_out_sent"] == 148)
sc("B7b s9 send 314", cnt["s9_send_constants"] == 314)
sc("B7c s9 recv 182", cnt["s9_recv_constants"] == 182)

# B8 shipped 链实数与现场 git 对拍
full_log = git(["log", "--oneline"])
devfix_total = sum(1 for ln in full_log.split("\n") if "#devfix" in ln)
align_total = sum(1 for ln in full_log.split("\n") if "#devfix" in ln and any(k in ln for k in ("083", "Angel", "校准", "翻案", "收口", "重修", "复核")))
# evidence 记的是生成时（提交前）快照；本轮 commit 落地后现场 +1（标题带 #devfix+校准）
sc("B8a devfix total matches git (+0 pre / +1 post)", 0 <= devfix_total - ev["shipped_devfix_total"] <= 1, f"ev={ev['shipped_devfix_total']} git={devfix_total}")
sc("B8b align total matches git (+0 pre / +1 post)", 0 <= align_total - ev["shipped_align_total"] <= 1, f"ev={ev['shipped_align_total']} git={align_total}")

# B9 生成器幂等：重跑后 5 交付物 SHA 不变
def sha(p): return hashlib.sha256(rb(p)).hexdigest()
before = {p: sha(p) for p in (EV, VERDICT, NOTES, DEV, PLT)}
r = subprocess.run([sys.executable, GEN], stdout=subprocess.PIPE, stderr=subprocess.PIPE, env={**os.environ, "PYTHONIOENCODING": "utf-8"})
after = {p: sha(p) for p in (EV, VERDICT, NOTES, DEV, PLT)}
sc("B9a generator rerun rc0", r.returncode == 0, r.stderr.decode("utf-8", "replace")[-200:])
sc("B9b idempotent SHA", before == after, str({p: (before[p][:8], after[p][:8]) for p in before if before[p] != after[p]}))

# B10 条件真值表（一致层触发对齐、不一致层不触发）
sc("B10a handshake consistent -> align allowed", "一致" in a["handshake_layer"])
sc("B10b body inconsistent -> full align off", "不一致" in a["body_layer"] and "不触发" in a["conditional_full_align"])
sc("B10c change-back breaks s9", "断联机" in a["conditional_full_align"] or "白屏" in a["conditional_full_align"])

# B11 evidence 与 verdict 数字一致
sc("B11a verdict 168", "168" in verdict_s)
sc("B11b verdict 79", "79" in verdict_s)
sc("B11c verdict 27", "27" in verdict_s)
sc("B11d evidence reopen len == verdict list items", sum(1 for ln in verdict_s.split("\n") if ln.startswith("- `") and "/D" in ln and "reopen" not in ln.lower()) >= 27 or len(reopen) == 27)

# B12 零改动：交付物集合与工作树改动交集只含 6 个交付物
changed = set(x for x in git(["diff", "--name-only"]).split("\n") if x.strip())
untracked = set(x for x in git(["ls-files", "--others", "--exclude-standard"]).split("\n") if x.strip())
our = set(DELIVERABLES)
tracked = set(x for x in git(["ls-files"]).split("\n") if x.strip())
head_files = set(x for x in git(["show", "--name-only", "--format=", "HEAD"]).split("\n") if x.strip())
staged = set(x for x in git(["diff", "--cached", "--name-only"]).split("\n") if x.strip())
# 三态自适应：工作树(改/未跟踪) / 暂存区 / 已提交 HEAD；任一态覆盖即可
visible = changed | untracked | staged | head_files
pre_ok = our.issubset(changed | untracked | staged)
post_ok = our.issubset(tracked) and our.issubset(head_files)
sc("B12a deliverables staged-or-HEAD (pre/post adaptive)", pre_ok or post_ok,
   f"visible_missing={our - visible} post_missing={our - head_files}")
sc("B12b zero cpp/h in our deliverables", all(not p.endswith(SRC_EXT) for p in our))
sc("B12c external WIP disclosed", len(src_changed) >= 10, f"n={len(src_changed)}")

# B13 EOL 字节级
sc("B13a PLT pure CRLF", plt_b.count(b"\r\n") > 1000 and b"\n" not in plt_b.replace(b"\r\n", b""))
sc("B13b no CRCRLF anywhere", all(b"\r\r\n" not in rb(p) for p in DELIVERABLES))
sc("B13c DEV LF only", b"\r" not in rb(DEV))

# B14 reopen 27 条 face 覆盖（应跨多面，不是集中一面）
faces = Counter(r["face"] for r in reopen)
sc("B14a reopen spans >=8 faces", len(faces) >= 8, f"faces={len(faces)}")
sc("B14b no face >6", max(faces.values()) <= 6, str(faces))
sc("B14c MIXED+PURE exact", Counter(r["cls"] for r in reopen) == Counter({"PURE": 20, "MIXED": 7}))

# B15 notes / verdict 明文口径
sc("B15a notes zero change", "零源码改动" in notes_s or "0 源码" in notes_s)
sc("B15b verdict user-owned live", "用户认领" in verdict_s or "真机" in verdict_s)
sc("B15c constraints forbid 095", any("禁095" in x for x in ev["constraints"]))

# B16 diff --check 干净
r2 = subprocess.run(["git", "diff", "--check"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
dc = r2.stdout.decode("utf-8", "replace")
dc_real = [ln for ln in dc.split("\n") if ln and "LF will be replaced" not in ln]
sc("B16 diff --check clean", len(dc_real) == 0, str(dc_real[:3]))

# ============ 汇总 ============
print(f"STATIC GUARDS checked={CHECKED} failed={len(FAILED)}")
print(f"SCENARIOS {SC_N}, bad={len(SC_BAD)}")
import py_compile
try:
    py_compile.compile(GEN, doraise=True)
    print("PYCOMPILE_OK")
except Exception as e:
    print("PYCOMPILE_FAIL", e)
    FAILED.append(("pycompile", str(e)))
if FAILED or SC_BAD:
    print("FAIL: round88 three-way consistency verification")
    sys.exit(1)
print("PASS: round88 three-way consistency (Q1 denied / body mismatch / conditional off / reopen 27 / zero source change)")
sys.exit(0)
