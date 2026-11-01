#!/usr/bin/env python3
from PIL import Image

p = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/打开了S-E-I-K页面.png"
im = Image.open(p).convert("RGB")
px = im.load()

def bbox(x0, x1, y0, y1, pred):
    xs, ys = [], []
    for y in range(y0, y1):
        for x in range(x0, x1):
            if pred(px[x, y]):
                xs.append(x); ys.append(y)
    if not xs:
        return None
    return (min(xs), min(ys), max(xs), max(ys))

orange = lambda c: c[0] > 200 and 80 < c[1] < 180 and c[2] < 80
whiteT = lambda c: c[0] > 215 and c[1] > 215 and c[2] > 215
green = lambda c: c[1] > 130 and c[0] < 120 and c[2] < 120

print("LV number badge (orange):", bbox(0, 220, 680, 720, orange))
print("LV. text white in x<45:", bbox(0, 45, 685, 715, whiteT))
print("name chip text (white, x=45..215):", bbox(45, 215, 680, 720, whiteT))
print("HP label white (x=215..330, y=688..704):", bbox(215, 330, 688, 704, whiteT))
print("MP label white (x=325..440, y=688..704):", bbox(325, 440, 688, 704, whiteT))
print("EXP label white (x=438..560, y=688..704):", bbox(438, 560, 688, 704, whiteT))
print("green numbers [50/50]:", bbox(215, 560, 685, 704, green))
# chat row: target box and field
print("chatTarget box region (dark label x=0..92, y=649..680):")
for y in (655, 660, 665, 670, 675):
    row = []
    for x in range(0, 100, 5):
        row.append(px[x, y])
    print(y, row[:8])
