# -*- coding: utf-8 -*-
"""pass4：修正视口坐标（截图含 31px 标题栏，viewport = screenshot_y - 31）。

1) 宏指令红钮 (248,363) → 组合栏三行版式
2) K 关技能窗 → 组合栏应消失
3) NPC 娜娜 (812,421) 点击 → UINpcTalk
4) 玩家罗杰 (403,539) 点击 → UICharInfo
"""
import ctypes
import ctypes.wintypes as wt
import importlib.util
import os
import subprocess
import sys
import time

ALT_DIR = r'E:\MXD\ms-alt-b53'
EXE = r'E:\MXD\BeiDou-Client-xy\MapleStory.exe'
DATA_DIR = r'E:\MXD\BeiDou-Client-xy\Data'
OUT_DIR = r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\regress-20261107'
CAP2 = 'E:/project/_cap2.py'

LOG = open(OUT_DIR + '/_run4.txt', 'w', encoding='utf-8')


def log(m):
    LOG.write(str(m) + '\n')
    LOG.flush()


spec = importlib.util.spec_from_file_location('_drive', 'E:/project/_drive.py')
d = importlib.util.module_from_spec(spec)
argv = sys.argv
sys.argv = ['_drive']
spec.loader.exec_module(d)
sys.argv = argv

user32 = d.user32
user32.SetProcessDPIAware()


def hwnd_of_pid(pid):
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
        if 'XiaoYE' in buf.value and (r.right - r.left) > 100 and r.left != -32000:
            found.append((hwnd, buf.value))
        return True

    user32.EnumWindows(proc(cb), 0)
    return found[0] if found else (None, None)


def title_of(hwnd):
    n = user32.GetWindowTextLengthW(hwnd)
    buf = ctypes.create_unicode_buffer(n + 1)
    user32.GetWindowTextW(hwnd, buf, n + 1)
    return buf.value


cfg = os.path.join(ALT_DIR, 'xy-config.yaml')
src = open(cfg, encoding='utf-8', errors='replace').read()
open(cfg, 'w', encoding='utf-8').write(src.replace('auto: false', 'auto: true', 1))

env = dict(os.environ, XY_DATA_DIR=DATA_DIR)
proc = subprocess.Popen([EXE], cwd=ALT_DIR, env=env)
log('pid=%d' % proc.pid)

hwnd = None
t0 = time.time()
while time.time() - t0 < 60:
    time.sleep(1.0)
    h, t = hwnd_of_pid(proc.pid)
    if h:
        hwnd = h
        break
if not hwnd:
    open(cfg, 'w', encoding='utf-8').write(src)
    raise SystemExit('NO WINDOW')

in_game = False
t0 = time.time()
while time.time() - t0 < 180:
    time.sleep(2.0)
    if '\u5728\u7ebf\u65f6\u957f' in title_of(hwnd):
        in_game = True
        break
if not in_game:
    open(cfg, 'w', encoding='utf-8').write(src)
    proc.terminate()
    LOG.close()
    raise SystemExit('NOT IN GAME')

log('in game')
time.sleep(3.0)
ox, oy = d.client_origin(hwnd)


def key(name):
    if not d.foreground(hwnd):
        log('WARN fg ' + name)
    time.sleep(0.12)
    scan, ext = d.KEYS[name]
    d.send_key(scan, ext, False)
    time.sleep(0.06)
    d.send_key(scan, ext, True)
    time.sleep(0.3)


def cap(tag):
    out = OUT_DIR + ('/r4_%s.png' % tag)
    subprocess.run([sys.executable, CAP2, out, str(hwnd)], capture_output=True)
    log('cap %s' % tag)


def click(vx, vy, why):
    if not d.foreground(hwnd):
        log('WARN fg click ' + why)
    user32.SetCursorPos(ox + vx, oy + vy)
    time.sleep(0.12)
    d.send_click()
    log('click (%d,%d) %s' % (vx, vy, why))
    time.sleep(0.8)


key('scrolllock')
time.sleep(0.4)

# 1) 技能窗 + 宏指令
key('k')
time.sleep(1.2)
click(248, 363, 'macro btn')
time.sleep(1.0)
cap('01_macro_open')

# 2) K 关技能窗 → 组合栏应消失
key('k')
time.sleep(1.0)
cap('02_skill_closed_macro_gone')

# 3) NPC 娜娜点击
click(812, 421, 'npc nana')
time.sleep(1.2)
cap('03_npc_click')
key('escape')
time.sleep(0.5)

# 4) 玩家罗杰点击
click(403, 539, 'player luojie')
time.sleep(1.2)
cap('04_player_click')
key('escape')
time.sleep(0.5)

proc.terminate()
time.sleep(3)
if proc.poll() is None:
    proc.kill()
open(cfg, 'w', encoding='utf-8').write(src)
log('config restored')
LOG.close()
print('DONE4')
