"""Pixel measurements for the runtime Login.img/NewChar step4 artwork.

Run from the repository root:
    python docs/features/create-char-classic-dual-path/evidence/measure_charset.py

All inputs are exported by tools/nx_inspect.py image from the *deployed*
UI.nx (E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx), never from packed WZ.
"""
import os
from collections import Counter
from PIL import Image

D = os.path.dirname(os.path.abspath(__file__))


def load(name):
    return Image.open(os.path.join(D, name)).convert("RGBA")


def counts(im, pred):
    w, h = im.size
    px = im.load()
    rows = []
    for y in range(h):
        n = sum(1 for x in range(w) if pred(px[x, y]))
        rows.append(n)
    return rows


def bands(rows, threshold, minlen=1):
    out, start = [], None
    for y, n in enumerate(rows + [0]):
        if n >= threshold and start is None:
            start = y
        elif n < threshold and start is not None:
            if y - start >= minlen:
                out.append((start, y - 1, max(rows[start:y])))
            start = None
    return out


def bbox(im, pred):
    w, h = im.size
    px = im.load()
    xs, ys = [], []
    for y in range(h):
        for x in range(w):
            if pred(px[x, y]):
                xs.append(x)
                ys.append(y)
    return (min(xs), min(ys), max(xs), max(ys)) if xs else None


def top_colors(im, n=6):
    px = im.load()
    c = Counter(px[x, y][:3] for y in range(im.height) for x in range(im.width)
                if px[x, y][3] > 200)
    return c.most_common(n)


CREAM = lambda p: p[3] > 200 and p[0] > 225 and p[1] > 210 and p[2] > 165
WOOD = lambda p: p[3] > 200 and p[0] > 110 and p[1] < 140 and p[2] < 90
OPAQUE = lambda p: p[3] > 128

print("=== charSet 225x377 ===")
cs = load("native_charSet.png")
rows = counts(cs, CREAM)
print("cream bands (>=150 px/row):", bands(rows, 150, 2))
print("cream bbox:", bbox(cs, CREAM))
print("row samples:", [(y, rows[y]) for y in range(0, 377, 10)])
print("top colors:", top_colors(cs))

print("\n=== avatarSel/0 ===")
for name in ("native_avatarSel_0_normal.png", "native_avatarSel_0_disabled.png",
             "native_avatarSel_4_normal.png", "native_avatarSel_4_disabled.png"):
    im = load(name)
    print(name, "top colors:", top_colors(im, 3))

print("\n=== dice frames (opaque bbox) ===")
for i in range(4):
    im = load("native_dice_%d.png" % i)
    print(i, im.size, bbox(im, OPAQUE), top_colors(im, 2))

print("\n=== scroll frames (cream bbox) ===")
for g in (0, 1):
    for i in range(4):
        im = load("native_scroll_%d_%d.png" % (g, i))
        print(g, i, im.size, bbox(im, CREAM))

print("\n=== buttons (opaque bbox / top colors) ===")
for name in ("native_BtCheck_normal_0.png", "native_BtLeft_normal_0.png",
             "native_BtRight_normal_0.png", "native_BtYes_normal_0.png",
             "native_BtNo_normal_0.png"):
    im = load(name)
    print(name, im.size, bbox(im, OPAQUE), top_colors(im, 3))

print("\n=== statTb / charAlert / charName ===")
for name in ("native_statTb.png", "native_charAlert.png", "native_charName.png"):
    im = load(name)
    print(name, im.size, bbox(im, OPAQUE), top_colors(im, 3))
