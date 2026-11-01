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

def render(path, out):
    w, h, rb = img.decode_bitmap(img.find(path))
    im = Image.frombytes("RGBA", (w, h), rb)
    im.save(out)
    print(path, w, h, "->", out)

render("Notice/text/3", r"E:/MXD/BeiDou-Client-xy/_n_text3.png")
render("Notice/text/0", r"E:/MXD/BeiDou-Client-xy/_n_text0.png")
# show some sample colors of text/3
w, h, rb = img.decode_bitmap(img.find("Notice/text/3"))
from collections import Counter
c = Counter()
for p in range(w*h):
    c[(rb[p*4], rb[p*4+1], rb[p*4+2])] += 1
print("text/3 top colors:", c.most_common(5))
