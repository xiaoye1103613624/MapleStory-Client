#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""step4 逐页数值化分类器：当前模型读图为空，页面判定全部靠 NCC。
用法: python classify_step4.py [class ...]   不传则三类全跑。
"""
import os, sys
import numpy as np
from PIL import Image

SHOTS = r"C:\Users\11036\AppData\Local\Temp\term_verify\shots"

def load(path, size=(320, 180)):
    im = Image.open(path).convert("RGB").resize(size)
    return np.asarray(im, dtype=np.float32)

def ncc(a, b):
    a = a - a.mean(); b = b - b.mean()
    d = np.linalg.norm(a) * np.linalg.norm(b)
    return float((a * b).sum() / d) if d else 0.0

# 参考模板（都用新包 adventurer 的正确帧）
REFS = {
    "step4":      load(os.path.join(SHOTS, "create_s4_05_step4_adventurer_f00.png")),
    "naming":     load(os.path.join(SHOTS, "create_s4_03_naming_adventurer.png")),
    "charselect": load(os.path.join(SHOTS, "create_s4_00_charselect.png")),
    "raceselect": load(os.path.join(SHOTS, "create_s4_01_raceselect.png")),
}

def classify(path):
    f = load(path)
    sc = {k: ncc(f, v) for k, v in REFS.items()}
    return max(sc, key=sc.get), sc

STAGES = [
    "create_s4_00_charselect",
    "create_s4_01_raceselect",
    "create_s4_02_card_{w}",
    "create_s4_03_naming_{w}",
    "create_s4_04_named_{w}",
]

def run(w):
    print(f"\n===== {w} =====")
    for st in STAGES:
        fn = st.format(w=w) + ".png"
        p = os.path.join(SHOTS, fn)
        if not os.path.exists(p):
            print(f"  {fn:44s} MISSING"); continue
        best, sc = classify(p)
        print(f"  {fn:44s} -> {best:10s} s4={sc['step4']:.3f} nm={sc['naming']:.3f} cs={sc['charselect']:.3f} rs={sc['raceselect']:.3f}")
    for i in range(10):
        fn = f"create_s4_05_step4_{w}_f{i:02d}.png"
        p = os.path.join(SHOTS, fn)
        if not os.path.exists(p):
            print(f"  {fn:44s} MISSING"); continue
        best, sc = classify(p)
        print(f"  {fn:44s} -> {best:10s} s4={sc['step4']:.3f} nm={sc['naming']:.3f} cs={sc['charselect']:.3f}")

if __name__ == "__main__":
    classes = sys.argv[1:] if len(sys.argv) > 1 else ["adventurer", "cygnus", "aran"]
    for w in classes:
        run(w)
