#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""R5 属性板验收：同一判据复算（measure_r5b_rows_v3 口径），before=ch20 时代旧帧不可用，
直接用 before_src=既有审计基线数值（写死在脚本里，来自 measure_r5b_rows_v3.json）。"""
import os, sys, json, numpy as np
from PIL import Image
R5 = r"C:\Users\11036\AppData\Local\Temp\term_verify\ui_r5"
SHOTS = r"C:\Users\11036\AppData\Local\Temp\term_verify\shots"
TGT = r"C:\Users\11036\AppData\Local\Temp\term_verify\targets_desktop\desktop_char_select.png"
# 修复前实测基线（measure_r5b_rows_v3.json / measure_r5_charinfo.json 记录）
BEFORE = {"job_darkpx": 51, "luk_darkpx": 107, "stroke_ratio": 0.174, "rank_dy": -6,
          "level_dy_top": 1, "str_dy_top": 1}
# 修复后逐行扫描（与 measure_r5b_rows_v3 相同 ROI/阈值：值行黑字 <90）
def analyze(path, tag):
    im = np.asarray(Image.open(path).convert("RGB")).astype(int)
    g = im.mean(axis=2)
    rows = {}
    # 属性板 detail_pos = charpos + CS_CHARINFO_OFF；目标锚标签行 NCC 0.9994@(398,222)
    # 值行带（来自 measure_r5b_rows_v3 定标，静态坐标，两端一致）：
    bands = {"job": (225, 247), "level": (243, 262), "str": (280, 300),
             "dex": (298, 318), "int": (316, 336), "luk": (334, 354), "rank": (411, 431)}
    for name, (y0, y1) in bands.items():
        band = g[y0:y1, 430:580]
        dark = int((band < 90).sum())
        rows[name] = {"darkpx": dark,
                      "ys": [int(y) for y in range(y0, y1) if (g[y, 430:580] < 90).sum() > 2]}
    return {"tag": tag, "rows": rows,
            "total_dark": sum(v["darkpx"] for v in rows.values())}
def main():
    tgt = analyze(TGT, "TARGET")
    ours = analyze(os.path.join(SHOTS, "r5_charselect_f05.png"), "OURS_R5")
    t, o = tgt["rows"], ours["rows"]
    checks = {
        "job_no_outline": o["job"]["darkpx"] <= t["job"]["darkpx"] + 8,
        "luk_no_extra_ink": o["luk"]["darkpx"] <= max(t["luk"]["darkpx"], 1) + 12,
        "total_dark_close": abs(ours["total_dark"] - tgt["total_dark"]) <= 60,
    }
    out = {"target": tgt, "ours": ours, "before_baseline": BEFORE, "checks": checks,
           "verdict": "R5_ATTRBOARD_VERIFIED" if all(checks.values()) else "R5_ATTRBOARD_NOT_VERIFIED",
           "failed": [k for k, v in checks.items() if not v]}
    ev = os.path.join(R5, "r5_after_audit.json")
    json.dump(out, open(ev, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
    print(json.dumps({"verdict": out["verdict"], "failed": out["failed"],
                      "tgt_total": tgt["total_dark"], "ours_total": ours["total_dark"],
                      "job": [t["job"]["darkpx"], o["job"]["darkpx"]],
                      "luk": [t["luk"]["darkpx"], o["luk"]["darkpx"]],
                      "rank_ys_t": t["rank"]["ys"], "rank_ys_o": o["rank"]["ys"]},
                     ensure_ascii=False))
if __name__ == "__main__":
    main()
