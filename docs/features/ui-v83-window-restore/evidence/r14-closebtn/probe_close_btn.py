import sys
sys.path.insert(0, "E:/project/MapleStory-Client/tools")
from nx_inspect import Nx

UI = "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx"

PATHS = [
    "Basic.img/BtClose/normal/0",
    "Basic.img/BtClose2/normal/0",
    "UIWindow.img/CashGachapon/BtClose/normal/0",
    "UIWindow.img/Family/BtClose/normal/0",
    "UIWindow.img/FamilyTree/BtClose/normal/0",
    "UIWindow.img/NewYearsCard/BtClose/normal/0",
    "UIWindow.img/UtilDlgEx/BtClose/normal/0",
]

nx = Nx(UI)
out = []

def find(path):
    n = nx.find(path)
    return n

for p in PATHS:
    i = find(p)
    if i is None:
        out.append("MISSING %s" % p)
        continue
    w, h, bgra = nx.bitmap_pixels(i)
    lines = []
    opaque = 0
    rs = gs = bs = 0
    for y in range(h):
        row = []
        for x in range(w):
            k = (y * w + x) * 4
            b, g, r, a = bgra[k], bgra[k + 1], bgra[k + 2], bgra[k + 3]
            if a > 128:
                opaque += 1
                rs += r; gs += g; bs += b
                row.append("#" if (r + g + b) > 360 else ("+" if (r + g + b) > 150 else "."))
            else:
                row.append(" ")
        lines.append("".join(row).rstrip())
    mean = "rgb(%d,%d,%d)" % (bs // max(opaque, 1), gs // max(opaque, 1), rs // max(opaque, 1))
    out.append("== %s  %dx%d opaque=%d ratio=%.2f mean=%s" % (p, w, h, opaque, opaque / float(w * h), mean))
    out.extend(lines)
    out.append("")

open("E:/project/_r21_close_ascii.txt", "w", encoding="utf-8").write("\n".join(out))
print("ok")
