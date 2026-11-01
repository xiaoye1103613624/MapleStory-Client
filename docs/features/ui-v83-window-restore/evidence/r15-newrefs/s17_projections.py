# -*- coding: utf-8 -*-
"""s17_projections.py -- final: ink projections (row + column) for all 5 images,
   corrected feature bboxes, and the remaining crops for transcription.
"""
import io, os, json
from collections import Counter, deque
from PIL import Image

D = os.path.dirname(os.path.abspath(__file__))
IDX = json.load(io.open(os.path.join(D, "_s0_paths.json"), encoding="utf-8"))
out = io.open(os.path.join(D, "_s17_projections.txt"), "w", encoding="utf-8")

def L(s=""):
    out.write(s + "\n")

def load(k):
    e = IDX[k]
    p = e["path"] if isinstance(e, dict) else e[0]
    im = Image.open(p).convert("RGBA")
    return im, im.load(), im.size

def sz(bb):
    return ("%dx%d" % (bb[2] - bb[0] + 1, bb[3] - bb[1] + 1)) if bb else "-"

def bbox_where(px, pred, x0, y0, x1, y1):
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
    return ((bx0, by0, bx1, by1) if n else None), n

def cc(px, W, H, seed, pred):
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
        bx0 = min(bx0, x); bx1 = max(bx1, x)
        by0 = min(by0, y); by1 = max(by1, y)
        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            a, b = x + dx, y + dy
            if 0 <= a < W and 0 <= b < H and (a, b) not in seen and pred(px[a, b]):
                seen.add((a, b))
                q.append((a, b))
    return (bx0, by0, bx1, by1), n

def hist(px, box, top=8, q=1):
    c = Counter()
    for y in range(box[1], box[3]):
        for x in range(box[0], box[2]):
            c[px[x, y][:3]] += 1
    return c.most_common(top)

def comp(vals, thr=0):
    """compress list of (idx,count) into runs of non-zero"""
    s = []
    i = 0
    while i < len(vals):
        if vals[i][1] > thr:
            j = i
            while j + 1 < len(vals) and vals[j + 1][1] > thr:
                j += 1
            s.append("%d-%d" % (vals[i][0], vals[j][0]))
            i = j + 1
        else:
            i += 1
    return " ".join(s) if s else "(none)"

# predicates ---------------------------------------------------------------
MENU_TEXT = lambda p: p[0] >= 100 and p[1] >= 165 and p[2] >= 200      # light text on blue popup
MENU_DARK = lambda p: p[0] + p[1] + p[2] < 170                        # black text / border
SCR_TEXT = lambda p: (p[0] + p[1] + p[2] < 300) or (p[0] > 150 and p[1] < 90 and p[2] < 90)
RED = lambda p: p[0] > 150 and p[1] < 90 and p[2] < 110
TIP = lambda p: p[2] > p[0] + 12 and p[2] > p[1] + 12 and p[0] <= 100 and p[2] <= 150 and abs(p[0] - p[1]) <= 16
WHITE = lambda p: min(p[:3]) >= 235
BLACK = lambda p: p[0] + p[1] + p[2] < 120

# ============================================================== img1
L("=" * 78)
L("IMAGE 1  (界面 popup)")
L("=" * 78)
im, px, (W, H) = load("img1")
L("size=%dx%d mode=%s  file=%s" % (W, H, im.mode, IDX["img1"][0] if isinstance(IDX["img1"], list) else IDX["img1"]["path"]))
L("")
L("-- row projection, ink = light text (r>=100,g>=165,b>=200), x scanned 6..90 --")
L("   non-zero y runs : " + comp([(y, bbox_where(px, MENU_TEXT, 6, y, 91, y + 1)[1]) for y in range(0, 241)]))
L("   per-row counts  :")
for y in range(0, 241):
    _, n = bbox_where(px, MENU_TEXT, 6, y, 91, y + 1)
    if n:
        L("      y=%3d %4d" % (y, n))
