# -*- coding: utf-8 -*-
"""逐行棕色带密度：目标帧详情板 vs 四张 charInfo 画布，定死目标端用哪张底板。
棕色带（标题条）特征：R>G>B 且 R∈[150,220] 且饱和度中等；逐行统计占比。
"""
import os, json
import numpy as np
from PIL import Image
B = r"C:\Users\11036\AppData\Local\Temp\term_verify"
V = os.path.join(B, "ui_gap_audit", "v3"); CI = os.path.join(V, "charinfo")
tgt = np.asarray(Image.open(os.path.join(B, "targets_desktop", "desktop_char_select.png")).convert("RGB")).astype(float)

def brown_rows(a, x0, y0, w, h):
    r = a[y0:y0+h, x0:x0+w]
    R,G,Bl = r[:,:,0], r[:,:,1], r[:,:,2]
    brown = (R > 140) & (R < 225) & (R > G+18) & (G > Bl+8) & (G > 100)
    dens = brown.mean(1)
    segs=[]; s=None
    for i,d in enumerate(dens):
        if d > 0.35 and s is None: s=i
        elif d <= 0.35 and s is not None:
            if i-s >= 4: segs.append([s+y0, i+y0, round(float(dens[s:i].max()),2)])
            s=None
    if s is not None and len(dens)-s>=4: segs.append([s+y0, len(dens)+y0, round(float(dens[s:].max()),2)])
    return segs

# 目标端板：按 settle 实测纸面 x≈350..536，y≈160..330；取棕色带
res = {}
res["target_board"] = brown_rows(tgt, 352, 150, 185, 200)
for n in ["charInfo_183.png","charInfo1_183.png","charInfo2_183.png","charInfo3_183.png"]:
    c = np.asarray(Image.open(os.path.join(CI,n)).convert("RGB")).astype(float)
    h,w = c.shape[:2]
    res[n] = {"size":[w,h], "bands": brown_rows(c, 0, 0, w, h)}
print(json.dumps(res, ensure_ascii=False, indent=2))
open(os.path.join(V,"board_bands.json"),"w",encoding="utf-8").write(json.dumps(res,ensure_ascii=False,indent=2))
