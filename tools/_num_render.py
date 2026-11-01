#!/usr/bin/env python3
import sys
sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz
from PIL import Image
from collections import Counter

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/UI/StatusBar.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

idx = img.find("number")
print("number node children:", [img.nodes[c]["name"] for c in img.nodes[idx]["children"]][:20])
# render a few glyphs
n0 = img.nodes[idx]["children"][0]
g0 = img.nodes[n0]["children"][0]
w, h, rb = img.decode_bitmap(g0)
print("glyph0", img.nodes[n0]["name"], w, h)
im = Image.frombytes("RGBA", (w, h), rb)
c = Counter()
for p in range(w * h):
    c[(rb[p*4], rb[p*4+1], rb[p*4+2])] += 1
print("glyph0 colors:", c.most_common(4))
im = Image.new("RGBA", (w, h), (120,120,120,255))
im.alpha_composite(Image.frombytes("RGBA", (w, h), rb))
im.resize((w*8, h*8), Image.NEAREST).save(r"E:/MXD/BeiDou-Client-xy/_sb_num0.png")
