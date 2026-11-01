# -*- coding: utf-8 -*-
"""R15 pass 3: dump raw pixel values around the crop edges to explain the 1-px non-palette lines.
'*' marks a pixel whose R,G,B are NOT all multiples of 17 (i.e. off the UI palette)."""
import io, os
from PIL import Image

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
CASES = [
    ("勋章框.png", [(0, 0), (0, 364), (247, 0)]),
    ("组员列表框.png", [(0, 0), (0, 378), (305, 0)]),
    ("拒绝列表框.png", [(0, 0), (0, 383), (302, 0)]),
    ("学院.png", [(0, 0), (0, 384), (219, 0)]),
    ("个人请报框.png", [(0, 0), (0, 189), (503, 0)]),
]


def off(c):
    return (c[0] % 17) or (c[1] % 17) or (c[2] % 17)


def main():
    L = []
    ap = L.append
    for name, anchors in CASES:
        im = Image.open(os.path.join(SRC, name)).convert("RGBA")
        W, H = im.size
        px = im.load()
        ap("=" * 90)
        ap("%s %dx%d" % (name, W, H))
        for ax, ay in anchors:
            if ax >= W or ay >= H:
                continue
            ap("  --- 8x8 raw RGB block at x=%d y=%d ---" % (ax, ay))
            for y in range(ay, min(ay + 8, H)):
                cells = []
                for x in range(ax, min(ax + 8, W)):
                    c = px[x, y]
                    cells.append("%s%3d,%3d,%3d" % ("*" if off(c) else " ", c[0], c[1], c[2]))
                ap("   y=%3d %s" % (y, " ".join(cells)))
            ap("  --- 12 vertical samples at x=%d..%d ---" % (ax, min(ax + 3, W - 1)))
            for y in range(ay, min(ay + 12, H)):
                ap("   y=%3d %s" % (y, "  ".join("x%d=%3d,%3d,%3d%s" %
                    (x, px[x, y][0], px[x, y][1], px[x, y][2], "*" if off(px[x, y]) else " ")
                    for x in range(ax, min(ax + 4, W)))))
    with io.open(os.path.join(OUT, "_p3_edges.txt"), "w", encoding="utf-8") as f:
        f.write("\n".join(L) + "\n")
    print("OK")


if __name__ == "__main__":
    main()
