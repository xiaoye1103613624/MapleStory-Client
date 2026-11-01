"""Read-only .nx scan for rose/pink cell art (pet-equip slot backgrounds).

Usage: python tools/_nx_rosesearch.py <file.nx> <out.txt>
"""
import sys

sys.path.insert(0, __file__.replace("\\", "/").rsplit("/", 1)[0])
from nx_inspect import Nx

nx = Nx(sys.argv[1])
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


lines = []
for i in range(nx.node_count):
    _, _, _, ntype, _, w, h = nx.node(i)
    if ntype != 5 or not (8 <= w <= 120 and 8 <= h <= 120):
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
        if 170 <= r <= 235 and 120 <= g <= 190 and 140 <= b <= 200 and (r - b) >= 20 and (r - g) >= 20:
            hit += 1
    if total and hit / total > 0.30:
        lines.append("%s  %dx%d  hit=%.2f" % (path_of(i), w, h, hit / total))

with open(sys.argv[2], "wb") as f:
    f.write(("\n".join(lines) + "\n").encode("utf-8"))
print(len(lines), "rose candidates ->", sys.argv[2])
