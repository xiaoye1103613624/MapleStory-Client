#!/usr/bin/env python3
import sys
sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/UI/StatusBar.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

def tree(idx, depth=0, maxdepth=3):
    n = img.nodes[idx]
    extra = ""
    if n["type"] == "bitmap":
        extra = " %dx%d" % (n["w"], n["h"])
    elif n["type"] == "vector":
        extra = " v(%s,%s)" % (n.get("x"), n.get("y"))
    print("  "*depth + str(n["name"]) + extra)
    if depth < maxdepth:
        for c in n["children"]:
            tree(c, depth+1, maxdepth)
    elif n["children"]:
        print("  "*(depth+1) + "... %d children" % len(n["children"]))

tree(img.root, 0, 2)
