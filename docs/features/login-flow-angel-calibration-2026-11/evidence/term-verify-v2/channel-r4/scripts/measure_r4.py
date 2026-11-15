import os, json, io
import numpy as np
from PIL import Image, ImageDraw

ROOT = r"C:\Users\11036\AppData\Local\Temp\term_verify"
WZ = os.path.join(ROOT, "ui_r4", "wz")
OUT = os.path.join(ROOT, "ui_r4")
USER = r"C:\Users\11036\.local\state\billion-context\retrieve\img\session-8943caa6-a38f-405b-a307-55370c5ccf7f\m08207-15d4d79d.webp"
TARGET = os.path.join(ROOT, "targets_desktop", "desktop_channel_expanded.png")
OURS = os.path.join(ROOT, "shots", "ch20_home_f05.png")

os.makedirs(OUT, exist_ok=True)

def arr(p):
    return np.asarray(Image.open(p).convert("RGB"), dtype=np.float32)

def gray(a):
    return a[...,0]*0.299 + a[...,1]*0.587 + a[...,2]*0.114

def load_tpl(name):
    im = Image.open(os.path.join(WZ, name + ".png")).convert("RGBA")
    a = np.asarray(im, dtype=np.float32)
    rgb, alpha = a[...,:3], a[...,3]
    return rgb, alpha, im.size

def ncc_best(img, tplname, roi, step=1, alpha_thr=128, bottom_crop=None):
    rgb, alpha, (w,h) = load_tpl(tplname)
    if bottom_crop:
        y0 = h - bottom_crop
        rgb, alpha = rgb[y0:,:,:], alpha[y0:,:]
        h = bottom_crop
    x0,y0,x1,y1 = roi
    m = alpha > alpha_thr
    if m.sum() < 20:
        return {"error":"template too sparse", "valid": int(m.sum())}
    tm = gray(rgb)[m]
    tm = tm - tm.mean()
    denom_t = np.sqrt((tm*tm).sum())
    best = (-2, None, None, None)
    H,W = img.shape[:2]
    for y in range(y0, min(y1-h, H-h)+1, step):
        for x in range(x0, min(x1-w, W-w)+1, step):
            win = img[y:y+h, x:x+w]
            v = gray(win)[m]
            v = v - v.mean()
            denom_v = np.sqrt((v*v).sum())
            if denom_v < 1e-6 or denom_t < 1e-6:
                continue
            n = float((v*tm).sum()/(denom_v*denom_t))
            if n > best[0]:
                best = (n,x,y,(w,h))
    return {"ncc": round(best[0],4), "xy": [best[1], best[2]], "size": list(best[3]) if best[3] else None}

def band_ink(img, roi, thr=115):
    x0,y0,x1,y1 = roi
    g = gray(img[y0:y1,x0:x1])
    dark = g < thr
    rows = dark.sum(axis=1)
    active = np.where(rows > 2)[0]
    if len(active)==0:
        return {"rows": rows.tolist(), "bbox": None, "weighted": None}
    return {
        "rows": rows.tolist(),
        "bbox": [x0+int(np.where(dark)[1].min()), y0+int(active.min()), x0+int(np.where(dark)[1].max())+1, y0+int(active.max())+1],
        "weighted": round(float((active*rows[active]).sum()/rows[active].sum()),2),
        "peak": [y0+int(active[np.argmax(rows[active])]), int(rows.max())]
    }

imgs = {"target": arr(TARGET), "user_current": arr(USER), "ours_ch20": arr(OURS)}
res = {"sources": {"target": TARGET, "user_current": USER, "ours_ch20": OURS}, "measurements": {}}

for label,img in imgs.items():
    r = {}
    r["channel0"] = ncc_best(img, "WorldSelect_channel_0_normal", (420,300,820,490), 1, 200)
    r["notice_title"] = ncc_best(img, "WorldNotice_BalloonForLoginProcess_Title", (240,280,500,360), 1, 200)
    r["notice_nw"] = ncc_best(img, "WorldNotice_BalloonForLoginProcess_nw", (240,260,340,350), 1, 128)
    r["notice_se"] = ncc_best(img, "WorldNotice_BalloonForLoginProcess_se", (420,330,500,400), 1, 128)
    r["noticescroll1_top"] = ncc_best(img, "NoticeScroll1_0", (380,460,940,550), 1, 128)
    r["noticescroll2_top"] = ncc_best(img, "NoticeScroll2_0", (380,460,940,550), 1, 128)
    r["scroll_open_bottom"] = ncc_best(img, "WorldSelect_scroll_0_3", (380,460,940,550), 1, 128, bottom_crop=40)
    r["scroll_open_full"] = ncc_best(img, "WorldSelect_scroll_0_3", (360,100,460,200), 2, 128)
    r["btgoworld"] = ncc_best(img, "WorldSelect_BtGoworld_normal_0", (720,470,940,560), 1, 128)
    r["balloon_body_ink"] = band_ink(img, (300,320,450,360), 115)
    r["balloon_title_ink"] = band_ink(img, (330,300,420,330), 115)
    res["measurements"][label] = r

for a,b in [("target","user_current"),("target","ours_ch20"),("user_current","ours_ch20")]:
    mae = float(np.abs(imgs[a]-imgs[b]).mean())
    res.setdefault("pairwise",{})[f"{a}_vs_{b}_mean_abs"] = round(mae,3)

with open(os.path.join(OUT,"measure_r4.json"),"w",encoding="utf-8") as f:
    json.dump(res,f,ensure_ascii=False,indent=2)
print(json.dumps(res,ensure_ascii=False,indent=2))

# side-by-side full frames + bottom strip + notice strip
def sheet(paths, box, scale, labels, out):
    crops=[]
    for p in paths:
        im=Image.open(p).convert("RGB").crop(box)
        if scale!=1: im=im.resize((im.width*scale, im.height*scale), Image.Resampling.NEAREST)
        crops.append(im)
    w=max(c.width for c in crops); h=sum(c.height+24 for c in crops)
    sh=Image.new("RGB",(w,h),(240,240,240)); d=ImageDraw.Draw(sh); y=0
    for lab,im in zip(labels,crops):
        d.text((4,y+4),lab,fill=(0,0,0)); y+=24; sh.paste(im,(0,y)); y+=im.height
    sh.save(out); print("SHEET",out,sh.size)

sheet([TARGET,USER,OURS],(230,250,940,560),2,["TARGET","USER_CURRENT","OURS_CH20"],os.path.join(OUT,"cmp_full_r4.png"))
sheet([TARGET,USER,OURS],(400,460,930,550),3,["TARGET BOTTOM","USER BOTTOM","OURS BOTTOM"],os.path.join(OUT,"cmp_bottom_r4.png"))
sheet([TARGET,USER,OURS],(240,260,500,400),3,["TARGET NOTICE","USER NOTICE","OURS NOTICE"],os.path.join(OUT,"cmp_notice_r4.png"))
