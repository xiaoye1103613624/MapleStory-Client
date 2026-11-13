# Alpha-masked zero-mean NCC template matcher (r29 pixel alignment).
# ncc[y,x] uses template top-left anchored at (y,x); transparent template
# pixels are excluded from every window statistic.
# Usage: python r29_ncc_match.py <shot.png> <template.png> [search_box x0,y0,x1,y1]
import sys
import numpy as np
from PIL import Image

def load_rgba(p):
    return np.asarray(Image.open(p).convert("RGBA")).astype(np.float32)

def corr(f, t, out_shape):
    """corr[y,x] = sum_{dy,dx} f[y+dy,x+dx]*t[dy,dx] via FFT (TL anchor)."""
    H, W = f.shape; h, w = t.shape
    pad = (H + h, W + w)
    F = np.fft.rfft2(f, pad); T = np.fft.rfft2(t, pad)
    c = np.fft.irfft2(F * np.conj(T), pad)
    return c[:out_shape[0], :out_shape[1]]

def ncc_map(s, t):
    H, W = s.shape[:2]; h, w = t.shape[:2]
    oy, ox = H - h + 1, W - w + 1
    g = s[..., :3].mean(axis=2)
    a = t[..., 3] / 255.0
    tt = t[..., :3].mean(axis=2)
    S = float(a.sum())
    Sg = corr(g, a, (oy, ox))
    Sgg = corr(g * g, a, (oy, ox))
    Sgt = corr(g, a * tt, (oy, ox))
    St = float((a * tt).sum()); Stt = float((a * tt * tt).sum())
    mu_g = Sg / S; mu_t = St / S
    var_g = np.maximum(Sgg / S - mu_g * mu_g, 0.0)
    var_t = max(Stt / S - mu_t * mu_t, 1e-9)
    cov = Sgt / S - mu_g * mu_t
    ncc = cov / (np.sqrt(var_g) * np.sqrt(var_t) + 1e-6)
    return np.nan_to_num(ncc)

def peaks(ncc, k=10, min_dist=25):
    order = np.argsort(ncc.ravel())[::-1]
    out = []; H, W = ncc.shape
    for idx in order:
        y, x = divmod(int(idx), W)
        if any(abs(y - py) < min_dist and abs(x - px) < min_dist for py, px, _ in out):
            continue
        out.append((y, x, float(ncc[y, x])))
        if len(out) >= k: break
    return out

def main():
    shot_p, tmpl_p = sys.argv[1], sys.argv[2]
    box = tuple(int(v) for v in sys.argv[3].split(",")) if len(sys.argv) > 3 else None
    shot = load_rgba(shot_p); tmpl = load_rgba(tmpl_p)
    off = (0, 0)
    if box:
        x0, y0, x1, y1 = box; shot = shot[y0:y1, x0:x1]; off = (x0, y0)
    ncc = ncc_map(shot, tmpl)
    print(f"maskedNCC shot={shot_p} tmpl={tmpl_p} {tmpl.shape[1]}x{tmpl.shape[0]}")
    for y, x, v in peaks(ncc):
        print(f"ncc={v:.4f}  TL=({x+off[0]},{y+off[1]})  BR=({x+off[0]+tmpl.shape[1]},{y+off[1]+tmpl.shape[0]})")

if __name__ == "__main__":
    main()
