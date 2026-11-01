#!/usr/bin/env python3
"""Dump Map/Obj/login.img/WorldSelect subtree."""
import sys
sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/Map/Obj/login.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

def walk(i, d, maxd):
    n = img.nodes[i]
    ind = "  " * d
    extra = ""
    if n.get("w"):
        extra = " w=%s h=%s" % (n.get("w"), n.get("h"))
    print(ind + repr(n["name"]), n["type"], extra)
    if d < maxd:
        for c in n["children"]:
            walk(c, d + 1, maxd)

try:
    idx = img.find("WorldSelect")
    walk(idx, 0, 4)
except Exception as e:
    print("ERR", e)
    # 顶层列表
    for c in img.nodes[0]["children"]:
        print("TOP:", repr(img.nodes[c]["name"]), img.nodes[c]["type"])
