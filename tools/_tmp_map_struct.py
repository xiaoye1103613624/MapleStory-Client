#!/usr/bin/env python3
import sys
sys.path.insert(0, r"E:\project\MapleStory-Client\tools")
from nx_inspect import Nx

m = Nx(r"E:\MXD\BeiDou-Client-xy\Data\.nxcache\Map002.nx")

for mid in ["000000001", "000000002", "000000003", "000000004"]:
    print("====", mid)
    root = "Map/Map0/%s.img" % mid
    for sub in ["info", "back", "portal", "foothold", "life", "0", "1", "2", "3", "4", "5", "6", "7", "miniMap"]:
        n = m.find("%s/%s" % (root, sub))
        if n is None:
            print(" ", sub, "MISSING")
            continue
        kids = list(m.children(n))
        print(" ", sub, "node", n, "children", len(kids), "val", m.value(n))
        if sub in ("0", "1", "2") and kids:
            for k in kids[:8]:
                print("    ", m.name(k), m.value(k), "ck", len(list(m.children(k))))
    # portals detail
    portal = m.find(root + "/portal")
    if portal:
        for c in list(m.children(portal))[:5]:
            pn = m.name(c)
            base = root + "/portal/" + pn + "/"
            def v(k):
                n = m.find(base + k)
                return None if n is None else m.value(n)
            print("  portal[%s] pn=%s pt=%s x=%s y=%s tm=%s" % (pn, v("pn"), v("pt"), v("x"), v("y"), v("tm")))
