#!/usr/bin/env python3
"""Measure the LOADING bar in the user's screenshot (1280x720 game window)."""
import sys
from PIL import Image

shot = sys.argv[1]
im = Image.open(shot).convert("RGB")
w, h = im.size
print("screenshot size:", w, h)
px = im.load()

# orange bar blocks: strong orange (r>200, g in 100..210, b<120)
pts = [(x, y) for y in range(h) for x in range(w)
       if px[x, y][0] > 200 and 90 <= px[x, y][1] <= 215 and px[x, y][2] < 130]
if pts:
    xs = [p[0] for p in pts]
    ys = [p[1] for p in pts]
    print("orange bbox: x=%d..%d y=%d..%d (w=%d h=%d)"
          % (min(xs), max(xs), min(ys), max(ys), max(xs) - min(xs) + 1, max(ys) - min(ys) + 1))
    # column runs to count blocks
    cols = {}
    for (x, y) in pts:
        cols[x] = cols.get(x, 0) + 1
    runs = []
    x = min(xs)
    while x <= max(xs):
        if x in cols:
            x0 = x
            while x <= max(xs) and x in cols:
                x += 1
            runs.append((x0, x - 1))
        else:
            x += 1
    print("block runs:", runs)
else:
    print("no orange found")

# expected: window pos (499,288); bar rel (99,97) size 109x8 -> screen x 598..706, y 385..392
print("expected bar screen rect: x=598..706 y=385..392 (if centered 1280x720)")
