# -*- coding: utf-8 -*-
"""R15 pass 2: test the 'mod-17 palette' discriminator between UI art and game scene.

MapleStory UI packs are 8-bit-ish: flat colours are multiples of 17 (0,17,...,255).
Compressed/3D scene pixels are not. Measure per-image agreement and dump masks.
"""
import io, os, collections
from PIL import Image

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
NAMES = [
    "勋章框.png", "好友列表框.png", "组员列表框.png", "拒绝列表框.png",
    "家族联盟.png", "好友、组员、家族、家族联盟、拒绝列表.png", "个人请报框.png", "学院.png",
]


def is17(c):
    return (c[0] % 17 == 0) and (c[1] % 17 == 0) and (c[2] % 17 == 0)


def main():
    L = []
    P = L.append
    for name in NAMES:
        im = Image.open(os.path.join(SRC, name)).convert("RGBA")
        W, H = im.size
        px = im.load()
        mask = Image.new("L", (W, H), 0)
        mp = mask.load()
        tot = 0
        hit = 0
        for y in range(H):
            for x in range(W):
                tot += 1
                if is17(px[x, y]):
                    hit += 1
                    mp[x, y] = 255
        P("=" * 70)
        P("%s  %dx%d   mod17-exact pixels: %d/%d = %.2f%%" % (name, W, H, hit, tot, 100.0 * hit / tot))
        # edge profile: fraction of mod17 along the first/last 8 rows and cols
        # top rows
        for y in range(0, min(8, H)):
            c = sum(1 for x in range(W) if is17(px[x, y]))
            P("   row %3d mod17 = %5d/%5d (%.1f%%)" % (y, c, W, 100.0 * c / W))
        for y in range(max(0, H - 8), H):
            c = sum(1 for x in range(W) if is17(px[x, y]))
            P("   row %3d mod17 = %5d/%5d (%.1f%%)" % (y, c, W, 100.0 * c / W))
        for x in range(0, min(8, W)):
            c = sum(1 for y in range(H) if is17(px[x, y]))
            P("   col %3d mod17 = %5d/%5d (%.1f%%)" % (x, c, H, 100.0 * c / H))
        for x in range(max(0, W - 8), W):
            c = sum(1 for y in range(H) if is17(px[x, y]))
            P("   col %3d mod17 = %5d/%5d (%.1f%%)" % (x, c, H, 100.0 * c / H))
        mask.save(os.path.join(OUT, "_p2_mask_%s.png" % name.replace(".png", "").replace("、", "_")))
    with io.open(os.path.join(OUT, "_p2_mod17.txt"), "w", encoding="utf-8") as f:
        f.write("\n".join(L) + "\n")
    print("OK")


if __name__ == "__main__":
    main()
