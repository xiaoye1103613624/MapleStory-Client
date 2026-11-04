#!/usr/bin/env python3
"""多分辨率实机取证：启动部署客户端，按固定间隔记录窗口客户区尺寸与进程存活。

用法：
    python tools/resolution/monitor_viewport.py --label tier6 --seconds 55 --interval 2 --out evidence/live/tier6.json

只读系统状态（EnumWindows/GetClientRect）+ 启动/结束被测进程；
不发送任何键鼠输入、不修改配置文件（档位由调用方写 Settings）。
输出 JSON：label / desktop / samples[{t, alive, exitcode, windows[]}] / summary。

判据：
  - 登录态窗口恒 1280x720；
  - 进入游戏后窗口变为所选档位；
  - alive 突然变 false 且 exitcode 为负数 = 崩溃（配合 Application Error 1000 判型）。
"""
import argparse
import ctypes
import json
import subprocess
import sys
import time
from ctypes import wintypes
from pathlib import Path

user32 = ctypes.windll.user32
EnumWindowsProc = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)


class RECT(ctypes.Structure):
    _fields_ = [("left", wintypes.LONG), ("top", wintypes.LONG),
                ("right", wintypes.LONG), ("bottom", wintypes.LONG)]


TITLE_HINTS = ("XiaoYE", "Maple", "Admin", "北斗")


def inspect_window(hwnd):
    if not user32.IsWindowVisible(hwnd):
        return None

    buf = ctypes.create_unicode_buffer(512)
    user32.GetWindowTextW(hwnd, buf, 512)
    title = buf.value
    if not title or not any(k in title for k in TITLE_HINTS):
        return None

    owner_pid = wintypes.DWORD()
    user32.GetWindowThreadProcessId(hwnd, ctypes.byref(owner_pid))

    cr = RECT()
    wr = RECT()
    user32.GetClientRect(hwnd, ctypes.byref(cr))
    user32.GetWindowRect(hwnd, ctypes.byref(wr))
    return {
        "hwnd": int(hwnd),
        "pid": int(owner_pid.value),
        "title": title,
        "client_w": cr.right - cr.left,
        "client_h": cr.bottom - cr.top,
        "window_rect": [wr.left, wr.top, wr.right, wr.bottom],
    }


def enum_windows(only_pid=None):
    """枚举客户端窗口；only_pid 用于排除并发实例的窗口（实测会污染逐档取证）。"""
    found = []

    @EnumWindowsProc
    def cb(hwnd, _):
        item = inspect_window(hwnd)
        if item and (only_pid is None or item["pid"] == only_pid):
            found.append(item)
        return True

    user32.EnumWindows(cb, 0)
    return found


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--label", required=True)
    ap.add_argument("--exe", default=r"E:\MXD\BeiDou-Client-xy\MapleStory.exe")
    ap.add_argument("--seconds", type=float, default=55.0)
    ap.add_argument("--interval", type=float, default=2.0)
    ap.add_argument("--out", default="")
    ap.add_argument("--keep", action="store_true", help="结束后保留客户端进程（供后续 UI 操作）")
    args = ap.parse_args()

    exe = Path(args.exe)
    if not exe.is_file():
        print(json.dumps({"error": "exe not found", "exe": str(exe)}))
        return 2

    proc = subprocess.Popen([str(exe)], cwd=str(exe.parent))
    start = time.time()
    samples = []

    while time.time() - start < args.seconds:
        time.sleep(args.interval)
        rc = proc.poll()
        samples.append({
            "t": round(time.time() - start, 1),
            "alive": rc is None,
            "exitcode": rc,
            "windows": enum_windows(proc.pid) if rc is None else [],
        })
        if rc is not None:
            break

    sizes = []
    for s in samples:
        for w in s["windows"]:
            size = (w["client_w"], w["client_h"])
            if size not in sizes:
                sizes.append(size)

    result = {
        "label": args.label,
        "exe": str(exe),
        "pid": proc.pid,
        "desktop_w": int(user32.GetSystemMetrics(0)),
        "desktop_h": int(user32.GetSystemMetrics(1)),
        "samples": samples,
        "summary": {
            "distinct_client_sizes": [list(s) for s in sizes],
            "crashed": any(s["exitcode"] is not None and s["exitcode"] != 0 for s in samples),
            "last_alive": samples[-1]["alive"] if samples else False,
        },
    }

    if not args.keep and proc.poll() is None:
        proc.terminate()
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()

    text = json.dumps(result, ensure_ascii=False, indent=2)
    if args.out:
        out = Path(args.out)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(text + "\n", encoding="utf-8")
        print(json.dumps({
            "label": args.label,
            "written": str(out),
            "summary": result["summary"],
        }, ensure_ascii=False))
    else:
        print(text)
    return 0


if __name__ == "__main__":
    sys.exit(main())
