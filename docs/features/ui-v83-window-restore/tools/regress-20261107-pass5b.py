# -*- coding: utf-8 -*-
"""pass5b：主目录 + MapleStoryTest.exe（xiaoye 号）。

用户实例 49704 = admin（主目录 MapleStory.exe，锁文件），不冲突。
主目录 xy-config.yaml 临时换 xiaoye + auto（跑完还原，口令不落仓库：
从 ms-alt-b53 的 config 里读账号段拼进主 config）。
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

LOG = open(OUT_DIR + '/_run5b.txt', 'w', encoding='utf-8')


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


# 主目录 config：备份 → 换 xiaoye 账号段 + auto=true
cfg = os.path.join(MAIN_DIR, 'xy-config.yaml')
main_src = open(cfg, encoding='utf-8', errors='replace').read()
alt_src = open(ALT_CFG, encoding='utf-8', errors='replace').read()

bak = cfg + '.bak.pass5b'
if not os.path.exists(bak):
    open(bak, 'w', encoding='utf-8').write(main_src)

# 用 alt 的 account/pass 行替换 main 的
m = re.search(r'^(\s*account:\s*).*$', alt_src, re.M)
a = re.search(r'^(\s*pass:\s*).*$', alt_src, re.M)
patched = main_src
if m and a:
    acct_line = m.group(0)
    pass_line = a.group(0)
    patched = re.sub(r'^\s*account:\s*.*$', lambda _: acct_line, patched, count=1, flags=re.M)
    patched = re.sub(r'^\s*pass:\s*.*$', lambda _: pass_line, patched, count=1, flags=re.M)
patched = patched.replace('auto: false', 'auto: true', 1)
open(cfg, 'w', encoding='utf-8').write(patched)
log('main cfg patched to xiaoye+auto (bak kept)')


def restore_cfg():
    open(cfg, 'w', encoding='utf-8').write(main_src)
    log('main cfg restored')


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
    subprocess.run([sys.executable, CAP2, OUT_DIR + '/r5b_abort.png', str(hwnd)], capture_output=True)
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
    out = OUT_DIR + ('/r5b_%s.png' % tag)
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

# 1) 技能窗 + 宏指令 -> 三行空槽（新修复）
key('k')
time.sleep(1.2)
click(248, 363, 'macro btn')
time.sleep(1.0)
cap('01_macro_three_rows')

# 2) K 关技能窗 -> 组合栏应一起消失（新修复）
key('k')
time.sleep(1.0)
cap('02_both_closed')

# 3) NPC 娜娜点击（修正坐标 807,376）
click(807, 376, 'npc nana')
time.sleep(1.4)
cap('03_npc_click')
key('escape')
time.sleep(0.6)

# 4) 玩家罗杰点击（身体 403,545）
click(403, 545, 'player luojie')
time.sleep(1.4)
cap('04_player_click')

proc.terminate()
time.sleep(3)
if proc.poll() is None:
    proc.kill()
restore_cfg()
LOG.close()
print('DONE5B')
