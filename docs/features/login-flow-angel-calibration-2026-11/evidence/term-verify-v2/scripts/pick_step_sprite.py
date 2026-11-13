"""事实驱动：目标端三页 step 标题牌互相 NCC + 与本端已渲染的建角页(step/3)比对，
确定 UICharSelect/UIRaceSelect 该用 Common/step 的哪一号，不靠猜。"""
import os, json
import numpy as np
from PIL import Image

ROOT = r"C:\Users\11036\AppData\Local\Temp\term_verify"
BOX = (0, 30, 160, 92)   # step 标题牌 y36..85 + 少量边距

def load(rel):
    return np.asarray(Image.open(os.path.join(ROOT, rel)).convert("RGB").crop(BOX)).astype(np.float64)

def ncc(a, b):
    A = a - a.mean(axis=(0, 1), keepdims=True)
    B = b - b.mean(axis=(0, 1), keepdims=True)
    den = np.sqrt((A * A).sum() * (B * B).sum())
    return float((A * B).sum() / den) if den > 0 else 0.0

frames = {
    "TGT_char":   "targets_desktop/desktop_char_select.png",
    "TGT_race":   "targets_desktop/desktop_racecreate_race.png",
    "TGT_create": "targets_desktop/desktop_racecreate_adventurer.png",
    "OUR_create": "shots/scene_reset_2_creation.png",  # 已渲染 step/3，NCC=1.0 基准
    "OUR_char":   "shots/v2_racecreate_settled.png",
    "OUR_race":   "shots/scene_reset_1_race_before.png",
}
imgs = {k: load(v) for k, v in frames.items() if os.path.exists(os.path.join(ROOT, v))}

keys = list(imgs)
mat = {a: {b: round(ncc(imgs[a], imgs[b]), 4) for b in keys} for a in keys}
for a in keys:
    print(a, {b: mat[a][b] for b in keys if b != a})

out = {
    "box": list(BOX), "frames": frames, "ncc": mat,
    "conclusion": "see printed matrix; OUR_create is the proven step/3 render baseline",
}
dst = os.path.join(ROOT, "ui_left_planks", "step_sprite_pick.json")
json.dump(out, open(dst, "w", encoding="utf-8"), indent=2, ensure_ascii=False)
print("WROTE", dst)
