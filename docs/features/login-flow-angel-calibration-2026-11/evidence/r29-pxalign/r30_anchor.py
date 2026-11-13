import numpy as np
from PIL import Image

SHOT = r"E:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/选择频道页.png"

def load(p):
    im = Image.open(p).convert("RGBA")
    a = np.asarray(im).astype(np.float64)
    return a[...,:3].mean(2), a[...,3]/255.0

def ncc_map(shot_g, shot_a, tpl_g, tpl_a, box=None):
    th, tw = tpl_g.shape
    if box:
        x0,y0,x1,y1 = box
    else:
        x0,y0,x1,y1 = 0,0,shot_g.shape[1],shot_g.shape[0]
    y1 = min(y1, shot_g.shape[0]); x1 = min(x1, shot_g.shape[1])
    m = tpl_a > 0.5
    n = m.sum()
    if n < 30: return None
    tg = tpl_g[m]; tm = tg.mean(); tz = tg - tm
    tden = np.sqrt((tz**2).sum())
    if tden < 1e-6: return None
    H = y1-y0-th+1; W = x1-x0-tw+1
    if H<=0 or W<=0: return None
    from numpy.lib.stride_tricks import sliding_window_view
    win = sliding_window_view(shot_g[y0:y1, x0:x1], (th,tw))
    vals = win[:,:,m]                       # H,W,n
    wm = vals.mean(axis=2, keepdims=True)
    z = vals - wm
    num = (z*tz).sum(axis=2)
    den = np.sqrt((z**2).sum(axis=2))*tden
    with np.errstate(invalid='ignore', divide='ignore'):
        ncc = num/den
    ncc = np.nan_to_num(ncc, nan=-1.0)
    iy,ix = np.unravel_index(np.argmax(ncc), ncc.shape)
    return float(ncc[iy,ix]), (x0+int(ix), y0+int(iy)), (tw,th)

shot_g, shot_a = load(SHOT)
print("shot", shot_g.shape)

tests = [
 ("BtWorld_t0_normal", "tmpl/btworld__BtWorld_t0_normal_0.png", None),
 ("BtWorld_t1_normal", "tmpl/btworld__BtWorld_t1_normal_0.png", None),
 ("BtWorld_e_normal",  "tmpl/btworld__BtWorld_e_normal_0.png",  None),
 ("world_97", "tmpl/world__world_97.png", (420,230,900,340)),
 ("world_98", "tmpl/world__world_98.png", (420,230,900,340)),
 ("chBackgrn","tmpl/btworld__BtWorld_chBackgrn.png", None),
]
for name, tp, box in tests:
    tg, ta = load(tp)
    r = ncc_map(shot_g, shot_a, tg, ta, box)
    print(f"{name:20s} -> ncc={r[0]:.4f} TL={r[1]} size={r[2]}" if r else f"{name} none")
