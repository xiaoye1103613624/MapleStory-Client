#!/usr/bin/env python3
"""量两端公告正文「Scania」暗字 bbox + 框底木轴，判正文是否需要独立 y 修正。"""
import json, os
import numpy as np
from PIL import Image

TV = r"C:\Users\11036\AppData\Local\Temp\term_verify"
T = os.path.join(TV, "targets_desktop", "desktop_channel_expanded.png")
O = os.path.join(TV, "shots", "channel_grid_f05.png")
OUT = os.path.join(TV, "ui_r3")

def body_bbox(p):
    a = np.asarray(Image.open(p).convert("RGB"), np.int16)
    R,G,B = a[...,0],a[...,1],a[...,2]
    lum = 0.299*R+0.587*G+0.114*B
    # 暗中性墨字（黑/深灰），排除暖色纸与绿背景
    neutral = (a.max(2)-a.min(2)) < 55
    dark = (lum < 115) & neutral
    # 正文带 y 325..360，x 270..480
    reg = dark[325:360, 270:480]
    ys,xs = np.where(reg)
    if len(ys)==0:
        return None
    return [int(xs.min()+270), int(ys.min()+325), int(xs.max()+271), int(ys.max()+326)]

def roller_bands(p):
    """框底木轴：x 290..470 内暖亮行的连续带（宽松阈值，看真实下沿）。"""
    a = np.asarray(Image.open(p).convert("RGB"), np.int16)
    R,G,B=a[...,0],a[...,1],a[...,2]
    lum=0.299*R+0.587*G+0.114*B
    warm=(lum>165)&(R>140)&(G>125)&(R>=B-12)
    reg=warm[330:390,290:470]
    cnt=reg.sum(1)
    rows=[330+i for i,v in enumerate(cnt) if v>=90]
    runs=[]
    if rows:
        s=pr=rows[0]
        for v in rows[1:]:
            if v==pr+1: pr=v
            else: runs.append([s,pr]); s=pr=v
        runs.append([s,pr])
    return runs

bt=body_bbox(T); bo=body_bbox(O)
rt=roller_bands(T); ro=roller_bands(O)
print("TARGET body",bt,"roller_runs",rt)
print("OURS   body",bo,"roller_runs",ro)
res={"target_body":bt,"ours_body":bo,
     "body_shift":None if not(bt and bo) else [bo[i]-bt[i] for i in range(4)],
     "target_roller":rt,"ours_roller":ro}
if bt and bo:
    res["body_center_x_shift"]=(bo[0]+bo[2])/2-(bt[0]+bt[2])/2
    res["body_center_y_shift"]=(bo[1]+bo[3])/2-(bt[1]+bt[3])/2
print(json.dumps(res,ensure_ascii=False,indent=2))
json.dump(res,open(os.path.join(OUT,"notice_body.json"),"w",encoding="utf-8"),ensure_ascii=False,indent=2)
