#!/usr/bin/env python3
"""Native bottom UI: horizontal feature scan + zoomed crops."""
from PIL import Image

p = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/打开了S-E-I-K页面.png"
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()

print("=== row y=665 (status bar strip): color runs ===")
y = 665
runs = []
prev = None
start = 0
for x in range(W):
    c = px[x, y]
    key = "L" if (c[0] > 235 and c[1] > 235 and c[2] > 235) else ("D" if sum(c) < 200 else "M")
    if key != prev:
        if prev is not None:
            runs.append((prev, start, x - 1))
        prev = key
        start = x
runs.append((prev, start, W - 1))
# merge tiny runs for readability, show runs longer than 6px
for k, a, b in runs:
    if b - a >= 6:
        print("%s x=%d..%d (w=%d) mid=%s" % (k, a, b, b - a + 1, px[(a + b) // 2, y]))

print()
print("=== row y=700 (lower part): color runs ===")
y = 700
runs = []
prev = None
start = 0
for x in range(W):
    c = px[x, y]
    key = "L" if (c[0] > 235 and c[1] > 235 and c[2] > 235) else ("D" if sum(c) < 200 else "M")
    if key != prev:
        if prev is not None:
            runs.append((prev, start, x - 1))
        prev = key
        start = x
runs.append((prev, start, W - 1))
for k, a, b in runs:
    if b - a >= 6:
        print("%s x=%d..%d (w=%d) mid=%s" % (k, a, b, b - a + 1, px[(a + b) // 2, y]))

# zoomed crop of the whole bottom strip
crop = im.crop((0, 630, 1280, 720)).resize((1280 * 2, 90 * 2), Image.NEAREST)
crop.save(r"E:/MXD/BeiDou-Client-xy/_native_bottom.png")
# left half and right half separately at 3x
im.crop((0, 630, 640, 720)).resize((640 * 3, 90 * 3), Image.NEAREST).save(r"E:/MXD/BeiDou-Client-xy/_native_bottom_L.png")
im.crop((640, 630, 1280, 720)).resize((640 * 3, 90 * 3), Image.NEAREST).save(r"E:/MXD/BeiDou-Client-xy/_native_bottom_R.png")
print("crops saved")
