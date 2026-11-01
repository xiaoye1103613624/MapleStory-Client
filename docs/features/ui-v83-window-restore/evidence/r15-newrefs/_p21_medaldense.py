# -*- coding: utf-8 -*-
"""Dense full-template verification for the two medal-window frame candidates.

For each candidate node bitmap we place it at the ring-best offset and compare
EVERY template pixel whose alpha == 255 that lands inside the screenshot
(not just the 4 px ring).  Reports MAE and the fraction of pixels within
|delta| <= 10 / 20 / 40 per channel-max.
"""
import io
import os
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, r"E:\project\MapleStory-Client\tools")
from nx_inspect import Nx  # noqa: E402

NX = r"E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx"
SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "_p21_medaldense.txt")

SHOTS = {
    "medal": ("\u52cb\u7ae0\u6846.png", [
        ("UIWindow.img/Title/backgrnd", 0, 1),
        ("UIWindow.img/Maker/backgrnd", -12, 1),
        ("UIWindow.img/Quest/backgrnd2", -5, 1),
        ("UIWindow.img/MapleEvent/backgrnd", -16, 1),
        ("UIWindow.img/UserInfo/backgrnd7", -15, 1),
        ("UIWindow.img/Title/backgrnd3", -20, 0),
        ("UIWindow.img/Title/backgrnd5", -20, 0),
    ]),
    "academy": ("\u5b66\u9662.png", [
        ("UIWindow.img/Family/backgrnd", 1, -1),
        ("UIWindow.img/Title/backgrnd4", -38, 0),
    ]),
    "rightwin": ("\u4e2a\u4eba\u8bf7\u62a5\u6846.png", [
        ("UIWindow.img/UserInfo/backgrnd4", 270, -1),
        ("UIWindow.img/UserInfo/backgrnd3", 270, -1),
    ]),
}


def bgra_to_rgba(w, h, raw):
    a = np.frombuffer(raw, dtype=np.uint8).reshape(h, w, 4)
    return a[:, :, [2, 1, 0, 3]].copy()


def main():
    nx = Nx(NX)
    L = []
    for stem, (fn, cands) in SHOTS.items():
        img = np.array(Image.open(os.path.join(SRC, fn)).convert("RGB")).astype(np.int32)
        H, Iw = img.shape[:2]
        L.append("=" * 74)
        L.append("%s  %s  %dx%d" % (stem, fn, Iw, H))
        L.append("=" * 74)
        for path, dx, dy in cands:
            nid = nx.find(path)
            if nid is None:
                L.append("  MISS %s" % path)
                continue
            w, h, raw = nx.bitmap_pixels(nid)
            t = bgra_to_rgba(w, h, raw)
            m = t[:, :, 3] == 255
            ys, xs = np.nonzero(m)
            ii = ys + dy
            jj = xs + dx
            ok = (ii >= 0) & (ii < H) & (jj >= 0) & (jj < Iw)
            n = int(ok.sum())
            if n == 0:
                L.append("  %-40s no pixel inside" % path)
                continue
            d = np.abs(img[ii[ok], jj[ok], :3] - t[ys[ok], xs[ok], :3]).max(axis=1)
            L.append("  %-40s %dx%d dx=%+d dy=%+d  n=%d/%d (%.1f%% of opaque)"
                     % (path, w, h, dx, dy, n, int(m.sum()), 100.0 * n / m.sum()))
            L.append("      MAE=%.4f  <=10:%.4f  <=20:%.4f  <=40:%.4f  >40:%d"
                     % (float(np.abs(img[ii[ok], jj[ok], :3] - t[ys[ok], xs[ok], :3]).mean()),
                        float((d <= 10).mean()), float((d <= 20).mean()),
                        float((d <= 40).mean()), int((d > 40).sum())))
    with io.open(OUT, "w", encoding="utf-8") as f:
        f.write("\n".join(L) + "\n")
    print("wrote", OUT)


if __name__ == "__main__":
    main()
