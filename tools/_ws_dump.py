#!/usr/bin/env python3
"""Dump Login.img/WorldSelect subtree with sizes/origins to find native expand animation nodes."""
import sys
sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/UI/Login.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

def dump(name, depth=0, maxdepth=3):
    try:
        idx = img.find(name)
    except Exception as e:
        print("ERR", name, e); return
    if idx is None or idx < 0:
        print("MISSING:", name); return

    def walk(i, d):
        n = img.nodes[i]
        ind = "  " * d
        extra = ""
        if n["type"] == "bitmap" or n.get("w"):
            extra = " w=%s h=%s" % (n.get("w"), n.get("h"))
        print(ind + repr(n["name"]), n["type"], extra)
        if d < maxdepth:
            for c in n["children"]:
                walk(c, d + 1)

    walk(idx, 0)

for p in ("WorldSelect", "WorldSelect/scroll", "WorldSelect/chBackgrn", "WorldSelect/channel"):
    print("=== %s ===" % p)
    dump(p)
