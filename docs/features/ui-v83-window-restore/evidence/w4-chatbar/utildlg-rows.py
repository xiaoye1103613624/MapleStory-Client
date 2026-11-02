"""B5-2 逐行指纹：悄悄话弹层的竖向拼法到底是 t / c×k / s 的哪些行。

方法：对截帧每一行 y，只用**左侧 21 列**（x 0..20，避开中部的输入框、下拉、按钮）
去 Basic.img/YesNo3 与 Notice4 的 t/c/s 里找全等（通道差<=8）的行，
列出所有命中，再由人工读出序列。

同时验证左沿对齐：顶带在 dx=0 已经 3090/3090 全等，这里不再搜 dx。
"""
import io
import sys

sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx
from PIL import Image

OUT = io.open(r"E:/project/_ms_probe/_utildlg_rows.txt", "w", encoding="utf-8")
nx = Nx("E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx")
shot = Image.open(r"E:/project/MapleStory-Client/docs/features/ui-v83-window-restore/"
                  r"evidence/w4-chatbar/whisper-dlg-native.png").convert("RGB")
SW, SH = shot.size
sp = shot.load()
X0, X1 = 0, 21


def piece(path):
    i = nx.find(path)
    if i is None:
        return None
    w, h, bgra = nx.bitmap_pixels(i)
    if w is None:
        return None
    rows = []
    for y in range(h):
        r0 = y * w * 4
        rows.append([(bgra[r0 + x * 4 + 2], bgra[r0 + x * 4 + 1], bgra[r0 + x * 4],
                      bgra[r0 + x * 4 + 3]) for x in range(w)])
    return path.split("/")[-1], rows


LIB = []
for fam in ("YesNo3", "Notice4"):
    for n in ("t", "c", "s", "s_line"):
        p = "Basic.img/%s/%s" % (fam, n)
        got = piece(p)
        if got:
            for ri, row in enumerate(got[1]):
                LIB.append(("%s/%s" % (fam, n), ri, len(got[1]), row))

OUT.write("lib rows=%d\n" % len(LIB))
prev = None
for y in range(SH):
    target = [sp[x, y] for x in range(X0, X1)]
    hits = []
    for name, ri, rh, row in LIB:
        seg = row[X0:X1]
        cmp_n = 0
        ok = True
        for (r, g, b, a), (sr, sg, sb) in zip(seg, target):
            if a != 255:
                continue  # 位图那格是透明（圆角/阴影），不比
            cmp_n += 1
            if max(abs(r - sr), abs(g - sg), abs(b - sb)) > 8:
                ok = False
                break
        if not ok or cmp_n < 12:
            continue
        hits.append("%s!r%d/%d" % (name, ri, rh))
    OUT.write("y=%-3d %s\n" % (y, "  ".join(hits) if hits else "(no exact row)"))
OUT.close()
print("written")
