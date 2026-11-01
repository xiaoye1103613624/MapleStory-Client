# zz/s4_img2.py: final RGB measurement of img2 (button-help tooltip + inventory window).
import os, io, json, hashlib
from PIL import Image
from collections import Counter
ZZ = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs\zz"
man = json.load(io.open(os.path.join(ZZ, "manifest.json"), encoding="utf-8"))
p = os.path.join(ZZ, "src", "img2.png")
assert hashlib.md5(open(p, "rb").read()).hexdigest() == man["img2"]["md5"], "SNAPSHOT DRIFT"
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()
log = io.open(os.path.join(ZZ, "img2_meas.txt"), "w", encoding="utf-8")
log.write("size=%dx%d\n" % (W, H))
log.write("corners TL=%s TR=%s BL=%s BR=%s\n" % (px[0,0], px[W-1,0], px[0,H-1], px[W-1,H-1]))
def lum(c): return 0.299*c[0]+0.587*c[1]+0.114*c[2]
fam = {(66, 66, 66), (51, 51, 51)}
log.write("-- rows with >=20 family px --\n")
for y in range(H):
    xs = [x for x in range(W) if px[x, y] in fam]
    if len(xs) >= 20:
        log.write("y=%3d n=%3d x[%d..%d]\n" % (y, len(xs), min(xs), max(xs)))
log.write("-- cols with >=10 family px --\n")
for x in range(W):
    ys = [y for y in range(H) if px[x, y] in fam]
    if len(ys) >= 10:
        log.write("x=%3d n=%3d y[%d..%d]\n" % (x, len(ys), min(ys), max(ys)))
log.write("-- per-row colour census x84..172 y15..42 --\n")
for y in range(15, 43):
    c = Counter(px[x, y] for x in range(84, 173))
    log.write("y=%2d 66:%3d 51:%3d 255:%3d rest:%s\n" % (y, c.get((66,66,66),0), c.get((51,51,51),0),
              c.get((255,255,255),0), [(k, v) for k, v in c.most_common(6)
              if k not in ((66,66,66),(51,51,51),(255,255,255))]))
def wtxt(c): return lum(c) > 150 and max(c)-min(c) < 40
colocc = [x for x in range(86, 171) if any(wtxt(px[x, y]) for y in range(18, 40))]
rowocc = [y for y in range(18, 40) if any(wtxt(px[x, y]) for x in range(86, 171))]
def runs(idxs):
    out = []
    for i in idxs:
        if out and i == out[-1][1] + 1: out[-1][1] = i
        else: out.append([i, i])
    return [tuple(o) for o in out]
log.write("white-text col runs (y18..39): %s\n" % runs(colocc))
log.write("white-text row runs (x86..170): %s\n" % runs(rowocc))
# per-column counts to separate glyph clusters
prof = [sum(1 for y in range(18, 40) if wtxt(px[x, y])) for x in range(86, 171)]
log.write("text col profile: %s\n" % prof)
rprof = [sum(1 for x in range(86, 171) if wtxt(px[x, y])) for y in range(14, 44)]
log.write("text row profile y14..43: %s\n" % rprof)
# separators: rows inside fill that are a uniform mid-grey line across >70% of box
for y in range(18, 40):
    c = Counter(px[x, y] for x in range(86, 171)).most_common(1)[0]
    log.write("rowdom y=%2d %s x%d\n" % (y, c[0], c[1]))
# window frame: brown cols/rows
def brown(c): return c[0] > c[2]+15 and abs(c[0]-108) < 50 and abs(c[1]-91) < 50 and abs(c[2]-73) < 50
bcols = [x for x in range(W) if sum(1 for y in range(H) if brown(px[x, y])) > 150]
brows = [y for y in range(H) if sum(1 for x in range(W) if brown(px[x, y])) > 100]
log.write("brown cols: %s rows: %s\n" % (runs(bcols), runs(brows)))
# white grid lines (alpha-confirmed): vertical lines within y70..250
vcols = [x for x in range(W) if sum(1 for y in range(70, 250) if px[x, y] == (255,255,255)) > 100]
hrows = [y for y in range(40, 280) if sum(1 for x in range(5, 140) if px[x, y] == (255,255,255)) > 80]
log.write("grid vertical white cols: %s\n" % runs(vcols))
log.write("grid horizontal white rows: %s\n" % runs(hrows))
# red tab underline rows
red = [y for y in range(H) if sum(1 for x in range(W) if px[x, y][0] > 150 and px[x, y][1] < 60 and px[x, y][2] < 60) > 100]
log.write("red rows: %s\n" % runs(red))
# title bar extent: (85,85,130) family
ti = [y for y in range(H) if sum(1 for x in range(W) if px[x, y] == (85,85,130)) > 60]
log.write("titlebar(555582) rows: %s\n" % runs(ti))
im.crop((84, 15, 173, 42)).resize((89*4, 27*4), Image.NEAREST).save(os.path.join(ZZ, "crop", "img2_tooltip4x.png"))
log.close()
print("OK4")
