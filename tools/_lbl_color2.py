#!/usr/bin/env python3
from PIL import Image
from collections import Counter

p = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/打开了S-E-I-K页面.png"
im = Image.open(p).convert("RGB")
px = im.load()
c = Counter()
xs = []
for y in range(685, 706):
    for x in range(215, 570):
        r, g, b = px[x, y]
        if r > 140 and g > 140 and b < 130 and not (r > 240 and g > 240):
            c[(r, g, b)] += 1
            xs.append((x, y))
if xs:
    print("yellowish count:", len(xs), "bbox:", min(a for a, b in xs), min(b for a, b in xs), max(a for a, b in xs), max(b for a, b in xs))
print(c.most_common(5))
