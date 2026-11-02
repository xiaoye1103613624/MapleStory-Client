"""B5-2 决定性取证（第 4 版）：只用**最左 17 列**这条纯框带，搜出竖向拼法。

为什么只比左带：输入框从 x=19 起、下拉箭头 x=247..256、按钮 x=159..254 全在中部，
左带 x 0..16 是 100% 窗框，不被控件污染（前几版就是被 x=19..20 的框线污染，
才出现「no exact row」）。

搜索：family ∈ Basic.img 的 Notice*/YesNo* × k（c 段平铺次数 0..4）× dy（顶阴影跳过的行数 0..4）。
判据：左带逐格通道差<=8 且合成层 alpha==255 视为命中；命中率 + 命中格数一起排。
"""
import io
import sys

sys.path.insert(0, r"E:/project/MapleStory-Client/tools")
from nx_inspect import Nx
from PIL import Image

OUT = io.open(r"E:/project/_ms_probe/_utildlg_fit4.txt", "w", encoding="utf-8")
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


FAMS = ["Notice", "Notice2", "Notice3", "Notice4", "Notice5", "YesNo", "YesNo2", "YesNo3"]
res = []
for fam in FAMS:
    base = "Basic.img/%s" % fam
    t, c, s = rows_of(base + "/t"), rows_of(base + "/c"), rows_of(base + "/s")
    if not (t and s):
        # 单张 backgrnd 的家族
        bg = rows_of(base + "/backgrnd")
        if bg:
            for dy in range(0, 5):
                hit = tot = 0
                for y in range(SH):
                    ry = y + dy
                    if ry >= len(bg):
                        continue
                    for x in range(BAND):
                        r, g, b, a = bg[ry][x]
                        if a != 255:
                            continue
                        tot += 1
                        sr, sg, sb = sp[x, y]
                        if max(abs(r - sr), abs(g - sg), abs(b - sb)) <= 8:
                            hit += 1
                res.append((hit / tot if tot else 0, hit, tot, fam, "backgrnd", dy, len(bg)))
        continue
    for k in range(0, 5):
        comp = t + (c * k) + s
        for dy in range(0, 5):
            hit = tot = 0
            for y in range(SH):
                ry = y + dy
                if ry >= len(comp):
                    continue
                for x in range(BAND):
                    r, g, b, a = comp[ry][x]
                    if a != 255:
                        continue
                    tot += 1
                    sr, sg, sb = sp[x, y]
                    if max(abs(r - sr), abs(g - sg), abs(b - sb)) <= 8:
                        hit += 1
            res.append((hit / tot if tot else 0, hit, tot, fam, "t+c*%d+s" % k, dy, len(comp)))

res.sort(reverse=True)
OUT.write("shot %dx%d  左带 x0..%d\n\n" % (SW, SH, BAND - 1))
for r in res[:20]:
    OUT.write("match=%.4f 命中=%-5d/%-5d %-9s %-9s dy=%d 合成高=%d\n" % r)
OUT.close()
print("written")
