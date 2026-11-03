# -*- coding: utf-8 -*-
"""把原生「系统设置」帧按 10 个选项行切开、放大 4 倍，逐行看滑钮落位。"""
import os
from PIL import Image

SRC = os.path.join(os.environ["USERPROFILE"], "Desktop", "\u6b63\u786e\u5ba2\u6237\u7aef\u622a\u56fe", "\u7cfb\u7edf\u8bbe\u7f6e.png")
OUT = r"E:\project\_r28"

im = Image.open(SRC).convert("RGB")
rows = [
    ("r01_quality", 26, 56),
    ("r02_bgm", 56, 86),
    ("r03_sfx", 86, 116),
    ("r04_shotpath", 116, 146),
    ("r05_mouse", 146, 176),
    ("r06_hp", 176, 206),
    ("r07_mp", 206, 236),
    ("r08_effect", 236, 266),
    ("r09_mobinfo", 266, 296),
    ("r10_window", 296, 330),
]
for name, y0, y1 in rows:
    band = im.crop((0, y0, im.size[0], y1))
    band.resize((band.size[0] * 4, band.size[1] * 4), Image.NEAREST).save(
        os.path.join(OUT, name + ".png"))

# 拼成一张总表：左列行名 + 右列放大条带
bands = [(n, im.crop((0, y0, im.size[0], y1))) for n, y0, y1 in rows]
gap = 6
total_h = sum(b.size[1] for _, b in bands) + gap * len(bands)
sheet = Image.new("RGB", (im.size[0], total_h), (255, 0, 255))
y = 0
for _, b in bands:
    sheet.paste(b, (0, y))
    y += b.size[1] + gap
sheet.save(os.path.join(OUT, "nat_sysopt_sheet.png"))
print("sheet", sheet.size)

# 顺带把两条下拉整块裁出来（含右端箭头）
im.crop((150, 120, 250, 145)).resize((400, 100), Image.NEAREST).save(os.path.join(OUT, "combo_path.png"))
im.crop((150, 271, 250, 296)).resize((400, 100), Image.NEAREST).save(os.path.join(OUT, "combo_mob.png"))
print("ok")
