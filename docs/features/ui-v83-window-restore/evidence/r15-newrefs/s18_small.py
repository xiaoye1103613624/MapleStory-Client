# -*- coding: utf-8 -*-
"""s18_small.py -- last few targeted measurements."""
import io, os, json
from PIL import Image

D = os.path.dirname(os.path.abspath(__file__))
IDX = json.load(io.open(os.path.join(D, "_s0_paths.json"), encoding="utf-8"))
out = io.open(os.path.join(D, "_s18_small.txt"), "w", encoding="utf-8")
def L(s=""):
    out.write(s + "\n")
def load(k):
    e = IDX[k]
    p = e["path"] if isinstance(e, dict) else e[0]
    im = Image.open(p).convert("RGBA")
    return im, im.load(), im.size
def bbox_where(px, pred, x0, y0, x1, y1):
    bx0 = by0 = 10 ** 9; bx1 = by1 = -1; n = 0
    for y in range(y0, y1):
        for x in range(x0, x1):
            if pred(px[x, y]):
                n += 1
                bx0 = min(bx0, x); bx1 = max(bx1, x); by0 = min(by0, y); by1 = max(by1, y)
    return ((bx0, by0, bx1, by1) if n else None), n
BLACK = lambda p: p[0] + p[1] + p[2] < 120
WHITE = lambda p: min(p[:3]) >= 235
LIGHT = lambda p: p[0] >= 190 and p[1] >= 190 and p[2] >= 190

# ---- img1 title text ----
im, px, (W, H) = load("img1")
L("== img1 ==")
bb, n = bbox_where(px, BLACK, 10, 2, 87, 18)
L("  title '界面' black ink bbox (x 10..86, y 2..17) = %s n=%d" % (bb, n))
for i, (y0, y1) in enumerate([(22, 46), (48, 72), (74, 98), (100, 124), (126, 150), (152, 176), (178, 202), (204, 228)]):
    a, na = bbox_where(px, LIGHT, 10, y0, 88, y0 + 13)
    b, nb = bbox_where(px, LIGHT, 10, y0 + 13, 88, y1 + 1)
    L("  row %d y=%3d..%3d  CJK %s (n=%d)   EN %s (n=%d)" % (i + 1, y0, y1, a, na, b, nb))
    L("          CJK left x=%s   EN left x=%s" % (a[0] if a else None, b[0] if b else None))
# icon column
bb, n = bbox_where(px, LIGHT, 10, 22, 20, 231)
L("  icon column x=10..19 over y=22..230 : bbox=%s n=%d" % (bb, n))

# ---- img2 ----
im, px, (W, H) = load("img2")
L("== img2 ==")
bb, n = bbox_where(px, BLACK, 5, 5, 82, 21)
L("  title '主菜单' black ink bbox (x 5..81, y 5..20) = %s n=%d" % (bb, n))
for i, (y0, y1) in enumerate([(25, 49), (51, 75), (77, 101), (103, 127)]):
    a, na = bbox_where(px, LIGHT, 5, y0, W, y0 + 13)
    b, nb = bbox_where(px, LIGHT, 5, y0 + 13, W, y1 + 1)
    L("  row %d y=%3d..%3d  CJK %s (n=%d)   EN %s (n=%d)" % (i + 1, y0, y1, a, na, b, nb))
L("  right-edge scan: for y in (28,54,80,106) the last x with light text ink:")
for y in (28, 54, 80, 106):
    xs = [x for x in range(5, W) if LIGHT(px[x, y])]
    L("     y=%3d light xs last=%s  (image width-1 = %d)" % (y, xs[-1] if xs else None, W - 1))
L("  colours along y=30 x=76..81: %s" % [px[x, 30][:3] for x in range(76, 82)])
L("  colours along y=128 x=76..81: %s" % [px[x, 128][:3] for x in range(76, 82)])

# ---- img3 ----
im3, p3, (W3, H3) = load("img3")
L("== img3 ==")
# 下项 button: expand from the blue fill (452,243)-(493,256)
L("  black pixels in the box x 440..512, y 232..270 grouped per row:")
for y in range(232, 271):
    xs = [x for x in range(440, 513) if BLACK(p3[x, y])]
    if xs:
        L("     y=%3d n=%3d x=%d..%d" % (y, len(xs), xs[0], xs[-1]))
