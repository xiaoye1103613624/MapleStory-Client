#!/usr/bin/env python3
"""Scan the native 1280x720 screenshot: bottom status bar / quickslot layout."""
from PIL import Image

p = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/打开了S-E-I-K页面.png"
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()

print("=== vertical profile (bottom 90 rows): count of 'bar-ish' light pixels per row ===")
for y in range(H-90, H):
    cnt = sum(1 for x in range(0, W, 4) if px[x, y][0] > 235 and px[x, y][1] > 235 and px[x, y][2] > 235)
    dark = sum(1 for x in range(0, W, 4) if sum(px[x, y]) < 150)
    print("y=%d light=%3d dark=%3d" % (y, cnt, dark))
