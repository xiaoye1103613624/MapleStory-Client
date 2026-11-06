# -*- coding: utf-8 -*-
"""Dump SoftKey from Login_img.json (wz_dump flat schema)."""
import json
from pathlib import Path

src = Path(r"E:\MXD\BeiDou-Client-xy\REA反编译\wz_dump\Login_img.json")
out = Path(__file__).with_name("evidence")
out.mkdir(exist_ok=True)
report = out / "softkey-img-dump.txt"

data = json.load(src.open(encoding="utf-8"))
common = data["Common"]
# SoftKey may be nested under Common children or direct
sk = None
if "SoftKey" in common:
    sk = common["SoftKey"]
elif "children" in common and "SoftKey" in common["children"]:
    sk = common["children"]["SoftKey"]
else:
    # search
    def find(o):
        if isinstance(o, dict):
            if o.get("_dirName") == "SoftKey" or "SoftKey" in o:
                if "SoftKey" in o and isinstance(o["SoftKey"], dict):
                    return o["SoftKey"]
                if o.get("_dirName") == "SoftKey":
                    return o
            for v in o.values():
                r = find(v)
                if r:
                    return r
        return None
    sk = find(common)

lines = [f"source={src}", f"found={sk is not None}"]


def walk(node, path, depth=0):
    if not isinstance(node, dict):
        return
    name = node.get("_dirName", path.split("/")[-1])
    typ = node.get("_dirType", "?")
    w = node.get("_width", node.get("width"))
    h = node.get("_height", node.get("height"))
    origin = None
    if "origin" in node and isinstance(node["origin"], dict):
        o = node["origin"]
        origin = (o.get("_x", o.get("x")), o.get("_y", o.get("y")))
    # also nested children.origin
    ch = node.get("children")
    if origin is None and isinstance(ch, dict) and "origin" in ch:
        o = ch["origin"]
        origin = (o.get("_x", o.get("x")), o.get("_y", o.get("y")))
    extra = ""
    if w is not None:
        extra += f" {w}x{h}"
    if origin is not None:
        extra += f" origin={origin}"
    lines.append("  " * depth + f"{path} [{typ}]{extra}")

    # iterate keys that look like children
    keys = []
    if isinstance(ch, dict):
        keys = list(ch.items())
    else:
        skip = {"_dirName", "_dirType", "_width", "_height", "origin", "z", "delay", "_value", "width", "height", "children"}
        keys = [(k, v) for k, v in node.items() if k not in skip and isinstance(v, dict)]

    for k, v in keys:
        if k in ("origin", "z", "delay"):
            continue
        walk(v, f"{path}/{k}", depth + 1)


walk(sk, "SoftKey", 0)

# compact button origin table: SoftKey/<btn>/normal/0
lines.append("\n=== button normal/0 origins ===")


def find_normals(node, path):
    if not isinstance(node, dict):
        return
    if path.endswith("/normal/0") or path.endswith("/normal"):
        w = node.get("_width", node.get("width"))
        h = node.get("_height", node.get("height"))
        origin = None
        if "origin" in node:
            o = node["origin"]
            origin = (o.get("_x", o.get("x")), o.get("_y", o.get("y")))
        ch = node.get("children")
        if origin is None and isinstance(ch, dict) and "origin" in ch:
            o = ch["origin"]
            origin = (o.get("_x", o.get("x")), o.get("_y", o.get("y")))
        # if this is normal folder, dig into 0
        if "0" in node:
            find_normals(node["0"], path + "/0")
            return
        if isinstance(ch, dict) and "0" in ch:
            find_normals(ch["0"], path + "/0")
            return
        lines.append(f"{path}: {w}x{h} origin={origin}")
        return
    # recurse
    ch = node.get("children")
    items = []
    if isinstance(ch, dict):
        items = list(ch.items())
    else:
        skip = {"_dirName", "_dirType", "_width", "_height", "origin", "z", "delay"}
        items = [(k, v) for k, v in node.items() if k not in skip and isinstance(v, dict)]
    for k, v in items:
        if k in ("origin", "z", "delay"):
            continue
        find_normals(v, f"{path}/{k}")


find_normals(sk, "SoftKey")

# list SoftKey top-level child names
lines.append("\n=== SoftKey top children ===")
if isinstance(sk, dict):
    ch = sk.get("children")
    if isinstance(ch, dict):
        for k in ch:
            lines.append(f"  {k}")
    else:
        for k, v in sk.items():
            if k.startswith("_"):
                continue
            if isinstance(v, dict):
                lines.append(f"  {k} [{v.get('_dirType')}]")

text = "\n".join(lines) + "\n"
report.write_text(text, encoding="utf-8")
print(text)
print("wrote", report)
