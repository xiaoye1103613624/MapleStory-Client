#!/usr/bin/env python3
import sys
sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/UI/Login.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

def coverage(idx):
    w, h, rb = img.decode_bitmap(idx)
    total = w * h
    opaque = sum(1 for p in range(total) if rb[p*4+3] >= 200)
    semi = sum(1 for p in range(total) if 30 < rb[p*4+3] < 200)
    return w, h, opaque*100.0/total, semi*100.0/total

text_node = img.find("Notice/text")
print("Notice/text child count:", len(img.nodes[text_node]["children"]))
for c in img.nodes[text_node]["children"]:
    name = img.nodes[c]["name"]
    try:
        w, h, op, se = coverage(c)
        print("text/%-3s %dx%d opaque=%.1f%% semi=%.1f%%" % (name, w, h, op, se))
    except Exception as e:
        print("text/%-3s ERR %s" % (name, e))

print("---- backgrnd ----")
bg = img.find("Notice/backgrnd")
for c in img.nodes[bg]["children"]:
    name = img.nodes[c]["name"]
    try:
        w, h, op, se = coverage(c)
        print("backgrnd/%-2s %dx%d opaque=%.1f%%" % (name, w, h, op))
    except Exception as e:
        print("backgrnd/%-2s ERR %s" % (name, e))
