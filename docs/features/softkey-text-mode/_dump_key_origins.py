"""Authoritative geometry dump for UIWindow.img/SoftKeyboard from runtime UI.nx.

The key caps are laid out by each key/N/normal/0/origin VECTOR (a fixed shuffled
grid, anti-screenshot), NOT by key number order. This script reads every origin,
width and height straight from the PKG4 binary (nx_inspect.Nx), so the rewrite
does not depend on the earlier hand-transcribed grid (which had a duplicated x=207).

Output: evidence/keys/origins.json  (UTF-8, ensure_ascii=False)
"""
import json
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import nx_inspect as ni  # noqa: E402

NX_PATH = r"E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx"
OUT = os.path.join(HERE, "evidence", "keys", "origins.json")
BASE = "UIWindow.img/SoftKeyboard"

# vision-describe of keys_sheet.png (5x nearest, 10 cols, k00..k36):
# k00=0 k01=1 ... k09=9, k10=A ... k35=Z, k36=-
CHARS = [str(i) for i in range(10)] + [chr(ord("A") + i) for i in range(26)] + ["-"]

nx = ni.Nx(NX_PATH)


def node_of(path):
    return nx.find(path)


def children_map(idx):
    """Return {child_name: child_node_index} for a node."""
    out = {}
    if idx is None:
        return out
    for c in nx.children(idx):
        out[nx.name(c)] = c
    return out


def read_vector(idx):
    """Read an ntype==4 vector node -> (x, y).

    PKG4 stores a vector's x/y IN the node record: x = int32 @ 64+i*20+12
    (the 'data' slot), y = int32 @ +16 (the 'width'+'height' slot). `data`
    is NOT a file offset to read from. (See tools/nx_inspect.py value().)
    """
    if idx is None:
        return None
    _, _, _, ntype, _, _, _ = nx.node(idx)
    if ntype != 4:
        return None
    off = 64 + idx * 20
    x, y = struct.unpack_from("<ii", nx.data, off + 12)
    return [x, y]


def bitmap_info(path):
    """Return {w,h,origin} for a bitmap node at path, origin from its 'origin' child."""
    idx = node_of(path)
    if idx is None:
        return None
    _, _, _, ntype, _, w, h = nx.node(idx)
    if ntype != 5:
        return None
    org = read_vector(children_map(idx).get("origin"))
    return {"w": w, "h": h, "origin": org}


result = {
    "source": NX_PATH,
    "node": BASE,
    "chars_by_key_index": CHARS,
    "keys": {},
    "funckey": {},
    "backgrnd": {},
    "Bt": {},
}

# 1) 37 keys: normal/0 is the canonical bitmap; also record which of the 4 states exist
for n in range(37):
    states = {}
    for st in ("normal", "mouseOver", "pressed", "disabled"):
        info = bitmap_info("%s/key/%d/%s/0" % (BASE, n, st))
        if info is not None:
            states[st] = info
    result["keys"][str(n)] = {
        "char": CHARS[n],
        "states": states,
        "normal": states.get("normal"),
    }

# 2) funckey: 5 keys x 4 states
for name in ("capslock", "lshift", "rshift", "enter", "backspace"):
    states = {}
    for st in ("normal", "mouseOver", "pressed", "disabled"):
        info = bitmap_info("%s/funckey/%s/%s/0" % (BASE, name, st))
        if info is not None:
            states[st] = info
    result["funckey"][name] = {"states": states, "normal": states.get("normal")}

# 3) backgrnd skins 0/1: children back / keyboard / 1
for skin in ("0", "1"):
    skin_map = {}
    for child in ("back", "keyboard", "1"):
        info = bitmap_info("%s/backgrnd/%s/%s" % (BASE, skin, child))
        if info is not None:
            skin_map[child] = info
    result["backgrnd"][skin] = skin_map

# 4) Bt skins 0/1: BtClose/BtMin/BtMax x 4 states
for skin in ("0", "1"):
    skin_map = {}
    for bt in ("BtClose", "BtMin", "BtMax"):
        states = {}
        for st in ("normal", "mouseOver", "pressed", "disabled"):
            info = bitmap_info("%s/Bt/%s/%s/%s/0" % (BASE, skin, bt, st))
            if info is not None:
                states[st] = info
        if states:
            skin_map[bt] = {"states": states, "normal": states.get("normal")}
    result["Bt"][skin] = skin_map

with open(OUT, "w", encoding="utf-8") as f:
    json.dump(result, f, ensure_ascii=False, indent=2)

# compact console summary
print("wrote", OUT)
missing_origin = [k for k, v in result["keys"].items() if not (v["normal"] and v["normal"]["origin"])]
print("keys missing origin:", missing_origin)
print("\nkey grid (char @ x,y from normal/0/origin):")
byrow = {}
for n in range(37):
    nm = result["keys"][str(n)]["normal"]
    if not nm or not nm["origin"]:
        continue
    x, y = nm["origin"]
    byrow.setdefault(y, []).append((x, n, CHARS[n]))
for y in sorted(byrow):
    row = sorted(byrow[y])
    print("  y=%4d : " % y + " ".join("x%3d k%02d '%s'" % (x, n, c) for x, n, c in row))
print("\nfunckey normal origins:")
for name, v in result["funckey"].items():
    print("  %-10s %s" % (name, v["normal"]))
print("\nbackgrnd:")
for skin, m in result["backgrnd"].items():
    print("  skin", skin, {k: (v["w"], v["h"], v["origin"]) for k, v in m.items()})
print("\nBt:")
for skin, m in result["Bt"].items():
    for bt, v in m.items():
        print("  %s/%s %s" % (skin, bt, v["normal"]))
