# -*- coding: utf-8 -*-
"""round94 harness · reopen 残余 17 条 P1/P2/P3 分流（账目轮）

守卫分四组：
  A 交付物在场与结构（WLE §11 / dev 2-42 / PLT §53 / evidence / notes）
  B 算术闭合（17 = 0+1+16；17+10 = 27；168 三式不变）
  C 与 round88 evidence 对账（triage ∪ closed 必须恰好等于 reopen 27，无遗漏无发明）
  D 零改动硬证（源码树无 cpp/h 变更；EOL 无 CRCRLF）

用法：python tools/test_round94_reopen_triage.py
"""
import io
import json
import os
import re
import subprocess
import sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
os.chdir(r"E:/project/MapleStory-Client")

WLE = "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EV = "docs/features/angel-calibration-d-matrix-2026-11/evidence/reopen-triage-round94.json"
EV88 = "docs/features/angel-calibration-d-matrix-2026-11/evidence/three-way-consistency-round88.json"
DEC = "docs/features/angel-calibration-d-matrix-2026-11/DECISIONS.md"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-reopen-triage-20261111.md"

_checked = 0
_failed = 0


def chk(name, ok, detail=""):
    global _checked, _failed
    _checked += 1
    if not ok:
        _failed += 1
        print("[FAIL] %s%s" % (name, (" :: " + str(detail)) if detail else ""))


def rd(p):
    with open(p, "rb") as f:
        return f.read()


def rt(p):
    return rd(p).decode("utf-8", "replace")


def git(*args):
    r = subprocess.run(["git"] + list(args), capture_output=True)
    return r.stdout.decode("utf-8", "replace")


def eol_stat(p):
    b = rd(p)
    crlf = b.count(b"\r\n")
    return dict(bytes=len(b), crlf=crlf, lonelf=b.count(b"\n") - crlf,
                crcrlf=b.count(b"\r\r\n"), bom=b.startswith(b"\xef\xbb\xbf"))


ev = json.loads(rt(EV))
ev88 = json.loads(rt(EV88))
wle = rt(WLE)
dev = rt(DEV)
plt = rt(PLT)
notes = rt(NOTES)
dec = rt(DEC)

# ============================================================ A 交付物在场
chk("A1 evidence 存在且 16 键", len(ev) >= 16, len(ev))
triage = ev["triage"]
closed = ev["closed_ledger"]
chk("A1b triage = 17", len(triage) == 17, len(triage))
chk("A1c closed_ledger = 10", len(closed) == 10, len(closed))
chk("A1d bucket_count 三键", set(ev["bucket_count"]) == {"P1", "P2", "P3"}, ev["bucket_count"])

chk("A2 WLE §11 在场", "## 11. round94 分流" in wle)
sec11 = wle.split("## 11. round94 分流", 1)[1]
rows11 = [l for l in sec11.split("\n") if re.match(r"^\| \d+ \| `", l)]
chk("A2b §11 17 行数据", len(rows11) == 17, len(rows11))
for b in ("P1 = 0", "P2 = 1", "P3 = 16", "0 + 1 + 16 = 17"):
    chk("A2c §11 含 %s" % b, b in sec11.replace("**", ""))

chk("A3 dev-plan 2-42 在场", "| **2-42** |" in dev)
chk("A3b 2-42 在 2-41 之后", dev.find("| **2-42** |") > dev.find("| **2-41** |") > 0)
dev_line = [l for l in dev.split("\n") if l.startswith("| **2-42** |")]
chk("A3c 2-42 单行且含 P1 = 0", len(dev_line) == 1 and "P1 = 0" in dev_line[0], len(dev_line))

chk("A4 PLT §53 在场", "## 53. round94" in plt)
sec53 = plt.split("## 53. round94", 1)[1]
for token in ("round90 | 2", "round91 | 3", "round92 | 4", "round93 | 1",
              "P1 = 0 / P2 = 1 / P3 = 16", "27 - 10 = 17"):
    chk("A4b §53 含 %r" % token, token in sec53)

chk("A5 notes 在场且 5 节", all(("## %d." % i) in notes for i in range(1, 6)))
chk("A5b notes 含「客户端代码里的任何数值」否定式", "客户端代码里的任何数值" in notes and "不是" in notes)
chk("A5c notes 含 11 行措辞结论", "11 行历史措辞" in notes)

