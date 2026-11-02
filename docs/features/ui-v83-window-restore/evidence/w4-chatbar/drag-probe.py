# -*- coding: utf-8 -*-
"""B5-1 R1/R3 定点探针：登录 -> 点 + 展开 -> 拖顶边 60px -> 抓帧 -> 优雅退出。
配合 exe 里的 [DBG-B51] 追踪，定位「拖顶边不改高度」断在哪一环。
"""
import importlib.util
import os
import re
import subprocess
import sys
import time

CLIENT_DIR = r'E:\MXD\BeiDou-Client-xy'
DIAG = CLIENT_DIR + r'\_ui_asset_diag.log'
MARK = os.path.getsize(DIAG) if os.path.exists(DIAG) else 0

raw = open(CLIENT_DIR + r'\xy-config.yaml', 'rb').read()
text = None
for enc in ('utf-8', 'gbk'):
    try:
        text = raw.decode(enc)
        break
    except Exception:
        pass
vals = {}
sec = None
for line in text.splitlines():
    m = re.match(r'^(\w+):', line)
    if m:
        sec = m.group(1)
    m2 = re.match(r'^\s+(account|pass)\s*:\s*(.+?)\s*$', line)
    if m2 and sec == 'login':
        vals[m2.group(1)] = m2.group(2).strip('"')
assert len(vals) == 2

spec = importlib.util.spec_from_file_location('_drive', 'E:/project/_drive.py')
d = importlib.util.module_from_spec(spec)
argv = sys.argv
sys.argv = ['_drive']
spec.loader.exec_module(d)
sys.argv = argv
d.user32.SetProcessDPIAware()

LOG = open('E:/project/_r27_drag.txt', 'w', encoding='utf-8')


def log(m):
    print(m, flush=True)
    LOG.write(m + '\n')
    LOG.flush()


env = dict(os.environ, R27_USER=vals['account'], R27_PASS=vals['pass'])
r = subprocess.run([sys.executable,
                    r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\tools\live\login.py'],
                   env=env, capture_output=True, text=True)
log('login rc=%d' % r.returncode)
time.sleep(12)

hwnd = d.find_hwnd()
if not hwnd:
    raise SystemExit('NO WINDOW')
if not d.foreground(hwnd):
    raise SystemExit('ABORT: 抢不到前台')
ox, oy = d.client_origin(hwnd)
log('hwnd=%d origin=(%d,%d)' % (hwnd, ox, oy))


def cap(tag):
    out = 'E:/project/_r27_drag_%s.png' % tag
    subprocess.run([sys.executable, 'E:/project/_cap2.py', out, str(hwnd)], check=True, capture_output=True)
    log('cap %s' % tag)


def must_fg(step):
    if not d.foreground(hwnd):
        raise SystemExit('ABORT @%s' % step)


def mhover(vx, vy):
    d.user32.SetCursorPos(ox + vx, oy + vy + 3)
    time.sleep(0.18)
    d.user32.SetCursorPos(ox + vx, oy + vy)
    time.sleep(0.18)


def mclick(vx, vy):
    must_fg('click')
    mhover(vx, vy)
    d.send_click()
    time.sleep(0.35)


def mbtn(flag):
    inp = d.INPUT()
    inp.type = 0
    inp.iu.mi = d.MOUSEINPUT(0, 0, 0, flag, 0, None)
    d.user32.SendInput(1, d.ctypes.byref(inp), d.ctypes.sizeof(d.INPUT))


def mdrag(x0, y0, x1, y1, steps=10):
    must_fg('drag')
    mhover(x0, y0)
    mbtn(d.MOUSEEVENTF_LEFTDOWN)
    time.sleep(0.2)
    for i in range(1, steps + 1):
        d.user32.SetCursorPos(ox + int(x0 + (x1 - x0) * i / steps),
                              oy + int(y0 + (y1 - y0) * i / steps))
        time.sleep(0.12)
    time.sleep(0.15)
    mbtn(d.MOUSEEVENTF_LEFTUP)
    time.sleep(0.3)


BAND_TOP = 653
PLUS = (542, BAND_TOP + 14)
GRIP = (300, BAND_TOP - 80 + 3)

cap('a_folded')
mclick(*PLUS)
cap('b_expanded')
mdrag(GRIP[0], GRIP[1], GRIP[0], GRIP[1] - 60)
cap('c_after_drag')
must_fg('esc')
subprocess.run([sys.executable, 'E:/project/_postkey.py', 'esc'], capture_output=True)
time.sleep(0.7)
q = subprocess.run([sys.executable, 'E:/project/_quit.py'], capture_output=True, text=True)
log('quit rc=%d' % q.returncode)
time.sleep(2)

with open(DIAG, 'rb') as f:
    f.seek(MARK)
    tail = f.read().decode('utf-8', errors='replace')
hits = [l for l in tail.splitlines() if 'DBG-B51' in l]
open('E:/project/_r27_drag_diag.txt', 'w', encoding='utf-8').write('\n'.join(hits))
log('diag DBG-B51 lines = %d' % len(hits))
log('DONE')
