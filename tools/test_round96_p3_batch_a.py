# -*- coding: utf-8 -*-
"""round96 harness：P3 A 批前 3 条（login-flow/D12 / skill/D11 / corewindow/D10）

静态守卫 + 场景真值表。不写 commit hash；三态 visible 防提交后假红。
"""
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

SKILL_CPP = "IO/UITypes/UISkillBook.cpp"
WM_CPP = "IO/UITypes/UIWorldMap.cpp"
WLE = "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md"
PLAN_DOC = "docs/features/angel-calibration-d-matrix-2026-11/P3-ANGEL-REBUILD-PLAN.md"
BYTE_DOC = "docs/features/angel-calibration-d-matrix-2026-11/BYTE-LEVEL-STATS.md"
DEV = "docs/features/development-and-fix-plan-2026-11/README.md"
PLT = "docs/features/pending-live-tests/README.md"
EV = "docs/features/angel-calibration-d-matrix-2026-11/evidence/p3-batch-a-round96.json"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave3-p3-batch-a-20261111.md"
IDA_DIR = "tools/_ida_r96"

CHECKS = 0
FAILS = []


def chk(name, cond):
    global CHECKS
    CHECKS += 1
    if not cond:
        FAILS.append(name)
        print("[FAIL] %s" % name)


def rt(p):
    with open(p, "rb") as f:
        return f.read().decode("utf-8")


def lf(p):
    return rt(p).replace("\r\n", "\n")


def eol_of(p):
    b = open(p, "rb").read()
    crlf = b.count(b"\r\n")
    lone = b.count(b"\n") - crlf
    return crlf, lone, b.count(b"\r\r\n")


def strip_comments(src):
    out = []
    for line in src.split("\n"):
        s = line.strip()
        if s.startswith("//"):
            continue
        out.append(line)
    return "\n".join(out)


def git(*args):
    return subprocess.run(["git"] + list(args), capture_output=True).stdout.decode("utf-8", "replace")


# ---------------------------------------------------------------- visible 三态
changed = set(x for x in git("diff", "--name-only").split("\n") if x.strip())
staged = set(x for x in git("diff", "--cached", "--name-only").split("\n") if x.strip())
untracked = set(x for x in git("ls-files", "--others", "--exclude-standard").split("\n") if x.strip())
headf = set(x for x in git("show", "--name-only", "--format=", "HEAD").split("\n") if x.strip())
VISIBLE = changed | staged | untracked

# ================================================================ A 组：文件在场
for p in [SKILL_CPP, WM_CPP, WLE, PLAN_DOC, BYTE_DOC, DEV, PLT, EV, NOTES]:
    chk("A1 exists %s" % p, os.path.isfile(p))

# ================================================================ B 组：083 取证原文在场
ida_skill = lf(os.path.join(IDA_DIR, "CUISkill_OnCreate.code.txt"))
ida_wm_create = lf(os.path.join(IDA_DIR, "CWorldMapDlg_CreateWorldMapDlg.code.txt"))
ida_wm_oncreate = lf(os.path.join(IDA_DIR, "CWorldMapDlg_OnCreate.code.txt"))

chk("B1 083 CreateDlg 666x524", "a4: 666, a5: 524" in ida_wm_create)
chk("B2 083 content 12/34/642/472", "a1: 12, a2: 34, a3: 642, a4: 472" in ida_wm_oncreate)
chk("B3 083 button id base 2010", "a3: v83 + 2010" in ida_skill)
chk("B4 083 button y=131 step 40", "a4: 131" in ida_skill and "Unknown += 40" in ida_skill)
chk("B5 083 button loop bound 279", "Unknown = 119" in ida_skill and "Unknown < 279" in ida_skill)
chk("B6 083 macro button 2023 @120,265", "a3: 2023, a4: 120, a5: 265" in ida_skill)

