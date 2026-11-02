"""B5-2 决定性取证（第 5 版）：把全 UI.nx 的位图**左带 17 列**做成指纹索引，
逐行反查悄悄话弹层截帧的每一行来自哪个节点的哪一行。

为什么要反查：前几版按「t + c×k + s」猜拼法，最好也只有 0.88，
差集中在 x5..7 与 x8..16 —— 说明弹层用的框根本不是我以为的那一族。
反查不猜，直接让截帧自己报出处。

索引口径：位图宽 200..400、高 4..200；键 = 左带 17 列按 16 量化后的哈希，
命中后再按通道差<=8 精验；同时允许 bmp 起点 x=0..3 的平移（有些件带左边距）。
"""
import io
import sys
from collections import defaultdict

sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx
from PIL import Image

OUT = io.open(r"E:/project/_ms_probe/_utildlg_lookup.txt", "w", encoding="utf-8")
nx = Nx("E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx")
shot = Image.open(r"E:/project/MapleStory-Client/docs/features/ui-v83-window-restore/"
                  r"evidence/w4-chatbar/whisper-dlg-native.png").convert("RGB")
SW, SH = shot.size
sp = shot.load()
BAND = 17
X0 = 3          # 位图最左 0..2 列是半透明投影（alpha 34/102/136），不能当指纹用
SHIFTS = (0, 1, 2, 3)

index = defaultdict(list)
count_bm = 0


def add(path, i):
    global count_bm
    w, h, bgra = nx.bitmap_pixels(i)
    if w is None or not (200 <= w <= 400) or not (4 <= h <= 200):
        return
    count_bm += 1
    for y in range(h):
        r0 = y * w * 4
        for sh in SHIFTS:
            if sh + X0 + BAND > w:
                break
            px = [(bgra[r0 + (sh + X0 + x) * 4 + 2], bgra[r0 + (sh + X0 + x) * 4 + 1],
                   bgra[r0 + (sh + X0 + x) * 4], bgra[r0 + (sh + X0 + x) * 4 + 3]) for x in range(BAND)]
            if any(p[3] != 255 for p in px):
                continue
            key = tuple((p[0] >> 4) * 289 + (p[1] >> 4) * 17 + (p[2] >> 4) for p in px)
            index[key].append((path, y, sh, px))


def walk(i, path):
    for c in nx.children(i):
        cn = nx.name(c)
        if cn == "origin":
            continue
        p = path + "/" + cn
        _, _, _, nt, _, _, _ = nx.node(c)
        if nt == 5:
            add(p, c)
        walk(c, p)


walk(0, "")
OUT.write("indexed bitmaps=%d  keys=%d\n\n" % (count_bm, len(index)))

for y in range(SH):
    target = [sp[X0 + x, y] for x in range(BAND)]
    hits = []
    for sh in SHIFTS:
        key = tuple((t[0] >> 4) * 289 + (t[1] >> 4) * 17 + (t[2] >> 4) for t in target)
        for path, ry, ish, px in index.get(key, []):
            if ish != sh:
                continue
            if all(max(abs(a[0] - b[0]), abs(a[1] - b[1]), abs(a[2] - b[2])) <= 8
                   for a, b in zip(px, target)):
                hits.append("%s!r%d@x%d" % (path, ry, sh))
    OUT.write("shot y=%-3d %s\n" % (y, "  ".join(sorted(set(hits))[:8]) if hits else "(无命中)"))
OUT.close()
print("written")
