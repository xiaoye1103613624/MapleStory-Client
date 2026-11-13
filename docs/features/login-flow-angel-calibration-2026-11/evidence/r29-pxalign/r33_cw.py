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
def subpx(ncc,y,x):
    H,W=ncc.shape;ys=max(1,y-1);ye=min(H-2,y+1);xs=max(1,x-1);xe=min(W-2,x+1)
    ny=0.5*(ncc[y-1,x]-ncc[y+1,x])/(ncc[y-1,x]-2*ncc[y,x]+ncc[y+1,x]) if (ncc[y-1,x]-2*ncc[y,x]+ncc[y+1,x])!=0 else 0
    nx=0.5*(ncc[y,x-1]-ncc[y,x+1])/(ncc[y,x-1]-2*ncc[y,x]+ncc[y,x+1]) if (ncc[y,x-1]-2*ncc[y,x]+ncc[y,x+1])!=0 else 0
    return x+nx, y+ny

t=load_rgba("tmpl_channel0_normal.png")
S=load_rgba(SP)
x0,y0,x1,y1=400,300,1000,540
sub=S[y0:y1,x0:x1]
ncc=ncc_map(sub,t)
H,W=ncc.shape;order=np.argsort(ncc.ravel())[::-1]
peaks=[];thr=0.80
for idx in order:
    y,x=divmod(int(idx),W);v=float(ncc[y,x])
    if v<thr:break
    if any(abs(y-py)<14 and abs(x-px)<14 for py,px,_ in peaks):continue
    peaks.append((x0+x,y0+y,v))
peaks.sort()
print(f"strong channel0 peaks (ncc>{thr}): {len(peaks)}")
for px,py,v in peaks: print(f"  TL=({px},{py}) ncc={v:.3f}")
# group into columns by x
xs=sorted(set(p[0] for p in peaks))
print("distinct x:",xs)
# sub-pixel refine each distinct x using its best peak
col_x=[]
for cx in xs:
    best=max([p for p in peaks if p[0]==cx],key=lambda p:p[2])
    fx,fy=subpx(ncc,best[1]-y0,best[0]-x0)
    col_x.append((x0+fx,best[2]))
col_x.sort()
print("sub-pixel col x:",[round(c[0],2) for c in col_x])
if len(col_x)>=2:
    # estimate step from consecutive diffs
    diffs=[round(col_x[i+1][0]-col_x[i][0],2) for i in range(len(col_x)-1)]
    print("consecutive steps:",diffs)
    if len(col_x)>=4:
        step=(col_x[-1][0]-col_x[0][0])/(len(col_x)-1)
        print(f"robust step (first->last / (n-1)) = {step:.2f}")
