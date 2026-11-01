# r15 s10: img3 region probes + text-area / nametag / button rects + text line pitch.
import os, io
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
P3 = r"C:\Users\11036\Desktop\正确客户端截图\点击“帮助”按钮打开的脚本框.png"
im = Image.open(P3).convert("RGBA"); W, H = im.size; px = im.load()
log = io.open(os.path.join(OUTDIR, "_s10_img3b.txt"), "w", encoding="utf-8")
log.write("img3 %dx%d\n\n--- sample grid colours ---\n" % (W, H))
for y in (10, 30, 60, 100, 140, 180, 220, 250, 270, 285, 295, 305):
    log.write(" y=%3d: " % y + " ".join("%d%s" % (x, px[x, y][:3]) for x in (10, 80, 150, 160, 200, 300, 450, 500, 515, 522)))
    log.write("\n")

def bbox(pred, x0=0, x1=None, y0=0, y1=None):
    if x1 is None: x1 = W - 1
    if y1 is None: y1 = H - 1
    ax = ay = bx = by = None
    n = 0
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            if pred(px[x, y]):
                n += 1
                ax = x if ax is None else min(ax, x); bx = x if bx is None else max(bx, x)
                ay = y if ay is None else min(ay, y); by = y if by is None else max(by, y)
    return (ax, ay, bx, by, n)

def darktext(p):  # black-ish glyph ink
    return p[0] + p[1] + p[2] < 240
# text area: find the bright rectangle right of x=155
bright = lambda p: min(p[:3]) >= 225
log.write("\n--- bright(>=225) bbox within x>=155,y 10..275: %s\n" % str(bright and bbox(bright, 155, 505, 10, 275)))
# nametag: dark blue band
nt = lambda p: p[2] > p[0] + 15 and p[2] < 130 and p[1] < 110 and p[0] < 110
log.write("--- darkblue nametag bbox within x 0..200, y 100..220: %s\n" % str(bbox(nt, 0, 200, 100, 220)))
nt2 = lambda p: p[0] < 90 and p[1] < 110 and 90 <= p[2] <= 170
log.write("--- nametag alt bbox x0..200 y100..220: %s\n" % str(bbox(nt2, 0, 200, 100, 220)))
# blue button (下项)
bb = lambda p: p[2] > 180 and p[2] > p[0] + 50 and p[1] > 100
log.write("--- blue-button bbox whole: %s\n" % str(bbox(bb)))
# yellow-green button (结束对话)
yg = lambda p: p[0] > 150 and p[1] > 150 and p[2] < 120
log.write("--- yellowgreen bbox whole: %s\n" % str(bbox(yg)))
# per-row ink profile inside the text area (x 160..500)
log.write("\n--- text-area per-row dark-ink counts, x=160..500, y=10..275 ---\n")
rows = []
for y in range(10, 276):
    n = sum(1 for x in range(160, 501) if darktext(px[x, y]))
    rows.append((y, n))
# collapse into bands
bands = []
inb = False
for y, n in rows:
    if n > 0 and not inb: st = y; inb = True
    if n == 0 and inb: bands.append((st, y - 1)); inb = False
if inb: bands.append((st, 275))
log.write("dark-ink bands (y0,y1,h): %s\n" % [(a, b, b - a + 1) for a, b in bands])
pitches = [bands[i + 1][0] - bands[i][0] for i in range(len(bands) - 1)]
log.write("band start pitches: %s\n" % pitches)
log.write("\n--- per-band x extents of dark ink (x=155..505) ---\n")
for a, b in bands:
    xs = [x for x in range(155, 506) if any(darktext(px[x, y]) for y in range(a, b + 1))]
    if xs: log.write("  y=%3d..%3d : xmin=%d xmax=%d\n" % (a, b, xs[0], xs[-1]))
log.close()
print("done")
