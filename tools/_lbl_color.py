#!/usr/bin/env python3
from PIL import Image
from collections import Counter

p = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/打开了S-E-I-K页面.png"
im = Image.open(p).convert("RGB")
px = im.load()
c = Counter()
for y in range(689, 704):
    for x in range(244, 330):
        c[px[x, y]] += 1
print("[50/50] region top colors:", c.most_common(6))
c2 = Counter()
for y in range(689, 704):
    for x in range(219, 240):
        c2[px[x, y]] += 1
print("HP label region top colors:", c2.most_common(4))
