"""事实驱动：对 6 帧左侧栏 x[0,160) y[0,470) 逐行算暗像素占比，
输出连续段(y 区间)，钉死每页目标端到底有几块横向木牌。
不靠肉眼、不靠历史摘要。"""
import os, json
import numpy as np
from PIL import Image

ROOT = r"C:\Users\11036\AppData\Local\Temp\term_verify"
PAIRS = [
    ("TGT_login",    "targets_desktop/desktop_login.png"),
    ("TGT_channel",  "targets_desktop/desktop_channel_expanded.png"),
    ("TGT_char",     "targets_desktop/desktop_char_select.png"),
    ("TGT_race",     "targets_desktop/desktop_racecreate_race.png"),
    ("TGT_create",   "targets_desktop/desktop_racecreate_adventurer.png"),
    ("OUR_char",     "shots/v2_racecreate_settled.png"),
    ("OUR_race",     "shots/scene_reset_1_race_before.png"),
    ("OUR_create",   "shots/scene_reset_2_creation.png"),
]

def segments(dark_ratio, thr=0.35, minlen=6):
    segs, start = [], None
    for y, r in enumerate(dark_ratio):
        if r > thr and start is None:
            start = y
        elif r <= thr and start is not None:
            if y - start >= minlen:
                segs.append([start, y - 1])
            start = None
    if start is not None and len(dark_ratio) - start >= minlen:
        segs.append([start, len(dark_ratio) - 1])
    return segs

out = {}
for name, rel in PAIRS:
    p = os.path.join(ROOT, rel)
    if not os.path.exists(p):
        out[name] = {"error": "MISSING", "path": p}
        continue
    im = np.asarray(Image.open(p).convert("RGB"))[:470, :160, :].astype(np.float32)
    # 暗像素：亮度 < 90（木牌深红/棕），排除天空/草地亮背景
    lum = 0.299 * im[:, :, 0] + 0.587 * im[:, :, 1] + 0.114 * im[:, :, 2]
    dark_ratio = (lum < 90).mean(axis=1)
    segs = segments(dark_ratio)
    # 各段峰值占比
    peaks = [round(float(dark_ratio[a:b + 1].max()), 3) for a, b in segs]
    out[name] = {"path": rel, "segments": segs, "seg_peaks": peaks,
                 "mean_dark": round(float(dark_ratio.mean()), 4)}
    print(name, "segments=", segs, "peaks=", peaks, "mean_dark=", out[name]["mean_dark"])

dst = os.path.join(ROOT, "ui_left_planks", "settle_plank_counts.json")
json.dump(out, open(dst, "w", encoding="utf-8"), indent=2, ensure_ascii=False)
print("WROTE", dst)
