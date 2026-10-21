#!/usr/bin/env python3
import sys
sys.path.insert(0, r"E:\project\MapleStory-Client\tools")
from nx_inspect import Nx

nx = Nx(r"E:\MXD\BeiDou-Client-xy\Data\.nxcache\Map002.nx")
base = "Map/Map1/100000000.img"

print("=== info ===")
for c in nx.children(nx.find(base + "/info")):
    print(nx.name(c), "=", nx.value(c))

print("=== back bS ===")
back = nx.find(base + "/back")
bs = set()
for c in nx.children(back):
    bS = nx.find(base + "/back/" + nx.name(c) + "/bS")
    no = nx.find(base + "/back/" + nx.name(c) + "/no")
    bsv = nx.value(bS).strip("'") if bS else None
    print("back", nx.name(c), "bS", bsv, "no", nx.value(no) if no else None)
    if bsv:
        bs.add(bsv)

print("=== layer tS / oS ===")
ts = set()
osets = set()
for layer in range(8):
    info = nx.find("%s/%d/info" % (base, layer))
    if info:
        for c in nx.children(info):
            if nx.name(c) == "tS":
                v = nx.value(c).strip("'")
                ts.add(v)
                print("layer", layer, "tS", v)
    obj = nx.find("%s/%d/obj" % (base, layer))
    if obj:
        for o in list(nx.children(obj))[:3]:
            for ch in nx.children(o):
                if nx.name(ch) == "oS":
                    v = nx.value(ch).strip("'")
                    osets.add(v)
                    print("layer", layer, "obj", nx.name(o), "oS", v)

mapnx = Nx(r"E:\MXD\BeiDou-Client-xy\Data\.nxcache\Map.nx")
map001 = Nx(r"E:\MXD\BeiDou-Client-xy\Data\.nxcache\Map001.nx")

print("=== presence ===")
for b in sorted(bs):
    print("Back", b + ".img", "in Map001", map001.find("Back/" + b + ".img") is not None)
for t in sorted(ts):
    print("Tile", t + ".img", "in Map", mapnx.find("Tile/" + t + ".img") is not None)
for o in sorted(osets):
    print("Obj", o + ".img", "in Map", mapnx.find("Obj/" + o + ".img") is not None)

all_tile = [mapnx.name(c) for c in mapnx.children(mapnx.find("Tile"))]
all_back = [map001.name(c) for c in map001.children(map001.find("Back"))]
all_obj = [mapnx.name(c) for c in mapnx.children(mapnx.find("Obj"))]
print("total tiles", len(all_tile), "backs", len(all_back), "objs", len(all_obj))
print("sample missing grassy?", "grassySoil.img" in all_tile, "midForest.img" in all_back)
