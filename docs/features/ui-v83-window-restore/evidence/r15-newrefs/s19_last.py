# -*- coding: utf-8 -*-
"""s19_last.py -- 下项 button full extent + img3 line-1 metrics."""
import io, os, json
from PIL import Image
D = os.path.dirname(os.path.abspath(__file__))
IDX = json.load(io.open(os.path.join(D, "_s0_paths.json"), encoding="utf-8"))
out = io.open(os.path.join(D, "_s19_last.txt"), "w", encoding="utf-8")
def L(s=""):
    out.write(s + "\n")
e = IDX["img3"]
im = Image.open(e[0] if isinstance(e, list) else e["path"]).convert("RGBA")
px = im.load()
NOTPANEL = lambda p: not (min(p[:3]) >= 225)
L("== 下项 button: 'not panel white' in x 440..512, y 234..270 ==")
for y in range(234, 271):
    xs = [x for x in range(440, 513) if NOTPANEL(px[x, y])]
    if xs:
        L("   y=%3d n=%3d x=%d..%d" % (y, len(xs), xs[0], xs[-1]))
bx0 = by0 = 10 ** 9; bx1 = by1 = -1; n = 0
for y in range(234, 271):
    for x in range(440, 513):
        if NOTPANEL(px[x, y]):
            n += 1
            bx0 = min(bx0, x); bx1 = max(bx1, x); by0 = min(by0, y); by1 = max(by1, y)
L("   bbox=(%d,%d)-(%d,%d) size=%dx%d n=%d" % (bx0, by0, bx1, by1, bx1 - bx0 + 1, by1 - by0 + 1, n))
L("== 结束对话 button: 'not background' in x 5..100, y 290..312 ==")
NB = lambda p: not (p[2] > p[0] + 20 and p[2] > p[1] + 5 and p[2] < 200)
bx0 = by0 = 10 ** 9; bx1 = by1 = -1; n = 0
for y in range(290, 313):
    for x in range(5, 101):
        if NB(px[x, y]):
            n += 1
            bx0 = min(bx0, x); bx1 = max(bx1, x); by0 = min(by0, y); by1 = max(by1, y)
L("   bbox=(%d,%d)-(%d,%d) size=%dx%d n=%d" % (bx0, by0, bx1, by1, bx1 - bx0 + 1, by1 - by0 + 1, n))
L("== img3 line-1 (欢迎来到BeiDou脚本中心) ==")
INK = lambda p: (p[0] + p[1] + p[2] < 300) or (p[0] > 150 and p[1] < 90 and p[2] < 90)
for y in range(34, 52):
    xs = [x for x in range(152, 512) if INK(px[x, y])]
    if xs:
        L("   y=%3d n=%3d x=%d..%d" % (y, len(xs), xs[0], xs[-1]))
out.close()
print("done")
