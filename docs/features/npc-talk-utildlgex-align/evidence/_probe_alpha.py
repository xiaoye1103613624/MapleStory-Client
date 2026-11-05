# -*- coding: utf-8 -*-
"""Measure UtilDlgEx t/c/s canvas alpha from UI.nx (A11c)."""
from collections import Counter
import sys

sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx

nx = Nx(r"E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx")


def child(i, name):
    for c in nx.children(i):
        if nx.name(c) == name:
            return c
    return None


ud = child(child(0, "UIWindow.img"), "UtilDlgEx")
for name in ["t", "c", "s"]:
    n = child(ud, name)
    w, h, bgra = nx.bitmap_pixels(n)
    alphas = bgra[3::4]
    ctr = Counter(alphas)
    total = len(alphas)
    opaque = ctr.get(255, 0)
    zero = ctr.get(0, 0)
    mid = total - opaque - zero
    print(
        "%s %dx%d total=%d opaque255=%.4f zero=%.4f mid=%d top=%s"
        % (
            name,
            w,
            h,
            total,
            opaque / total,
            zero / total,
            mid,
            sorted(ctr.items(), key=lambda kv: -kv[1])[:6],
        )
    )
