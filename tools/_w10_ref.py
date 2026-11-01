"""W10 reference-screenshot measurement helper (read-only). UTF-8 report file."""
import os
from PIL import Image

BASE = os.path.dirname(os.path.abspath(__file__))
EV = os.path.abspath(os.path.join(BASE, "..", "docs", "features", "ui-v83-window-restore",
                                  "evidence", "w10-quest"))
sh = Image.open(r"C:\Users\11036\Desktop\正确客户端截图\任务栏.png").convert("RGBA")
px = sh.load()
W, H = sh.size
out = open(os.path.join(EV, "_measure_ref.txt"), "w", encoding="utf-8")


def w(*a):
    print(*a, file=out)


def bbox(pred, x0, y0, x1, y1, label):
    pts = [(x, y) for y in range(y0, y1) for x in range(x0, x1) if pred(px[x, y])]
    if not pts:
        w("%-22s NONE" % label)
        return
    xs = [p[0] for p in pts]
    ys = [p[1] for p in pts]
    w("%-22s x %d..%d (w=%d)  y %d..%d (h=%d)  n=%d" %
      (label, min(xs), max(xs), max(xs) - min(xs) + 1, min(ys), max(ys),
       max(ys) - min(ys) + 1, len(pts)))


def near(c, t, tol=26):
    return all(abs(c[i] - t[i]) <= tol for i in range(3))


PINK = (236, 116, 137)
GRAYTAB = (215, 215, 215)
BLUEBAND = (153, 187, 204)
DARKBLUE = (51, 102, 153)
DETAILBLUE = (51, 119, 187)
CYANBTN = (0, 153, 204)
CLOSEBLUE = (51, 85, 119)
GREENBTN = (153, 204, 51)

w("### reference 548x396 color-region bboxes (window-relative)")
for name, col, box in [
    ("pink tab0", PINK, (0, 15, 245, 60)),
    ("gray tab", GRAYTAB, (0, 15, 245, 60)),
    ("lightblue hdr band", BLUEBAND, (0, 55, 245, 110)),
    ("darkblue sel band", DARKBLUE, (0, 55, 245, 200)),
    ("detail blue banner", DETAILBLUE, (243, 0, 548, 130)),
    ("marknpc cyan btn", CYANBTN, (243, 300, 548, 396)),
    ("close blue btn", CLOSEBLUE, (150, 0, 245, 30)),
    ("expand green btn", GREENBTN, (400, 0, 548, 40)),
]:
    bbox(lambda c, col=col: near(c, col, 30), *box, name)

w("")
w("### transitions along y=32 (tab row), x 0..245")
prev = None
for x in range(0, 245):
    c = px[x, 32][:3]
    if c != prev:
        w("  x %3d %s" % (x, c))
        prev = c

w("")
w("### transitions along x=30, y 15..60")
prev = None
for y in range(15, 60):
    c = px[30, y][:3]
    if c != prev:
        w("  y %3d %s" % (y, c))
        prev = c

w("")
w("### transitions along x=120, y 0..120 (left panel top)")
prev = None
for y in range(0, 120):
    c = px[120, y][:3]
    if c != prev:
        w("  y %3d %s" % (y, c))
        prev = c

w("")
w("### transitions along x=120, y 330..396 (left panel bottom)")
prev = None
for y in range(330, 396):
    c = px[120, y][:3]
    if c != prev:
        w("  y %3d %s" % (y, c))
        prev = c

w("")
w("### transitions along y=200, x 230..260 (panel seam)")
prev = None
for x in range(230, 262):
    c = px[x, 200][:3]
    if c != prev:
        w("  x %3d %s" % (x, c))
        prev = c

w("")
w("### transitions along x=400, y 0..140 (right panel top)")
prev = None
for y in range(0, 140):
    c = px[400, y][:3]
    if c != prev:
        w("  y %3d %s" % (y, c))
        prev = c

w("")
w("### transitions along x=400, y 330..396 (right panel bottom)")
prev = None
for y in range(330, 396):
    c = px[400, y][:3]
    if c != prev:
        w("  y %3d %s" % (y, c))
        prev = c

w("")
w("### backgrnd vs backgrnd2 seam probe: transitions along y=250, x 0..20")
prev = None
for x in range(0, 20):
    c = px[x, 250][:3]
    if c != prev:
        w("  x %3d %s" % (x, c))
        prev = c

out.close()
print("done")
