#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""_p18_rightwin.py -- the SECOND window in 个人请报框.png.

个人请报框.png is 511x197 and holds two overlapping windows: the 「个人情报」
window (UIWindow.img/UserInfo/backgrnd, matched at dx=0 dy=-1, outer x[0..274])
and a narrower one to its right.  A whole-image ring match cannot find the
second one (the image is far wider than it), so its outer rectangle is measured
from the dark frame lines inside x>=276 first, and only then are candidate
`backgrnd` bitmaps of that size ring-matched.
"""
import io, os, sys
import numpy as np
from PIL import Image

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = os.path.dirname(os.path.abspath(__file__))
NX = r"E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx"
sys.path.insert(0, r"E:\project\MapleStory-Client\tools")
from nx_inspect import Nx

TH = 40          # dark-ish frame line threshold on max channel
rep = io.open(os.path.join(OUT, "_p18_rightwin.txt"), "w", encoding="utf-8")
W = rep.write

a = np.array(Image.open(os.path.join(SRC, "个人请报框.png")).convert("RGB")).astype(np.int16)
H, Iw = a.shape[:2]
dark = (a.max(axis=2) < 130)
W("image %dx%d ; dark = max(r,g,b) < 130\n\n" % (Iw, H))

RX0 = 276
W("--- horizontal frame lines measured inside x[%d..%d] ---\n" % (RX0, Iw - 2))
for y in range(H):
    c = int(dark[y, RX0:].sum())
    if c >= 60:
        xs = np.nonzero(dark[y, RX0:])[0] + RX0
        W("   y=%-4d dark=%-4d x[%d..%d]\n" % (y, c, xs[0], xs[-1]))
W("--- vertical frame lines measured inside y[0..%d] ---\n" % (H - 40))
for x in range(RX0, Iw):
    c = int(dark[:H - 40, x].sum())
    if c >= 60:
        ys = np.nonzero(dark[:H - 40, x])[0]
        W("   x=%-4d dark=%-4d y[%d..%d]\n" % (x, c, ys[0], ys[-1]))

# ---- ring match of every UIWindow backgrnd* against the right-hand window ----
def bgra_to_rgba(w, h, raw):
    b = np.frombuffer(raw, dtype=np.uint8).reshape(h, w, 4)
    return b[:, :, [2, 1, 0, 3]].copy()


def opaque_bbox(arr, lo=250):
    m = arr[:, :, 3] >= lo
    ys, xs = np.nonzero(m)
    if len(xs) == 0:
        return None
    return int(xs.min()), int(xs.max()), int(ys.min()), int(ys.max())


def ring_mask(arr, obb, band=4):
    x0, x1, y0, y1 = obb
    hh, ww = arr.shape[:2]
    m = np.zeros((hh, ww), bool)
    m[y0:y0 + band, x0:x1 + 1] = True
    m[y1 - band + 1:y1 + 1, x0:x1 + 1] = True
    m[y0:y1 + 1, x0:x0 + band] = True
    m[y0:y1 + 1, x1 - band + 1:x1 + 1] = True
    return m & (arr[:, :, 3] == 255)


import re
def parse(text):
    stack = {}
    for line in text.splitlines():
        if line.startswith("====="):
            continue
        m = re.match(r"^(\s*)(\S+) \[(\w+)\](.*)$", line)
        if not m:
            continue
        ind, name, typ, rest = m.groups()
        lvl = len(ind) // 2
        stack[lvl] = name
        for k in list(stack):
            if k > lvl:
                del stack[k]
        yield "/".join(stack[i] for i in range(0, lvl + 1)), name, typ, rest.strip()


text = io.open(os.path.join(OUT, "_uiwin_full.txt"), encoding="utf-8").read()
cands = sorted({(f, int(m.group(1)), int(m.group(2)))
                for f, n, t, r in parse(text) if t == "bitmap" and n.startswith("backgrnd")
                for m in [re.search(r"(\d+)x(\d+)", r)] if m})
nx = Nx(NX)
img = a
res = []
for full, tw, th in cands:
    nid = nx.find(full)
    if nid is None:
        continue
    w, h, raw = nx.bitmap_pixels(nid)
    if not raw:
        continue
    arr = bgra_to_rgba(w, h, raw)
    obb = opaque_bbox(arr)
    if obb is None:
        continue
    ow, oh = obb[1] - obb[0] + 1, obb[3] - obb[2] + 1
    if not (150 <= ow <= 260 and 100 <= oh <= 340):
        continue
    rm = ring_mask(arr, obb)
    ys, xs = np.nonzero(rm)
    tt = arr[:, :, :3][ys, xs].astype(np.int16)
    best = None
    for dy in range(-oh + 60, 40, 2):
        for dx in range(262, 292, 1):
            ii, jj = ys + dy, xs + dx
            ok = (ii >= 0) & (ii < H) & (jj >= 0) & (jj < Iw)
            n = int(ok.sum())
            if n < 700:
                continue
            # the ring must sit in the RIGHT half only
            if (jj[ok] < RX0).mean() > 0.05:
                continue
            mae = float(np.abs(img[ii[ok], jj[ok], :3] - tt[ok]).sum()) / (n * 3.0)
            if best is None or mae < best[0]:
                best = (mae, dx, dy, n)
    if best:
        # refine the coarse (step-2) dy by +/-1 so the reported offset is exact
        cmae, cdx, cdy, _ = best
        for dy in range(cdy - 1, cdy + 2):
            for dx in range(cdx - 1, cdx + 2):
                ii, jj = ys + dy, xs + dx
                ok = (ii >= 0) & (ii < H) & (jj >= 0) & (jj < Iw)
                n = int(ok.sum())
                if n < 700 or (jj[ok] < RX0).mean() > 0.05:
                    continue
                mae = float(np.abs(img[ii[ok], jj[ok], :3] - tt[ok]).sum()) / (n * 3.0)
                if mae < best[0]:
                    best = (mae, dx, dy, n)
        res.append((best[0], full, ow, oh, best[1], best[2], best[3]))
res.sort()
W("\n--- ring match of UIWindow.img backgrnd* (opaque 150..260 x 100..340) against the\n"
  "    right-hand window; dx searched 240..299, dy searched (100-oh)..39;\n"
  "    a placement is only scored if >=95%% of its ring pixels are at x>=%d ---\n" % RX0)
for (mae, full, ow, oh, dx, dy, n) in res[:15]:
    W("   MAE=%6.3f n=%-5d dx=%+d dy=%+d  %-52s opaque %dx%d  outer x[%d..%d] y[%d..%d]\n"
      % (mae, n, dx, dy, full, ow, oh, dx, dx + ow - 1, dy, dy + oh - 1))
if not res:
    W("   NO candidate survived the size filter\n")
rep.close()
print("ok")
