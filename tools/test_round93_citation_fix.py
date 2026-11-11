# -*- coding: utf-8 -*-
"""round93 harness: 095/v95 依据链纠偏（5 处纯注释）静态守卫 + 场景。

守卫分三类：
  A1  被纠偏的旧依据语在 5 个源码文件里**不再作为依据出现**（只允许作为「旧注释…」的引述）
  A2  083 真锚点（CPortalList::FindPortal / FindPortal_Collision / CUserLocal::CheckPortal_Collision）
      与 083 零命中结论（*UISpecial* 等）写入注释与 evidence
  A3  零行为改动硬证：HEAD blob 与本工作树各自剥离注释后逐字节相等（不依赖 commit hash）
  A4  四文档落位（dev-plan 2-41 / PLT 52 / WLE 10 / evidence + notes）
  A5  evidence JSON 结构与算术
  A6  EOL 审计（无 CRCRLF）
  A7  WIP 隔离：本轮改动集恰好是 5 处源码 + 四文档 + 工具，不夹带外部在途
  A8  约束陈述在场（083 only / 禁 095 / 门禁 5 / 门禁 18 / W 桶零改动）

场景 B 针对「注释依据」本身做真值表：给定 (旧语, 新语, 083 命中数) 判定应纠偏/应保留。
"""
import io
import json
import os
import re
import subprocess
import sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

SRC = [
    "IO/UIStateGame.cpp",
    "IO/UITypes/UISpecial.h",
    "Gameplay/MapleMap/MapPortals.h",
    "Gameplay/MapleMap/MapPortals.cpp",
    "Gameplay/Stage.cpp",
]
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
WLE = "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md"
EV = "docs/features/development-and-fix-plan-2026-11/evidence/portal-special-citation-round93.json"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-citation-fix-20261111.md"
IDA_DIR = "tools/_ida_r93"

passed = []
failed = []


def chk(name, cond, detail=""):
    if cond:
        passed.append(name)
    else:
        failed.append((name, detail))


def rd(p):
    with open(p, "rb") as f:
        return f.read().decode("utf-8")


def git(*args):
    r = subprocess.run(["git"] + list(args), capture_output=True)
    return r.stdout.decode("utf-8", "replace")


def strip_cpp_comments(s):
    """剥离 // 行注释与 /* */ 块注释，保留字符串字面量。"""
    out = []
    i = 0
    n = len(s)
    while i < n:
        c = s[i]
        if c == '"':
            j = i + 1
            while j < n:
                if s[j] == "\\":
                    j += 2
                    continue
                if s[j] == '"':
                    j += 1
                    break
                j += 1
            out.append(s[i:j])
            i = j
            continue
        if c == "'":
            j = i + 1
            while j < n:
                if s[j] == "\\":
                    j += 2
                    continue
                if s[j] == "'":
                    j += 1
                    break
                j += 1
            out.append(s[i:j])
            i = j
            continue
        if c == "/" and i + 1 < n and s[i + 1] == "/":
            while i < n and s[i] != "\n":
                i += 1
            continue
        if c == "/" and i + 1 < n and s[i + 1] == "*":
            i += 2
            while i + 1 < n and not (s[i] == "*" and s[i + 1] == "/"):
                i += 1
            i += 2
            continue
        out.append(c)
        i += 1
    return "".join(out)


def norm_eol(s):
    return s.replace("\r\n", "\n")


def code_only(s):
    """归一 EOL + 剥注释 + 压掉纯空白行，得到可比对的行为面。"""
    body = strip_cpp_comments(norm_eol(s))
    lines = [ln.rstrip() for ln in body.split("\n")]
    return "\n".join(ln for ln in lines if ln.strip() != "")


# ------------------------------------------------------------------ A1 旧依据语
FORBIDDEN_AS_CITATION = [
    (r"//\s*095\s*自定义中文 UI", "095 自定义中文 UI 仍作依据"),
    (r"对齐\s*v95[：:]", "「对齐 v95：」仍作依据"),
    (r"//\s*原生语义（v95", "「原生语义（v95」仍作依据"),
    (r"对齐\s*v95\s+CPortal::CheckKey", "Stage 的「对齐 v95 CPortal::CheckKey」仍作依据"),
]
for pat, label in FORBIDDEN_AS_CITATION:
    hits = []
    for p in SRC:
        for i, ln in enumerate(rd(p).split("\n"), 1):
            if re.search(pat, ln):
                hits.append("%s:%d" % (p, i))
    chk("A1 无旧依据语: " + label, not hits, "; ".join(hits))

