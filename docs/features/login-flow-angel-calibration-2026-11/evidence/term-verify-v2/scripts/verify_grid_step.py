#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
频道网格 CELL_W/CELL_H 校准的「本端改后渲染帧 ↔ 目标帧」像素闭环。

之前 a90fda3 只拿目标帧量出 94/32 改代码，缺本端渲染帧这一端。
本脚本两端用同一算法，把 CELL_W=94 从「照目标帧改代码」升级成
「本端渲染实测 = 目标帧」。

主方法: 列信号自相关测水平周期(对负载条/高亮叠加不敏感)
  - 取频道首行水平条带, 逐列算「格内深色像素占比」得到 1D 信号
  - 去均值后对 lag∈[60,130] 归一化自相关, 峰值 lag = 列步长
旁证: WZ 抽出的 channel/0/normal(92x30) 模板滑窗 NCC(高阈值只留真峰)

输出 ui_channel/grid_step_verdict.json
"""
import os, json
import numpy as np
from PIL import Image

ROOT = r"C:\Users\11036\AppData\Local\Temp\term_verify"
SHOT = os.path.join(ROOT, "shots", "channel_grid_f05.png")
TGT = os.path.join(ROOT, "targets", "channel_selected.png")
CAND = [r"E:\project\MapleStory-Client\docs\features\login-flow-angel-calibration-2026-11\evidence\r29-pxalign\tmpl_channel0_normal.png",
        os.path.join(ROOT, "evidence", "r29-pxalign", "tmpl_channel0_normal.png"),
        os.path.join(ROOT, "shots", "tmpl_channel0_normal.png")]


def load_rgba(p):
    return np.asarray(Image.open(p).convert("RGBA"), dtype=np.float64)


def load_rgb(p):
    return np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)


def gray(x):
    return 0.299 * x[..., 0] + 0.587 * x[..., 1] + 0.114 * x[..., 2]


def col_period(img, y0, y1, x0, x1, lag_lo=60, lag_hi=130):
    """水平条带内逐列深像素占比信号的去均值自相关 → 主周期。"""
    g = gray(img[y0:y1, x0:x1])
    # 单元位图是米色底 + 深色 CH.N 文字/边框; 用「暗于条带中位」的列占比
    thr = np.median(g) - 8
    sig = (g < thr).mean(axis=0).astype(float)
    sig -= sig.mean()
    n = len(sig)
    ac = []
    for lag in range(lag_lo, lag_hi + 1):
        a, b = sig[:n - lag], sig[lag:]
        d = np.sqrt((a * a).sum() * (b * b).sum())
        ac.append((lag, float((a * b).sum() / d) if d > 1e-9 else 0.0))
    ac.sort(key=lambda t: -t[1])
    return {"best_lag": ac[0][0], "best_corr": round(ac[0][1], 4),
            "top5": [[l, round(v, 4)] for l, v in ac[:5]]}


def ncc_masked(shot_rgb, tmpl_rgba, x, y):
    a = tmpl_rgba[..., 3]
    m = a > 40
    if m.sum() < 50:
        return -1.0
    h, w = tmpl_rgba.shape[:2]
    reg = shot_rgb[y:y + h, x:x + w, :3]
    tg = gray(reg)
    tt = gray(tmpl_rgba[..., :3])
    tv = tt[m] - tt[m].mean()
    sv = tg[m] - tg[m].mean()
    d = np.sqrt((tv * tv).sum() * (sv * sv).sum())
    return float((tv * sv).sum() / d) if d > 1e-9 else 0.0


def scan(img, tmpl, roi, step=2, min_ncc=0.85):
    x0, y0, x1, y1 = roi
    th, tw = tmpl.shape[:2]
    vals = []
    for y in range(y0, min(y1, img.shape[0] - th), step):
        for x in range(x0, min(x1, img.shape[1] - tw), step):
            v = ncc_masked(img, tmpl, x, y)
            if v >= min_ncc:
                vals.append((v, x, y))
    vals.sort(reverse=True)
    peaks = []
    for v, x, y in vals:
        if all(abs(x - px) > 60 or abs(y - py) > 20 for _, px, py in peaks):
            peaks.append((v, x, y))
        if len(peaks) >= 8:
            break
    return peaks


def main():
    tp = next((p for p in CAND if os.path.exists(p)), None)
    res = {"note": "本端服务端只发 3 频道 → 只实测列步长; 行步长 CELL_H=32 以目标帧+代码常量对账",
           "template": tp}
    shot = load_rgb(SHOT)
    tgt = load_rgb(TGT)

    # 主方法: 两端同算法, 频道首行条带
    # 本端 3 格首行 y≈336..362, x 覆盖 450..760
    sp = col_period(shot, 336, 362, 450, 760)
    # 目标 20 格首行 y≈338..364, x 覆盖 460..850
    tp_ = col_period(tgt, 338, 364, 460, 850)
    res["shot_period"] = sp
    res["target_period"] = tp_

    # 旁证: NCC 真峰(高阈值)
    if tp:
        tmpl = load_rgba(tp)
        res["template_size"] = [int(tmpl.shape[1]), int(tmpl.shape[0])]
        ps = scan(load_rgba(SHOT), tmpl, (430, 326, 780, 372), min_ncc=0.85)
        res["shot_ncc_peaks"] = sorted(
            [{"x": x, "y": y, "ncc": round(v, 4)} for v, x, y in ps], key=lambda d: d["x"])
        xs = [p["x"] for p in res["shot_ncc_peaks"]]
        res["shot_ncc_steps"] = [xs[i + 1] - xs[i] for i in range(len(xs) - 1)]

    # 判定: 两端列步长都≈94(±3)
    s_lag = sp["best_lag"]
    t_lag = tp_["best_lag"]
    shot_ok = abs(s_lag - 94) <= 3
    tgt_ok = abs(t_lag - 94) <= 3
    ncc_ok = bool(res.get("shot_ncc_steps") and
                  all(abs(v - 94) <= 3 for v in res["shot_ncc_steps"]))
    res["verdict"] = {
        "shot_col_step_is_94": bool(shot_ok),
        "target_col_step_is_94": bool(tgt_ok),
        "shot_ncc_steps_are_94": bool(ncc_ok),
        "overall": "PIXEL_CLOSED_CELL_W" if (shot_ok and tgt_ok and ncc_ok) else
                   ("PIXEL_CLOSED_CELL_W_PERIOD_ONLY" if (shot_ok and tgt_ok) else "INCONCLUSIVE"),
        "row_step_caveat": "本端仅 3 频道 1 行, CELL_H=32 无法在本端渲染实证; 以目标帧 y=[337,369,401,433,465] + 代码常量对账"
    }
    out = os.path.join(ROOT, "ui_channel")
    os.makedirs(out, exist_ok=True)
    ev = os.path.join(out, "grid_step_verdict.json")
    json.dump(res, open(ev, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
    res["evidence"] = ev
    print(json.dumps(res, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
