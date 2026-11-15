import os, json
import numpy as np
from PIL import Image, ImageDraw

ROOT = r"C:\Users\11036\AppData\Local\Temp\term_verify"
OUT = os.path.join(ROOT, "ui_r5")
os.makedirs(OUT, exist_ok=True)
A = r"C:\Users\11036\.local\state\billion-context\retrieve\img\session-8943caa6-a38f-405b-a307-55370c5ccf7f\m08335-79794459.png"
B = r"C:\Users\11036\.local\state\billion-context\retrieve\img\session-8943caa6-a38f-405b-a307-55370c5ccf7f\m08337-745cb49d.png"
TARGET = os.path.join(ROOT, "targets_desktop", "desktop_char_select.png")
OURS = os.path.join(ROOT, "shots", "ui_final_charselect.png")
OURS2 = os.path.join(ROOT, "shots", "sidebar_fix_1_charselect.png")

def arr(p): return np.asarray(Image.open(p).convert("RGB"), dtype=np.float32)
def g(a): return a[...,0]*0.299+a[...,1]*0.587+a[...,2]*0.114

def locate(tpl_path, frame_path):
    t = arr(tpl_path); f = arr(frame_path)
    th,tw = t.shape[:2]; fh,fw = f.shape[:2]
    # alpha-less NCC over plausible region
    tg = g(t); tm = tg - tg.mean(); dt = float(np.sqrt((tm*tm).sum()))
    best=(-2.0,None,None)
    for y in range(120, min(460-th, fh-th)+1, 2):
        for x in range(280, min(680-tw, fw-tw)+1, 2):
            v = g(f[y:y+th,x:x+tw]); v = v-v.mean(); dv=float(np.sqrt((v*v).sum()))
            if dv<1e-6: continue
            n=float((v*tm).sum()/(dv*dt))
            if n>best[0]: best=(n,x,y)
    return {"ncc":round(best[0],4),"xy":[best[1],best[2]],"tpl":[tw,th]}

def ink_stats(p, cells):
    im=arr(p); res={}
    for name,(x0,y0,x1,y1) in cells.items():
        roi=im[y0:y1,x0:x1]; gr=g(roi)
        dark=gr<130
        ys,xs=np.where(dark)
        if len(xs)==0:
            res[name]={"dark_px":0}; continue
        rgb=roi[dark]
        res[name]={
            "dark_px":int(dark.sum()),
            "bbox":[x0+int(xs.min()),y0+int(ys.min()),x0+int(xs.max())+1,y0+int(ys.max())+1],
            "ink_rgb_mean":[round(float(rgb[:,i].mean()),1) for i in range(3)],
            "ink_rgb_median":[round(float(np.median(rgb[:,i])),1) for i in range(3)],
            "center_x":round(float(x0+xs.mean()),1),
            "center_y":round(float(y0+ys.mean()),1),
            "stroke_ratio":round(float(dark.sum()/max(1,(x1-x0)*(y1-y0))),3)
        }
    return res

# crops are 221x198 / 219x186; use common cells relative to their own panel top.
# panel top roller is at y~0..18; content rows start ~25.
cells_common = {
    "row_job":(20,24,120,48),
    "row_level":(20,48,120,72),
    "row_fame":(120,48,205,72),
    "row_str":(20,72,120,96),
    "row_int":(120,72,205,96),
    "row_dex":(20,96,120,120),
    "row_luk":(120,96,205,120),
    "rank_header":(20,120,205,148),
    "rank_na":(20,148,205,180),
}

res={"sources":{"A_79794459":A,"B_745cb49d":B,"target_full":TARGET,"ours_full":OURS,"ours2_full":OURS2},"locate":{},"ink":{}}
for lab,p in [("A_79794459",A),("B_745cb49d",B)]:
    res["locate"][lab]={
        "in_target_full":locate(p,TARGET),
        "in_ours_full":locate(p,OURS),
        "in_ours2_full":locate(p,OURS2) if os.path.exists(OURS2) else None,
    }
    res["ink"][lab]=ink_stats(p,cells_common)

# target/ours full-frame crops around known R3 charInfo bbox [392,220,575,335]
BOX=(380,200,590,360)
def crop(p): return Image.open(p).convert("RGB").crop(BOX)
for lab,p in [("target_charinfo_crop",TARGET),("ours_charinfo_crop",OURS),("ours2_charinfo_crop",OURS2)]:
    if os.path.exists(p):
        im=crop(p); im.save(os.path.join(OUT,lab+".png"))
        res.setdefault("crops",{})[lab]=os.path.join(OUT,lab+".png")

with open(os.path.join(OUT,"measure_r5_charinfo.json"),"w",encoding="utf-8") as f:
    json.dump(res,f,ensure_ascii=False,indent=2)
print(json.dumps(res,ensure_ascii=False,indent=2))

# 4x zoom sheet of A/B + target/ours crops
ims=[]
labels=[]
for lab,p in [("A",A),("B",B)]:
    im=Image.open(p).convert("RGB"); labels.append(lab); ims.append(im)
for lab,p in [("TARGET full crop",TARGET),("OURS full crop",OURS)]:
    if os.path.exists(p): im=crop(p); labels.append(lab); ims.append(im)
S=3
zoomed=[im.resize((im.width*S,im.height*S),Image.Resampling.NEAREST) for im in ims]
W=max(i.width for i in zoomed); H=sum(i.height+26 for i in zoomed)
sh=Image.new("RGB",(W,H),(235,235,235)); d=ImageDraw.Draw(sh); y=0
for lab,im in zip(labels,zoomed):
    d.text((4,y+4),lab,fill=(0,0,0)); y+=26; sh.paste(im,(0,y)); y+=im.height
sh.save(os.path.join(OUT,"charinfo_zoom_sheet.png")); print("SHEET",os.path.join(OUT,"charinfo_zoom_sheet.png"),sh.size)
