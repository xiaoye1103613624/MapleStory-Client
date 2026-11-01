#!/usr/bin/env python3
import sys
sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz
from PIL import Image

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/UI/StatusBar.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

w, h, rb = img.decode_bitmap(img.find("base/backgrnd2"))
art = Image.frombytes("RGBA", (w, h), rb)
bg = Image.new("RGBA", (w, h), (120, 120, 120, 255))
bg.alpha_composite(art)
# label row region of the art: x=210..570 y=36..71, zoom 4x
bg.crop((210, 36, 570, 71)).resize((360 * 4, 35 * 4), Image.NEAREST).save(r"E:/MXD/BeiDou-Client-xy/_sb_bg2_labels.png")
# name chip region: x=0..220 y=36..71 zoom 4x
bg.crop((0, 36, 220, 71)).resize((220 * 4, 35 * 4), Image.NEAREST).save(r"E:/MXD/BeiDou-Client-xy/_sb_bg2_chip.png")
print("saved art crops")

# native name chip zoom
p = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/打开了S-E-I-K页面.png"
im = Image.open(p).convert("RGB")
im.crop((40, 678, 220, 716)).resize((180 * 6, 38 * 6), Image.NEAREST).save(r"E:/MXD/BeiDou-Client-xy/_native_namechip.png")
print("saved native chip")
