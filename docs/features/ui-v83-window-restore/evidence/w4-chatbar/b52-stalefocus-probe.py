# -*- coding: utf-8 -*-
"""R27-B5-2 追加取证：确认「弹层关掉后的第一下点击被吞掉」是不是
UIStateGame::send_cursor 里那条 stale-focus 分支（IO/UIStateGame.cpp:524）。

同一个客户端实例里连点两次 chatTarget 框，每次点完抓一帧：
  第 1 下若被吞 -> 列表不开；第 2 下 -> 列表开。
"""
import ctypes.wintypes as wt
import importlib.util
import subprocess
import sys
import time

spec = importlib.util.spec_from_file_location('_drive', 'E:/project/_drive.py')
d = importlib.util.module_from_spec(spec)
argv = sys.argv
sys.argv = ['_drive']
spec.loader.exec_module(d)
sys.argv = argv

user32 = d.user32
user32.SetProcessDPIAware()
LOG = open('E:/project/_r27_b52x.txt', 'w', encoding='utf-8')


def log(m):
    print(m, flush=True)
    LOG.write(m + '\n')
    LOG.flush()


def click(x, y, note):
    if not d.foreground(hwnd):
        raise SystemExit('ABORT: 抢不到前台，未发送任何输入')
    cx, cy = d.client_origin(hwnd)
    sx, sy = cx + x, cy + y
    user32.SetCursorPos(sx, sy)
    time.sleep(0.15)
    log('click(%d,%d) %s' % (x, y, note))
    d.send_click()
    time.sleep(0.45)


def cap(tag):
    out = 'E:/project/_r27_b52x_%s.png' % tag
    subprocess.run([sys.executable, 'E:/project/_cap2.py', out, str(hwnd)],
                   check=True, capture_output=True)
    log('cap %s' % tag)


hwnd = d.find_hwnd()
if not hwnd:
    raise SystemExit('NO WINDOW')
log('hwnd=%d' % hwnd)

cap('00_before')
click(46, 666, 'box #1')
cap('01_after_first')
click(46, 666, 'box #2')
cap('02_after_second')
log('DONE')
