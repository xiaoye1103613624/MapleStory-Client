#!/usr/bin/env python3
from PIL import Image

p = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/打开了S-E-I-K页面.png"
im = Image.open(p).convert("RGB")
im.crop((810, 638, 1280, 720)).resize((470 * 2, 82 * 2), Image.NEAREST).save(r"E:/MXD/BeiDou-Client-xy/_native_qs.png")
print("saved")
