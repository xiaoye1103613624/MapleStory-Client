#!/usr/bin/env python3
import sys
sys.path.insert(0, r"E:\project\MapleStory-Client\tools")
from nx_inspect import Nx

nx = Nx(r"E:\MXD\BeiDou-Client-xy\Data\.nxcache\Map002.nx")
mapnx = Nx(r"E:\MXD\BeiDou-Client-xy\Data\.nxcache\Map.nx")
map001 = Nx(r"E:\MXD\BeiDou-Client-xy\Data\.nxcache\Map001.nx")

for mapname in ["000000000.img", "000000001.img", "000000002.img"]:
    base = "Map/Map0/" + mapname
    print("========", mapname)
    info = nx.find(base + "/info")
    if not info:
        print(" MISSING"); continue
    for key in ["bgm", "VRLeft", "VRRight", "VRTop", "VRBottom", "mapMark"]:
        n = nx.find(base + "/info/" + key)
        print(" ", key, "=", nx.value(n) if n else None)
    back = nx.find(base + "/back")
    bs = set()
    if back:
        for c in nx.children(back):
            bS = nx.find(base + "/back/" + nx.name(c) + "/bS")
            if bS:
                bs.add(nx.value(bS).strip("'"))
    ts = set(); osets = set()
    for layer in range(8):
        info = nx.find("%s/%d/info" % (base, layer))
        if info:
            for c in nx.children(info):
                if nx.name(c) == "tS":
                    ts.add(nx.value(c).strip("'"))
        obj = nx.find("%s/%d/obj" % (base, layer))
        if obj:
            for o in nx.children(obj):
                for ch in nx.children(o):
                    if nx.name(ch) == "oS":
                        osets.add(nx.value(ch).strip("'"))
    print(" backs", sorted(bs))
    print(" tiles", sorted(ts))
    print(" objs", sorted(osets)[:20], "count", len(osets))
    for b in sorted(bs):
        print("  Back", b, map001.find("Back/" + b + ".img") is not None)
    for t in sorted(ts):
        print("  Tile", t, mapnx.find("Tile/" + t + ".img") is not None)
    for o in sorted(osets):
        ok = mapnx.find("Obj/" + o + ".img") is not None
        if not ok:
            print("  MISSING Obj", o)
