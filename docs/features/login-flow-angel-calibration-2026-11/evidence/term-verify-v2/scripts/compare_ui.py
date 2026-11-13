#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
UI 层像素闭环：本端改后 GL 渲染帧  vs  correct_shots 目标帧
纯 numpy + PIL，无 cv2 / 无 scipy。
输出：全局 NCC(灰度+逐通道)、分块 NCC 热区、差分图、50% 叠图、
      可选模板多峰 NCC 量频道网格步长(列/行)，对比两端。
用法:
  python compare_ui.py <shot.png> <target.png> [tmpl.png] [roi x0,y0,x1,y1] [-o outdir]
JSON 打到 stdout；-o 时另存 evidence.json + diff.png + blend.png。
"""
import sys, json, os
import numpy as np
from PIL import Image


def load_rgba(p):
    return np.asarray(Image.open(p).convert("RGBA"), dtype=np.float64)


def to_gray(rgba):
    rgb = rgba[..., :3]
    return 0.299 * rgb[..., 0] + 0.587 * rgb[..., 1] + 0.114 * rgb[..., 2]


def ncc_vec(a, b):
    """两同形向量/矩阵的零均值归一化互相关（标量）。"""
    x = a - a.mean()
    y = b - b.mean()
    d = np.sqrt((x * x).sum() * (y * y).sum())
    return float((x * y).sum() / d) if d > 1e-9 else 0.0


def align_size(shot, target):
    """尺寸不同则把 target 中心裁/补到 shot 的 HxW。"""
    if shot.shape[:2] == target.shape[:2]:
        return target
    H, W = shot.shape[:2]
    t = np.zeros((H, W, 4), dtype=np.float64)
    t[..., 3] = 0
    th, tw = target.shape[:2]
    hh, ww = min(H, th), min(W, tw)
    sy, sx = (th - hh) // 2, (tw - ww) // 2
    dy, dx = (H - hh) // 2, (W - ww) // 2
    t[dy:dy + hh, dx:dx + ww] = target[sy:sy + hh, sx:sx + ww]
    return t


def global_ncc(shot, target):
    gs, gt = to_gray(shot), to_gray(target)
    out = {"gray": round(ncc_vec(gs, gt), 6)}
    for i, ch in enumerate("RGB"):
        out[ch] = round(ncc_vec(shot[..., i], target[..., i]), 6)
    # 有效像素：两端 alpha 都 >8
    valid = (shot[..., 3] > 8) & (target[..., 3] > 8)
    if valid.sum() > 2:
        out["gray_valid_only"] = round(ncc_vec(gs[valid], gt[valid]), 6)
        out["valid_px"] = int(valid.sum())
    return out


def block_ncc(shot, target, grid=8, thresh=0.80):
    """grid×grid 分块 NCC，返回低于 thresh 的差异热区。"""
    gs, gt = to_gray(shot), to_gray(target)
    H, W = gs.shape
    hot, cells = [], []
    for r in range(grid):
        for c in range(grid):
            y0, y1 = r * H // grid, (r + 1) * H // grid
            x0, x1 = c * W // grid, (c + 1) * W // grid
            v = ncc_vec(gs[y0:y1, x0:x1], gt[y0:y1, x0:x1])
            cell = {"r": r, "c": c, "box": [x0, y0, x1, y1], "ncc": round(v, 4)}
            cells.append(cell)
            if v < thresh:
                hot.append(cell)
    hot.sort(key=lambda d: d["ncc"])
    return {"grid": grid, "thresh": thresh, "hot_count": len(hot),
            "hot": hot[:16], "mean": round(float(np.mean([c["ncc"] for c in cells])), 4)}


def template_ncc_map(gray, tmpl, roi):
    """带 alpha 掩码的滑窗零均值 NCC 相关图；分批处理窗口行防内存爆。"""
    from numpy.lib.stride_tricks import sliding_window_view
    x0, y0, x1, y1 = roi
    g = to_gray(tmpl)
    a = tmpl[..., 3] > 8
    tm = np.where(a, g - g[a].mean(), 0.0)
    tnorm = float(np.sqrt((tm * tm).sum()))
    th, tw = tm.shape
    sub = np.ascontiguousarray(gray[y0:y1, x0:x1])
    H, W = sub.shape
    if H < th or W < tw:
        return None, (x0, y0, th, tw)
    wins = sliding_window_view(sub, (th, tw))
    out = np.full((H - th + 1, W - tw + 1), -1.0)
    for r in range(0, wins.shape[0], 16):
        wb = wins[r:r + 16].astype(np.float64)
        wm = wb - wb.mean(axis=(2, 3), keepdims=True)
        num = (wm * tm[None, None]).sum(axis=(2, 3))
        den = np.sqrt((wm * wm).sum(axis=(2, 3))) * tnorm
        out[r:r + 16] = np.where(den > 1e-6, num / np.maximum(den, 1e-6), -1.0)
    return out, (x0, y0, th, tw)


def nms_peaks(cmap, off, topn=12, min_dist=20, thresh=0.5):
    x0, y0, th, tw = off
    work = cmap.copy()
    peaks = []
    for _ in range(topn * 40):
        idx = np.unravel_index(np.argmax(work), work.shape)
        v = float(work[idx])
        if v < thresh:
            break
        ry, rx = int(idx[0]), int(idx[1])
        peaks.append([rx + x0, ry + y0, round(v, 4)])
        ya, yb = max(0, ry - min_dist), min(work.shape[0], ry + min_dist + 1)
        xa, xb = max(0, rx - min_dist), min(work.shape[1], rx + min_dist + 1)
        work[ya:yb, xa:xb] = -1.0
        if len(peaks) >= topn:
            break
    return peaks


def step_from(vals):
    """一维坐标序列 → 相邻差中位数(步长)。"""
    if len(vals) < 2:
        return None
    s = sorted(set(int(v) for v in vals))
    # 合并 6px 内近似重复
    merged = []
    for v in s:
        if not merged or v - merged[-1] > 6:
            merged.append(v)
        else:
            merged[-1] = (merged[-1] + v) // 2
    diffs = [merged[i + 1] - merged[i] for i in range(len(merged) - 1)]
    diffs = [d for d in diffs if d > 3]
    return {"coords": merged, "step_median": int(np.median(diffs)) if diffs else None,
            "diffs": diffs}


def grid_steps(gray, tmpl, roi):
    cmap, off = template_ncc_map(gray, tmpl, roi)
    if cmap is None:
        return {"error": "roi smaller than template"}
    peaks = nms_peaks(cmap, off, topn=14, min_dist=22, thresh=0.5)
    xs = [p[0] for p in peaks]
    ys = [p[1] for p in peaks]
    return {"peak_count": len(peaks),
            "peaks": peaks[:14],
            "col": step_from(xs),
            "row": step_from(ys),
            "best": round(max((p[2] for p in peaks), default=-1.0), 4)}


def main():
    args = [a for a in sys.argv[1:]]
    outdir = None
    if "-o" in args:
        i = args.index("-o")
        outdir = args[i + 1]
        del args[i:i + 2]
    if len(args) < 2:
        print(json.dumps({"error": "need <shot.png> <target.png> [tmpl.png] [x0,y0,x1,y1]"}))
        sys.exit(2)
    shot_p, target_p = args[0], args[1]
    tmpl_p, roi = None, None
    for a in args[2:]:
        if "," in a:
            roi = [int(v) for v in a.split(",")]
        else:
            tmpl_p = a
    shot = load_rgba(shot_p)
    target = align_size(shot, load_rgba(target_p))
    res = {"shot": os.path.basename(shot_p), "target": os.path.basename(target_p),
           "size": [int(shot.shape[1]), int(shot.shape[0])],
           "global": global_ncc(shot, target),
           "blocks": block_ncc(shot, target)}
    if tmpl_p and roi:
        tmpl = load_rgba(tmpl_p)
        gs, gt = to_gray(shot), to_gray(target)
        res["template"] = os.path.basename(tmpl_p)
        res["tmpl_size"] = [int(tmpl.shape[1]), int(tmpl.shape[0])]
        res["roi"] = roi
        res["grid_target"] = grid_steps(gt, tmpl, roi)
        res["grid_shot"] = grid_steps(gs, tmpl, roi)
    if outdir:
        os.makedirs(outdir, exist_ok=True)
        diff = np.abs(to_gray(shot) - to_gray(target))
        d8 = np.clip(diff * 3.0, 0, 255).astype(np.uint8)
        Image.fromarray(d8, "L").save(os.path.join(outdir, "diff.png"))
        blend = (shot[..., :3] * 0.5 + target[..., :3] * 0.5).clip(0, 255).astype(np.uint8)
        Image.fromarray(blend, "RGB").save(os.path.join(outdir, "blend.png"))
        with open(os.path.join(outdir, "evidence.json"), "w", encoding="utf-8") as f:
            json.dump(res, f, ensure_ascii=False, indent=2)
    print(json.dumps(res, ensure_ascii=False))


if __name__ == "__main__":
    main()
