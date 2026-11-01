#!/usr/bin/env python3
"""半透明叠图：correct_shots vs current_shots → evidence/r20/overlay_out/

用法：把当前端 1280x720 截图放进 current_shots/ 后运行本脚本。
"""
from __future__ import annotations

import os
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    raise SystemExit("pip install Pillow")

ROOT = Path(__file__).resolve().parent
CORRECT = ROOT / "correct_shots"
CURRENT = ROOT / "current_shots"
OUT = ROOT / "overlay_out"


def main() -> None:
    OUT.mkdir(exist_ok=True)
    names = sorted(p.name for p in CORRECT.glob("*.png"))
    if not names:
        raise SystemExit("no correct_shots")
    done = 0
    for name in names:
        cur = CURRENT / name
        if not cur.exists():
            print(f"SKIP missing current: {name}")
            continue
        a = Image.open(CORRECT / name).convert("RGBA")
        b = Image.open(cur).convert("RGBA")
        if a.size != b.size:
            b = b.resize(a.size, Image.Resampling.BILINEAR)
            print(f"WARN resize {name} -> {a.size}")
        overlay = Image.blend(a, b, 0.45)
        overlay.save(OUT / name)
        done += 1
        print(f"OK {name}")
    print(f"done={done} out={OUT}")


if __name__ == "__main__":
    main()
