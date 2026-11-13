import numpy as np, os
from PIL import Image

BASE = r"E:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots"
SHOTS = ["选择频道页.png", "选中频道页.png"]

def load_rgba(p): return np.asarray(Image.open(p).convert("RGBA")).astype(np.float32)
def corr(f,t,os_):
    H,W=f.shape;h,w=t.shape;pad=(H+h,W+w)
    F=np.fft.rfft2(f,pad);T=np.fft.rfft2(t,pad)
    c=np.fft.irfft2(F*np.conj(T),pad)
    return c[:os_[0],:os_[1]]
def ncc_map(s,t):
    H,W=s.shape[:2];h,w=t.shape[:2];oy,ox=H-h+1,W-w+1
    g=s[...,:3].mean(2);a=t[...,3]/255.0;tt=t[...,:3].mean(2)
    S=float(a.sum());Sg=corr(g,a,(oy,ox));Sgg=corr(g*g,a,(oy,ox));Sgt=corr(g,a*tt,(oy,ox))
    St=float((a*tt).sum());Stt=float((a*tt*tt).sum())
    mu_g=Sg/S;mu_t=St/S;var_g=np.maximum(Sgg/S-mu_g*mu_g,0.0);var_t=max(Stt/S-mu_t*mu_t,1e-9)
    cov=Sgt/S-mu_g*mu_t;ncc=cov/(np.sqrt(var_g)*np.sqrt(var_t)+1e-6)
    return np.nan_to_num(ncc)
def topk(ncc,tw,th,k=3,min_dist=12):
    order=np.argsort(ncc.ravel())[::-1];H,W=ncc.shape;out=[]
    for idx in order:
        y,x=divmod(int(idx),W)
        if any(abs(y-py)<min_dist and abs(x-px)<min_dist for py,px,_ in out):continue
        out.append((x,y,float(ncc[y,x])))
        if len(out)>=k:break
    return out

TMPLS = {
 "channel0": ("tmpl_channel0_normal.png", None),
 "chgauge":  ("tmpl_chgauge.png", None),
 "chBackgrn":("tmpl/btworld__BtWorld_chBackgrn.png", None),
 "chSelect3":("tmpl_chSelect3.png", None),
 "world97":  ("tmpl/world__world_97.png", (420,230,900,360)),
 "world98":  ("tmpl/world__world_98.png", (420,230,900,360)),
 "btworld_t0":("tmpl/btworld__BtWorld_t0_normal_0.png", None),
}
for shot in SHOTS:
    sp=os.path.join(BASE,shot)
    if not os.path.exists(sp): print("MISSING",shot); continue
    S=load_rgba(sp)
    print(f"\n===== {shot} ({S.shape[1]}x{S.shape[0]}) =====")
    for name,(tp,box) in TMPLS.items():
        t=load_rgba(tp)
        sub=S; off=(0,0)
        if box:
            x0,y0,x1,y1=box; sub=S[y0:y1,x0:x1]; off=(x0,y0)
        ncc=ncc_map(sub,t)
        for x,y,v in topk(ncc,t.shape[1],t.shape[0],3):
            print(f"{name:10s} ncc={v:.4f} TL=({x+off[0]},{y+off[1]}) BR=({x+off[0]+t.shape[1]},{y+off[1]+t.shape[0]})")
