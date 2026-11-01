# r15a_s13_img2fix: definitive tooltip box rows/cols in img2.
import os, io, json
from PIL import Image
from collections import Counter
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
p = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))["img2"]
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()
log = io.open(os.path.join(OUTDIR, "_i_img2fix.txt"), "w", encoding="utf-8")
def lum(c): return 0.299*c[0]+0.587*c[1]+0.114*c[2]
for y in range(16, 46):
    row = [px[x, y] for x in range(80, 175)]
    n66 = sum(1 for c in row if c == (66, 66, 66))
    n51 = sum(1 for c in row if c == (51, 51, 51))
    xs = [x for x in range(80, 175) if px[x, y] in ((66, 66, 66), (51, 51, 51))]
    log.write("y=%2d n66=%3d n51=%3d span x[%s..%s]\n" % (y, n66, n51, min(xs) if xs else -1, max(xs) if xs else -1))
for x in range(82, 92):
    ys = [y for y in range(14, 44) if px[x, y] in ((66, 66, 66), (51, 51, 51))]
    log.write("col x=%d n=%d y[%s..%s]\n" % (x, len(ys), min(ys) if ys else -1, max(ys) if ys else -1))
for x in range(166, 175):
    ys = [y for y in range(14, 44) if px[x, y] in ((66, 66, 66), (51, 51, 51))]
    log.write("col x=%d n=%d y[%s..%s]\n" % (x, len(ys), min(ys) if ys else -1, max(ys) if ys else -1))
# white text inside fill area only (y 20..38, x 86..170)
t = [(x, y) for y in range(19, 39) for x in range(86, 171) if lum(px[x, y]) > 150 and px[x, y][0] > 200 and abs(px[x,y][0]-px[x,y][2]) < 30]
xs = [a for a, b in t]; ys = [b for a, b in t]
log.write("white-in-box bbox x[%d..%d] y[%d..%d] n=%d\n" % (min(xs), max(xs), min(ys), max(ys), len(t)))
prof = [sum(1 for y in range(19, 39) if lum(px[x, y]) > 150 and abs(px[x,y][0]-px[x,y][2]) < 30) for x in range(86, 171)]
log.write("col profile: %s\n" % prof)
runs = []
inr = False
for i, v in enumerate(prof + [0]):
    if v > 0 and not inr: s = i; inr = True
    if v == 0 and inr: runs.append((86+s, 86+i-1)); inr = False
log.write("glyph col-runs: %s\n" % runs)
rowh = [sum(1 for x in range(86, 171) if lum(px[x, y]) > 150 and abs(px[x,y][0]-px[x,y][2]) < 30) for y in range(14, 44)]
log.write("row profile y14..43: %s\n" % rowh)
im.crop((83, 16, 174, 43)).resize((91*4, 27*4), Image.NEAREST).save(os.path.join(OUTDIR, "rgb", "img2_tooltip4xc.png"))
log.close()
print("OK")