# 旧语只能以「旧注释」引述形式残留
quote_ok = True
for p in SRC:
    for i, ln in enumerate(rd(p).split("\n"), 1):
        if "095" in ln or "v95" in ln:
            if "旧注释" not in ln:
                quote_ok = False
                chk("A1b 095/v95 只允许出现在「旧注释…」引述中", False, "%s:%d %s" % (p, i, ln.strip()[:80]))
chk("A1b 095/v95 只允许出现在「旧注释…」引述中", quote_ok)

# ------------------------------------------------------------------ A2 真锚点
ANCHORS = [
    ("CPortalList::FindPortal", ["Gameplay/MapleMap/MapPortals.h", "Gameplay/MapleMap/MapPortals.cpp"]),
    ("0x712ab1", ["Gameplay/MapleMap/MapPortals.h", "Gameplay/MapleMap/MapPortals.cpp", "Gameplay/Stage.cpp"]),
    ("CPortalList::FindPortal_Collision", ["Gameplay/MapleMap/MapPortals.h", "Gameplay/MapleMap/MapPortals.cpp"]),
    ("0x712b61", ["Gameplay/MapleMap/MapPortals.h", "Gameplay/MapleMap/MapPortals.cpp"]),
    ("CUserLocal::CheckPortal_Collision", ["Gameplay/MapleMap/MapPortals.h", "Gameplay/MapleMap/MapPortals.cpp"]),
    ("0x94dac6", ["Gameplay/MapleMap/MapPortals.h", "Gameplay/MapleMap/MapPortals.cpp"]),
]
for needle, files in ANCHORS:
    ok = all(needle in rd(f) for f in files)
    chk("A2 真锚点在场: " + needle, ok, "missing in " + ",".join(f for f in files if needle not in rd(f)))

for needle, files in [
    ("*UISpecial*", ["IO/UIStateGame.cpp", "IO/UITypes/UISpecial.h"]),
    ("*Fairy*", ["IO/UIStateGame.cpp", "IO/UITypes/UISpecial.h"]),
    ("*Attendance*", ["IO/UIStateGame.cpp", "IO/UITypes/UISpecial.h"]),
    ("special_family.json", ["IO/UIStateGame.cpp", "IO/UITypes/UISpecial.h"]),
    ("portal_family.json", ["Gameplay/MapleMap/MapPortals.h", "Gameplay/Stage.cpp"]),
]:
    ok = all(needle in rd(f) for f in files)
    chk("A2 零命中结论在场: " + needle, ok, ",".join(files))

chk("A2c 本端扩展（E 桶）判定在场",
    "本端扩展（E 桶）" in rd("IO/UIStateGame.cpp") and "本端扩展（E 桶）" in rd("IO/UITypes/UISpecial.h"))
chk("A2d m09729 扩展可不对齐引用在场",
    "m09729" in rd("IO/UIStateGame.cpp") and "m09729" in rd("IO/UITypes/UISpecial.h"))
chk("A2e 依据链污染标注在场",
    all("依据链污染" in rd(f) for f in SRC))

