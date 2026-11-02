# -*- coding: utf-8 -*-
"""R27-B6 目录气球锚定复验（第二账号并行实例）。

起独立实例（工作目录 E:/MXD/ms-alt-b53，资源用 XY_DATA_DIR 指回原部署，
句柄按 pid 绑定），自动登录后点击底栏「目录」钮，抓一帧。
判据不在脚本里，交给 b6-menu-anchor-check.py 做逐像素模板配准。

跑法：python b6-menu-anchor-alt.py
产物：E:/project/_ms_probe/alt/_b6alt_*.png + _b6alt.txt
"""
import ctypes
import ctypes.wintypes as wt
import importlib.util
import os
import subprocess
import sys
import time

ALT_DIR = r'E:\MXD\ms-alt-b53'
HERE = os.path.dirname(os.path.abspath(__file__))
EXE = r'E:\MXD\BeiDou-Client-xy\MapleStory.exe'
DATA_DIR = r'E:\MXD\BeiDou-Client-xy\Data'
OUT_DIR = r'E:\project\_ms_probe\alt'
os.makedirs(OUT_DIR, exist_ok=True)

LOG = open(OUT_DIR + '/_b6alt.txt', 'w', encoding='utf-8')


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

WM_CLOSE = 0x0010


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


def hwnd_of_pid_any(pid):
    """同 pid 的任意可见顶层窗口（退出确认框标题不含角色名）。"""
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
        if r.left != -32000 and (r.right - r.left) > 40:
            found.append((hwnd, buf.value))
        return True

    user32.EnumWindows(proc(cb), 0)
    return found[0] if found else (None, None)


def title_of(hwnd):
    n = user32.GetWindowTextLengthW(hwnd)
    buf = ctypes.create_unicode_buffer(n + 1)
    user32.GetWindowTextW(hwnd, buf, n + 1)
    return buf.value


log('launching instance from %s' % ALT_DIR)
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
        log('hwnd=%d' % hwnd)
        break
if not hwnd:
    raise SystemExit('NO WINDOW for pid %d' % proc.pid)

in_game = False
t0 = time.time()
last = ''
while time.time() - t0 < 150:
    time.sleep(2.0)
    t = title_of(hwnd)
    if t != last:
        log('title: %s' % t.encode('gbk', 'replace').decode('gbk'))
        last = t
    if '\u5728\u7ebf\u65f6\u957f' in t:
        in_game = True
        break
if not in_game:
    log('ABORT: 150s 内未进图，未发送任何输入')
    LOG.close()
    raise SystemExit('NOT IN GAME')

time.sleep(3.0)
ox, oy = d.client_origin(hwnd)
log('client_origin=(%d,%d)' % (ox, oy))

# view 坐标 = 客户区坐标（底栏按 VWIDTH/VHEIGHT 贴底），本构建为 1280x720。
# 底栏几何与 IO/UITypes/UIStatusBar.cpp:1993/1998 同源：
#   group_x = VWIDTH - qs_w(466) - group_gap(241) = 573
#   BtMenu  = (group_x + 112, VHEIGHT - 71 + 36) = (685, 685)，54x34 -> 中心 (712, 702)
VW, VH = 1280, 720
GROUP_X = VW - 466 - 241
BT_MENU_CX = GROUP_X + 112 + 27
BT_MENU_CY = VH - 71 + 36 + 17
log('BtMenu center view=(%d,%d)' % (BT_MENU_CX, BT_MENU_CY))


def fg(step):
    if not d.foreground(hwnd):
        raise SystemExit('ABORT @%s: 抢不到前台，未发送输入' % step)


def cap(tag):
    if not d.foreground(hwnd):
        log('WARN cap %s: 抢不到前台' % tag)
    out = OUT_DIR + '/_b6alt_%s.png' % tag
    subprocess.run([sys.executable, 'E:/project/_cap2.py', out, str(hwnd)],
                   check=True, capture_output=True)
    log('cap %s' % tag)
    return out


def click_abs(vx, vy, why, retry=True):
    """点击后由调用方断言结果；首个按键/点击可能被客户端吞掉（项目记忆）。"""
    fg('click ' + why)
    user32.SetCursorPos(ox + vx, oy + vy)
    time.sleep(0.2)
    d.send_click()
    time.sleep(0.6)
    log('click %s view=(%d,%d)' % (why, vx, vy))


