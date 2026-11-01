"""W10 measurement helper (read-only). Writes UTF-8 report; never prints CJK to stdout."""
import os
import sys
from collections import Counter

from PIL import Image

BASE = os.path.dirname(os.path.abspath(__file__))
PNG = os.path.join(BASE, "..", "docs", "features", "ui-v83-window-restore",
                   "evidence", "w10-quest", "pngs")
SHOT = r"C:\Users\11036\Desktop\正确客户端截图\任务栏.png"
OUT = os.path.join(BASE, "..", "docs", "features", "ui-v83-window-restore",
                   "evidence", "w10-quest", "_measure.txt")
out = open(OUT, "w", encoding="utf-8")


def w(*a):
    print(*a, file=out)


def stats(fp):
    im = Image.open(fp).convert("RGBA")
    px = im.load()
    W, H = im.size
    opaque = [(x, y) for y in range(H) for x in range(W) if px[x, y][3] > 16]
    w("== %s  %dx%d  opaque=%d (%.1f%%)" % (os.path.basename(fp), W, H, len(opaque),
                                            100.0 * len(opaque) / (W * H)))
    if opaque:
        xs = [p[0] for p in opaque]
        ys = [p[1] for p in opaque]
        w("   opaque bbox: x %d..%d  y %d..%d" % (min(xs), max(xs), min(ys), max(ys)))
        c = Counter(px[x, y][:3] for x, y in opaque)
        w("   top colors:", c.most_common(5))
    return im


def rowbands(im, x):
    """column scan at x: report y-ranges of identical color (band structure)."""
    px = im.load()
    W, H = im.size
    prev = None
    start = 0
    w("-- vertical bands along column x=%d (%dx%d)" % (x, W, H))
    for y in range(H):
        c = px[x, y][:3]
        if c != prev:
            if prev is not None and y - start >= 1:
                w("   y %3d..%3d  %s" % (start, y - 1, prev))
            prev = c
            start = y
    w("   y %3d..%3d  %s" % (start, H - 1, prev))


w("### assets")
for n in ["UIWindow.img_Quest_Tab_disabled_0.png", "UIWindow.img_Quest_Tab_disabled_1.png",
          "UIWindow.img_Quest_Tab_disabled_2.png", "UIWindow.img_Quest_Tab_enabled_0.png",
          "UIWindow.img_Quest_Tab_enabled_1.png", "UIWindow.img_Quest_Tab_enabled_2.png",
          "UIWindow.img_Quest_icon0.png", "UIWindow.img_Quest_icon1.png",
          "UIWindow.img_Quest_icon4.png", "UIWindow.img_Quest_icon2_0.png",
          "UIWindow.img_Quest_icon3_0.png", "UIWindow.img_Quest_icon5_0.png",
          "UIWindow.img_Quest_icon6_0.png", "UIWindow.img_Quest_icon7_0.png",
          "UIWindow.img_Quest_icon8_0.png", "UIWindow.img_Quest_icon9_0.png",
          "UIWindow.img_Quest_Gauge_frame.png", "UIWindow.img_Quest_Gauge_gauge.png",
          "UIWindow.img_Quest_Gauge_spot.png", "UIWindow.img_Quest_Gauge2_frame.png",
          "UIWindow.img_BtUIClose_normal_0.png", "UIWindow.img_BtUIClose2_normal_0.png",
          "Basic.img_BtClose2_normal_0.png"]:
    fp = os.path.join(PNG, n)
    if os.path.exists(fp):
        stats(fp)
    else:
        w("MISSING", n)

w("")
w("### backgrnd band structure")
bg = Image.open(os.path.join(PNG, "UIWindow.img_Quest_backgrnd.png")).convert("RGBA")
rowbands(bg, 4)
rowbands(bg, 120)
rowbands(bg, 240)

bg2 = Image.open(os.path.join(PNG, "UIWindow.img_Quest_backgrnd2.png")).convert("RGBA")
w("")
w("### backgrnd2 band structure")
rowbands(bg2, 4)
rowbands(bg2, 150)
rowbands(bg2, 300)

w("")
w("### reference screenshot")
sh = Image.open(SHOT).convert("RGBA")
w("size", sh.size)
sh.save(os.path.join(PNG, "ref-quest-full.png"))
