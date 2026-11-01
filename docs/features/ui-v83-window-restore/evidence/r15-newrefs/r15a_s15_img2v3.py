# r15a_s15_img2v3: full re-measure of img2 on the NOW-STABLE file (md5 818237...).
# Writes ASCII map + tooltip row/col profiles + text bbox + crops.
import os, io, json
from PIL import Image
from collections import Counter
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
p = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))["img2"]
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()
log = io.open(os.path.join(OUTDIR, "_k_img2v3.txt"), "w", encoding="utf-8")
log.write("size=%dx%d\n" % (W, H))

def b(c):
    lum = (c[0] + c[1] + c[2]) / 3
    if lum < 40: return "#"
    if lum < 80: return "-"
    if lum < 130: return "+"
    if lum < 200: return "="
    return "."
log.write("ASCII map (rows every 2px, 1 char/px):\n")
for y in range(0, H, 2):
    log.write("%3d|%s\n" % (y, "".join(b(px[x, y]) for x in range(W))))

def lum(c): return 0.299*c[0]+0.587*c[1]+0.114*c[2]
# tooltip = neutral dark grey family
fam = {(66, 66, 66), (51, 51, 51)}
log.write("-- rows: count of (66,66,66)/(51,51,51) per row over full width --\n")
for y in range(H):
    n = sum(1 for x in range(W) if px[x, y] in fam)
    if n:
        xs = [x for x in range(W) if px[x, y] in fam]
        log.write("y=%3d n=%3d x[%d..%d]\n" % (y, n, min(xs), max(xs)))
log.close()
print("OK")
