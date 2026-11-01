# zz/qf_s12_img1view.py: 3x half views of img1 with 10px grid labels.
import os, hashlib, json
from PIL import Image, ImageDraw
ZZ = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-qf-work"
man = json.load(io.open(os.path.join(ZZ, "manifest.json"), encoding="utf-8")) if False else None
import io
man = json.load(io.open(os.path.join(ZZ, "manifest.json"), encoding="utf-8"))
p = os.path.join(ZZ, "src", "img1.png")
assert hashlib.md5(open(p, "rb").read()).hexdigest() == man["img1"]["md5"]
im = Image.open(p).convert("RGB")
W, H = im.size
S = 3
g = im.resize((W*S, H*S), Image.NEAREST)
d = ImageDraw.Draw(g)
for x in range(0, W, 10):
    d.line([(x*S, 0), (x*S, H*S)], fill=(0, 255, 0), width=1)
    d.rectangle([x*S, 0, x*S+17, 10], fill=(0, 0, 0))
    d.text((x*S+1, 1), str(x), fill=(255, 255, 0))
for y in range(0, H, 10):
    d.line([(0, y*S), (W*S, y*S)], fill=(0, 255, 0), width=1)
    d.rectangle([0, y*S, 14, y*S+10], fill=(0, 0, 0))
    d.text((1, y*S+1), str(y), fill=(0, 255, 255))
g.crop((0, 0, W*S, 200*S)).save(os.path.join(ZZ, "crop", "img1_top3x.png"))
g.crop((0, 190*S, W*S, H*S)).save(os.path.join(ZZ, "crop", "img1_bot3x.png"))
print("OK12")
