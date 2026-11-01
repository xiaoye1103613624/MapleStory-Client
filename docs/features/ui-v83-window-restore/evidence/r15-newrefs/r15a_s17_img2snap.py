# r15a_s17_img2snap: definitive img2 measurement on the STABLE snapshot.
import os, io, json, sys
from PIL import Image
from collections import Counter
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
p = os.path.join(OUTDIR, "src", "img2.png")
tag = sys.argv[1] if len(sys.argv) > 1 else "a"
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()
log = io.open(os.path.join(OUTDIR, "_m_img2snap_%s.txt" % tag), "w", encoding="utf-8")
log.write("size=%dx%d\n" % (W, H))
log.write("spot px[82,23]=%s px[86,20]=%s px[100,30]=%s px[0,0]=%s px[174,289]=%s\n"
          % (px[82, 23], px[86, 20], px[100, 30], px[0, 0], px[174, 289]))
def lum(c): return 0.299*c[0]+0.587*c[1]+0.114*c[2]
fam = {(66, 66, 66), (51, 51, 51)}
n66 = sum(1 for y in range(H) for x in range(W) if px[x, y] == (66, 66, 66))
log.write("count(66,66,66)=%d\n" % n66)
log.write("-- tooltip-family row profile (full width) --\n")
for y in range(H):
    xs = [x for x in range(W) if px[x, y] in fam]
    if len(xs) > 20:
        log.write("y=%3d n=%3d x[%d..%d]\n" % (y, len(xs), min(xs), max(xs)))
log.write("-- tooltip-family col profile (rows 10..50) --\n")
for x in range(W):
    ys = [y for y in range(10, 50) if px[x, y] in fam]
    if len(ys) > 10:
        log.write("x=%3d n=%3d y[%d..%d]\n" % (x, len(ys), min(ys), max(ys)))
# exact box rows around edges
for y in range(14, 44):
    row = Counter(px[x, y] for x in range(84, 173))
    log.write("y=%2d %s\n" % (y, row.most_common(5)))
# white text bbox inside box interior y19..38 x86..170 (neutral white only)
t = [(x, y) for y in range(19, 39) for x in range(86, 171)
     if lum(px[x, y]) > 150 and max(px[x, y]) - min(px[x, y]) < 40]
xs = [a for a, b in t]; ys = [b for a, b in t]
log.write("white-text bbox x[%d..%d] y[%d..%d] n=%d\n" % (min(xs), max(xs), min(ys), max(ys), len(t)))
prof = [sum(1 for y in range(19, 39) if lum(px[x, y]) > 150 and max(px[x, y])-min(px[x, y]) < 40) for x in range(86, 171)]
runs = []
inr = False
for i, v in enumerate(prof + [0]):
    if v > 0 and not inr: s = i; inr = True
    if v == 0 and inr: runs.append((86+s, 86+i-1)); inr = False
log.write("text col runs: %s\n" % runs)
rprof = [sum(1 for x in range(86, 171) if lum(px[x, y]) > 150 and max(px[x, y])-min(px[x, y]) < 40) for y in range(14, 44)]
log.write("text row runs y14..43: %s\n" % rprof)
im.crop((84, 16, 173, 42)).resize((89*4, 26*4), Image.NEAREST).save(os.path.join(OUTDIR, "rgb", "img2_snap_tooltip4x.png"))
im.resize((W*2, H*2), Image.NEAREST).save(os.path.join(OUTDIR, "rgb", "img2_snap_2x.png"))
log.close()
print("OK")
