"""Locate a UI.nx sticker inside a reference screenshot by exhaustive template
match (alpha-weighted, tolerant).  Read-only; prints UTF-8-safe ASCII only.

Usage:
    python tools/_nx_match.py <ref.png> <nx> <node> <search x0,y0,x1,y1>
"""
import sys
sys.path.insert(0, __file__.replace("\\", "/").rsplit("/", 1)[0])
from nx_inspect import Nx
from PIL import Image

ref = Image.open(sys.argv[1]).convert("RGB")
nx = Nx(sys.argv[2])
node = sys.argv[3]
x0, y0, x1, y1 = (int(v) for v in sys.argv[4].split(","))

idx = nx.find(node)
w, h, raw = nx.bitmap_pixels(idx)
tpl = []
for p in range(0, len(raw), 4):
    b, g, r, a = raw[p], raw[p + 1], raw[p + 2], raw[p + 3]
    tpl.append((r, g, b, a))

rp = ref.load()
best = []
for oy in range(y0, y1 - h + 1):
    for ox in range(x0, x1 - w + 1):
        bad = 0
        n = 0
        for j in range(h):
            base = j * w
            for i in range(w):
                r, g, b, a = tpl[base + i]
                if a < 200:
                    continue
                n += 1
                pr, pg, pb = rp[ox + i, oy + j]
                if abs(pr - r) > 26 or abs(pg - g) > 26 or abs(pb - b) > 26:
                    bad += 1
        if n:
            best.append((bad / n, ox, oy))

best.sort()
print(node, "tpl", w, h, "opaque px", n)
for score, ox, oy in best[:6]:
    print("  %.4f  @(%d,%d)" % (score, ox, oy))
