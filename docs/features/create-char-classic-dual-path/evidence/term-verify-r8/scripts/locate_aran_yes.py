#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""模板匹配：用冒险家命名页的 yes/name 区域当模板，在战神命名页里找真实位置。"""
import numpy as np
from PIL import Image
S = r"C:\Users\11036\AppData\Local\Temp\term_verify\shots\\"

def L(f):
    return np.asarray(Image.open(S + f).convert("L"), dtype=np.float32)

def ncc(a, b):
    a = a - a.mean(); b = b - b.mean()
    d = np.linalg.norm(a) * np.linalg.norm(b)
    return float((a * b).sum() / d) if d else 0.0

adv = L("create_s4_03_naming_adventurer.png")   # 冒险家命名页：yes 已在 (788,353) 点通
ara = L("create_s4_03_naming_aran.png")          # 战神命名页：yes (790,324) 没点通

# 候选模板：冒险家命名页里 yes 按钮周边、name 输入框周边（用冒险家已点通的位置当真值）
# 任务目标是找这些"内容块"在战神帧里出现在哪 —— 若在同位置，则用冒险家锚点
regions = {
    "yes_area_adv788_353": (758, 333, 818, 373),   # 60x40 around冒险家 yes
    "name_area_adv824_271": (764, 255, 884, 287),  # 120x32 around冒险家 name
    "sign_area": (700, 140, 760, 200),
}

def search(tpl, img, step=3):
    th, tw = tpl.shape
    h, w = img.shape
    best = (-2, 0, 0)
    # 搜索窗口：期望位置附近 ±60px
    for y in range(max(0, 140), min(h - th, 420), step):
        for x in range(max(0, 680), min(w - tw, 920), step):
            v = ncc(tpl, img[y:y+th, x:x+tw])
            if v > best[0]:
                best = (v, x, y)
    return best

for name, (x0, y0, x1, y1) in regions.items():
    tpl = adv[y0:y1, x0:x1]
    v, bx, by = search(tpl, ara)
    # 模板左上角在冒险家是 (x0,y0)，在战神找到 (bx,by)，中心偏移
    adv_center = ((x0+x1)//2, (y0+y1)//2)
    ara_center = (bx + (x1-x0)//2, by + (y1-y0)//2)
    print(f"{name:26s} NCC={v:.4f} adv_center={adv_center} ara_center={ara_center} delta=({ara_center[0]-adv_center[0]:+d},{ara_center[1]-adv_center[1]:+d})")
