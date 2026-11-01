#!/usr/bin/env python3
"""Render WorldSelect panel assets: chBackgrn + scroll/0 frames, print origins."""
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

def origin(path):
    idx = img.find(path)
    n = img.nodes[idx]
    for c in n["children"]:
        if img.nodes[c]["name"] == "origin":
            o = img.nodes[c]
            return o.get("x"), o.get("y")
    return None

bg = get("WorldSelect/chBackgrn")
bg.save(r"E:/MXD/BeiDou-Client-xy/_ws_chbackgrn.png")
print("chBackgrn", bg.size, "origin", origin("WorldSelect/chBackgrn"))

frames = []
for i in range(4):
    f = get("WorldSelect/scroll/0/%d" % i)
    f.save(r"E:/MXD/BeiDou-Client-xy/_ws_scroll0_%d.png" % i)
    print("scroll/0/%d" % i, f.size, "origin", origin("WorldSelect/scroll/0/%d" % i))
    frames.append(f)

# 合成：末帧与 chBackgrn 对比
comp = Image.new("RGBA", (600, 500), (30, 30, 40, 255))
comp.alpha_composite(bg, (10, 10))
comp.alpha_composite(frames[3], (300, 10))
comp.save(r"E:/MXD/BeiDou-Client-xy/_ws_cmp.png")
print("saved _ws_chbackgrn.png _ws_scroll0_*.png _ws_cmp.png")
