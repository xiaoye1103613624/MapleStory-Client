# r15a_s12_ascii: full-image luminance ASCII map of img2 (1 char per px) +
# exact colour dumps of two rows; also re-verify px identity after crop.
import os, io, json
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
p = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))["img2"]
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()
log = io.open(os.path.join(OUTDIR, "_h_ascii.txt"), "w", encoding="utf-8")
log.write("size %dx%d px[86,20]=%s px[86,14]=%s\n" % (W, H, px[86, 20], px[86, 14]))
def b(c):
    r, g, bl = c
    lum = (r + g + bl) / 3
    if lum < 40: return "#"
    if lum < 80: return "-"
    if lum < 130: return "+"
    if lum < 200: return "="
    return "."
for y in range(0, H, 2):
    log.write("%3d|%s\n" % (y, "".join(b(px[x, y]) for x in range(0, W, 1))))
log.close()
print("OK")
