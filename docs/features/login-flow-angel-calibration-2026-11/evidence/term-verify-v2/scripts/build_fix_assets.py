# -*- coding: utf-8 -*-
"""生成两张 live 资源修复素材（RBGA，供 orange-wz set_png 用）：
1) MSTitle_BeiDou_clean.png 397x219 —— 差分抠 BeiDouMS 黑字+鲜红叶，连通域去噪。
2) charInfo1_fixed.png 183x151 —— 抹掉底部多余「排行榜」居中字，恢复纸纹。
只生成素材，不动 live。
"""
import os, json
import numpy as np
from PIL import Image

B = r"C:\Users\11036\AppData\Local\Temp\term_verify"
V = os.path.join(B, "ui_gap_audit", "v3")
CI = os.path.join(V, "charinfo")
T = os.path.join(B, "targets_desktop"); S = os.path.join(B, "shots")

def lum(a): return 0.299*a[:,:,0]+0.587*a[:,:,1]+0.114*a[:,:,2]

# ---------------- 1) MSTitle 干净重建 ----------------
AX, AY, W, H = 434, 80, 397, 219
ot = np.asarray(Image.open(os.path.join(T, "desktop_login.png")).convert("RGB")).astype(float)
ours = np.asarray(Image.open(os.path.join(S, "v2_login_typed.png")).convert("RGB")).astype(float)
tb = ot[AY:AY+H, AX:AX+W]; ob = ours[AY:AY+H, AX:AX+W]
tl, ol = lum(tb), lum(ob)

black = (tl < 105) & ((tb.max(2)-tb.min(2)) < 85) & (ol > tl + 25)
red = (tb[:,:,0] > 140) & (tb[:,:,0] > tb[:,:,1]*1.5) & (tb[:,:,0] > tb[:,:,2]*1.5) & \
      (tb[:,:,0] > ob[:,:,0] + 22)
pink = (tb[:,:,0] > 128) & (tb[:,:,0] > tb[:,:,1]*1.14) & (tb[:,:,0] > tb[:,:,2]*1.14) & \
       (tb[:,:,0] > ob[:,:,0] + 10) & ~red

# 黑字主体 bbox：行/列密度峰值区域（BeiDouMS 在左半、中下部）
by, bx = np.where(black)
# 用中位数聚类：保留 x<250, y 在主体行带的黑像素
rowden = black[:, :250].sum(1)
rows = np.where(rowden > 6)[0]
r0 = int(rows.min()) - 2 if len(rows) else 90
r1 = int(rows.max()) + 2 if len(rows) else 205
colden = black[r0:r1, :].sum(0)
cols = np.where(colden > 2)[0]
col_cut = int(cols.max()) + 3 if len(cols) else 235
zone = np.zeros((H,W), bool); zone[max(0,r0):min(H,r1), 0:min(W,col_cut)] = True
black &= zone
# 红叶区限制
rzone = np.zeros((H,W), bool); rzone[20:200, 165:397] = True
red &= rzone; pink &= rzone
# 形态学去孤立（3x3 邻域 >=2）
from numpy.lib.stride_tricks import sliding_window_view
def despeckle(m):
    pad = np.pad(m.astype(np.uint8), 1)
    cnt = sliding_window_view(pad,(3,3)).sum((2,3)) - m.astype(np.uint8)
    return m & (cnt >= 2)
black = despeckle(black); red = despeckle(red); pink = despeckle(pink)

out = np.zeros((H,W,4), np.uint8)
out[:,:,:3] = tb.astype(np.uint8)
out[black,:3] = (22,20,18)                 # 近黑
a = np.zeros((H,W), np.uint8)
a[black] = 255; a[red] = 255; a[pink] = 150
out[:,:,3] = a
Image.fromarray(out,"RGBA").save(os.path.join(V,"MSTitle_BeiDou_clean.png"))

# 预览
bg = np.full((H,W,3),205,np.uint8); af = out[:,:,3:4].astype(float)/255.0
comp = (out[:,:,:3]*af + bg*(1-af)).astype(np.uint8)
pv = Image.new("RGB",(W*3+16,H+22),(20,20,20))
pv.paste(Image.fromarray(ob.astype(np.uint8)),(0,22))
pv.paste(Image.fromarray(tb.astype(np.uint8)),(W+8,22))
pv.paste(Image.fromarray(comp),(W*2+16,22))
from PIL import ImageDraw
d=ImageDraw.Draw(pv); d.text((6,4),"OURS",fill=(255,255,255)); d.text((W+14,4),"TARGET",fill=(255,255,255)); d.text((W*2+22,4),"CLEAN",fill=(255,255,255))
pv.save(os.path.join(V,"MSTitle_clean_preview.png"))

# ---------------- 2) charInfo1 抹字 ----------------
c1 = np.asarray(Image.open(os.path.join(CI,"charInfo1_183.png")).convert("RGB")).astype(np.uint8)
c = c1.copy()
# 逐行诊断：暗字（非纸）像素占比，打印定位「排行榜」行
cl = lum(c1.astype(float))
# 纸色取全图中位（浅米）
paper = np.median(cl[70:84, :])  # 字段行与标题带之间的空白纸
dark = (cl < paper - 45)
darkfrac = dark.mean(1)
diag = {str(y): round(float(darkfrac[y]),3) for y in range(108,151,3)}
# 底部多余字行：y>=116 且暗像素占比>2% 的行
extra_rows = [y for y in range(115,151) if darkfrac[y] > 0.02]
# 用 y=78..84 的纯纸行 tile 覆盖 y=115..151
tile = c1[78:85, :, :].copy()  # 7 行纸纹
for y in range(115,151):
    c[y,:,:] = tile[(y-115) % 7]
Image.fromarray(c,"RGB").save(os.path.join(V,"charInfo1_fixed.png"))
# 预览 原 | 修复
sc=3
p1=Image.fromarray(c1).resize((183*sc,151*sc),Image.NEAREST)
p2=Image.fromarray(c).resize((183*sc,151*sc),Image.NEAREST)
cmp=Image.new("RGB",(183*sc*2+10,151*sc+20),(20,20,20))
cmp.paste(p1,(0,20)); cmp.paste(p2,(183*sc+10,20))
dc=ImageDraw.Draw(cmp); dc.text((4,4),"charInfo1 ORIG",fill=(255,255,255)); dc.text((183*sc+14,4),"charInfo1 FIXED",fill=(255,255,255))
cmp.save(os.path.join(V,"charInfo1_fix_preview.png"))

info = {"mstitle":{"black_px":int(black.sum()),"red_px":int(red.sum()),"pink_px":int(pink.sum()),
                   "row_band":[r0,r1],"col_cut":col_cut},
        "charinfo1":{"paper_lum":round(float(paper),1),"extra_dark_rows":extra_rows[:20],
                     "row_diag":diag}}
open(os.path.join(V,"fix_assets.json"),"w",encoding="utf-8").write(json.dumps(info,ensure_ascii=False,indent=2))
print(json.dumps(info,ensure_ascii=False,indent=2))
