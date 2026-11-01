# r15a_s7_img2final: exact tooltip box (row/col profiles), border colours,
# interior fill samples, text bbox -> padding; 4x crop for transcription.
import os, io, json
from PIL import Image
from collections import Counter
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
p = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))["img2"]
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()
log = io.open(os.path.join(OUTDIR, "_d3_img2final.txt"), "w", encoding="utf-8")

def lum(c): return 0.299*c[0]+0.587*c[1]+0.114*c[2]
def darkish(c): return lum(c) < 140 and max(c)-min(c) < 40   # grey-ish dark, low saturation

# restrict to search window around visually located tooltip
X0, X1, Y0, Y1 = 60, W-1, 10, 50
rows = {}
for y in range(Y0, Y1):
    xs = [x for x in range(X0, X1) if darkish(px[x, y])]
    rows[y] = (len(xs), min(xs) if xs else -1, max(xs) if xs else -1)
for y in sorted(rows):
    log.write("row %3d n=%3d x[%d..%d]\n" % ((y,) + rows[y]))
cols = {}
for x in range(X0, X1):
    ys = [y for y in range(Y0, Y1) if darkish(px[x, y])]
    cols[x] = (len(ys), min(ys) if ys else -1, max(ys) if ys else -1)
keep = [x for x in sorted(cols) if cols[x][0] >= 10]
log.write("cols with >=10 dark px: x[%d..%d] detail:\n" % (min(keep), max(keep)))
for x in keep:
    log.write("col %3d n=%3d y[%d..%d]\n" % ((x,) + cols[x]))

# box = dense region rows 17..39; print every pixel of the 4 edge bands
def band(y0, y1, label):
    log.write("== %s ==\n" % label)
    for y in range(y0, y1+1):
        cnt = Counter(px[x, y] for x in range(86, 171))
        log.write(" y=%d top=%s\n" % (y, cnt.most_common(4)))
band(15, 19, "top edge bands y15..19 (x86..170)")
band(37, 41, "bottom edge bands y37..41")
log.write("== left/right edges per row (x83..88 | x168..174) ==\n")
for y in range(16, 41):
    L = [(x, px[x, y]) for x in range(83, 89)]
    R = [(x, px[x, y]) for x in range(168, 175)]
    log.write(" y=%2d L=%s R=%s\n" % (y, [c for _, c in L], [c for _, c in R]))

# interior fill samples (avoid text rows 19..37 centre): corners inside box
pts = [(88, 18), (168, 18), (88, 38), (168, 38), (88, 22), (168, 28), (100, 38), (140, 19)]
log.write("interior fill samples: %s\n" % [(pt, px[pt[0], pt[1]]) for pt in pts])
fillcnt = Counter(px[x, y] for y in range(18, 39) for x in range(87, 170) if px[x, y] in ((66,66,66),(51,51,51)))
log.write("fill-family counts inside box: %s\n" % fillcnt.most_common())

# text pixels (bright) inside box -> bbox & line bands
tx = [(x, y) for y in range(16, 41) for x in range(86, 171) if lum(px[x, y]) > 150]
if tx:
    xs = [a for a, b in tx]; ys = [b for a, b in tx]
    log.write("text bbox x[%d..%d] y[%d..%d] n=%d\n" % (min(xs), max(xs), min(ys), max(ys), len(tx)))
    rc = Counter(ys)
    log.write("text row histogram: %s\n" % sorted(rc.items()))
# separator check: any full-width non-fill row between?
im.crop((84, 14, 173, 42)).resize((89*4, 28*4), Image.NEAREST).save(os.path.join(OUTDIR, "rgb", "img2_tooltip4x.png"))
log.close()
print("OK")
