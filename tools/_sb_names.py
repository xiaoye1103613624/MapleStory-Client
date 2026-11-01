#!/usr/bin/env python3
import sys
sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/UI/StatusBar.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

for c in img.nodes[img.root]["children"]:
    n = img.nodes[c]
    if n["type"] == "bitmap":
        print("%-14s bitmap %dx%d" % (n["name"], n["w"], n["h"]))
    elif n["children"]:
        # count numeric-looking children (charset)
        names = [img.nodes[x]["name"] for x in n["children"]]
        print("%-14s node  children=%d %s" % (n["name"], len(names), names[:16]))
    else:
        print("%-14s %s" % (n["name"], n["type"]))
