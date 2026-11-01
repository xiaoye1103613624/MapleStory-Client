# zz/s6_img2box: luminance-band profiles to isolate the tooltip box rows/cols.
import os, io
from PIL import Image
from collections import Counter
ZZ = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs\zz"
im = Image.open(os.path.join(ZZ, "src", "img2.png")).convert("RGB")
W, H = im.size
px = im.load()
log = io.open(os.path.join(ZZ, "img2_box.txt"), "w", encoding="utf-8")
def lum(c): return 0.299*c[0]+0.587*c[1]+0.114*c[2]
# dark rows across x86..170
for y in range(8, 48):
    row = [px[x, y] for x in range(86, 171)]
    nd = sum(1 for c in row if lum(c) < 100)
    c0 = Counter(row).most_common(2)
    log.write("y=%2d dark<100:%3d dom=%s\n" % (y, nd, c0))
# dark cols across y8..40
log.write("-- cols x80..174 dark count y8..40 --\n")
for x in range(80, 175):
    nd = sum(1 for y in range(8, 41) if lum(px[x, y]) < 100)
    log.write("x=%3d %3d %s\n" % (x, nd, "D" * min(nd, 60)))
log.close()
print("OK6")
