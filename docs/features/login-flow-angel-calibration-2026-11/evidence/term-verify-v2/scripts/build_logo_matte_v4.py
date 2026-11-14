#!/usr/bin/env python3
"""登录 LOGO 覆盖素材：字形 alpha 遮罩（v4，用 10 帧背景中位数做基准）。

决定性实测（无覆盖层、同一实例 10 帧，ROI 434,80,397x219）：
  · 字形实心核心（lum<70, 17779px）跨帧 std mean = 2.27，frac>6 = 0.143  → 字本身是静止贴图
  · 非字区                     跨帧 std mean =10.98，frac>6 = 0.570        → 背景透过半透明光晕在动
  · 区块整体 std p90 = 23.44，frac>6 = 0.483
  ⇒ 该区域的"动画"是**背景穿过 LOGO 半透明光晕**，不是 LOGO 自己在动。
  ⇒ 整块不透明贴图会把 48% 的动像素冻成 0，违反「动画要一致」。

正确做法：alpha=1 只在**字形实心**处（新字 + 旧字实心，旧字必须盖掉），
光晕/外围 alpha=0 让背景动画透出，边缘软过渡避免硬边。

背景基准：本次用 background_frames/ 里 10 帧（当时覆盖层已临时移走）的逐像素中位数。

自评四项（全满足才签 MATTE_OK）：
  · fit_ncc_vs_target > 0.97
  · old_glyph_leftover_px == 0
  · anim_survival_ratio >= 0.8     掩码外帧间变化保留比例（相对无覆盖层基准）
  · corners_alpha_max < 0.05

用法：
  python build_logo_matte_v4.py            # 生成 + 自评
  python build_logo_matte_v4.py --deploy   # 同时部署到运行目录与仓库资源目录
"""
import argparse
import glob
import json
import os
import shutil

import numpy as np
from PIL import Image, ImageFilter

TV = r"C:\Users\11036\AppData\Local\Temp\term_verify"
BOX = (434, 80, 434 + 397, 80 + 219)
TGT = os.path.join(TV, "targets_desktop", "desktop_login.png")
OLD_LOGO = r"E:\MXD\图片素材\MSTitle.png"
BG_GLOB = os.path.join(TV, "shots", "logoanim_f*.png")   # 无覆盖层时的连续帧
OUT_DIR = os.path.join(TV, "ui_gap_audit", "v3")
DEPLOY_PATHS = [
    r"E:\MXD\BeiDou-Client-xy\UI\Creation\LoginLogo_override.png",
    r"E:\project\MapleStory-Client\docs\features\login-flow-ui-ida-align\resources\creation\LoginLogo_override.png",
]


