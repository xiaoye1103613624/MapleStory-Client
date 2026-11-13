#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
chSelect 红圈动画层终验：本端点 CH.1 后连抓 3 帧，验
  1) 帧间确有变化(抓帧链路活 / 背景动画在跑)
  2) CH.1 格 ROI 内红色像素三帧稳定 + ROI NCC 极高 = 末帧常驻(非循环)
  3) 红色像素质心落在 CH.1 格附近 = 落点与 CH_SELECT_CIRCLE_OFFSET 自洽
对照真值：WorldSelect/channel/chSelect /0 d100 /1 d50 /2 d50 /3 无delay(末帧常驻)
代码：UIWorldSelect.cpp:497-518 播完定格 size()-1；:720-721 非playing 画末帧。
输出 ui_channel/chselect_anim_verdict.json
"""
import os, json
import numpy as np
from PIL import Image

ROOT = r"C:\Users\11036\AppData\Local\Temp\term_verify"
SHOTS = [os.path.join(ROOT, "shots", "chselect_anim_f%02d.png" % i) for i in range(3)]
OUT = os.path.join(ROOT, "ui_channel")


def load(p):
    return np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)


def gray(x):
    return 0.299 * x[..., 0] + 0.587 * x[..., 1] + 0.114 * x[..., 2]


def ncc(x, y):
    x = x - x.mean()
    y = y - y.mean()
    d = np.sqrt((x * x).sum() * (y * y).sum())
    return float((x * y).sum() / d) if d > 1e-9 else 0.0


def red_mask(im):
    r, g, b = im[..., 0], im[..., 1], im[..., 2]
    return (r > 140) & (r > g * 1.45) & (r > b * 1.45) & (g < 150)


def main():
    fs = [load(p) for p in SHOTS]
    res = {"frames": [os.path.basename(p) for p in SHOTS],
           "truth": "chSelect /0 d100 /1 d50 /2 d50 /3 no-delay(last-frame-persist)",
           "code": "UIWorldSelect.cpp:497-518 stop at size()-1; :720-721 draws last frame when !playing"}

    # 1) 帧间差分(全图 + 背景 ROI 云动画)
    diffs = []
    for i in range(len(fs) - 1):
        d = np.abs(fs[i] - fs[i + 1]).mean()
        diffs.append(round(float(d), 4))
    bg_diffs = []
    for i in range(len(fs) - 1):
        # 背景天空区(避开频道面板): 右上
        d = np.abs(fs[i][60:180, 900:1180] - fs[i + 1][60:180, 900:1180]).mean()
        bg_diffs.append(round(float(d), 4))
    res["frame_diff"] = {"global_mean_abs": diffs, "bg_sky_mean_abs": bg_diffs,
                         "capture_alive": bool(max(diffs) > 0.05)}

    # 2) 频道格区域红色像素 + ROI NCC(末帧常驻)
    #    grid_step 实测本端频道格 x=458,552,646 y=336(92x30 normal)，红圈末帧 59x33
    ROI = (430, 305, 570, 380)  # CH.1 格 + 红圈外扩
    x0, y0, x1, y1 = ROI
    red_counts, centroids = [], []
    for f in fs:
        sub = f[y0:y1, x0:x1]
        m = red_mask(sub)
        red_counts.append(int(m.sum()))
        if m.sum() > 0:
            yy, xx = np.nonzero(m)
            centroids.append([round(float(x0 + xx.mean()), 1), round(float(y0 + yy.mean()), 1)])
        else:
            centroids.append(None)
    roi_ncc = [round(ncc(gray(fs[0][y0:y1, x0:x1]), gray(fs[i][y0:y1, x0:x1])), 6)
               for i in range(1, len(fs))]
    res["ch1_red"] = {"roi": ROI, "red_pixel_counts": red_counts,
                      "red_centroids": centroids, "roi_ncc_vs_f00": roi_ncc}

    # 3) 判定(口径先钉死)
    red_stable = len(red_counts) == 3 and min(red_counts) > 20 and \
        (max(red_counts) - min(red_counts)) / max(red_counts) < 0.15
    roi_static = all(v > 0.995 for v in roi_ncc)
    bg_alive = max(bg_diffs) > 0.05
    # 质心应落在 CH.1 格(x∈[440,560]) 附近，且三帧一致
    cent_ok = all(c is not None and 430 < c[0] < 570 and 305 < c[1] < 380 for c in centroids)
    if centroids[0] and centroids[-1]:
        drift = abs(centroids[0][0] - centroids[-1][0]) + abs(centroids[0][1] - centroids[-1][1])
    else:
        drift = 999
    cent_no_drift = drift < 3.0

    overall = "LAST_FRAME_PERSIST_VERIFIED" if (red_stable and roi_static and cent_ok and cent_no_drift) \
        else ("ANIM_PLAYING_OR_NO_RED" if not red_stable else "INCONCLUSIVE")
    res["verdict"] = {
        "red_pixel_stable": bool(red_stable),
        "ch1_roi_static_ncc_gt_0995": bool(roi_static),
        "bg_animation_alive": bool(bg_alive),
        "red_centroid_in_ch1": bool(cent_ok),
        "red_centroid_no_drift_px": round(drift, 2),
        "centroid_no_drift": bool(cent_no_drift),
        "overall": overall,
        "note": "3 frames captured 0.5s after CH.1 click; truth 100+50+50ms => all 3 should be last frame"}

    os.makedirs(OUT, exist_ok=True)
    ev = os.path.join(OUT, "chselect_anim_verdict.json")
    json.dump(res, open(ev, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
    res["evidence"] = ev
    print(json.dumps(res, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
