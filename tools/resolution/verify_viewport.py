#!/usr/bin/env python3
"""多分辨率实机验证：只读枚举 MapleStory 窗口的客户区尺寸。

用法：
    python tools/resolution/verify_viewport.py [label]

输出 JSON 到 stdout（由调用方重定向到 evidence/live/*.json），字段：
    label, desktop_w, desktop_h,
    windows[] = [{ hwnd, title, client_w, client_h, window_rect }]
desktop_* 用于判定档位是否超桌面；只读、不发送任何输入、不修改系统状态。
"""
import ctypes
import json
import sys
from ctypes import wintypes

user32 = ctypes.windll.user32
EnumWindowsProc = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)


class RECT(ctypes.Structure):
    _fields_ = [("left", wintypes.LONG), ("top", wintypes.LONG),
                ("right", wintypes.LONG), ("bottom", wintypes.LONG)]


def inspect(hwnd):
    if not user32.IsWindowVisible(hwnd):
        return None

    buf = ctypes.create_unicode_buffer(512)
    user32.GetWindowTextW(hwnd, buf, 512)
    title = buf.value

    if not title or not any(k in title for k in ("XiaoYE", "Maple", "Admin", "北斗")):
        return None

    cr = RECT()
    wr = RECT()
    user32.GetClientRect(hwnd, ctypes.byref(cr))
    user32.GetWindowRect(hwnd, ctypes.byref(wr))

    return {
        "hwnd": int(hwnd),
        "title": title,
        "client_w": cr.right - cr.left,
        "client_h": cr.bottom - cr.top,
        "window_rect": [wr.left, wr.top, wr.right, wr.bottom],
    }


def main():
    found = []

    @EnumWindowsProc
    def cb(hwnd, _):
        item = inspect(hwnd)
        if item:
            found.append(item)
        return True

    user32.EnumWindows(cb, 0)
    print(json.dumps({
        "label": sys.argv[1] if len(sys.argv) > 1 else "unnamed",
        "desktop_w": int(user32.GetSystemMetrics(0)),
        "desktop_h": int(user32.GetSystemMetrics(1)),
        "windows": found,
    }, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
