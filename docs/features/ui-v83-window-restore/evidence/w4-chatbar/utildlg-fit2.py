"""B5-2 决定性取证（第 2 版）：找窗框族，只比「框自己」的边带，避开被控件盖住的中部。

对齐搜索：dx in -10..10、dy in 0..6、k = c 段竖向平铺次数 0..4。
计分区：截帧四条边带（上 14 行 / 下 14 行 / 左 14 列 / 右 14 列）——
控件（输入框、下拉箭头、两颗按钮、提示文字）都落在中部，不会污染边带。
只在合成层 alpha==255 的像素上计分。
"""
import io
import sys

sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx
from PIL import Image

OUT = io.open(r"E:/project/_ms_probe/_utildlg_fit2.txt", "w", encoding="utf-8")
nx = Nx("E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx")

shot = Image.open(r"E:/project/MapleStory-Client/docs/features/ui-v83-window-restore/"
                  r"evidence/w4-chatbar/whisper-dlg-native.png").convert("RGB")
SW, SH = shot.size
sp = shot.load()
OUT.write("shot %dx%d\n" % (SW, SH))

EDGE = 14


def in_edge(x, y):
    return x < EDGE or x >= SW - EDGE or y < EDGE or y >= SH - EDGE


EDGEPX = [(x, y, sp[x, y]) for y in range(SH) for x in range(SW) if in_edge(x, y)]
OUT.write("edge pixels=%d\n" % len(EDGEPX))


def in_edge(x, y):
    return x < EDGE or x >= SW - EDGE or y < EDGE or y >= SH - EDGE


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


def build(fam, k):
    t = piece("Basic.img/%s/t" % fam)
    c = piece("Basic.img/%s/c" % fam) if k else None
    s = piece("Basic.img/%s/s" % fam)
    if not (t and s):
        return None
    rows = t[2]
    for _ in range(k):
        if c:
            rows = rows + c[2]
    rows = rows + s[2]
    return t[0], rows


results = []
for fam in ("YesNo2", "YesNo3", "Notice2", "Notice3", "Notice4"):
    for k in range(0, 5):
        b = build(fam, k)
        if not b:
            continue
        fw, rows = b
        fh = len(rows)
        for dy in range(0, 7):
            for dx in range(-10, 11):
                hit = tot = 0
                worst = 0
                for x, y, (sr, sg, sb) in EDGEPX:
                    ry = y + dy
                    if ry >= fh:
                        continue
                    rx = x + dx
                    if rx < 0 or rx >= fw:
                        continue
                    r, g, bb, a = rows[ry][rx]
                    if a != 255:
                        continue
                    tot += 1
                    d = max(abs(r - sr), abs(g - sg), abs(bb - sb))
                    if d <= 8:
                        hit += 1
                    else:
                        worst = max(worst, d)
                if tot >= 4000:
                    results.append((hit / tot, tot, fam, k, dy, dx, worst))

results.sort(reverse=True)
for r in results[:25]:
    OUT.write("match=%.4f 计分=%-6d %-9s k=%d dy=%d dx=%+d 最差差=%d\n" % (r[0], r[1], r[2], r[3], r[4], r[5], r[6]))
OUT.close()
print("written")
