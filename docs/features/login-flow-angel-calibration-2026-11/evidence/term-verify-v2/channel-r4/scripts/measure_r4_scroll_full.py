import os, json
import numpy as np
from PIL import Image

ROOT = r"C:\Users\11036\AppData\Local\Temp\term_verify"
WZ = os.path.join(ROOT, "ui_r4", "wz")
OUT = os.path.join(ROOT, "ui_r4")
USER = r"C:\Users\11036\.local\state\billion-context\retrieve\img\session-8943caa6-a38f-405b-a307-55370c5ccf7f\m08207-15d4d79d.webp"
TARGET = os.path.join(ROOT, "targets_desktop", "desktop_channel_expanded.png")
OURS = os.path.join(ROOT, "shots", "ch20_home_f05.png")

def arr(p):
    return np.asarray(Image.open(p).convert("RGB"), dtype=np.float32)

def gray(a):
    return a[...,0]*0.299 + a[...,1]*0.587 + a[...,2]*0.114

def load_tpl(name):
    im = Image.open(os.path.join(WZ, name + ".png")).convert("RGBA")
    a = np.asarray(im, dtype=np.float32)
    return a[...,:3], a[...,3], im.size

def ncc_best(img, tplname, roi, coarse=4, fine=1, alpha_thr=128):
    rgb, alpha, (w,h) = load_tpl(tplname)
    m = alpha > alpha_thr
    tm = gray(rgb)[m]
    tm = tm - tm.mean()
    denom_t = float(np.sqrt((tm*tm).sum()))
    x0,y0,x1,y1 = roi
    H,W = img.shape[:2]
    x1 = min(x1-w, W-w); y1 = min(y1-h, H-h)
    best = (-2.0, None, None)
    for y in range(y0, y1+1, coarse):
        for x in range(x0, x1+1, coarse):
            v = gray(img[y:y+h, x:x+w])[m]
            v = v - v.mean()
            dv = float(np.sqrt((v*v).sum()))
            if dv < 1e-6:
                continue
            n = float((v*tm).sum()/(dv*denom_t))
            if n > best[0]:
                best = (n,x,y)
    if best[1] is None:
        return {"error":"roi too small", "roi":[x0,y0,x1,y1], "tpl":[w,h]}
    cx,cy = best[1],best[2]
    finebest = (best[0],cx,cy)
    for y in range(max(y0,cy-coarse), min(y1,cy+coarse)+1, fine):
        for x in range(max(x0,cx-coarse), min(x1,cx+coarse)+1, fine):
            v = gray(img[y:y+h, x:x+w])[m]
            v = v - v.mean()
            dv = float(np.sqrt((v*v).sum()))
            if dv < 1e-6:
                continue
            n = float((v*tm).sum()/(dv*denom_t))
            if n > finebest[0]:
                finebest = (n,x,y)
    return {"ncc": round(finebest[0],4), "xy":[finebest[1],finebest[2]], "size":[w,h], "roi":[x0,y0,x1,y1]}

imgs = {"target": arr(TARGET), "user_current": arr(USER), "ours_ch20": arr(OURS)}
res = {"sources": {"target": TARGET, "user_current": USER, "ours_ch20": OURS}, "full_frame_ncc": {}}
# scroll/0/3 = 513x401 opened scroll. Search whole plausible placement area.
ROI = (340, 90, 950, 560)
for label,img in imgs.items():
    r = {}
    r["scroll_0_3_open"] = ncc_best(img, "WorldSelect_scroll_0_3", ROI, 4, 1, 128)
    r["scroll_1_0_open"] = ncc_best(img, "WorldSelect_scroll_1_0", ROI, 4, 1, 128)
    r["scroll_0_0_collapsed"] = ncc_best(img, "WorldSelect_scroll_0_0", ROI, 4, 1, 128)
    res["full_frame_ncc"][label] = r

with open(os.path.join(OUT,"measure_r4_scroll_full.json"),"w",encoding="utf-8") as f:
    json.dump(res,f,ensure_ascii=False,indent=2)
print(json.dumps(res,ensure_ascii=False,indent=2))
