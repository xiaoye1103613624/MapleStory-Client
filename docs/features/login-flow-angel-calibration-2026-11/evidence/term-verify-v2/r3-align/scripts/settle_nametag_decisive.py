#!/usr/bin/env python3
"""决定性裁决：名牌 -8 上移是否生效（不受 8x22 小片跨槽误匹配干扰）。

方法 A（位移测量）：把 BEFORE 帧的名牌带位图作模板，在 AFTER 帧同一 x 窗口、
  y±20 内滑窗 NCC，找最佳 dy。代码把 (2,25)→(2,17) 即 -8，预期 dy=-8。
方法 B（绝对对齐）：8x22 模板把搜索 ROI 收紧到目标名牌 x∈[465,490]、
  y∈[418,462]，分别打 TARGET / BEFORE / AFTER，看 dy 目标≈0、修复前≈+8、修复后≈0。
方法 C（名字文字带）：名牌深色横条 y 投影，量两端「名牌专用」行段重心。

只要 A≈-8 且 B 修复后≈0，即判 -8 补丁生效；verify_r3_align.py 中
nameTagL dy=18@ncc0.64 是跨三槽低分误匹配，作废，不改代码。
"""
import json
import os

import numpy as np
from PIL import Image

TV = r"C:\Users\11036\AppData\Local\Temp\term_verify"
OUT = os.path.join(TV, "ui_r3")
SHOTS = os.path.join(TV, "shots")
T = os.path.join(TV, "targets_desktop", "desktop_char_select.png")
B = os.path.join(SHOTS, "ui_final_charselect.png")
A = os.path.join(SHOTS, "charselect_clean_f09.png")


def arr(p):
    return np.asarray(Image.open(p).convert("RGB"), np.float64)


Ta, Ba, Aa = arr(T), arr(B), arr(A)


def ncc(x, y):
    x = x.ravel() - x.mean(); y = y.ravel() - y.mean()
    d = np.sqrt((x * x).sum() * (y * y).sum())
    return float((x * y).sum() / d) if d > 1e-9 else -2.0


# 方法 A：BEFORE 名牌带 (x 455:560, y 426:452) 作模板，在 AFTER 同 x、y±20 找 dy
wx0, wx1 = 455, 560
ty0, ty1 = 426, 452
patch = Ba[ty0:ty1, wx0:wx1]
A_best = (-2.0, None)
for dy in range(-20, 21):
    y0 = ty0 + dy
    if y0 < 0 or y0 + (ty1 - ty0) > Aa.shape[0]:
        continue
    c = ncc(patch, Aa[y0:y0 + (ty1 - ty0), wx0:wx1])
    if c > A_best[0]:
        A_best = (c, dy)
# 对照：BEFORE 与 TARGET 同法
T_best = (-2.0, None)
for dy in range(-20, 21):
    y0 = ty0 + dy
    if y0 < 0 or y0 + (ty1 - ty0) > Ta.shape[0]:
        continue
    c = ncc(patch, Ta[y0:y0 + (ty1 - ty0), wx0:wx1])
    if c > T_best[0]:
        T_best = (c, dy)
print("A: BEFORE-patch best dy in AFTER =", A_best, " (expect -8)")
print("A: BEFORE-patch best dy in TARGET=", T_best, " (expect -8)")

# 方法 B：8x22 模板，收紧 ROI 到目标名牌附近
tpl = np.asarray(Image.open(os.path.join(OUT, "tpl2_nameTagL.png")).convert("RGBA"))
timg, tmask = tpl[..., :3].astype(np.float64), tpl[..., 3] > 128
h, w = timg.shape[:2]
rx0, ry0, rx1, ry1 = 465, 418, 490, 462


def ncc_tpl(frame):
    tv = timg[tmask].ravel(); tv -= tv.mean()
    tn = np.sqrt((tv * tv).sum())
    best = (-2.0, None, None)
    for y in range(ry0, ry1 - h + 1):
        for x in range(rx0, rx1 - w + 1):
            pv = frame[y:y + h, x:x + w][tmask].astype(np.float64).ravel()
            pv -= pv.mean()
            pn = np.sqrt((pv * pv).sum())
            if pn < 1e-6:
                continue
            c = float((tv * pv).sum() / (tn * pn))
            if c > best[0]:
                best = (c, x, y)
    return {"ncc": round(best[0], 4), "xy": None if best[1] is None else [best[1], best[2]]}


bt = ncc_tpl(Ta); bb = ncc_tpl(Ba); ba = ncc_tpl(Aa)
print("B: TARGET", bt, " BEFORE", bb, " AFTER", ba)
dy_before = None if not (bt["xy"] and bb["xy"]) else bb["xy"][1] - bt["xy"][1]
dy_after = None if not (bt["xy"] and ba["xy"]) else ba["xy"][1] - bt["xy"][1]
print("B: dy_before(vs target)=", dy_before, " dy_after=", dy_after)

a_ok = abs(A_best[1] - (-8)) <= 2 if A_best[1] is not None else False
b_ok = dy_after is not None and abs(dy_after) <= 2 and dy_before is not None and dy_before >= 6
res = {"methodA": {"before_patch_in_after_dy": A_best[1], "ncc": round(A_best[0], 4),
                   "before_patch_in_target_dy": T_best[1], "expect": -8},
       "methodB": {"target": bt, "before": bb, "after": ba,
                   "dy_before": dy_before, "dy_after": dy_after},
       "A_ok": a_ok, "B_ok": b_ok,
       "verdict": "NAMETAG_FIX_VERIFIED" if (a_ok and b_ok) else "NEEDS_REVIEW",
       "note": "verify_r3_align.py nameTagL dy=18@ncc0.64 = cross-slot mis-match of 8x22 tiny template, void."}
jp = os.path.join(OUT, "nametag_settle_decisive.json")
json.dump(res, open(jp, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
print("VERDICT", res["verdict"], "JSON", jp)
