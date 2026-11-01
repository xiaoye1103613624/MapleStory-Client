# zz/qf_s10_img1.py: measure the item tooltip box in img1 (dark purple region).
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
log = io.open(os.path.join(ZZ, "qf_img1_tt.txt"), "w", encoding="utf-8")
def lum(c): return 0.299*c[0]+0.587*c[1]+0.114*c[2]
def dark(c): return lum(c) < 95 and c[2] >= c[0] - 10   # purple/blue dark
# row/col occupancy of dark mask
rr = []
for y in range(H):
    xs = [x for x in range(W) if dark(px[x, y])]
    if len(xs) > 100:
        rr.append((y, len(xs), min(xs), max(xs)))
log.write("dark rows n>100:\n")
for t in rr: log.write("  y=%3d n=%3d x[%d..%d]\n" % t)
cc = []
for x in range(W):
    ys = [y for y in range(H) if dark(px[x, y])]
    if len(ys) > 100:
        cc.append((x, len(ys), min(ys), max(ys)))
log.write("dark cols n>100:\n")
for t in cc: log.write("  x=%3d n=%3d y[%d..%d]\n" % t)
# box candidate: from occupancy plateau
log.write("done\n")
log.close()
print("OK10")
