#!/usr/bin/env python3
import sys
sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz
from PIL import Image

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/UI/StatusBar.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

def tree(idx, depth=0, maxdepth=2, prefix=""):
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

tree(img.find("base"), 0, 2)
print("---- buttons ----")
for b in ("BtShop", "BtNPT", "BtMenu", "BtShort", "EquipKey", "InvenKey",
          "StatKey", "SkillKey", "KeySet", "QuickSlot", "QuickSlotD",
          "BtHelp", "BtContents", "BtInterface", "BtClaim", "BtWhisper"):
    idx = img.find("base/" + b) if img.find("base/" + b) is not None else None
    if idx is None:
        print(b, ": MISSING")
        continue
    n0 = img.nodes[idx]["children"]
    first = img.nodes[n0[0]]["children"][0] if n0 and img.nodes[n0[0]]["children"] else None
    if first is not None and img.nodes[first]["type"] == "bitmap":
        print("%s: normal/0 %dx%d" % (b, img.nodes[first]["w"], img.nodes[first]["h"]))
    else:
        print(b, ": (no normal/0 bitmap)")
