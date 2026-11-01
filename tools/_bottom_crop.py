#!/usr/bin/env python3
from PIL import Image

p = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/打开了S-E-I-K页面.png"
im = Image.open(p).convert("RGB")
# info panel left part (LV + name + labels), zoomed 4x
im.crop((0, 680, 570, 720)).resize((570 * 4, 40 * 4), Image.NEAREST).save(r"E:/MXD/BeiDou-Client-xy/_native_info.png")
# middle cluster rows
im.crop((560, 640, 820, 720)).resize((260 * 4, 80 * 4), Image.NEAREST).save(r"E:/MXD/BeiDou-Client-xy/_native_mid.png")
print("saved")