def ncc(a, b):
    a = a - a.mean()
    b = b - b.mean()
    return float((a * b).sum() / np.sqrt((a * a).sum() * (b * b).sum() + 1e-9))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--deploy", action="store_true")
    ap.add_argument("--ink-thresh", type=float, default=52.0)
    ap.add_argument("--edge-blur", type=float, default=1.4)
    args = ap.parse_args()

    fs = sorted(glob.glob(BG_GLOB))
    if len(fs) < 4:
        raise SystemExit(f"需要 >=4 帧无覆盖层背景帧，实际 {len(fs)}")

    tgt = Image.open(TGT).convert("RGB").crop(BOX)
    T = np.asarray(tgt, np.float32)

    stack = np.stack([np.asarray(Image.open(f).convert("RGB").crop(BOX), np.float32) for f in fs])
    bg_med = np.median(stack, axis=0)
    bg_std = stack.std(0).max(2)                     # 该像素动画强度
    bg_lum_med = bg_med.mean(2)

    old = Image.open(OLD_LOGO).convert("RGBA")
    oa = np.asarray(old, np.float32)[..., 3] / 255.0

    R, G, B = T[..., 0], T[..., 1], T[..., 2]
    T_lum = T.mean(2)
    chroma = T.max(2) - T.min(2)
    neutral = (chroma < 52) & (np.abs(R - B) < 42)

    # 目标相对背景中位数的"更暗"增量 = 目标图上的墨迹（实心+光晕）
    ink = bg_lum_med - T_lum
    # 只信"背景本来就稳"的像素（std 小），动画区不可判，交给旧 alpha 兜底
    stable = bg_std < 8.0
    solid_new = (ink > args.ink_thresh) & neutral & stable
    # 叶片实心：红
    solid_leaf = ((R - B) > 58) & ((R - G) > 12) & (R > 115)
    # 旧字实心：旧贴图 alpha 高处必须盖掉
    solid_old = oa > 0.60

    core = (solid_new | solid_leaf | solid_old).astype(np.float32)
    sup = np.asarray(Image.fromarray((core * 255).astype(np.uint8))
                     .filter(ImageFilter.MaxFilter(7)), np.float32) / 255.0
    core = core * (sup > 0)

    alpha = np.asarray(Image.fromarray((core * 255).astype(np.uint8))
                       .filter(ImageFilter.MaxFilter(3))
                       .filter(ImageFilter.GaussianBlur(args.edge_blur)), np.float32) / 255.0
    alpha = np.clip(alpha, 0, 1)

    matte = np.dstack([T, alpha[..., None] * 255]).astype(np.uint8)
    os.makedirs(OUT_DIR, exist_ok=True)
    matte_path = os.path.join(OUT_DIR, "LoginLogo_matte.png")
    Image.fromarray(matte, "RGBA").save(matte_path)

    al = alpha[..., None]
    comp = T * al + bg_med * (1 - al)

    # 动画存活：用两帧真实背景分别合成，比较掩码外区域是否仍变化
    i0, i1 = 0, len(fs) // 2
    f0, f1 = stack[i0], stack[i1]
    c0 = T * al + f0 * (1 - al)
    c1 = T * al + f1 * (1 - al)
    out_mask = alpha < 0.05
    d_after = np.abs(c0 - c1).max(2)[out_mask]
    d_before = np.abs(f0 - f1).max(2)[out_mask]

    corners = [alpha[0:24, 0:24], alpha[0:24, -24:], alpha[-24:, 0:24], alpha[-24:, -24:]]

    res = {
        "box": list(BOX),
        "bg_frames": len(fs),
        "ink_thresh": args.ink_thresh,
        "edge_blur": args.edge_blur,
        "alpha_mean": round(float(alpha.mean()), 4),
        "alpha_gt_half_frac": round(float((alpha > 0.5).mean()), 4),
        "alpha_zero_frac": round(float((alpha < 0.02).mean()), 4),
        "fit_ncc_vs_target": round(ncc(comp, T), 4),
        "fit_gray_ncc_vs_target": round(ncc(comp.mean(2), T_lum), 4),
        "fit_mean_abs": round(float(np.abs(comp - T).mean()), 3),
        "old_glyph_leftover_px": int(((oa > 0.5) & (alpha < 0.5)).sum()),
        "anim_in_outside_mask_before": {
            "mean": round(float(d_before.mean()), 3),
            "frac_gt16": round(float((d_before > 16).mean()), 4)},
        "anim_in_outside_mask_after": {
            "mean": round(float(d_after.mean()), 3),
            "frac_gt16": round(float((d_after > 16).mean()), 4)},
        "anim_survival_ratio": round(
            float((d_after > 16).mean() / max((d_before > 16).mean(), 1e-9)), 4),
        "corners_alpha_max": [round(float(c.max()), 4) for c in corners],
        "matte_path": matte_path,
        "matte_bytes": os.path.getsize(matte_path),
    }
    ok = (res["fit_ncc_vs_target"] > 0.97
          and res["old_glyph_leftover_px"] == 0
          and res["anim_survival_ratio"] >= 0.8
          and max(res["corners_alpha_max"]) < 0.05)
    res["verdict"] = "MATTE_OK" if ok else "MATTE_NEEDS_WORK"

    if args.deploy and ok:
        for p in DEPLOY_PATHS:
            os.makedirs(os.path.dirname(p), exist_ok=True)
            shutil.copyfile(matte_path, p)
        res["deployed"] = DEPLOY_PATHS

    sheet = Image.new("RGB", (397, 219 * 3 + 16), (20, 20, 20))
    sheet.paste(tgt, (0, 0))
    sheet.paste(Image.fromarray(comp.astype(np.uint8)), (0, 227))
    sheet.paste(Image.fromarray((alpha * 255).astype(np.uint8)).convert("RGB"), (0, 454))
    sheet.resize((794, (219 * 3 + 16) * 2), Image.NEAREST).save(
        os.path.join(OUT_DIR, "logo_matte_preview.png"))

    jp = os.path.join(TV, "shots", "logo_matte_verdict.json")
    with open(jp, "w", encoding="utf-8") as fh:
        json.dump(res, fh, ensure_ascii=False, indent=2)
    print(json.dumps(res, ensure_ascii=False, indent=2))
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
