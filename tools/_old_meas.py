#!/usr/bin/env python3
from PIL import Image

old = r"E:/project/MapleStory-Client/docs/features/beidou-v16-parity/evidence/current_14_charselect.png"
im = Image.open(old).convert("RGB")
crop = im.crop((150, 250, 700, 620))
crop = crop.resize((crop.width, crop.height), Image.NEAREST)
crop.save(r"E:/project/MapleStory-Client/tools/_old_char_crop.png")
print("saved", crop.size)
