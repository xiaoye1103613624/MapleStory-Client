#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""_p19_allroots.py -- search EVERY .img root of UI.nx (not just UIWindow.img)
for a `backgrnd*` bitmap that can be the right-hand 「道具」 window of
个人请报框.png.

The right-hand window is overlapped on its left by the 个人情报 window and cut
off at the top of the crop, so only two of its frame edges are observable:
a full-height dark column at x=509 (y[0..156]) and a dark row at y=160
(x[276..510]) -- measured by _p18_rightwin.py.  The search therefore anchors a
template so that its opaque bottom-right corner lands on (509, 160) and scores
the ring pixels that are actually visible.
"""
import io, os, re, sys
import numpy as np
from PIL import Image

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = os.path.dirname(os.path.abspath(__file__))
NX = r"E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx"
sys.path.insert(0, r"E:\project\MapleStory-Client\tools")
from nx_inspect import Nx

rep = io.open(os.path.join(OUT, "_p19_allroots.txt"), "w", encoding="utf-8")
W = rep.write
nx = Nx(NX)
N = nx.node_count
first = np.zeros(N, np.int64)
count = np.zeros(N, np.int64)
for i in range(N):
    _, f, c, _, _, _, _ = nx.node(i)
    first[i] = f
    count[i] = c
paths = [""] * N
order = [0]
paths[0] = ""
frontier = [0]
while frontier:
    nxt = []
    for p in frontier:
        for c in range(first[p], first[p] + count[p]):
            nm = nx.name(c)
            paths[c] = (paths[p] + "/" + nm) if paths[p] else nm
            nxt.append(c)
    frontier = nxt
W("nodes=%d  paths built\n" % N)

cands = []
for i in range(N):
    if count[i]:
        continue
    _, _, _, t, _, w, h = nx.node(i)
    if t != 5 or not w:
        continue
    nm = paths[i].rsplit("/", 1)[-1]
    if not nm.startswith("backgrnd"):
        continue
    if not (190 <= w <= 275 and 90 <= h <= 420):
        continue
    cands.append((paths[i], w, h, i))
W("backgrnd* bitmaps 190..275 wide and 90..420 tall across ALL roots: %d\n" % len(cands))
roots = {}
for p, w, h, i in cands:
    roots[p.split("/")[0]] = roots.get(p.split("/")[0], 0) + 1
W("by root: %s\n\n" % sorted(roots.items(), key=lambda kv: -kv[1]))


def bgra(w, h, raw):
    b = np.frombuffer(raw, dtype=np.uint8).reshape(h, w, 4)
    return b[:, :, [2, 1, 0, 3]].copy()


def obbox(a, lo=250):
    m = a[:, :, 3] >= lo
    ys, xs = np.nonzero(m)
    if len(xs) == 0:
        return None
    return int(xs.min()), int(xs.max()), int(ys.min()), int(ys.max())


img = np.array(Image.open(os.path.join(SRC, "个人请报框.png")).convert("RGB")).astype(np.int16)
H, Iw = img.shape[:2]
BRX, BRY = 509, 160          # observable bottom-right corner of the 道具 window
res = []
for p, w, h, i in cands:
    ww, hh, raw = nx.bitmap_pixels(i)
    if not raw:
        continue
    a = bgra(ww, hh, raw)
    ob = obbox(a)
    if ob is None:
        continue
    ow, oh = ob[1] - ob[0] + 1, ob[3] - ob[2] + 1
    if not (200 <= ow <= 262 and 100 <= oh <= 400):
        continue
    m = np.zeros((hh, ww), bool)
    B = 4
    m[ob[2]:ob[2] + B, ob[0]:ob[1] + 1] = True
    m[ob[3] - B + 1:ob[3] + 1, ob[0]:ob[1] + 1] = True
    m[ob[2]:ob[3] + 1, ob[0]:ob[0] + B] = True
    m[ob[2]:ob[3] + 1, ob[1] - B + 1:ob[1] + 1] = True
    m &= (a[:, :, 3] == 255)
    ys, xs = np.nonzero(m)
    tt = a[:, :, :3][ys, xs].astype(np.int16)
    best = None
    # anchor: bottom-right of the opaque bbox on (BRX,BRY), then slide +-6 px
    ax, ay = BRX - ob[1], BRY - ob[3]
    for dy in range(ay - 6, ay + 7):
        for dx in range(ax - 6, ax + 7):
            ii, jj = ys + dy, xs + dx
            ok = (ii >= 0) & (ii < H) & (jj >= 276) & (jj < Iw)
            n = int(ok.sum())
            if n < 900:
                continue
            mae = float(np.abs(img[ii[ok], jj[ok], :3] - tt[ok]).sum()) / (n * 3.0)
            if best is None or mae < best[0]:
                best = (mae, dx, dy, n)
    if best:
        res.append((best[0], p, ow, oh, best[1], best[2], best[3], ob))
res.sort()
W("ring match anchored on the observable bottom-right corner (%d,%d), +-6 px:\n"
  % (BRX, BRY))
for (mae, p, ow, oh, dx, dy, n, ob) in res[:20]:
    W("   MAE=%6.3f n=%-5d dx=%+d dy=%+d  %-56s opaque %dx%d outer x[%d..%d] y[%d..%d]\n"
      % (mae, n, dx, dy, p, ow, oh, dx + ob[0], dx + ob[1], dy + ob[2], dy + ob[3]))
if not res:
    W("   nothing scored\n")
rep.close()
print("ok")
