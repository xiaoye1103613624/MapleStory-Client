# -*- coding: utf-8 -*-
"""侧栏补齐后目视证据：本端帧 | 目标帧 左栏 ROI 3× 放大并排图。
一次一张 read_image 避免错配。"""
import os
from PIL import Image

BASE = r"C:\Users\11036\AppData\Local\Temp\term_verify"
SHOTS = os.path.join(BASE, "shots")
T = os.path.join(BASE, "targets_desktop")
OUT = os.path.join(BASE, "ui_left_planks")
os.makedirs(OUT, exist_ok=True)

BOX = (0, 0, 160, 470)
SCALE = 3
PAIRS = [
    ("sidebar_fix_1_charselect.png", "desktop_char_select.png", "sidefix_cmp_char.png"),
    ("sidebar_fix_2_raceselect.png", "desktop_racecreate_race.png", "sidefix_cmp_race.png"),
]

for ours, tgt, name in PAIRS:
    a = Image.open(os.path.join(SHOTS, ours)).convert("RGB").crop(BOX)
    b = Image.open(os.path.join(T, tgt)).convert("RGB").crop(BOX)
    a = a.resize((a.width * SCALE, a.height * SCALE), Image.LANCZOS)
    b = b.resize((b.width * SCALE, b.height * SCALE), Image.LANCZOS)
    canvas = Image.new("RGB", (a.width + b.width + 12, a.height), (24, 24, 24))
    canvas.paste(a, (0, 0))
    canvas.paste(b, (a.width + 12, 0))
    p = os.path.join(OUT, name)
    canvas.save(p)
    print(name, canvas.size, "left=OURS right=TARGET")
