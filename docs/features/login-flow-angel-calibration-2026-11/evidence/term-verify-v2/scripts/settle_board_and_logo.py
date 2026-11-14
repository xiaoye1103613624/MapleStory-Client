# -*- coding: utf-8 -*-
"""两件定死：
A) 目标端详情板纸内 ROI ↔ charInfo/charInfo1/charInfo2/charInfo3 四张画布 NCC 滑窗，
   决定多余「排行榜」是「换底板(纯代码)」还是「抹画布(live 资源)」。
B) 从目标帧差分抠出干净 BeiDouMS LOGO（背景相消：目标独有黑字 + 鲜红叶 → RGBA）。
"""
import os, json
import numpy as np
from PIL import Image

B = r"C:\Users\11036\AppData\Local\Temp\term_verify"
V = os.path.join(B, "ui_gap_audit", "v3")
CI = os.path.join(V, "charinfo")
T = os.path.join(B, "targets_desktop")
S = os.path.join(B, "shots")

def ncc(a, b, mask=None):
    a = a.astype(float); b = b.astype(float)
    if mask is None:
        va = a.ravel(); vb = b.ravel()
    else:
        va = a[mask].ravel(); vb = b[mask].ravel()
    va = va - va.mean(); vb = vb - vb.mean()
    d = np.sqrt((va*va).sum() * (vb*vb).sum())
    return float((va*vb).sum()/d) if d > 0 else 0.0

# ---------- A) 详情板画布匹配 ----------
tgt = np.asarray(Image.open(os.path.join(T, "desktop_char_select.png")).convert("RGB")).astype(float)
# 目标板纸内大致 x 353..536, y 160..330（浅米色）；滑窗找 183×151 / 183×115 最佳对齐
names = ["charInfo_183.png", "charInfo1_183.png", "charInfo2_183.png", "charInfo3_183.png"]
canv = {n: np.asarray(Image.open(os.path.join(CI, n)).convert("RGB")).astype(float) for n in names}
board_res = {}
for n, c in canv.items():
    h, w = c.shape[:2]
    best = (-9, None, None)
    for y in range(150, 210, 2):
        for x in range(330, 380, 2):
            roi = tgt[y:y+h, x:x+w]
            if roi.shape[:2] != (h, w): continue
            v = ncc(roi, c)
            if v > best[0]: best = (v, x, y)
    board_res[n] = {"h": h, "w": w, "ncc": round(best[0], 4), "at": [best[1], best[2]]}
print("BOARD MATCH", json.dumps(board_res))

# ---------- B) LOGO 差分抠图 ----------
# MSTitle anchor 在登录帧 (434,80)，397×219
AX, AY, W, H = 434, 80, 397, 219
ot = np.asarray(Image.open(os.path.join(T, "desktop_login.png")).convert("RGB")).astype(np.uint8)
# 本端帧（同场景背景，含 MapleStory 字标）
ours = np.asarray(Image.open(os.path.join(S, "v2_login_typed.png")).convert("RGB")).astype(np.uint8)
ob = ours[AY:AY+H, AX:AX+W].astype(float)
tb = ot[AY:AY+H, AX:AX+W].astype(float)

def lum(a): return 0.299*a[:,:,0]+0.587*a[:,:,1]+0.114*a[:,:,2]
tl, ol = lum(tb), lum(ob)
# 目标独有的黑字：目标暗 + 低饱和 + 本端不暗（本端是白字/浅底）
black = (tl < 100) & ((tb.max(2)-tb.min(2)) < 80) & (ol > tl + 28)
# 目标鲜红叶：高饱和红 + 比本端更红
red = (tb[:,:,0] > 140) & (tb[:,:,0] > tb[:,:,1]*1.5) & (tb[:,:,0] > tb[:,:,2]*1.5) & \
      (tb[:,:,0].astype(float) > ob[:,:,0] + 25)
# 浅粉半透明尾巴（目标端叶的淡色部分）
pink = (tb[:,:,0] > 130) & (tb[:,:,0] > tb[:,:,1]*1.15) & (tb[:,:,0] > tb[:,:,2]*1.15) & \
       (tb[:,:,0].astype(float) > ob[:,:,0] + 12) & ~red
keep = black | red | pink
# 形态学：去掉孤立噪点（3×3 至少 2 个邻域命中）
from numpy.lib.stride_tricks import sliding_window_view
pad = np.pad(keep.astype(np.uint8), 1)
cnt = sliding_window_view(pad, (3,3)).sum((2,3)) - keep.astype(np.uint8)
keep = keep & (cnt >= 2)
out = np.zeros((H, W, 4), np.uint8)
# 颜色：黑字用目标原色（去掉背景光）、红叶用目标原色、pink 保留目标色
out[:,:,:3] = tb.astype(np.uint8)
# 黑字统一压成近黑（目标是深棕黑，避免带背景绿）
out[black, :3] = (24, 22, 20)
alpha = np.zeros((H,W), np.uint8)
alpha[black] = 255
alpha[red] = 255
alpha[pink & keep] = 150
out[:,:,3] = alpha * keep
Image.fromarray(out, "RGBA").save(os.path.join(V, "MSTitle_BeiDou_extracted.png"))
# 预览：本端 | 目标 | 抠图合成到浅灰
bg = np.full((H,W,3), 205, np.uint8)
a = out[:,:,3:4].astype(float)/255.0
comp = (out[:,:,:3]*a + bg*(1-a)).astype(np.uint8)
pv = Image.new("RGB", (W*3+16, H+22), (20,20,20))
pv.paste(Image.fromarray(ob.astype(np.uint8)), (0,22))
pv.paste(Image.fromarray(tb.astype(np.uint8)), (W+8,22))
pv.paste(Image.fromarray(comp), (W*2+16,22))
from PIL import ImageDraw
d = ImageDraw.Draw(pv)
d.text((6,4),"OURS",fill=(255,255,255)); d.text((W+14,4),"TARGET",fill=(255,255,255))
d.text((W*2+22,4),"EXTRACTED",fill=(255,255,255))
pv.save(os.path.join(V, "MSTitle_extract_preview.png"))
info = {"board_match": board_res,
        "logo": {"anchor":[AX,AY], "size":[W,H], "black_px":int(black.sum()),
                 "red_px":int(red.sum()), "pink_px":int((pink&keep).sum()),
                 "kept_px":int(keep.sum())}}
open(os.path.join(V, "settle_board_and_logo.json"),"w",encoding="utf-8").write(json.dumps(info,ensure_ascii=False,indent=2))
print(json.dumps(info,ensure_ascii=False))
