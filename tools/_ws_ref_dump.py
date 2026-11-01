#!/usr/bin/env python3
"""Dump reference client Data/UI/Login.img WorldSelect subtree + render candidates."""
import sys
sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz
from PIL import Image

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-ClientV17.7/Data/UI/Login.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

def kids(idx):
    return [img.nodes[c]["name"] for c in img.nodes[idx]["children"]]

ws = img.find("WorldSelect")
print("WorldSelect children:", kids(ws))
try:
    ch = img.find("WorldSelect/chBackgrn")
    n = img.nodes[ch]
    print("chBackgrn:", n["type"], "w=%s h=%s" % (n.get("w"), n.get("h")))
    w, h, rb = img.decode_bitmap(ch)
    Image.frombytes("RGBA", (w, h), rb).save(r"E:/MXD/BeiDou-Client-xy/_ref_chbackgrn.png")
    print("rendered _ref_chbackgrn.png", (w, h))
except Exception as e:
    print("chBackgrn ERR", e)

sc = img.find("WorldSelect/scroll")
if sc is not None and sc >= 0:
    print("scroll children:", kids(sc))
    for sub in kids(sc):
        subkids = kids(img.find("WorldSelect/scroll/%s" % sub))
        sizes = []
        for k in subkids:
            n = img.nodes[img.find("WorldSelect/scroll/%s/%s" % (sub, k))]
            sizes.append("%s:%sx%s" % (k, n.get("w"), n.get("h")))
        print("  scroll/%s: %s" % (sub, sizes))
