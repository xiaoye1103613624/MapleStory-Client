#!/usr/bin/env python3
"""Render Map/Back/login.img back/5 and back/9."""
import sys
sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz
from PIL import Image

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/Map/Back/login.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

for name in ("back/5", "back/9"):
    w, h, rb = img.decode_bitmap(img.find(name))
    im = Image.frombytes("RGBA", (w, h), rb)
    out = r"E:/MXD/BeiDou-Client-xy/_mapback_%s.png" % name.replace("/", "_")
    im.save(out)
    print(name, im.size, "->", out)