bb, n = bbox_where(p3, lambda p: BLACK(p) or p[2] > 150 and p[2] > p[0] + 50, 440, 232, 512, 270)
L("  下项 'outline+fill' bbox = %s n=%d" % (bb, n))
# 结束对话 button outline
L("  black pixels around the close button, x 0..140, y 288..313:")
for y in range(288, 313):
    xs = [x for x in range(0, 141) if BLACK(p3[x, y])]
    if xs:
        L("     y=%3d n=%3d x=%d..%d" % (y, len(xs), xs[0], xs[-1]))
# nametag white border, tight region
bb, n = bbox_where(p3, WHITE, 10, 160, 150, 200)
L("  nametag white border+text (x10..149 y160..199) bbox=%s n=%d" % (bb, n))
bb, n = bbox_where(p3, lambda p: p[2] > p[0] + 25 and p[2] > 60 and p[2] < 140 and p[0] < 80, 10, 160, 150, 200)
L("  nametag navy fill  (same window)        bbox=%s n=%d" % (bb, n))
# text panel edges
colw = [x for x in range(120, 519) if bbox_where(p3, WHITE, x, 10, x + 1, 285)[1] > 150]
L("  panel columns with >150 white px: %s..%s (n=%d)" % (colw[0], colw[-1], len(colw)) if colw else "  none")
roww = [y for y in range(10, 285) if bbox_where(p3, WHITE, 155, y, 505, y + 1)[1] > 250]
L("  panel rows with >250 white px (x155..504): %s..%s (n=%d)" % (roww[0], roww[-1], len(roww)) if roww else "  none")
# left beige panel boundary
L("  horizontal colour walk at y=250 x=0..170:")
prev = None; s = 0
for x in range(0, 171):
    c = p3[x, 250][:3]
    if prev is not None and c != prev:
        L("     [%3d..%3d] %s" % (s, x - 1, prev)); s = x
    prev = c
L("     [%3d..%3d] %s" % (s, 170, prev))
# text line 1 centring
bb, n = bbox_where(p3, lambda p: (p[0] + p[1] + p[2] < 300) or (p[0] > 150 and p[1] < 90 and p[2] < 90), 152, 34, 512, 52)
L("  line1 ink bbox=%s  centre=%.1f ; panel centre=%.1f" % (bb, (bb[0] + bb[2]) / 2.0, (151 + 506) / 2.0))
# link grid columns
L("  link rows ink clusters (x) per band:")
for (y0, y1) in ((169, 180), (187, 198), (205, 216), (223, 234)):
    xs = [x for x in range(152, 512) if any(BLACK(p3[x, y]) or (p3[x, y][0] > 150 and p3[x, y][1] < 90) for y in range(y0, y1 + 1))]
    groups = []
    if xs:
        a = b = xs[0]
        for v in xs[1:]:
            if v - b > 6:
                groups.append((a, b)); a = v
            b = v
        groups.append((a, b))
    L("     y=%3d..%3d %s" % (y0, y1, groups))

# ---- img4 separators, clean ----
im4, p4, (W4, H4) = load("img4")
L("== img4 ==")
cnt = [(x, sum(1 for y in range(8, 86) if BLACK(p4[x, y]))) for x in range(W4)]
groups = []
run = None
for x in range(W4):
    d = cnt[x][1] >= 40
    if d and run is None:
        run = x
    elif not d and run is not None:
        groups.append((run, x - 1, (run + x - 1) / 2.0, max(cnt[i][1] for i in range(run, x))))
        run = None
L("  black vertical lines with >=40 dark px over y=8..85:")
for g in groups:
    L("     x=%d..%d centre=%.1f count=%d" % g)
c = [g[2] for g in groups]
L("  centres: %s" % c)
L("  pitches: %s" % [round(c[i + 1] - c[i], 2) for i in range(len(c) - 1)])
L("  mean pitch = %.3f" % ((c[-1] - c[0]) / (len(c) - 1)))
L("  horizontal black lines with >=300 dark px over x=100..499:")
for y in range(H4):
    k = sum(1 for x in range(100, 500) if BLACK(p4[x, y]))
    if k >= 300:
        L("     y=%d count=%d" % (y, k))
L("  white horizontal lines with >=300 white px:")
for y in range(H4):
    k = sum(1 for x in range(100, 500) if WHITE(p4[x, y]))
    if k >= 300:
        L("     y=%d count=%d" % (y, k))
out.close()
print("done")
