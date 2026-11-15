#!/usr/bin/env python3
"""决定性实验：量 channel_grid_f00..f11 十二帧里公告模板的 y 收敛曲线。

目的：裁决上一轮「noticeTitle shift=-12 是 notice_slide 过场态还是常量错误」。
  UIWorldSelect.cpp:742  notice_dy = (1-notice_slide)*(-40)
  :140  CH_SLIDE_MS=180ms；capture_channel_grid.py 每帧 sleep(0.18)
  ⇒ 若 f05 仍滑入中，f00..f11 的 title y 应单调下移收敛到稳态；
    若 12 帧 title y 全一致（且比目标 -12），则是稳态常量偏差，可安全改 +15→+27。
同时量公告「整体框」的浅色木轴底缘 y（独立于标题模板）做交叉验证。
"""
import json
import os

import numpy as np
from PIL import Image

TV = r"C:\Users\11036\AppData\Local\Temp\term_verify"
OUT = os.path.join(TV, "ui_r3")
SHOTS = os.path.join(TV, "shots")
TPL = os.path.join(OUT, "tpl2_noticeTitle.png")   # 170x18
NWT = os.path.join(OUT, "tpl2_noticeNw.png")     # 22x40
TARGET = os.path.join(TV, "targets_desktop", "desktop_channel_expanded.png")

ROI = (180, 250, 560, 380)


def load_tpl(p):
    a = np.asarray(Image.open(p).convert("RGBA"))
    return a[..., :3].astype(np.float64), a[..., 3] > 128


def ncc_best(frame, t, m, roi):
    x0, y0, x1, y1 = roi
    h, w = t.shape[:2]
    tv = t[m].ravel(); tv -= tv.mean()
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
    return round(best[0], 4), best[1], best[2]


def bottom_roller_y(frame):
    """公告木轴底缘：ROI(240,330)-(480,390) 内浅色(lum>190)行的最大连续带下沿。"""
    reg = frame[330:390, 240:480]
    lum = reg.mean(2)
    light = lum > 190
    cnt = light.sum(1)
    rows = [330 + i for i, v in enumerate(cnt) if v > 150]
    return max(rows) if rows else None


def main():
    t, m = load_tpl(TPL)
    nwt, nwm = load_tpl(NWT)
    tf = np.asarray(Image.open(TARGET).convert("RGB"), np.float64)
    ct, tx, ty = ncc_best(tf, t, m, ROI)
    cn, nx, ny = ncc_best(tf, nwt, nwm, (200, 250, 400, 380))
    print("TARGET  title ncc=%.4f @(%d,%d)  nw ncc=%.4f @(%d,%d)  roller_bottom=%s"
          % (ct, tx, ty, cn, nx, ny, bottom_roller_y(tf)))

    seq = []
    for i in range(12):
        p = os.path.join(SHOTS, "channel_grid_f%02d.png" % i)
        if not os.path.exists(p):
            continue
        a = np.asarray(Image.open(p).convert("RGB"), np.float64)
        c, x, y = ncc_best(a, t, m, ROI)
        cn, nx2, ny2 = ncc_best(a, nwt, nwm, (200, 250, 400, 380))
        rb = bottom_roller_y(a)
        rec = {"i": i, "mean": round(float(a.mean()), 2),
               "title_ncc": c, "title_xy": [x, y], "title_dy_vs_target": None if y is None else y - ty,
               "nw_ncc": cn, "nw_xy": [nx2, ny2], "nw_dy_vs_target": None if ny2 is None else ny2 - ny,
               "roller_bottom": rb,
               "roller_dy_vs_target": None if rb is None else rb - bottom_roller_y(tf)}
        seq.append(rec)
        print("f%02d mean=%6.2f title ncc=%.4f @(%s,%s) dy=%s | nw @(%s,%s) dy=%s | roller=%s dy=%s"
              % (i, rec["mean"], c, x, y, rec["title_dy_vs_target"],
                 nx2, ny2, rec["nw_dy_vs_target"], rb, rec["roller_dy_vs_target"]))

    # 判定：后 6 帧 title y 是否稳定（max-min<=2）→ steady
    tail = [r["title_xy"][1] for r in seq[6:] if r["title_xy"][1] is not None]
    steady = bool(tail) and (max(tail) - min(tail) <= 2)
    verdict = "STEADY_CONSTANT_OFFSET" if steady and tail and abs(sum(tail) / len(tail) - ty) >= 6 \
        else ("TRANSIENT_SLIDE_IN" if not steady else "ALREADY_ALIGNED")
    res = {"target": {"title_xy": [tx, ty], "title_ncc": ct,
                      "nw_xy": [nx, ny], "roller_bottom": bottom_roller_y(tf)},
           "seq": seq, "tail_y": tail, "tail_stable": steady, "verdict": verdict,
           "note": "CH_SLIDE_MS=180ms; frames sleep 0.18s; f05≈t0.9s. "
                   "If tail y stable and offset≠0 → constant bug, safe to patch +15→+27. "
                   "If y keeps drifting → f05 was transient, do NOT patch y."}
    jp = os.path.join(OUT, "notice_slide_curve.json")
    json.dump(res, open(jp, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
    print("VERDICT", verdict, "JSON", jp)


if __name__ == "__main__":
    main()
