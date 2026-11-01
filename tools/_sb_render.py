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

def render(path, out, scale=2):
    w, h, rb = img.decode_bitmap(img.find(path))
    im = Image.frombytes("RGBA", (w, h), rb)
    bg = Image.new("RGBA", (w, h), (200, 200, 200, 255))
    bg.alpha_composite(im)
    bg = bg.resize((w * scale, h * scale), Image.NEAREST)
    bg.save(out)
    print(path, w, h)

render("base/backgrnd2", r"E:/MXD/BeiDou-Client-xy/_sb_bg2.png", 2)
render("base/quickSlot", r"E:/MXD/BeiDou-Client-xy/_sb_qs.png", 1)
render("base/chatTarget", r"E:/MXD/BeiDou-Client-xy/_sb_ct.png", 3)
