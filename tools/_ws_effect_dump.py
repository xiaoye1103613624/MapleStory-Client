#!/usr/bin/env python3
"""Render map signboard assets for visual check."""
import sys
sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz
from PIL import Image

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/Map/Obj/login.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

for p in ("WorldSelect/signboard/0/0", "WorldSelect/signboard/1/0", "WorldSelect/signboard/2/0"):
    w, h, rb = img.decode_bitmap(img.find(p))
    im = Image.frombytes("RGBA", (w, h), rb)
    out = r"E:/MXD/BeiDou-Client-xy/_mapsb_%s.png" % p.split("/")[-2]
    im.save(out)
    print(p, im.size, "->", out)
