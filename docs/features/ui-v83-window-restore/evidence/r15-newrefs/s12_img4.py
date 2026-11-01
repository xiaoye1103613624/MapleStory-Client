# r15 s12: img4 (509x86 whole-client crop) -> shortcut window frame rect + slot grid.
import os, io
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
P4 = r"C:\Users\11036\Desktop\正确客户端截图\关闭快捷键向下时 快捷键处于展开状态.png"
im = Image.open(P4).convert("RGBA"); W, H = im.size; px = im.load()
log = io.open(os.path.join(OUTDIR, "_s12_img4.txt"), "w", encoding="utf-8")
log.write("img4 %dx%d (whole-client crop)\n" % (W, H))

def runs_along_h(y, pred, x0=0, x1=None):
    if x1 is None: x1 = W - 1
    out = []; s = None
    for x in range(x0, x1 + 1):
        if pred(px[x, y]) and s is None: s = x
        if not pred(px[x, y]) and s is not None:
            out.append((s, x - 1, x - s)); s = None
    if s is not None: out.append((s, x1, x1 - s + 1))
    return out
def runs_along_v(x, pred, y0=0, y1=None):
    if y1 is None: y1 = H - 1
    out = []; s = None
    for y in range(y0, y1 + 1):
        if pred(px[x, y]) and s is None: s = y
        if not pred(px[x, y]) and s is not None:
            out.append((s, y - 1, y - s)); s = None
    if s is not None: out.append((s, y1, y1 - s + 1))
    return out

white = lambda p: min(p[:3]) >= 235
black = lambda p: p[0] + p[1] + p[2] < 120
slotbg = lambda p: abs(p[0] - 119) < 14 and abs(p[1] - 136) < 14 and abs(p[2] - 153) < 14

log.write("\n### window frame: white bevel / black border projections\n")
colw = [sum(1 for y in range(H) if white(px[x, y])) for x in range(W)]
roww = [sum(1 for x in range(W) if white(px[x, y])) for y in range(H)]
cold = [sum(1 for y in range(H) if black(px[x, y])) for x in range(W)]
rowd = [sum(1 for x in range(W) if black(px[x, y])) for y in range(H)]
log.write(" cols with white>=%d : %s\n" % (H * 0.6, [x for x in range(W) if colw[x] >= H * 0.6]))
log.write(" cols with black>=%d : %s\n" % (H * 0.6, [x for x in range(W) if cold[x] >= H * 0.6]))
log.write(" rows with white>=%d : %s\n" % (W * 0.6, [y for y in range(H) if roww[y] >= W * 0.6]))
log.write(" rows with black>=%d : %s\n" % (W * 0.6, [y for y in range(H) if rowd[y] >= W * 0.6]))

log.write("\n### vertical structure at x=250 (panel interior column)\n")
log.write(" white runs: %s\n" % runs_along_v(250, white, 0, H - 1))
log.write(" black runs: %s\n" % runs_along_v(250, black, 0, H - 1))
log.write(" slotbg runs: %s\n" % runs_along_v(250, slotbg, 0, H - 1))
for x in (60, 100, 200, 300, 400, 460, 500):
    log.write(" x=%3d white=%s black=%s\n" % (x, runs_along_v(x, white, 8, 85), runs_along_v(x, black, 8, 85)))

log.write("\n### horizontal slot runs (white slot faces) at several y\n")
for y in (16, 22, 30, 40, 46, 50, 58, 66, 74, 78):
    r = [q for q in runs_along_h(y, white, 45, W - 1) if q[2] >= 6]
    log.write(" y=%2d : n=%d %s\n" % (y, len(r), r))

log.write("\n### black vertical separators at y=30 and y=66\n")
for y in (14, 30, 47, 48, 66, 79):
    r = [q for q in runs_along_h(y, black, 45, W - 1) if q[2] >= 2]
    log.write(" y=%2d : %s\n" % (y, r))
log.close()
print("done")
