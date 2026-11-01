"""W10 probe 2: row bands + tab boxes + scrollbar (reference) and asset inner bboxes."""
import os
from PIL import Image

BASE = os.path.dirname(os.path.abspath(__file__))
EV = os.path.abspath(os.path.join(BASE, "..", "docs", "features", "ui-v83-window-restore",
                                  "evidence", "w10-quest"))
PNG = os.path.join(EV, "pngs")
out = open(os.path.join(EV, "_probe2.txt"), "w", encoding="utf-8")


def w(*a):
    print(*a, file=out)


sh = Image.open(r"C:\Users\11036\Desktop\正确客户端截图\任务栏.png").convert("RGBA")
sp = sh.load()

w("== ref column x=200 (list rows), y 44..260 transitions")
prev = None
for y in range(44, 260):
    c = sp[200, y][:3]
    if c != prev:
        w("   y %3d %s" % (y, c))
        prev = c

w("== ref column x=214 (list rows, right of text), y 44..260 transitions")
prev = None
for y in range(44, 260):
    c = sp[214, y][:3]
    if c != prev:
        w("   y %3d %s" % (y, c))
        prev = c

w("== ref tab boxes: dark(<120 sum) columns, y 24..40")
cols = [x for x in range(0, 245) if sum(sp[x, 30][:3]) < 150]
w("   y=30 dark cols:", cols)
cols = [x for x in range(0, 245) if sum(sp[x, 24][:3]) < 150]
w("   y=24 dark cols:", cols)
rows = [y for y in range(15, 50) if sum(sp[60, y][:3]) < 150]
w("   x=60 dark rows:", rows)

w("== ref tab fill samples")
for x, y in [(60, 26), (60, 36), (100, 26), (100, 36), (160, 26), (160, 36), (220, 30), (30, 30)]:
    w("   (%d,%d) %s" % (x, y, sp[x, y][:3]))

w("== ref left scrollbar: saturated-blue columns x 215..245, y 45..360")
for x in range(215, 245):
    n = sum(1 for y in range(45, 360) if sp[x, y][2] > 120 and sp[x, y][2] - sp[x, y][0] > 40)
    if n:
        w("   x %d n=%d" % (x, n))

w("== ref right scrollbar: saturated-blue columns x 520..548, y 120..360")
for x in range(518, 548):
    n = sum(1 for y in range(120, 360) if sp[x, y][2] > 120 and sp[x, y][2] - sp[x, y][0] > 40)
    if n:
        w("   x %d n=%d" % (x, n))

w("== ref right panel detail text rows: column x=270, y 118..360 transitions")
prev = None
for y in range(118, 360):
    c = sp[270, y][:3]
    if c != prev:
        w("   y %3d %s" % (y, c))
        prev = c


def inner(fp, label, thr=16):
    im = Image.open(fp).convert("RGBA")
    p = im.load()
    W, H = im.size
    pts = [(x, y) for y in range(H) for x in range(W) if p[x, y][3] > thr]
    xs = [q[0] for q in pts]
    ys = [q[1] for q in pts]
    w("%-16s %dx%d opaque bbox x %d..%d y %d..%d" % (label, W, H, min(xs), max(xs), min(ys), max(ys)))
    # per-row: first/last opaque x, to find the white content rect
    return p, W, H


w("")
w("== assets")
for n, lab in [("UIWindow.img_Quest_backgrnd.png", "backgrnd"),
               ("UIWindow.img_Quest_backgrnd2.png", "backgrnd2"),
               ("UIWindow.img_Quest_backgrnd4.png", "backgrnd4")]:
    p, W, H = inner(os.path.join(PNG, n), lab)
    # white rows (content area)
    white = [y for y in range(H) if all(p[x, y][3] > 16 and min(p[x, y][:3]) > 245 for x in range(20, W - 20))]
    if white:
        runs = []
        s = white[0]
        pr = white[0]
        for y in white[1:]:
            if y != pr + 1:
                runs.append((s, pr))
                s = y
            pr = y
        runs.append((s, pr))
        w("   white runs: %s" % [r for r in runs if r[1] - r[0] > 3])
    # saturated blue banner rows
    blue = [y for y in range(H) if all(p[x, y][3] > 16 and p[x, y][2] > p[x, y][0] + 40 for x in range(30, W - 30))]
    if blue:
        w("   blue rows: %d..%d" % (min(blue), max(blue)))
    # bottom band rows (light blue 153,187,204)
    band = [y for y in range(H) if all(p[x, y][3] > 16 and abs(p[x, y][0] - 153) < 30 and abs(p[x, y][1] - 187) < 30 for x in range(30, W - 30))]
    if band:
        w("   bottomband rows: %d..%d" % (min(band), max(band)))

out.close()
print("done")
