"""B5-2 决定性取证：悄悄话弹层的窗框到底是哪一族位图。

做法：把 Basic.img/YesNo*/Notice* 的 t / c(平铺 k 次) / s 按原生拼法合成，
再和截帧 whisper-dlg-native.png（261x117）逐像素比对；只在合成层**全不透明**
的像素上计分（半透明阴影底下是什么背景不知道，比了也不算证据）。

判据：match 比例 >= 0.98 且通道最大差 <= 8 ⇒ 认定同一张框。
"""
import io
import sys

sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx
from PIL import Image

OUT = io.open(r"E:/project/_ms_probe/_utildlg_fit.txt", "w", encoding="utf-8")
nx = Nx("E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx")

shot = Image.open(r"E:/project/MapleStory-Client/docs/features/ui-v83-window-restore/"
                  r"evidence/w4-chatbar/whisper-dlg-native.png").convert("RGB")
SW, SH = shot.size
sp = shot.load()
OUT.write("shot %dx%d\n" % (SW, SH))


def piece(path):
    i = nx.find(path)
    if i is None:
        return None
    w, h, bgra = nx.bitmap_pixels(i)
    if w is None:
        return None
    px = []
    for y in range(h):
        row = []
        for x in range(w):
            p = (y * w + x) * 4
            row.append((bgra[p + 2], bgra[p + 1], bgra[p], bgra[p + 3]))
        px.append(row)
    return w, h, px


def family(fam):
    t = piece("Basic.img/%s/t" % fam)
    c = piece("Basic.img/%s/c" % fam)
    s = piece("Basic.img/%s/s" % fam)
    if not (t and s):
        return None
    return t, c, s


def try_fit(fam, dy):
    pieces = family(fam)
    if not pieces:
        return None
    t, c, s = pieces
    tw, th, tp = t
    best = None
    ks = range(0, 6) if c else [0]
    for k in ks:
        ch = c[1] if c else 0
        body = (c[2] * k) if c else []
        rows = tp[dy:] + body + s[2]
        h = len(rows)
        if h < SH:
            continue
        score = [0, 0]
        worst = 0
        for y in range(min(SH, h)):
            row = rows[y]
            for x in range(min(SW, tw)):
                r, g, b, a = row[x]
                if a != 255:
                    continue
                sr, sg, sb = sp[x, y]
                d = max(abs(r - sr), abs(g - sg), abs(b - sb))
                worst = max(worst, d) if d > 8 else worst
                score[1] += 1
                if d <= 8:
                    score[0] += 1
        if score[1] == 0:
            continue
        ratio = score[0] / score[1]
        cand = (ratio, k, h, score[1], worst)
        if best is None or cand[0] > best[0]:
            best = cand
    return best


for fam in ("YesNo2", "YesNo3", "Notice2", "Notice3", "Notice4"):
    for dy in range(0, 5):
        r = try_fit(fam, dy)
        if r:
            OUT.write("%-9s dy=%d best: match=%.4f k=%d 合成高=%d 计分像素=%d 最差通道差=%d\n"
                      % (fam, dy, r[0], r[1], r[2], r[3], r[4]))
OUT.close()
print("written")
