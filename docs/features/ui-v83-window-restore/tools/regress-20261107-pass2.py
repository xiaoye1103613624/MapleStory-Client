# -*- coding: utf-8 -*-
"""回归第二轮（字体修复后）：专项取证 M 循环 / 聊天目标文字 / NPC 点击 / 装备窗。

键序修正：ESC 在无窗时会开主菜单（原生行为），本轮用 X 关主菜单、
用原键二次按下收窗（K 收技能、I 收道具），避免 ESC 链污染。
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

LOG = open(OUT_DIR + '/_run2.txt', 'w', encoding='utf-8')


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
log('origin=(%d,%d)' % (ox, oy))


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
    out = OUT_DIR + ('/r2_%s.png' % tag)
    subprocess.run([sys.executable, CAP2, out, str(hwnd)], capture_output=True)
    log('cap %s' % tag)


def click(vx, vy, why):
    if not d.foreground(hwnd):
        log('WARN fg click ' + why)
    user32.SetCursorPos(ox + vx, oy + vy)
    time.sleep(0.1)
    d.send_click()
    log('click (%d,%d) %s' % (vx, vy, why))
    time.sleep(0.6)


key('scrolllock')
time.sleep(0.5)

# --- 1) M 循环：MIN -> NORMAL -> MAX ---
# 小地图默认可能已在 NORMAL；先连按记录三态
cap('00_base')
key('m')
time.sleep(0.9)
cap('01_m1')
key('m')
time.sleep(0.9)
cap('02_m2')
key('m')
time.sleep(0.9)
cap('03_m3')
key('m')
time.sleep(0.9)
cap('04_m4_off')   # 第4次应关窗
key('m')
time.sleep(0.9)
cap('05_m5_back')  # 再开回 MIN

# --- 2) 聊天目标文字：Enter 聚焦输入框 ---
key('enter')
time.sleep(0.8)
cap('06_chat_focus')
key('escape')      # 收聊天焦点（输入框聚焦时 ESC 先退焦点）
time.sleep(0.5)

# --- 3) NPC 点击（#11）：娜娜 预览 (455,340) -> 实际视口 (728,544) ---
click(728, 544, 'npc nana real')
time.sleep(1.2)
cap('07_npc_click')
key('escape')
time.sleep(0.5)

# --- 4) 玩家名点击（罗杰 预览 (300,420) -> 实际 (480,672)） ---
click(480, 672, 'player luojie')
time.sleep(1.0)
cap('08_player_click')
key('escape')
time.sleep(0.4)

# --- 5) 装备窗排版（#5）：I 开背包再按装备页签；悬浮已装备格看 tooltip ---
key('i')
time.sleep(1.0)
# 装备页签（预览 x18 y56 -> 实际 x29 y90）
click(29, 90, 'equip tab')
time.sleep(0.8)
cap('09_equip_tab')
# 悬浮第一格（预览 x40..70 y75 -> 实际 x64 y120）
if not d.foreground(hwnd):
    log('WARN fg hover')
user32.SetCursorPos(ox + 90, oy + 120)
time.sleep(1.2)
cap('10_equip_hover')
key('escape')
time.sleep(0.5)

# --- 6) 技能组合栏三行（#12）：K 开技能，T 开组合栏 ---
key('k')
time.sleep(1.0)
key('t')
time.sleep(1.0)
cap('11_macro_open')
# 关：K 二次收技能（走 close() 收组合栏）
key('k')
time.sleep(1.0)
cap('12_macro_closed')

proc.terminate()
time.sleep(3)
if proc.poll() is None:
    proc.kill()
open(cfg, 'w', encoding='utf-8').write(src)
log('config restored')
LOG.close()
print('DONE2')
