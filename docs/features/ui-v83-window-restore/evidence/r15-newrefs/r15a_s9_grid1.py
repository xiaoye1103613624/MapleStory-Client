# r15a_s9_grid1: grid-annotated 2x views for img1 (equipment tooltip) plus a
# neutral-dark component scan to find its tooltip box.
import os, io, json
from PIL import Image, ImageDraw
from collections import Counter
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
paths = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))

def grid(key, step=20, scale=2):
    im = Image.open(paths[key]).convert("RGB")
    g = im.resize((im.width*scale, im.height*scale), Image.NEAREST)
    d = ImageDraw.Draw(g)
    for x in range(0, im.width, step):
        d.line([(x*scale, 0), (x*scale, g.height)], fill=(0,255,0), width=1)
        d.text((x*scale+2, 2), str(x), fill=(255,0,255))
    for y in range(0, im.height, step):
        d.line([(0, y*scale), (g.width, y*scale)], fill=(0,255,0), width=1)
        d.text((2, y*scale+2), str(y), fill=(0,0,255))
    g.save(os.path.join(OUTDIR, "rgb", "%s_grid.png" % key))

grid("img1")
im = Image.open(paths["img1"]).convert("RGB")
W, H = im.size
px = im.load()
log = io.open(os.path.join(OUTDIR, "_e_img1scan.txt"), "w", encoding="utf-8")
log.write("size=%dx%d corners TL=%s TR=%s BL=%s BR=%s\n" % (W,H,px[0,0],px[W-1,0],px[0,H-1],px[W-1,H-1]))
def lum(c): return 0.299*c[0]+0.587*c[1]+0.114*c[2]
def nd(c): return abs(c[0]-c[1])<=12 and abs(c[1]-c[2])<=12 and lum(c)<90
cnt = Counter((x//10*10, y//10*10) for y in range(H) for x in range(W) if nd(px[x,y]))
log.write("neutral-dark 10x10 block occupancy: %s\n" % sorted(cnt.items(), key=lambda kv:-kv[1])[:20])
n = sum(1 for y in range(H) for x in range(W) if nd(px[x,y]))
log.write("total neutral-dark px=%d\n" % n)
# full-colour block map every 4 px for rows 0..H step 4 is too big; instead dominant colour per 20x20 block
for y in range(0, H, 20):
    row = ""
    for x in range(0, W, 20):
        c = Counter(px[xx, yy] for yy in range(y, min(y+20,H)) for xx in range(x, min(x+20,W)))
        dom = c.most_common(1)[0][0]
        row += "%s%s " % ("ABCDEFHJKLMNPQRSTUVWXY"[len(row) % 24], "")  # placeholder
    log.write("y=%3d %s\n" % (y, row))
log.close()
print("OK")
