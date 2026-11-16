#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Region gradient energy: distinguish step4 (9-row avatar UI) from naming page (name field).
Regions in 1280x720 coords:
  rows9  : x335:545 y168:400   step4 has 9 avatar rows+arrows => HIGH grad
  avatar : x550:720 y120:520   both have char preview
  namebd : x700:830 y150:290   naming has input box+text => HIGH grad; step4 LOW
"""
import numpy as np
from PIL import Image
import os
S = r"C:\Users\11036\AppData\Local\Temp\term_verify\shots"

def load(p):
    return np.asarray(Image.open(p).convert("L"), dtype=np.float32)

def ge(a):
    return float(np.abs(np.diff(a, axis=1)).mean() + np.abs(np.diff(a, axis=0)).mean())

REG = {"rows9": (335,168,545,400), "avatar": (550,120,720,520), "namebd": (700,150,830,290)}
def prof(a):
    h,w = a.shape
    out={}
    for k,(x0,y0,x1,y1) in REG.items():
        xa,ya,xb,yb = int(x0*w/1280),int(y0*h/720),int(x1*w/1280),int(y1*h/720)
        out[k]=ge(a[ya:yb,xa:xb])
    return out

SAMPLES = {
 "ADV_step4_f00": "create_s4_05_step4_adventurer_f00.png",
 "ADV_naming":    "create_s4_03_naming_adventurer.png",
 "CYG_step4_f00": "create_s4_05_step4_cygnus_f00.png",
 "CYG_naming":    "create_s4_03_naming_cygnus.png",
 "ARA_step4_f00": "create_s4_05_step4_aran_f00.png",
 "ARA_step4_f05": "create_s4_05_step4_aran_f05.png",
 "ARA_step4_f09": "create_s4_05_step4_aran_f09.png",
 "ARA_naming":    "create_s4_03_naming_aran.png",
 "ARA_card":      "create_s4_02_card_aran.png",
}
print(f"{'sample':18s} {'rows9':>8s} {'avatar':>8s} {'namebd':>8s}  rows9/namebd")
for k,fn in SAMPLES.items():
    p=os.path.join(S,fn)
    if not os.path.exists(p): print(k,"MISSING"); continue
    g=prof(load(p))
    ratio=g["rows9"]/g["namebd"] if g["namebd"] else 0
    print(f"{k:18s} {g['rows9']:8.2f} {g['avatar']:8.2f} {g['namebd']:8.2f}  {ratio:6.2f}")
