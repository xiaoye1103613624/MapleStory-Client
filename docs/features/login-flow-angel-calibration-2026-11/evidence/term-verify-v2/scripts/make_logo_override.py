# -*- coding: utf-8 -*-
"""生成 LOGO 代码级覆盖素材并验证像素前提：
背景在 (434,80)-(831,299) 两端逐像素对齐 → 直接取目标帧该 ROI 整块覆盖，
连背景带字一起贴，接缝为零（无需抠图）。
输出 LoginLogo_override.png(397x219 RGBA 全不透明) + 数值验证 json。
"""
import os, json
import numpy as np
from PIL import Image
B = r"C:\Users\11036\AppData\Local\Temp\term_verify"
V = os.path.join(B,"ui_gap_audit","v3")
T = os.path.join(B,"targets_desktop"); S = os.path.join(B,"shots")
AX, AY, W, H = 434, 80, 397, 219
ours = np.asarray(Image.open(os.path.join(S,"v2_login_typed.png")).convert("RGB")).astype(float)
tgt  = np.asarray(Image.open(os.path.join(T,"desktop_login.png")).convert("RGB")).astype(float)
ob = ours[AY:AY+H, AX:AX+W]; tb = tgt[AY:AY+H, AX:AX+W]

def lum(a): return 0.299*a[:,:,0]+0.587*a[:,:,1]+0.114*a[:,:,2]
def ncc(x,y):
    x=x-x.mean(); y=y-y.mean(); d=np.sqrt((x*x).sum()*(y*y).sum())
    return round(float((x*y).sum()/d),4) if d>1e-9 else 0.0

# 字/叶 mask：本端 MapleStory 黑字、目标 BeiDouMS 黑字、两端红叶、Beta 小字
# 背景一致性只在「非前景」区评估
tl, ol = lum(tb), lum(ob)
fg = np.zeros((H,W),bool)
# 黑字（任一端明显暗）
fg |= (tl<95)&((tb.max(2)-tb.min(2))<95)
fg |= (ol<95)&((ob.max(2)-ob.min(2))<95)
# 红叶（任一端偏红）
for a in (tb,ob):
    fg |= (a[:,:,0]>130)&(a[:,:,0]>a[:,:,1]*1.35)&(a[:,:,0]>a[:,:,2]*1.35)
# 膨胀 3px 覆盖抗锯齿边
from numpy.lib.stride_tricks import sliding_window_view
pad=np.pad(fg.astype(np.uint8),2)
fg = sliding_window_view(pad,(5,5)).sum((2,3))>0
bg = ~fg
bg_ncc = ncc(ob.mean(2)[bg], tb.mean(2)[bg])  # 1D 不好，改 2D masked
# 2D 背景 NCC（前景置各自均值 → 近似零贡献）
obg = ob.mean(2).copy(); tbg = tb.mean(2).copy()
obg[fg]=ob.mean(); tbg[fg]=tb.mean()
bg_ncc2 = ncc(obg,tbg)

# 四周边缘 1px 带：本端帧 ROI 外圈 vs 目标帧 ROI 外圈相邻像素的连续性
# 取 ROI 外扩 1px 的框，比较两端（背景应完全一致）
def ring(a):
    return np.concatenate([
        a[AY-1, AX-1:AX+W+1].mean(1), a[AY+H, AX-1:AX+W+1].mean(1),
        a[AY-1:AY+H+1, AX-1].mean(1), a[AY-1:AY+H+1, AX+W].mean(1)])
ring_ncc = ncc(ring(ours), ring(tgt))
ring_mad = float(np.abs(ring(ours)-ring(tgt)).mean())

# 模拟整块覆盖：本端帧 ROI 换成目标 ROI，整幅图 vs 目标
sim = ours.copy(); sim[AY:AY+H, AX:AX+W]=tb
full_before = ncc(ours.mean(2), tgt.mean(2))
full_after  = ncc(sim.mean(2), tgt.mean(2))
roi_before  = ncc(ob.mean(2), tb.mean(2))
roi_after   = 1.0

# 生成覆盖素材（目标帧 ROI，RGBA 全不透明）
rgba = np.dstack([tb.astype(np.uint8), np.full((H,W),255,np.uint8)])
out_png = os.path.join(V,"LoginLogo_override.png")
Image.fromarray(rgba,"RGBA").save(out_png)

info={"anchor":[AX,AY],"size":[W,H],
      "bg_ncc_masked":bg_ncc2,"bg_fg_frac":round(float(fg.mean()),3),
      "ring_ncc":ring_ncc,"ring_mad":round(ring_mad,2),
      "full_gray_ncc_before":full_before,"full_gray_ncc_after_sim":full_after,
      "roi_ncc_before":roi_before,"roi_ncc_after_exact":roi_after,
      "verdict": "OVERRIDE_EXACT" if (bg_ncc2>0.90 and ring_ncc>0.90 and ring_mad<12) else "BACKGROUND_NOT_ALIGNED_DO_NOT_OVERRIDE"}
open(os.path.join(V,"logo_override_verdict.json"),"w",encoding="utf-8").write(json.dumps(info,ensure_ascii=False,indent=2))
print(json.dumps(info,ensure_ascii=False,indent=2))
