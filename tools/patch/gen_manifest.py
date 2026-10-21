#!/usr/bin/env python3
"""Generate patch manifest.json by scanning final .img / .wz / .nx files."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path


EXTS = {".img", ".wz", ".nx"}


def sha256_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def format_of(path: Path) -> str:
    return path.suffix.lstrip(".").lower()


def main() -> int:
    ap = argparse.ArgumentParser(description="Generate MapleStory client patch manifest")
    ap.add_argument("--root", required=True, help="Publish root directory to scan")
    ap.add_argument("--base-url", required=True, help="HTTP base URL for files")
    ap.add_argument("--out", default="manifest.json", help="Output manifest path")
    ap.add_argument("--channel", default="stable")
    ap.add_argument("--client-min-version", default="228.3")
    ap.add_argument(
        "--path-prefix",
        default="",
        help="Prefix prepended to each relative path (e.g. Data when root is the Data folder)",
    )
    args = ap.parse_args()

    root = Path(args.root).resolve()
    if not root.is_dir():
        raise SystemExit(f"root not a directory: {root}")

    files = []
    for dirpath, _, filenames in os.walk(root):
        for name in filenames:
            p = Path(dirpath) / name
            if p.suffix.lower() not in EXTS:
                continue
            rel = p.relative_to(root).as_posix()
            if args.path_prefix:
                prefix = args.path_prefix.replace("\\", "/").rstrip("/")
                rel = f"{prefix}/{rel}"
            files.append(
                {
                    "path": rel,
                    "size": p.stat().st_size,
                    "sha256": sha256_file(p),
                    "format": format_of(p),
                }
            )

    files.sort(key=lambda x: x["path"])
    manifest = {
        "clientMinVersion": args.client_min_version,
        "channel": args.channel,
        "baseUrl": args.base_url.rstrip("/") + "/",
        "files": files,
    }

    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(manifest, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"wrote {out} ({len(files)} files)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
