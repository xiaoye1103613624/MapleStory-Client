# r15a_s6_img2probe: where exactly is the tooltip? locate (66,66,66) pixels and
# print a 4x4-block colour map of rows 0..60.
import os, io, json
from PIL import Image
from collections import Counter
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
p = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))["img2"]
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()
log = io.open(os.path.join(OUTDIR, "_d2_img2probe.txt"), "w", encoding="utf-8")
loc = [(x, y) for y in range(H) for x in range(W) if px[x, y] == (66, 66, 66)]
if loc:
    xs = [a for a, b in loc]; ys = [b for a, b in loc]
    log.write("(66,66,66) n=%d bbox x[%d..%d] y[%d..%d]\n" % (len(loc), min(xs), max(xs), min(ys), max(ys)))
# per-row counts of that colour
rc = Counter(y for _, y in loc)
log.write("rows with (66,66,66): %s\n" % sorted(rc.items()))
# block map rows 0..60, 4px blocks: dominant colour code
palette = {}
def code(c):
    if c not in palette:
        palette[c] = chr(ord('A') + len(palette))
    return palette[c]
for y in range(0, 60, 2):
    row = ""
    for x in range(0, W, 2):
        c = px[x, y]
        row += code(c)
    log.write("%3d %s\n" % (y, row))
log.write("palette: %s\n" % {v: k for k, v in palette.items()})
log.close()
print("OK")