L("-- column projection, same predicate, y scanned 22..231 --")
cols = [(x, bbox_where(px, MENU_TEXT, x, 22, x + 1, 231)[1]) for x in range(0, W)]
L("   " + " ".join("%d:%d" % (a, b) for a, b in cols if b))
L("-- frame lines: rows/cols that are mostly BLACK or mostly WHITE --")
L("   rows with >=60 black px (x 6..90): %s" % [y for y in range(H) if bbox_where(px, MENU_DARK, 6, y, 91, y + 1)[1] >= 60])
L("   rows with >=60 white px          : %s" % [y for y in range(H) if bbox_where(px, WHITE, 6, y, 91, y + 1)[1] >= 60])
L("   cols with >=150 black px (y 0..233): %s" % [x for x in range(W) if bbox_where(px, MENU_DARK, x, 0, x + 1, 233)[1] >= 150])
L("   cols with >=150 white px          : %s" % [x for x in range(W) if bbox_where(px, WHITE, x, 0, x + 1, 233)[1] >= 150])
L("-- per text band: CJK line bbox and English line bbox --")
B1 = [(22, 46), (48, 72), (74, 98), (100, 124), (126, 150), (152, 176), (178, 202), (204, 228)]
for i, (y0, y1) in enumerate(B1):
    a, na = bbox_where(px, MENU_TEXT, 6, y0, 91, y0 + 13)
    b, nb = bbox_where(px, MENU_TEXT, 6, y0 + 13, 91, y1 + 1)
    L("   row %d band y=%3d..%3d : line1 %s (n=%d)  line2 %s (n=%d)" % (i + 1, y0, y1, a, na, b, nb))
L("-- background colours of row1 (hovered) vs row2 --")
L("   y=22..46 top: %s" % hist(px, (6, 22, 91, 47), 5))
L("   y=48..72 top: %s" % hist(px, (6, 48, 91, 73), 5))

# ============================================================== img2
L("")
L("=" * 78)
L("IMAGE 2  (目录 -> 主菜单 popup)")
L("=" * 78)
im, px, (W, H) = load("img2")
L("size=%dx%d mode=%s" % (W, H, im.mode))
L("-- row projection, light-text predicate, x 3..81 --")
L("   non-zero y runs : " + comp([(y, bbox_where(px, MENU_TEXT, 3, y, 82, y + 1)[1]) for y in range(0, 135)]))
for y in range(0, 135):
    _, n = bbox_where(px, MENU_TEXT, 3, y, 82, y + 1)
    if n:
        L("      y=%3d %4d" % (y, n))
L("-- column projection (y 25..128) --")
L("   " + " ".join("%d:%d" % (x, bbox_where(px, MENU_TEXT, x, 25, x + 1, 129)[1]) for x in range(W)
                  if bbox_where(px, MENU_TEXT, x, 25, x + 1, 129)[1]))
L("-- frame lines --")
L("   rows >=55 black px : %s" % [y for y in range(H) if bbox_where(px, MENU_DARK, 2, y, W, y + 1)[1] >= 55])
L("   rows >=55 white px : %s" % [y for y in range(H) if bbox_where(px, WHITE, 2, y, W, y + 1)[1] >= 55])
L("   cols >=90 black px : %s" % [x for x in range(W) if bbox_where(px, MENU_DARK, x, 3, x + 1, 130)[1] >= 90])
L("   cols >=90 white px : %s" % [x for x in range(W) if bbox_where(px, WHITE, x, 3, x + 1, 130)[1] >= 90])
L("-- right edge truncation test: is there ANY dark/white vertical border in x=70..81? --")
for x in range(70, W):
    L("      x=%2d black=%d white=%d" % (x, bbox_where(px, MENU_DARK, x, 3, x + 1, 130)[1],
                                        bbox_where(px, WHITE, x, 3, x + 1, 130)[1]))
L("-- per text band --")
B2 = [(25, 49), (51, 75), (77, 101), (103, 127)]
for i, (y0, y1) in enumerate(B2):
    a, na = bbox_where(px, MENU_TEXT, 3, y0, W, y0 + 13)
    b, nb = bbox_where(px, MENU_TEXT, 3, y0 + 13, W, y1 + 1)
    L("   row %d band y=%3d..%3d : line1 %s (n=%d)  line2 %s (n=%d)" % (i + 1, y0, y1, a, na, b, nb))

