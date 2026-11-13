# -*- coding: utf-8 -*-
"""pass6：修正坐标版（主目录 MapleStoryTest.exe + xiaoye）。

宏指令按钮 = ui.position.skill(344,76) + MACRO_BTN_TOPLEFT(121,265) + (22,7)/2
           ≈ 视口 (487, 348)
NPC 娜娜   = 截图 (812,380) - 31 标题栏 = 视口 (812, 349)
"""
import ctypes
import ctypes.wintypes as wt
import importlib.util
import os
import re
import subprocess
import sys
import time

MAIN_DIR = r'E:\MXD\BeiDou-Client-xy'
EXE = os.path.join(MAIN_DIR, 'MapleStoryTest.exe')
ALT_CFG = r'E:\MXD\ms-alt-b53\xy-config.yaml'
OUT_DIR = r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\regress-20261107'
CAP2 = 'E:/project/_cap2.py'

LOG = open(OUT_DIR + '/_run6.txt', 'w', encoding='utf-8')


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


cfg = os.path.join(MAIN_DIR, 'xy-config.yaml')
main_src = open(cfg, encoding='utf-8', errors='replace').read()
alt_src = open(ALT_CFG, encoding='utf-8', errors='replace').read()

bak = cfg + '.bak.pass6'
if not os.path.exists(bak):
    open(bak, 'w', encoding='utf-8').write(main_src)

m = re.search(r'^(\s*account:\s*).*$', alt_src, re.M)
a = re.search(r'^(\s*pass:\s*).*$', alt_src, re.M)
patched = main_src
if m and a:
    patched = re.sub(r'^\s*account:\s*.*$', lambda _: m.group(0), patched, count=1, flags=re.M)
    patched = re.sub(r'^\s*pass:\s*.*$', lambda _: a.group(0), patched, count=1, flags=re.M)
patched = patched.replace('auto: false', 'auto: true', 1)
open(cfg, 'w', encoding='utf-8').write(patched)
log('cfg xiaoye+auto')


def restore_cfg():
    open(cfg, 'w', encoding='utf-8').write(main_src)
    log('cfg restored')


proc = subprocess.Popen([EXE], cwd=MAIN_DIR)
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
    restore_cfg()
    raise SystemExit('NO WINDOW')

in_game = False
t0 = time.time()
while time.time() - t0 < 180:
    time.sleep(2.0)
    if '\u5728\u7ebf\u65f6\u957f' in title_of(hwnd):
        in_game = True
        break
if not in_game:
    restore_cfg()
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
    out = OUT_DIR + ('/r6_%s.png' % tag)
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

# 1) K 开技能窗 → 宏指令按钮 (487,348)
key('k')
time.sleep(1.2)
click(487, 348, 'macro btn fixed')
time.sleep(1.0)
cap('01_macro_three_rows')

# 2) K 关技能窗 → 组合栏应一起消失
key('k')
time.sleep(1.0)
cap('02_both_closed')

# 3) NPC 娜娜 (812,349)
click(812, 349, 'npc nana fixed')
time.sleep(1.4)
cap('03_npc_click')
key('escape')
time.sleep(0.6)

proc.terminate()
time.sleep(3)
if proc.poll() is None:
    proc.kill()
restore_cfg()
LOG.close()
print('DONE6')
