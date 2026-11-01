# -*- coding: utf-8 -*-
from PIL import Image, ImageDraw, ImageFont
import os
import shutil

src = r"E:\project\MapleStory-Client\docs\features\login-layout-1024-fix\evidence\bt-viewchoice.png"
out_dir = r"E:\project\MapleStory-Client\docs\features\beidou-v17-parity\ui\worldselect-sidebar\v1"
deploy = r"E:\MXD\BeiDou-Client-xy\UI\WorldSelectSidebar"
os.makedirs(out_dir, exist_ok=True)
os.makedirs(deploy, exist_ok=True)

img = Image.open(src).convert("RGBA")
w, h = img.size
print("template", w, h)

draw = ImageDraw.Draw(img)
margin_x, margin_y = 6, 5
arrow_reserve = 18
draw.rectangle([margin_x, margin_y, w - arrow_reserve, h - margin_y], fill=(90, 40, 30, 255))

text = "查看所有角色"
font = None
for fp in [
    r"C:\Windows\Fonts\msyh.ttc",
    r"C:\Windows\Fonts\simhei.ttf",
    r"C:\Windows\Fonts\simsun.ttc",
]:
    if os.path.exists(fp):
        try:
            font = ImageFont.truetype(fp, 12)
            print("font", fp)
            break
        except Exception as e:
            print("font fail", fp, e)

if font is None:
    font = ImageFont.load_default()

bbox = draw.textbbox((0, 0), text, font=font)
tw, th = bbox[2] - bbox[0], bbox[3] - bbox[1]
tx = margin_x + max(0, (w - arrow_reserve - margin_x - tw) // 2)
ty = (h - th) // 2 - 1
draw.text((tx, ty), text, fill=(255, 255, 255, 255), font=font)

normal = os.path.join(out_dir, "BtViewAll_normal.png")
img.save(normal)

mo = img.copy()
px = mo.load()
for y in range(h):
    for x in range(w):
        r, g, b, a = px[x, y]
        if a > 0:
            px[x, y] = (min(255, r + 20), min(255, g + 20), min(255, b + 15), a)
mo_path = os.path.join(out_dir, "BtViewAll_mouseOver.png")
mo.save(mo_path)

shutil.copy(normal, os.path.join(deploy, "BtViewAll_normal.png"))
shutil.copy(mo_path, os.path.join(deploy, "BtViewAll_mouseOver.png"))
print("ok", normal, mo_path)
