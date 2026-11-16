#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""f02 覆盖真相判定：分块 NCC 定位变化区 + 面板梯度能量判断 UI 是否还在。
梯度能量保留 = UI 线条/文字仍在（背景透出，正常美术）；梯度骤降 = UI 被抹掉（真 bug）。
"""
import numpy as np
from PIL import Image

SHOTS = r"C:\Users\11036\AppData\Local\Temp\term_verify\shots"

def load(path):
    return np.asarray(Image.open(path).convert("L"), dtype=np.float32)

def ncc(a, b):
    a = a - a.mean(); b = b - b.mean()
    d = np.linalg.norm(a) * np.linalg.norm(b)
    return float((a * b).sum() / d) if d else 0.0

def grad_energy(a):
    # Sobel 近似：相邻差分绝对值和
    gx = np.abs(np.diff(a, axis=1)).mean()
    gy = np.abs(np.diff(a, axis=0)).mean()
    return float(gx + gy)

def block_ncc(a, b, ny=6, nx=8):
    h, w = a.shape
    out = []
    for j in range(ny):
        row = []
        for i in range(nx):
            ya, yb = j*h//ny, (j+1)*h//ny
            xa, xb = i*w//nx, (i+1)*w//nx
            row.append(ncc(a[ya:yb, xa:xb], b[ya:yb, xa:xb]))
        out.append(row)
    return np.array(out)

w = "adventurer"
f00 = load(f"{SHOTS}/create_s4_05_step4_{w}_f00.png")
f09 = load(f"{SHOTS}/create_s4_05_step4_{w}_f09.png")
print("frame shape:", f00.shape)

# 1) 分块 NCC：哪些区域变化最大
bm = block_ncc(f00, f09)
print("\n[分块 NCC f00 vs f09, 数值越低变化越大]")
for j, row in enumerate(bm):
    print("  " + " ".join(f"{v:5.2f}" for v in row))

# 2) 面板上半区（9 行 UI 所在）梯度能量对比
# 面板大致 x335:545, y168:400（1280x720 坐标，按实际帧尺寸缩放）
h, ww = f00.shape
def sc(box):
    x0, y0, x1, y1 = box
    return (int(x0*ww/1280), int(y0*h/720), int(x1*ww/1280), int(y1*h/720))
regions = {
    "面板上半(9行UI)": (335, 168, 545, 400),
    "面板下半(木板/骰子)": (335, 400, 545, 520),
    "全屏": (0, 0, 1280, 720),
}
print("\n[梯度能量: UI 细节是否保留]")
for name, box in regions.items():
    xa, ya, xb, yb = sc(box)
    g0 = grad_energy(f00[ya:yb, xa:xb])
    g9 = grad_energy(f09[ya:yb, xa:xb])
    ratio = g9/g0 if g0 else 0
    verdict = "UI保留(背景透出)" if ratio > 0.6 else ("UI部分丢失" if ratio > 0.35 else "UI被抹掉(疑bug)")
    print(f"  {name:20s} f00={g0:6.3f} f09={g9:6.3f} ratio={ratio:.3f} -> {verdict}")
