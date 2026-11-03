"""光标 API 二进制扫描：原生六模块是否有 SetCursorPos 类回写（R28-B4b 取证）。

用法：python cursor_api_scan.py  （只读，无副作用）
结论见 README-B4b-cursor-api.md。
"""
import os

API = [b"SetCursorPos", b"GetCursorPos", b"SendInput", b"mouse_event",
       b"ShowCursor", b"ClipCursor", b"SetWindowsHookEx",
       b"WarpMouseInWindow"]

ROOT = r"E:\MXD\BeiDou-ClientV17.7"
TARGETS = [
    "BeiDou.exe", "Gr2D_DX8.dll", "suipre.dll", "ijl15.dll",
    "Canvas.dll", "Shape2D.dll",
]

for name in TARGETS:
    p = os.path.join(ROOT, name)
    if not os.path.exists(p):
        print("MISS", p)
        continue
    b = open(p, "rb").read()
    hits = {a.decode(): b.count(a) for a in API if b.count(a)}
    print("%-14s %10d  %s" % (name, len(b), hits or "{}"))
