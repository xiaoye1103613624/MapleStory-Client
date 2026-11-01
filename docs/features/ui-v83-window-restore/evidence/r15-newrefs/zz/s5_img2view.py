# zz/s5_img2view: 3x view with 10px grid + row ruler, saved as two halves for legibility.
import os
from PIL import Image, ImageDraw
ZZ = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs\zz"
S = 3
im = Image.open(os.path.join(ZZ, "src", "img2.png")).convert("RGB")
W, H = im.size
g = im.resize((W*S, H*S), Image.NEAREST)
d = ImageDraw.Draw(g)
for x in range(0, W, 10):
    d.line([(x*S, 0), (x*S, H*S)], fill=(0, 255, 0), width=1)
    d.rectangle([x*S, 0, x*S+18, 10], fill=(0, 0, 0))
    d.text((x*S+1, 1), str(x), fill=(255, 255, 0))
for y in range(0, H, 10):
    d.line([(0, y*S), (W*S, y*S)], fill=(0, 255, 0), width=1)
    d.rectangle([0, y*S, 14, y*S+10], fill=(0, 0, 0))
    d.text((1, y*S+1), str(y), fill=(0, 255, 255))
g.crop((0, 0, W*S, 100*S)).save(os.path.join(ZZ, "crop", "img2_grid_top.png"))
g.crop((0, 90*S, W*S, H*S)).save(os.path.join(ZZ, "crop", "img2_grid_bot.png"))
print("OK5")
