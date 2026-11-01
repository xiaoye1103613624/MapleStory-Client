#!/usr/bin/env python3
import sys

sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz
from PIL import Image

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/UI/Login.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)


def rgba_of(idx):
    w, h, fb = img.decode_bitmap(idx)
    rgba = bytearray(w * h * 4)
    for i in range(w * h):
        b, g, r_, a = fb[i * 4:i * 4 + 4]
        rgba[i * 4:i * 4 + 4] = bytes((r_, g, b, a))
    return w, h, bytes(rgba)


for name in ("CharSelect/charInfo1", "CharSelect/charInfo"):
    w, h, rgba = rgba_of(img.find(name))
    opaque = sum(1 for i in range(w * h) if rgba[i * 4 + 3] > 128)
    print("%s: %dx%d opaque=%.1f%%" % (name, w, h, opaque * 100.0 / (w * h)))
    im = Image.frombytes("RGBA", (w, h), rgba)
    # composite over a mid-blue to see the art
    bg = Image.new("RGBA", (w, h), (90, 140, 200, 255))
    bg.paste(im, (0, 0), im)
    bg = bg.resize((w * 2, h * 2), Image.NEAREST)
    bg.save(r"E:/MXD/BeiDou-Client-xy/_dbg_%s.png" % name.replace("/", "_"))
    # also save with alpha
    im.resize((w * 2, h * 2), Image.NEAREST).save(r"E:/MXD/BeiDou-Client-xy/_dbg_alpha_%s.png" % name.replace("/", "_"))
