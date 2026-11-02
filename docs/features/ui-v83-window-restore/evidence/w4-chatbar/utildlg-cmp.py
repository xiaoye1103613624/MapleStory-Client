"""B5-2 候选窗框比对：把 YesNo*/Notice*/UtilDlgEx 的 t/c/s 落 PNG，并打不透明包围盒 +
逐行中心色，直接和悄悄话弹层截帧（261x117）的 §1 读数对齐。

判据：截图 261 宽若 = 位图 266 宽裁掉左右透明边，则包围盒宽应正好 261；
「t + c*k + s」三段行数相加若 = 117，就是同一张框。
"""
import io
import sys

sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx, write_png

OUT = io.open(r"E:/project/_ms_probe/_utildlg_cmp.txt", "w", encoding="utf-8")
PNGDIR = "E:/project/_ms_probe/utildlg_png"
import os
os.makedirs(PNGDIR, exist_ok=True)

nx = Nx("E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx")


def kids(i):
    return [c for c in nx.children(i)]


def child(i, name):
    for c in kids(i):
        if nx.name(c) == name:
            return c
    return None


def stats(i, label):
    w, h, bgra = nx.bitmap_pixels(i)
    if w is None:
        OUT.write("%s NOT BITMAP\n" % label)
        return None
    xs, ys = [], []
    for y in range(h):
        for x in range(w):
            a = bgra[(y * w + x) * 4 + 3]
            if a:
                xs.append(x)
                ys.append(y)
    if not xs:
        OUT.write("%s FULLY TRANSPARENT\n" % label)
        return None
    x0, x1, y0, y1 = min(xs), max(xs), min(ys), max(ys)
    out = "%-34s bmp=%dx%d  opaque=%dx%d (x %d..%d, y %d..%d) origin=%s\n" % (
        label, w, h, x1 - x0 + 1, y1 - y0 + 1, x0, x1, y0, y1,
        nx.value(child(i, "origin")) if child(i, "origin") is not None else None)
    # 中心列逐行色（与 rows 取证同口径）
    cx = (x0 + x1) // 2
    rows = []
    for y in range(y0, y1 + 1):
        p = (y * w + cx) * 4
        b, g, r, a = bgra[p], bgra[p + 1], bgra[p + 2], bgra[p + 3]
        rows.append("y%d=(%d,%d,%d,%d)" % (y - y0, r, g, b, a))
    out += "   centercol: " + " ".join(rows) + "\n"
    OUT.write(out)
    write_png("%s/%s.png" % (PNGDIR, label.replace("/", "_")), w, h, bgra)
    return x1 - x0 + 1, y1 - y0 + 1


for fam in ("YesNo2", "YesNo3", "YesNo4", "Notice2", "Notice3", "Notice4"):
    base = nx.find("Basic.img/" + fam)
    if base is None:
        OUT.write("%s MISSING\n" % fam)
        continue
    OUT.write("=== Basic.img/%s ===\n" % fam)
    for c in kids(base):
        n = nx.name(c)
        if n == "origin":
            continue
        _, _, _, ntype, _, _, _ = nx.node(c)
        if ntype == 5:
            stats(c, "Basic.img/%s/%s" % (fam, n))
        else:
            for st in kids(c):
                _, _, _, nt2, _, _, _ = nx.node(st)
                if nt2 == 5:
                    stats(st, "Basic.img/%s/%s/%s" % (fam, n, nx.name(st)))

OUT.write("\n=== UtilDlgEx t/c/s ===\n")
for n in ("t", "c", "s", "it", "ic", "is", "notice", "bar", "line"):
    c = child(nx.find("UIWindow.img/UtilDlgEx"), n)
    if c is not None:
        stats(c, "UIWindow.img/UtilDlgEx/%s" % n)

OUT.close()
print("written")
