# -*- coding: utf-8 -*-
"""Dump SoftKey subtree from Login.img_runtime.json for origin/keypos audit."""
import json
import sys
from pathlib import Path

src = Path(r"E:\MXD\BeiDou-Client-xy\Login.img_runtime.json")
out = Path(__file__).with_name("evidence")
out.mkdir(exist_ok=True)
report = out / "softkey-img-dump.txt"


def find_by_name(obj, name):
    if not isinstance(obj, dict):
        return None
    if obj.get("_dirName") == name:
        return obj
    ch = obj.get("children")
    if isinstance(ch, dict):
        for v in ch.values():
            r = find_by_name(v, name)
            if r:
                return r
    elif isinstance(ch, list):
        for v in ch:
            r = find_by_name(v, name)
            if r:
                return r
    return None


def show(node, lines, indent=0, maxd=5):
    if indent > maxd or not isinstance(node, dict):
        return
    name = node.get("_dirName") or "?"
    typ = node.get("_dirType") or "?"
    extra = ""
    if "width" in node or "height" in node:
        extra += f" {node.get('width')}x{node.get('height')}"
    if typ == "vector":
        extra += f" ({node.get('x')},{node.get('y')})"
    lines.append("  " * indent + f"{name} [{typ}]{extra}")
    ch = node.get("children")
    if isinstance(ch, dict):
        for v in ch.values():
            show(v, lines, indent + 1, maxd)
    elif isinstance(ch, list):
        for v in ch[:40]:
            show(v, lines, indent + 1, maxd)


def dump_canvases(node, path, rows):
    if not isinstance(node, dict):
        return
    name = node.get("_dirName", "?")
    typ = node.get("_dirType")
    cur = f"{path}/{name}" if path else name
    if typ in ("canvas", "bitmap", "png") or ("width" in node and "height" in node and typ not in ("directory", "imgdir", "uol")):
        origin = None
        ch = node.get("children") or {}
        if isinstance(ch, dict) and "origin" in ch:
            o = ch["origin"]
            origin = (o.get("x"), o.get("y"))
        elif typ == "vector":
            origin = (node.get("x"), node.get("y"))
        rows.append((cur, node.get("width"), node.get("height"), origin, typ))
    ch = node.get("children")
    if isinstance(ch, dict):
        for v in ch.values():
            dump_canvases(v, cur, rows)
    elif isinstance(ch, list):
        for v in ch:
            dump_canvases(v, cur, rows)


def main():
    with src.open("r", encoding="utf-8") as f:
        data = json.load(f)
    sk = find_by_name(data, "SoftKey")
    lines = []
    lines.append(f"source={src}")
    lines.append(f"SoftKey_found={sk is not None}")
    if not sk:
        report.write_text("\n".join(lines), encoding="utf-8")
        print("SoftKey not found")
        return 1
    lines.append("=== tree ===")
    show(sk, lines, 0, 6)
    rows = []
    dump_canvases(sk, "", rows)
    lines.append("=== canvases / sized nodes ===")
    for cur, w, h, origin, typ in rows:
        lines.append(f"{cur}: {w}x{h} origin={origin} typ={typ}")
    # also list immediate SoftKey children names
    ch = sk.get("children") or {}
    if isinstance(ch, dict):
        lines.append("=== SoftKey direct children ===")
        for k, v in ch.items():
            typ = v.get("_dirType") if isinstance(v, dict) else type(v).__name__
            name = v.get("_dirName") if isinstance(v, dict) else k
            lines.append(f"  {name} [{typ}]")
    text = "\n".join(lines) + "\n"
    report.write_text(text, encoding="utf-8")
    print(text)
    print(f"wrote {report}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
