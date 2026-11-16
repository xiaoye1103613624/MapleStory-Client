#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""R7 audit: scroll rod + nametag vertical alignment vs target; channel bottom scroll."""
import numpy as np
from PIL import Image, ImageDraw
TV = r"C:\Users\11036\AppData\Local\Temp\term_verify"

def arr(p):
    return np.asarray(Image.open(p).convert("RGB"), np.float64)

T  = arr(TV + r"\targets_desktop\desktop_char_select.png")
R6 = arr(TV + r"\shots\r6_charselect_f05.png")
R7 = arr(TV + r"\shots\r7_charselect_f00.png")
CH = arr(TV + r"\targets_desktop\desktop_channel_expanded.png")
CH4= arr(TV + r"\shots\r4_home_f05.png")

print("=== page check (mean_abs; low=char page) ===")
for nm, g in (("R6", R6), ("R7", R7)):
    print(nm, "vs char", round(float(abs(g-T).mean()),2),
          "vs channel", round(float(abs(g-CH).mean()),2))

def ncc(a, b):
    a = a.ravel() - a.mean(); b = b.ravel() - b.mean()
    d = np.sqrt((a*a).sum()*(b*b).sum())
    return float((a*b).sum()/d) if d > 1e-9 else -2.0

def best_dy(tpl, img, x0, x1, y0, y1, span):
    best = (-2.0, 0)
    for dy in range(-span, span+1):
        yy0, yy1 = y0+dy, y0+dy+(y1-y0)
        if yy0 < 0 or yy1 > img.shape[0]:
            continue
        c = ncc(tpl, img[yy0:yy1, x0:x1])
        if c > best[0]:
            best = (c, dy)
    return best

print("=== scroll rod (target band y195..215, x400..560) ===")
rod = T[195:215, 400:560]
for nm, g in (("R6", R6), ("R7", R7)):
    c, dy = best_dy(rod, g, 400, 560, 195, 215, 45)
    print(nm, "dy", dy, "ncc", round(c,3))

print("=== nametag (target band y430..452, x466..492) ===")
tag = T[430:452, 466:492]
for nm, g in (("R6", R6), ("R7", R7)):
    c, dy = best_dy(tag, g, 466, 492, 430, 452, 40)
    print(nm, "dy", dy, "ncc", round(c,3))

print("=== channel bottom scroll (y520..545, x440..920) dark count ===")
for y in range(520, 546):
    tc = int((np.mean(CH[y,440:920],axis=1) < 110).sum())
    oc = int((np.mean(CH4[y,440:920],axis=1) < 110).sum())
    if tc or oc:
        print(y, "target", tc, "ours", oc)

def crop(im, box, s=2):
    c = im[int(box[1]):int(box[3]), int(box[0]):int(box[2])].astype(np.uint8)
    return Image.fromarray(c).resize((c.shape[1]*s, c.shape[0]*s), Image.NEAREST)

parts = []
for nm, g in (("TARGET", T), ("R6", R6), ("R7", R7)):
    parts.append((nm, crop(g, (370, 155, 600, 360), 2)))
w = parts[0][1].width; h = parts[0][1].height
sheet = Image.new("RGB", (w*3+40, h+40), (25,25,25))
d = ImageDraw.Draw(sheet)
for i,(nm,im) in enumerate(parts):
    x = 10+i*(w+10)
    d.text((x,5), nm, fill=(255,255,120))
    sheet.paste(im,(x,20))
sheet.save(TV + r"\ui_r5\cmp_r7_scroll_tag.png")
print("saved cmp_r7_scroll_tag.png", sheet.size)