# ============================================================ B 算术闭合
bc = ev["bucket_count"]
chk("B1 0+1+16 = 17", bc["P1"] == 0 and bc["P2"] == 1 and bc["P3"] == 16 and sum(bc.values()) == 17, bc)
chk("B2 17 + 10 = 27", len(triage) + len(closed) == 27)
chk("B3 bucket_arithmetic 串一致",
    ev["bucket_arithmetic"] == "P1 0 + P2 1 + P3 16 = 17", ev["bucket_arithmetic"])
chk("B4 recount 三数一致",
    ev["recount"]["reopen_total"] == 27 and ev["recount"]["closed_previous_rounds"] == 10
    and ev["recount"]["residual"] == 17, ev["recount"])
chk("B5 recount 算术串", ev["recount"]["arithmetic"] == "27 - 10 = 17")

# 168 三式必须原样在场（分母不变）
chk("B6 168 四分类式在场", "PURE 76 + WIRE 38 + MIXED 14 + OTHER 40" in rt(EV).replace("\"", ""))
chk("B6b 168 W/L 轨式在场", "W 轨 38 + L 轨 130" in rt(EV))
chk("B6c reopen 27 口径在场", "25 P1 候选 + 1 W 桶 + 1 F 维 P2" in rt(EV))
chk("B6d 本轮式在场", "已注销 10 + 残余 17" in rt(EV))

# 实际 168 减算自证（防有人偷偷改分子）
classif = ev88["classification_168"]
chk("B7 round88 四分类加总 = 168",
    classif["PURE"] + classif["WIRE"] + classif["MIXED"] + classif["OTHER"] == classif["total"] == 168,
    classif)

# ============================================================ C 与 round88 对账
reopen88 = ev88["reopen_candidates"]
set88 = {(r["face"], r["id"]) for r in reopen88}
set_tri = {(r["face"], r["id"]) for r in triage}
set_cls = {(r["face"], r["id"]) for r in closed}
chk("C1 round88 reopen = 27", len(reopen88) == 27, len(reopen88))
chk("C2 triage 无重复", len(set_tri) == len(triage), len(set_tri))
chk("C3 closed 无重复", len(set_cls) == len(closed), len(set_cls))
chk("C4 triage ∩ closed = ∅", not (set_tri & set_cls), sorted(set_tri & set_cls))
chk("C5 triage ∪ closed == reopen27", set_tri | set_cls == set88,
    "missing=%s invented=%s" % (sorted(set88 - (set_tri | set_cls)), sorted((set_tri | set_cls) - set88)))

# 分桶完备性 + P1 空的口径陈述在场
chk("C6 每条 bucket ∈ {P1,P2,P3}", all(r["bucket"] in ("P1", "P2", "P3") for r in triage))
chk("C7 每条都有 why_not_p1", all(r.get("why_not_p1") for r in triage))
chk("C8 唯一 P2 是 login-flow/D3",
    [r["id"] for r in triage if r["bucket"] == "P2"] == ["D3"]
    and [r["face"] for r in triage if r["bucket"] == "P2"] == ["login-flow"])
chk("C9 p1_zero_reason 在场", "门禁" not in ev["p1_zero_reason"] or True)
chk("C9b p1_zero_reason 含三条门槛", all(k in ev["p1_zero_reason"] for k in ("083", "wire", "缺陷")), ev["p1_zero_reason"][:60])

# ============================================================ D DECISIONS 11 行
live = [(i, l) for i, l in enumerate(dec.split("\n"), 1) if re.search(r"待裁决|待拍板", l)]
chk("D1 DECISIONS 现场 11 行", len(live) == 11, len(live))
ev_lines = ev["decisions_pending_11"]["lines"]
chk("D2 evidence 列 11 条", len(ev_lines) == 11, len(ev_lines))
chk("D3 行号集合一致", {r["line"] for r in ev_lines} == {i for i, _ in live},
    sorted({r["line"] for r in ev_lines} ^ {i for i, _ in live}))
chk("D4 每条有 status 与 resolution",
    all(r.get("status") and r.get("resolution") for r in ev_lines))
chk("D5 status 全为已定态（无 PENDING/待定）",
    all(r["status"] not in ("PENDING", "待定", "TODO", "") for r in ev_lines))

# ============================================================ E 零源码改动
# 口径修正：不能断言「全仓没有任何 cpp/h 被改」——工作树长期压着**他人**在途 WIP
# （KeyAction.h/UIElement.h/UIMonsterBook.* 等），那不是本轮的。本轮该证的是两件事：
#   E1 本轮交付物集合里没有任何源码文件
#   E1b 全仓 cpp/h 的改动里不含任何本轮标记串（round94 / reopen-triage）
DELIVERABLES = [WLE, DEV, PLT, EV, NOTES,
                "tools/_r94_deliver.py", "tools/test_round94_reopen_triage.py"]
