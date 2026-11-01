#!/usr/bin/env python3
import sys
sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/UI/Login.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

def dump(idx, depth=0, maxdepth=2):
    n = img.nodes[idx]
    print("  "*depth + repr(n["name"]), n["type"], "w=%s h=%s" % (n.get("w"), n.get("h")), "extra=%s" % (n.get("extra"),))
    if depth < maxdepth:
        for c in n["children"]:
            dump(c, depth+1, maxdepth)

dump(img.find("Notice/text/3"), 0, 1)
dump(img.find("Notice/BtYes"), 0, 2)
