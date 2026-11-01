#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""_p17_allframes.py -- exhaustive frame verification.

The earlier passes only tested a hand-picked short-list of `backgrnd` bitmaps.
This one exports EVERY bitmap whose node name starts with "backgrnd" that lives
under UIWindow.img (from the full depth-6 dump _uiwin_full.txt), then ring-band
matches every one of them against every screenshot, so "which node is the frame"
is answered by measurement rather than by assumption.

Ring band = the 4 px wide border ring of the template's opaque bbox.  A frame is
accepted when, at its best (dx,dy):
    ring MAE <= RING_MAE_MAX (6.0)  and  ring n >= 800
MAE is the mean per-channel absolute difference over the ring's opaque pixels.
"""
import io, os, re, sys
import numpy as np
from PIL import Image

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = os.path.dirname(os.path.abspath(__file__))
NX = r"E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx"
AD = os.path.join(OUT, "_nx_all")
if not os.path.isdir(AD):
    os.makedirs(AD)
RING = 4
RING_MAE_MAX = 6.0
RNG = range(-16, 17)

sys.path.insert(0, r"E:\project\MapleStory-Client\tools")
from nx_inspect import Nx, write_png

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


def bgra_to_rgba(w, h, raw):
    a = np.frombuffer(raw, dtype=np.uint8).reshape(h, w, 4)
    return a[:, :, [2, 1, 0, 3]].copy()


def opaque_bbox(a, lo=250):
    m = a[:, :, 3] >= lo
    ys, xs = np.nonzero(m)
    if len(xs) == 0:
        return None
    return int(xs.min()), int(xs.max()), int(ys.min()), int(ys.max())


def ring_mask(a, obb, band=RING):
    x0, x1, y0, y1 = obb
    H, W = a.shape[:2]
    m = np.zeros((H, W), bool)
    m[y0:y0 + band, x0:x1 + 1] = True
    m[y1 - band + 1:y1 + 1, x0:x1 + 1] = True
    m[y0:y1 + 1, x0:x0 + band] = True
    m[y0:y1 + 1, x1 - band + 1:x1 + 1] = True
    return m & (a[:, :, 3] == 255)


text = io.open(os.path.join(OUT, "_uiwin_full.txt"), encoding="utf-8").read()
cands = []
for full, name, typ, rest in parse(text):
    if typ != "bitmap" or not name.startswith("backgrnd"):
        continue
    m = re.search(r"(\d+)x(\d+)", rest)
    if not m:
        continue
    cands.append((full, int(m.group(1)), int(m.group(2))))
# de-duplicate identical paths, keep only frames plausibly big enough
cands = sorted(set(cands))

nx = Nx(NX)
loaded = {}
rep = io.open(os.path.join(OUT, "_p17_allframes.txt"), "w", encoding="utf-8")
W = rep.write
W("backgrnd bitmaps under UIWindow.img found in the dump: %d\n" % len(cands))

SHOTS = [("勋章框.png", "medal"), ("好友列表框.png", "friend"),
         ("组员列表框.png", "party"), ("拒绝列表框.png", "blacklist"),
         ("家族联盟.png", "guildunion"),
         ("好友、组员、家族、家族联盟、拒绝列表.png", "allfive"),
         ("个人请报框.png", "userinfo"), ("学院.png", "academy")]

# preload every candidate as an RGBA numpy array (and cache the PNG on disk)
for full, tw, th in cands:
    nid = nx.find(full)
    if nid is None:
        W("  MISSING in nx: %s\n" % full)
        continue
    w, h, raw = nx.bitmap_pixels(nid)
    if not raw:
        W("  EMPTY bitmap: %s\n" % full)
        continue
    a = bgra_to_rgba(w, h, raw)
    loaded[full] = (w, h, a)
    flat = full.replace("/", "_")
    if not os.path.exists(os.path.join(AD, flat + ".png")):
        write_png(os.path.join(AD, flat + ".png"), w, h, raw)
W("loaded %d candidate frames\n\n" % len(loaded))

for fn, stem in SHOTS:
    img = np.array(Image.open(os.path.join(SRC, fn)).convert("RGBA")).astype(np.int16)
    H, Iw = img.shape[:2]
    res = []
    for full, (tw, th, a) in loaded.items():
        obb = opaque_bbox(a)
        if obb is None:
            continue
        ow = obb[1] - obb[0] + 1
        oh = obb[3] - obb[2] + 1
        if not (0.55 * Iw <= ow <= 1.35 * Iw and 0.55 * H <= oh <= 1.35 * H):
            continue
        rm = ring_mask(a, obb)
        ys, xs = np.nonzero(rm)
        if len(xs) < 800:
            continue
        tt = a[:, :, :3][ys, xs].astype(np.int16)
        best = None
        for dy in RNG:
            for dx in RNG:
                ii, jj = ys + dy, xs + dx
                ok = (ii >= 0) & (ii < H) & (jj >= 0) & (jj < Iw)
                n = int(ok.sum())
                # the crops are loose: up to ~40% of the ring may fall outside
                # the image, so only a small in-image sample is rejected.
                if n < 800:
                    continue
                d = np.abs(img[ii[ok], jj[ok], :3] - tt[ok]).sum(axis=1)
                mae = float(d.sum()) / (n * 3.0)
                if best is None or mae < best[0]:
                    best = (mae, dx, dy, n)
        if best:
            res.append((best[0], full, tw, th, obb, best[1], best[2], best[3]))
    res.sort()
    ok = [r for r in res if r[0] <= RING_MAE_MAX]
    W("=" * 78 + "\n%s  %dx%d  -- %d of %d size-plausible frames within ring-MAE<=%.2f\n"
      % (fn, Iw, H, len(ok), len(res), RING_MAE_MAX))
    for (mae, full, tw, th, obb, dx, dy, n) in res[:8]:
        W("   %s MAE=%6.3f n=%-5d dx=%+d dy=%+d  %-52s bitmap %dx%d opaque %dx%d  "
          "outer x[%d..%d] y[%d..%d]\n"
          % ("ACCEPT" if mae <= RING_MAE_MAX else "reject", mae, n, dx, dy, full, tw, th,
             obb[1] - obb[0] + 1, obb[3] - obb[2] + 1,
             dx + obb[0], dx + obb[1], dy + obb[2], dy + obb[3]))
    if not ok:
        W("   NO MATCH among UIWindow.img backgrnd* bitmaps\n")
    W("\n")
rep.close()
print("ok")
