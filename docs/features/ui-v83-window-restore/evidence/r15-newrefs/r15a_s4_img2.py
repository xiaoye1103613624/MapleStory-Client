# r15a_s4_img2: calibrate on the button-help tooltip inside img2 (175x290).
# Method: tooltip = the only large near-black region -> luminance threshold.
import os, io, json
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
p = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))["img2"]
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()
log = io.open(os.path.join(OUTDIR, "_d_img2.txt"), "w", encoding="utf-8")
log.write("size=%dx%d mode=RGB(tRNS junk ignored)\n" % (W, H))
log.write("corners TL=%s TR=%s BL=%s BR=%s\n" % (px[0, 0], px[W-1, 0], px[0, H-1], px[W-1, H-1]))

def lum(c):
    return 0.299 * c[0] + 0.587 * c[1] + 0.114 * c[2]

# dark mask (tooltip fill is near-black grey)
TH = 80
mask = [[1 if lum(px[x, y]) < TH else 0 for x in range(W)] for y in range(H)]
# connected components (4-neighbour, iterative)
seen = [[0] * W for _ in range(H)]
comps = []
for y in range(H):
    for x in range(W):
        if mask[y][x] and not seen[y][x]:
            stack = [(x, y)]
            seen[y][x] = 1
            pts = []
            while stack:
                cx, cy = stack.pop()
                pts.append((cx, cy))
                for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                    nx, ny = cx + dx, cy + dy
                    if 0 <= nx < W and 0 <= ny < H and mask[ny][nx] and not seen[ny][nx]:
                        seen[ny][nx] = 1
                        stack.append((nx, ny))
            comps.append(pts)
comps.sort(key=len, reverse=True)
log.write("dark(lum<%d) components: sizes=%s\n" % (TH, [len(c) for c in comps[:6]]))
big = comps[0]
xs = [q[0] for q in big]; ys = [q[1] for q in big]
bx0, bx1, by0, by1 = min(xs), max(xs), min(ys), max(ys)
log.write("tooltip dark-fill bbox x[%d..%d] y[%d..%d] w=%d h=%d npix=%d\n"
          % (bx0, bx1, by0, by1, bx1-bx0+1, by1-by0+1, len(big)))

# slices through the tooltip edges to see border colours
log.write("-- vertical slice x=%d, y=%d..%d (through top/bottom border):\n" % ((bx0+bx1)//2, max(0, by0-4), min(H-1, by1+5)))
m = (bx0 + bx1) // 2
for y in range(max(0, by0 - 4), min(H, by1 + 6)):
    log.write("   y=%3d %s\n" % (y, px[m, y]))
log.write("-- horizontal slice y=%d, x=%d..%d:\n" % ((by0+by1)//2, max(0, bx0-4), min(W-1, bx1+5)))
n = (by0 + by1) // 2
for x in range(max(0, bx0 - 4), min(W, bx1 + 6)):
    log.write("   x=%3d %s\n" % (x, px[x, n]))

# fill samples: interior points far from text (text is white, fill dark grey)
from collections import Counter
cnt = Counter()
for y in range(by0, by1 + 1):
    for x in range(bx0, bx1 + 1):
        c = px[x, y]
        if lum(c) < TH:
            cnt[c] += 1
log.write("fill colour histogram (dark px inside bbox) top8: %s\n" % cnt.most_common(8))
pts = [(bx0+3, by0+3), (bx1-3, by0+3), (bx0+3, by1-3), (bx1-3, by1-3), (m, by0+2), (m, by1-2)]
log.write("interior point samples: %s\n" % [(pt, px[pt[0], pt[1]]) for pt in pts])

# border ring: pixels just outside the dark bbox
ring = Counter()
for x in range(bx0 - 3, bx1 + 4):
    for y in (by0 - 3, by0 - 2, by0 - 1, by1 + 1, by1 + 2, by1 + 3):
        if 0 <= x < W and 0 <= y < H:
            ring[px[x, y]] += 1
for y in range(by0 - 3, by1 + 4):
    for x in (bx0 - 3, bx0 - 2, bx0 - 1, bx1 + 1, bx1 + 2, bx1 + 3):
        if 0 <= x < W and 0 <= y < H:
            ring[px[x, y]] += 1
log.write("ring(1..3 px outside) top10: %s\n" % ring.most_common(10))

# separator lines: rows inside bbox where a non-fill, non-white colour spans >70% width
for y in range(by0, by1 + 1):
    row = [px[x, y] for x in range(bx0, bx1 + 1)]
    c0 = Counter(row).most_common(1)[0]
    if c0[1] > 0.7 * len(row) and lum(c0[0]) > TH:
        log.write("possible separator row y=%d colour=%s count=%d/%d\n" % (y, c0[0], c0[1], len(row)))

# crops for OCR: whole tooltip and text band, 4x NEAREST
crop = im.crop((bx0 - 2, by0 - 2, bx1 + 3, by1 + 3))
crop.resize((crop.width * 4, crop.height * 4), Image.NEAREST).save(os.path.join(OUTDIR, "rgb", "img2_tooltip4x.png"))
im.crop((0, 0, W, 60)).resize((W * 4, 240), Image.NEAREST).save(os.path.join(OUTDIR, "rgb", "img2_top4x.png"))
log.close()
print("OK")
