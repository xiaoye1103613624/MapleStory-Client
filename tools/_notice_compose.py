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

def get(path):
    w, h, rb = img.decode_bitmap(img.find(path))
    return Image.frombytes("RGBA", (w, h), rb)

bg = get("Notice/backgrnd/0")
t3 = get("Notice/text/3")
byes = get("Notice/BtYes/normal/0")
bno = get("Notice/BtNo/normal/0")
print("bg", bg.size, "text3", t3.size, "BtYes", byes.size, "BtNo", bno.size)

# candidate text positions: centered-x, various y
for i, ty in enumerate((40, 55, 70, 85)):
    comp = bg.copy()
    comp.alpha_composite(t3, ((bg.width - t3.width)//2, ty))
    # buttons like native: centered pair at bottom
    pw = byes.width + 12 + bno.width
    bx = (bg.width - pw)//2
    by = bg.height - byes.height - 24
    comp.alpha_composite(byes, (bx, by))
    comp.alpha_composite(bno, (bx + byes.width + 12, by))
    comp = comp.resize((bg.width*2, bg.height*2), Image.NEAREST)
    comp.save(r"E:/MXD/BeiDou-Client-xy/_n_comp%d.png" % i)
    print("saved _n_comp%d.png text_y=%d" % (i, ty))
