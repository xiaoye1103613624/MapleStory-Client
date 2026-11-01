# r15a_s11_img2edge: exact tooltip edges (rows 11..16, left/right border columns),
# text bbox excluding the hand cursor, and the inventory-window extent + bottom band.
import os, io, json
from PIL import Image
from collections import Counter
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
p = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))["img2"]
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()
log = io.open(os.path.join(OUTDIR, "_g_img2edge.txt"), "w", encoding="utf-8")
for y in range(10, 17):
    log.write("y=%2d: %s\n" % (y, " ".join("%d:%02X%02X%02X" % (x, *px[x, y]) for x in range(80, 175, 1))))
log.write("-- left border cols x82..90, y13..41 --\n")
for x in range(82, 91):
    log.write("x=%3d: %s\n" % (x, " ".join("%d:%02X%02X%02X" % (y, *px[x, y]) for y in range(13, 42))))
log.write("-- right border cols x168..174, y13..41 --\n")
for x in range(168, 175):
    log.write("x=%3d: %s\n" % (x, " ".join("%d:%02X%02X%02X" % (y, *px[x, y]) for y in range(13, 42))))
log.write("-- bottom rows y38..43, x86..170 --\n")
for y in range(38, 44):
    log.write("y=%2d: %s\n" % (y, " ".join("%d:%02X%02X%02X" % (x, *px[x, y]) for x in range(86, 171, 2))))
def lum(c): return 0.299*c[0]+0.587*c[1]+0.114*c[2]
# text bbox inside box y14..40 x86..170, EXCLUDING cursor zone (cursor: white blob
# touching box from above; find white runs connected to y<=14 outside box? simpler:
# report bbox for y>=20 only and full, to show sensitivity)
for ystart in (14, 20, 23):
    t = [(x, y) for y in range(ystart, 41) for x in range(86, 171) if lum(px[x, y]) > 150]
    xs = [a for a, b in t]; ys = [b for a, b in t]
    log.write("text(y>=%d) bbox x[%d..%d] y[%d..%d] n=%d\n" % (ystart, min(xs), max(xs), min(ys), max(ys), len(t)))
# column profile of text rows y23..37 to find glyph groups
prof = [sum(1 for y in range(23, 38) if lum(px[x, y]) > 150) for x in range(86, 171)]
log.write("text col profile y23..37 (x86..170): %s\n" % prof)
# window extent: brown frame colour family
def brown(c): return abs(c[0]-108) < 40 and abs(c[1]-91) < 45 and abs(c[2]-73) < 50 and c[0] > c[2]
cols_with_frame = [x for x in range(W) if sum(1 for y in range(H) if brown(px[x, y])) > 100]
rows_with_frame = [y for y in range(H) if sum(1 for x in range(W) if brown(px[x, y])) > 100]
log.write("brown-frame cols(>100px)=%s rows(>100px)=%s\n" % (cols_with_frame[:6]+cols_with_frame[-6:], rows_with_frame[:6]+rows_with_frame[-6:]))
for y in range(268, H):
    log.write("bot y=%3d: %s\n" % (y, " ".join("%d:%02X%02X%02X" % (x, *px[x, y]) for x in range(0, W, 10))))
im.crop((84, 12, 173, 42)).resize((89*4, 30*4), Image.NEAREST).save(os.path.join(OUTDIR, "rgb", "img2_tooltip4xb.png"))
log.close()
print("OK")
