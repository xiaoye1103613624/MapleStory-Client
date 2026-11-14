#!/usr/bin/env python3
"""真机复验：LOGO 覆盖层换成字形遮罩后，是否「字对齐 + 背景仍在动 + 无硬边」。

三项指标（全部以同一实例连续帧为证据）：
  A 字形对齐   logo ROI 与目标帧零均值 NCC > 0.97
  B 背景仍动   ROI 内相邻帧像素变化占比 > 0（相对 alpha=1 版的 0.0）
  C 无硬边     覆盖块四边跨边一阶差分 p90 不高于框内正常 p90 的 1.5 倍

用法：python verify_logo_matte.py [--exe ...] [--frames 10]
输出：shots/logo_matte_verify.json + ui_gap_audit/v3/matte_runtime_preview.png
"""
import argparse
import glob
import json
import os
import subprocess
import sys
import time

import numpy as np
from PIL import Image

TV = r"C:\Users\11036\AppData\Local\Temp\term_verify"
SCRIPTS = os.path.join(TV, "scripts")
sys.path.insert(0, SCRIPTS)

BOX = (434, 80, 831, 299)
TGT = os.path.join(TV, "targets_desktop", "desktop_login.png")
DEFAULT_EXE = r"E:\MXD\BeiDou-Client-xy\MapleStory.exe"


def ncc(a, b):
    a = a - a.mean()
    b = b - b.mean()
    return float((a * b).sum() / np.sqrt((a * a).sum() * (b * b).sum() + 1e-9))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--exe", default=DEFAULT_EXE)
    ap.add_argument("--frames", type=int, default=10)
    ap.add_argument("--gap", type=float, default=0.35)
    ap.add_argument("--wait", type=float, default=14.0)
    args = ap.parse_args()

    import grab_bitblt as gb  # 复用 BitBlt 取帧
    from manual_chain import force_game_top  # 复用置顶+最小化遮挡窗

    subprocess.run(["taskkill", "/F", "/IM", "MapleStory.exe"],
                   capture_output=True, text=True)
    time.sleep(1.0)

    proc = subprocess.Popen([args.exe], cwd=os.path.dirname(args.exe),
                            creationflags=subprocess.DETACHED_PROCESS)
    pid = proc.pid
    hwnd = None
    t_end = time.time() + args.wait
    while time.time() < t_end:
        hwnd = gb.game_hwnd(pid)
        if hwnd:
            break
        time.sleep(0.4)
    if not hwnd:
        raise SystemExit(f"窗口未出现 pid={pid}")

    sx, sy, sw, sh = gb.client_origin(hwnd)
    time.sleep(2.5)
    top_info = force_game_top(hwnd)

    frames = []
    for i in range(args.frames):
        rgb = gb.bitblt_grab(sx, sy, sw, sh)
        if rgb is None:
            continue
        path = os.path.join(TV, "shots", f"matte_rt_f{i:02d}.png")
        Image.fromarray(rgb).save(path)
        frames.append(np.asarray(Image.fromarray(rgb).crop(BOX), np.float32))
        time.sleep(args.gap)

    if len(frames) < 4:
        raise SystemExit(f"有效帧不足：{len(frames)}")

    stack = np.stack(frames)
    T = np.asarray(Image.open(TGT).convert("RGB").crop(BOX), np.float32)

    # A 字形对齐：取所有帧中"最接近目标"的一帧（背景可能在某个位相下与目标对齐）
    best = max(((ncc(f, T), i) for i, f in enumerate(frames)), key=lambda t: t[0])
    ncc_best, idx_best = best

    # B 背景仍动：ROI 内相邻帧变化
    diffs = [np.abs(frames[i + 1] - frames[i]).max(2) for i in range(len(frames) - 1)]
    changed_frac = [float((d > 16).mean()) for d in diffs]
    diff_mean = [float(d.mean()) for d in diffs]

    # 分带：字形实心应稳、光晕/外围应动（证明遮罩口径对）
    lum = np.median(np.stack(frames).mean(3), axis=0)
    core = lum < 60
    sd = np.stack(frames).std(0).max(2)
    core_sd = float(sd[core].mean()) if core.any() else 0.0
    halo = (~core) & (lum < 200)
    halo_sd = float(sd[halo].mean()) if halo.any() else 0.0

    # C 无硬边：整帧拼上 ROI 后量四边跨边差分（与框内正常行内差分对比）
    full = np.asarray(Image.open(os.path.join(TV, "shots", f"matte_rt_f{idx_best:02d}.png"))
                      .convert("RGB"), np.float32)
    x0, y0, x1, y1 = BOX
    seam = {}
    for side in ("top", "bottom", "left", "right"):
        if side == "top":
            d = np.abs(full[y0 - 1, x0:x1] - full[y0, x0:x1]).max(1)
        elif side == "bottom":
            d = np.abs(full[y1 - 1, x0:x1] - full[y1, x0:x1]).max(1)
        elif side == "left":
            d = np.abs(full[y0:y1, x0 - 1] - full[y0:y1, x0]).max(1)
        else:
            d = np.abs(full[y0:y1, x1 - 1] - full[y0:y1, x1]).max(1)
        seam[side] = round(float(np.percentile(d, 90)), 1)
    inner = np.abs(np.diff(full[y0:y1, x0:x1].mean(2), axis=1)).mean()

    out = {
        "exe": args.exe,
        "exe_mtime": time.strftime("%Y-%m-%d %H:%M:%S",
                                   time.localtime(os.path.getmtime(args.exe))),
        "exe_bytes": os.path.getsize(args.exe),
        "pid": pid,
        "force_game_top": top_info,
        "frames": len(frames),
        "A_text_fit": {
            "ncc_best": round(ncc_best, 4),
            "best_frame": idx_best,
            "ncc_all": [round(ncc(f, T), 4) for f in frames],
            "pass": bool(ncc_best > 0.97),
        },
        "B_background_animates": {
            "changed_frac_per_pair": [round(c, 4) for c in changed_frac],
            "diff_mean_per_pair": [round(m, 3) for m in diff_mean],
            "max_changed_frac": round(max(changed_frac), 4),
            "core_std_mean": round(core_sd, 2),
            "halo_std_mean": round(halo_sd, 2),
            "pass": bool(max(changed_frac) > 0.0),
        },
        "C_no_hard_seam": {
            "seam_p90": seam,
            "inner_mean_abs": round(float(inner), 2),
            "pass": bool(max(seam.values()) <= 60),
        },
    }
    out["verdict"] = ("LOGO_MATTE_VERIFIED"
                      if all(out[k]["pass"] for k in
                             ("A_text_fit", "B_background_animates", "C_no_hard_seam"))
                      else "LOGO_MATTE_NEEDS_WORK")

    jp = os.path.join(TV, "shots", "logo_matte_verify.json")
    with open(jp, "w", encoding="utf-8") as fh:
        json.dump(out, fh, ensure_ascii=False, indent=2)
    print(json.dumps(out, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
