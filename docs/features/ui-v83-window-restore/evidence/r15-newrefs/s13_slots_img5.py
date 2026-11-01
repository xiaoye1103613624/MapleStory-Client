# r15 s13: img4 slot internals (key-name vs action-label bands) + img5 tooltip.
import os, io
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
P4 = r"C:\Users\11036\Desktop\正确客户端截图\关闭快捷键向下时 快捷键处于展开状态.png"
P5 = r"C:\Users\11036\Desktop\正确客户端截图\打开快捷键，向上时快捷键处于收起状态.png"
log = io.open(os.path.join(OUTDIR, "_s13_slots_img5.txt"), "w", encoding="utf-8")
im = Image.open(P4).convert("RGBA"); W, H = im.size; px = im.load()
log.write("### img4 %dx%d : slot cell ink profile\n" % (W, H))
# slot column boundaries derived from black separators (pitch 35 from x=52)
bounds = [52 + 35 * i for i in range(13)] + [505]
log.write("assumed slot left edges: %s\n" % bounds[:-1])
rows = [(15, 45), (49, 78)]
for ri, (ry0, ry1) in enumerate(rows):
    log.write("\n-- slot row %d (y=%d..%d) --\n" % (ri + 1, ry0, ry1))
    for ci in range(13):
        x0, x1 = bounds[ci], bounds[ci + 1] - 1
        prof = []
        for y in range(ry0, ry1 + 1):
            n = sum(1 for x in range(x0, min(x1, W - 1) + 1)
                    if px[x, y][0] + px[x, y][1] + px[x, y][2] < 300)
            g = sum(1 for x in range(x0, min(x1, W - 1) + 1)
                    if min(px[x, y][:3]) >= 245)
            prof.append((y, n, g))
        ink = [y for y, n, g in prof if n > 0]
        wt = [y for y, n, g in prof if g > 0]
        log.write("  slot%2d x=%3d..%3d : dark-ink y=%s ; white-ink y=%s\n" % (
            ci + 1, x0, x1, (ink[0], ink[-1]) if ink else None, (wt[0], wt[-1]) if wt else None))
# per-row aggregate ink bands for the whole slot grid
for ry0, ry1 in rows:
    prof = [(y, sum(1 for x in range(52, 506) if px[x, y][0]+px[x, y][1]+px[x, y][2] < 300)) for y in range(ry0, ry1 + 1)]
    log.write("\nrow y=%d..%d dark-ink per scanline: %s\n" % (ry0, ry1, prof))

# red collapse button at far left
def bbox(pred, x0, x1, y0, y1):
    ax=ay=bx=by=None;n=0
    for y in range(y0,y1+1):
        for x in range(x0,x1+1):
            if pred(px[x,y]):
                n+=1
                ax=x if ax is None else min(ax,x);bx=x if bx is None else max(bx,x)
                ay=y if ay is None else min(ay,y);by=y if by is None else max(by,y)
    return (ax,ay,bx,by,n)
red = lambda p: p[0] > 150 and p[1] < 90 and p[2] < 110
log.write("\nimg4 red-button bbox x<50: %s\n" % str(bbox(red, 0, 49, 0, H - 1)))
tipbg = lambda p: abs(p[0]-74)<16 and abs(p[1]-79)<16 and abs(p[2]-125)<20
log.write("img4 tooltip bg bbox x<50: %s\n" % str(bbox(tipbg, 0, 60, 0, H - 1)))

im5 = Image.open(P5).convert("RGBA"); W5, H5 = im5.size; q = im5.load()
log.write("\n### img5 %dx%d\n" % (W5, H5))
def bbox5(pred, x0, x1, y0, y1):
    ax=ay=bx=by=None;n=0
    for y in range(y0,y1+1):
        for x in range(x0,x1+1):
            if pred(q[x,y]):
                n+=1
                ax=x if ax is None else min(ax,x);bx=x if bx is None else max(bx,x)
                ay=y if ay is None else min(ay,y);by=y if by is None else max(by,y)
    return (ax,ay,bx,by,n)
log.write("tooltip bg bbox: %s\n" % str(bbox5(tipbg, 0, W5-1, 0, H5-1)))
log.write("red-button bbox: %s\n" % str(bbox5(red, 0, 60, 0, H5-1)))
# tooltip text rows
tb = bbox5(tipbg, 0, W5-1, 0, H5-1)
if tb[0] is not None:
    x0,y0,x1,y1,_ = tb
    log.write("\ntooltip rect = (%d,%d)-(%d,%d) size %dx%d\n" % (x0,y0,x1,y1,x1-x0+1,y1-y0+1))
    for y in range(y0, y1+1):
        n = sum(1 for x in range(x0, x1+1) if min(q[x,y][:3])>=240)
        log.write("  y=%2d white-ink=%d\n" % (y, n))
log.close()
print("done")
