# -*- coding: utf-8 -*-
"""正确读 RGBA 抹 charInfo1 底部多余「排行榜」带。
先逐行诊断（只在 alpha>128 的不透明区统计暗字/棕带），定位多余带 y 范围，
再用同画布「字段行之间的纯纸行」RGBA tile 覆盖，尺寸/origin/alpha 结构全不变。
"""
import os, json
import numpy as np
from PIL import Image
B = r"C:\Users\11036\AppData\Local\Temp\term_verify"
V = os.path.join(B,"ui_gap_audit","v3"); CI = os.path.join(V,"charinfo")
src = os.path.join(CI,"charInfo1_183.png")
a = np.asarray(Image.open(src).convert("RGBA")).astype(np.int16)
H,W = a.shape[:2]
rgb = a[:,:,:3]; al = a[:,:,3]
opaque = al > 128
lum = 0.299*rgb[:,:,0]+0.587*rgb[:,:,1]+0.114*rgb[:,:,2]
R,G,Bl = rgb[:,:,0],rgb[:,:,1],rgb[:,:,2]
# 纸色（浅米，偏暖）：用 y=0..75 的不透明像素中位数
paper_rgb = np.median(rgb[(np.indices((H,W))[0] < 75) & opaque].reshape(-1,3),axis=0)
paper_lum = 0.299*paper_rgb[0]+0.587*paper_rgb[1]+0.114*paper_rgb[2]
# 暗字：不透明 + 明显暗于纸
dark = opaque & (lum < paper_lum - 55)
# 棕带：不透明 + 暖棕 + 中等亮（比纸暗但比字亮）
brown = opaque & (R > G+12) & (G > Bl+6) & (lum < paper_lum-8) & (lum > paper_lum-70)
row_dark = (dark.sum(1)/W); row_brown = (brown.sum(1)/W)
diag = [{"y":y,"dark":round(float(row_dark[y]),3),"brown":round(float(row_brown[y]),3)}
        for y in range(0,H,3)]
# 打印每段连续「文字/棕带」活动行
active = (row_dark>0.02)|(row_brown>0.12)
segs=[]; s=None
for y in range(H):
    if active[y] and s is None: s=y
    elif not active[y] and s is not None:
        if y-s>=3: segs.append([s,y])
        s=None
if s is not None: segs.append([s,H])
print("paper_rgb",paper_rgb.tolist(),"paper_lum",round(float(paper_lum),1))
print("active_y_segments",segs)
print("row_diag",json.dumps(diag[-16:]))

# 多余「排行榜」带 = 最后一个活动段（在「服务器排行榜」带之后）。
# 目标端只用 charInfo(115) 的字段行+单带；把 charInfo1 从第 115 行起
# （即「服务器排行榜」带以下）用纯纸 tile 覆盖，去掉多余带，保留上 115 行不动。
# 纯纸 tile 取字段行间的空白纸：y=80..92 里 dark/brown 最低的连续 6 行
cand = []
# 只在「服务器排行榜」带(y82-94) 与多余带(y121-131) 之间的纯纸区取 tile，
# 避开任何横边框线（y=76 那行带线，早期版本抹出条纹即因此）。
for y in range(98,114):
    if y+6<=H and (row_dark[y:y+6].max()<0.005) and (row_brown[y:y+6].max()<0.02):
        cand.append(y)
tile_y = cand[0] if cand else 102
tile = a[tile_y:tile_y+6,:,:].copy()  # RGBA 纸纹 6 行
out = a.copy()
CUT = 115
for y in range(CUT,H):
    out[y,:,:] = tile[(y-CUT)%6]
Image.fromarray(out.astype(np.uint8),"RGBA").save(os.path.join(V,"charInfo1_fixed.png"))
# 预览
sc=3
p1=Image.open(src).convert("RGBA").resize((W*sc,H*sc),Image.NEAREST).convert("RGB")
p2=Image.fromarray(out.astype(np.uint8),"RGBA").resize((W*sc,H*sc),Image.NEAREST).convert("RGB")
cmp=Image.new("RGB",(W*sc*2+10,H*sc+20),(20,20,20))
cmp.paste(p1,(0,20)); cmp.paste(p2,(W*sc+10,20))
from PIL import ImageDraw
d=ImageDraw.Draw(cmp); d.text((4,4),"charInfo1 ORIG",fill=(255,255,255)); d.text((W*sc+14,4),"charInfo1 FIXED",fill=(255,255,255))
cmp.save(os.path.join(V,"charInfo1_fix_preview.png"))
info={"paper_rgb":paper_rgb.tolist(),"paper_lum":round(float(paper_lum),1),
      "active_segments":segs,"tile_y":tile_y,"cut_y":CUT,"size":[W,H]}
open(os.path.join(V,"charinfo_fix.json"),"w",encoding="utf-8").write(json.dumps(info,ensure_ascii=False,indent=2))
print("FIXED tile_y",tile_y,"cut",CUT,json.dumps(info))
