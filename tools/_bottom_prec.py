#!/usr/bin/env python3
"""Precise native bottom-bar measurements at 1280x720."""
from PIL import Image

p = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/打开了S-E-I-K页面.png"
im = Image.open(p).convert("RGB")
W, H = im.size
px = im.load()

def runs_at(y, pred, minw=4):
    out = []
    start = None
    for x in range(W):
        if pred(px[x, y]):
            if start is None:
                start = x
        else:
            if start is not None:
                if x - start >= minw:
                    out.append((start, x - 1))
                start = None
    if start is not None:
        out.append((start, W - 1))
    return out

red = lambda c: c[0] > 150 and c[1] < 90 and c[2] < 90
blue = lambda c: c[2] > 140 and c[0] < 90 and c[1] < 140
white = lambda c: c[0] > 235 and c[1] > 235 and c[2] > 235
dark = lambda c: sum(c) < 160

print("=== HP red bar / MP blue bar / EXP white bar x-ranges per row ===")
for y in range(695, 715, 2):
    r = runs_at(y, red, 20)
    b = runs_at(y, blue, 20)
    w = runs_at(y, white, 20)
    print("y=%d RED=%s BLUE=%s WHITE=%s" % (y, r, b, w))

print()
print("=== info panel vertical extent: column x=200 ===")
for y in range(640, 720):
    c = px[200, y]
    print(y, c, "D" if dark(c) else ("L" if white(c) else "M"))
