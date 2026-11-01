#!/usr/bin/env python3
import sys

sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/UI/Login.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

for i in (0, 1, 2, 3, 16, 36, 47, 51, 53):
    idx = img.find("Notice/text/%d" % i)
    if idx is None:
        continue
    w, h, fb = img.decode_bitmap(idx)
    total = w * h
    opaque = sum(1 for p in range(total) if fb[p * 4 + 3] > 200)
    semi = sum(1 for p in range(total) if 30 < fb[p * 4 + 3] <= 200)
    print("text/%-3d %dx%d opaque=%.0f%% semi=%.0f%%" % (i, w, h, opaque * 100.0 / total, semi * 100.0 / total))

# also backgrnd plates
for i in (0, 1, 2):
    idx = img.find("Notice/backgrnd/%d" % i)
    w, h, fb = img.decode_bitmap(idx)
    total = w * h
    opaque = sum(1 for p in range(total) if fb[p * 4 + 3] > 200)
    print("backgrnd/%d %dx%d opaque=%.0f%%" % (i, w, h, opaque * 100.0 / total))
