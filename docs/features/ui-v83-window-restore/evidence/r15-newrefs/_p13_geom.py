#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""_p13_geom.py -- skeleton geometry of the drawn (non-bitmap-matched) chrome.

The template matcher (_p7/_p8) finds controls whose art exists as its own bitmap
in UI.nx.  Things the client draws procedurally -- tab outlines, banner boxes,
list frames, red separator rules -- have no single source bitmap, so their boxes
are measured from the screenshot instead:

  ink  = r<120 and g<120 and b<120                      (dark glyph / outline px)
  red  = |r-204|<=17 and g<=34 and b<=34                (the 204,0,0 separator)
  H-rule = a row whose ink count >= HROW (120)          -> a horizontal rule
  V-rule = a column whose ink count >= VCOL (60)        -> a vertical rule
  run    = maximal contiguous x with ink>0 inside a y-band, width >= MINW (3)

All numbers below come from this script only.
"""
import io, os
import numpy as np
from PIL import Image

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = os.path.dirname(os.path.abspath(__file__))
HROW, VCOL, MINW = 120, 60, 3

SHOTS = [("勋章框.png", "medal"), ("好友列表框.png", "friend"),
         ("组员列表框.png", "party"), ("拒绝列表框.png", "blacklist"),
         ("家族联盟.png", "guildunion"),
         ("好友、组员、家族、家族联盟、拒绝列表.png", "allfive"),
         ("个人请报框.png", "userinfo"), ("学院.png", "academy")]

rep = io.open(os.path.join(OUT, "_p13_geom.txt"), "w", encoding="utf-8")
W = rep.write
for fn, stem in SHOTS:
    a = np.array(Image.open(os.path.join(SRC, fn)).convert("RGB")).astype(np.int32)
    H, Wd = a.shape[:2]
    ink = (a[:, :, 0] < 120) & (a[:, :, 1] < 120) & (a[:, :, 2] < 120)
    red = (np.abs(a[:, :, 0] - 204) <= 17) & (a[:, :, 1] <= 34) & (a[:, :, 2] <= 34)
    W("=" * 78 + "\n%s  %dx%d  (%s)\n" % (fn, Wd, H, stem))
    rc = red.sum(axis=1)
    W("  red-rule rows (red px >= 30): ")
    W(", ".join("y=%d:%d" % (y, rc[y]) for y in range(H) if rc[y] >= 30) + "\n")
    for y in range(H):
        if rc[y] >= 30:
            xs = np.nonzero(red[y])[0]
            W("     y=%d red x[%d..%d]\n" % (y, xs[0], xs[-1]))
    ri = ink.sum(axis=1)
    W("  H-rules (ink>= %d): " % HROW)
    W(", ".join("y=%d(%d)" % (y, ri[y]) for y in range(H) if ri[y] >= HROW) + "\n")
    ci = ink.sum(axis=0)
    W("  V-rules (ink>= %d): " % VCOL)
    W(", ".join("x=%d(%d)" % (x, ci[x]) for x in range(Wd) if ci[x] >= VCOL) + "\n")

    # section bands delimited by H-rules; inside each, list the ink x-runs
    hr = [y for y in range(H) if ri[y] >= HROW]
    edges = [0] + hr + [H - 1]
    W("  sections between consecutive H-rules (ink x-runs, width>=%d):\n" % MINW)
    for i in range(len(edges) - 1):
        y0, y1 = edges[i], edges[i + 1]
        if y1 - y0 < 2:
            continue
        sub = ink[y0:y1 + 1]
        col = sub.sum(axis=0)
        runs, s = [], None
        for x in range(Wd):
            if col[x] > 0 and s is None:
                s = x
            elif col[x] == 0 and s is not None:
                if x - s >= MINW:
                    runs.append((s, x - 1))
                s = None
        if s is not None and Wd - s >= MINW:
            runs.append((s, Wd - 1))
        prof = sub.sum(axis=1)
        W("    y[%3d..%3d] h=%-3d inkrows=%-3d runs=%d: %s\n"
          % (y0, y1, y1 - y0 + 1, int((prof > 0).sum()), len(runs),
             " ".join("x[%d..%d]" % r for r in runs[:24]) +
             (" ..." if len(runs) > 24 else "")))
    W("\n")
rep.close()
print("ok")
