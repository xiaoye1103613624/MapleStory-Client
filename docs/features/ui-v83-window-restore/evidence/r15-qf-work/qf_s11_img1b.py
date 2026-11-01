# zz/qf_s11_img1b.py: tooltip border/fill/separators/text-bands/icon for img1.
import os, io, hashlib, json
from PIL import Image
from collections import Counter
ZZ = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-qf-work"
man = json.load(io.open(os.path.join(ZZ, "manifest.json"), encoding="utf-8"))
p = os.path.join(ZZ, "src", "img1.png")
assert hashlib.md5(open(p, "rb").read()).hexdigest() == man["img1"]["md5"]
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()
log = io.open(os.path.join(ZZ, "qf_img1b.txt"), "w", encoding="utf-8")
def lum(c): return 0.299*c[0]+0.587*c[1]+0.114*c[2]
BX0, BX1, BY0, BY1 = 143, 358, 140, 403   # visible tooltip bbox (right/bottom clipped)
# edge rings
for x in range(BX0-4, BX0+6):
    col = Counter(px[x, y] for y in range(BY0+5, BY1-5))
    log.write("left col x=%3d top=%s\n" % (x, col.most_common(3)))
for y in range(BY0-4, BY0+6):
    row = Counter(px[x, y] for x in range(BX0+5, BX1-5))
    log.write("top row y=%3d top=%s\n" % (y, row.most_common(3)))
for x in range(BX1-5, BX1+1):
    col = Counter(px[x, y] for y in range(BY0+5, BY1-5))
    log.write("right col x=%3d top=%s\n" % (x, col.most_common(3)))
for y in range(BY1-5, BY1+1):
    row = Counter(px[x, y] for x in range(BX0+5, BX1-5))
    log.write("bottom row y=%3d top=%s\n" % (y, row.most_common(3)))
# interior fill census
c = Counter(px[x, y] for y in range(BY0, BY1+1) for x in range(BX0, BX1+1))
log.write("interior top12: %s\n" % c.most_common(12))
pts = [(150, 240), (340, 240), (250, 300), (160, 380), (330, 380), (250, 250)]
log.write("interior samples: %s\n" % [(q, px[q[0], q[1]]) for q in pts])
# separator rows: dominant colour that is NOT the fill family, spanning >80% width
fillfam = {(17, 17, 51), (28, 23, 57), (34, 28, 57), (28, 28, 57), (23, 23, 51)}
for y in range(BY0, BY1+1):
    row = [px[x, y] for x in range(BX0+2, BX1-1)]
    dom = Counter(row).most_common(1)[0]
    frac = dom[1] / float(len(row))
    if frac > 0.8 and dom[0] not in fillfam:
        log.write("separator? y=%3d dom=%s frac=%.2f\n" % (y, dom[0], frac))
# text bands: rows with many bright px
log.write("-- bright row profile (lum>140) --\n")
prev = 0
for y in range(BY0, BY1+1):
    n = sum(1 for x in range(BX0, BX1+1) if lum(px[x, y]) > 140)
    log.write("y=%3d n=%3d\n" % (y, n))
log.close()
print("OK11")