def balloon_open(path, *chk_args):
    """蓝钮行剖面判据（退出码 0 = 气球在位且落位对）。"""
    r = subprocess.run([sys.executable, HERE + '/b6-menu-anchor-check.py', path]
                       + [str(a) for a in chk_args], capture_output=True)
    return r.returncode == 0


def balloon_tail(base, path, exp_x, exp_y, art_h):
    """剪影判据（独立第二把尺）：未开帧 vs 气球帧做差，直接量下摆尖端末行/中心 x。"""
    r = subprocess.run([sys.executable, HERE + '/b6-menu-anchor-check.py', '--tail',
                        base, path, str(exp_x), str(exp_y), str(art_h)],
                       capture_output=True)
    return r.returncode == 0


def open_balloon(vx, vy, tag, chk_args, base):
    """点 -> 双尺断言 -> 重试（进图后第一次输入会被吞，见 anchor-rule.md §4）。

    两把尺都要过：蓝钮行法依赖「首颗钮 = 气球顶 + 22」的同族版式推断，剪影尺不依赖它，
    单尺过顶多说明「按那条尺摆对了」。
    """
    for i in range(3):
        click_abs(vx, vy, '%s try%d' % (tag, i))
        out = cap('%s_try%d' % (tag, i))
        latt = balloon_open(out, *chk_args)
        tail = balloon_tail(base, out, chk_args[1], chk_args[2], chk_args[4])
        if latt and tail:
            log('%s try %d -> OPEN（蓝钮行 + 剪影 双尺 PASS）帧=%s' % (tag, i, out))
            return out
        log('%s try %d -> 蓝钮行=%s 剪影=%s' % (tag, i, latt, tail))
        time.sleep(0.6)
    log('ABORT: %s 三次点击后双尺未齐过' % tag)
    return None


def post_esc():
    user32.PostMessageW(hwnd, 0x0100, 0x1B, 0x00000001)
    time.sleep(0.05)
    user32.PostMessageW(hwnd, 0x0101, 0x1B, 0xC0000001)
    log('post ESC -> hwnd=%d' % hwnd)


base = cap('01_baseline')

# --- 目录（GameMenu，4 颗蓝钮，期望 (666,545)，底板高 140）---
menu_frame = open_balloon(BT_MENU_CX, BT_MENU_CY, '02_menu', (4, 666, 545, 22, 140), base)

# 第三次点击探一下「再点一次钮能不能收起」。实测收不起：UIGameMenu::FOCUSED=true，
# 激活期间鼠标事件被气球吃掉，底栏 BtMenu 收不到第二下 —— 所以改用 ESC 关。
click_abs(BT_MENU_CX, BT_MENU_CY, 'BtMenu click#3 (toggle probe)')
out2 = cap('03_after_third_click')
log('after click#3 balloon_open=%s' % balloon_open(out2, 4, 666, 545, 22, 140))
post_esc()
time.sleep(1.0)
out2b = cap('04_after_esc')
log('after ESC balloon_open=%s tail=%s（两个气球都应为 False）'
    % (balloon_open(out2b, 4, 666, 545, 22, 140), balloon_tail(base, out2b, 666, 545, 140)))

# --- 界面（ShortCut，8 颗蓝钮，期望 (722,440)，底板高 245）---
#   BtShort = group_x + 168，54x34 -> 中心 view (573+168+27, 702) = (768, 702)
sc_frame = open_balloon(GROUP_X + 168 + 27, BT_MENU_CY, '05_shortcut', (8, 722, 440, 22, 245), base)

log('closing instance pid=%d' % proc.pid)
user32.PostMessageW(hwnd, WM_CLOSE, 0, 0)
time.sleep(4.0)
if proc.poll() is None:
    # 退出确认框（若有）是同 pid 的另一个顶层窗口，回车 = 默认按钮
    h2, t2 = hwnd_of_pid_any(proc.pid)
    if h2:
        log('exit-confirm window=%s -> post ENTER' % t2.encode('gbk', 'replace').decode('gbk'))
        user32.PostMessageW(h2, 0x0100, 0x0D, 0x00000001)
        user32.PostMessageW(h2, 0x0101, 0x0D, 0xC0000001)
        time.sleep(4.0)
if proc.poll() is None:
    log('WARN: pid %d 仍在（未强杀，留给你处理）' % proc.pid)
LOG.close()
