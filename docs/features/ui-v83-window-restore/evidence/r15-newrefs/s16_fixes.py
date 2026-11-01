# -*- coding: utf-8 -*-
"""s16_fixes.py -- diagnostics + corrected measurements for the gaps left by s15."""
import io, os, json
from collections import Counter, deque
from PIL import Image

D = os.path.dirname(os.path.abspath(__file__))
IDX = json.load(io.open(os.path.join(D, "_s0_paths.json"), encoding="utf-8"))

out = io.open(os.path.join(D, "_s16_fixes.txt"), "w", encoding="utf-8")
def sz(bb):
    return ("%dx%d" % (bb[2] - bb[0] + 1, bb[3] - bb[1] + 1)) if bb else "-"

def L(s=""):
    out.write(s + "\n")

def load(k):
    e = IDX[k]
    p = e["path"] if isinstance(e, dict) else e[0]
    im = Image.open(p).convert("RGBA")
    return im, im.load(), im.size

def bbox_where(px, W, H, pred, x0=0, y0=0, x1=None, y1=None):
    x1 = W if x1 is None else x1
    y1 = H if y1 is None else y1
    bx0 = by0 = 10 ** 9
    bx1 = by1 = -1
    n = 0
    for y in range(y0, y1):
        for x in range(x0, x1):
            if pred(px[x, y]):
                n += 1
                if x < bx0: bx0 = x
                if x > bx1: bx1 = x
                if y < by0: by0 = y
                if y > by1: by1 = y
    if n == 0:
        return None, 0
    return (bx0, by0, bx1, by1), n

def cc(px, W, H, seed, pred):
    """connected component bbox from seed"""
    sx, sy = seed
    if not pred(px[sx, sy]):
        return None, 0
    seen = {(sx, sy)}
    q = deque([(sx, sy)])
    bx0 = bx1 = sx
    by0 = by1 = sy
    n = 0
    while q:
        x, y = q.popleft()
        n += 1
        if x < bx0: bx0 = x
        if x > bx1: bx1 = x
        if y < by0: by0 = y
        if y > by1: by1 = y
        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            a, b = x + dx, y + dy
            if 0 <= a < W and 0 <= b < H and (a, b) not in seen and pred(px[a, b]):
                seen.add((a, b))
                q.append((a, b))
    return (bx0, by0, bx1, by1), n

# ------------------------------------------------------------------ 0. diag
L("=" * 78)
L("PART 0 -- diagnostic: why did 'min(rgb)>=235' find 0 white px in img1 bands 3+?")
L("=" * 78)
im1, p1, (W1, H1) = load("img1")
for (y0, y1) in ((22, 46), (48, 72), (74, 98), (126, 150)):
    c = Counter()
    mx = 0
    for y in range(y0, y1 + 1):
        for x in range(6, 91):
            p = p1[x, y][:3]
            c[p] += 1
            mx = max(mx, min(p))
    L("  img1 y=%d..%d  max(min(rgb))=%d  top colours: %s" % (
        y0, y1, mx, ", ".join("%s x%d" % (str(k), v) for k, v in c.most_common(6))))
im2, p2, (W2, H2) = load("img2")
for (y0, y1) in ((25, 49), (51, 75)):
    c = Counter()
    mx = 0
    for y in range(y0, y1 + 1):
        for x in range(3, 82):
            p = p2[x, y][:3]
            c[p] += 1
            mx = max(mx, min(p))
    L("  img2 y=%d..%d  max(min(rgb))=%d  top colours: %s" % (
        y0, y1, mx, ", ".join("%s x%d" % (str(k), v) for k, v in c.most_common(6))))

LIGHT = lambda p: p[0] >= 190 and p[1] >= 190 and p[2] >= 190
L("")
L("  re-scan with light predicate (r,g,b all >=190):")
for k, px, (W, H), bands, xlo, xhi in (
        ("img1", p1, (W1, H1), [(3, 16)] + [(22 + 26 * i, 46 + 26 * i) for i in range(8)], 6, 91),
        ("img2", p2, (W2, H2), [(6, 19)] + [(25 + 26 * i, 49 + 26 * i) for i in range(4)], 3, 82)):
    for i, (y0, y1) in enumerate(bands):
        bb, n = bbox_where(px, W, H, LIGHT, xlo, y0, xhi, y1 + 1)
        L("    %s band %d y=%3d..%3d  light-ink n=%5d bbox=%s" % (k, i + 1, y0, y1, n, bb))