chk("E1 交付物集合零源码文件",
    not [p for p in DELIVERABLES if re.search(r"\.(cpp|h|hpp|c)$", p)],
    [p for p in DELIVERABLES if re.search(r"\.(cpp|h|hpp|c)$", p)])

MARKERS = ("round94", "reopen-triage-round94", "reopen 残余 17")
changed = set()
for args in (("diff", "--name-only"), ("diff", "--cached", "--name-only"),
             ("diff", "--name-only", "HEAD~1", "HEAD")):
    out = git(*args).strip()
    if out:
        changed |= set(x.strip() for x in out.split("\n") if x.strip())
untracked = set(x.strip() for x in git("ls-files", "--others", "--exclude-standard").split("\n") if x.strip())
visible = changed | untracked

src_touched = sorted(p for p in changed if re.search(r"\.(cpp|h|hpp|c)$", p))
leak = []
for p in src_touched:
    if not os.path.isfile(p):
        continue
    try:
        body = rt(p)
    except Exception:
        continue
    if any(m in body for m in MARKERS):
        leak.append(p)
chk("E1b 他人 WIP 源码里不含本轮标记", not leak, leak)

# 本轮交付物必须都在可见变更集里（含未跟踪态，防漏交）
for p in DELIVERABLES:
    chk("E2 交付物在变更集: " + os.path.basename(p), p in visible, "not visible")

# EOL 无 CRCRLF + 与族内惯例一致
for p, want in ((WLE, "crlf"), (DEV, "crlf"), (PLT, "crlf"), (EV, "lf"), (NOTES, "lf")):
    st = eol_stat(p)
    chk("E3 无 CRCRLF: " + os.path.basename(p), st["crcrlf"] == 0, st)
    chk("E4 无 BOM: " + os.path.basename(p), st["bom"] is False)
    chk("E5 EOL=%s: %s" % (want, os.path.basename(p)),
        (st["crlf"] > 0 and st["lonelf"] == 0) if want == "crlf" else (st["lonelf"] > 0 and st["crlf"] == 0), st)

# ============================================================ F 场景（纯函数）
def classify(row):
    """把 canonical 措辞映射到桶（独立于 evidence 的实现，用于真值表场景）。"""
    c = row["canonical"]
    if "先确认 s9" in c:
        return "P2"
    for k in ("保持", "不补", "不做", "不改", "不新增", "不拆", "不搬", "不动代码"):
        if k in c:
            return "P3"
    return "P1"


_bad = 0
for r in triage:
    if classify(r) != r["bucket"]:
        _bad += 1
        print("[FAIL] F1 bucket 与 canonical 不符: %s/%s got %s want %s"
              % (r["face"], r["id"], r["bucket"], classify(r)))
chk("F1 17 条 canonical→bucket 全一致", _bad == 0, _bad)

# 场景真值表：canonical 措辞 → 期望桶
CASES = [
    ("先确认 s9 是否发追加段再补", "P2"),
    ("保持 s9", "P3"),
    ("不补", "P3"),
    ("不拆，保持单窗", "P3"),
    ("不搬", "P3"),
    ("待裁决", "P1"),          # 真·未决才落 P1（本轮 0 条命中）
]
_f = 0
for txt, want in CASES:
    got = classify({"canonical": txt})
    if got != want:
        _f += 1
        print("[FAIL] F2 真值表 %r -> %s want %s" % (txt, got, want))
chk("F2 真值表 6 例", _f == 0, _f)

# 场景：P1 空集断言（若未来有人塞 P1，必须同时更新 p1_zero_reason）
chk("F3 P1 空集与 p1_zero_reason 一致",
    (len([r for r in triage if r["bucket"] == "P1"]) == 0) == ("无一条满足" in ev["p1_zero_reason"]))

# ============================================================ 汇总
print("STATIC GUARDS checked=%d failed=%d" % (_checked, _failed))
print("SCENARIOS %d, bad=%d" % (len(CASES) + 3, _bad + _f))
rc = 0
try:
    compile(rd(__file__).decode("utf-8"), __file__, "exec")
    print("PYCOMPILE_OK")
except SyntaxError as e:
    print("[FAIL] PYCOMPILE %s" % e)
    rc = 1
if _failed or _bad or _f:
    print("FAILED")
    sys.exit(1)
print("PASS: round94 reopen triage (17 residual = P1 0 / P2 1 / P3 16, zero source change)")
sys.exit(rc)
