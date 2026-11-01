#!/usr/bin/env python3
import sys
from PIL import Image
p = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/打开了S-E-I-K页面.png"
im = Image.open(p).convert("RGBA")
W,H = im.size
print("size", W, H)
# Save a copy we can inspect pixel-wise
im.save(r"E:/MXD/BeiDou-Client-xy/_seik.png")
# Top-level scan: find distinct UI panels by color clustering roughly via simple sampling
px = im.load()
# report some sentinel colors along bottom strip (status bar / quickslot)
print("--- bottom 60px color sample (every 80px) ---")
for y in range(H-60, H-10, 10):
    row=[]
    for x in range(0, W, 80):
        row.append(px[x,y][:3])
    print(y, row)
print("--- top-left corner of image ---")
for y in range(0,30,5):
    row=[px[x,y][:3] for x in range(0,200,20)]
    print(y, row)