# ============================================================== img3
L("")
L("=" * 78)
L("IMAGE 3  (帮助 / NPC script window)")
L("=" * 78)
im3, p3, (W3, H3) = load("img3")
L("size=%dx%d mode=%s" % (W3, H3, im3.mode))
L("-- frame: rows/cols that are almost entirely black or white --")
L("   rows with >=400 black px : %s" % [y for y in range(H3) if bbox_where(p3, BLACK, 0, y, W3, y + 1)[1] >= 400])
L("   rows with >=400 white px : %s" % [y for y in range(H3) if bbox_where(p3, WHITE, 0, y, W3, y + 1)[1] >= 400])
L("   cols with >=200 black px : %s" % [x for x in range(W3) if bbox_where(p3, BLACK, x, 0, x + 1, H3)[1] >= 200])
L("   cols with >=200 white px : %s" % [x for x in range(W3) if bbox_where(p3, WHITE, x, 0, x + 1, H3)[1] >= 200])
L("   -> inner black outline rect = (7,6)-(519,279)  size 513x274")
L("   -> outermost non-background extent (see s15 B2 line probes) = (2,1)-(522,287) size 521x287")
L("-- white inner panel (min>=235) --")
colw = [x for x in range(120, 519) if bbox_where(p3, WHITE, x, 10, x + 1, 285)[1] > 120]
roww = [y for y in range(10, 285) if bbox_where(p3, WHITE, 120, y, 519, y + 1)[1] > 200]
L("   cols with >120 white px : %d..%d (n=%d)" % (colw[0], colw[-1], len(colw)) if colw else "   none")
L("   rows with >200 white px : %d..%d (n=%d)" % (roww[0], roww[-1], len(roww)) if roww else "   none")
L("-- text-area row projection, x 152..512, ink = dark or red --")
L("   non-zero y runs : " + comp([(y, bbox_where(p3, SCR_TEXT, 152, y, 512, y + 1)[1]) for y in range(14, 286)]))
for y in range(14, 286):
    _, n = bbox_where(p3, SCR_TEXT, 152, y, 512, y + 1)
    if n:
        L("      y=%3d %4d" % (y, n))
L("-- text-area column projection, y 20..280 --")
L("   " + comp([(x, bbox_where(p3, SCR_TEXT, x, 20, x + 1, 280)[1]) for x in range(152, 512)], thr=0))
L("-- feature bboxes --")
bb, n = cc(p3, W3, H3, (50, 300), lambda p: p[1] > p[2] + 30 and p[1] > 90 and p[2] < 130)
L("   结束对话 button (yellow-green fill) bbox=%s size=%s  n=%d" % (bb, sz(bb), n))
bb2, _ = bbox_where(p3, BLACK, 0, 288, 140, H3)
L("   结束对话 incl. black outline        bbox=%s size=%s" % (bb2, sz(bb2)))
bb, n = cc(p3, W3, H3, (470, 250), lambda p: p[2] > 150 and p[2] > p[0] + 50 and p[0] < 130 and p[1] < 200)
L("   下项 button (blue fill)             bbox=%s size=%s n=%d" % (bb, sz(bb), n))
bb2, _ = bbox_where(p3, BLACK, 440, 235, 512, 270)
L("   下项 black outline box              bbox=%s size=%s" % (bb2, sz(bb2)))
L("   nametag colour hist at (21..137,171..185): %s" % hist(p3, (25, 173, 130, 183), 6))
bb, n = bbox_where(p3, lambda p: p[2] > p[0] + 25 and p[2] > 60 and p[2] < 140 and p[0] < 80, 0, 140, 200, 230)
L("   nametag navy fill                   bbox=%s size=%s n=%d" % (bb, sz(bb), n))
bb2, _ = bbox_where(p3, WHITE, 0, 140, 200, 230)
L("   nametag white border+text           bbox=%s" % (bb2,))
bb, n = bbox_where(p3, lambda p: p[1] > p[0] + 15 and p[1] > p[2] + 15, 0, 90, 160, 205)
L("   avatar green sprite                 bbox=%s size=%s n=%d" % (bb, sz(bb), n))
L("-- scroll bar hunt: x 500..518, y 20..275, any pixel darker than the beige frame --")
bb, n = bbox_where(p3, lambda p: p[0] + p[1] + p[2] < 420, 500, 20, 519, 275)
L("   bbox=%s n=%d  (a 1px column = frame shading, NOT a slider)" % (bb, n))
L("   per-col counts x=500..518 : %s" % [(x, bbox_where(p3, lambda p: p[0] + p[1] + p[2] < 420, x, 20, x + 1, 275)[1]) for x in range(500, 519)])
L("-- close button hunt in the frame's top strip y=2..20 --")
_dbb, _dbn = bbox_where(p3, BLACK, 8, 2, 518, 21)
L("   dark-pixel bbox x 8..518 y 2..20 = %s n=%d" % (_dbb, _dbn))
L("   (only the 1px black outline row y=6 should appear; a button would add a blob)")
for y in range(2, 21):
    L("      y=%2d dark=%4d" % (y, bbox_where(p3, BLACK, 8, y, 518, y + 1)[1]))
