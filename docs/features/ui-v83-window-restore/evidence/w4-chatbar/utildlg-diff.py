"""B5-2 收尾取证：在最佳候选 (Notice4 t+c*1+s, dy=1) 下，列出左带 x0..16 的每一处不吻合，
看清「差在哪几行、差在哪个列」，据此判定弹层框到底是哪种拼法。
"""
import io
import sys

sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx
from PIL import Image

OUT = io.open(r"E:/project/_ms_probe/_utildlg_diff.txt", "w", encoding="utf-8")
nx = Nx("E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx")
shot = Image.open(r"E:/project/MapleStory-Client/docs/features/ui-v83-window-restore/"
                  r"evidence/w4-chatbar/whisper-dlg-native.png").convert("RGB")
SW, SH = shot.size
sp = shot.load()
BAND = 17


def rows_of(path):
    i = nx.find(path)
    if i is None:
        return None
    w, h, bgra = nx.bitmap_pixels(i)
    if w is None:
        return None
    out = []
    for y in range(h):
        r0 = y * w * 4
        out.append([(bgra[r0 + x * 4 + 2], bgra[r0 + x * 4 + 1], bgra[r0 + x * 4],
                      bgra[r0 + x * 4 + 3]) for x in range(w)])
    return out


for label, parts in (("Notice4 t+c+s", ["Notice4/t", "Notice4/c", "Notice4/s"]),
                     ("Notice4 t+c+s_line", ["Notice4/t", "Notice4/c", "Notice4/s_line"]),
                     ("Notice3 t+c+c+s", ["Notice3/t", "Notice3/c", "Notice3/c", "Notice3/s"])):
    comp = []
    src = []
    for p in parts:
        rs = rows_of("Basic.img/" + p)
        base = len(comp)
        comp += rs
        src.append((p, base, len(comp)))
    OUT.write("\n==== %s  高=%d ====\n" % (label, len(comp)))
    for y in range(SH):
        ry = y + 1
        bad = []
        if ry >= len(comp):
            OUT.write("shot y=%-3d 合成越界(ry=%d)\n" % (y, ry))
            continue
        for x in range(BAND):
            r, g, b, a = comp[ry][x]
            if a != 255:
                continue
            sr, sg, sb = sp[x, y]
            if max(abs(r - sr), abs(g - sg), abs(b - sb)) > 8:
                bad.append((x, (r, g, b), (sr, sg, sb)))
        if bad:
            for (p, b0, b1) in src:
                if b0 <= ry < b1:
                    OUT.write("shot y=%-3d <- %s!r%d  差 %d 列: %s\n" % (
                        y, p, ry - b0, len(bad),
                        " ".join("x%d 框(%d,%d,%d)/帧(%d,%d,%d)" % (x, r, g, b, sr, sg, sb)
                                 for x, (r, g, b), (sr, sg, sb) in bad[:6])))
                    break
OUT.close()
print("written")
