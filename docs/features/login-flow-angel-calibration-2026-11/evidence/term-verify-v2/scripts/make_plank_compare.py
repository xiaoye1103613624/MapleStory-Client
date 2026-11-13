# -*- coding: utf-8 -*-
"""生成左侧木牌栏并排放大图(左=本端 右=目标) + 6张创建角色目标帧总览拼图 + 逐行木牌密度。
一次读一张即可定性，避免多图并行错配。
"""
import os, json
import numpy as np
from PIL import Image, ImageDraw

TV = r"C:\Users\11036\AppData\Local\Temp\term_verify"
DESK = os.path.join(TV, "targets_desktop")
SHOTS = os.path.join(TV, "shots")
OUT = os.path.join(TV, "ui_left_planks")
os.makedirs(OUT, exist_ok=True)

PAIRS = [
    ("login", "v2_login_typed.png", "desktop_login.png"),
    ("channel", "v2_channel_grid.png", "desktop_channel_expanded.png"),
    ("char_select", "v2_racecreate_settled.png", "desktop_char_select.png"),
    ("racecreate", "racecreate_click_f06.png", "desktop_racecreate_race.png"),
]
ROI = (0, 0, 160, 470)

def strip(p):
    im = Image.open(p).convert("RGB").crop(ROI)
    return im

for page, shot, tgt in PAIRS:
    a = strip(os.path.join(SHOTS, shot))
    b = strip(os.path.join(DESK, tgt))
    w, h = a.size
    scale = 2
    canvas = Image.new("RGB", (w * 2 * scale + 24, h * scale + 46), (24, 24, 28))
    aa = a.resize((w * scale, h * scale))
    bb = b.resize((w * scale, h * scale))
    canvas.paste(aa, (0, 34))
    canvas.paste(bb, (w * scale + 24, 34))
    d = ImageDraw.Draw(canvas)
    d.text((6, 8), f"LOCAL  {shot}", fill=(120, 230, 140))
    d.text((w * scale + 30, 8), f"TARGET  {tgt}", fill=(255, 190, 120))
    d.line([(w * scale + 12, 34), (w * scale + 12, h * scale + 34)], fill=(200, 60, 60), width=2)
    canvas.save(os.path.join(OUT, f"cmp_{page}.png"))

# 6 张创建角色目标帧总览（3x2 缩略，标注名，一次读分辨各页状态）
race_names = [
    "desktop_racecreate_race.png", "desktop_racecreate_adventurer.png",
    "desktop_racecreate_knight.png", "desktop_racecreate_knight2.png",
    "desktop_racecreate_aran.png", "desktop_racecreate_aran2.png",
]
tw, th = 640, 360
grid = Image.new("RGB", (tw * 3, (th + 26) * 2), (20, 20, 24))
gd = ImageDraw.Draw(grid)
for i, n in enumerate(race_names):
    im = Image.open(os.path.join(DESK, n)).convert("RGB").resize((tw, th))
    x, y = (i % 3) * tw, (i // 3) * (th + 26)
    grid.paste(im, (x, y + 26))
    gd.text((x + 6, y + 6), f"[{i}] {n}", fill=(255, 220, 120))
grid.save(os.path.join(OUT, "race_targets_grid.png"))

# 逐行「木牌密度」：暗色像素(R,G,B均值<110)在左栏每行占比，用于定位木牌 y 区段
profile = {}
for page, shot, tgt in PAIRS:
    prof = {"local": [], "target": []}
    for key, p in (("local", os.path.join(SHOTS, shot)), ("target", os.path.join(DESK, tgt))):
        arr = np.asarray(Image.open(p).convert("RGB").crop(ROI), dtype=np.float64)
        dark = (arr.mean(2) < 110).mean(1)  # 每行暗像素占比
        prof[key] = [round(float(v), 4) for v in dark]
    profile[page] = prof
with open(os.path.join(OUT, "plank_row_profile.json"), "w", encoding="utf-8") as f:
    json.dump({"roi": ROI, "profiles": profile}, f)

# 找出「暗像素占比>0.35」的连续区段(木牌候选)，两端对比
segs = {}
for page in profile:
    out = {}
    for key in ("local", "target"):
        arr = profile[page][key]
        runs, cur = [], None
        for y, v in enumerate(arr):
            if v > 0.35:
                if cur is None:
                    cur = [y, y]
                else:
                    cur[1] = y
            else:
                if cur and cur[1] - cur[0] >= 6:
                    runs.append(cur)
                cur = None
        if cur and cur[1] - cur[0] >= 6:
            runs.append(cur)
        out[key] = runs
    segs[page] = out
with open(os.path.join(OUT, "plank_segments.json"), "w", encoding="utf-8") as f:
    json.dump(segs, f, indent=2)

print("=== 木牌候选 y 区段(暗像素>35% 连续>=6行) ===")
for page in segs:
    print(f"  {page:14s} LOCAL {segs[page]['local']}")
    print(f"  {'':14s} TARGET{segs[page]['target']}")
print("images:", [f for f in os.listdir(OUT) if f.endswith('.png')])
