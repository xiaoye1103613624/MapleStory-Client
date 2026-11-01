# zz/qf_s13_img1bands.py: text bands in img1 tooltip + 4x crops per band.
import os, io, hashlib, json
from PIL import Image
ZZ = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-qf-work"
man = json.load(io.open(os.path.join(ZZ, "manifest.json"), encoding="utf-8"))
p = os.path.join(ZZ, "src", "img1.png")
assert hashlib.md5(open(p, "rb").read()).hexdigest() == man["img1"]["md5"]
im = Image.open(p).convert("RGB")
px = im.load()
log = io.open(os.path.join(ZZ, "qf_img1bands.txt"), "w", encoding="utf-8")
def lum(c): return 0.299*c[0]+0.587*c[1]+0.114*c[2]
BX0, BX1, BY0, BY1 = 143, 358, 140, 403
prof = []
for y in range(BY0, BY1+1):
    n = sum(1 for x in range(BX0, BX1+1) if lum(px[x, y]) > 140)
    prof.append((y, n))
bands = []
inb = False
for y, n in prof + [(BY1+1, 0)]:
    if n > 3 and not inb:
        s = y; inb = True
    elif n <= 3 and inb:
        bands.append((s, y-1)); inb = False
log.write("bright bands: %s\n" % bands)
# left x of first text pixel per band (scan from BX0)
for (a, b) in bands:
    xs = [x for y in range(a, b+1) for x in range(BX0, BX1+1) if lum(px[x, y]) > 140]
    log.write("band y[%d..%d] x[%d..%d] n=%d\n" % (a, b, min(xs), max(xs), len(xs)))
    crop = im.crop((BX0-1, a-1, min(BX1+2, 358), b+2))
    crop = crop.resize((crop.width*4, crop.height*4), Image.NEAREST)
    crop.save(os.path.join(ZZ, "crop", "img1_band_%d_%d.png" % (a, b)))
log.close()
print("OK13")
