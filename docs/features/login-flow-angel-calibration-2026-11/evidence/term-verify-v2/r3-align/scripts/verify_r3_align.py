#!/usr/bin/env python3
"""R3 修复后真机复验：新二进制 50ec0bd3 的稳态帧逐组件 bbox 收敛。

证据链：
  修复前 = ui_r3/measure_r3_final.json（charInfo -23 / scroll -3 / nameTag +8 / notice -34,-12）
  修复后 = 本脚本对新抓帧逐模板滑窗 NCC
判据（用户 m06596/m06605/m06637 的原生对齐要求）：
  charInfo / scroll / noticeTitle / noticeNw : |shift_x|<=2 且 |shift_y|<=2
  nameTagL : |shift_y|<=2（x 差<=5 系选中角色名长度导致 CENTER 左片位移，已知非 bug）
另外输出帧序列稳定性（每帧 bbox），坐实「动画收敛到稳态」而非拿过场帧凑数。
"""
import json
import os

import numpy as np
from PIL import Image

TV = r"C:\Users\11036\AppData\Local\Temp\term_verify"
OUT = os.path.join(TV, "ui_r3")
SHOTS = os.path.join(TV, "shots")
TGT = os.path.join(TV, "targets_desktop")

TPLS = {
    "charInfo":    ("tpl2_charInfo.png",    "char", (330, 150, 640, 380)),
    "scroll":      ("tpl2_scroll.png",      "char", (350, 120, 620, 400)),
    # 8x22 名牌左端片极小，宽 ROI 会跨三槽误匹配（曾配到 [459,448] 另一槽）。
    # 收紧到目标名牌 (469,430) 邻域，只在本槽内搜索；X 差<=5 是名字长度导致
    # CENTER 左片漂移（已知非 bug），本槽 Y 才是判据。
    "nameTagL":    ("tpl2_nameTagL.png",    "char", (465, 418, 492, 462)),
    "noticeTitle": ("tpl2_noticeTitle.png", "chan", (180, 250, 560, 380)),
    "noticeNw":    ("tpl2_noticeNw.png",    "chan", (200, 250, 560, 400)),
}

# 目标帧 + 目标 bbox（来自 measure_r3_final.json，目标侧不随二进制变）
TARGET_BBOX = {
    "charInfo":    [392, 220, 575, 335],
    "scroll":      [372, 161, 589, 354],
    "nameTagL":    [469, 430, 477, 452],
    "noticeTitle": [293, 313, 463, 331],
    "noticeNw":    [280, 296, 302, 336],
}

NEW_FRAMES = {
    "char": [os.path.join(SHOTS, "charselect_clean_f%02d.png" % i) for i in range(10)],
    "chan": [os.path.join(SHOTS, "channel_grid_f%02d.png" % i) for i in range(12)],
}
# 单帧代表（用于最终判定）：char 取最亮稳态 f09、chan 取 f05
PICK = {"char": 9, "chan": 5}
# 修复前帧（留作对照）：char = 旧 ui_final_charselect.png；chan 旧值已在 json 里
OLD_CHAR = os.path.join(SHOTS, "ui_final_charselect.png")


def load_tpl(p):
    a = np.asarray(Image.open(p).convert("RGBA"))
    return a[..., :3].astype(np.float64), a[..., 3] > 128


def ncc_best(frame, t, m, roi):
    x0, y0, x1, y1 = roi
    h, w = t.shape[:2]
    tv = t[m].ravel().copy(); tv -= tv.mean()
    tn = np.sqrt((tv * tv).sum())
    best = (-2.0, None, None)
    for y in range(y0, y1 - h + 1):
        for x in range(x0, x1 - w + 1):
            pv = frame[y:y + h, x:x + w][m].astype(np.float64).ravel()
            pv -= pv.mean()
            pn = np.sqrt((pv * pv).sum())
            if pn < 1e-6:
                continue
            c = float((tv * pv).sum() / (tn * pn))
            if c > best[0]:
                best = (c, x, y)
    return {"ncc": round(best[0], 4),
            "bbox": None if best[1] is None else [best[1], best[2], best[1] + w, best[2] + h]}


def arr(p):
    return np.asarray(Image.open(p).convert("RGB"), np.float64)


def main():
    tpls = {k: load_tpl(os.path.join(OUT, v[0])) for k, v in TPLS.items()}
    res = {"binary": "50ec0bd3226a2cb502229c2d88455c13 / 2026-11-15 13:30:45 / 5117952B",
           "target_bbox": TARGET_BBOX, "components": {}, "frame_sequences": {}}

    for key, (fn, page, roi) in TPLS.items():
        t, m = tpls[key]
        tb = TARGET_BBOX[key]
        # 逐帧稳定性
        seq = []
        for i, p in enumerate(NEW_FRAMES[page]):
            if not os.path.exists(p):
                continue
            a = arr(p)
            r = ncc_best(a, t, m, roi)
            seq.append({"i": i, "mean": round(float(a.mean()), 2),
                        "ncc": r["ncc"], "bbox": r["bbox"],
                        "dy": None if not r["bbox"] else r["bbox"][1] - tb[1]})
        res["frame_sequences"][key] = seq
        # 代表帧
        pk = PICK[page]
        rep = next((s for s in seq if s["i"] == pk), None)
        # 修复前对照
        old = None
        if page == "char" and os.path.exists(OLD_CHAR):
            # 修复前同用收紧 ROI（宽 ROI 会跨槽误匹配）。
            # 名牌用目标帧 T 自身复测为基准由 TARGET_BBOX 给出；历史 bbox 取自
            # measure_r3_final.json（474,438）/ nameTagL 模板，此处只做同 ROI 复测。
            orr = ncc_best(arr(OLD_CHAR), t, m, roi)
            old = {"ncc": orr["ncc"], "bbox": orr["bbox"],
                   "shift": None if not orr["bbox"] else
                            [orr["bbox"][j] - tb[j] for j in range(4)]}
        shift = None if not (rep and rep["bbox"]) else [rep["bbox"][j] - tb[j] for j in range(4)]
        # 判定
        if shift is None:
            ok = False
        elif key == "nameTagL":
            ok = abs(shift[1]) <= 2 and abs(shift[3]) <= 2 and abs(shift[0]) <= 5
        else:
            ok = all(abs(v) <= 2 for v in shift)
        # 稳态：后 6 帧 y 极差 <=2
        tail = [s["dy"] for s in seq[6:] if s["dy"] is not None]
        steady = bool(tail) and (max(tail) - min(tail) <= 2)
        res["components"][key] = {"pick_frame": pk, "rep": rep, "before": old,
                                  "shift_after": shift, "pass": ok,
                                  "tail_stable": steady,
                                  "tail_dy": tail}
        print("%-12s before=%-26s after=%-26s shift=%-22s steady=%s pass=%s"
              % (key,
                 str(old["shift"]) if old else "(-23 etc, see json)",
                 str(rep["bbox"]) if rep else None,
                 str(shift), steady, ok))

    res["all_pass"] = all(v["pass"] for v in res["components"].values())
    res["all_steady"] = all(v["tail_stable"] for v in res["components"].values())
    jp = os.path.join(OUT, "verify_r3_align.json")
    json.dump(res, open(jp, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
    print("ALL_PASS", res["all_pass"], "ALL_STEADY", res["all_steady"], "JSON", jp)


if __name__ == "__main__":
    main()
