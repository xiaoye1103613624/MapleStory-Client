#!/usr/bin/env python3
"""Reference: locate 职业 label text rows and panel top edge precisely."""
from PIL import Image

ref = Image.open(r"C:/Users/11036/Desktop/正确客户端截图/选择角色页.png").convert("RGB")
px = ref.load()

def is_dark(c):
    r, g, b = c
    return r < 150 and g < 140 and b < 130

print("--- dark rows at x 395..430 (职业 label) ---")
for y in range(170, 230):
    n = sum(1 for x in range(395, 430) if is_dark(px[x, y]))
    if n > 2:
        print("  y=%d dark=%d" % (y, n))

print("--- col x=400 colors y=150..215 ---")
for y in range(150, 216, 2):
    print("  y=%d %s" % (y, px[400, y]))
