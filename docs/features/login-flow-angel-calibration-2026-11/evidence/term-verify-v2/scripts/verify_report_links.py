# -*- coding: utf-8 -*-
"""报告链接零缺失 + §10 关键数字对账（独立实跑，防 commit 写错数）。"""
import os, re, json, sys

ROOT = r"E:\project\MapleStory-Client"
DOC = os.path.join(ROOT, "docs", "features",
                   "login-flow-angel-calibration-2026-11", "TERM_VERIFY_REPORT.md")
A = os.path.join(ROOT, "docs", "features",
                 "login-flow-angel-calibration-2026-11", "evidence", "term-verify-v2")

raw = open(DOC, "r", encoding="utf-8").read()
links = re.findall(r"\]\((evidence/term-verify-v2/[^)]+)\)", raw)
missing = [p for p in links if not os.path.isfile(os.path.join(
    os.path.dirname(DOC), p.replace("/", os.sep)))]
print("links_total", len(links), "missing", len(missing))
for p in missing:
    print("  MISSING", p)

# §10 关键数字对账
v = json.load(open(os.path.join(A, "verdicts", "sidebar_fix.json"), encoding="utf-8"))
pages = {p["page"]: p for p in v["pages"]}
checks = [
    ("verdict_all_pass", v["verdict"]["all_pass"] is True),
    ("char_strip_0.9313", pages["char_select"]["strip_ncc"] == 0.9313),
    ("race_strip_0.9508", pages["race_select"]["strip_ncc"] == 0.9508),
    ("char_4blocks", pages["char_select"]["blocks_present_gt0_5"] == 4),
    ("race_4blocks", pages["race_select"]["blocks_present_gt0_5"] == 4),
    ("char_real", pages["char_select"]["real_gl_frame"] is True),
    ("race_real", pages["race_select"]["real_gl_frame"] is True),
    ("section10_present", "## 10. 左侧木牌栏补齐" in raw),
    ("section10_sidebar_fix_verified", "SIDEBAR_FIX_VERIFIED" in raw),
    ("old_verdict_annotated", "已于 §10 补齐并真机逐块验收" in raw),
    ("exe_b10df", "b10df425d42b4c127" in raw),
    ("no_external_block_for_target", "桌面目标帧已到位" in raw or "桌面目标帧到位" in raw),
]
print("--- numeric/structure checks ---")
for k, ok in checks:
    print(("PASS" if ok else "FAIL"), k)
allok = (not missing) and all(ok for _, ok in checks)
print("ALL_PASS", allok)
sys.exit(0 if allok else 1)