L("")
L("  per-scanline light-ink counts (img1 y=0..240):")
for y in range(0, 241):
    bb, n = bbox_where(p1, W1, H1, LIGHT, 6, y, 91, y + 1)
    L("    y=%3d n=%3d %s" % (y, n, bb if bb else ""))

# ------------------------------------------------------------------ 1. img3
L("")
L("=" * 78)
L("PART 1 -- img3 corrected feature bboxes (tight predicates + region + CC)")
L("=" * 78)
im3, p3, (W3, H3) = load("img3")

YG = lambda p: p[1] > p[2] + 40 and p[0] > p[2] + 20 and p[1] > 110 and p[2] < 120
bb, n = bbox_where(p3, W3, H3, YG, 0, 283, W3, H3)
L("  1a. yellow-green button fill  bbox=%s n=%d size=%s" % (bb, n, sz(bb)))
bb, n = cc(p3, W3, H3, (50, 300), lambda p: p[1] > p[2] + 30 and p[1] > 90 and p[2] < 130)
L("      CC from (50,300)          bbox=%s n=%d size=%s" % (bb, n, sz(bb)))
# include the black outline: expand until we hit pure background
bb2, n2 = bbox_where(p3, W3, H3, lambda p: not (p[2] > p[0] + 25 and p[2] > p[1] + 5), 0, 288, 140, H3)
L("      non-bg in x0..140 y288..  bbox=%s n=%d" % (bb2, n2))

BLUE = lambda p: p[2] > 150 and p[2] > p[0] + 50 and p[1] > p[0] and p[1] < 200
bb, n = bbox_where(p3, W3, H3, BLUE, 400, 225, 519, 282)
L("  1b. blue 'next' button fill   bbox=%s n=%d size=%s" % (bb, n, sz(bb)))
bb, n = cc(p3, W3, H3, (470, 250), BLUE)
L("      CC from (470,250)         bbox=%s n=%d size=%s" % (bb, n, sz(bb)))
bb, n = bbox_where(p3, W3, H3, lambda p: p[0]+p[1]+p[2] < 200, 430, 225, 519, 282)
L("      dark outline near it      bbox=%s n=%d" % (bb, n))

NAVY = lambda p: 40 <= p[2] <= 110 and p[2] > p[0] + 25 and p[0] < 70 and abs(p[0]-p[1]) <= 12
bb, n = bbox_where(p3, W3, H3, NAVY, 0, 140, 200, 230)
L("  1c. nametag navy fill         bbox=%s n=%d size=%s" % (bb, n, sz(bb)))
bb, n = cc(p3, W3, H3, (80, 178), NAVY)
L("      CC from (80,178)          bbox=%s n=%d size=%s" % (bb, n, sz(bb)))
bb, n = bbox_where(p3, W3, H3, LIGHT, 0, 140, 200, 230)
L("      nametag white border+text bbox=%s n=%d" % (bb, n))

L("")
L("  1d. white text panel (neutral, all >=245)")
bb, n = bbox_where(p3, W3, H3, lambda p: min(p[:3]) >= 245, 120, 10, 519, 285)
L("      bbox=%s n=%d size=%s" % (bb, n, sz(bb)))
colw = [x for x in range(120, 519) if sum(1 for y in range(10, 285) if min(p3[x, y][:3]) >= 245) > 60]
roww = [y for y in range(10, 285) if sum(1 for x in range(120, 519) if min(p3[x, y][:3]) >= 245) > 150]
if colw:
    L("      cols with >60 white px : %d..%d  (%d cols)" % (colw[0], colw[-1], len(colw)))
else:
    L("      cols with >60 white px : NONE")
if roww:
    L("      rows with >150 white px : %d..%d  (%d rows)" % (roww[0], roww[-1], len(roww)))
else:
    L("      rows with >150 white px : NONE")

