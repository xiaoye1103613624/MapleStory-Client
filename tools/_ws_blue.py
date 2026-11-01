#!/usr/bin/env python3
"""Find blue board bbox inside scroll/0/3 (to derive chBackgrn offset)."""
from PIL import Image

im = Image.open(r"E:/MXD/BeiDou-Client-xy/_ws_scroll0_3.png").convert("RGBA")
W, H = im.size
px = im.load()

# 蓝板：亮蓝 (150,182,255) 附近
def is_blue(c):
    r, g, b, a = c
    return a > 200 and b > 180 and b - r > 40 and g > 120

xs, ys = [], []
for y in range(H):
    for x in range(W):
        if is_blue(px[x, y]):
            xs.append(x); ys.append(y)
print("blue bbox: x=%d..%d y=%d..%d  (w=%d h=%d)" % (
    min(xs), max(xs), min(ys), max(ys), max(xs)-min(xs)+1, max(ys)-min(ys)+1))
print("scroll size:", W, H)
