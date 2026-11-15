#!/usr/bin/env python3
"""新翻页落点前后：底栏 CH.17 遮挡收敛 + 标签覆盖 + 翻页扩展 ROI 复算。

before = shots/ch20_before_pager/{home_f05,page2_f05}.png（旧落点 (448,485)）
after  = shots/{ch20_home_f05,ch20_page2_f05}.png（新落点 (646,497)）
target = targets_desktop/desktop_channel_expanded.png（原生 20 格，无翻页箭头）
判据：CH.17 gauge_band_mean_abs 从 16.1 回落到与 CH.18/19/20 同量级（≈10.x），
      CH.17 与同排其它格的差值 ≤2；pager_region 不再压在 CH.17 格子内；
      次页标签覆盖仍生效（home vs page2 差异像素 >200）。
"""
import json
import os

import numpy as np
from PIL import Image

TV = r"C:\Users\11036\AppData\Local\Temp\term_verify"
SHOTS = os.path.join(TV, "shots")
OUT = os.path.join(TV, "ui_r3")

# 本端网格相位（R3 既有口径）：cell(i)=(458+94*(i%4), 336+32*(i//4))
CELL_X0, CELL_Y0, CELL_W, CELL_H, COLS = 458, 336, 94, 32, 4


def cell_box(i):
    x = CELL_X0 + CELL_W * (i % COLS)
    y = CELL_Y0 + CELL_H * (i // COLS)
    return x, y, x + 92, y + 30


def arr(p):
    return np.asarray(Image.open(p).convert("RGB"), np.int16)


def mean_abs(a, b, box):
    x0, y0, x1, y1 = box
    return round(float(np.abs(a[y0:y1, x0:x1] - b[y0:y1, x0:x1]).mean()), 2)


def main():
    T = arr(os.path.join(TV, "targets_desktop", "desktop_channel_expanded.png"))
    before = arr(os.path.join(SHOTS, "ch20_before_pager", "home_f05.png"))
    after_home = arr(os.path.join(SHOTS, "ch20_home_f05.png"))
    after_p2 = arr(os.path.join(SHOTS, "ch20_page2_f05.png"))

    cells = {}
    for idx, name in [(16, "CH.17"), (17, "CH.18"), (18, "CH.19"), (19, "CH.20")]:
        x0, y0, x1, y1 = cell_box(idx)
        full = (x0, y0, x1, y1)
        gauge = (x0 + 7, y0 + 18, x0 + 80, y0 + 27)  # chgauge 73x9 相对格子 (+7,+18)
        cells[name] = {
            "box": list(full),
            "before_full_mean_abs": mean_abs(T, before, full),
            "before_gauge_mean_abs": mean_abs(T, before, gauge),
            "after_full_mean_abs": mean_abs(T, after_home, full),
            "after_gauge_mean_abs": mean_abs(T, after_home, gauge),
        }
    b17 = cells["CH.17"]["before_gauge_mean_abs"]
    bpeers = np.mean([cells[k]["before_gauge_mean_abs"] for k in ("CH.18", "CH.19", "CH.20")])
    a17 = cells["CH.17"]["after_gauge_mean_abs"]
    apeers = np.mean([cells[k]["after_gauge_mean_abs"] for k in ("CH.18", "CH.19", "CH.20")])

    old_pager = (448, 485, 516, 515)     # 旧 PREV/NEXT 两块 28x30
    new_pager = (646, 497, 714, 527)     # 新 PREV/NEXT 两块 28x30
    pager = {
        "old_region_vs_target": mean_abs(T, before, old_pager),
        "new_region_vs_target": mean_abs(T, after_home, new_pager),
        "old_inside_CH17": bool(448 < cell_box(16)[2] and 485 < cell_box(16)[3]
                                and 516 > cell_box(16)[0] and 515 > cell_box(16)[1]),
        "new_inside_CH17": bool(646 < cell_box(16)[2] and 497 < cell_box(16)[3]
                                and 714 > cell_box(16)[0] and 527 > cell_box(16)[1]),
    }

    label_roi = tuple(cell_box(0)[:2]) + (cell_box(0)[0] + 40, cell_box(0)[1] + 15)
    diff = np.abs(after_home[label_roi[1]:label_roi[3], label_roi[0]:label_roi[2]]
                  - after_p2[label_roi[1]:label_roi[3], label_roi[0]:label_roi[2]]).max(axis=2)
    label_override = {
        "roi": list(label_roi),
        "home_vs_page2_mean_abs": mean_abs(after_home, after_p2, label_roi),
        "changed_pixels_gt35": int((diff > 35).sum()),
    }

    res = {
        "target": "desktop_channel_expanded.png",
        "before": "ch20_before_pager/home_f05.png (pager 448,485)",
        "after": "ch20_home_f05.png (pager 646,497)",
        "cells": cells,
        "gauge_peer_gap_before": round(float(b17 - bpeers), 2),
        "gauge_peer_gap_after": round(float(a17 - apeers), 2),
        "ch17_occlusion_resolved": bool(abs(a17 - apeers) <= 2.0 and (b17 - bpeers) > 3.0),
        "pager": pager,
        "label_override": label_override,
        "label_override_active": bool(label_override["changed_pixels_gt35"] > 200),
    }
    jp = os.path.join(OUT, "channel20_final_audit.json")
    json.dump(res, open(jp, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
    print(json.dumps({k: v for k, v in res.items() if k != "cells"}, ensure_ascii=False, indent=2))
    print(" cells:", json.dumps({k: {"before_g": v["before_gauge_mean_abs"],
                                     "after_g": v["after_gauge_mean_abs"]}
                                 for k, v in cells.items()}, ensure_ascii=False))
    print("JSON", jp)


if __name__ == "__main__":
    main()
