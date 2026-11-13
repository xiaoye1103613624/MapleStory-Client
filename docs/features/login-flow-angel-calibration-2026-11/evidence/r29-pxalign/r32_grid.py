import numpy as np, os
from PIL import Image

SP = r"E:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/选中频道页.png"
def load_rgba(p): return np.asarray(Image.open(p).convert("RGBA")).astype(np.float32)
def corr(f,t,os_):
    H,W=f.shape;h,w=t.shape;pad=(H+h,W+w)
    F=np.fft.rfft2(f,pad);T=np.fft.rfft2(t,pad)
    c=np.fft.irfft2(F*np.conj(T),pad);return c[:os_[0],:os_[1]]
def ncc_map(s,t):
    H,W=s.shape[:2];h,w=t.shape[:2];oy,ox=H-h+1,W-w+1
    g=s[...,:3].mean(2);a=t[...,3]/255.0;tt=t[...,:3].mean(2)
    S=float(a.sum());Sg=corr(g,a,(oy,ox));Sgg=corr(g*g,a,(oy,ox));Sgt=corr(g,a*tt,(oy,ox))
    St=float((a*tt).sum());Stt=float((a*tt*tt).sum())
    mu_g=Sg/S;mu_t=St/S;var_g=np.maximum(Sgg/S-mu_g*mu_g,0.0);var_t=max(Stt/S-mu_t*mu_t,1e-9)
    cov=Sgt/S-mu_g*mu_t;ncc=cov/(np.sqrt(var_g)*np.sqrt(var_t)+1e-6)
    return np.nan_to_num(ncc)
t=load_rgba("tmpl_channel0_normal.png")
S=load_rgba(SP)
# search tight box around the channel grid
x0,y0,x1,y1=400,300,1000,520
sub=S[y0:y1,x0:x1]
ncc=ncc_map(sub,t)
H,W=ncc.shape;order=np.argsort(ncc.ravel())[::-1]
out=[];thr=0.70
for idx in order:
    y,x=divmod(int(idx),W)
    v=float(ncc[y,x])
    if v<thr: break
    if any(abs(y-py)<14 and abs(x-px)<14 for py,px,_ in out):continue
    out.append((x0+x,y0+y,v))
out.sort()
print(f"channel0 peaks above {thr}: {len(out)}")
for x,y,v in out: print(f"  TL=({x},{y}) ncc={v:.3f}")
# fit grid
xs=sorted(set(p[0] for p in out)); ys=sorted(set(p[1] for p in out))
print("xs:",xs)
print("ys:",ys)
