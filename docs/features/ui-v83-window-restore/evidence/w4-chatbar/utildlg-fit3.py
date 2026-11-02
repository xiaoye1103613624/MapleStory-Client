"""B5-2 窗框定位（第 3 版）：在最佳对齐 (YesNo3/Notice3, k=1, dy=1, dx=0) 下分段看差异。

输出：
 1) 四条边带各自的 match 率（上/下/左/右）；
 2) x=130 一条列的 shot vs 合成 逐行对照（只看全不透明处）；
 3) y=110 一条行的对照。
"""
import io
import sys

sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx
from PIL import Image

OUT = io.open(r"E:/project/_ms_probe/_utildlg_fit3.txt", "w", encoding="utf-8")
nx = Nx("E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx")
shot = Image.open(r"E:/project/MapleStory-Client/docs/features/ui-v83-window-restore/"
                  r"evidence/w4-chatbar/whisper-dlg-native.png").convert("RGB")
SW, SH = shot.size
sp = shot.load()


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
    return w, h, rows


def build(fam, k, skip):
    t = piece("Basic.img/%s/t" % fam)
    s = piece("Basic.img/%s/s" % fam)
    c = piece("Basic.img/%s/c" % fam)
    rows = t[2][skip:]
    for _ in range(k):
        rows = rows + c[2]
    rows = rows + s[2]
    return t[0], rows


for fam, k, dy in (("YesNo3", 1, 1), ("Notice4", 0, 1), ("Notice3", 2, 1)):
    fw, rows = build(fam, k, dy)
    fh = len(rows)
    bands = {"top": (0, 0, SW, 14), "bottom": (0, SH - 14, SW, SH),
             "left": (0, 0, 14, SH), "right": (SW - 14, 0, SW, SH)}
    OUT.write("\n=== %s k=%d dy=%d  合成 %dx%d ===\n" % (fam, k, dy, fw, fh))
    for name, (x0, y0, x1, y1) in bands.items():
        hit = tot = 0
        for y in range(y0, y1):
            if y >= fh:
                continue
            for x in range(x0, min(x1, fw)):
                r, g, b, a = rows[y][x]
                if a != 255:
                    continue
                tot += 1
                sr, sg, sb = sp[x, y]
                if max(abs(r - sr), abs(g - sg), abs(b - sb)) <= 8:
                    hit += 1
        OUT.write("  %-7s match=%s/%s = %.3f\n" % (name, hit, tot, hit / tot if tot else 0))
    OUT.write("  列 x=130 对照（shot | 合成，标 * 为差>8）：\n")
    for y in range(SH):
        if y >= fh:
            break
        r, g, b, a = rows[y][130]
        if a != 255:
            continue
        sr, sg, sb = sp[130, y]
        d = max(abs(r - sr), abs(g - sg), abs(b - sb))
        OUT.write("    y=%-3d shot=(%3d,%3d,%3d) comp=(%3d,%3d,%3d) %s\n"
                  % (y, sr, sg, sb, r, g, b, "*" if d > 8 else ""))
OUT.close()
print("written")
