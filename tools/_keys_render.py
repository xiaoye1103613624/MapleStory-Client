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

imgs = []
for i in range(8):
    w, h, rb = img.decode_bitmap(img.find("key/%d" % i))
    im = Image.frombytes("RGBA", (w, h), rb)
    print("key/%d %dx%d" % (i, w, h))
    imgs.append(im)

total_w = sum(im.width for im in imgs) + 10 * len(imgs)
mh = max(im.height for im in imgs)
sheet = Image.new("RGBA", (total_w, mh), (150, 150, 150, 255))
x = 0
for im in imgs:
    sheet.alpha_composite(im, (x, 0))
    x += im.width + 10
sheet = sheet.resize((total_w * 3, mh * 3), Image.NEAREST)
sheet.save(r"E:/MXD/BeiDou-Client-xy/_sb_keys.png")
print("saved")
