#!/usr/bin/env python3
"""蓝军自检：覆盖块是否把背景动画冻成死图。

做法：附着到已运行的登录页实例，连抓 N 帧（间隔 gap），比较
  A) 覆盖块 (434,80)-(831,299) 内逐帧差分
  B) 同尺寸对照区（右侧闪光/天空）逐帧差分
若 A 恒为 0 而 B 明显变化 → 覆盖块是静态贴图，把该区域的闪光/飘叶动画冻死了。
"""
import argparse
import ctypes
import ctypes.wintypes as wintypes
import json
import os
import sys
import time

import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from grab_bitblt import BITMAPINFO, BITMAPINFOHEADER, SRCCOPY, bitblt_grab, client_origin, game_hwnd  # noqa: E402

u = ctypes.windll.user32
g = ctypes.windll.gdi32

LOGO_BOX = (434, 80, 434 + 397, 80 + 219)          # 覆盖块
CTRL_BOX = (900, 80, 900 + 397, 80 + 219)          # 同尺寸对照区（右侧天空/闪光）
OUT = r"C:\Users\11036\AppData\Local\Temp\term_verify\shots"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pid", type=int, required=True)
    ap.add_argument("--frames", type=int, default=8)
    ap.add_argument("--gap", type=float, default=0.35)
    args = ap.parse_args()

    hwnd = game_hwnd(args.pid)
    if not hwnd:
        print(json.dumps({"error": "no game hwnd"}))
        return 2
    ox, oy, w, h = client_origin(hwnd)
    print(f"hwnd={hwnd} client_origin=({ox},{oy}) size=({w},{h})")

    frames = []
    for i in range(args.frames):
        rgb = bitblt_grab(ox, oy, w, h)
        if rgb is None:
            print(json.dumps({"error": f"grab failed at frame {i}"}))
            return 3
        frames.append(rgb.astype(np.int16))
        time.sleep(args.gap)

    def box(arr, b):
        x0, y0, x1, y1 = b
        return arr[y0:y1, x0:x1]

    def diffs(b):
        vals = []
        for i in range(1, len(frames)):
            d = np.abs(box(frames[i], b) - box(frames[i - 1], b))
            vals.append((float(d.mean()), float((d.max(2) > 16).mean())))
        return vals

    logo = diffs(LOGO_BOX)
    ctrl = diffs(CTRL_BOX)
    whole = diffs((0, 0, w, h))

    res = {
        "frames": args.frames,
        "gap": args.gap,
        "logo_box": LOGO_BOX,
        "ctrl_box": CTRL_BOX,
        "logo_frames_changed": sum(1 for m, _ in logo if m > 0.5),
        "ctrl_frames_changed": sum(1 for m, _ in ctrl if m > 0.5),
        "logo_consecutive_diffs": [[round(m, 4), round(f, 6)] for m, f in logo],
        "ctrl_consecutive_diffs": [[round(m, 4), round(f, 6)] for m, f in ctrl],
        "whole_consecutive_diffs": [[round(m, 4), round(f, 6)] for m, f in whole],
    }

    # 覆盖块内有变化的像素占比（若恒 0 = 完全静止）
    res["logo_max_changed_frac"] = round(max(f for _, f in logo), 6) if logo else None
    res["ctrl_max_changed_frac"] = round(max(f for _, f in ctrl), 6) if ctrl else None
    res["ctrl_animates"] = res["ctrl_max_changed_frac"] > 0.01
    res["logo_animates"] = res["logo_max_changed_frac"] > 0.01
    if res["ctrl_animates"] and not res["logo_animates"]:
        res["verdict"] = "LOGO_REGION_FROZEN"    # 覆盖块把动画冻死
    elif res["logo_animates"]:
        res["verdict"] = "LOGO_REGION_ANIMATES"  # 覆盖块区域仍有动画（本地 PNG 是静态，除非别层动画盖上来）
    else:
        res["verdict"] = "BOTH_STATIC_INCONCLUSIVE"

    os.makedirs(OUT, exist_ok=True)
    for i, fr in enumerate(frames):
        Image.fromarray(fr.astype(np.uint8)).save(os.path.join(OUT, f"logoanim_f{i:02d}.png"))

    p = os.path.join(OUT, "logo_anim_verdict.json")
    with open(p, "w", encoding="utf-8") as fh:
        json.dump(res, fh, ensure_ascii=False, indent=2)
    print(json.dumps(res, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
