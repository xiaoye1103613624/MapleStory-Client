# r15 s8: run-length scans of img3 to locate frame, panels, text area, buttons.
import os, io
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
P3 = r"C:\Users\11036\Desktop\正确客户端截图\点击“帮助”按钮打开的脚本框.png"
im = Image.open(P3).convert("RGBA"); W, H = im.size; px = im.load()
BUCKET = 6  # quantise so anti-aliased noise collapses

def key(p):
    return (p[0] // BUCKET, p[1] // BUCKET, p[2] // BUCKET)

def rle_h(y, x0=0, x1=None):
    if x1 is None: x1 = W - 1
    out = []; s = x0; cur = key(px[x0, y])
    for x in range(x0 + 1, x1 + 1):
        k = key(px[x, y])
        if k != cur:
            out.append((s, x - 1, cur)); s = x; cur = k
    out.append((s, x1, cur))
    return out

def rle_v(x, y0=0, y1=None):
    if y1 is None: y1 = H - 1
    out = []; s = y0; cur = key(px[x, y0])
    for y in range(y0 + 1, y1 + 1):
        k = key(px[x, y])
        if k != cur:
            out.append((s, y - 1, cur)); s = y; cur = k
    out.append((s, y1, cur))
    return out

log = io.open(os.path.join(OUTDIR, "_s8_img3_rle.txt"), "w", encoding="utf-8")
log.write("img3 %dx%d  RLE bucket=%d (colour = bucket*%d approx)\n" % (W, H, BUCKET, BUCKET))
log.write("\n#### HORIZONTAL scans (start,end,colourbucket,realRGB at start)\n")
for y in (0, 1, 2, 3, 4, 6, 8, 30, 60, 100, 133, 140, 150, 160, 180, 200, 220, 240, 250, 258, 262, 266, 270, 280, 290, 300, 305, 308, 310, 312):
    log.write("y=%3d: " % y)
    for s, e, k in rle_h(y):
        log.write("[%d..%d]%s " % (s, e, px[s, y][:3]))
    log.write("\n")
log.write("\n#### VERTICAL scans\n")
for x in (0, 1, 2, 3, 6, 10, 20, 40, 80, 120, 140, 148, 160, 170, 200, 300, 400, 470, 500, 510, 515, 520, 522, 523):
    log.write("x=%3d: " % x)
    for s, e, k in rle_v(x):
        log.write("[%d..%d]%s " % (s, e, px[x, s][:3]))
    log.write("\n")
log.close()
print("done")
