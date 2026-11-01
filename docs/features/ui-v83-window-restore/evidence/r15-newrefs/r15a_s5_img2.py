# r15a_s5_img2: v2 - tooltip fill is NEUTRAL dark grey; window frame is brown,
# scrollbar blue (both saturated). Mask = low-saturation dark pixels.
import os, io, json
from PIL import Image
from collections import Counter
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
p = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))["img2"]
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()
log = io.open(os.path.join(OUTDIR, "_d_img2.txt"), "w", encoding="utf-8")
log.write("size=%dx%d mode=RGB(tRNS junk ignored)\n" % (W, H))
log.write("corners TL=%s TR=%s BL=%s BR=%s\n" % (px[0, 0], px[W-1, 0], px[0, H-1], px[W-1, H-1]))

def lum(c): return 0.299*c[0] + 0.587*c[1] + 0.114*c[2]
def neutral_dark(c): return abs(c[0]-c[1]) <= 12 and abs(c[1]-c[2]) <= 12 and lum(c) < 90

mask = [[1 if neutral_dark(px[x, y]) else 0 for x in range(W)] for y in range(H)]
seen = [[0]*W for _ in range(H)]
comps = []
for y in range(H):
    for x in range(W):
        if mask[y][x] and not seen[y][x]:
            stack=[(x,y)]; seen[y][x]=1; pts=[]
            while stack:
                cx,cy=stack.pop(); pts.append((cx,cy))
                for dx,dy in ((1,0),(-1,0),(0,1),(0,-1),(1,1),(1,-1),(-1,1),(-1,-1)):
                    nx,ny=cx+dx,cy+dy
                    if 0<=nx<W and 0<=ny<H and mask[ny][nx] and not seen[ny][nx]:
                        seen[ny][nx]=1; stack.append((nx,ny))
            comps.append(pts)
comps.sort(key=len, reverse=True)
log.write("neutral-dark components (8-conn) sizes top8: %s\n" % [len(c) for c in comps[:8]])
for c in comps[:4]:
    xs=[q[0] for q in c]; ys=[q[1] for q in c]
    log.write("   bbox x[%d..%d] y[%d..%d] n=%d\n" % (min(xs),max(xs),min(ys),max(ys),len(c)))
big = comps[0]
xs=[q[0] for q in big]; ys=[q[1] for q in big]
bx0,bx1,by0,by1 = min(xs),max(xs),min(ys),max(ys)
# the white TEXT sits inside the fill; expand bbox to include non-neutral holes enclosed:
# recompute box as fill bbox (text can touch edges, so also dilate by 1 for AA border)
log.write("TOOLTIP fill bbox x[%d..%d] y[%d..%d] w=%d h=%d\n" % (bx0,bx1,by0,by1,bx1-bx0+1,by1-by0+1))

log.write("-- vertical slice x=%d y=%d..%d:\n" % ((bx0+bx1)//2, max(0,by0-5), min(H-1,by1+6)))
for y in range(max(0,by0-5), min(H,by1+7)):
    log.write("   y=%3d %s\n" % (y, px[(bx0+bx1)//2, y]))
log.write("-- horizontal slice y=%d x=%d..%d:\n" % ((by0+by1)//2, max(0,bx0-5), min(W-1,bx1+6)))
for x in range(max(0,bx0-5), min(W,bx1+7)):
    log.write("   x=%3d %s\n" % (x, px[x, (by0+by1)//2]))

cnt = Counter(px[x,y] for y in range(by0,by1+1) for x in range(bx0,bx1+1) if neutral_dark(px[x,y]))
log.write("fill histogram top6: %s\n" % cnt.most_common(6))
ring = Counter()
for x in range(bx0-3, bx1+4):
    for y in (by0-3,by0-2,by0-1,by1+1,by1+2,by1+3):
        if 0<=x<W and 0<=y<H: ring[px[x,y]] += 1
for y in range(by0-3, by1+4):
    for x in (bx0-3,bx0-2,bx0-1,bx1+1,bx1+2,bx1+3):
        if 0<=x<W and 0<=y<H: ring[px[x,y]] += 1
log.write("ring(1..3px outside) top10: %s\n" % ring.most_common(10))

# separator rows: a row inside the box whose dominant colour is a mid-grey line
for y in range(by0, by1+1):
    row = Counter(px[x,y] for x in range(bx0,bx1+1))
    c0 = row.most_common(1)[0]
    if lum(c0[0]) >= 90 and c0[1] > 0.5*(bx1-bx0+1):
        log.write("row y=%d dominant=%s n=%d/%d\n" % (y, c0[0], c0[1], bx1-bx0+1))

im.crop((bx0-2, by0-2, bx1+3, by1+3)).resize(((bx1-bx0+5)*4,(by1-by0+5)*4), Image.NEAREST)\
  .save(os.path.join(OUTDIR,"rgb","img2_tooltip4x.png"))
im.crop((0,0,W,60)).resize((W*4,240), Image.NEAREST).save(os.path.join(OUTDIR,"rgb","img2_top4x.png"))
log.close()
print("OK")