L("-- crops --")
for box, nm, sc in (((21, 165, 140, 192), "crop_img3_nametag_4x.png", 4),
                    ((440, 235, 512, 270), "crop_img3_btn_next_4x.png", 4),
                    ((0, 288, 140, 313), "crop_img3_btn_close_4x.png", 4),
                    ((163, 52, 235, 70), "crop_img3_glyph_8x.png", 8),
                    ((163, 70, 240, 88), "crop_img3_glyph_b_8x.png", 8),
                    ((152, 232, 512, 262), "crop_img3_nextrow_4x.png", 4)):
    im3.crop(box).resize(((box[2] - box[0]) * sc, (box[3] - box[1]) * sc), Image.NEAREST).save(os.path.join(D, nm))
    L("   wrote %-34s box=%s scale=%dx" % (nm, box, sc))

# ============================================================== img4
L("")
L("=" * 78)
L("IMAGE 4  (快捷键 expanded)")
L("=" * 78)
im4, p4, (W4, H4) = load("img4")
L("size=%dx%d mode=%s" % (W4, H4, im4.mode))
L("-- frame lines (x restricted to 100..500 to skip the tooltip/button) --")
L("   rows with >=390 black px : %s" % [y for y in range(H4) if bbox_where(p4, BLACK, 100, y, 500, y + 1)[1] >= 390])
L("   rows with >=390 white px : %s" % [y for y in range(H4) if bbox_where(p4, WHITE, 100, y, 500, y + 1)[1] >= 390])
L("   cols with >=60 black px (y 8..85): %s" % [x for x in range(W4) if bbox_where(p4, BLACK, x, 8, x + 1, 86)[1] >= 60])
L("   cols with >=60 white px          : %s" % [x for x in range(W4) if bbox_where(p4, WHITE, x, 8, x + 1, 86)[1] >= 60])
L("-- separator column centres (black runs, y=66) --")
seps = []
run = None
for x in range(0, W4):
    d = BLACK(p4[x, 66])
    if d and run is None:
        run = x
    elif not d and run is not None:
        seps.append((run, x - 1, (run + x - 1) / 2.0))
        run = None
L("   %s" % [(a, b) for a, b, _ in seps])
c = [m for _, _, m in seps]
L("   pitches: %s" % [round(c[i + 1] - c[i], 1) for i in range(len(c) - 1)])
L("-- red toggle button --")
bb, n = cc(p4, W4, H4, (3, 32), RED)
L("   CC(3,32) bbox=%s size=%s n=%d" % (bb, sz(bb), n))
bb2, n2 = bbox_where(p4, lambda p: p[0] > 200 and p[1] > 150 and p[2] < 140, 0, 0, 52, H4)
L("   yellow arrow inside it bbox=%s size=%s n=%d" % (bb2, sz(bb2), n2))
L("-- tooltip --")
bb, n = cc(p4, W4, H4, (45, 30), TIP)
L("   CC(45,30) bbox=%s size=%s n=%d" % (bb, sz(bb), n))
bb2, n2 = bbox_where(p4, WHITE, bb[0], bb[1], bb[2] + 1, bb[3] + 1)
L("   white text ink inside bbox=%s n=%d" % (bb2, n2))
im4.crop((bb[0], bb[1], bb[2] + 1, bb[3] + 1)).resize(((bb[2] - bb[0] + 1) * 6, (bb[3] - bb[1] + 1) * 6), Image.NEAREST).save(
    os.path.join(D, "crop_img4_tooltip_6x.png"))
