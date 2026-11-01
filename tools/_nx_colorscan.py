"""Read-only UI.nx scan: find bitmap nodes whose dominant opaque color is close
to a target RGB and whose size is inside a band.

Used to answer "which sticker in this package is the pink skill tab?" without
guessing node names.  Nothing is written except a UTF-8 report file.

Usage:
    python tools/_nx_colorscan.py <file.nx> <R,G,B> <minW,maxW,minH,maxH> <out.txt>
"""
import struct
import sys

sys.path.insert(0, __file__.replace("\\", "/").rsplit("/", 1)[0])
from nx_inspect import Nx

TARGET = tuple(int(v) for v in sys.argv[2].split(","))
W0, W1, H0, H1 = (int(x) for x in sys.argv[3].split(","))
out_path = sys.argv[4]

nx = Nx(sys.argv[1])

# Parent map built once: path_of() recomputed it per call, which is O(nodes^2).
parent = {}
for n in range(nx.node_count):
    _, first, count, _, _, _, _ = nx.node(n)
    for c in range(first, first + count):
        parent[c] = n


def path_of(i):
    parts = []
    while i > 0:
        parts.append(nx.name(i))
        i = parent.get(i, 0)
    return "/".join(reversed(parts))


tr, tg, tb = TARGET
lines = []
for i in range(nx.node_count):
    _, _, _, ntype, _, w, h = nx.node(i)
    if ntype != 5 or not (W0 <= w <= W1 and H0 <= h <= H1):
        continue
    try:
        _, _, raw = nx.bitmap_pixels(i)
    except Exception:
        continue
    if not raw:
        continue
    hit = total = 0
    for p in range(0, len(raw), 4):
        b, g, r, a = raw[p], raw[p + 1], raw[p + 2], raw[p + 3]
        if a < 200:
            continue
        total += 1
        if abs(r - tr) <= 14 and abs(g - tg) <= 14 and abs(b - tb) <= 14:
            hit += 1
    if total and hit / total > 0.20:
        lines.append("%s  %dx%d  hit=%.2f" % (path_of(i), w, h, hit / total))

with open(out_path, "wb") as f:
    f.write(("\n".join(lines) + "\n").encode("utf-8"))
print(len(lines), "matches ->", out_path)
