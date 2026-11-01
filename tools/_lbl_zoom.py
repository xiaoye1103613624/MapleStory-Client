#!/usr/bin/env python3
from PIL import Image

p = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/打开了S-E-I-K页面.png"
im = Image.open(p).convert("RGB")
im.crop((215, 685, 570, 716)).resize((355 * 4, 31 * 4), Image.NEAREST).save(r"E:/MXD/BeiDou-Client-xy/_native_labels.png")
print("saved")