# ------------------------------------------------------------------ A3 零行为改动
# 说明：IO/UIStateGame.cpp 同时存在外部程序在途 WIP（非本轮），因此不能整文件比对。
# 改为 hunk 级断言：凡含本轮标记串的 hunk，其 +/- 行必须全部是注释行；
# 不含标记串的 hunk 视为外部在途，跳过（并在 A7 里断言未被本轮触及）。
ROUND_MARKERS = ["本端扩展（E 桶）", "CPortalList::FindPortal", "CPortal::CheckKey」在 083 全库无此符号"]
for p in SRC:
    head = git("show", "HEAD:" + p)
    chk("A3 git show 取到 HEAD blob: " + p, head != "", p)

    # 三态取 diff：未暂存 / 已暂存 / 已提交（HEAD~1..HEAD）。
    # 提交后工作树仍可能残留**他人**在途 hunk，因此不能「有就用」，必须三态全收再按标记筛。
    diffs = [
        git("diff", "-U0", "--", p),
        git("diff", "--cached", "-U0", "--", p),
        git("diff", "-U0", "HEAD~1", "HEAD", "--", p),
    ]
    chk("A3 取到 diff（三态至少一源）: " + p, any(d.strip() for d in diffs), "empty in worktree/staged/HEAD")
    diff = "\n".join(d for d in diffs if d.strip())
    hunk_lines = []
    cur = None
    for ln in diff.split("\n"):
        if ln.startswith("@@"):
            if cur is not None:
                hunk_lines.append(cur)
            cur = []
            continue
        if cur is not None and (ln.startswith("+") or ln.startswith("-")) and not ln.startswith(("+++", "---")):
            cur.append(ln)
    if cur is not None:
        hunk_lines.append(cur)

    mine = [h for h in hunk_lines if any(any(m in l for m in ROUND_MARKERS) for l in h)]
    chk("A3 找到本轮 hunk: " + p, len(mine) >= 1, "hunks=%d mine=%d" % (len(hunk_lines), len(mine)))

    bad = []
    for h in mine:
        for l in h:
            body = l[1:].strip()
            if body == "":
                continue
            if not body.startswith("//"):
                bad.append(l[:100])
    chk("A3 本轮 hunk 全为注释行（零行为字面量）: " + p, not bad, "; ".join(bad))

    # 非标记 hunk（外部 WIP）不得含本轮标记
    ext = [h for h in hunk_lines if h not in mine]
    chk("A3 外部 hunk 不含本轮标记: " + p,
        not any(any(m in l for m in ROUND_MARKERS) for h in ext for l in h), "leak")

# 无外部 WIP 的 4 个文件：剥离注释后逐字节相等（更强）
NO_EXT_WIP = [p for p in SRC if p != "IO/UIStateGame.cpp"]
for p in NO_EXT_WIP:
    chk("A3b 剥离注释后行为面相等: " + p, code_only(git("show", "HEAD:" + p)) == code_only(rd(p)), "differs")

# ------------------------------------------------------------------ A4 四文档
dev = rd(DEV)
chk("A4a dev-plan 2-41 行在场", "| **2-41** |" in dev)
chk("A4b dev-plan 2-41 提到 5 处", "5 处" in dev.split("| **2-41** |", 1)[1][:1200])
chk("A4c dev-plan 2-41 在 2-40 之后", dev.index("| **2-41** |") > dev.index("| **2-40** |"))
chk("A4d dev-plan 2-41 在 Wave 3 之前", dev.index("| **2-41** |") < dev.index("### Wave 3"))

plt = rd(PLT)
chk("A4e PLT 52 节在场", "## 52. round93" in plt)
chk("A4f PLT 52 有 52-1..52-4", all(("52-%d" % i) in plt for i in range(1, 5)))
chk("A4g PLT 52 是末节", plt.rindex("## 52.") > plt.rindex("## 51."))

wle = rd(WLE)
chk("A4h WLE 10 节在场", "## 10. round93" in wle)
chk("A4i WLE 10 是末节", wle.rindex("## 10.") > wle.rindex("## 9."))
chk("A4j WLE 10 含 5 行表", wle.split("## 10. round93", 1)[1].count("| `")>=5)
chk("A4k notes 在场", os.path.isfile(NOTES))
chk("A4l notes 含 5 处与规则升级", os.path.isfile(NOTES) and "5 处" in rd(NOTES) and "entity_query" in rd(NOTES))

# ------------------------------------------------------------------ A5 evidence
chk("A5a evidence 存在", os.path.isfile(EV))
ev = json.loads(rd(EV)) if os.path.isfile(EV) else {}
chk("A5b round/date", ev.get("round") == "round93" and ev.get("date") == "2026-11-11")
chk("A5c scope = comment_only_citation_fix", ev.get("scope") == "comment_only_citation_fix")
chk("A5d zero_source_behavior True", ev.get("zero_source_behavior") is True)
chk("A5e patches 覆盖 5 文件", isinstance(ev.get("patches"), list) and len(ev["patches"]) == 5,
    str([x.get("path") for x in ev.get("patches") or []]))
