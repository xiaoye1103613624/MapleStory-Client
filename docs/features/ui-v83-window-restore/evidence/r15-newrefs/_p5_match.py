# -*- coding: utf-8 -*-
"""R15 pass 5: locate each window frame inside its screenshot by template matching the
exported UI.nx `backgrnd` bitmap, and quantify the pixel diff.

Two metrics per offset (dx,dy) = where template pixel (0,0) lands in the screenshot:
  FULL  = mean per-pixel sum(|dR|+|dG|+|dB|) over template pixels that are FULLY OPAQUE.
          (inflated, because the screenshot paints tabs/rows/buttons on top of the frame)
  RING  = same, restricted to a 4-px band hugging the template's opaque silhouette,
          i.e. pure frame art that nothing else covers -> the trustworthy alignment score.
"""
import io, os
import numpy as np
from PIL import Image

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
NX = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs\_nx"
OUT = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"

JOBS = [
    ("勋章框.png", ["Title_backgrnd.png"]),
    ("好友列表框.png", ["UserList_backgrnd.png", "UserList_backgrnd2.png", "UserList_backgrnd3.png"]),
    ("组员列表框.png", ["UserList_backgrnd.png", "UserList_backgrnd2.png"]),
    ("拒绝列表框.png", ["UserList_backgrnd.png", "UserList_backgrnd2.png"]),
    ("家族联盟.png", ["UserList_backgrnd.png", "UserList_backgrnd2.png"]),
    ("好友、组员、家族、家族联盟、拒绝列表.png", ["UserList_backgrnd.png", "UserList_backgrnd2.png"]),
    ("个人请报框.png", ["UserInfo_backgrnd.png", "UserInfo_backgrnd5.png"]),
    ("学院.png", ["Family_backgrnd.png"]),
]
RNG = range(-14, 15)


def load(path):
    return np.asarray(Image.open(path).convert("RGBA"), dtype=np.uint8)


def opaque_bbox(a):
    al = a[..., 3]
    ys, xs = np.nonzero(al >= 250)
    return int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())


def ring_mask(a, obb, band=4):
    x0, y0, x1, y1 = obb
    m = np.zeros(a.shape[:2], dtype=bool)
    m[y0:y0 + band, x0:x1 + 1] = True          # top
    m[y1 - band + 1:y1 + 1, x0:x1 + 1] = True  # bottom
    m[y0:y1 + 1, x0:x0 + band] = True          # left
    m[y0:y1 + 1, x1 - band + 1:x1 + 1] = True  # right
    m &= (a[..., 3] >= 250)
    return m


def score(img, tpl, mask, dx, dy):
    H, W = img.shape[:2]
    th, tw = tpl.shape[:2]
    x0, x1 = max(0, dx), min(W, dx + tw)
    y0, y1 = max(0, dy), min(H, dy + th)
    if x1 <= x0 or y1 <= y0:
        return None, 0
    si = img[y0:y1, x0:x1, :3].astype(np.int16)
    st = tpl[y0 - dy:y1 - dy, x0 - dx:x1 - dx, :3].astype(np.int16)
    sm = mask[y0 - dy:y1 - dy, x0 - dx:x1 - dx]
    n = int(sm.sum())
    if n == 0:
        return None, 0
    d = np.abs(si - st).sum(axis=2)
    return float(d[sm].mean()), n


def best(img, tpl, mask, rng=RNG):
    out = []
    for dy in rng:
        for dx in rng:
            s, n = score(img, tpl, mask, dx, dy)
            if s is not None:
                out.append((s, dx, dy, n))
    out.sort()
    return out


def main():
    L = []
    ap = L.append
    for shot, tpls in JOBS:
        img = load(os.path.join(SRC, shot))
        H, W = img.shape[:2]
        ap("=" * 92)
        ap("SCREENSHOT %s   %dx%d" % (shot, W, H))
        for t in tpls:
            tp = os.path.join(NX, t)
            if not os.path.exists(tp):
                ap("   template %s MISSING" % t)
                continue
            a = load(tp)
            th, tw = a.shape[:2]
            obb = opaque_bbox(a)
            op = int((a[..., 3] >= 250).sum())
            ap("  template %-26s bitmap %dx%d  opaque(>=250) bbox x[%d..%d] y[%d..%d] = %dx%d  opaque px=%d"
               % (t, tw, th, obb[0], obb[2], obb[1], obb[3],
                  obb[2] - obb[0] + 1, obb[3] - obb[1] + 1, op))
            rm = ring_mask(a, obb, 4)
            fb = best(img, a, a[..., 3] >= 250)
            rb = best(img, a, rm)
            s, dx, dy, n = fb[0]
            ap("     FULL-mask best: dx=%+d dy=%+d MAE=%.2f (n=%d) ; runner-ups %s"
               % (dx, dy, s, n, ["(%+d,%+d)=%.2f" % (b[1], b[2], b[0]) for b in fb[1:4]]))
            s2, dx2, dy2, n2 = rb[0]
            ap("     RING-mask best: dx=%+d dy=%+d MAE=%.2f (n=%d) ; runner-ups %s"
               % (dx2, dy2, s2, n2, ["(%+d,%+d)=%.2f" % (b[1], b[2], b[0]) for b in rb[1:4]]))
            # window rect implied by the RING alignment
            wx0, wy0 = dx2, dy2
            wx1, wy1 = dx2 + tw - 1, dy2 + th - 1
            ap("     -> template outer rect in screenshot: x[%d..%d] y[%d..%d] = %dx%d"
               % (wx0, wx1, wy0, wy1, tw, th))
            ox0, oy0, ox1, oy1 = obb
            ap("     -> OPAQUE frame rect in screenshot:    x[%d..%d] y[%d..%d] = %dx%d"
               % (wx0 + ox0, wx1 and wx0 + ox1, wy0 + oy0, wy0 + oy1,
                  ox1 - ox0 + 1, oy1 - oy0 + 1))
            ap("     -> visible inside image (clipped):     x[%d..%d] y[%d..%d]"
               % (max(0, wx0), min(W - 1, wx1), max(0, wy0), min(H - 1, wy1)))
        ap("")
    with io.open(os.path.join(OUT, "_p5_match.txt"), "w", encoding="utf-8") as f:
        f.write("\n".join(L) + "\n")
    print("OK")


if __name__ == "__main__":
    main()
