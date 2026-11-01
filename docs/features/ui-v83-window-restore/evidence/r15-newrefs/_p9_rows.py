#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""_p9_rows.py -- row-ink projection for list areas + scrollbar geometry.

Method (must be reproducible, so it is stated here and echoed in the report):
  * frame-local coords: image(x,y) = local(x-dx, y-dy) using the dx,dy produced
    by the template match in _p7_buttons.py / _p8_chrome.py.
  * "ink" = a fully-opaque pixel whose *value* channel is dark:
        max(r,g,b) < INK_MAX (=120)
    v83 window panels are light blue-grey (>= 119 on every channel for the
    palette multiples of 17 that appear as panel fills), while glyph/border
    pixels are dark greys/blacks, so this separates text+rule ink from fill.
  * For every frame-local row we count ink pixels in the row (restricted to the
    list area x window, excluding the scrollbar column) -> ink[y].
  * A "band" is a maximal run of rows with ink[y] > 0, merged when the gap
    between runs is <= GAP (=3) rows, and kept only if the run's ink peak
    >= MINPEAK (=3) px.  Pitch = median of band-top differences.
  * "left text x" = the smallest local x that contributes ink inside the band,
    using only bands whose total ink >= MINPEAK*height/2 (i.e. real rows).

Everything printed is ASCII.  Report is UTF-8 (labels are NOT printed here).
"""
import io, os, sys
import numpy as np
from PIL import Image

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = os.path.dirname(os.path.abspath(__file__))

INK_MAX = 120
GAP = 3
MINPEAK = 3

# filename -> (frame node, template size, dx, dy, opaque rect in image coords)
SHOTS = [
    ("勋章框.png", "UIWindow.img/Title/backgrnd", (260, 374), (0, 1), (1, 258, 2, 373)),
    ("好友列表框.png", "UIWindow.img/UserList/backgrnd3", (312, 389), (-4, -4), (-3, 306, -3, 383)),
    ("组员列表框.png", "UIWindow.img/UserList/backgrnd", (312, 389), (1, -3), (2, 311, -2, 384)),
    ("拒绝列表框.png", "UIWindow.img/UserList/backgrnd", (312, 389), (0, 2), (1, 310, 3, 389)),
    ("家族联盟.png", "UIWindow.img/UserList/backgrnd2", (312, 389), (-1, 0), (0, 309, 1, 387)),
    ("好友、组员、家族、家族联盟、拒绝列表.png", "UIWindow.img/UserList/backgrnd2",
     (312, 389), (-1, -2), (0, 309, -1, 385)),
    ("个人请报框.png", "UIWindow.img/UserInfo/backgrnd", (275, 199), (0, -1), (1, 273, 0, 196)),
    ("学院.png", "UIWindow.img/Family/backgrnd", (224, 392), (1, -1), (2, 223, 0, 389)),
]

# list area = local y range (below header / above footer buttons) and x range
# (inside frame, right of the left border, left of the scrollbar arrows).
# These are derived from the control bboxes found by _p7/_p8; stated per shot.
LISTAREA = {
    "勋章框.png":              None,
    "好友列表框.png":           (100, 335, 2, 285),
    "组员列表框.png":           (45, 330, 2, 290),
    "拒绝列表框.png":           (45, 335, 2, 289),
    "家族联盟.png":             (75, 310, 2, 288),
    "好友、组员、家族、家族联盟、拒绝列表.png": (75, 310, 2, 288),
    "个人请报框.png":           None,
    "学院.png":                 None,
}

# scrollbar arrow column found by _p8_chrome.py (image coords), if any
SCROLL = {
    "勋章框.png":      ("Basic.img/VScr4/disabled", (239, 253), (94, 338), (94, 106), (107, 119), (326, 338), None),
    "好友列表框.png":   ("Basic.img/VScr4/enabled", (286, 300), (109, 296), (109, 121), (122, 134), (284, 296), None),
    "组员列表框.png":   ("Basic.img/VScr4/enabled", (291, 305), (50, 327), (50, 62), (63, 75), (315, 327), None),
    "拒绝列表框.png":   ("Basic.img/VScr4/enabled", (290, 304), (55, 332), (55, 67), (68, 80), (320, 332), None),
    "家族联盟.png":     ("Basic.img/VScr4/enabled", (289, 303), (113, 304), (113, 125), (126, 150), (151, 163), (292, 304)),
    "好友、组员、家族、家族联盟、拒绝列表.png":
                       ("Basic.img/VScr4/enabled", (289, 303), (111, 302), (111, 123), (124, 136), (290, 302), None),
    "个人请报框.png":   ("Basic.img/VScr4/enabled", (488, 502), (32, 143), (32, 44), (45, 57), (131, 143), None),
    "学院.png":         (None, None, None, None, None, None, None),
}


def inkmap(rgb):
    return (rgb[:, :, 0].astype(np.int32) < INK_MAX) & \
           (rgb[:, :, 1].astype(np.int32) < INK_MAX) & \
           (rgb[:, :, 2].astype(np.int32) < INK_MAX)


def bands(ink, y0, y1, x0, x1):
    """maximal runs of rows with ink>0, merged across gaps of <= GAP zero rows."""
    prof = ink[y0:y1, x0:x1].sum(axis=1)
    n = len(prof)
    runs = []
    i = 0
    while i < n:
        if prof[i] == 0:
            i += 1
            continue
        j = i
        k = i + 1
        while k < n:
            if prof[k] > 0:
                j = k
                k += 1
            else:
                # try to bridge a gap of <= GAP zero rows
                look = k
                while look < n and look <= k + GAP and prof[look] == 0:
                    look += 1
                if look < n and look <= k + GAP and prof[look] > 0:
                    j = look
                    k = look + 1
                else:
                    break
        runs.append((i, j))
        i = j + 1
    out = []
    for (a, b) in runs:
        seg = prof[a:b + 1]
        if seg.max() >= MINPEAK:
            out.append((y0 + a, y0 + b, int(seg.max()), int(seg.sum())))
    return prof, out


def left_ink_x(ink, y0, y1, x0, x1):
    sub = ink[y0:y1 + 1, :]
    cols = sub[:, x0:x1].sum(axis=0)
    nz = np.nonzero(cols)[0]
    return None if len(nz) == 0 else (x0 + int(nz[0]), x0 + int(nz[-1]))


def main():
    rep = io.open(os.path.join(OUT, "_p9_rows.txt"), "w", encoding="utf-8")
    W = rep.write
    W("INK definition: r<%d and g<%d and b<%d (all three dark) -> dark glyph/rule ink\n"
      % (INK_MAX, INK_MAX, INK_MAX))
    W("band merge gap<=%d rows, band kept when peak ink >= %d px\n" % (GAP, MINPEAK))
    W("coords are FRAME-LOCAL: image(x,y) = local(x-dx, y-dy)\n\n")

    for fn, node, tsize, (dx, dy), obb in SHOTS:
        p = os.path.join(SRC, fn)
        im = np.array(Image.open(p).convert("RGBA"))
        rgb = im[:, :, :3]
        H, Iw = rgb.shape[:2]
        ink = inkmap(rgb)
        W("=" * 78 + "\n")
        W("%s  img %dx%d  frame %s  dx=%d dy=%d\n" % (fn, Iw, H, node, dx, dy))
        W("  ink px in image = %d (%.2f%%)\n" % (int(ink.sum()), 100.0 * ink.sum() / (Iw * H)))

        la = LISTAREA.get(fn)
        if la:
            ly0, ly1, lx0, lx1 = la
            prof, bs = bands(ink, ly0, ly1, lx0, lx1)
            W("  list area (image coords) x[%d..%d] y[%d..%d]\n" % (lx0, lx1, ly0, ly1))
            W("  row-ink profile y:count  (image coords)\n    ")
            line = []
            for y in range(ly0, ly1):
                line.append("%d:%d" % (y, int(prof[y - ly0])))
            for i in range(0, len(line), 12):
                W(" ".join(line[i:i + 12]) + "\n    ")
            W("\n")
            W("  bands (image y): %d found\n" % len(bs))
            tops = []
            for (a, b, pk, tot) in bs:
                tops.append(a)
                W("    y[%3d..%3d] h=%-3d peak=%-4d total_ink=%-5d local y[%d..%d]"
                  % (a, b, b - a + 1, pk, tot, a - dy, b - dy))
                lx = left_ink_x(ink, a, b, lx0, lx1)
                if lx:
                    W("  ink-x[%d..%d] local[%d..%d]" % (lx[0], lx[1], lx[0] - dx, lx[1] - dx))
                W("\n")
            if len(tops) >= 2:
                diffs = [tops[i + 1] - tops[i] for i in range(len(tops) - 1)]
                ds = sorted(diffs)
                W("  band-top pitches: %s   median=%s\n" % (diffs, ds[len(ds) // 2]))
        else:
            W("  (no list area declared for this window)\n")

        sc = SCROLL.get(fn)
        if sc and sc[0]:
            snode, (ax0, ax1), (ty0, ty1), (py0, py1), (by0, by1), (ny0, ny1), th = sc
            W("  scrollbar: arrows+track %s  x[%d..%d] (%dw)\n"
              % (snode, ax0, ax1, ax1 - ax0 + 1))
            W("     track  bbox y[%d..%d] (%dh)  [top arrow top .. bottom arrow bottom]\n"
              % (ty0, ty1, ty1 - ty0 + 1))
            W("     up-arrow    y[%d..%d]   base-tile y[%d..%d]   down-arrow y[%d..%d]\n"
              % (py0, py1, by0, by1, ny0, ny1))
            if th:
                W("     THUMB (matched VScr4/enabled/thumb0 15x25) y[%d..%d] x[%d..%d]\n"
                  % (th[0], th[1], ax0, ax1))
            else:
                # look for a lighter/darker thumb band inside the track heuristically:
                # rows inside the track where the arrow column has a uniform run that
                # differs from the base tile colour at by0..by1
                W("     thumb: not matched to a pack bitmap; row-mean colour scan of\n")
                W("            the track column x[%d..%d] y[%d..%d] below (mean rgb per row)\n"
                  % (ax0, ax1, ty0, ny1))
                W("      ")
                for y in range(ty0, ny1 + 1):
                    m = rgb[y, ax0:ax1 + 1].mean(axis=0).astype(int)
                    W("%d:(%d,%d,%d) " % (y, m[0], m[1], m[2]))
                W("\n")
            W("  full row-ink profile of the WHOLE frame (image y:count) -- for eyeballing\n    ")
            fx0 = max(0, dx); fx1 = min(Iw, dx + tsize[0])
            prof_all = ink[0:H, fx0:fx1].sum(axis=1)
            line = ["%d:%d" % (y, int(prof_all[y])) for y in range(H)]
            for i in range(0, len(line), 14):
                W(" ".join(line[i:i + 14]) + "\n    ")
            W("\n")
        else:
            W("  scrollbar: NONE DETECTED (no VScr* bitmap matched anywhere in the shot)\n")
        W("\n")
    rep.close()
    print("ok")


main()