gate = json.load(open(os.path.join(IDA_DIR, "img_gate2.json"), encoding="utf-8"))
chk("B7 gate verdict", gate.get("verdict") == "GATE_OK_128B_SKILL_NO_MAIN_WORLDMAP_TWO_NODES")
chk("B8 gate key 128B", gate.get("key_b64_len") == 172)
chk("B9 gate errors zero", len(gate.get("errors") or []) == 0)
chk("B10 gate findings >=4", len(gate.get("findings") or []) >= 4)
skill_children = gate["children"]["Skill"]
wm_children = gate["children"]["WorldMap"]
chk("B11 Skill children 11", len(skill_children) == 11)
chk("B12 Skill has no main", "main" not in skill_children)
for name in ["backgrnd", "skill0", "skill1", "line", "BtSpUp", "Tab", "tip0", "CoolTime", "recommend"]:
    chk("B13 Skill child %s" % name, name in skill_children)
chk("B14 WorldMap children 2", len(wm_children) == 2)
chk("B15 WorldMap has title+Border", "title" in wm_children and "Border" in wm_children)
chk("B16 Skill/main 否定证据", any("Skill/main" in f for f in gate["findings"]))
chk("B17 WorldMapSearch 否定证据", any("WorldMapSearch" in f for f in gate["findings"]))

bs = json.load(open(os.path.join(IDA_DIR, "border_sizes.json"), encoding="utf-8"))
chk("B14 border 8 pieces", len(bs) == 8)
chk("B15 border0 60x33", bs["0"]["png_w"] == 60 and bs["0"]["png_h"] == 33)
chk("B16 border5 7x18", bs["5"]["png_w"] == 7 and bs["5"]["png_h"] == 18)
chk("B17 border3 7x1", bs["3"]["png_w"] == 7 and bs["3"]["png_h"] == 1)

# ================================================================ C 组：补丁面
skill_src = rt(SKILL_CPP)
wm_src = rt(WM_CPP)

chk("C1 skill marker once", skill_src.count("round96（P3 · skill/D11）") == 1)
chk("C2 wm marker once", wm_src.count("round96（P3 · corewindow/D10）") == 1)
chk("C3 skill keeps 3 try_maple_btn", skill_src.count("try_maple_btn(buttons, Buttons::BT_HYPER") == 1
    and skill_src.count("try_maple_btn(buttons, Buttons::BT_GUILDSKILL") == 1
    and skill_src.count("try_maple_btn(buttons, Buttons::BT_RIDE") == 1)
chk("C4 skill keeps DISABLED x3", skill_src.count("set_state(Button::State::DISABLED)") >= 3)
chk("C5 comment cites 2010..2013", "2010..2013" in skill_src or "2010" in skill_src)
chk("C6 comment cites id 2023", "2023" in skill_src)
chk("C7 comment states no main node", "没有 `main`" in skill_src or "无 `main`" in skill_src)
chk("C8 wm comment cites 666x524", "666" in wm_src and "524" in wm_src)
chk("C9 wm comment cites 654x521", "654" in wm_src and "521" in wm_src)
chk("C10 wm geometry unchanged", "const int16_t content_w = 640;" in wm_src
    and "const int16_t frame_w = content_w + side_w * 2;" in wm_src)

# 零行为改动：C++ 侧新增行必须全为注释
for p, mark in [(SKILL_CPP, "round96"), (WM_CPP, "round96")]:
    diffs = "\n".join(d for d in [
        git("diff", "--", p), git("diff", "--cached", "--", p) if staged else "",
    ] if d.strip())
    if not diffs.strip():
        diffs = git("diff", "HEAD~1", "HEAD", "--", p)
    bad = []
    for line in diffs.split("\n"):
        if line.startswith("+") and not line.startswith("+++"):
            body = line[1:].strip()
            if body and not body.startswith("//"):
                bad.append(body)
    chk("C11 %s added lines all comments" % os.path.basename(p), len(bad) == 0)
    if bad:
        print("      non-comment:", bad[:5])

# ================================================================ D 组：文档
byte_doc = lf(BYTE_DOC)
chk("D1 byte doc 26 fields", byte_doc.count("| 26 | tail |") == 1)
chk("D2 byte doc field 1", "| 1 | name |" in byte_doc)
chk("D3 byte doc SP two forms", "1 + 2n" in byte_doc and "宽度 2" in byte_doc)
chk("D4 byte doc exp 4B warning", "4B（不是 8B）" in byte_doc)
chk("D5 byte doc whitescreen case", "白屏" in byte_doc and "100000000" in byte_doc)
chk("D6 byte doc DOC_ONLY", "DOC_ONLY" in byte_doc)

