#!/usr/bin/env python3
"""把逐档实机 JSON 汇总成 evidence/v3-tier-matrix.csv（可复跑校验）。

用法：python tools/resolution/summarize_matrix.py
只读 evidence/live/tier<id>-<WxH>.json，输出同名目录下的汇总 CSV。
"""
import csv
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2] / "docs" / "features" / "multi-resolution" / "evidence"
LIVE = ROOT / "live"
OUT = ROOT / "v3-tier-matrix.csv"

rows = []
for path in sorted(LIVE.glob("tier*-*.json")):
    data = json.loads(path.read_text(encoding="utf-8"))
    if "verdict" not in data:
        continue
    verdict = data["verdict"]
    rows.append([
        data["tier"],
        "%dx%d" % tuple(data["expected"]),
        int(verdict["login_1280x720"]),
        int(verdict["reached_expected"]),
        int(verdict["crashed"]),
        "PASS" if verdict["pass"] else "FAIL",
        path.name,
    ])

rows.sort(key=lambda r: r[0])
with OUT.open("w", newline="", encoding="utf-8") as handle:
    writer = csv.writer(handle)
    writer.writerow(["tier", "expected", "login_1280x720", "reached_expected",
                     "crashed", "verdict", "evidence"])
    writer.writerows(rows)

print(json.dumps({"written": str(OUT), "tiers": len(rows),
                  "passed": sum(1 for r in rows if r[5] == "PASS")}, ensure_ascii=False))