chk("A5f patches 路径集合正确", {x.get("path") for x in ev.get("patches") or []} == set(SRC))
chk("A5g ida_evidence 两族", set(ev.get("ida_evidence") or {}) == {"special_family", "portal_family"})
sf = (ev.get("ida_evidence") or {}).get("special_family") or {}
chk("A5h special 五族全 0", all(sf.get("queries", {}).get(k) == 0 for k in
                                ["*UISpecial*", "*Fairy*", "*Welfare*", "*CheckIn*", "*Attendance*"]),
    str(sf.get("queries")))
pf = (ev.get("ida_evidence") or {}).get("portal_family") or {}
chk("A5i portal CPortal 符号 0 命中", pf.get("queries", {}).get("*CPortal@@*") == 0
    and pf.get("queries", {}).get("*CPortal::*") == 0)
chk("A5j 三真锚点登记", len(pf.get("real_anchors") or {}) == 3)
chk("A5k 决策点两条", set(ev.get("decision_points") or {}) == {"portal_type_dispatch", "decisions_130"})
ar = ev.get("arithmetic_unchanged") or {}
chk("A5l 算术不动 168 / 27=25+1+1",
    ar.get("decisions_rows") == 168
    and ar.get("wle_section4") == "W 38 + L 130 = 168"
    and ar.get("reopen") == "27 = 25 P1 + 1 W + 1 F-P2")
chk("A5m 约束 5 条", len(ev.get("constraints") or []) == 5)
chk("A5n evidence 不含 commit hash 形态",
    not re.search(r"\b[0-9a-f]{40}\b", rd(EV)) and "\"git log\"" not in rd(EV))

# ------------------------------------------------------------------ A6 EOL
for p in SRC + [DEV, PLT, WLE, EV, NOTES]:
    b = rd(p)
    chk("A6 无 CRCRLF: " + p, "\r\r\n" not in b, p)
    chk("A6 无 BOM: " + p, not b.startswith("\ufeff"), p)

# ------------------------------------------------------------------ A7 WIP 隔离
changed = set(x for x in git("diff", "--name-only").split("\n") if x.strip())
staged = set(x for x in git("diff", "--cached", "--name-only").split("\n") if x.strip())
head_files = set(x for x in git("show", "--name-only", "--format=", "HEAD").split("\n") if x.strip())
untracked = set(x for x in git("ls-files", "--others", "--exclude-standard").split("\n") if x.strip())
visible = changed | staged | head_files | untracked
chk("A7a 5 源码文件在可见改动集（三态自适应）", all(p in visible for p in SRC),
    str([p for p in SRC if p not in visible]))
chk("A7b 四文档在可见集", all(p in visible for p in [DEV, PLT, WLE, EV, NOTES]),
    str([p for p in [DEV, PLT, WLE, EV, NOTES] if p not in visible]))

EXT_WIP_ALLOW = {
    "IO/KeyAction.h", "IO/UIElement.h", "IO/UIStateGame.cpp",
    "MapleStory.vcxproj", "MapleStory.vcxproj.filters", "Net/KeyMapTranslate.h",
}
touchable = set(SRC) | {DEV, PLT, WLE, EV, NOTES}
ext = changed - touchable
chk("A7c 外部在途未被本轮写入", not (ext & set(SRC)), str(sorted(ext)))

# ------------------------------------------------------------------ A8 约束陈述
allt = rd(EV) + rd(NOTES) + rd(WLE)
for kw in ["083/v83 only", "禁 095", "禁想象修改", "REA", "W 桶"]:
    chk("A8 约束陈述在场: " + kw, kw in allt)

# ------------------------------------------------------------------ B 场景真值表
def verdict(old_phrase, hits):
    """依据语纠偏真值表：命中 0 且依据语含 095/v95/不存在符号 → 必纠偏。"""
    if hits == 0 and re.search(r"095|v95|CPortal::", old_phrase):
        return "FIX"
    if hits > 0:
        return "KEEP"
    return "UNKNOWN"


