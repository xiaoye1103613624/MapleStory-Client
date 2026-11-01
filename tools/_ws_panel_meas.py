#!/usr/bin/env python3
"""Analyze user's world-select screenshot (743x469 windowed): board bbox + colors."""
from PIL import Image

im = Image.open(r"C:\Users\11036\AppData\Local\PixPin\Temp\XiaoYe_2026-10-24_00-27-49.png").convert("RGB")
W, H = im.size
px = im.load()

def is_blue(c):
    r, g, b = c
    return b > 170 and b - r > 30 and g > 110

xs, ys = [], []
for y in range(0, H):
    for x in range(0, W):
        if is_blue(px[x, y]):
            xs.append(x); ys.append(y)
if xs:
    print("blue bbox: x=%d..%d y=%d..%d (w=%d h=%d)" % (min(xs), max(xs), min(ys), max(ys), max(xs)-min(xs)+1, max(ys)-min(ys)+1))
else:
    print("no blue board found")

# 竖直剖面：x = 板中心
cx = (min(xs) + max(xs)) // 2 if xs else W // 2
prev = None
for y in range(0, H):
    c = px[cx, y]
    if prev is not None:
        d = sum(abs(a - b) for a, b in zip(c, prev))
        if d > 50:
            print("y=%d %s -> %s d=%d" % (y, prev, c, d))
    prev = c