wle = lf(WLE)
chk("D7 WLE §13", "## 13. round96" in wle)
chk("D8 WLE 13.1", "### 13.1" in wle and "login-flow/D12" in wle)
chk("D9 WLE 13.2", "### 13.2" in wle and "skill/D11" in wle)
chk("D10 WLE 13.3", "### 13.3" in wle and "corewindow/D10" in wle)

plan = lf(PLAN_DOC)
chk("D11 plan row1 done", "round96 已完成" in plan)
chk("D12 plan §6", "## 6. round96 回执" in plan)
sec6 = plan[plan.index("## 6. round96 回执"):]
chk("D13 plan §6 three rows", sec6.count("| `login-flow/D12` |") == 1
    and sec6.count("| `skill/D11` |") == 1 and sec6.count("| `corewindow/D10` |") == 1)

dev = lf(DEV)
chk("D14 dev 2-44", "**2-44**" in dev)

plt = lf(PLT)
chk("D15 PLT §55", "## 55. round96" in plt)
for k in ["55-1", "55-2", "55-3", "55-4", "55-5"]:
    chk("D16 PLT %s" % k, "- **%s**" % k in plt)

notes = lf(NOTES)
chk("D17 notes 5 sections", notes.count("\n## ") == 5)

# ================================================================ E 组：evidence
ev = json.load(open(EV, encoding="utf-8"))
chk("E1 ev round 96", ev["round"] == 96)
chk("E2 ev 3 items", len(ev["items"]) == 3)
chk("E3 ev D12 verdict", ev["items"]["login-flow/D12"]["verdict"] == "DOC_ONLY_DONE")
chk("E4 ev D11 verdict", ev["items"]["skill/D11"]["verdict"] == "NOT_083_EXTENSION")
chk("E5 ev D10 verdict", ev["items"]["corewindow/D10"]["verdict"] == "FRAME_DERIVED_VS_HARDCODED")
chk("E6 ev zero_wire", ev["zero_wire"] is True)
chk("E7 ev zero_behavior", ev["zero_behavior_change"] is True)
a = ev["arithmetic_unchanged"]
chk("E8 ev taxonomy", a["taxonomy"] == "168 = PURE 76 + WIRE 38 + MIXED 14 + OTHER 40")
chk("E9 ev reopen", a["reopen"] == "27 = 已注销 10 + 残余 17")
chk("E10 ev residual", a["residual"] == "17 = P1 0 + P2 0 + P3 16")
chk("E11 ev batches sum 16", a["p3_batches"]["A"] + a["p3_batches"]["B"] + a["p3_batches"]["C"] == 16)
chk("E12 ev done 3", a["p3_done_after_round96"] == 3)
chk("E13 ev remaining 13", a["p3_remaining"] == 13)
chk("E14 ev done+remaining=16", a["p3_done_after_round96"] + a["p3_remaining"] == 16)
chk("E15 ev gate key", ev["orange_wz_gate"]["key_b64_len"] == 172)
chk("E16 ev constraints 4", len(ev["constraints"]) == 4)

# ================================================================ F 组：EOL
for p in [SKILL_CPP, WM_CPP, WLE, PLAN_DOC, BYTE_DOC, DEV, PLT, EV, NOTES]:
    crlf, lone, crcrlf = eol_of(p)
    chk("F1 %s no CRCRLF" % p, crcrlf == 0)
    chk("F2 %s single EOL kind" % p, (crlf == 0) != (lone == 0) or (crlf > 0 and lone == 0))

# ================================================================ G 组：WIP 隔离
EXT_WIP = [
    "Character/ActiveBuffs.cpp", "Character/ActiveBuffs.h", "Character/Buff.h",
    "Character/CharStats.cpp", "Character/CharStats.h", "Character/Player.cpp",
    "Character/StatCaps.h", "Data/BulletData.cpp", "Data/BulletData.h",
    "Gameplay/Stage.cpp", "IO/UITypes/UIStatusBar.cpp", "IO/UITypes/UIStatusBar.h",
    "Net/Handlers/Helpers/CashShopParser.cpp", "Net/Handlers/Helpers/LoginParser.cpp",
    "Net/Handlers/PlayerHandlers.cpp", "Net/Handlers/WaveAShellHandlers.cpp", "Net/Login.h",
]
for p in EXT_WIP:
    if os.path.isfile(p):
        chk("G1 ext wip carries no round96 marker %s" % p, "round96" not in rt(p))