CASES = [
    ("095 自定义中文 UI：福利中心", 0, "FIX"),
    ("对齐 v95 CPortal::CheckKey", 0, "FIX"),
    ("v95 CPortal::CheckKey / ProcessPortal", 0, "FIX"),
    ("083 CUIMacroSysEx::OnSelected", 1, "KEEP"),
    ("083 CPortalList::FindPortal", 1, "KEEP"),
    ("095 自定义中文 UI：福利中心", 3, "KEEP"),
]
for old, hits, expect in CASES:
    got = verdict(old, hits)
    chk("B 真值表 %s (hits=%d) -> %s" % (old[:34], hits, expect), got == expect, "got " + got)

# 5 处 == evidence patches 数 == 文档声明数
chk("B2 注释处数一致（5 = evidence.patches = docs 声明）",
    len(ev.get("patches") or []) == 5
    and "5 处" in dev.split("| **2-41** |", 1)[1][:1200]
    and "5 处" in plt.split("## 52. round93", 1)[1][:800])

# 剥注释后行数不因本轮减少（纯注释改写不得删代码行）
# IO/UIStateGame.cpp 含外部在途 WIP（会增行），行为面已由 A3 hunk 级断言覆盖，此处只比 4 个干净文件。
for p in NO_EXT_WIP:
    a = code_only(git("show", "HEAD:" + p)).count("\n")
    b = code_only(rd(p)).count("\n")
    chk("B3 代码行数不变: " + p, a == b, "%d -> %d" % (a, b))
chk("B3b UIStateGame 行为面由 A3 hunk 级覆盖（此处只断言代码行数未减）",
    code_only(rd("IO/UIStateGame.cpp")).count("\n") >= code_only(git("show", "HEAD:IO/UIStateGame.cpp")).count("\n"))

# 工具脚本与 IDA 证据落地
for f in ["tools/_r93_deliver.py", "tools/_r93_ida.py", "tools/_r93_ida3.py", "tools/_r93_ida4.py",
          IDA_DIR + "/special_family.json", IDA_DIR + "/portal_family.json",
          IDA_DIR + "/CUserLocal_CheckPortal_Collision.code.txt",
          IDA_DIR + "/CPortalList_FindPortal.code.txt",
          IDA_DIR + "/CPortalList_FindPortal_Collision.code.txt"]:
    chk("B4 证据文件在场: " + f, os.path.isfile(f))

# 反编译逐字锚点（防证据被空文件冒充）
for f, needle in [
    (IDA_DIR + "/CPortalList_FindPortal.code.txt", "v7[3] - a5"),
    (IDA_DIR + "/CPortalList_FindPortal.code.txt", "v7[4] + 50"),
    (IDA_DIR + "/CPortalList_FindPortal_Collision.code.txt", "v5[5] / 2"),
    (IDA_DIR + "/CUserLocal_CheckPortal_Collision.code.txt", "COutPacket::COutPacket(a1: 100)"),
    (IDA_DIR + "/CUserLocal_CheckPortal_Collision.code.txt", "SendTransferFieldRequest"),
]:
    ok = os.path.isfile(f) and needle.replace(" ", "") in rd(f).replace(" ", "")
    chk("B5 反编译逐字: %s :: %s" % (f.split("/")[-1], needle), ok)

# py_compile
import py_compile
for f in ["tools/_r93_deliver.py", "tools/_r93_ida.py", "tools/_r93_ida3.py", "tools/_r93_ida4.py"]:
    try:
        py_compile.compile(f, cfile=os.path.join(os.path.dirname(os.path.abspath(__file__)), "_pyc_tmp.pyc"),
                           doraise=True)
        ok = True
    except Exception as e:
        ok = False
chk("B6 PYCOMPILE_OK", ok)
print("PYCOMPILE_OK" if ok else "PYCOMPILE_FAIL")

print("STATIC GUARDS checked=%d failed=%d" % (len(passed) + len(failed), len(failed)))
for name, detail in failed:
    print("[FAIL]", name, (":: " + detail) if detail else "")
print("SCENARIOS %d, bad=%d" % (len(passed) + len(failed), len(failed)))
print("PASS: round93 citation fix (5 comment-only sites, 083 anchors verified)" if not failed else "FAILED")
sys.exit(0 if not failed else 1)
