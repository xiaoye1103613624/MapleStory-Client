# -*- coding: utf-8 -*-
"""第二账号实例：先把装备窗关掉，再单独验「C 开聊天窗」是否生效。

上一轮 _b53alt 里 C 没能开窗（白底计数与无窗基线相同），但打「hello」里的 E
却开出了装备窗，说明按键通道是通的 —— 所以要单独把 C 这一步隔离出来重打。
"""
import ctypes
import importlib.util
import os
import subprocess
import sys
import time

PID = int(sys.argv[1])
OUT = r'E:\project\_ms_probe\alt'

spec = importlib.util.spec_from_file_location('_drive', 'E:/project/_drive.py')
d = importlib.util.module_from_spec(spec)
argv = sys.argv
sys.argv = ['_drive']
spec.loader.exec_module(d)
sys.argv = argv
user32 = d.user32
user32.SetProcessDPIAware()


def hwnd_of_pid(pid):
    import ctypes.wintypes as wt
    found = []
    proc = ctypes.WINFUNCTYPE(wt.BOOL, wt.HWND, wt.LPARAM)

    def cb(hwnd, lp):
        wpid = wt.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(wpid))
        if wpid.value != pid or not user32.IsWindowVisible(hwnd):
            return True
        n = user32.GetWindowTextLengthW(hwnd)
        if not n:
            return True
        buf = ctypes.create_unicode_buffer(n + 1)
        user32.GetWindowTextW(hwnd, buf, n + 1)
        r = wt.RECT()
        user32.GetWindowRect(hwnd, ctypes.byref(r))
        if 'XiaoYE' in buf.value and (r.right - r.left) > 100:
            found.append((hwnd, buf.value))
        return True

    user32.EnumWindows(proc(cb), 0)
    return found[0] if found else (None, None)


h, t = hwnd_of_pid(PID)
if not h:
    raise SystemExit('NO WINDOW for pid %d' % PID)
hwnd = h
if not d.foreground(hwnd):
    raise SystemExit('ABORT: 抢不到前台')
ox, oy = d.client_origin(hwnd)


def key(name):
    if not d.foreground(hwnd):
        raise SystemExit('ABORT: 打键前抢不到前台')
    scan, ext = d.KEYS[name]
    d.send_key(scan, ext, False)
    time.sleep(0.06)
    d.send_key(scan, ext, True)
    time.sleep(0.3)


def cap(tag):
    subprocess.run([sys.executable, 'E:/project/_cap2.py',
                    OUT + '/_cprobe_%s.png' % tag, str(hwnd)],
                   check=True, capture_output=True)
    print('cap', tag)


def move(vx, vy):
    user32.SetCursorPos(ox + vx, oy + vy)
    time.sleep(0.25)


cap('00_now')
# 装备窗开着会盖住读数，先按 E 关掉（UI::emplace 同族：再按一次即收起）
key('e')
time.sleep(0.8)
move(900, 300)
cap('01_equip_closed')
for i in range(2):
    key('c')
    time.sleep(1.0)
    cap('02_c%d' % i)
    if i == 0:
        move(900, 300)