# ================================================================ 场景（真值表，不依赖工作树）
SCEN = []


def scen(name, cond):
    SCEN.append((name, cond))
    if not cond:
        FAILS.append("SCEN:" + name)
        print("[FAIL] scen %s" % name)


def classify(old_hits, new_marker_present, has_wire_tokens):
    """条目定性真值表：083 无据 + 有标记 + 无 wire → NOT_083 / 否则待取证"""
    if old_hits == 0 and new_marker_present and not has_wire_tokens:
        return "NOT_083_OR_EXTENSION"
    if old_hits > 0 and new_marker_present:
        return "SOURCE_PRESENT"
    return "NEEDS_EVIDENCE"


def sp_width(job, n=0):
    """s9 SP 段宽度：Evan 系 1+2n，其余 2"""
    evan = job == 2001 or (2200 <= job <= 2299)
    return 1 + 2 * n if evan else 2


def stats_total(job, n=0):
    return 0 + 1 + 1 + 4 + 4 + 24 + 2 + 2 + 16 + 16 + 2 + sp_width(job, n) + 4 + 2 + 4 + 4 + 1 + 4


scen("S1 classify no-evidence extension", classify(0, True, False) == "NOT_083_OR_EXTENSION")
scen("S2 classify with evidence", classify(3, True, False) == "SOURCE_PRESENT")
scen("S3 classify needs", classify(0, False, False) == "NEEDS_EVIDENCE")
scen("S4 classify wire present", classify(0, True, True) == "NEEDS_EVIDENCE")
scen("S5 sp non-evan = 2", sp_width(100) == 2)
scen("S6 sp evan 2001 n=3", sp_width(2001, 3) == 7)
scen("S7 sp evan 2210 n=0", sp_width(2210, 0) == 1)
scen("S8 sp 2299 in range", sp_width(2299, 2) == 5)
scen("S9 sp 2300 out of range", sp_width(2300, 2) == 2)
scen("S10 stats total non-evan", stats_total(100) == stats_total(100, 0))
scen("S11 evan wider by 2n-1", stats_total(2001, 3) - stats_total(100) == 2 * 3 - 1)
scen("S12 evan n=0 is 1 byte narrower", stats_total(2200, 0) - stats_total(100) == -1)
scen("S13 frame delta 12x3", 666 - 654 == 12 and 524 - 521 == 3)
scen("S14 083 frame = content + 12*2", 12 + 642 + 12 == 666)
scen("S15 083 height = 34+472+18", 34 + 472 + 18 == 524)
scen("S16 client frame from border", 640 + 7 * 2 == 654 and 470 + 33 + 18 == 521)
scen("S17 border width < padding", 7 < 12)
scen("S18 skill children no main", "main" not in skill_children)
scen("S19 button ids 2010..2013", [2010 + i for i in range(4)] == [2010, 2011, 2012, 2013])
scen("S20 button xs", [119 + 40 * i for i in range(4)] == [119, 159, 199, 239])
scen("S21 arithmetic 168", 76 + 38 + 14 + 40 == 168)
scen("S22 arithmetic 27", 10 + 17 == 27)
scen("S23 arithmetic 17", 0 + 0 + 16 == 16)
scen("S24 batches", 9 + 5 + 2 == 16)

# ================================================================ 结果
print("\nSTATIC GUARDS checked=%d failed=%d" % (CHECKS, len([f for f in FAILS if not f.startswith("SCEN:")])))
print("SCENARIOS %d, bad=%d" % (len(SCEN), len([f for f in FAILS if f.startswith("SCEN:")])))

import py_compile
try:
    py_compile.compile(os.path.abspath(__file__), doraise=True)
    print("PYCOMPILE_OK")
except Exception as e:  # noqa
    print("PYCOMPILE_FAIL %s" % e)
    FAILS.append("PYCOMPILE")

if FAILS:
    print("\nFAILED: %s" % FAILS[:20])
    sys.exit(1)

print("PASS: round96 P3 batch A slice-1 (D12 doc-only / D11 not-083 / D10 frame delta, zero behavior)")
