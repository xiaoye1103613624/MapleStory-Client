# -*- coding: utf-8 -*-
"""regress-20261107 第二账号自测驱动（不碰用户开着的两个 Admin 实例）。

隔离方式沿用 b53-recheck-alt（项目记忆 live-client-verification-gotchas）：
  * cwd = E:/MXD/ms-alt-b53（xiaoye 测试号；口令只在该副本，脚本不落盘）
  * XY_DATA_DIR 指回主部署 Data
  * 窗口按 pid 绑定；SendInput 前必须 foreground
  * 进图哨兵 = 标题「在线时长」段
  * 进图后头一个键会丢：先打 scrolllock 吃掉

覆盖取证项（对应用户 14 报）：
  01_game      进图首帧：底部名称/职业(#3)、LV、EXP 百分比、快捷键栏提示(#7)
  02_charinfo  W 角色信息：属性数值(#1/#2)
  03_stats     S 能力值
  04_inventory I 道具：金币(#4)
  06_skill     K 技能：等级/标题/SP(#12)
  06b/c        T 组合栏开 → ESC 关技能窗 → 组合栏应消失(#12)
  07a/b/c      M×3 小地图循环 MIN/NORMAL/MAX(#13)
  08_chatbar   折叠聊天栏左侧「对所有人」文字(#14)
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
os.makedirs(OUT_DIR, exist_ok=True)
CAP2 = 'E:/project/_cap2.py'

LOG = open(OUT_DIR + '/_run.txt', 'w', encoding='utf-8')


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


# --- 临时打开 auto 登录（跑完还原） ---
cfg = os.path.join(ALT_DIR, 'xy-config.yaml')
src = open(cfg, encoding='utf-8', errors='replace').read()
bak = cfg + '.bak.regress'
if not os.path.exists(bak):
    open(bak, 'w', encoding='utf-8').write(src)
open(cfg, 'w', encoding='utf-8').write(src.replace('auto: false', 'auto: true', 1))
log('config auto=true (bak kept)')

log('launching second instance from %s' % ALT_DIR)
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


def restore_cfg():
    open(cfg, 'w', encoding='utf-8').write(src)
    log('config restored')


in_game = False
t0 = time.time()
while time.time() - t0 < 180:
    time.sleep(2.0)
    t = title_of(hwnd)
    if '\u5728\u7ebf\u65f6\u957f' in t:
        in_game = True
        break
if not in_game:
    subprocess.run([sys.executable, CAP2, OUT_DIR + '/reg_abort_not_ingame.png', str(hwnd)],
                   capture_output=True)
    restore_cfg()
    proc.terminate()
    LOG.close()
    raise SystemExit('NOT IN GAME after 180s')

log('in game, title=%s' % title_of(hwnd))
time.sleep(3.0)

ox, oy = d.client_origin(hwnd)
log('client_origin=(%d,%d)' % (ox, oy))


def fg(why):
    if not d.foreground(hwnd):
        log('WARN %s: 抢不到前台' % why)
    time.sleep(0.15)


def key(name):
    fg('key ' + name)
    scan, ext = d.KEYS[name]
    d.send_key(scan, ext, False)
    time.sleep(0.06)
    d.send_key(scan, ext, True)
    time.sleep(0.3)


def cap(tag):
    out = OUT_DIR + ('/reg_%s.png' % tag)
    subprocess.run([sys.executable, CAP2, out, str(hwnd)], capture_output=True)
    log('cap %s -> %s' % (tag, os.path.exists(out)))


# 进图后头一个键会丢（项目记忆）：scrolllock 无绑定，吃掉
key('scrolllock')
time.sleep(0.5)

cap('01_game')            # #3 底部名称/职业 + LV + EXP% + #7 快捷键栏提示

key('w')                  # W 角色信息
time.sleep(1.2)
cap('02_charinfo')        # #1/#2 属性数值
key('escape')
time.sleep(0.6)

key('s')                  # S 能力值
time.sleep(1.2)
cap('03_stats')
key('escape')
time.sleep(0.6)

key('i')                  # I 道具
time.sleep(1.2)
cap('04_inventory')       # #4 金币
key('escape')
time.sleep(0.6)

key('k')                  # K 技能
time.sleep(1.2)
cap('06_skill')           # #12 技能等级/标题/SP

key('t')                  # T 组合栏
time.sleep(1.0)
cap('06b_macro_open')
key('escape')             # 关技能窗（第一次 ESC）
time.sleep(0.8)
key('escape')             # 兜底再 ESC
time.sleep(0.8)
cap('06c_macro_after_close')   # #12 组合栏应已消失

key('m')                  # M 小地图循环
time.sleep(0.9)
cap('07a_minimap_1')
key('m')
time.sleep(0.9)
cap('07b_minimap_2')
key('m')
time.sleep(0.9)
cap('07c_minimap_3')

cap('08_chatbar')         # #14 折叠聊天栏「对所有人」

proc.terminate()
time.sleep(3)
if proc.poll() is None:
    proc.kill()
restore_cfg()
LOG.close()
print('DONE ->', OUT_DIR)
