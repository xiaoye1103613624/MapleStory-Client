# r15 s1: background detection + frame bbox + row/col projections for all 5 images.
# Writes UTF-8 report; prints nothing but ascii.
import os, io
from collections import Counter
from PIL import Image

OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
PATHS = {
    "img1": r"C:\Users\11036\Desktop\正确客户端截图\”界面“按钮点击后的列表.png",
    "img2": r"C:\Users\11036\Desktop\正确客户端截图\目录按钮点击后的列表.png",
    "img3": r"C:\Users\11036\Desktop\正确客户端截图\点击“帮助”按钮打开的脚本框.png",
    "img4": r"C:\Users\11036\Desktop\正确客户端截图\关闭快捷键向下时 快捷键处于展开状态.png",
    "img5": r"C:\Users\11036\Desktop\正确客户端截图\打开快捷键，向上时快捷键处于收起状态.png",
}
THRESHOLDS = [1, 4, 8, 16, 24, 32, 48, 64, 96]

def rgbatuple(px):
    return (px[0], px[1], px[2], px[3])

def run(key, path, log):
    im = Image.open(path).convert("RGBA")
    W, H = im.size
    px = im.load()
    log.write("=" * 78 + "\n")
    log.write("%s  file=%s\n" % (key, os.path.basename(path)))
    log.write("size=%dx%d mode=%s\n" % (W, H, im.mode))

    # ---- corners (3x3 block average of RGB, and exact corner pixel) ----
    corners = {"TL": (0, 0), "TR": (W - 1, 0), "BL": (0, H - 1), "BR": (W - 1, H - 1)}
    log.write("corners (exact px then 3x3 mean RGB):\n")
    for name, (cx, cy) in corners.items():
        ex = rgbatuple(px[cx, cy])
        rs = gs = bs = n = 0
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                x, y = cx + dx, cy + dy
                if 0 <= x < W and 0 <= y < H:
                    p = px[x, y]
                    rs += p[0]; gs += p[1]; bs += p[2]; n += 1
        log.write("  %-2s (%d,%d) exact=%s  3x3mean=(%d,%d,%d) n=%d\n" % (
            name, cx, cy, ex, rs // n, gs // n, bs // n, n))

    # ---- colour histogram ----
    cnt = Counter(im.getdata())
    log.write("unique RGBA colours=%d ; total px=%d\n" % (len(cnt), W * H))
    log.write("top 14 colours:\n")
    for c, k in cnt.most_common(14):
        log.write("   %s x %d (%.2f%%)\n" % (c, k, 100.0 * k / (W * H)))

    # ---- candidate background = most common colour among the 4 corner pixels ----
    corner_cols = [px[0, 0], px[W - 1, 0], px[0, H - 1], px[W - 1, H - 1]]
    ccnt = Counter(corner_cols)
    bg = ccnt.most_common(1)[0][0]
    log.write("corner colours=%s -> bg=%s (agreement %d/4)\n" % (
        [tuple(c) for c in corner_cols], tuple(bg), ccnt[bg]))
    histbg = cnt.most_common(1)[0][0]
    log.write("most-common-colour overall=%s\n" % (tuple(histbg),))

    # ---- bbox vs candidate backgrounds at several thresholds ----
    def bbox_against(bgcol, thr, alpha_too=True):
        x0 = y0 = 10 ** 9; x1 = y1 = -1; ink = 0
        for y in range(H):
            for x in range(W):
                p = px[x, y]
                d = abs(p[0] - bgcol[0]) + abs(p[1] - bgcol[1]) + abs(p[2] - bgcol[2])
                if alpha_too:
                    d += abs(p[3] - bgcol[3])
                if d > thr:
                    ink += 1
                    if x < x0: x0 = x
                    if x > x1: x1 = x
                    if y < y0: y0 = y
                    if y > y1: y1 = y
        if x1 < 0:
            return None
        return (x0, y0, x1, y1, x1 - x0 + 1, y1 - y0 + 1, ink)

    for label, col in (("cornerbg", bg), ("globalbg", histbg)):
        log.write("bbox scan against %s=%s (RGB+Alpha L1):\n" % (label, tuple(col)))
        for t in THRESHOLDS:
            bb = bbox_against(col, t)
            if bb is None:
                log.write("   thr=%3d  (no ink)\n" % t)
            else:
                log.write("   thr=%3d  bbox=(%d,%d)-(%d,%d) size=%dx%d ink=%d (%.2f%%)\n" % (
                    t, bb[0], bb[1], bb[2], bb[3], bb[4], bb[5], bb[6],
                    100.0 * bb[6] / (W * H)))
        log.write("\n")

    # ---- alpha channel info ----
    amin = min(p[3] for p in cnt)
    log.write("alpha values present (min=%d): %s\n" % (
        amin, sorted({p[3] for p in cnt})[:20]))
    log.write("\n")

log = io.open(os.path.join(OUTDIR, "_s1_measure.txt"), "w", encoding="utf-8")
for key in sorted(PATHS):
    run(key, PATHS[key], log)
log.close()
print("done")