L("")
L("  1e. avatar (speaker) sprite bbox, green-ish, x<160, y 90..200")
bb, n = bbox_where(p3, W3, H3, lambda p: p[1] > p[0] + 15 and p[1] > p[2] + 15, 0, 90, 160, 205)
L("      bbox=%s n=%d size=%s" % (bb, n, sz(bb)))

L("")
L("  1f. window frame rect candidates")
L("      inner black outline rows (>=300 dark px): %s" % [y for y in range(H3) if sum(1 for x in range(W3) if p3[x, y][0]+p3[x, y][1]+p3[x, y][2] < 120) >= 300])
L("      inner black outline cols (>=150 dark px): %s" % [x for x in range(W3) if sum(1 for y in range(H3) if p3[x, y][0]+p3[x, y][1]+p3[x, y][2] < 120) >= 150])
L("      white(>=235) rows >=300: %s" % [y for y in range(H3) if sum(1 for x in range(W3) if min(p3[x, y][:3]) >= 235) >= 300][:6])
L("      white(>=235) cols >=200: %s" % [x for x in range(W3) if sum(1 for y in range(H3) if min(p3[x, y][:3]) >= 235) >= 200][:8])
NBG = lambda p: not (p[2] > p[0] + 20 and p[2] > p[1] + 5 and p[2] < 200)
bb, n = bbox_where(p3, W3, H3, NBG, 0, 0, W3, 292)
L("      'not game-blue-bg' bbox over y<292 = %s n=%d" % (bb, n))

# ------------------------------------------------------------------ 2. img4
L("")
L("=" * 78)
L("PART 2 -- img4 frame rect and red toggle button")
L("=" * 78)
im4, p4, (W4, H4) = load("img4")
L("  per-col white(>=235) counts, x=40..60 : %s" % [(x, sum(1 for y in range(H4) if min(p4[x, y][:3]) >= 235)) for x in range(40, 61)])
L("  per-col white(>=235) counts, x=498..508: %s" % [(x, sum(1 for y in range(H4) if min(p4[x, y][:3]) >= 235)) for x in range(498, 509)])
L("  per-col dark(<120) counts,   x=40..60 : %s" % [(x, sum(1 for y in range(H4) if sum(p4[x, y][:3]) < 120)) for x in range(40, 61)])
L("  per-col dark(<120) counts,   x=498..508: %s" % [(x, sum(1 for y in range(H4) if sum(p4[x, y][:3]) < 120)) for x in range(498, 509)])
L("  per-row white counts, y=4..20 (x restricted 100..500): %s" % [(y, sum(1 for x in range(100, 500) if min(p4[x, y][:3]) >= 235)) for y in range(4, 21)])
L("  per-row dark counts,  y=4..20 (x restricted 100..500): %s" % [(y, sum(1 for x in range(100, 500) if sum(p4[x, y][:3]) < 120)) for y in range(4, 21)])
L("  per-row white counts, y=76..85: %s" % [(y, sum(1 for x in range(100, 500) if min(p4[x, y][:3]) >= 235)) for y in range(76, H4)])
L("  per-row dark counts,  y=76..85: %s" % [(y, sum(1 for x in range(100, 500) if sum(p4[x, y][:3]) < 120)) for y in range(76, H4)])

RED = lambda p: p[0] > 150 and p[1] < 90 and p[2] < 110
bb, n = cc(p4, W4, H4, (10, 30), RED)
L("  red toggle btn CC from (10,30) bbox=%s n=%d size=%s" % (bb, n, sz(bb)))
bb, n = bbox_where(p4, W4, H4, RED, 0, 0, 52, H4)
L("  red any, x<52             bbox=%s n=%d" % (bb, n))
for yy in range(18, 52, 2):
    xs = [x for x in range(0, 52) if RED(p4[x, yy])]
    L("    y=%2d red xs=%s" % (yy, xs))

TIP = lambda p: p[2] > p[0] + 20 and p[2] > p[1] + 20 and p[0] <= 90 and p[2] <= 145 and abs(p[0] - p[1]) <= 12
bb, n = cc(p4, W4, H4, (45, 30), TIP)
L("  img4 tooltip CC from (45,30) bbox=%s n=%d size=%s" % (bb, n, sz(bb)))
bb, n = bbox_where(p4, W4, H4, TIP, 20, 0, 120, 60)
L("  img4 tooltip bbox x20..120 y0..60 = %s n=%d" % (bb, n))
im4.crop((bb[0]-2, bb[1]-2, bb[2]+3, bb[3]+3)).resize(((bb[2]-bb[0]+5)*6, (bb[3]-bb[1]+5)*6), Image.NEAREST).save(
    os.path.join(D, "crop_img4_tooltip_6x.png"))