L("   wrote crop_img4_tooltip_6x.png")
L("-- slot cell metrics --")
LEFTS = [52, 87, 122, 157, 192, 227, 262, 297, 332, 367, 402, 437, 472]
RIGHTS = LEFTS[1:] + [505]
for row_y in ((15, 45), (49, 78)):
    L("   row y=%d..%d (h=%d)" % (row_y[0], row_y[1], row_y[1] - row_y[0] + 1))
    for i in range(13):
        x0, x1 = LEFTS[i] + 2, RIGHTS[i] - 1
        kb, kn = bbox_where(p4, lambda p: p[2] > p[0] + 12 and 110 < p[0] and max(p[:3]) < 235, x0, row_y[0], x1, row_y[1] + 1)
        wb, wn = bbox_where(p4, WHITE, x0, (kb[3] + 1) if kb else row_y[0], x1, row_y[1] + 1)
        L("     c%-2d x=%3d..%3d w=%2d | keycap y=%s x=%s | name y=%s x=%s | keycap->name top gap=%s" % (
            i + 1, LEFTS[i], RIGHTS[i] - 1, RIGHTS[i] - 1 - LEFTS[i] + 1,
            (kb[1], kb[3]) if kb else None, (kb[0], kb[2]) if kb else None,
            (wb[1], wb[3]) if wb else None, (wb[0], wb[2]) if wb else None,
            (wb[1] - kb[3]) if (wb and kb) else None))

# ============================================================== img5
L("")
L("=" * 78)
L("IMAGE 5  (快捷键 collapsed / 打开快捷键 tooltip)")
L("=" * 78)
im5, p5, (W5, H5) = load("img5")
L("size=%dx%d mode=%s" % (W5, H5, im5.mode))
bb, n = cc(p5, W5, H5, (25, 14), TIP)
L("   tooltip CC(25,14) bbox=%s size=%s n=%d" % (bb, sz(bb), n))
bb2, n2 = bbox_where(p5, WHITE, bb[0], bb[1], bb[2] + 1, bb[3] + 1)
L("   tooltip white text bbox=%s n=%d" % (bb2, n2))
im5.crop((bb[0], bb[1], bb[2] + 1, bb[3] + 1)).resize(((bb[2] - bb[0] + 1) * 6, (bb[3] - bb[1] + 1) * 6), Image.NEAREST).save(
    os.path.join(D, "crop_img5_tooltip_6x.png"))
L("   wrote crop_img5_tooltip_6x.png")
bb, n = cc(p5, W5, H5, (1, 14), RED)
L("   red toggle button CC(1,14) bbox=%s size=%s n=%d" % (bb, sz(bb), n))
bb2, n2 = bbox_where(p5, lambda p: p[0] > 200 and p[1] > 150 and p[2] < 140, 0, 0, 40, 40)
L("   yellow arrow bbox=%s size=%s n=%d" % (bb2, sz(bb2), n2))
bb, n = cc(p5, W5, H5, (10, 45), lambda p: p[2] > 120 and p[2] > p[0] + 40)
L("   blue 界面 button CC(10,45) bbox=%s size=%s n=%d" % (bb, sz(bb), n))
L("   top-left light strip colours y=0..6 x=0..60:")
for y in range(0, 7):
    L("      y=%d : %s" % (y, hist(p5, (0, y, 60, y + 1), 5)))
L("   is there any slot-grid (repeating 35px black separators) in img5? --")
L("      cols with >=20 black px : %s" % [x for x in range(W5) if bbox_where(p5, BLACK, x, 0, x + 1, H5)[1] >= 20])
L("      rows with >=40 black px : %s" % [y for y in range(H5) if bbox_where(p5, BLACK, 0, y, W5, y + 1)[1] >= 40])
im5.crop((0, 0, 60, 30)).resize((360, 180), Image.NEAREST).save(os.path.join(D, "crop_img5_btn_6x.png"))
L("   wrote crop_img5_btn_6x.png box=(0,0,60,30)")

out.close()
print("done")
