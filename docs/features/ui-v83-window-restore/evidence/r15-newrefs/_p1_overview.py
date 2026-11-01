# -*- coding: utf-8 -*-
"""R15 forensic pass 1: size/mode, corner colours, background estimate, frame bbox."""
import io, os, sys, collections
from PIL import Image

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
NAMES = [
    "勋章框.png",
    "好友列表框.png",
    "组员列表框.png",
    "拒绝列表框.png",
    "家族联盟.png",
    "好友、组员、家族、家族联盟、拒绝列表.png",
    "个人请报框.png",
    "学院.png",
]
THRESH = 24  # per-channel max-abs-delta from estimated background


def px(img, x, y):
    return img.getpixel((x, y))


def avg(samples):
    n = len(samples)
    return tuple(round(sum(s[i] for s in samples) / n) for i in range(len(samples[0])))


def report():
    lines = []
    P = lines.append
    for name in NAMES:
        p = os.path.join(SRC, name)
        im = Image.open(p)
        mode0 = im.mode
        rgba = im.convert("RGBA")
        W, H = rgba.size
        P("=" * 78)
        P("FILE: %s" % name)
        P("  native mode=%s  RGBA size=%dx%d  file=%d bytes" % (mode0, W, H, os.path.getsize(p)))

        # --- corners (5x5 blocks) ---
        corners = {}
        for tag, cx, cy in (("TL", 0, 0), ("TR", W - 5, 0), ("BL", 0, H - 5), ("BR", W - 5, H - 5)):
            s = [rgba.getpixel((cx + i, cy + j)) for i in range(5) for j in range(5)]
            corners[tag] = avg(s)
        for tag in ("TL", "TR", "BL", "BR"):
            P("  corner %-2s 5x5avg RGBA%s" % (tag, corners[tag]))

        # --- 1px border ring colour histogram (what surrounds the window?) ---
        ring = []
        for x in range(W):
            ring.append(rgba.getpixel((x, 0)))
            ring.append(rgba.getpixel((x, H - 1)))
        for y in range(H):
            ring.append(rgba.getpixel((0, y)))
            ring.append(rgba.getpixel((W - 1, y)))
        rh = collections.Counter(ring).most_common(6)
        P("  border-ring top colours (RGBA,count): %s" % rh)

        # --- alpha ---
        alphas = collections.Counter(rgba.getchannel("A").getdata()).most_common(5)
        P("  alpha histogram top: %s" % alphas)

        # --- global colour count ---
        cols = collections.Counter(rgba.getdata())
        P("  distinct RGBA colours: %d ; top10: %s"
          % (len(cols), [(c, n) for c, n in cols.most_common(10)]))

        # --- background = most common border-ring colour; bbox of pixels differing from it ---
        bg = rh[0][0]
        minx, miny, maxx, maxy = W, H, -1, -1
        n_out = 0
        for y in range(H):
            for x in range(W):
                r, g, b, a = rgba.getpixel((x, y))
                if max(abs(r - bg[0]), abs(g - bg[1]), abs(b - bg[2])) > THRESH or a != bg[3]:
                    n_out += 1
                    if x < minx: minx = x
                    if x > maxx: maxx = x
                    if y < miny: miny = y
                    if y > maxy: maxy = y
        P("  bg(border-ring modal)=%s  THRESH=%d" % (bg, THRESH))
        if maxx < 0:
            P("  bbox: EMPTY")
        else:
            P("  bbox x[%d..%d] y[%d..%d]  -> W=%d H=%d   (outlier px=%d, %.2f%%)"
              % (minx, maxx, miny, maxy, maxx - minx + 1, maxy - miny + 1, n_out,
                 100.0 * n_out / (W * H)))
            # how many pixels sit ON the bg colour inside the bbox
            P("  bbox offset from image edges: left=%d top=%d right=%d bottom=%d"
              % (minx, miny, W - 1 - maxx, H - 1 - maxy))
        P("")
    return lines


if __name__ == "__main__":
    out = report()
    with io.open(os.path.join(OUT, "_p1_overview.txt"), "w", encoding="utf-8") as f:
        f.write("\n".join(out) + "\n")
    print("OK lines=%d" % len(out))