L("  wrote crop_img4_tooltip_6x.png box=%s" % (bb,))

L("")
L("  keycap-vs-name vertical metrics (pink slots only)")
LEFTS = [52, 87, 122, 157, 192, 227, 262, 297, 332, 367, 402, 437, 472]
RIGHTS = LEFTS[1:] + [505]
BG = lambda p: p[0] > 150 and p[1] < 190 and p[2] > 130 and p[2] < 200 and p[0] > p[1] + 15
for row_y in ((15, 45), (49, 78)):
    for i in range(13):
        x0, x1 = LEFTS[i] + 2, RIGHTS[i] - 1
        kb, kn = bbox_where(p4, W4, H4, lambda p: p[2] > p[0] + 12 and 110 < p[0] and max(p[:3]) < 235, x0, row_y[0], x1, row_y[1] + 1)
        pb, pn = bbox_where(p4, W4, H4, BG, x0, row_y[0], x1, row_y[1] + 1)
        if pn < 100:
            continue
        # white name text = white pixels BELOW the keycap
        wb, wn = bbox_where(p4, W4, H4, lambda p: min(p[:3]) >= 235, x0, (kb[3] + 1) if kb else row_y[0], x1, row_y[1] + 1)
        L("   r%d c%-2d slot x=%3d..%3d y=%2d..%2d | keycap y=%s | name-white y=%s | gap=%s" % (
            1 if row_y[0] == 15 else 2, i + 1, LEFTS[i], RIGHTS[i] - 1, row_y[0], row_y[1],
            (kb[1], kb[3]) if kb else None, (wb[1], wb[3]) if wb else None,
            (wb[1] - kb[3]) if (wb and kb) else None))

# ------------------------------------------------------------------ 3. img5
L("")
L("=" * 78)
L("PART 3 -- img5 corrected")
L("=" * 78)
im5, p5, (W5, H5) = load("img5")
bb, n = cc(p5, W5, H5, (60, 14), TIP)
L("  tooltip CC from (60,14)   bbox=%s n=%d size=%s" % (bb, n, sz(bb)))
if bb:
    im5.crop((bb[0]-2, bb[1]-2, bb[2]+3, bb[3]+3)).resize(((bb[2]-bb[0]+5)*6, (bb[3]-bb[1]+5)*6), Image.NEAREST).save(
        os.path.join(D, "crop_img5_tooltip_6x.png"))
    L("  wrote crop_img5_tooltip_6x.png box=%s" % (bb,))
    bb2, n2 = bbox_where(p5, W5, H5, lambda p: min(p[:3]) >= 235, bb[0], bb[1], bb[2]+1, bb[3]+1)
    L("  tooltip white text ink bbox=%s n=%d" % (bb2, n2))
bb, n = cc(p5, W5, H5, (10, 14), RED)
L("  red toggle btn CC (10,14) bbox=%s n=%d size=%s" % (bb, n, sz(bb)))
bb, n = cc(p5, W5, H5, (10, 45), lambda p: p[2] > 120 and p[2] > p[0] + 40)
L("  blue bottom btn CC (10,45) bbox=%s n=%d size=%s" % (bb, n, sz(bb)))
bb, n = bbox_where(p5, W5, H5, lambda p: min(p[:3]) >= 200, 0, 0, 60, 8)
L("  light strip y<8 x<60      bbox=%s n=%d" % (bb, n))
L("  row profile x=0..60 y=0..30 (light>=200 / red / navy counts)")
for y in range(0, 31):
    li = sum(1 for x in range(0, 60) if min(p5[x, y][:3]) >= 200)
    rd = sum(1 for x in range(0, 60) if RED(p5[x, y]))
    nv = sum(1 for x in range(0, 60) if TIP(p5[x, y]))
    L("    y=%2d light=%3d red=%3d navy=%3d" % (y, li, rd, nv))

out.close()
print("done")
