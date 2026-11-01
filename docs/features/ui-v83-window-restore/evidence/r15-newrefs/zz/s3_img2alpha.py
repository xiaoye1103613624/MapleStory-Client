# zz/s3_img2alpha: alpha scanlines + tooltip/window segmentation truth for img2.
import os, io, json
from PIL import Image
from collections import Counter
ZZ = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs\zz"
im = Image.open(os.path.join(ZZ, "src", "img2.png")).convert("RGBA")
W, H = im.size
px = im.load()
log = io.open(os.path.join(ZZ, "img2_alpha.txt"), "w", encoding="utf-8")
log.write("size %dx%d\n" % (W, H))
for y in (5, 30, 60, 150, 250, 280):
    log.write("y=%3d: %s\n" % (y, "".join(str(px[x, y][3] // 85) for x in range(W))))
for x in (2, 40, 86, 128, 168, 173):
    log.write("x=%3d: %s\n" % (x, "".join(str(px[x, y][3] // 85) for y in range(0, H, 1))))
log.write("alpha hist: %s\n" % Counter(px[x, y][3] for y in range(H) for x in range(W)).most_common())
# where is alpha=255?
pts = [(x, y) for y in range(H) for x in range(W) if px[x, y][3] == 255]
xs = [a for a, b in pts]; ys = [b for a, b in pts]
log.write("opaque bbox x[%d..%d] y[%d..%d] n=%d\n" % (min(xs), max(xs), min(ys), max(ys), len(pts)))
# rows of opaque
rc = Counter(ys)
log.write("opaque rows sample: %s\n" % sorted(rc.items())[::10])
# RGB at a slot centre and title bar
for pt in [(50, 150), (10, 5), (100, 28), (30, 270), (150, 270), (86, 20)]:
    log.write("px%s = %s\n" % (pt, px[pt[0], pt[1]]))
log.close()
print("OK3")
