# r15a_s19_img2def: FINAL img2 measurement on pinned snapshot (md5-guarded).
import os, io, json
from PIL import Image
from collections import Counter
import r15a_lib as L

OUTDIR = L.OUTDIR
im = L.open_snap("img2")
W, H = im.size
px = im.load()
log = io.open(os.path.join(OUTDIR, "_o_img2def.txt"), "w", encoding="utf-8")
log.write("img2 snapshot md5=%s size=%dx%d\n" % (L.check_md5("img2"), W, H))
log.write("corners TL=%s TR=%s BL=%s BR=%s\n" % (px[0,0], px[W-1,0], px[0,H-1], px[W-1,H-1]))
log.write(L.ascii_map(im) + "\n")

fam = {(66, 66, 66), (51, 51, 51)}
L.log_rows(log, L.row_profile(px, W, H, lambda c: c in fam, 20), label="family(66/51 grey) rows n>=20:")
L.log_cols(log, L.col_profile(px, W, H, lambda c: c in fam, 10), label="family cols n>=10:")

# tooltip edges: exact per-row counts of 66 vs 51 in x84..172
for y in range(15, 43):
    c = Counter(px[x, y] for x in range(84, 173))
    log.write("y=%2d 66:%3d 51:%3d white:%3d other:%s\n"
              % (y, c.get((66,66,66),0), c.get((51,51,51),0), c.get((255,255,255),0),
                 [k for k in c if k not in ((66,66,66),(51,51,51),(255,255,255))][:6]))

# white text inside fill box (y18..39, x86..170), neutral-white only
def wtxt(c): return L.lum(c) > 150 and max(c) - min(c) < 40
colocc = [x for x in range(86, 171) if any(wtxt(px[x, y]) for y in range(18, 40))]
rowocc = [y for y in range(18, 40) if any(wtxt(px[x, y]) for x in range(86, 171))]
log.write("text cols %s\n" % (L.runs(colocc),))
log.write("text rows %s\n" % (L.runs(rowocc),))
# exclude hand cursor: cursor is the white blob x>=150 touching top border; recompute
# text runs restricted to y22..37 (below cursor body) for comparison
colocc2 = [x for x in range(86, 171) if any(wtxt(px[x, y]) for y in range(22, 38))]
log.write("text cols (y22..37 only) %s\n" % (L.runs(colocc2),))
rowocc2 = [y for y in range(18, 40) if sum(1 for x in range(86, 150) if wtxt(px[x, y])) > 2]
log.write("text rows (x86..149, n>2) %s\n" % (L.runs(rowocc2),))

# ---- window frame ----
def brown(c): return c[0] > c[2] + 20 and abs(c[0]-108) < 60 and abs(c[1]-91) < 60 and abs(c[2]-73) < 60
L.log_cols(log, L.col_profile(px, W, H, brown, 150), label="brown-frame cols n>=150:")
L.log_rows(log, L.row_profile(px, W, H, brown, 100), label="brown-frame rows n>=100:")
# red line rows
L.log_rows(log, L.row_profile(px, W, H, lambda c: c[0] > 150 and c[1] < 60 and c[2] < 60, 100), label="red rows n>=100:")
# slot cream colour bbox
def cream(c): return c in ((242, 242, 224),)
L.log_rows(log, L.row_profile(px, W, H, cream, 50), label="cream(242,242,224) rows n>=50:")
# white slot gaps columns within grid area y70..250
def wht(c): return c == (255, 255, 255)
colw = [x for x in range(W) if sum(1 for y in range(70, 250) if wht(px[x, y])) > 60]
log.write("grid vertical white lines cols: %s\n" % (L.runs(colw),))
roww = [y for y in range(60, 260) if sum(1 for x in range(5, 140) if wht(px[x, y])) > 60]
log.write("grid horizontal white lines rows: %s\n" % (L.runs(roww),))
# scrollbar: blue family right side
def blue(c): return c[2] > 150 and c[2] > c[0] + 60
L.log_cols(log, L.col_profile(px, W, H, blue, 30), label="blue cols n>=30:")
L.log_rows(log, L.row_profile(px, W, H, blue, 30), label="blue rows n>=30:")
# meso bar bottom: gold coin at left
def gold(c): return c[0] > 200 and c[1] > 150 and c[2] < 100
L.log_rows(log, L.row_profile(px, W, H, gold, 5), label="gold rows n>=5:")

im.crop((84, 15, 173, 42)).resize((89*4, 27*4), Image.NEAREST).save(os.path.join(OUTDIR, "rgb", "img2_FINAL_tooltip4x.png"))
im.crop((0, 0, W, 80)).resize((W*3, 240), Image.NEAREST).save(os.path.join(OUTDIR, "rgb", "img2_FINAL_top3x.png"))
im.crop((0, 250, W, H)).resize((W*3, (H-250)*3), Image.NEAREST).save(os.path.join(OUTDIR, "rgb", "img2_FINAL_bottom3x.png"))
log.close()
print("OK")
