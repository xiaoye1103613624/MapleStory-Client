#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""用 WZ 真值 CharSelect/BtNew/normal/0 (101x35, ARGB8888) 的 alpha 掩码 NCC
在清晰选角色帧上滑窗定位「创建角色」按钮真实屏幕落点。
输出 ui_charselect_clean/btnew_locate.json + 叠图 btnew_overlay.png
"""
import os, json
import numpy as np
from PIL import Image, ImageDraw

ROOT = r"C:\Users\11036\AppData\Local\Temp\term_verify"
SHOT = os.path.join(ROOT, "shots", "raceselect_f00_baseline.png")
TMPL = os.path.join(ROOT, "shots", "tmpl_BtNew_normal.png")
OUT = os.path.join(ROOT, "ui_charselect_clean")
os.makedirs(OUT, exist_ok=True)


def gray(x):
    return 0.299 * x[..., 0] + 0.587 * x[..., 1] + 0.114 * x[..., 2]


def ncc_masked(shot, tmpl, x, y):
    a = tmpl[..., 3]
    m = a > 40
    if m.sum() < 200:
        return -1.0
    h, w = tmpl.shape[:2]
    reg = shot[y:y + h, x:x + w, :3]
    if reg.shape[:2] != (h, w):
        return -1.0
    tv = gray(tmpl[..., :3])[m]
    sv = gray(reg)[m]
    tv = tv - tv.mean(); sv = sv - sv.mean()
    d = np.sqrt((tv * tv).sum() * (sv * sv).sum())
    return float((tv * sv).sum() / d) if d > 1e-9 else 0.0


def main():
    shot = np.asarray(Image.open(SHOT).convert("RGB"), dtype=np.float64)
    tmpl = np.asarray(Image.open(TMPL).convert("RGBA"), dtype=np.float64)
    th, tw = tmpl.shape[:2]
    H, W = shot.shape[:2]
    # 右侧木牌区域 (据 charselect_signs_zoom 裁框 740..980 / 120..420, 放宽)
    x0, y0, x1, y1 = 700, 120, 1000, 420
    vals = []
    for y in range(y0, min(y1, H - th)):
        for x in range(x0, min(x1, W - tw)):
            v = ncc_masked(shot, tmpl, x, y)
            if v > 0:
                vals.append((v, x, y))
    vals.sort(reverse=True)
    # NMS 保留彼此距离 > 50px 的峰
    peaks = []
    for v, x, y in vals:
        if all(abs(x - px) > 50 or abs(y - py) > 25 for _, px, py in peaks):
            peaks.append((v, x, y))
        if len(peaks) >= 6:
            break
    res = {"method": "alpha-mask zero-mean NCC, WZ CharSelect/BtNew/normal/0 101x35",
           "shot": os.path.basename(SHOT), "template": os.path.basename(TMPL),
           "roi": [x0, y0, x1, y1],
           "peaks": [{"ncc": round(v, 4), "x": x, "y": y,
                      "center": [x + tw // 2, y + th // 2]} for v, x, y in peaks]}
    # 叠图画框
    ov = Image.open(SHOT).convert("RGB")
    d = ImageDraw.Draw(ov)
    for i, (v, x, y) in enumerate(peaks[:3]):
        col = [(255, 0, 0), (0, 255, 0), (0, 128, 255)][i]
        d.rectangle([x, y, x + tw, y + th], outline=col, width=2)
        d.text((x, y - 12), "#%d ncc=%.3f" % (i, v), fill=col)
    ov.save(os.path.join(OUT, "btnew_overlay.png"))
    json.dump(res, open(os.path.join(OUT, "btnew_locate.json"), "w", encoding="utf-8"),
              ensure_ascii=False, indent=2)
    print(json.dumps(res, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
