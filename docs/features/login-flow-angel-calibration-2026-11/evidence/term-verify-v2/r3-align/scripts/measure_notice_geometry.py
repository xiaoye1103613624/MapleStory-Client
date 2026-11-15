#!/usr/bin/env python3
"""量两端公告框的绝对外轮廓四边 + 标题/正文相对框顶的偏移。

背景：notice_slide_curve.json 证明 f01..f11 稳态，但
  nw  dy=-1 / title dy=-12 / roller_bottom dy=-8 三个量不一致，
  ⇒ 可能不是纯平移，而是框尺寸/内部布局差异。
本脚本用「暖亮纸木色」判据直接量框四边，判到底该不该整体挪。
"""
import json
import os

import numpy as np
from PIL import Image, ImageDraw

TV = r"C:\Users\11036\AppData\Local\Temp\term_verify"
OUT = os.path.join(TV, "ui_r3")
TARGET = os.path.join(TV, "targets_desktop", "desktop_channel_expanded.png")
OURS = os.path.join(TV, "shots", "channel_grid_f05.png")

# 公告框搜索窗
X0, Y0, X1, Y1 = 230, 260, 490, 395


def frame_geometry(path, label):
    a = np.asarray(Image.open(path).convert("RGB"), np.int16)
    R, G, B = a[..., 0], a[..., 1], a[..., 2]
    lum = (R * 0.299 + G * 0.587 + B * 0.114)
    # 暖亮纸/木轴：亮 + 偏暖(R>=B) + 非纯绿
    warm = (lum > 178) & (R >= B - 6) & (R > 150) & (G > 140)
    reg = warm[Y0:Y1, X0:X1]
    rowcnt = reg.sum(1)
    colcnt = reg.sum(0)

    # 横带：连续 >=120px 宽的浅色行 → 框顶/底木轴
    rows = [Y0 + i for i, v in enumerate(rowcnt) if v >= 110]
    # 竖带：连续 >=40px 高的浅色列 → 框左右缘
    cols = [X0 + i for i, v in enumerate(colcnt) if v >= 40]

    def runs(vals):
        out = []
        if not vals:
            return out
        s = p = vals[0]
        for v in vals[1:]:
            if v == p + 1:
                p = v
            else:
                out.append([s, p]); s = p = v
        out.append([s, p])
        return out

    rr = runs(rows)
    cr = runs(cols)
    # 棕红标题墨行（不依赖模板）：R>G+18, 暗, 在框上半
    ink = (R > 60) & (R < 195) & (R > G + 16) & (G >= B - 10) & (lum < 155)
    inkreg = ink[Y0:Y1, 270:470]
    inkcnt = inkreg.sum(1)
    inkrows = [Y0 + i for i, v in enumerate(inkcnt) if v >= 12]
    ir = runs(inkrows)

    res = {"warm_row_runs_ge110": rr, "warm_col_runs_ge40": cr,
           "ink_runs_ge12": ir,
           "frame_top": rr[0][0] if rr else None,
           "frame_bottom": rr[-1][1] if rr else None,
           "frame_left": cr[0][0] if cr else None,
           "frame_right": cr[-1][1] if cr else None,
           "title_ink_run": ir[0] if ir else None}
    print(label, json.dumps(res, ensure_ascii=False))
    return a, res


T, rt = frame_geometry(TARGET, "TARGET")
O, ro = frame_geometry(OURS, "OURS")

shift = {}
for k in ("frame_top", "frame_bottom", "frame_left", "frame_right"):
    shift[k] = None if rt[k] is None or ro[k] is None else ro[k] - rt[k]
print("EDGE_SHIFT(ours-target)", json.dumps(shift, ensure_ascii=False))

# 3x 对照裁图，画两端框 bbox
box = (X0, Y0, X1, Y1)
scale = 3
tif = Image.open(TARGET).convert("RGB").crop(box)
oif = Image.open(OURS).convert("RGB").crop(box)
w, h = tif.size
canvas = Image.new("RGB", (w * scale, h * scale * 2 + 26), (18, 18, 18))


def draw(im, r, color):
    d = ImageDraw.Draw(im)
    if r["frame_left"] is not None and r["frame_top"] is not None:
        d.rectangle([r["frame_left"] - X0, r["frame_top"] - Y0,
                     r["frame_right"] - X0, r["frame_bottom"] - Y0],
                    outline=color, width=1)
    return im


canvas.paste(draw(tif, rt, (255, 40, 40)).resize((w * scale, h * scale), Image.NEAREST), (0, 0))
canvas.paste(draw(oif, ro, (40, 255, 40)).resize((w * scale, h * scale), Image.NEAREST), (0, h * scale + 26))
p = os.path.join(OUT, "notice_geometry.png"); canvas.save(p)
print("CROP", p)

verdict = ("PURE_TRANSLATION" if all(v is not None and abs(v - shift["frame_top"]) <= 3
                                    for k, v in shift.items() if k != "frame_top")
           else "NONUNIFORM_GEOMETRY")
res = {"target": rt, "ours": ro, "edge_shift": shift, "verdict": verdict,
       "crop": p,
       "note": "If edges shift uniformly, +15/+WS_NOTICE_GAP patch is right. "
               "If nonuniform (top vs bottom differ), frame SIZE differs and translation alone over-corrects."}
jp = os.path.join(OUT, "notice_geometry.json")
json.dump(res, open(jp, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
print("VERDICT", verdict, "JSON", jp)
