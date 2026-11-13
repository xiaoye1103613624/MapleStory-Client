#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Convert OmniParser /parse/ JSON (or MCP parse_screen dump) into skill layout-*.json.

Usage:
  python omni_to_layout.py --image ../evidence/screens/login.png \\
      --parse parse_login.json --out ../notes/layout-login.json \\
      --window CUITitle --why "主路径残余：BtLogin 相对木牌 1px"

If --parse is omitted, POSTs base64 image to OMNIPARSER_API_URL/parse/ when reachable.
"""
from __future__ import annotations

import argparse
import base64
import json
import os
import sys
import urllib.error
import urllib.request
from pathlib import Path


def load_parse(path: Path | None, image: Path, api: str) -> dict:
    if path and path.is_file():
        return json.loads(path.read_text(encoding="utf-8"))
    raw = image.read_bytes()
    b64 = base64.b64encode(raw).decode("ascii")
    body = json.dumps({"base64_image": b64}).encode("utf-8")
    req = urllib.request.Request(
        api.rstrip("/") + "/parse/",
        data=body,
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=120) as resp:
            return json.loads(resp.read().decode("utf-8"))
    except urllib.error.URLError as e:
        print(f"OmniParser unreachable ({e}); write empty elements stub.", file=sys.stderr)
        return {"parsed_content_list": [], "error": str(e)}


def elements_from_parse(data: dict, w: int, h: int) -> list[dict]:
    out = []
    items = data.get("parsed_content_list") or data.get("elements") or []
    for i, it in enumerate(items):
        # OmniParser usually: bbox [x1,y1,x2,y2] normalized 0-1 OR pixel
        bbox = it.get("bbox") or it.get("bbox_px") or it.get("bounding_box")
        content = it.get("content") or it.get("text") or it.get("caption") or ""
        typ = it.get("type") or it.get("interactivity") or "unknown"
        name = it.get("name") or (content[:32] if content else f"e{i+1}")
        if not bbox or len(bbox) < 4:
            continue
        x1, y1, x2, y2 = [float(v) for v in bbox[:4]]
        # Heuristic: values in 0..1.5 => normalized
        if max(x1, y1, x2, y2) <= 1.5:
            bx, by = x1 * w, y1 * h
            bw, bh = (x2 - x1) * w, (y2 - y1) * h
            bbox_norm = [x1, y1, x2 - x1, y2 - y1]
        else:
            bx, by, bw, bh = x1, y1, x2 - x1, y2 - y1
            bbox_norm = [bx / w, by / h, bw / w, bh / h]
        out.append(
            {
                "id": f"e{i+1}",
                "name": str(name).strip() or f"e{i+1}",
                "type": str(typ),
                "bbox_norm": [round(v, 6) for v in bbox_norm],
                "bbox_px": [int(round(bx)), int(round(by)), int(round(bw)), int(round(bh))],
                "wz_hint": it.get("wz_hint") or "",
                "content": str(content),
            }
        )
    return out


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--image", required=True, type=Path)
    ap.add_argument("--parse", type=Path, default=None)
    ap.add_argument("--out", required=True, type=Path)
    ap.add_argument("--window", default="unknown")
    ap.add_argument("--why", default="主路径已尽仍有视觉残差")
    ap.add_argument("--client-version", default="v083")
    ap.add_argument(
        "--api",
        default=os.environ.get("OMNIPARSER_API_URL", "http://127.0.0.1:8010"),
    )
    args = ap.parse_args()

    try:
        from PIL import Image

        with Image.open(args.image) as im:
            w, h = im.size
    except Exception:
        w = h = 0

    data = load_parse(args.parse, args.image, args.api)
    elements = elements_from_parse(data, w or 1, h or 1)
    doc = {
        "schema_version": 1,
        "source": "screenshot-fallback",
        "client": {"version": args.client_version, "scale": "1x", "window": args.window},
        "image": {
            "path": str(args.image).replace("\\", "/"),
            "width_px": w,
            "height_px": h,
        },
        "parser": {
            "tool": "omniparser-mcp",
            "refined_by": None,
            "api": args.api,
        },
        "why_fallback": args.why,
        "elements": elements,
    }
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(doc, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"wrote {args.out} elements={len(elements)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
