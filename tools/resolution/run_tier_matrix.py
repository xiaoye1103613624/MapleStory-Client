#!/usr/bin/env python3
"""多分辨率 9 档逐档实机矩阵：自动写档位、启动客户端、记录窗口尺寸与崩溃。

用法：
    python tools/resolution/run_tier_matrix.py --tiers 0,1,2,3,4 --seconds 42
    python tools/resolution/run_tier_matrix.py --tiers 5,6,7,8 --seconds 42
    python tools/resolution/run_tier_matrix.py --all --restore-tier 5

每档输出 evidence/live/tier<i>-<WxH>.json；最后可用 --restore-tier 把
Settings 恢复成指定档位（默认 5 = 1280x720，避免把验证档位留给用户）。
只读/写被测客户端的 Settings 与 evidence，不改源码、不发包。
"""
import argparse
import json
import re
import subprocess
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from monitor_viewport import enum_windows  # noqa: E402
from verify_viewport import user32  # noqa: E402

TIERS = [
    (800, 600), (1024, 768), (1366, 768), (1600, 900), (1920, 1080),
    (1280, 720), (1440, 900), (1680, 1050), (2560, 1440),
]

EXE = Path(r"E:\MXD\BeiDou-Client-xy\MapleStory.exe")
SETTINGS = Path(r"E:\MXD\BeiDou-Client-xy\Settings")
OUT_DIR = Path(__file__).resolve().parents[2] / "docs" / "features" / "multi-resolution" / "evidence" / "live"


def write_tier(tier):
    width, height = TIERS[tier]
    text = SETTINGS.read_bytes().decode("utf-8")
    text = re.sub(r"(?m)^(\s*game_width:)\s*\d+", r"\g<1> %d" % width, text)
    text = re.sub(r"(?m)^(\s*game_height:)\s*\d+", r"\g<1> %d" % height, text)
    text = re.sub(r"(?m)^(\s*game_tier:)\s*\d+", r"\g<1> %d" % tier, text)
    SETTINGS.write_bytes(text.encode("utf-8"))
    return width, height


def run_tier(tier, seconds, interval):
    width, height = write_tier(tier)
    proc = subprocess.Popen([str(EXE)], cwd=str(EXE.parent))
    start = time.time()
    samples = []

    while time.time() - start < seconds:
        time.sleep(interval)
        rc = proc.poll()
        samples.append({
            "t": round(time.time() - start, 1),
            "alive": rc is None,
            "exitcode": rc,
            "sizes": [[w["client_w"], w["client_h"]] for w in enum_windows(proc.pid)] if rc is None else [],
        })
        if rc is not None:
            break

    if proc.poll() is None:
        proc.terminate()
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()
        proc.wait(timeout=5)

    reached = any([width, height] in s["sizes"] for s in samples)
    login_sizes = all(all(size in ([1280, 720], [width, height]) for size in s["sizes"])
                      for s in samples if s["sizes"])
    crashed = any(s["exitcode"] is not None and s["exitcode"] != 0 for s in samples)

    result = {
        "tier": tier,
        "expected": [width, height],
        "desktop": [int(user32.GetSystemMetrics(0)), int(user32.GetSystemMetrics(1))],
        "duration_s": seconds,
        "samples": samples,
        "verdict": {
            "login_1280x720": login_sizes,
            "reached_expected": reached,
            "crashed": crashed,
            "pass": login_sizes and reached and not crashed,
        },
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    out = OUT_DIR / ("tier%d-%dx%d.json" % (tier, width, height))
    out.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    return {"tier": tier, "expected": "%dx%d" % (width, height),
            "pass": result["verdict"]["pass"], "crashed": crashed,
            "reached": reached, "file": out.name}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--tiers", default="")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--seconds", type=float, default=42.0)
    ap.add_argument("--interval", type=float, default=1.5)
    ap.add_argument("--restore-tier", type=int, default=5)
    args = ap.parse_args()

    tiers = list(range(len(TIERS))) if args.all else [int(x) for x in args.tiers.split(",") if x.strip()]
    if not tiers:
        print("no tiers selected")
        return 2

    rows = []
    for tier in tiers:
        row = run_tier(tier, args.seconds, args.interval)
        rows.append(row)
        print(json.dumps(row, ensure_ascii=False), flush=True)

    write_tier(args.restore_tier)
    print(json.dumps({"restored_tier": args.restore_tier,
                      "passed": sum(1 for r in rows if r["pass"]),
                      "total": len(rows)}, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    sys.exit(main())
