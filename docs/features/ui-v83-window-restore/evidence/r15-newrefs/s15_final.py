# -*- coding: utf-8 -*-
"""s15_final.py -- remaining forensic measurements (img1..img5).
Writes UTF-8 report to _s15_final.txt.  Nothing is printed to stdout except 'done'.
"""
import io, os, json
from collections import Counter
from PIL import Image

D = os.path.dirname(os.path.abspath(__file__))
IDX = json.load(io.open(os.path.join(D, "_s0_paths.json"), encoding="utf-8"))
KEYS = ["img1", "img2", "img3", "img4", "img5"]

out = io.open(os.path.join(D, "_s15_final.txt"), "w", encoding="utf-8")
def L(s=""):
    out.write(s + "\n")

def load(k):
    e = IDX[k]
    p = e["path"] if isinstance(e, dict) else (e[0] if isinstance(e, list) else e)
    im = Image.open(p).convert("RGBA")
    return im, im.load(), im.size

def modal(im, px, box, q=8):
    c = Counter()
    for y in range(box[1], box[3]):
        for x in range(box[0], box[2]):
            p = px[x, y]
            c[(p[0] // q * q, p[1] // q * q, p[2] // q * q)] += 1
    return c.most_common(12)

def bbox_where(im, px, W, H, pred, x0=0, y0=0, x1=None, y1=None):
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

def dark(p, s=150):
    return p[0] + p[1] + p[2] < s

def white(p, s=235):
    return min(p[0], p[1], p[2]) >= s

# ---------------------------------------------------------------- img1 / img2
L("=" * 78)
L("PART A -- img1 (94x278) and img2 (82x182) : per-row text ink extents")
L("=" * 78)
for k in KEYS:
    e = IDX[k]
    L("  %s -> %s" % (k, e["path"] if isinstance(e, dict) else e[0]))
for k, bands, xlo, xhi in (
        ("img1", [(3, 16)] + [(22 + 26 * i, 46 + 26 * i) for i in range(8)], 6, 91),
        ("img2", [(6, 19)] + [(25 + 26 * i, 49 + 26 * i) for i in range(4)], 2, 82)):
    im, px, (W, H) = load(k)
    L("")
    L("--- %s  %dx%d %s ---" % (k, W, H, im.mode))
    for i, (y0, y1) in enumerate(bands):
        # white text ink (menu body) and black text ink (title bar)
        res = {}
        for nm, pr in (("white", lambda p: min(p[:3]) >= 235),
                       ("black", lambda p: p[0] + p[1] + p[2] < 160)):
            bb, n = bbox_where(im, px, W, H, pr, xlo, y0, xhi, y1 + 1)
            res[nm] = (bb, n)
        bb, n = res["white"]
        L("  band %d y=%3d..%3d  white-ink n=%5d  bbox=%s" % (i + 1, y0, y1, n, bb))
        bb2, n2 = res["black"]
        L("                    black-ink n=%5d  bbox=%s" % (n2, bb2))

# ---------------------------------------------------------------- img3
L("")
L("=" * 78)
L("PART B -- img3 (524x313) : window frame rect, buttons, nametag, scrollbar")
L("=" * 78)
im3, p3, (W3, H3) = load("img3")

L("")
L("--- B1. outer frame rect: per-row / per-col counts of black(sum<120) and white(min>=235)")
rowblk = []
rowwht = []
for y in range(H3):
    b = w = 0
    for x in range(W3):
        p = p3[x, y]
        if p[0] + p[1] + p[2] < 120: b += 1
        if min(p[:3]) >= 235: w += 1
    rowblk.append(b)
    rowwht.append(w)
colblk = []
colwht = []
for x in range(W3):
    b = w = 0
    for y in range(H3):
        p = p3[x, y]
        if p[0] + p[1] + p[2] < 120: b += 1
        if min(p[:3]) >= 235: w += 1
    colblk.append(b)
    colwht.append(w)
L("  rows with black-run >= 300 px : %s" % [y for y in range(H3) if rowblk[y] >= 300])
L("  rows with white-run >= 300 px : %s" % [y for y in range(H3) if rowwht[y] >= 300])
L("  cols with black-run >= 150 px : %s" % [x for x in range(W3) if colblk[x] >= 150])
L("  cols with white-run >= 150 px : %s" % [x for x in range(W3) if colwht[x] >= 150])
L("  (thresholds chosen because the frame is ~287 px tall / ~521 px wide)")

L("")
L("--- B2. exact border probe: horizontal runs at y=140 (mid-window)")
runs = []
cur = None
for x in range(0, 12):
    p = p3[x, 140]
    if cur and cur[2] == p:
        cur[1] = x
    else:
        if cur: runs.append(cur)
        cur = [x, x, p]
runs.append(cur)
L("  x=0..11 : " + " | ".join("[%d..%d]%s" % (r[0], r[1], r[2][:3]) for r in runs))
runs = []
cur = None
for x in range(512, 524):
    p = p3[x, 140]
    if cur and cur[2] == p:
        cur[1] = x
    else:
        if cur: runs.append(cur)
        cur = [x, x, p]
runs.append(cur)
L("  x=512..523: " + " | ".join("[%d..%d]%s" % (r[0], r[1], r[2][:3]) for r in runs))
L("--- vertical runs at x=260 (mid-window)")
runs = []
cur = None
for y in range(0, 12):
    p = p3[260, y]
    if cur and cur[2] == p:
        cur[1] = y
    else:
        if cur: runs.append(cur)
        cur = [y, y, p]
runs.append(cur)
L("  y=0..11 : " + " | ".join("[%d..%d]%s" % (r[0], r[1], r[2][:3]) for r in runs))
runs = []
cur = None
for y in range(278, 292):
    p = p3[260, y]
    if cur and cur[2] == p:
        cur[1] = y
    else:
        if cur: runs.append(cur)
        cur = [y, y, p]
runs.append(cur)
L("  y=278..291: " + " | ".join("[%d..%d]%s" % (r[0], r[1], r[2][:3]) for r in runs))

L("")
L("--- B3. feature bboxes (relative to window origin (2,1) as well)")
feats = [
    ("yellow-green close btn", lambda p: p[1] > p[0] - 40 and p[1] > p[2] + 60 and p[1] > 110 and p[2] < 90),
    ("blue next btn", lambda p: p[2] > p[0] + 40 and p[2] > 150 and p[0] < 130),
    ("nametag navy", lambda p: p[2] > p[0] + 30 and p[2] > 60 and p[2] < 130 and p[0] < 80 and p[1] < 90),
]
for nm, pr in feats:
    bb, n = bbox_where(im3, p3, W3, H3, pr)
    L("  %-22s abs=%s n=%d size=%s rel=%s" % (
        nm, bb, n,
        ("%dx%d" % (bb[2] - bb[0] + 1, bb[3] - bb[1] + 1)) if bb else "-",
        ("(%d,%d)" % (bb[0] - 2, bb[1] - 1)) if bb else "-"))

L("")
L("--- B4. close-button hunt in the title strip (UtilDlgEx has no title bar here)")
L("    top band y=1..20, x=470..523 : colour histogram")
for c, n in modal(im3, p3, (470, 1, 524, 22), q=16)[:8]:
    L("      %s x%d" % (str(c), n))
L("    top band y=1..20, x=4..60 : colour histogram")
for c, n in modal(im3, p3, (4, 1, 60, 22), q=16)[:8]:
    L("      %s x%d" % (str(c), n))

L("")
L("--- B5. scroll-bar hunt: any non-beige vertical furniture in x=500..518, y=25..270")
cnt = Counter()
for y in range(25, 271):
    for x in range(500, 519):
        cnt[p3[x, y][:3]] += 1
for c, n in cnt.most_common(10):
    L("      %s x%d" % (str(c), n))
bb, n = bbox_where(im3, p3, W3, H3, lambda p: p[0] < 130 and p[1] < 130 and p[2] < 170 and abs(p[0] - p[2]) < 60,
                   500, 25, 519, 271)
L("      'dark-ish slider-like' bbox in x500..518 y25..270 = %s n=%d" % (bb, n))

L("")
L("--- B6. text-area bright rect (the white inner panel)")
bb, n = bbox_where(im3, p3, W3, H3, lambda p: min(p[:3]) >= 225, 100, 10, 512, 282)
L("      bright panel bbox = %s n=%d size=%s" % (bb, n, "%dx%d" % (bb[2] - bb[0] + 1, bb[3] - bb[1] + 1)))
L("      left avatar beige panel: histogram x=12..150 y=200..260")
for c, n2 in modal(im3, p3, (12, 200, 150, 260), q=16)[:6]:
    L("        %s x%d" % (str(c), n2))

L("")
L("--- B7. per-text-band ink extents, x from 152 to 512")
BANDS3 = [(37, 48), (55, 66), (73, 84), (91, 102), (109, 120),
          (127, 138), (145, 156), (163, 174),
          (169, 180), (187, 198), (205, 216), (223, 234)]
for (y0, y1) in BANDS3:
    bb, n = bbox_where(im3, p3, W3, H3,
                       lambda p: (p[0] + p[1] + p[2] < 300) or (p[0] > 150 and p[1] < 90 and p[2] < 90),
                       152, y0, 512, y1 + 1)
    L("      y=%3d..%3d rel_y=%3d..%3d  ink n=%5d bbox=%s" % (
        y0, y1, y0 - 1, y1 - 1, n, bb))

L("")
L("--- B8. 8x zoom of the ambiguous glyph in text band 2 (y=52..70, x=163..235)")
im3.crop((163, 52, 235, 70)).resize((72 * 8, 18 * 8), Image.NEAREST).save(
    os.path.join(D, "crop_img3_glyph_8x.png"))
im3.crop((163, 70, 240, 88)).resize((77 * 8, 18 * 8), Image.NEAREST).save(
    os.path.join(D, "crop_img3_glyph_b_8x.png"))

# ---------------------------------------------------------------- img4
L("")
L("=" * 78)
L("PART C -- img4 (509x86) : shortcut window frame, slot grid, keycap vs label")
L("=" * 78)
im4, p4, (W4, H4) = load("img4")
L("")
L("--- C1. frame edges")
L("  vertical black runs at y=43 (between the two slot rows):")
runs = []
cur = None
for x in range(W4):
    p = p4[x, 43]
    d = dark(p, 140)
    if cur and cur[2] == d:
        cur[1] = x
    else:
        if cur: runs.append(cur)
        cur = [x, x, d]
runs.append(cur)
L("    " + " ".join("%s[%d..%d]" % ("B" if r[2] else ".", r[0], r[1]) for r in runs if r[1] - r[0] >= 0 and (r[2] or r[1] - r[0] > 2)))
for yy in (43, 44, 45, 80, 81, 82, 83):
    xs = [x for x in range(W4) if dark(p4[x, yy], 140)]
    L("  y=%2d dark x-min=%s x-max=%s count=%d" % (yy, xs[0] if xs else None, xs[-1] if xs else None, len(xs)))
for xx in (46, 47, 48, 49, 50, 51, 506, 507, 508):
    ys = [y for y in range(H4) if dark(p4[xx, y], 140)]
    L("  x=%3d dark y-min=%s y-max=%s count=%d" % (xx, ys[0] if ys else None, ys[-1] if ys else None, len(ys)))
for yy in (6, 7, 8, 9, 10, 84, 85):
    xs = [x for x in range(W4) if dark(p4[x, yy], 140)]
    L("  y=%2d dark count=%d first=%s last=%s" % (yy, len(xs), xs[0] if xs else None, xs[-1] if xs else None))

L("")
L("--- C2. slot grid geometry (black separator columns, measured at y=66)")
seps = []
inrun = False
for x in range(50, W4):
    d = dark(p4[x, 66], 150)
    if d and not inrun:
        s = x
        inrun = True
    elif not d and inrun:
        seps.append((s, x - 1, (s + x - 1) / 2.0))
        inrun = False
L("  separator runs at y=66: %s" % seps)
L("  separator runs at y=30: %s" % [(s, e) for (s, e) in
   [ (x, x) for x in range(50, W4) if dark(p4[x, 30], 150)]])

L("")
L("--- C3. keycap glyph (blue-grey) vs command name (white) inside each slot")
LEFTS = [52, 87, 122, 157, 192, 227, 262, 297, 332, 367, 402, 437, 472]
RIGHTS = LEFTS[1:] + [505]
def bluegrey(p):
    r, g, b = p[:3]
    return b > r + 12 and b > 120 and r > 110 and max(p[:3]) < 235
def pinkbg(p):
    r, g, b = p[:3]
    return r > g + 20 and r > 120 and b > g + 5 and b < r
for row_y in ((15, 45), (49, 78)):
    L("  -- slot row y=%d..%d --" % row_y)
    for i in range(13):
        x0, x1 = LEFTS[i] + 2, RIGHTS[i] - 1
        kb, kn = bbox_where(im4, p4, W4, H4, bluegrey, x0, row_y[0], x1, row_y[1] + 1)
        wb, wn = bbox_where(im4, p4, W4, H4, white, x0, row_y[0], x1, row_y[1] + 1)
        pb, pn = bbox_where(im4, p4, W4, H4, pinkbg, x0, row_y[0], x1, row_y[1] + 1)
        L("   c%-2d x=%3d..%3d keycap=%s n=%-4d whitetext=%s n=%-4d pinkfill=%s n=%d" % (
            i + 1, x0, x1, kb, kn, wb, wn, pb, pn))

L("")
L("--- C4. neighbouring UI in img4 (outside the window frame rect)")
bb, n = bbox_where(im4, p4, W4, H4, lambda p: p[0] > 150 and p[1] < 80 and p[2] < 80, 0, 0, 52, H4)
L("  red toggle button  bbox=%s n=%d size=%s" % (bb, n, "%dx%d" % (bb[2] - bb[0] + 1, bb[3] - bb[1] + 1)))
L("  tooltip region histogram x=27..62 y=22..42")
for c, n2 in modal(im4, p4, (27, 22, 62, 42), q=16)[:6]:
    L("      %s x%d" % (str(c), n2))
tipcol = modal(im4, p4, (27, 22, 62, 42), q=1)[0][0]
bb, n = bbox_where(im4, p4, W4, H4,
                   lambda p: abs(p[0] - tipcol[0]) <= 10 and abs(p[1] - tipcol[1]) <= 10 and abs(p[2] - tipcol[2]) <= 10,
                   0, 0, 509, 86)
L("  tooltip bg modal=%s -> bbox=%s n=%d size=%s" % (
    str(tipcol), bb, n, "%dx%d" % (bb[2] - bb[0] + 1, bb[3] - bb[1] + 1)))

# ---------------------------------------------------------------- img5
L("")
L("=" * 78)
L("PART D -- img5 (234x67) : what is actually in this crop")
L("=" * 78)
im5, p5, (W5, H5) = load("img5")
L("")
L("--- D1. whole-image colour histogram (top 20)")
cnt = Counter()
for y in range(H5):
    for x in range(W5):
        cnt[p5[x, y][:3]] += 1
for c, n in cnt.most_common(20):
    L("      %s x%d" % (str(c), n))
L("--- D2. histogram restricted to x=25..95, y=2..26 (tooltip guess)")
for c, n in modal(im5, p5, (25, 2, 95, 26), q=8)[:10]:
    L("      %s x%d" % (str(c), n))
L("--- D3. horizontal run map at y=14 and y=18, x=0..110")
for yy in (14, 18):
    runs = []
    cur = None
    for x in range(0, 111):
        p = p5[x, yy][:3]
        if cur and cur[2] == p:
            cur[1] = x
        else:
            if cur: runs.append(cur)
            cur = [x, x, p]
    runs.append(cur)
    L("  y=%d : %s" % (yy, " | ".join("[%d..%d]%s" % (r[0], r[1], str(r[2])) for r in runs)))
L("--- D4. vertical run map at x=60 and x=10, y=0..66")
for xx in (10, 60):
    runs = []
    cur = None
    for y in range(0, H5):
        p = p5[xx, y][:3]
        if cur and cur[2] == p:
            cur[1] = y
        else:
            if cur: runs.append(cur)
            cur = [y, y, p]
    runs.append(cur)
    L("  x=%d : %s" % (xx, " | ".join("[%d..%d]%s" % (r[0], r[1], str(r[2])) for r in runs)))

tipcol = modal(im5, p5, (30, 6, 90, 22), q=4)[0][0]
bb, n = bbox_where(im5, p5, W5, H5,
                   lambda p: abs(p[0] - tipcol[0]) <= 12 and abs(p[1] - tipcol[1]) <= 12 and abs(p[2] - tipcol[2]) <= 12)
L("--- D5. tooltip bbox from modal %s = %s n=%d size=%s" % (
    str(tipcol), bb, n, "%dx%d" % (bb[2] - bb[0] + 1, bb[3] - bb[1] + 1)))
if bb:
    x0 = max(0, bb[0] - 3); y0 = max(0, bb[1] - 3)
    x1 = min(W5, bb[2] + 4); y1 = min(H5, bb[3] + 4)
    im5.crop((x0, y0, x1, y1)).resize(((x1 - x0) * 6, (y1 - y0) * 6), Image.NEAREST).save(
        os.path.join(D, "crop_img5_tooltip_6x.png"))
    L("      wrote crop_img5_tooltip_6x.png box=%s" % ((x0, y0, x1, y1),))
bb, n = bbox_where(im5, p5, W5, H5, lambda p: p[0] > 150 and p[1] < 80 and p[2] < 80)
L("--- D6. red toggle button bbox = %s n=%d size=%s" % (bb, n, "%dx%d" % (bb[2] - bb[0] + 1, bb[3] - bb[1] + 1)))
bb, n = bbox_where(im5, p5, W5, H5, lambda p: p[2] > 120 and p[2] > p[0] + 40 and p[1] < 160)
L("--- D7. blue bottom-bar button bbox = %s n=%d" % (bb, n))
bb, n = bbox_where(im5, p5, W5, H5, lambda p: min(p[:3]) >= 225, 0, 0, 60, 10)
L("--- D8. light strip top-left (possible collapsed bar) bbox = %s n=%d" % (bb, n))

out.close()
print("done")
